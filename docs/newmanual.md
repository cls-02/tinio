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

  They can be obtained from Cypress, the manufacturer of the chip. The board I personally use and recommend is the [CY8CKIT-049-4xxx PSoC4 prototyping kit](http://www.cypress.com/documentation/development-kitsboards/psoc-4-cy8ckit-049-4xxx-prototyping-kits), but there are more of them out there, for example, the [CYUSBS234 USB-Serial (Single Channel) Development Kit](http://www.cypress.com/documentation/development-kitsboards/cyusbs234-usb-serial-single-channel-development-kit), which uses the same chip, or the [CYUSB232 USB-UART LP Reference Design Kit](http://www.cypress.com/documentation/development-kitsboards/cyusbs232-usb-uart-lp-reference-design-kit) and the [CYUSBS236 USB-Serial (Dual Channel) Development Kit](http://www.cypress.com/documentation/development-kitsboards/cyusbs236-usb-serial-dual-channel-development-kit), that have different chips from the same family.

# Building and installing TinI/O

## The easy way
After you make sure that your computer complies with the system requirements (it probably does), you need to build and install TinI/O. The simplest way to do that is to run the `autobuilder` script in the project root (aka the folder you downloaded TinI/O source to) __as root__. The script will then start the build process and redirect any text from its underlying programs to the standard output. A typical output from a succesful installation looks approximately like this:

<details>
<summary>
Long text - Click to expand
</summary>
<pre>
<code>

--------------------------
Autobuilder for TinI/O 0.1
--------------------------
Verifying main directory...
DONE!
Entering the library directory...
DONE!
Compiling and installing the library...
-- The C compiler identification is GNU 6.3.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Found PkgConfig: /usr/bin/pkg-config (found version "0.29")
-- Checking libusb version...
-- libusb version: 1.0.9

-- Looking for pthread.h
-- Looking for pthread.h - found
-- Looking for pthread_create
-- Looking for pthread_create - not found
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - found
-- Found Threads: TRUE
CMake Warning at lib/CMakeLists.txt:86 (message):


libusb >= 1.0.16 is HIGHLY recommended.  If you experience issues or poor
performance, please try updating libusb.

Detected version: 1.0.9



-- Configuring done
-- Generating done
-- Build files have been written to: /root/projekti/CyUSB/cylib/build
Scanning dependencies of target cyusbserial
[ 10%] Building C object lib/CMakeFiles/cyusbserial.dir/cyusb.c.o
[ 20%] Building C object lib/CMakeFiles/cyusbserial.dir/cyuart.c.o
[ 30%] Building C object lib/CMakeFiles/cyusbserial.dir/cyi2c.c.o
[ 40%] Building C object lib/CMakeFiles/cyusbserial.dir/cyspi.c.o
[ 50%] Building C object lib/CMakeFiles/cyusbserial.dir/cyphdc.c.o
[ 60%] Building C object lib/CMakeFiles/cyusbserial.dir/cyjtag.c.o
[ 70%] Building C object lib/CMakeFiles/cyusbserial.dir/cymisc.c.o
[ 80%] Linking C shared library libcyusbserial.so
[ 80%] Built target cyusbserial
Scanning dependencies of target cyusbserialtest
[ 90%] Building C object tools/CMakeFiles/cyusbserialtest.dir/cyusbserialtest.c.o
[100%] Linking C executable cyusbserialtest
[100%] Built target cyusbserialtest
[ 80%] Built target cyusbserial
[100%] Built target cyusbserialtest
Install the project...
-- Install configuration: ""
-- Installing: /usr/local/include/CyUSBSerial.h
-- Installing: /usr/local/lib/libcyusbserial.so.1
-- Installing: /usr/local/lib/libcyusbserial.so
-- Set runtime path of "/usr/local/lib/libcyusbserial.so.1" to ""
-- Installing: /usr/local/bin/cyusbserialtest
-- Set runtime path of "/usr/local/bin/cyusbserialtest" to ""
DONE!
Library installation complete.
Going back...
DONE!
Entering the TinI/O build directory
DONE!
</code>
</pre>
</details>


If a part of the installation process fails, the script will abort the installation and print out: `Error! The last action autobuild tried to perform failed.` In such case, try also the manual installation described below. If your installation completes succesfully, proceed to the next chapter.

## The hard way

In case the automated installation fails, or if you specifically want to build TinI/O manually, you can build TinI/O with a GNU Makefile and the library for it with CMake. The order of installation is important: the library must be built before TinI/O, because TinI/O needs the library to be built.
To build the library, go to the `cylib/build` directory and run `cmake ..` folowed by `make` and `make install` __as root__. The output should look similiar to this:
```
# cd cylib/build
# cmake ..
-- The C compiler identification is GNU 6.4.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Found PkgConfig: /usr/bin/pkg-config (found version "0.29")
-- Checking libusb version...
-- libusb version: 1.0.9

-- Looking for pthread.h
-- Looking for pthread.h - found
-- Looking for pthread_create
-- Looking for pthread_create - not found
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - found
-- Found Threads: TRUE
CMake Warning at lib/CMakeLists.txt:86 (message):


 libusb >= 1.0.16 is HIGHLY recommended.  If you experience issues or poor
 performance, please try updating libusb.

 Detected version: 1.0.9



-- Configuring done
-- Generating done
-- Build files have been written to: /root/projekti/CyUSB/cylib/build
# make
Scanning dependencies of target cyusbserial
[ 10%] Building C object lib/CMakeFiles/cyusbserial.dir/cyusb.c.o
[ 20%] Building C object lib/CMakeFiles/cyusbserial.dir/cyuart.c.o
[ 30%] Building C object lib/CMakeFiles/cyusbserial.dir/cyi2c.c.o
[ 40%] Building C object lib/CMakeFiles/cyusbserial.dir/cyspi.c.o
[ 50%] Building C object lib/CMakeFiles/cyusbserial.dir/cyphdc.c.o
[ 60%] Building C object lib/CMakeFiles/cyusbserial.dir/cyjtag.c.o
[ 70%] Building C object lib/CMakeFiles/cyusbserial.dir/cymisc.c.o
[ 80%] Linking C shared library libcyusbserial.so
[ 80%] Built target cyusbserial
Scanning dependencies of target cyusbserialtest
[ 90%] Building C object tools/CMakeFiles/cyusbserialtest.dir/cyusbserialtest.c.o
[100%] Linking C executable cyusbserialtest
[100%] Built target cyusbserialtest
# make install
[ 80%] Built target cyusbserial
[100%] Built target cyusbserialtest
Install the project...
-- Install configuration: ""
-- Up-to-date: /usr/local/include/CyUSBSerial.h
-- Installing: /usr/local/lib/libcyusbserial.so.1
-- Up-to-date: /usr/local/lib/libcyusbserial.so
-- Set runtime path of "/usr/local/lib/libcyusbserial.so.1" to ""
-- Installing: /usr/local/bin/cyusbserialtest
-- Set runtime path of "/usr/local/bin/cyusbserialtest" to ""
```
After the succesful library installation, go to the `tinio` directory in the package root and run `make` and `make install` __as root__. The output should look similar to this:
