CC=g++

default: tinio utils

tinio:
	echo "Building tinio..."
	$(CC) -lcyusbserial tinio.cpp -o tinio

utils:
	$(CC) -lcyusbserial cy-config.c -o cy-config

install:
	echo "Installing tinio and flasher"
	cp tinio /usr/bin
	cp cy-config /usr/bin
	mkdir /usr/share/tinio
	mkdir /usr/share/tinio/flashes
	cp flashes/* /usr/share/tinio/flashes
	
