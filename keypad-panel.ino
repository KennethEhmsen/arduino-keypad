#include "keypad.h"

// keymap
static const char _keymap[3][4] = {
  { '7', '4', '1', '0' },
  { '8', '5', '2', 'A' },
  { '9', '6', '3', 'B' },
};

// test Keypad instance
static Keypad _keypad((FlatKeymap)_keymap, 3, 4);

static volatile int ticks = 0;
static volatile bool process_tick = false;

#define TIMER_PERIOD_MS (50)

void init_timer() {
  OCR0A = 0xaf;
  TIMSK0 |= _BV(OCIE0A);
}

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


ISR(TIMER0_COMPA_vect) {
  if (++ticks >= TIMER_PERIOD_MS) {
    ticks = 0;
    process_tick = true;
  }
}


