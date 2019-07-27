// TinI/O code rewrite of 2019 - fixing a kid's mistakes
// This code is licensed under the ISC license, by permission of myself
#include <stdbool.h>
#include <assert.h>
#include "header/CyUSBSerial.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

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

void attachHandles(CY_HANDLE *h) {
  uint8_t num = 0;
  cyErrHandler(CyGetListofDevices(&num));
  assert(num > 0); /* sanity check, no devices should trigger a
                      CY_ERROR_DEVICE_NOT_FOUND error in cyErrHandler */
  CY_DEVICE_INFO deviceInfo[num];
  for(uint8_t i = num; i >= 0; i--) {
    cyErrHandler(CyGetDeviceInfo(i, deviceInfo+i));
  }

  for (uint8_t i = 0; i < num; i++) {
    if (strcmp((char *)(deviceInfo+i) -> manufacturerName, "tinio")) {
      cyErrHandler(CyOpen(i, 0, h));
    }
  }
}

inErr parser(int argc, char **args) {
  opterr=0;
  const char *opts  = "r:w:f:d::";
  getopt(argc, args, opts);
}

int main(int argc, char **args) {

}
