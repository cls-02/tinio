CC=g++
tinio:
	echo "Building tinio..."
	$(CC) -lcyusbserial tinio.cpp -o tinio
	echo "done."

install:
	echo "Installing tinio..."
	mkdir -p /usr/share/tinio
	cp -r flash /usr/share/tinio
	cp tinio /usr/bin
	echo "done."
