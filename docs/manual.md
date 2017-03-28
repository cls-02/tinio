# TinI/O

is a tool for interfacing the Cypress CY7C65211 USB-Serial Bridge Controller and its GPIO.

--------------------------------------------------------------------------------

## 1\. How to build it

To set up TinI/O, you'll need:

- A linux machine (the processor architecture doesn't matter, but Debian or Ubuntu is almost guaranteed to work)
- an USB dongle with the chip
- a C and C++ compiler (GCC is most common)
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

## 2\. Getting started
Before you start using TinI/O, you have to reflash the target chip so it can be recognised by your PC. That is done with the supplied `cy-config` utility.
To reflash a chip, you'll need to navigate to `/usr/share/tinio/flashes` directory. There you'll find files that contain different flash images. (I'll say more about the images later in the chapter TODO)
For a general configuration (5 inputs, 5 outputs), we'll use TODO
You can get a quick summary on TinI/O by executing it.

```
# ./tinio
TinI/0 0.1
Usage:
tinio <options>
The supported options are:
-d<device number> - specifies the desired device - integer 0 to 15
-i<interface number - specifies the USB interface number - integer 0 to 255
-s<pin number> - sets the specified pin to the value specified with -v  - integer 0 to 11
-r<pin number> - reads the specified pin's value and prints it to the stdout - integer 0 to 11
-v<value> - value for -s option - integer 0 to 1
-e - enables expert mode (enables protected pins)
#
#
```
