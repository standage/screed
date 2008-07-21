#seqdb2 makefile

all: seqdb2 seqdb2_test

seqdb2: seqdb2.drv.o seqdb2.lib.o
	g++ seqdb2.drv.o seqdb2.lib.o -o seqdb2

seqdb2_test: seqdb2.test.o seqdb2.lib.o
	g++ seqdb2.test.o seqdb2.lib.o -o seqdb2_test

seqdb2.test.o: src/seqdb2.test.cpp src/lib/seqdb2.h
	g++ -Wall -g -c src/seqdb2.test.cpp

seqdb2.drv.o: src/seqdb2.drv.cpp src/lib/seqdb2.h
	g++ -Wall -g -c src/seqdb2.drv.cpp

seqdb2.lib.o: src/lib/seqdb2.lib.cpp src/lib/seqdb2.h
	g++ -Wall -g -c src/lib/seqdb2.lib.cpp
