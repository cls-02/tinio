# Welcome to the TinI/O Manual!

Welcome to the TinI/O manual. In this document you can find information about installing, configuring and using TinI/O, the simple interface to I/O.

-----

# What is TinI/O?
Simply put, TinI/O is a Linux CLI program that, when combined with a certain chip, adds more general purpose input/output (GPIO) pins to your computer. They can then be used to control custom electronics outside the computer, for example a MOSFET driving a fan, a LED, a sensor or anything that can be controlled or read from with logic ones and zeros. That certain chip is in this case, the [Cypress CY7C65211 USB-Serial Bridge Controller](http://www.cypress.com/products/usb-uart-controller-gen-2). TinI/O sends commands to it over USB using [the libcyusbserial library](https://github.com/cyrozap/libcyusbserial) made by cyrozap and [the libusb library](http://libusb.info/). When the chip recieves the command, it changes or reads the state of the requested GPIO pin.

Using TinI/O is free (as beer _and_ speech), because it's licensed under GNU General Public License. You can use it free of any charges, but you need a TinI/O compatible board ([like this one](http://www.cypress.com/documentation/development-kitsboards/psoc-4-cy8ckit-049-4xxx-prototyping-kits)), that (at the time of writing) costs $4 at Cypress.com.

# Where to start?
To start using TinI/O, make sure you have:
- A computer running some sort of Linux that has:
  + A free USB port to use the board with
  + installed libusb
  + 16 MB of free space (or 8 for the bare minimum)
  + A C and C++ compiler (preferably gcc and g++)


- A TinI/O compatible board

  They can be obtained from Cypress, the manufacturer of the chip. The board I personally use and recommend is the [CY8CKIT-049-4xxx PSoC4 prototyping kit](http://www.cypress.com/documentation/development-kitsboards/psoc-4-cy8ckit-049-4xxx-prototyping-kits), but there are more of them out there, for example, the [CYUSBS234 USB-Serial (Single Channel) Development Kit](http://www.cypress.com/documentation/development-kitsboards/cyusbs234-usb-serial-single-channel-development-kit).

# Building and installing TinI/O

## The easy way
After you make sure that your computer complies with the system requirements (it probably does), you need to build and install TinI/O. The simplest way to do that is to run the `autobuilder` script in the project root (aka the folder you downloaded TinI/O source to) __as root__. The script will then start the build process and redirect any text from its underlying programs to the standard output. The output should look like this:

```
# ./autobuilder
    --------------------------
    Autobuilder for TinI/O 0.1
    --------------------------
    Verifying main directory...
    DONE!
    Entering the library directory...
    DONE!
    Compiling and installing the library...
gcc -fPIC -g -Wall -o libcyusb.o -c cyusb.c -I ../../common/header
cyusb.c: In function ‘CyOpen’:
cyusb.c:556:1: warning: ‘rStatus’ may be used uninitialized in this function [-Wmaybe-uninitialized]
 printf("rstatus6 %d", rStatus);
 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
gcc -fPIC -g -Wall -o libcyuart.o -c cyuart.c -I ../../common/header
gcc -fPIC -g -Wall -o libcyi2c.o -c cyi2c.c -I ../../common/header
gcc -fPIC -g -Wall -o libcyspi.o -c cyspi.c -I ../../common/header
gcc -fPIC -g -Wall -o libcyphdc.o -c cyphdc.c -I ../../common/header
gcc -fPIC -g -Wall -o libcyjtag.o -c cyjtag.c -I ../../common/header
gcc -fPIC -g -Wall -o libcymisc.o -c cymisc.c -I ../../common/header
gcc -fPIC -g -Wall -o libcyboot.o -c cyboot.c -I ../../common/header
cyboot.c:72:0: warning: "CY_USB_SERIAL_TIMEOUT" redefined
 #define CY_USB_SERIAL_TIMEOUT 0

In file included from cyboot.c:1:0:
CyUSBCommon.h:74:0: note: this is the location of the previous definition
 #define CY_USB_SERIAL_TIMEOUT 5000

cyboot.c: In function ‘CyFlashConfigEnable’:
cyboot.c:634:21: warning: unused variable ‘ioTimeout’ [-Wunused-variable]
     UINT32 rStatus, ioTimeout = CY_USB_SERIAL_TIMEOUT;
                     ^~~~~~~~~
gcc -shared -g -Wl,-soname,libcyusbserial.so -o libcyusbserial.so.1 libcyusb.o libcyuart.o libcyi2c.o libcyspi.o libcyphdc.o libcyjtag.o libcymisc.o libcyboot.o -l usb-1.0
cp libcyusbserial.so.1 /usr/local/lib
ln -sf /usr/local/lib/libcyusbserial.so.1 /usr/local/lib/libcyusbserial.so
ldconfig
rm -f libcyusb.o libcyuart.o libcyspi.o libcyi2c.o libcyphdc.o libcyjtag.o libcymisc.o libcyboot.o
    DONE!
    Library installation complete.
    Going back...
    DONE!
    Entering the TinI/O build directory
    DONE!
    Compiling TinI/O
echo "Building tinio..."
Building tinio...
g++ -lcyusbserial tinio.cpp -o tinio
echo "done."
done.
echo "Installing tinio..."
Installing tinio...
cp tinio /usr/bin
echo "done."
done.
    DONE!
    Compiling the flasher tool
#	gcc -g -o CyUSBSerialTestUtility Test_Utility.c -lcyusbserial -w
gcc -g -o CyUSBSerialCommandUtility Command_Utility.c -lcyusbserial
Command_Utility.c: In function ‘deviceHotPlug’:
Command_Utility.c:102:35: warning: passing argument 1 of ‘CyGetListofDevices’ from incompatible pointer type [-Wincompatible-pointer-types]
     rStatus = CyGetListofDevices (&numDevices);
                                   ^
In file included from Command_Utility.c:33:0:
../../common/header/CyUSBSerial.h:705:30: note: expected ‘UINT8 * {aka unsigned char *}’ but argument is of type ‘int *’
 CYWINEXPORT CY_RETURN_STATUS CyGetListofDevices (
                              ^~~~~~~~~~~~~~~~~~
Command_Utility.c:105:16: warning: ‘return’ with a value, in function returning void
         return rStatus;
                ^~~~~~~
Command_Utility.c:95:6: note: declared here
 void deviceHotPlug () {
      ^~~~~~~~~~~~~
Command_Utility.c:107:5: warning: implicit declaration of function ‘printListOfDevices’ [-Wimplicit-function-declaration]
     printListOfDevices (false);
     ^~~~~~~~~~~~~~~~~~
Command_Utility.c: In function ‘main’:
Command_Utility.c:139:35: warning: passing argument 1 of ‘CyGetListofDevices’ from incompatible pointer type [-Wincompatible-pointer-types]
     rStatus = CyGetListofDevices (&numDevices);
                                   ^
In file included from Command_Utility.c:33:0:
../../common/header/CyUSBSerial.h:705:30: note: expected ‘UINT8 * {aka unsigned char *}’ but argument is of type ‘int *’
 CYWINEXPORT CY_RETURN_STATUS CyGetListofDevices (
                              ^~~~~~~~~~~~~~~~~~
Command_Utility.c:268:51: warning: passing argument 2 of ‘CyReadDeviceConfig’ from incompatible pointer type [-Wincompatible-pointer-types]
                 rStatus=CyReadDeviceConfig(handle,&read_buffer);
                                                   ^
In file included from Command_Utility.c:34:0:
../../common/header/CyUSBBootloader.h:203:45: note: expected ‘UCHAR * {aka unsigned char *}’ but argument is of type ‘unsigned char (*)[512]’
 CYWINEXPORT CY_RETURN_STATUS  WINCALLCONVEN CyReadDeviceConfig (
                                             ^~~~~~~~~~~~~~~~~~
Command_Utility.c: At top level:
Command_Utility.c:382:6: warning: conflicting types for ‘printListOfDevices’
 void printListOfDevices (bool isPrint)
      ^~~~~~~~~~~~~~~~~~
Command_Utility.c:107:5: note: previous implicit declaration of ‘printListOfDevices’ was here
     printListOfDevices (false);
     ^~~~~~~~~~~~~~~~~~
Command_Utility.c: In function ‘printListOfDevices’:
Command_Utility.c:396:25: warning: passing argument 1 of ‘CyGetListofDevices’ from incompatible pointer type [-Wincompatible-pointer-types]
     CyGetListofDevices (&numDevices);
                         ^
In file included from Command_Utility.c:33:0:
../../common/header/CyUSBSerial.h:705:30: note: expected ‘UINT8 * {aka unsigned char *}’ but argument is of type ‘int *’
 CYWINEXPORT CY_RETURN_STATUS CyGetListofDevices (
                              ^~~~~~~~~~~~~~~~~~
Command_Utility.c:445:75: warning: backslash and newline separated by space
               printf ("%d             |%x  |%x    | %d     | %s\n", \

cp 90-cyusb.rules /etc/udev/rules.d
cp CyUSBSerialCommandUtility /usr/bin/cy-config
cp CyUSBSerial.sh /usr/bin
chmod 777 /usr/bin/CyUSBSerial.sh
    DONE!



The TinI/O installation is completed.

```

If a part of the installation process fails, the script will abort the installation and print out: `Error! The last action autobuild tried to perform failed.` In such case, try also the manual installation described below. If your installation completes succesfully, proceed to the next chapter.

## The hard way

In case the automated installation fails, or if you specifically want to build TinI/O manually, you can build TinI/O, the library for it, the flasher tool and the flash files with Makefiles. The order of installation is important: the library must be built first, because TinI/O and the flasher depend on it.

To build the library, go to the `cylib` directory and run `make`. The output will contain some warnings, which should be ignored. The output should look like this:
  ```
  make
  gcc -fPIC -g -Wall -o libcyusb.o -c cyusb.c -I ../../common/header
  cyusb.c: In function ‘CyOpen’:
  cyusb.c:556:1: warning: ‘rStatus’ may be used uninitialized in this function [-Wmaybe-uninitialized]
  printf("rstatus6 %d", rStatus);
  ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  gcc -fPIC -g -Wall -o libcyuart.o -c cyuart.c -I ../../common/header
  gcc -fPIC -g -Wall -o libcyi2c.o -c cyi2c.c -I ../../common/header
  gcc -fPIC -g -Wall -o libcyspi.o -c cyspi.c -I ../../common/header
  gcc -fPIC -g -Wall -o libcyphdc.o -c cyphdc.c -I ../../common/header
  gcc -fPIC -g -Wall -o libcyjtag.o -c cyjtag.c -I ../../common/header
  gcc -fPIC -g -Wall -o libcymisc.o -c cymisc.c -I ../../common/header
  gcc -fPIC -g -Wall -o libcyboot.o -c cyboot.c -I ../../common/header
  cyboot.c:72:0: warning: "CY_USB_SERIAL_TIMEOUT" redefined
  #define CY_USB_SERIAL_TIMEOUT 0

  In file included from cyboot.c:1:0:
  CyUSBCommon.h:74:0: note: this is the location of the previous definition
  #define CY_USB_SERIAL_TIMEOUT 5000

  cyboot.c: In function ‘CyFlashConfigEnable’:
  cyboot.c:634:21: warning: unused variable ‘ioTimeout’ [-Wunused-variable]
      UINT32 rStatus, ioTimeout = CY_USB_SERIAL_TIMEOUT;
                      ^~~~~~~~~
  gcc -shared -g -Wl,-soname,libcyusbserial.so -o libcyusbserial.so.1 libcyusb.o libcyuart.o libcyi2c.o libcyspi.o libcyphdc.o libcyjtag.o libcymisc.o libcyboot.o -l usb-1.0
  cp libcyusbserial.so.1 /usr/local/lib
  ln -sf /usr/local/lib/libcyusbserial.so.1 /usr/local/lib/libcyusbserial.so
  ldconfig
  rm -f libcyusb.o libcyuart.o libcyspi.o libcyi2c.o libcyphdc.o libcyjtag.o libcymisc.o libcyboot.o
  ```

If the library builds succesfully, build the flasher tool next by going into the `tool` directory in the project root and running `make`. Again, the output will contain some warnings and notes that are tobe ignored.
The output should look like this:

```
make
#	gcc -g -o CyUSBSerialTestUtility Test_Utility.c -lcyusbserial -w
gcc -g -o CyUSBSerialCommandUtility Command_Utility.c -lcyusbserial
Command_Utility.c: In function ‘deviceHotPlug’:
Command_Utility.c:102:35: warning: passing argument 1 of ‘CyGetListofDevices’ from incompatible pointer type [-Wincompatible-pointer-types]
     rStatus = CyGetListofDevices (&numDevices);
                                   ^
In file included from Command_Utility.c:33:0:
../../common/header/CyUSBSerial.h:705:30: note: expected ‘UINT8 * {aka unsigned char *}’ but argument is of type ‘int *’
 CYWINEXPORT CY_RETURN_STATUS CyGetListofDevices (
                              ^~~~~~~~~~~~~~~~~~
Command_Utility.c:105:16: warning: ‘return’ with a value, in function returning void
         return rStatus;
                ^~~~~~~
Command_Utility.c:95:6: note: declared here
 void deviceHotPlug () {
      ^~~~~~~~~~~~~
Command_Utility.c:107:5: warning: implicit declaration of function ‘printListOfDevices’ [-Wimplicit-function-declaration]
     printListOfDevices (false);
     ^~~~~~~~~~~~~~~~~~
Command_Utility.c: In function ‘main’:
Command_Utility.c:139:35: warning: passing argument 1 of ‘CyGetListofDevices’ from incompatible pointer type [-Wincompatible-pointer-types]
     rStatus = CyGetListofDevices (&numDevices);
                                   ^
In file included from Command_Utility.c:33:0:
../../common/header/CyUSBSerial.h:705:30: note: expected ‘UINT8 * {aka unsigned char *}’ but argument is of type ‘int *’
 CYWINEXPORT CY_RETURN_STATUS CyGetListofDevices (
                              ^~~~~~~~~~~~~~~~~~
Command_Utility.c:268:51: warning: passing argument 2 of ‘CyReadDeviceConfig’ from incompatible pointer type [-Wincompatible-pointer-types]
                 rStatus=CyReadDeviceConfig(handle,&read_buffer);
                                                   ^
In file included from Command_Utility.c:34:0:
../../common/header/CyUSBBootloader.h:203:45: note: expected ‘UCHAR * {aka unsigned char *}’ but argument is of type ‘unsigned char (*)[512]’
 CYWINEXPORT CY_RETURN_STATUS  WINCALLCONVEN CyReadDeviceConfig (
                                             ^~~~~~~~~~~~~~~~~~
Command_Utility.c: At top level:
Command_Utility.c:382:6: warning: conflicting types for ‘printListOfDevices’
 void printListOfDevices (bool isPrint)
      ^~~~~~~~~~~~~~~~~~
Command_Utility.c:107:5: note: previous implicit declaration of ‘printListOfDevices’ was here
     printListOfDevices (false);
     ^~~~~~~~~~~~~~~~~~
Command_Utility.c: In function ‘printListOfDevices’:
Command_Utility.c:396:25: warning: passing argument 1 of ‘CyGetListofDevices’ from incompatible pointer type [-Wincompatible-pointer-types]
     CyGetListofDevices (&numDevices);
                         ^
In file included from Command_Utility.c:33:0:
../../common/header/CyUSBSerial.h:705:30: note: expected ‘UINT8 * {aka unsigned char *}’ but argument is of type ‘int *’
 CYWINEXPORT CY_RETURN_STATUS CyGetListofDevices (
                              ^~~~~~~~~~~~~~~~~~
Command_Utility.c:445:75: warning: backslash and newline separated by space
               printf ("%d             |%x  |%x    | %d     | %s\n", \

cp 90-cyusb.rules /etc/udev/rules.d
cp CyUSBSerialCommandUtility /usr/bin/cy-config
cp CyUSBSerial.sh /usr/bin
chmod 777 /usr/bin/CyUSBSerial.sh
```

After the succesful installation of the flasher tool, install TinI/O by going into the `tinio` directory in the project root and running `make && make install`. The output should look like this:

```
make && make install
echo "Building tinio..."
Building tinio...
g++ -lcyusbserial tinio.cpp -o tinio
echo "done."
done.
echo "Installing tinio..."
Installing tinio...
mkdir -p /usr/share/tinio
cp -r flash /usr/share/tinio
cp tinio /usr/bin
echo "done."
done.
```

If TinI/O installs succesfully, the installation is completed. Proceed to the next chapter.

# Flashing the board
Before you use your board with TinI/O, you must program its chip with a custom flash that will make the board compatible with TinI/O. The flash files are located in the `/usr/share/tinio/flash` directory and can be flashed to the board with the installed `cy-config` utility. There are 2 flash files with slightly different pin configurations: `5-5.cyusbd` and  `3-3cs.cyusbd`. The 5-5 file sets half (5) of the GPIO pins as outputs and the other half as inputs. The 3-3cs sets 3 of the pins as outputs and 4 as inputs and 1 pin as a CapSense capacitive touch button and 1 as its output. See the table for the exact pin configuration:

| GPIO Pin  |  5-5 config  |  3-3cs config |
|  ---  |  ---  |  --- |
| 0  | Input | RESERVED - decouple to ground with a 2n2 capacitor |
| 1 | Input | Input |
| 2 | Input | Capsense button - connect to a metal button surface |
| 3 | Input | Capsense out - connect to an input to read |
| 4 | Input | Input|
| 5 | UART Tx - Don't connect | UART Tx - Don't connect |
| 6 | UART Rx - Don't connect | UART Rx - Don't connect |
| 7 | Output | Input |
| 8 | Output | Output |
| 9 | Output | Output |
| 10 | Output | Output |
| 11 |  Output | Input |

To flash a configuration file
