#seqdb2 makefile

all: fqdbm dbread fadbm

fqdbm: src/fqdbm.cpp
	g++ -Wall -g src/fqdbm.cpp -o fqdbm

fadbm: src/fadbm.cpp
	g++ -Wall -g src/fadbm.cpp -o fadbm

dbread: dbread.drv.o dbread.lib.o
	g++ dbread.drv.o dbread.lib.o -o dbread

dbread.drv.o: src/dbread.drv.cpp src/dbread.h
	g++ -Wall -g -c src/dbread.drv.cpp

dbread.lib.o: src/dbread.lib.cpp src/dbread.h
	g++ -Wall -g -c src/dbread.lib.cpp
