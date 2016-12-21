//Tinio - A tool to connect to Tinio USB IO controllers.
#include "CyUSBCommon.h"
#include <iostream>
#include <string>
using namespace std;

enum errorLevel_t {debug = 0,warning,error,crash};
namespace errorEngine
{
  struct errorEngineCreds
  {
    string errorLocation;
    string errorName;
    int8_t errorValue;
    errorLevel_t errorLevel;
  };
  void handleErrors(errorEngineCreds);
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
  void deviceSet();
  void deviceDestroy();
};
