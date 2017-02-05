// Tinio - the Tiny I/O tool
#include <CyUSBCommon.h>
#include <ctype.h>
#include <libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

// Those are flags used for flow control inside the program
uint8_t whichDev=177;       // 177 values indicate unset variables.
uint8_t interfaceNum=177;
uint8_t writeFlag=0;
uint8_t readFlag=0;
uint8_t exitFlag=1;
uint8_t pinNumber=177;
uint8_t valueToWrite=177;
// those vars are to be used with locateDevice function
const uint8_t maxDevs = 16; // 16 connected devices should be enough...
const CY_VID_PID deviceVidPid{UINT16(0x04b4),
                              UINT16(0x0004)}; // the id for the chip
uint8_t deviceNumList[maxDevs];
uint8_t deviceCount;
CY_DEVICE_INFO deviceInfoList[maxDevs];

// vars for deviceOpen function


CY_HANDLE deviceHandleList[maxDevs];
uint8_t defaultInterfaceNum = 0;

//----ACTUAL CODE BELOW
int verifyArgNum(
    const int num) // verifies the number of arguments. minimum is 2
{
  if (num < 2) {
    return 0;
  }
  return 1;
}

int initLib() // initializes the cypress library
{
  printf("Initializing library...");
  if (CyLibraryInit() != CY_SUCCESS) // if the init procedure fails
  {
    puts("The library init failed.");
    puts("Restarting...");
    if (CyLibraryExit() !=
        CY_SUCCESS) // try to close the possibly running library
    {
      return 1; // if even that fails, abort.
    }
    puts("OK");
    initLib(); // if the library closed gracefully
    // try to reinitialize
  }
  puts("OK");
  return 0;
}

void usage() {
  puts("Tinio test build 1");
  puts("Usage:");
  //----TODO----//
  // add a command that prints a
  // usage summary.
}

int evalErrors(const CY_RETURN_STATUS error) {
  int errCast = int(error);
  switch (errCast) { // evaluates the return status of a function
  case 0:            // everything goes OK
    puts("OK");
    return 0;
    break;
  case 1: // access denied by the OS
    puts("Acces denied. Consider sudoing.");
    return 1;
    break;
  case 2: // driver init failed. library's fault probably
    puts("Library failure. Try to rerun the tinio utility or reinstall the "
         "library");
    return 2;
    break;
  case 3: // CY_DEVICE_INFO_FETCH_FAILED error
    puts("Device info fetch failed.");
    puts("This should not happen.");
    puts("Please send me a bug report to: kristjan.komlosi@gmail.com");
    return 3;
    break;
  case 4:
    puts("Libusb can't open the device."); // another weird driver error
    return 4;
    break;
  case 5: // parameter error. shouldn't happen, but who knows...
    puts("Parameter error!");
    puts("This should not happen.");
    puts("Please send me a bug report to: kristjan.komlosi@gmail.com");
    return 5;
    break;
  case 6: // library request failed. The cypress or libusb lib may be
          // uninitialized
    puts("Request failed. Is library installed?");
    return 6;
    break;
  case 10: // device not found. nothing special, just exit
    puts("Device not found. Is tinio conected?");
    return 10;
    break;
  default:
    printf("Error no. %i!\n", errCast);
    puts("This should not happen.");
    puts("Please send me a bug report to: kristjan.komlosi@gmail.com");
    return errCast;
  }
}

int locateDevice() // locate the device and verify it's the right one
{
  printf("Locating devices...");
  try {
    CY_RETURN_STATUS retVal;
    retVal = CyGetDeviceInfoVidPid(deviceVidPid, deviceNumList, deviceInfoList,
                                   &deviceCount, maxDevs);
    throw retVal;
  } catch (CY_RETURN_STATUS errVal) {
    int r = evalErrors(errVal);
    if (r != 0) {
      return r;
    }
    if (deviceCount > 1) {
      puts("More than one device found. The first one will be used unless told "
           "otherwise.");
      return 0;
    }
    if (deviceCount == 0) {
      puts("No devices found!");
      return 10;
    }
    return 0;
  }
}

int attachDevices() {
  for (int i = 0; i < deviceCount; i++) {
    int rs = evalErrors(
        CyOpen(deviceNumList[i], interfaceNum, &deviceHandleList[i]));
    if (rs != 0) {
      return rs;
    }
  }
  return 0;
}

int setGPIO(int targetPin, uint8_t value) {
  int retval = evalErrors(CySetGpioValue(deviceHandleList[whichDev-1], targetPin, value));
  return retval;
}

int isstrdigit(const char *str) {
  for (; *str != '\0'; str++) {
    if (!(isdigit(*str))) {
      return 0;
    }
  }
  return 1;
}

void unknownOptarg(int option, char *argument)
{
  printf("Error: Unknown option argument -- \'%s\' at option -%c \n", argument, option);
}

void pinNum2HiLo(int number)
{
  printf("Error: The specified pin number - %i is too high.", number);
  puts("Pin numbers must be between 0 and 11");
}

void rwConflict()
{
  puts("Error: Can't write and read at the same time!");
}

void uselessValue()
{
  puts("Error: -v option can't be used when reading!");
}

void value2Hi(int number)
{
  printf("Error: The specified value - %i is too big/small.", number);
  puts("Values must be zero or positive integers, smaller than 255.");
}
void devNum2Hi(int number)
{
  printf("Error: The specified device number - %i is bigger than 15 or negative\n", number);
  puts("Note: The number of allowed devices can be adjusted by manipulating maxDevs");
  puts("variable in the source code.");
}


void test() {
  printf("read: %i | write: %i | pinNumber: %i | valueToWrite: %i \n", readFlag, writeFlag, pinNumber, valueToWrite);
  printf("whichDev: %i | interfaceNum: %i \n \n", whichDev, interfaceNum);
}

int parseCmdLine(int acount, char **arglist) {
  int opt;
  opt = getopt(acount, arglist, "d:s:r:v:i:");
  while (opt != -1) {
    switch (opt) {
    case 'd':
    if(!isstrdigit(optarg))
    {
      unknownOptarg(opt, optarg);
    }
    whichDev = atoi(optarg);
    if(whichDev > 15 || whichDev < 0 )
    {
      devNum2Hi(whichDev);
      whichDev=177;
      return -1;
    }
    break;

    case 's':
    if(!isstrdigit(optarg))
    {
      unknownOptarg(opt, optarg);
      return -1;
    }
    pinNumber=atoi(optarg);
    if(pinNumber > 11) {
      pinNum2HiLo(pinNumber);
      pinNumber=177;
      return -1;
    }
    if(readFlag == 1){
      rwConflict();
      pinNumber=177;
      return -1;
    }
    writeFlag=1;
    readFlag=0;
    break;

    case 'r':
    if(!isstrdigit(optarg))
    {
      unknownOptarg(opt, optarg);
      return -1;
    }
    pinNumber=atoi(optarg);
    if(pinNumber > 11) {
      pinNum2HiLo(pinNumber);
      pinNumber=177;
      return -1;
    }
    if(writeFlag == 1){
      rwConflict();
      pinNumber=177;
      return -1;
    }
    readFlag=1;
    writeFlag=0;
    break;

    case 'i':
    if(!isstrdigit(optarg))
    {
      unknownOptarg(opt, optarg);
      return -1;
    }
    interfaceNum=atoi(optarg);
    break;

    case 'v':
    if(!isstrdigit(optarg))
    {
      unknownOptarg(opt, optarg);
      valueToWrite=177;
      writeFlag=0;
      return -1;
    }
    if(readFlag==1)
    {
      uselessValue();
      valueToWrite=177;
      writeFlag=0;
      return -1;
    }
    valueToWrite=atoi(optarg);
    if(valueToWrite > 255 || valueToWrite < 0)
    {
      value2Hi(valueToWrite);
      valueToWrite=177;
      writeFlag=0;
      return -1;
    }

    }
    opt = getopt(acount, arglist, "d:s:r:v:i:");
  }

}
int main(int argc, char **argv) {
  initLib();
  locateDevice();
  attachDevices();
  parseCmdLine(argc, argv);
  test();
}
