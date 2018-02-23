/*
 * keypad.ino
 * The Keypad class implementation file

  Copyright (c) 2018 Lee Dowthwaite
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "Keypad.h"

#define ENABLE_TEST

/*
 * Constructor - initialise the I/O ports
 */
Keypad::Keypad(char *keymap, int ncols, int nrows) 
  : _keymap(keymap), _ncols(ncols), _nrows(nrows) {
  ROW_DDR &= ~ROW_MASK; // inputs
  ROW_PORT = ROW_MASK;  // pullups
  COL_DDR |= COL_MASK;  // outputs
  COL_PORT |= COL_MASK; // initialise high (inactive)
}

char Keypad::scan() {
  int res = 0;
  for (int col=0; col<_ncols && !res; col++) {
    // turn off all columns
    COL_PORT |= COL_MASK;
    // drive the colum to test
    COL_PORT &= ~(1<<(COL0_BIT+col));
/*  Need two NOPs otherwise we can miss the column signal coming back through matrix
 *  This makes entire scan really fast (estimated at approx 60us), so ensure we don't call this scan too often! 
 *  (Every 50ms is about perfect for bounce-free readings)
 */
    asm(
      "nop\n"
      "nop\n"
     );
    // works better with a 1ms delay here - no need to debounce because it slows down scan just enough, and is still responsive
    //delay(1);
    // read the row
    byte rows = ROW_PIN & ROW_MASK;
    if (rows != ROW_MASK) {
      // there was a keypress on this column: work out which row it was
      for (int row=0; row<_nrows; row++) {
        byte rowmask = 1<<(ROW0_BIT+row);
        // check valid row
        if ((rows & rowmask) == 0) {
          // we have a row: map to key
          res = _keymap[col*_nrows + row];
          break;
        }
      }
    }
  }
  // turn off all columns
  COL_PORT |= COL_MASK;
  return res;
}

// getkey() returns only new keypresses
//
char Keypad::getkey() {
  static char prevkey = 0;
  char key = scan();

  // this is not correct - need to count N scans in agreement?
  // or a long delay after accepting a valid key?
  
  if (key != prevkey) {
    prevkey = key;
    return key;
  } else {
    return 0;
  }
}


#ifdef ENABLE_TEST
/*
 * Standalone test code - remove when using library in another project
 */

// test keymap
static const char _test_keymap[3][4] = {
  { '7', '4', '1', '0' },
  { '8', '5', '2', 'A' },
  { '9', '6', '3', 'B' },
};

// test Keypad instance
static Keypad _keypad((FlatKeymap)_test_keymap, 3, 4);

static volatile int ticks = 0;
static volatile bool process_tick = false;

#define TIMER_PERIOD_MS (50)

// test code
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Keypad test");
  init_timer();
}

void loop() {

  if (process_tick) {
    process_tick = false;
    unsigned long ms = millis();
    Serial.print(".");
    //Serial.println(ms);
    char key = _keypad.getkey();
    if (key != 0) {
      Serial.print("key ");
      Serial.println(key);
    }
  }
  
}

void init_timer() {
  OCR0A = 0xaf;
  TIMSK0 |= _BV(OCIE0A);
}

SIGNAL(TIMER0_COMPA_vect) {
  if (++ticks >= TIMER_PERIOD_MS) {
    ticks = 0;
    process_tick = true;
  }
}

#endif

