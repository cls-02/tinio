# TinI/O

is a tool for interfacing the Cypress CY7C65211 USB-Serial Bridge Controller and its GPIO.

--------------------------------------------------------------------------------

## 1\. How to build it

To set up TinI/O, you'll need:

- A linux machine (the processor architecture doesn't matter, but Debian or Ubuntu is almost guaranteed to work)
- an USB dongle with the chip
- a C and C++ compiler (GCC recommended)
- GNU Make and CMake
- libusb 1.0

You can build TinI/O by

a)`cd`-ing to the `cylib/build` directory and build the needed libraries with:

```
# cmake ..
# make
# sudo make install
```

Then go to the `../tinio` directory and build and install TinI/O with:

```
# make
# make install
```

or<br>
b) executing the `autobuilder` script in the main directory that will do the above for you.

**NOTE: Although the autobuilder should be working in most use cases, it's still better to build the tools manually (if you have the needed knowledge, of course)**

## 2\. (Re)flashing your device

Before you start using TinI/O, you have to (re)flash the target chip so your PC can recognise it. You can do that by using the supplied `cy-config` utility. To reflash a chip, you'll need to navigate to `/usr/share/tinio/flashes` directory. There you'll find files that contain different flash images.  For a general configuration (5 inputs, 5 outputs), we'll use the file named `5in-5out.cyusbd`. We can flash it by typing

```
# cy-config 5in-5out.cyusbd


---------------------------------------------------------------------------------
Device Number | VID | PID | INTERFACE NUMBER | FUNCTIONALITY
---------------------------------------------------------------------------------
0             |1d6b  |1    | 0     | NA
1             |1d6b  |1    | 0     | NA
2             |1d6b  |1    | 0     | NA
3             |413c  |3016    | 0     | NA
4             |1d6b  |2    | 0     | NA
5             |1d6b  |1    | 0     | NA
6             |4b4  |4    | 0     | NA
6             |4b4  |4    | 1     | NA
7             |1d6b  |1    | 0     | NA
8             |461  |10    | 0     | NA
8             |461  |10    | 1     | NA
9             |1d6b  |2    | 0     | NA
10             |bda  |111    | 0     | NA
---------------------------------------------------------------------------------

Cydevices 11-------------------------------------------------------------------
1: Print list of devices
2: Select device...No device selected !!
```

At this point you have to manually search through the `VID` and `PID` columns for values `4b4`(VID) and `4` or `2`(PID). Those are the values specific to the chip. In this case, there are 2 entries with those numbers:
```
6             |4b4  |4    | 0     | NA
6             |4b4  |4    | 1     | NA
```
If there are no devices with the right IDs, hit Ctrl-C and make sure your device is plugged in. If everything seems fine, memorise the device number (the first column) and the last interface number listed (the 4th column of the last entry). Proceed by selecting the option 2.
```
2
Enter Device number to be selected..

```
Enter the device number that you memorised earlier.
```
6
Enter interface number..
```
Enter the interface number you memorised earlier.
```
1

 File opened successfully

 Bytes successfully read
 Programming Flash is done
 File stream closed
-------------------------------------------------------------------
1: Print list of devices
2: Change device selection--selected device: [Device number 6] : [Interface No 1] : NA


```
At this point, hit Ctrl-C and your device is programmed and can be used with TinI/O.

## 3. Using `tinio`
A short summary of TinI/O's options can be found by executing `tinio` without any options:
```
TinI/0 0.1
Usage:
tinio <options>
The supported options are:
-d<device number> - specifies the desired device - integer 0 to 15
-i<interface number - specifies the USB interface number - integer 0 to 255
-s<pin number> - sets the specified pin to the value specified with -v  - integer 0 to 11
-r<pin number> - reads the specified pin's value and prints it to the stdout - integer 0 to 11
-v<value> - value for -s option - integer 0 to 1
-e - enables expert mode (enables restricted pins)
```
**NOTE: the -d and -i options are optional and are only to be used when using more and/or custom programmed devices.**
The options are pretty self-explanatory, so you can easily start playing with them. For example:
`tinio -s8 -v1` sets the pin 8 on and
`tinio -s8 -v0` sets it off.
Similiarly,
`tinio -r8` reads the pin 8's value and outputs it to the stdout.
## That's all!
That's all you need to know to get started with TinI/O, for more info read the manual and look at some tutorials at TODO
