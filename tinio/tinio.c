// TinI/O code rewrite of 2019 - fixing a kid's mistakes
// This code is licensed under the ISC license, by permission of myself
#include <stdbool.h>
#include <assert.h>
#include "header/CyUSBSerial.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum inErr {
  SUCCESS = 0,
  USAGE,
  INVALID,
  NO_DEVICES
} inErr;

void cyErrHandler(CY_RETURN_STATUS err) {
  if (err == CY_SUCCESS) {
    return;
  }
}
void inErrHandler(inErr err) {
  if (err == SUCCESS) {
    return;
  }
}

bool setPin(CY_HANDLE h, int pinNumber, bool value) {
  assert(pinNumber <= 11); // sanity checks, should be sanitized in the parser
  assert(pinNumber >=0);
  cyErrHandler(CySetGpioValue(h, pinNumber, value));
  return value;
}

bool getPin(CY_HANDLE h, int pinNumber){
  assert(pinNumber <= 11); // sanity checks, should be sanitized in the parser
  assert(pinNumber >=0);
  uint8_t v;
  cyErrHandler(CyGetGpioValue(h, pinNumber, &v));
  return (bool) v;
}

bool flpPin(CY_HANDLE h, int pinNumber) {
  assert(pinNumber <= 11); // sanity checks, should be sanitized in the parser
  assert(pinNumber >= 0);
  bool val = getPin(h, pinNumber);
  val = !val;
  setPin(h, pinNumber, val);
  return val;
}

void parser(int argc, char **args);

<<<<<<< HEAD
void attachHandles(CY_HANDLE *h) {
  uint8_t num = 0;
  cyErrHandler(CyGetListofDevices(&num));
  assert(num > 0); /* sanity check, no devices should trigger a
                      CY_ERROR_DEVICE_NOT_FOUND error in cyErrHandler*/
  CY_DEVICE_INFO deviceInfo[num];
  for(uint8_t i = num; i >= 0; i--) { // iterate through the device info array
    /*  Pointer arithmetics: same as &deviceInfo[num] but without an
        unnecessary reference-to-dereference narrative. Keep it simple, stupid!
    */
    cyErrHandler(CyGetDeviceInfo(num, deviceInfo+i)); // fill deviceInfo
  }
  /*  at this point, every element of deviceInfo should have a CY_DEVICE_INFO
      struct inside. */
  // TODO: assert dis.
  for (uint8_t i = 0; i < num; i++) {
    if (strcmp((deviceInfo+i) -> manufacturerName, "tinio")) {
      cyErrHandler(CyOpen(i, 0, h));
    }
  }
  /*  This abominable trainwreck of a hack needs an explanation. i iterates
      through the deviceInfo array, until it hits a device with the manufacturer
      string set to "tinio". It then calls CyOpen on the device. This WILL fail
      miserably if the device is set to CDC mode, but custom and PHDC work fine.
  */
  // TODO: mek dis betr
=======
inErr attachHandles() {
  ;
>>>>>>> 107e36b03d1a4f9c568d462d8a245ac01a1f51da
}

int main(int argc, char **args){
}
