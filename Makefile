#seqdb2 makefile

all: fqdbm dbread fadbm

fqdbm: src/fqdbm.drv.o src/dbwrite.lib.o
	g++ src/fqdbm.drv.o src/dbwrite.lib.o -o fqdbm

fadbm: src/dbwrite.lib.o src/fadbm.drv.o
	g++ src/dbwrite.lib.o src/fadbm.drv.o -o fadbm

dbread: dbread.drv.o dbread.lib.o
	g++ dbread.drv.o dbread.lib.o -o dbread

fqdbm.drv.o: src/fqdmb.drv.cpp src/dbwrite.h
	g++ -Wall -g -c src/fqdbm.drv.cpp

fadbm.drv.o: src/fadbm.drv.cpp src/dbwrite.h
	g++ -Wall -g -c src/fadbm.drv.cpp

dbwrite.lib.o: src/dbwrite.lib.cpp src/dbwrite.h
	g++ -Wall -g -c src/dbwrite.lib.cpp

dbread.drv.o: src/dbread.drv.cpp src/dbread.h
	g++ -Wall -g -c src/dbread.drv.cpp

dbread.lib.o: src/dbread.lib.cpp src/dbread.h
	g++ -Wall -g -c src/dbread.lib.cpp
