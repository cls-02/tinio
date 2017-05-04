CC=g++

default: tinio utils

tinio:
	echo "Building tinio..."
	$(CC) -lcyusbserial main.cpp -o tinio

utils:
	$(CC) -lcyusbserial Command_Utility.c -o cy-config

install:
	echo "Installing tinio and flasher"
	cp tinio /usr/bin
	cp cy-config /usr/bin
	mkdir /usr/share/tinio
	mkdir /usr/share/tinio/flashes
	cp flashes/* /usr/share/tinio/flashes
