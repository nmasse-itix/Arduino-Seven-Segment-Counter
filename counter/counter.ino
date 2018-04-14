/*
 * A seven segement display counter
 *
 * Counts the number of times a button is pressed and displays this number on a 7 segment display
 *
 * MIT License
 *
 * Copyright (c) 2018 Nicolas Massé
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * The button is connected to PIN 2.
 * 
 * To prevent any rebound issue, filter the button signal with a low pass filter (R = 240 ohms, C = 6.8 micro farad)
 */
#define BUTTON_PIN 2

/*
 * The seven segment display has a common cathode for each digit
 */
#define CATHODE_1000_PIN B00000001
#define CATHODE_100_PIN B00000010
#define CATHODE_10_PIN B00000100
#define CATHODE_1_PIN B00001000
// The splitter is the colon between the second and third digit
#define CATHODE_SPLITTER_PIN B00010000

/*
 * All the cathodes are connected to PORTB's pins
 * 
 */
#define CATHODE_PORT PORTB
#define CATHODE_DDR DDRB

/*
 * The anodes will light up a segment when it is LOW and the cathode is HIGH.
 * 
 * The anode numbering refers to the standard numbering:
 * https://en.wikipedia.org/wiki/Seven-segment_display
 */
#define ANODE_A_PIN B10000000
#define ANODE_B_PIN B01000000
#define ANODE_C_PIN B00100000
#define ANODE_D_PIN B00010000
#define ANODE_E_PIN B00001000
// Yes, there is a gap since pin 2 is used for the button 
#define ANODE_F_PIN B00000010
#define ANODE_G_PIN B00000001

/*
 * All the anodes are connected to PORTD's pins
 */
#define ANODE_PORT PORTD
#define ANODE_DDR DDRD

/*
 * The number of milliseconds to wait in order to let the retinal persistence do its job. 
 * See https://en.wikipedia.org/wiki/Persistence_of_vision
 */
#define DELAY 5

/*
 * Set this in order to see how the display multiplexing work
 */
//#define DEBUG

#ifdef DEBUG
#define DELAY_DEBUG delay(50);
#define DELAY 500
#else
#define DELAY_DEBUG 
#endif

/*
 * Lookup table to drive the anode pins according to a digit between 0 and 9
 */
byte digit[] = {
  /* 0 */ ANODE_A_PIN|ANODE_B_PIN|ANODE_C_PIN|ANODE_D_PIN|ANODE_E_PIN|ANODE_F_PIN,
  /* 1 */ ANODE_B_PIN|ANODE_C_PIN,
  /* 2 */ ANODE_A_PIN|ANODE_B_PIN|ANODE_D_PIN|ANODE_E_PIN|ANODE_G_PIN,
  /* 3 */ ANODE_A_PIN|ANODE_B_PIN|ANODE_C_PIN|ANODE_D_PIN|ANODE_G_PIN,
  /* 4 */ ANODE_B_PIN|ANODE_C_PIN|ANODE_F_PIN|ANODE_G_PIN,
  /* 5 */ ANODE_A_PIN|ANODE_C_PIN|ANODE_D_PIN|ANODE_F_PIN|ANODE_G_PIN,
  /* 6 */ ANODE_A_PIN|ANODE_C_PIN|ANODE_D_PIN|ANODE_E_PIN|ANODE_F_PIN|ANODE_G_PIN,
  /* 7 */ ANODE_A_PIN|ANODE_B_PIN|ANODE_C_PIN,
  /* 8 */ ANODE_A_PIN|ANODE_B_PIN|ANODE_C_PIN|ANODE_D_PIN|ANODE_E_PIN|ANODE_F_PIN|ANODE_G_PIN,
  /* 9 */ ANODE_A_PIN|ANODE_B_PIN|ANODE_C_PIN|ANODE_D_PIN|ANODE_F_PIN|ANODE_G_PIN
};

// All the cathodes
const byte cathodes = CATHODE_1000_PIN | CATHODE_100_PIN | CATHODE_10_PIN | CATHODE_1_PIN | CATHODE_SPLITTER_PIN;

// All the anodes
const byte anodes = ANODE_A_PIN|ANODE_B_PIN|ANODE_C_PIN|ANODE_D_PIN|ANODE_E_PIN|ANODE_F_PIN|ANODE_G_PIN;

void setup() {
  // initialize cathodes pins as output with default to LOW
  CATHODE_DDR |= cathodes;
  CATHODE_PORT &= ~cathodes;
  
  // initialize anodes pins as output with default to HIGH
  ANODE_DDR |= anodes;
  ANODE_PORT |= anodes;
  
  // Enable the button and register an interrupt
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_interrupt, RISING);
}

// the counter is later incremented by an interrupt
volatile byte counter[] = {0,0,0,0};

void button_interrupt() {
  if (counter[0] >= 9) {
    counter[0] = 0;
    if (counter[1] >= 9) {
      counter[1] = 0;
      if (counter[2] >= 9) {
        counter[2] = 0;
        if (counter[3] >= 9) {
          // Overflow
          counter[3] = 0;
        } else {
          counter[3]++;
        }
      } else {
        counter[2]++;
      }
    } else {
      counter[1]++;
    }
  } else {
    counter[0]++;
  }
}

const byte digit_cathodes[] = { CATHODE_1_PIN, CATHODE_10_PIN, CATHODE_100_PIN, CATHODE_1000_PIN };

void loop() {
  // Iterate over the four digits to display
  for (byte c = 0; c < 4; c++) {
    // Set all cathodes pins to LOW
    CATHODE_PORT &= ~(CATHODE_1_PIN|CATHODE_10_PIN|CATHODE_100_PIN|CATHODE_1000_PIN);
    DELAY_DEBUG
    
    // Drive the matching anodes of the digit (a segment is enabled when the matching pin is LOW)
    ANODE_PORT = ~digit[counter[c]];
    DELAY_DEBUG

    // Set the selected cathode pin to HIGH to display to digit
    CATHODE_PORT |= digit_cathodes[c];
    DELAY_DEBUG

    // Let the persistence of vision do his job
    delay(DELAY);
  }
}

