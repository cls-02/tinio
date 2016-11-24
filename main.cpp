//Tinio - the Tiny I/O tool
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <CyUSBCommon.h>
#include <libusb.h>

CY_VID_PID deviceID{0x04b4, 0x0002}; //a variable containing the id of the chip.
uint8_t *deviceArray = new uint8_t[1024];
unsigned char deviceCount;
CY_DEVICE_INFO *deviceInfo = new CY_DEVICE_INFO[1024];
//----ACTUAL CODE BELOW
int verifyArgNum(int num) //verifies the number of arguments. minimum is 2
{
  if (num < 2)
  {
    return 0;
  }
  return 1;
}

// int initLib() //initializes the cypress library
// {
//   puts("Initializing library...");
//     if (CyLibraryInit() != CY_SUCCESS)  //if the init procedure fails
//     {
//       puts("The library init failed.");
//       puts("Restarting...");                                   //the library may be running.
//       if (CyLibraryExit() != CY_SUCCESS) //try to close the possibly running library
//       {
//         return 1;                        //if even that fails, abort.
//       }
//       puts("OK");
//       initLib();                          //if the library closed gracefully
//                                           //try to reinitialize
//     }
//   puts("OK");
//
// }

void usage()
{
  puts("Tinio test build 1");
  puts("Usage:");
  //----TODO----//
  //add a command that prints a
  //usage summary.
}

int locateDevice()  //locate the device and verify it's the right one
{
  int c = CyGetDeviceInfoVidPid(deviceID, deviceArray, deviceInfo, &deviceCount, uint8_t(1024));
  puts("boop");
  printf("test %i \n", c);
  if (c != CY_SUCCESS)
  {

    puts("Device not found.");
    printf("test: %i %i err %i \n",*deviceArray,deviceCount,c);
    return -1;
  }
  printf("test: %i %i \n",*deviceArray,deviceCount);


}

int main(int argc, char const *argv[]) {
  // if (!(verifyArgNum(argc)))
  // {
  //   usage();
  //   return 2;
  // }
  // initLib();
  int c = CyLibraryInit();
  printf("foo %i\n", c );
  locateDevice();
  CyLibraryExit();
}
