all:
	#	gcc -g -o CyUSBSerialTestUtility Test_Utility.c -lcyusbserial -w
	gcc -g -o CyUSBSerialCommandUtility Command_Utility.c -lcyusbserial
	cp 90-cyusb.rules /etc/udev/rules.d 
	cp CyUSBSerialCommandUtility /usr/bin/cy-config
	cp CyUSBSerial.sh /usr/bin
	chmod 777 /usr/bin/CyUSBSerial.sh

