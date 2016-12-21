// Tinio - the Tiny I/O tool
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <CyUSBCommon.h>
#include <libusb.h>

//those vars are to be used with locateDevice function
const UINT8 devInfoListLen = 200;
const CY_VID_PID deviceVidPid {UINT16(0x04b4), UINT16(0x0002)};
UINT8 deviceNumList[devInfoListLen];
UINT8 deviceCount;
CY_DEVICE_INFO deviceInfoList[devInfoListLen];

//vars for deviceOpen function
CY_HANDLE deviceHandle;
UINT8 intNum;
UINT8 devNum;


//----ACTUAL CODE BELOW
int verifyArgNum(const int num)  // verifies the number of arguments. minimum is 2
{
  if (num < 2)
  {
    return 0;
  }
  return 1;
}

int initLib() //initializes the cypress library
{
  printf("Initializing library...");
    if (CyLibraryInit() != CY_SUCCESS)  //if the init procedure fails
    {
      puts("The library init failed.");
      puts("Restarting...");
      if (CyLibraryExit() != CY_SUCCESS) //try to close the possibly running library
      {
        return 1;                        //if even that fails, abort.
      }
      puts("OK");
      initLib();                          //if the library closed gracefully
                                          //try to reinitialize
    }
  puts("OK");

}

void usage()
{
  puts("Tinio test build 1");
  puts("Usage:");
  //----TODO----//
  // add a command that prints a
  // usage summary.
}

int evalErrors(const CY_RETURN_STATUS error)
{
  int errCast = int(error);
  switch (errCast) {            //evaluates the return status of a function
    case 0:                     //everything goes OK
      puts("OK");
      return 0;
      break;
    case 1:                     //access denied by the OS
      puts("Acces denied. Consider sudoing.");
      return 1;
      break;
    case 2:                     //driver init failed. library's fault probably
      puts("Library failure. Try to rerun the tinio utility or reinstall the library");
      return 2;
      break;
    case 3:                     //CY_DEVICE_INFO_FETCH_FAILED error
      puts("Device info fetch failed.");
      puts("This should not happen.");
      puts("Please send me a bug report to: kristjan.komlosi@gmail.com");
      return 3;
      break;
    case 4:
      puts("Libusb can't open the device.");    //another weird driver error
      return 4;
      break;
    case 5:                     //parameter error. shouldn't happen, but who knows...
      puts("Parameter error!");
      puts("This should not happen.");
      puts("Please send me a bug report to: kristjan.komlosi@gmail.com");
      return 5;
      break;
    case 6:                     //library request failed. The cypress or libusb lib may be uninitialized
      puts("Request failed. Is library installed?");
      return 6;
      break;
    case 10:                    //device not found. nothing special, just exit
      puts("Device not found. Is tinio conected?");
      return 10;
      break;
    default:
      printf("Error no. %i!\n", errCast);
      puts("This should not happen.");
      puts("Please send me a bug report to: kristjan.komlosi@gmail.com");

  }
}

int locateDevice() // locate the device and verify it's the right one
{
  printf("Locating devices...");
  try
  {
    CY_RETURN_STATUS retVal;
    retVal = CyGetDeviceInfoVidPid(deviceVidPid, deviceNumList, deviceInfoList, &deviceCount, devInfoListLen);
    throw retVal;
  }
  catch(CY_RETURN_STATUS errVal)
  {
    int r = evalErrors(errVal);
    if( r != 0 )
    {
      return r;
    }
    if (deviceCount > 1)
    {
      puts("More than one device found. The first one will be used unless told otherwise.");
      return 0;
    }
    return 0;
  }
}

int selectDevice(const UINT8 whichDev = 1)
{
  if (deviceCount == 0)
  {
    puts("No devices found!");
    return 10;
  }
  if (whichDev > deviceCount)
  {
    puts("The requested device doesn't exist.");
    return 5;
  }
  int errval = CyOpen(13, 0, &deviceHandle);
  printf("errrval: %i, device %i\n", errval, deviceNumList[whichDev]);


}



int main(int argc, char const *argv[])
{
  initLib();
  locateDevice();
  for (size_t i = 0; i < 16; i++) {
  selectDevice(i);  /* code */
  }


}
