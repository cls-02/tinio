SHELL = /bin/bash
CC=g++

default: tinio

tinio:
	echo "Building tinio..."
	$(CC) -lcyusbserial -lusb-1.0 main.cpp -o tinio
