// TinI/O code rewrite of 2019 - fixing a kid's mistakes
// This code is licensed under the ISC license
#include <stdbool.h>
#include <assert.h>
#include "header/CyUSBSerial.h"
#include <stdio.h>

typedef enum inErr {
  USAGE = 0,
  INVALID
} inErr;

void cyErrHandler(CY_RETURN_STATUS err);
void inErrHandler(inErr err);

bool setPin(CY_HANDLE h, int pinNumber, bool value) {
  assert(pinNumber <= 11); // sanity checks, should be sanitized in the parser
  assert(pinNumber >=0);
  CY_RETURN_STATUS err = CySetGpioValue(h, pinNumber, value);
  if (err) cyErrHandler(err);
  return value;
}

bool getPin(CY_HANDLE h, int pinNumber){
  assert(pinNumber <= 11); // sanity checks, should be sanitized in the parser
  assert(pinNumber >=0);
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
int main(int argc, char **args);
