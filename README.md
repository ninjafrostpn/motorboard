Motor Board v4 Firmware
=======================

Prerequisites
-------------

These instructions assume that you are using a version of Linux, but should also work on BSD-derived OSes (including OS X). Windows users may wish to try [Cygwin][cygwin], create a Linux VM (although flashing the board from a VM may require some thought), or consider their life choices.

* `git` can be found in your distribution's repositories, or [online][git].
* A working C compiler, such as `gcc` or `clang`, also from your distribution's repositories.
* `make`, also from your distribution's repositories.
* The `arm-none-eabi` toolchain. This may be in your distribution's repositories, otherwise download it [from Launchpad][toolchain] and follow the instructions in the README.
* `stm32flash`, which can be found [on Google Code][stm32flash]. After downloading, build and install it:

	$ make
	$ sudo make install

Instructions
------------

1. From a shell, `cd` into the directory where you cloned the repository and download the `libopencm3` submodule:

	$ git submodule update --init

2. Determine your `arm-none-eabi` toolchain prefix, unless it is in your `$PATH`. If you downloaded it from the link above, it will be the path to the `bin` directory of the extracted folder, followed by `arm-none-eabi`.

3. Build `libopencm3`:

	$ cd libopencm3
	$ make PREFIX=<your toolchain prefix>

4. Change back to the project directory and build the firmware:

	$ cd ..
	$ make PREFIX=<your toolchain prefix>

5. Connect the motor board to your machine via USB. The USB Power Light (green, next to the connector) should turn on.

6. Connect the motor board to a power source. The main power light (green, in the middle of the board) should turn on. If using the SR power board, you will have to run a program to turn on the motor rail.

7. Press the firmware button on the side of the motor board with a long thin object.

8. To test connectivity, run the following command, where X is found by running `ls /dev/ttyUSB*`:

	$ stm32flash /dev/ttyUSB<X>

9. Load the firmware onto the motor board:

	$ stm32flash -w mcv4.bin -v /dev/ttyUSB<X>

[cygwin]:     http://cygwin.com/
[toolchain]:  https://launchpad.net/gcc-arm-embedded
[stm32flash]: http://stm32flash.googlecode.com
[git]:        http://git-scm.com
