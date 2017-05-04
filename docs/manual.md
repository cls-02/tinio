# TinI/O manual
-----
# 1. Introduction to TinI/O
## 1.1 What is TinI/O?
TinI/O (or tinio) is a Linux program that makes standard PCs with USB GPIO-capable and/or expands the existing capabilities on GPIO-enabled computers. It does that by talking to its hardware counterpart, [the Cypress CY7C65211 USB-Serial chip][1].

## 1.2 What features does TinI/O have?
+ It's written entirely in C and C++, making it platform-independent and fast
+ It's easy on the CPU (nothing runs in the background)
+ It's compatible with almost every Linux system
+ It operates over 1st gen USB 1.1, making it useful with older machines

## 1.3 How does it work?
TinI/O can't provide GPIO by itself. Its capabilites rely on its chip companion ([CY7C65211][chip]), libusb and cyrozap's libcyusbserial library. When TinI/O is ran, it processes its arguments and sends commands to the chip via the before mentioned libraries. The chip then takes care of the rest.

## 1.4 Under what license can it be used?
TinI/O is licensed under the GNU General Public License, which is publicly available at http://www.gnu.org/licenses/gpl.html and in the COPYING file in the `docs ` directory in the project's root.


# 2. Installing
## 2.1 What you should know
TinI/O binaries aren't available right now, but that shouldn't be a problem, because TinI/O is easy to build.
Before you proceed, make sure that you have:
+ at least 16 MB of disk space
+ a working C and C++ compiler (**a working C++ is a working C compiler, but not the other way round!**)
+ installed libusb
+ GNU Make and cmake

## 2.2 Manual build
*This chapter explains how to build TinI/O manually. If you don't feel comfortable or don't want to build it manually, skip this chapter.*


The TinI/O build process consists of:
1. Building the cyusbserial library and
2. Building TinI/O and the firmware flasher.


### 2.2.1 Building the Library
To communicate with the chip, TinI/O needs [a dedicated library written by cyrozap][cylib] that comes with TinI/O but needs to be built separately. To do that, run `cmake .. && make && sudo make install` inside the `cylib/build` directory in the project root.

### 2.2.2 Building everything else
To complete the installation process, run `make && make install` in the `tinio` directory in the project root. That will build and install the flasher utility and TinI/O itself.

### 2.2.3 More about the makefile
TODO explain the makefile's targets etc.

## 2.1 The `autobuilder`
Because some people don't like manual binary building, TinI/O comes with the `autobuilder` script, which allows non-experienced users to install TinI/O without knowing much about GNU Make, GCC and other tools that are used to build it.

To build TinI/O with `autobuilder`:
- Just run the autobuilder script in the project root in a terminal!
- The script will then guide you through the installation process.

# 3. After the installation
After the installation of TinI/O, you actually don't need to do anything at all with *it*. The next step is to prepare the actual device you'll be using TinI/O with.

# 4. Flashing the chip(s)
Cypress CY7C65211 can be flashed from Windows or from Linux, but only with a dedicated Cypress utility due to its unique flash file format. Fortunately, the utility is licensed under LGPL and free (as speech *and* beer). The utility is named `cy-config` and is text-based.
There are also 2 premade flash files that come with the utility:
- `5in-5out.cyusbd` - 5 inputs and 5 outputs
- `3in-3out-cs.cyusbd` - 3 inputs and 3 outputs + a capsense button

[chip]: http://www.cypress.com/part/cy7c65211-24ltxi
[cylib]: http://github.com/cyrozap/libcyusbserial
