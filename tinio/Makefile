all: library utility tinio

library:
	cd library && make

utility: library
	cd testUtility && make
tinio: library
	g++ -lcyusbserial tinio.cpp -o tinio

install:
	mkdir -p /usr/share/tinio
	cp -r flash /usr/share/tinio
	cp tinio /usr/bin
