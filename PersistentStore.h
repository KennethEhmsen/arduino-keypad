#ifndef __PersistentStore_h
#define __PersistentStore_h

#include <EEPROM.h>

#define PIN_CODE_LEN                  (4)

#define NV_PIN_CODE                   (0)
#define NV_PIN_CHECKSUM               (NV_PIN_CODE+PIN_CODE_LEN)
#define NV_TOP                        (NV_PIN_CHECKSUM+1)

class PersistentStore
{
  short _currentTop;
  //PersistentStore() : _currentTop(0) {}
public:
  bool readPinCode(char *digits);
  void writePinCode(char *digits); 
  char checksum(char *digits, int count);

protected:
//  template <class T> void createRecord( short key, const T& value );
//  float getFloatValueForKey( short key );
//  bool setFloatValueForKey( short key, float value );
  byte getByteValueForAddr( byte key );
  bool setByteValueForAddr( short key, byte value );

  template <class T> int writeBlob(int ee, const T& value);
  template <class T> int readBlob(int ee, T& value);
};


#endif __PersistentStore_h

