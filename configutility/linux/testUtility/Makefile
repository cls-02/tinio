all:
	#	gcc -g -o CyUSBSerialTestUtility Test_Utility.c -lcyusbserial -w
	gcc -g -o CyUSBSerialCommandUtility Command_Utility.c -lcyusbserial
	cp 90-cyusb.rules /etc/udev/rules.d 
	cp CyUSBSerialCommandUtility /usr/bin
	cp CyUSBSerial.sh /usr/bin
	chmod 777 /usr/bin/CyUSBSerial.sh
clean:
	rm -f CyUSBSerialCommandUtility
help:
	@echo	'make		would compile and create the library and create a link'
	@echo	'make clean	would remove the library and the soft link to the library (soname)'
