#!/bin/bash

while :
do
	echo "Enter serial:"
	read SERIAL

	sed "s/SERIAL/$SERIAL/" mcv4.conf.in > mcv4.conf
	/home/rbarlow/src/libftdi/build/ftdi_eeprom/ftdi_eeprom --flash-eeprom mcv4.conf

	echo "Unplug. Wait 2 seconds. Plug in. Wait 2 seconds. Press button. Press enter."
	read
	/home/rbarlow/src/stm32flash/stm32flash -b 115200 -w mcv4.bin -v /dev/ttyUSB0
done

