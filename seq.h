// sequencer related definitions and structures
#define NTRACKS 8   // we have 8 track sequences
#define MAX_SEQ_STEPS 16 // up to 16 step sequencer
#define DEFAULT_SEQ_STEPS 16 // up to 16 step sequencer
#define DEFAULT_VELOCITY_ON 100 // 
#define DEFAULT_DIVIDER 6 // clock divider for 16th notes

// from pikocore
#define CLOCK_RATE 264000?

#define TEMPO    120 // default tempo
#define PPQN 24  // clocks per quarter note
#define NOTE_DURATION (PPQN/6) // sixteenth note duration
#define CLOCKPULSE 15 // was 15duration of clock out pulse
// #define SYNCGAP_DEBUG 
#define EXPECTED_INTERVAL_TICKS 11

int16_t bpm = TEMPO;
int32_t lastMIDIclock; // timestamp of last MIDI clock
int16_t MIDIclocks=PPQN;  //  *2; // midi clock counter
int16_t MIDIsync = 16;  // number of clocks required to sync BPM
int16_t useMIDIclock = 0; // true if we are using MIDI clock
long clocktimer = 0; // clock rate in ms
long pulsetimer = 0;
long syncgap = 0;
long baseline_syncgap = 0;

bool reset = false; // used to reset bpm from CLOCKIN interrupt
int16_t indexAtPulse = 0;
bool update_baseline_syncgap = 1;
byte num_consecutive_resets = 0;

int16_t targetsync = 0;
byte interval_click_count = 0;
byte clockincounter = 99;
bool pulsetimer_running = 0;
bool clockin_received = 0;
bool syncgap_newvalue = 0;
byte sync_status = 0;
byte sync_ok_count = 0;
#ifdef SYNCGAP_DEBUG
   byte sync_gap_debug_cnt = 8;
#endif
// table of 24 ppqn clock dividers for 4/4 time 1/32,1/16,1/8,1/4,1/2,1 bar,2 bars,4 bars
int16_t divtable[] = {3,6,12,24,48,96,192,384};



// all of the sequencers use the same data structure even though the data may be different in each case
// this simplifies the code somewhat
// clocks are setup for 24ppqn MIDI clock
// note that there are two threads of execution running on the two Pico cores - UI and note handling
// must be careful about editing items that are used by the 2nd Pico core for note timing etc

struct sequencer {
  uint8_t note[MAX_SEQ_STEPS]; // MIDI note 64= nominal pitch, +- 1 octave range
  uint8_t velocity[MAX_SEQ_STEPS];  // MIDI velocity 0-127. velocity 0 = no note
  uint8_t probability[MAX_SEQ_STEPS];  // probability of playing 127=always play, 0 = don't play
  int16_t index;    // index of step we are on
  int16_t divider;   // clock rate divider - lookup via table
  int16_t clockticks;   //  clock counter
  bool enabled; // true when playing
};

int16_t ref_index = DEFAULT_SEQ_STEPS-1; // reference index used for sync mechanism
int16_t ref_clockticks = 24;     // 24 ppqn clock

// notes are stored as offsets from the root 
sequencer seq[NTRACKS] = {
  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  DEFAULT_DIVIDER,  // clock divider    1/16
  24,       // 24 ppqn clock
  true,   // track enabled

  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  DEFAULT_DIVIDER,  // clock divider    1/16
  24,       // 24 ppqn clock
  true,   // track enabled

  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  3,  // clock divider    1/32
  24,       // 24 ppqn clock
  true,   // track enabled

  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  12,  // clock divider   1/8
  24,       // 24 ppqn clock
  true,   // track enabled

  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  24,  // clock divider   1/4
  24,       // 24 ppqn clock
  true,   // track enabled

  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  48,  // clock divider   1/2
  24,       // 24 ppqn clock
  true,   // track enabled

  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  48,  // clock divider   1/2
  24,       // 24 ppqn clock
  true,   // track enabled

  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  96,  // clock divider   1 bar
  24,       // 24 ppqn clock
  true,   // track enabled
};

// clock all the sequencers
// clockperiod is the period of the 24ppqn clock - used for calculating gate times etc - not used as yet
// it loops thru all tracks looking for events to process
void clocktick (long clockperiod) {

  for (uint8_t track=0; track<NTRACKS;++track) { // 
    --seq[track].clockticks;
    if (seq[track].clockticks <1) { // clock has counted down, do next step
      seq[track].clockticks = seq[track].divider; // reset the clock counter
      ++seq[track].index;
      if ((seq[track].index) >= DEFAULT_SEQ_STEPS) seq[track].index=0; // restart the sequence 
      if (seq[track].enabled && (seq[track].velocity[seq[track].index] > 0)) { // velocity > 0 is a note on
        if (random(0,122) < seq[track].probability[seq[track].index]) { // probability threshold for 100% is a little lower - allows for a bit of slop in the pot
          voice[track].level=seq[track].velocity[seq[track].index]; // set the volume level
      // *** should probably do some sanity checks here
          voice[track].sampleincrement=pitchtable[seq[track].note[seq[track].index]-60+12]; // look up sample increment to get the pitch for the sample
          rp2040.idleOtherCore(); // stop other core because it modifies sampleindex as well
          voice[track].sampleindex=0; // trigger sample for this track
          rp2040.resumeOtherCore();
        }
      }
    }
  }
  --ref_clockticks;
  if (ref_clockticks <1) { // reference clock has counted down, do next step
      ref_clockticks = DEFAULT_DIVIDER; // reset the clock counter
      ++ref_index;
      if ((ref_index) >= DEFAULT_SEQ_STEPS) ref_index=0; // restart the sequence 
      if (ref_index == 0 ) {   // index has just gone to zero - start timer
       if (!pulsetimer_running) {
         pulsetimer_running=1;
         pulsetimer = millis();
         } 
       } 
  }
}

// sync all the sequencers by resetting their clocks
void sync_sequencers(void) {
  for (uint8_t track=0; track<NTRACKS;++track) { // 
    seq[track].clockticks = 1; // set the clock counter so it will roll over 
  }
  ref_clockticks = 1;
}
// sync all the sequencers by resetting their clocks
void sync_sequencers_and_indexes(void) {
  for (uint8_t track=0; track<NTRACKS;++track) { // 
    seq[track].clockticks = 1; // set the clock counter so it will roll over 
    seq[track].index = DEFAULT_SEQ_STEPS;
  }
  ref_index = DEFAULT_SEQ_STEPS;
}

uint16_t read_index(void) {
   return ref_index;
}

// must be called regularly for sequencer to run
// hard wired to 16th notes at the moment
void do_clocks(void) {
  //long clockperiod= (long)(((60.0/(float)bpm)/PPQN)*1000);

  long clockperiod = (long)(((60.0 / (float)bpm) / PPQN) * 1000);   // 24 ticks per step



  if (clockin_received) {
    clockin_received=0;
    clocktimer = millis();
    clocktick(clockperiod);
      #ifdef SYNCGAP_DEBUG
        if (sync_gap_debug_cnt > 0 ) {
        //Serial.print("RPM = "); Serial.print(RPM);
        //Serial.print(",bpm = "); Serial.print(bpm);
        //Serial.print(",clockperiod = "); Serial.print(clockperiod);
        //Serial.print(",interval_click_count = "); Serial.println(interval_click_count);
        if (syncgap_newvalue) {
          Serial.print("indexAtPulse = "); Serial.print(indexAtPulse);
          Serial.print(",Syncgap = "); Serial.println(syncgap);
          syncgap_newvalue=0;
        }
        }
      #endif
    interval_click_count=0;
  } else { 
    if (( (millis() - clocktimer) ) > clockperiod) {
       if (interval_click_count < EXPECTED_INTERVAL_TICKS) {
         interval_click_count++;
          clocktimer = millis();
          clocktick(clockperiod);
    //digitalWrite(CLOCKOUT, 1); // external clock high
    // reset reset for interrupt
    //reset = false;
  
       }
    }

  }
  

  //if ((millis() - clocktimer) > CLOCKPULSE) digitalWrite(CLOCKOUT, 0); // external clock low
}


