# TinI/O manual

--------------------------------------------------------------------------------

# 1. Introduction to TinI/O

## 1.1 What is TinI/O?

TinI/O (or tinio) is a program that makes Linux computers with USB GPIO-capable and expands the existing capabilities on GPIO-enabled computers. It does that with its hardware counterpart, [the Cypress CY7C65211 USB-Serial chip][chip].

## 1.2 What features does TinI/O have?

-   It can make any Linux computer GPIO enabled
-   It's tremendously easy to install and use
-   It runs as a standalone program that can be easily integrated into other programs via system() calls
-   It consumes almost none of the host computer's resources, because it never runs in the background
-   It's compatible with almost every Linux system that has USB, including the embedded and older ones
-   It operates over USB 1.1, which makes TinI/O compatible even with the oldest of USB-enabled computers

## 1.3 How does it work?

TinI/O can't provide GPIO by itself. Its capabilites rely on its chip companion, the Cypress ([CY7C65211][chip]), libusb and cyrozap's libcyusbserial library. When TinI/O is ran, it processes its arguments and sends commands to the chip with those libraries. The chip then changes its GPIO pins' states accordingly. However, there are some hardware limitations to that. The GPIO is digital-only, with no PWM, interrupts or ADCs. It's designed to be simple to minimize the amount of experience and knowledge needed, to straighten learning curves and not to be another conglomerate of libraries and C-like languages that some MCU platforms have become over the last decade.
## 1.4 Under what license can I use it?

TinI/O is licensed under the GNU General Public License, which is publicly available at <http://www.gnu.org/licenses/gpl.html> and in the COPYING file in the `docs` directory in the project's root.
The library it depends on is licensed under LGPL. Refer to the `cylib` subfolder fore more info.
# 2. Installing

## 2.1 What you should know

TinI/O binaries aren't available yet, but that shouldn't pose a problem, because TinI/O is very easy to build, even for people not familiar with program building procedures.

TinI/O requires some quite basic system requirements, so before you proceed, make sure that you have:

-   a working C and C++ compiler (**a working C++ is a working C compiler, but not the other way round!**)
-   installed libusb
-   GNU Make and cmake

## 2.2 Manual build

_This chapter explains how to build TinI/O manually. If you don't feel comfortable or don't want to build it manually, skip this chapter._

The TinI/O build process is pretty common. It's automated with a makefile, common to most *nix build processes. It has 4 build targets:

 |  **Target**  |  **Description**  |
 |  ----------- |  ---------------- |
 |  _default_  |  The default target. Calls _utils_ and _tinio_ targets.  |
 |  _tinio_    |  Builds main TinI/O program.  |
 |  _utils_    |  Builds the flasher utility.  |
 |  _install_  |  Installs TinI/O and the flasher utility to /usr/bin and the flash files to /usr/bin/tinio/flashes  |

### 2.2.1 Building the Library

To communicate with the chip, TinI/O needs [a dedicated library written by cyrozap][cylib] that comes with TinI/O but needs to be built separately. To do that, run `cmake .. && make && sudo make install` inside the `cylib/build` directory in the project root.

### 2.2.2 Building everything else

To complete the installation process, run `make && make install` in the `tinio` directory in the project root. That will build and install the flasher utility and TinI/O itself.

## 2.1 The `autobuilder`

Because some people don't like manual binary building, TinI/O comes with the `autobuilder` shell script, which allows non-experienced users to install TinI/O without knowing much about GNU Make, GCC and other tools that are used to build it. It's very minimalistic: the only thing it does is following the manual build process described in the previous chapter and checks forany errors at the end of each step.

The `autobuilder`-automated build process is simple. You just need to run the script, which will do a typical TinI/O instalation.

# 3. After the installation

After the installation of TinI/O, you actually don't need to do anything at all with _it_. The next step is to prepare the actual device you'll be using TinI/O with.

## 3.1 Flashing the chips


Cypress CY7C65211 can be flashed only from Windows with a dedicated Cypress utility due to its unique flash file format. Fortunately, after some bargaining with Cypress I got a Linux utility that is licensed under LGPL and free (as speech _and_ beer). It can flash special, decoded versions of flash files that can be produced with a special Windows decoder executable that I can't provide with TinI/O because it's not GPL'd (actually it's not even licensed!) and isn't released to the public (yet). Instead the TinI/O package includes 2 already decoded flash files that should satisfy the needs of a typical user. Their names are:

-   `5-5_decoded.cyusbd`, that provides 5 input and 5 output ports
-   `3-3cs_decoded.cyusbd`, that provides 3 input and 3 output ports, plus a CapSense button with its dedicated input.

And they provide the following configurations:
_Table 3.1.1: Flash files configuration_

 |  parameter  |  value in `5-5_decoded.cyusbd`  |  value in `3-3cs_decoded.cyusbd` |
 | ----------- | ----------- | ------------ |
 |  **USB**  |   |   |
 |  Max. current drawn from USB  |  350 mA  |  350 mA  |
 |  USB device class  |  PHDC  |  PHDC  |
 |  USB ID (vid:pid)  |  0x04b4:0004 (Cypress)  |  0x04b4:0004 (Cypress)  |
 |  Manufacturer string  |  TinI/O  |  TinI/O  |
 |  Product string  |  5/5  |  3/3CS  |
 |  **Serial**  |   |   |
 |  UART Tx/Rx pins  |  5(Tx)/6(Rx)  |  5/6  |
 |  UART Speed  |  9600 baud  |  9600 baud  |
 |  UART Data/Stop bits  |  8/1  |  8/1  |
 |  UART Parity  |  None  |  None  |
 |  **I/O**  |   |   |
 |  I/O logic level  |  LVTTL  |  LVTTL  |
 |  Pin 0  |  Input  |  **RESERVED for CapSense** Should be decoupled to ground by a 2n2 capacitor  |
 |  Pin 1  |  Input  |  Input  |
 |  Pin 2  |  Input  |  **RESERVED for CapSense** CapSense sense (connect to your button)  |
 |  Pin 3  |  Input  |  **RESERVED for CapSense** CapSense output (the output of your button)  |
 |  Pin 4  |  Input  |  Input  |
 |  Pin 5  |  **RESERVED** UART Tx  |  UART Tx  |
 |  Pin 6  |  **RESERVED** UART Rx  |  UART Rx  |
 |  Pin 7  |  Output  |  Input  |
 |  Pin 8  |  Output  |  Output  |
 |  Pin 9  |  Output  |  Output  |
 |  Pin 10  |  Output  |  Output  |
 |  Pin 11  |  Output  |  Input  |

_Note: Not all of the information here is neccesary to know when using TinI/O.
For a stripped-down version refer to the next chapter, "Using TinI/O"_

Please notice that flashing of these files isn't the only way of configuring chips. They can be also customized with the Windows utility. When configuring chips with it, you should pay attention to keeping USB settings as described in the table, as TinI/O can't communicate with unproperly configured devices.

## 3.2 The `cy-config` utility
The flash files I covered in the previous chapter can be flashed with the `cy-config` utility that installs with TinI/O. The utility is text-based. It can be used by running `cy-config`. Upon execution, it prints out a list of USB devices
```
#cy-config <path to the decrypted flash file>


---------------------------------------------------------------------------------
Device Number  |  VID  |  PID  |  INTERFACE NUMBER  |  FUNCTIONALITY
---------------------------------------------------------------------------------
0              | 1d6b   | 1     |  0      |  NA
1              | 8bb   | 2902     |  0      |  NA
1              | 8bb   | 2902     |  1      |  NA
1              | 8bb   | 2902     |  2      |  NA
1              | 8bb   | 2902     |  3      |  NA
2              | 17e9   | 101     |  0      |  NA
3              | 1d6b   | 1     |  0      |  NA
4              | 424   | 2504     |  0      |  NA
5              | 1d6b   | 1     |  0      |  NA
6              | 413c   | 3016     |  0      |  NA
7              | 1d6b   | 2     |  0      |  NA
8              | 1d6b   | 1     |  0      |  NA
9              | 4b4   | 4     |  0      |  NA
9              | 4b4   | 4     |  1      |  NA
10              | 1d6b   | 1     |  0      |  NA
11              | 461   | 10     |  0      |  NA
11              | 461   | 10     |  1      |  NA
12              | d8c   | c     |  0      |  NA
12              | d8c   | c     |  1      |  NA
12              | d8c   | c     |  2      |  NA
12              | d8c   | c     |  3      |  NA
13              | 1d6b   | 2     |  0      |  NA
14              | bda   | 111     |  0      |  NA
---------------------------------------------------------------------------------

Cydevices 15-------------------------------------------------------------------
1: Print list of devices
2: Select device...No device selected !!
```
At this point, you can either choose to rescan (1) or to choose a device to program (2). The rescan may help detecting new devices, but the second option should be selected for (re)programing the chip. Upon selecting it, it requests a device number:
```
2
Enter Device number to be selected..

```
At this point should look at the device printout and search for a device with a Cypress VID (`4b4`, refer to the table 3.1.1) and enter its number (in this case 9). After that, the utility will ask for the interface number to use:
```
9
Enter interface number..

```
You should enter the last interface number listed in the printout, for devices already programmed for use with TinI/O this would usually be 1, for factory programmed devices from Cypress it's usually 2, but those predictions aren't always right. In this case, it's 1 because the device is already programmed for TinI/O.
```
1

 File opened successfully

 Bytes successfully read
 Programming Flash is done
 File stream closed
-------------------------------------------------------------------
1: Print list of devices
2: Change device selection--selected device: [Device number 9] : [Interface No 1] : NA
```
Such output indicates that the chip was succesfully programmed and the utility can be exited (Ctrl-C).

# 4.Using TinI/O
## 4.1 The hardware
There is no dedicated TinI/O board out there, instead, the ([Cypress CY8CKIT-049][cykit]) is used. Its smaller part provides you with 12 GPIO pins, which can be configured in those two ways as described in the previous chapter:

|  Pin  |  5in/5out config  |  3in/3out config w/Capsense  |
|  ---  |  ---  |  --- |
| 0  | Input | RESERVED decouple to ground with a 2n2 capacitor |
| 1 | Input | Input |
| 2 | Input | Capsense button |
| 3 | Input | Capsense out |
| 4 | Input | Input|
| 5 | UART Tx | |
| 6 | UART Rx | |
| 7 | Output | Input |
| 8 | Output | Output |
| 9 | Output | Output |
| 10 | Output | Output |
| 11 |  Output | Input |


## 4.2 The software
TinI/O isn't particularly difficult to use, because it's very simple in its design (Like, common guys, how complex and obfuscated can a code written by a 14-years-old kid be?). The bare minimum that you need to know to use TinI/O is only its name. Example:
```
#tinio
TinI/0 0.1
Usage:
tinio <options>
The supported options are:
-d<device number> - specifies the desired device - integer 0 to 15
-i<interface number - specifies the USB interface number - integer 0 to 255
-s<pin number> - sets the specified pin to the value specified with -v  - integer 0 to 11
-r<pin number> - reads the specified pin's value and prints it to the stdout - integer 0 to 11
-v<value> - value for -s option - integer 0 to 1
-e - enables expert mode (enables additional pins)
```
As you can clearly see, there isn't a lot to memorize, but if that's still too much for you, memorize the `-s`,`-r` and the `-v` option that are used to read and write to the chip's GPIO.
A few examples:
`tinio -r8` -Reads the state of the pin 8 and outputs it to the stdout
`tinio -s7 -v1` -Turns the 7th pin on

The other options that you may use are `-d`, `-i` and `-e`. The `-d` option can be used when dealing with more than one connected device to select the one you want to work on. It's also important to know that there is absolutely no assurance that the devices will stay in order, which is the reason why wouldn't recommend using this option.
The `-i` option selects the USB interface number and doesn't need to be used in a typical used case as it defaults to 0, which is tested to work with chips flashed with the provided flash files.
The `-e` option is used to enable expert mode. Expert mode allows tampering with already-occupied pins, for example the UART transciever pins (5 and 6). It's never needed in a typical use case.
#5. Legal stuff
Windows is a registered trademark of Microsoft
Linux is a registered trademark of the Linux Foundation
Cypress is a registered trademark of Cypress
All trademarks are property of their rightful owners. This document does not intend to violate them. If I accidentaly do that, please email me at thecodingkid.devel@gmail.com and I will do my best to fix my mistakes.
This document is part of TinI/O, and is licensed under GNU GPL. Fell free to distribute!
Oh, and F*ck expensive proprietary software (unless it's a dope game)!
[chip]: http://www.cypress.com/part/cy7c65211-24ltxi
[cylib]: http://github.com/cyrozap/libcyusbserial
[cykit]: http://www.cypress.com/documentation/development-kitsboards/psoc-4-cy8ckit-049-4xxx-prototyping-kits
