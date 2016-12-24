//Tinio - A tool to connect to Tinio USB IO controllers.
#include "CyUSBCommon.h"
#include <iostream>
#include <string>
using namespace std;


namespace errorEngine
{

  enum errorLevel_t {debug = 0,warning,error,crash};
  errorLevel_t globalErrLvl = error;
  struct errorEngineCreds
  {
    string errorLocation;
    string errorName;
    int8_t errorValue;
    errorLevel_t errorLevel;
    bool isCyErrVal;
  };
  void handleErrors(errorEngineCreds eecToEval)
  {
    switch(eecToEval.errorLevel)
    {
      case debug:
        if (globalErrLvl == debug)
        {
          cerr << "Debug: " << eecToEval.errorLocation << ": " << eecToEval.errorName << endl;
        }

      case warning:
        if (globalErrLvl == warning)
        {
          cerr << "Warning: " << eecToEval.errorLocation << ": " << eecToEval.errorName << endl;
        }
      case error:
        if (globalErrLvl == error)
        {
          cerr << "ERROR: " << eecToEval.errorLocation << ": " << eecToEval.errorName << endl;
        }
      case crash:
        if (globalErrLvl == crash)
        {
          cerr << "FATAL ERROR: " << eecToEval.errorLocation << ": " << eecToEval.errorName << endl;
          cerr << "REQUESTING CRASH!" <<endl;
        }
    }
  }
}




struct accessNums_t
{
  uint8_t deviceNumber=0;
  uint8_t deviceInterfaceNumber=0;
};

class Device
{
private:
  errorEngine::errorEngineCreds localErrorCreds{"Device handler class", "", 0, };
  CY_HANDLE localDeviceHandle;
  accessNums_t localAccessNums;
public:
  Device(CY_HANDLE deviceHandle, accessNums_t accessNums)
  {
    localDeviceHandle = deviceHandle;
    localAccessNums = accessNums;
  }
  Device(void);
  void deviceSet()
  {
  }
  void deviceDestroy();
};
