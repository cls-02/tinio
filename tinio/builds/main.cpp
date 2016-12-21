//Tinio - the Tiny I/O tool
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <CyUSBCommon.h>
#include <CyUSBSerial.h>


//----ACTUAL CODE BELOW
int verifyArgNum(int num) //verifies the number of arguments. minimum is 2
{
  if (num < 2)
  {
    return 0;
  }
  return 1;
}

int initLib() //initializes the cypress library
{
    if (CyLibraryInit() != CY_SUCCESS)  //if the init procedure fails
    {                                   //the library may be running.
      if (CyLibraryExit() != CY_SUCCESS) //try to close the possibly running library
      {
        return 1;                        //if even that fails, abort.
      }
      initLib();                          //if the library closed gracefully
                                          //try to reinitialize
    }

}

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
;
}

int main(int argc, char const *argv[]) {
  if (!(verifyArgNum(argc)))
  {
    usage();
  }
  initLib();
}
