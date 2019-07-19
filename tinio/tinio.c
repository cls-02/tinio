// TinI/O code rewrite of 2019 - fixing a kid's mistakes
// This code is licensed under the ISC license
#include <stdbool.h>
#include <assert.h>
#include "header/CyUSBSerial.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum inErr {
  SUCCESS = 0,
  USAGE,
  INVALID,
  NO_DEVICES
} inErr;

void cyErrHandler(CY_RETURN_STATUS err);
void inErrHandler(inErr err);

bool setPin(CY_HANDLE h, int pinNumber, bool value) {
  assert(pinNumber <= 11); // sanity checks, should be sanitized in the parser
  assert(pinNumber >=0);
  CY_RETURN_STATUS r = CySetGpioValue(h, pinNumber, value);
  if (r != CY_SUCCESS) cyErrHandler(r);
  return value;
}

bool getPin(CY_HANDLE h, int pinNumber){
  assert(pinNumber <= 11); // sanity checks, should be sanitized in the parser
  assert(pinNumber >=0);
  uint8_t v;
  CY_RETURN_STATUS r = CyGetGpioValue(h, pinNumber, &v);
  if (r != CY_SUCCESS) cyErrHandler(r);
  return (bool)v;
}

bool flpPin(CY_HANDLE h, int pinNumber) {
  assert(pinNumber <= 11); // sanity checks, should be sanitized in the parser
  assert(pinNumber >=0);
  bool val = getPin(h, pinNumber);
  val = !val;
  setPin(h, pinNumber, val);
  return val;
}


void parser(int argc, char **args);

inErr attachHandles() {

}

int main(int argc, char **args){
  CY_RETURN_STATUS r = CyLibraryInit();
  if (r != CY_SUCCESS) cyErrHandler(r);


}
