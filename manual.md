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
  + libusb 1.0 and its headers installed
  + 16 MB of free space (or 8 for the bare minimum)
  + A C and C++ compiler (preferably gcc and g++)


- A TinI/O compatible board

  They can be obtained from Cypress, the manufacturer of the chip. The board I personally use and recommend is the [CY8CKIT-049-4xxx PSoC4 prototyping kit](http://www.cypress.com/documentation/development-kitsboards/psoc-4-cy8ckit-049-4xxx-prototyping-kits), but there are more of them out there, for example, the [CYUSBS234 USB-Serial (Single Channel) Development Kit](http://www.cypress.com/documentation/development-kitsboards/cyusbs234-usb-serial-single-channel-development-kit).

# Building and installing TinI/O
Installing TinI/O is very easy - just `cd` inside the `tinio` directory and run `make`. When the installation completes, run `make install`. With everything installed, proceed to the next chapter.

# Flashing the board
Before you use your board with TinI/O, you must program its chip with a custom flash file that will make the board compatible with TinI/O. The flash files are located in the `/usr/share/tinio/flash` directory and can be flashed to the board with the installed `cy-config` utility. There are 2 flash files with slightly different pin configurations: `5-5.cyusbd` and  `3-3cs.cyusbd`. The 5-5 file sets half (5) of the GPIO pins as outputs and the other half as inputs. The 3-3cs sets 3 of the pins as outputs and 4 as inputs and 1 pin as a CapSense capacitive touch button and 1 as its output. See the table for the exact pin configuration:

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

After you've decided which configuration you'll use, run the configuration utility with the path of the flash file as an argument , e.g.:`cy-config /usr/share/tinio/flash/5-5.cyusbd`.
A menu will appear:
```

---------------------------------------------------------------------------------
Device Number | VID | PID | INTERFACE NUMBER | FUNCTIONALITY
---------------------------------------------------------------------------------
0             |1d6b  |2    | 0     | NA
1             |413c  |3016    | 0     | NA
2             |461  |10    | 0     | NA
2             |461  |10    | 1     | NA
3             |1d6b  |1    | 0     | NA
4             |8bb  |2902    | 0     | NA
4             |8bb  |2902    | 1     | NA
4             |8bb  |2902    | 2     | NA
4             |8bb  |2902    | 3     | NA
5             |4b4  |4    | 0     | NA
5             |4b4  |4    | 1     | NA
6             |1d6b  |1    | 0     | NA
7             |1d6b  |1    | 0     | NA
8             |1d6b  |2    | 0     | NA
9             |1d6b  |1    | 0     | NA
10             |1d6b  |1    | 0     | NA
---------------------------------------------------------------------------------

Cydevices 11-------------------------------------------------------------------
1: Print list of devices
2: Select device...No device selected !!
```
At this point, you want to look for the device `4b4` in the VID column. Find its device number. In the example above, the device we're looking for has the device number 5. Select the `Select Device` option in the menu by entering `2`.
```
2
Enter Device number to be selected..
```
Enter the device number you remembered in the previous step.
```
5
Enter interface number..
```
Enter __the device's highest interface number in the table__. In the example it's 1, but it can also be 2.

```
1

 File opened successfully

 Bytes successfully read
 Programming Flash is done
 File stream closed
-------------------------------------------------------------------
1: Print list of devices
2: Change device selection--selected device: [Device number 5] : [Interface No 1] : NA
^C
```
This is how a successful flashing output should look like. If it doesn't:
+ If it gives you a fopen() error, the file you tried to flash doesn't exist or (rarely happens, but still) has a too long path. Find the file or move it to a less obscure folder.
+ Make sure you used a right device and file
+ Try running the utility as root

## Creating your own configs
If you want to program your device your own way, you can do it with Cypress' [USB-Serial Configuration Utility](http://www.cypress.com/documentation/software-and-drivers/usb-serial-software-development-kit). However, you must follow some guidelines to make your configuration TinI/O compatible.

The device must be configured to:
+ PHDC Interface
+ UID and VID left default
+ SCB: UART 2-pin

# Using TinI/O
TinI/O is pretty simple to use. To see a quick usage reminder, run `tinio` in the terminal, but here's a more extensive
## Usage

| __Option__ | __Action__ |
|------------|------------|
| -d<num> | Sets the device number. 0 if left out. Up to 15 devices. |
| -i<num> | Sets the interface number. 0 if left out. 0-255. |
| -s<pin> | Sets the specified pin's value to the value specified with the -v option. 0-11. |
| -r<pin> | Reads the pin's value and outputs it to the stdout. 0-11. |
| -v<val> | Specifies the output value. Either 0 or 1, other numbers default to 1. |
| -e      | Enables expert mode. The expert mode enables you to write to reserved pins (5&6) and shouldn't be enabled unless you know what you're doing. |

## Practical examples
The two basic functions of TinI/O are reading and writing to I/O on a single device:  
- To read, run `tinio -r<pin>` with <pin> replaced with the desired pin. The result will be printed to the stdout, followed by a newline.
- To set, run `tinio -s<pin> -v<val>` with <pin> replaced with the desired pin and <val> replaced with the value you want to write.

TinI/O is capable of handling up to 16 devices at once with the `-d` option, but there isn't a reliable way of identifying them. The best way to ensure the correct device order is to look at the device tree with `lsusb` and check which device comes first - it will have the first device number (0).

- To read from a specific device number, run `tinio -r<pin> -d<dev>` and replace <pin> with the pin number and <dev> with the device number.
- To write to a specific device run `tinio -s<pin> -v<val> -d<dev>` and replace <pin> with the pin number, <val> with the value and <dev> with the device number.

## Using TinI/O in your projects
+ As the TinI/O target chip (CY7C65211) isn't a microcontroller, it can't operate without a computer connected to it.
+ It can't do PWM.
+ Its I/O has 3.3V LVTTL logic level, and it's __not__ 5V compatible.
+ Its maximum output current is comparable to Arduino's at 25 mA.
+ When using CapSense, decouple the reserved 0 pin to the ground with a 2.2 nF capacitor and don't use it.

# Copyright, Links, ...
[Cypress' website](http://cypress.com)  
[CY7C65211 Datasheet](http://http://www.cypress.com/file/139886/download)  
[TinI/O Github](http://github.com/kristjan-komlosi/tinio)  
[My Youtube channel - You may expect some videos of TinI/O there](https://www.youtube.com/channel/UCmcIwYA2I2YkmQs7xgMU-DA)

## __Copyright disclaimer__
No copyright infringement was intended to be made with this document or the TinI/O project. All of trademarks stated in this document are property of their owners.

TinI/O is licensed under GNU GPL License.  
This document is licensed under the GNU FDL License.
