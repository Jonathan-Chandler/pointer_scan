ECHO = echo
MAKE = mingw32-make
# CC = mingw32-g++.exe
CC = g++.exe
CFLAGS = -Wall -std=c++17

export ECHO
export CC
export INCLUDES
export CFLAGS

all: $(OBJS)
	rm -rf build/scanner/*
	rm -rf scanner.exe
	rm -rf test_scanner.exe
	mkdir -p build/test/
	mkdir -p build/scanner/
	$(MAKE) -C ./src/scanner all
	$(MAKE) -C ./src/test all
#	$(MAKE) -C ./src/unit_tests all

clean:
	$(MAKE) -C ./src/scanner clean
	$(MAKE) -C ./src/test clean
#	$(MAKE) -C ./src/unit_tests clean

