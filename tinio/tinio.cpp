// Tinio - the Tiny I/O tool
#include "header/CyUSBSerial.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Those are flags used for flow control inside the program
uint16_t whichDev = 0;
uint16_t interfaceNum = 0;
uint16_t pinNumber = 0;
bool valueToWrite=false, writeFlag=false, readFlag=false, exitFlag=false, expertModeFlag=false; // flags
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
  // printf("Initializing library...");
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
  // puts("OK");
  return 0;
}

void usage() { // prints usage; called when program has no arguments
  puts("TinI/0 0.1");
  puts("Usage:");
  puts("tinio <options>");
  puts("The supported options are:");
  puts("-d<device number> - specifies the desired device - integer 0 to 15");
  puts("-i<interface number - specifies the USB interface number - integer 0 "
       "to 255");
  puts("-s<pin number> - sets the specified pin to the value specified with -v "
       " - integer 0 to 11");
  puts("-r<pin number> - reads the specified pin's value and prints it to the "
       "stdout - integer 0 to 11");
  puts("-v<value> - value for -s option - integer 0 to 1");
  puts("-e - enables expert mode (enables additional pins)");
}

// THOSE ARE ALL ARGUMENT ERROR MESSAGES!
// BEGIN

void unknownOptarg(int option, char *argument) {
  printf("Error: Unknown option argument -- \'%s\' at option -%c \n", argument,
         option);
}

void pinNum2HiLo(int number) {
  printf("Error: The specified pin number - %i is too high.\n", number);
  puts("Pin numbers must be between 0 and 11");
    exit(-1);
}

void rwConflict() { puts("Error: Can't write and read at the same time!"); }

void uselessValue() { puts("Error: -v option can't be used when reading!"); }

void value2Hi(int number) {
  printf("Error: The specified value - %i is too big/small.\n", number);
  puts("Values can only be zero or one.");
}
void devNum2Hi(int number) {
  printf("Error: The specified device number - %i is bigger than 15 or "
         "negative!\n",
         number);
  puts("Note: The number of allowed devices can be adjusted by manipulating "
       "maxDevs");
  puts("variable in the source code.");
}
void intNum2Hi(int number) {
  printf("Error: The specified interface number - %i is too big! \n", number);
  puts("Interface numbers must be unsigned 8-bit integers \n (smaller than 256 "
       "and bigger or equal than 0).");
}
void notAnExpert(int number) {
  printf("Error: The specified pin - %i isn't accessible unless in the "
         "expert mode.\n",
         number);
  puts("Those pins are assigned a special function in the chip's SCB block.");
  puts("If you still want to do it, use the -e option");
}

// END

int evalErrors(const CY_RETURN_STATUS error) { // evaluates errors, duh?
  int errCast = int(error); // casts the library's error value type into int
  switch (errCast) {        // evaluates the return status of a function
  case 0:                   // everything goes OK
    // puts("OK");   // don't need those debug messages anymore.
    return 0;
    break;
  case 1: // access denied by the OS          // an error handler structure:
    puts("access denied. Consider sudoing."); // print the error messages
    return 1; // return from the function with casted error number
    break;    // break to assure no errors (in the error function, cynical isn't
              // it?)
  case 2:     // driver init failed. library's fault probably
    puts("Library failure. Try to rerun the tinio utility or reinstall the"
         "library.");
    return 2;
    break;
  case 3: // CY_DEVICE_INFO_FETCH_FAILED error
    puts("Device info fetch failed.");
    puts("This should not happen.");
    puts("Please send me a bug report to: thecodingkid.devel@gmail.com.");
    return 3;
    break;
  case 4:
    puts("Libusb can't open the device."); // another weird driver error
    return 4;
    break;
  case 5: // parameter error. shouldn't happen, but who knows...
    puts("Parameter error!");
    puts("This should not happen.");
    puts("Please send me a bug report to: thecodingkid.devel@gmail.com.");
    return 5;
    break;
  case 6: // library request failed. The cypress or libusb lib may be
          // uninitialized
    puts("Request failed. Check if you passed the right interface");
    puts("number and if the library is set up correctly.");
    return 6;
    break;
  case 10: // device not found. nothing special, just exit
    puts("Device not found. Is the target device conected?");
    return 10;
    break;
  default: // if everything else fails, there is a generic error message that
           // saves the day ;)
    printf("Error no. %i!\n", errCast);
    puts("This should definitely not happen.");
    puts("Please send me a bug report to: thecodingkid.devel@gmail.com");
    return errCast;
  }
}

int locateDevice() // locate the device and verify it's the right one
{
  // printf("Locating devices...");         //debug, not needed

    CY_RETURN_STATUS retVal;
    retVal = CyGetDeviceInfoVidPid( //--searches for the device with given VID/PID
        deviceVidPid, deviceNumList, //--and stores them into lists
        deviceInfoList,         //--refer to lib docs for more info
        &deviceCount, maxDevs);


    int r = evalErrors(retVal); // evaluate the errors
    if (r != 0) {               // exit on error
      return r;
    }
    if (deviceCount > 1) { // those tiny ifs make the function dummy-proof
      return 0;
    }
    if (deviceCount == 0) {
      puts("No devices found!");
      return 10;
    }
    return 0;
}

int attachDevices() { // loops through devices and attaches them to handles
  for (int i = 0; i < deviceCount; i++) {
    int rs = evalErrors(
        CyOpen(deviceNumList[i], interfaceNum, &deviceHandleList[i]));
    if (rs != 0) { // exit on error
      return rs;
    }
  }
  return 0;
}

int setGPIO() { // sets the chosen gpio. arguments are taken from the global
                // flags
                // and vars at the top of the file
  if (writeFlag == false) // error checking;
    return -1;
  if (expertModeFlag == false &&
      (pinNumber == 6 || pinNumber == 5))
       // if expert mode isn't enabled you can't mess with some pins
  {
    notAnExpert(pinNumber);
    return -1;
  }
  int errval = evalErrors( // real code here
      CySetGpioValue(deviceHandleList[whichDev], pinNumber,
                     uint8_t(valueToWrite)));
  return errval;
}

int readGPIO() { // similiar to setGPIO just it reads
  if (readFlag == false)
    return -1;
  if (expertModeFlag == false &&
      (pinNumber == 6 || pinNumber == 5))
       // if expert mode isn't enabled you can't mess with some pins
  {
    notAnExpert(pinNumber);
    return -1;
  }
  uint8_t value;
  int errval =
      evalErrors(CyGetGpioValue(deviceHandleList[whichDev], pinNumber, &value));
  printf("%i\n", value); // print value to stdout
  return errval;
}

int isstrdigit(const char *str) { // a helper function for argument parser
  for (; *str != '\0'; str++) {
    if (!(isdigit(*str))) {
      return 0;
    }
  }
  return 1;
}

void test() { // a pretty good debug function
  printf("read: %i | write: %i | pinNumber: %i | valueToWrite: %i \n", readFlag,
         writeFlag, pinNumber, uint8_t(valueToWrite));
  printf("whichDev: %i | interfaceNum: %i \n \n", whichDev, interfaceNum);
}

int parseCmdLine(int acount, char **arglist) {  // the command line parser
  int opt;                                      // a wide character for getopt
  opt = getopt(acount, arglist, "d:s:r:v:i:u"); // getopts from parameters
  while (opt != -1) { // cycle if there are still options the command line
    switch (opt) {
    case 'd':                       // specifies the desired device
      if (!isstrdigit(optarg)) {    // if the opt's parameter isn't a number...
        unknownOptarg(opt, optarg); //...tell that to the user
        exitFlag = true;            // exit
        return -1;
      }
      whichDev = atoi(optarg); // set the target device var
      if (whichDev > 15 ||
          whichDev < 0) {    // if the specified device number is too high...
        devNum2Hi(whichDev); // error message
        whichDev = 0;        // reset the device number
        exitFlag = true;     // then exit
        return -1;           // return with error
      }
      break;

    case 's': // set a pin to value, value is specified with -v
      if (!isstrdigit(optarg)) {    // argument has to be a digit
        unknownOptarg(opt, optarg); // error message
        exitFlag = true;            // exit
        return -1;
      }
      pinNumber = atoi(optarg);              // set target var
      if (pinNumber > 11 || pinNumber < 0) { // pin num checks
        pinNum2HiLo(pinNumber);
        pinNumber = 0;
        exitFlag = true;
        return -1;
      }
      if (readFlag == true) { // for preventing read/write conflicts
        rwConflict();
        pinNumber = 0;
        exitFlag = true;
        return -1;
      }
      writeFlag = true; // enable write
      readFlag = false; // disable read
      break;

    case 'r': // reads a pin, value is printed to stdout
      if (!isstrdigit(optarg)) {
        unknownOptarg(opt, optarg);
        exitFlag = true;
        return -1;
      }
      pinNumber = atoi(optarg);
      if (pinNumber > 11 || pinNumber < 0) {
        pinNum2HiLo(pinNumber);
        exitFlag = true;
        pinNumber = 0;
        return -1;
      }
      if (writeFlag == true) {
        rwConflict();
        pinNumber = 0;
        return -1;
      }
      readFlag = true;
      writeFlag = false;
      break;

    case 'i':                    // used to specify USB interface number
      if (!isstrdigit(optarg)) { // if argument not a number
        unknownOptarg(opt, optarg);
        exitFlag = true;
        return -1;
      }
      interfaceNum = atoi(optarg);
      if (interfaceNum > 255 || interfaceNum < 0) { // if argument to high/low
        intNum2Hi(interfaceNum);                    // err message
        exitFlag = true;                            // exit
        interfaceNum = 0;                           // reset the var
        return -1;                                  // return
      }

      break;

    case 'v':   // the value to write to a pin
      if (!isstrdigit(optarg)) {  // number check
        unknownOptarg(opt, optarg);
        valueToWrite = false;
        exitFlag = true;
        writeFlag = false;
        return -1;
      }
      if (readFlag == true) { //conflict check
        uselessValue();
        valueToWrite = false;
        exitFlag = true;
        writeFlag = false;
        return -1;
      }
      (atoi(optarg) == 0) ? valueToWrite = false : valueToWrite = true; // set the value
    case 'e':
      expertModeFlag=true;
    }
    opt = getopt(acount, arglist, "d:s:r:v:i:e");
  }
  return 0;
}

int exitOnError() { //this function exits
  if (exitFlag == true) { //if the exit flag is on
    puts("Exiting");
    exit(-1);
    return -1;
  }
  if ((writeFlag | readFlag) == 0) { // or if nothing is ordered to do
    usage();
    exit(-1);
    return -1;
  }
  return 0;
}

int main(int argc, char **argv) {
  parseCmdLine(argc, argv);
  // test();
  exitOnError();
  initLib();
  locateDevice();
  attachDevices();
  setGPIO();
  readGPIO();
  return 0;
}
