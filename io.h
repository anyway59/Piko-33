
// Copyright 2023 Rich Heslip
//
// Author: Rich Heslip 
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// I/O pin definitions for Pikocore - I'm using VCC-GND YD-RP2040 board from AliExpress


#ifndef IO_H_
#define IO_H_

//#define CPU_USE 25
#define PWMOUT 22      // AUDIO OUT was 20

// analog inputs for voltage control - range approx 0-5v
// 
#define AIN0 	26
#define AIN1 	27
#define AIN2 	28
#define AIN3 	29 // not available on standard Pico board



// button inputs

#define BUTTON0 0 // key1 input on schematic  was 4
#define BUTTON1	2  // was 5
#define BUTTON2	4  // was 6
#define BUTTON3	6  // was 7
#define BUTTON4 8  // was 8
#define BUTTON5	10  // was 9
#define BUTTON6	12  // was 10
#define BUTTON7 14  // for the Pico board   was 11



#define SHIFTBUTTON 18 // additional button  was 24 (USR)
#define PAGEBUTTON 19 // additional button 

#define LED0 1 // LED1 output on schematic was 12
#define LED1 3  // was 13
#define LED2 5  // was 14
#define LED3 7  // was 15
#define LED4 9  // was 16
#define LED5 11  // was 17
#define LED6 13 // was 18
#define LED7 15  // was 19

#define FNLED 25 // indicates 2nd function mode

// MIDI serial port pins - not really MIDI but the serial port is exposed on the first two jacks
#define MIDIRX 17  // was 1
#define MIDITX 16  // was 0

#endif // IO_H_

