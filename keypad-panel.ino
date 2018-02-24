#include "keypad.h"
#include "PersistentStore.h"

#define TIMER_PERIOD_MS (50)

typedef enum {
  NONE = 0,
  ARMING,
  LOCKED,
  CODE_ENTRY,
  VALIDATING,
  UNLOCKED,
  PIN_UPDATE,
  DENIED,
  ALARM,
  PANIC
} State;

static const char _default_pin[4] = { '1', '2', '3', '4' };

// keymap
static const char _keymap[3][4] = {
  { '7', '4', '1', '0' },
  { '8', '5', '2', 'A' },
  { '9', '6', '3', 'B' },
};

static State _state = NONE;
static Keypad _keypad((FlatKeymap)_keymap, 3, 4);
static PersistentStore _persistentStore;

static volatile int ticks = 0;
static volatile bool process_tick = false;
static char _pin_code[PIN_CODE_LEN];
static char _entered_code[PIN_CODE_LEN];
static char _current_digit_index = 0;



void init_timer() {
  OCR0A = 0xaf;
  TIMSK0 |= _BV(OCIE0A);
}

// test code
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Keypad test");

  if (_persistentStore.readPinCode(_pin_code)) {
    Serial.print("read PIN code from EEPROM: ");
//    for (int i=0; i<PIN_CODE_LEN; i++) {
//      Serial.print(_pin_code[i]);
//    }
    printDigits(_pin_code, PIN_CODE_LEN);
    Serial.println();
  } else {
    // default code (factory reset)
    for (int i=0; i<PIN_CODE_LEN; i++) {
      _pin_code[i] = _default_pin[i];
    }
    Serial.println("No valid PIN code in EEPROM: reverting to factory setting");
    _persistentStore.writePinCode(_default_pin);
  }

  init_timer();

  updateState(LOCKED);
}

void loop() {

  if (process_tick) {
    process_tick = false;
    unsigned long ms = millis();
    char key = _keypad.getkey();
    if (key != 0) {
      Serial.print("key ");
      Serial.println(key);
      handleKeypress(key);
    }
  }
  
}

void printDigits(char *digits, int count) {
  for (int i=0; i<count; i++) {
    Serial.print(digits[i]);
  }
}

bool verifyPin() {
  for (int i=0; i<PIN_CODE_LEN; i++) {
    if (_entered_code[i] != _pin_code[i]) {
      return false;
    }
  }
  return true;  
}

void updateState(State newState) {
  if (newState != _state) {
    switch (newState) {
      case LOCKED:
        Serial.println("LOCKED");
        break;
      case ARMING:
        Serial.println("ARMING");
        break;
      case CODE_ENTRY:
        _current_digit_index = 0;
        break;
      case VALIDATING:
        break;
      case UNLOCKED:
        Serial.println("UNLOCKED");
        break;
      case DENIED:
        Serial.println("DENIED");
        // audible alert
        break;
      default:
        break;
    }
    _state = newState;
  }
}


void handleKeypress(char key) {

  if (_state == LOCKED) {
    updateState(CODE_ENTRY);
  }
  if (_state == CODE_ENTRY) {
    _entered_code[_current_digit_index] = key;
    if (++_current_digit_index >= PIN_CODE_LEN) {
      Serial.print("Checking PIN: ");
      printDigits(_entered_code, PIN_CODE_LEN);
      Serial.println();        
      if (verifyPin()) {
        updateState(UNLOCKED);
      } else {
        updateState(DENIED);
        delay(2);
        updateState(LOCKED);
      }
    }
  } else if (_state == UNLOCKED) {
    if (key == 'A') {
      // audible warning plus exit delay
      updateState(ARMING);
      delay(5);
      updateState(LOCKED);
    }
  }
}


ISR(TIMER0_COMPA_vect) {
  if (++ticks >= TIMER_PERIOD_MS) {
    ticks = 0;
    process_tick = true;
  }
}


