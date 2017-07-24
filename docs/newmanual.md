# Welcome to the TinI/O Manual!

Welcome to the TinI/O manual. In this document you'll find information about installing, configuring and using TinI/O, the simple interface to I/O.

-----

# What is TinI/O?
Simply put, TinI/O is a Linux program that, when combined with a certain chip, adds more general purpose input/output (GPIO) pins to your computer. Those can then be used to control custom electronics outside the computer, for example a MOSFET driving a fan, a LED, a sensor or anything that can be controlled or read from with logic ones and zeros. That certain chip is in this case, the [Cypress CY7C65211 USB-Serial Bridge Controller](http://www.cypress.com/products/usb-uart-controller-gen-2). TinI/O sends commands to it over USB using [the libcyusbserial library](https://github.com/cyrozap/libcyusbserial) made by cyrozap and [the libusb library](http://libusb.info/). When the chip recieves the command, it changes or reads the state of the requested GPIO pin.

Using TinI/O is free (as beer _and_ speech), because it's licensed under GNU General Public License. You can use it free of any charges. To run it, you'll need a computer with USB running Linux, and a board, compatible with TinI/O ([like this one](http://www.cypress.com/documentation/development-kitsboards/psoc-4-cy8ckit-049-4xxx-prototyping-kits)).

# Where do you start?
To start using TinI/O, make sure you have:
- A computer running some sort of Linux that has:
  + A free USB port for the board
  + installed libusb
  + 16 MB of free space (or 8 for the bare minimum)
  + A C and C++ compiler (preferably gcc and g++)


- A TinI/O compatible board

  They can be obtained from Cypress, the manufacturer of the chip. The board I personally use and recommend is the [CY8CKIT-049-4xxx PSoC4 prototyping kit](http://www.cypress.com/documentation/development-kitsboards/psoc-4-cy8ckit-049-4xxx-prototyping-kits), but there are more of them out there, for example, the [CYUSBS234 USB-Serial (Single Channel) Development Kit](http://www.cypress.com/documentation/development-kitsboards/cyusbs234-usb-serial-single-channel-development-kit), which uses the same chip, or the [CYUSB232 USB-UART LP Reference Design Kit](http://www.cypress.com/documentation/development-kitsboards/cyusbs232-usb-uart-lp-reference-design-kit) and the [CYUSBS236 USB-Serial (Dual Channel) Development Kit](http://www.cypress.com/documentation/development-kitsboards/cyusbs236-usb-serial-dual-channel-development-kit), that have different chips from the same family.

# Building and installing TinI/O
After you make sure that your computer complies with the system requirements (it probably does), you need to install TinI/O. The installation can be done either with makefiles or with a script. The script is simpler to use and recommended for users unfamiliar with Linux.
