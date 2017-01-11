// Tinio - A tool to connect to Tinio USB IO controllers.
#include "CyUSBCommon.h"
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

namespace errorEngine {

enum errorLevel_t { debug = 0, warning, error, crash };

errorLevel_t globalErrLvl = error;

struct errorEngineCreds {
  string errorLocation;
  string errorName;
  int8_t errorValue;
  errorLevel_t errorLevel;
};

void controlledCrash() {
  cerr << "Triggering a controlled crash..." << endl;
  exit(-177);
}


void handleErrors(errorEngineCreds eecToEval) {
  switch (eecToEval.errorLevel) {
  case debug:
    if (globalErrLvl <= debug) {
      cerr << "Debug: " << eecToEval.errorLocation << ": "
           << eecToEval.errorName << endl;
    }

  case warning:
    if (globalErrLvl <= warning) {
      cerr << "Warning: " << eecToEval.errorLocation << ": "
           << eecToEval.errorName << endl;
    }
  case error:
    if (globalErrLvl <= error) {
      cerr << "ERROR: " << eecToEval.errorLocation << ": "
           << eecToEval.errorName << endl;
    }
  case crash:
    if (globalErrLvl <= crash) {
      cerr << "FATAL ERROR: " << eecToEval.errorLocation << ": "
           << eecToEval.errorName << endl;
      cerr << "REQUESTING CRASH!" << endl;
      controlledCrash();
    }
  }
}


}

struct accessNums_t {
  uint8_t deviceNumber = 0;
  uint8_t deviceInterfaceNumber = 0;
};

class Device {
private:
  errorEngine::errorEngineCreds localErrorCreds{
      "Device handler class", "", 0, errorEngine::errorLevel_t::debug};
  CY_HANDLE localDeviceHandle;
  accessNums_t localAccessNums;

  int callExternFun(CY_RETURN_STATUS function()) {
    int retval = function();
    if (retval != CY_SUCCESS) {
      this->localErrorCreds.errorValue = retval;
      localErrorCreds.errorName = "error number ";
      localErrorCreds.errorName.append(itoa(localErrorCreds.errorValue));
      errorEngine::handleErrors(localErrorCreds);
      return -177;
    }
  }

  public:
    Device(CY_HANDLE deviceHandle, accessNums_t accessNums) {
      localDeviceHandle = deviceHandle;
      localAccessNums = accessNums;
    }
    Device(void);
    void deviceSet(accessNums_t deviceAccessNums) {
      CyOpen(deviceAccessNums.deviceNumber,
             deviceAccessNums.deviceInterfaceNumber, &localDeviceHandle);
    }
    void deviceDestroy();
};
