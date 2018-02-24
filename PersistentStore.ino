#include <EEPROM.h>

#include "PersistentStore.h"

bool PersistentStore::readPinCode(char *digits)
{
  for (int i=0; i<PIN_CODE_LEN; i++) {
    digits[i] = EEPROM.read(NV_PIN_CODE+i);
  }
  char calcChecksum = checksum(digits, PIN_CODE_LEN);
  char storedChecksum = EEPROM.read(NV_PIN_CHECKSUM);
  return calcChecksum == storedChecksum;
}


void PersistentStore::writePinCode(char *digits)
{
  for (int i=0; i<PIN_CODE_LEN; i++) {
    EEPROM.write(NV_PIN_CODE+i, digits[i]);
  }
  char calcChecksum = checksum(digits, PIN_CODE_LEN);
  EEPROM.write(NV_PIN_CHECKSUM, calcChecksum);
}


char PersistentStore::checksum(char *digits, int count)
{
  char cks = 0;
  for (int i=0; i<count; i++) {
    cks ^= digits[i];
  }
  return cks;
}
/*
// not currently used
template <class T> void PersistentStore::createRecord( short key, const T& value )
{
  short sz = writeBlob( _currentTop, value );
  // store key->address map in EEPROM to save RAM and for persistence?
}
*/


// Primitives
//

/*
float PersistentStore::getFloatValueForKey( short key )
{
  
}

bool PersistentStore::setFloatValueForKey( short key, float value )
{
}
*/
byte PersistentStore::getByteValueForAddr(byte addr)
{
  return EEPROM.read(addr);
}

bool PersistentStore::setByteValueForAddr(short addr, byte value)
{
  if (addr < NV_TOP)
  {
    EEPROM.write(addr, value);
    return true;
  }
  return false;
}

// these templates allow read/write of arbitrary types and return the number of bytes used.
// use this to keep track of EEPROM file layout. Keys can be ordinal and mapping can be done inside class.
//
template <class T> int PersistentStore::writeBlob(int addr, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    int i;
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(addr++, *p++);
    return i;
}

template <class T> int PersistentStore::readBlob(int addr, T& value)
{
    byte* p = (byte*)(void*)&value;
    int i;
    for (i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(addr++);
    return i;
}



