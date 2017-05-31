# TinI/O manual

--------------------------------------------------------------------------------

# 1\. Introduction to TinI/O

## 1.1 What is TinI/O?

TinI/O (or tinio) is a program that makes Linux computers with USB GPIO-capable and expands the existing capabilities on GPIO-enabled computers. It does that with its hardware counterpart, [the Cypress CY7C65211 USB-Serial chip][chip].

## 1.2 What features does TinI/O have?

- It can make any linux computer GPIO enabled
- It runs as a standalone program that can be easily integrated into other programs via system() calls
- It consumes almost none of the host computer's resources, because it never runs in the background
- It's compatible with almost every Linux system that has USB, including the embedded and older ones
- It operates over USB 1.1 that make TinI/O compatible even with the oldest of USB-enabled computers

## 1.3 How does it work?

TinI/O can't provide GPIO by itself. Its capabilites rely on its chip companion ([CY7C65211][chip]), libusb and cyrozap's libcyusbserial library. When TinI/O is ran, it processes its arguments and sends commands to the chip via the before mentioned libraries. The chip then changes its GPIO pins' states accordingly.

## 1.4 Under what license can it be used?

TinI/O is licensed under the GNU General Public License, which is publicly available at [<http://www.gnu.org/licenses/gpl.html>] and in the COPYING file in the `docs` directory in the project's root.

# 2\. Installing

## 2.1 What you should know

TinI/O binaries aren't available yet, but that isn't a problem, because TinI/O is very easy to build, even for people not familiar with program building procedures.

TinI/O requires some quite basic system requirements, so before you proceed, make sure that you have:

- a working C and C++ compiler (**a working C++ is a working C compiler, but not the other way round!**)
- installed libusb
- GNU Make and cmake

## 2.2 Manual build

_This chapter explains how to build TinI/O manually. If you don't feel comfortable or don't want to build it manually, skip this chapter._ The TinI/O build process is pretty common. It's automated with a makefile, common to most *nix build processes. It has 4 build targets: | **Target** | **Description** | |:----------:|:----------------| | _default_ | The default target. Calls _utils_ and _tinio_ |

### 2.2.1 Building the Library

To communicate with the chip, TinI/O needs [a dedicated library written by cyrozap][cylib] that comes with TinI/O but needs to be built separately. To do that, run `cmake .. && make && sudo make install` inside the `cylib/build` directory in the project root.

### 2.2.2 Building everything else

To complete the installation process, run `make && make install` in the `tinio` directory in the project root. That will build and install the flasher utility and TinI/O itself.

## 2.1 The `autobuilder`

Because some people don't like manual binary building, TinI/O comes with the `autobuilder` shell script, which allows non-experienced users to install TinI/O without knowing much about GNU Make, GCC and other tools that are used to build it. It's very minimalistic: the only thing it does is following the manual build process described in the previous chapter and checks forany errors at the end of each step.

The `autobuilder`-automated build process is simple. You just need to run the script, which will do a typical TinI/O instalation.

# 3\. After the installation

After the installation of TinI/O, you actually don't need to do anything at all with _it_. The next step is to prepare the actual device you'll be using TinI/O with.

## 3.1 Flashing the chips

Cypress CY7C65211 can be flashed only from Windows with a dedicated Cypress utility due to its unique flash file format. Fortunately, after some bargaining with Cypress I got a Linux utility that is licensed under LGPL and free (as speech _and_ beer). It can flash special, decoded versions of flash files that can be produced with a special Windows decoder executable that I can't provide with TinI/O because it's not GPL'd (actually it's not even licensed!) and isn't released to the public (yet). Instead the TinI/O package includes 2 already decoded flash files that should satisfy the needs of a typical user. Their names are:

- `5-5_decoded.cyusbd`, that provides 5 input and 5 output ports
- `3-3cs_decoded.cyusbd`, that provides 3 input and 3 output ports, plus a CapSense button with its dedicated input.

## 3.2 A little more detail on the flash files

_Here is a detailed description of the flash files configurations for the curious and other developers. It contains a lot of technical details, that aren't particularly useful in a typical TinI/O use case. A stripped-down version of the table below can be found at the end of the previous chapter "3.1 Flashing the chips"_

TODO Make a big ol' able with them config's

## 3.3 Flashing with the `cy-config` utility

[chip]: http://www.cypress.com/part/cy7c65211-24ltxi
[cylib]: http://github.com/cyrozap/libcyusbserial
