#seqdb2 makefile

DOB = g++ -Wall -g -c

all: fqdbm dbread fadbm fadb_test fqdb_test

fqdbm: src/fqdbm.drv.o src/dbwrite.lib.o
	g++ src/fqdbm.drv.o src/dbwrite.lib.o -o fqdbm

fadbm: src/dbwrite.lib.o src/fadbm.drv.o
	g++ src/dbwrite.lib.o src/fadbm.drv.o -o fadbm

dbread: src/dbread.drv.o src/dbread.lib.o
	g++ src/dbread.drv.o src/dbread.lib.o -o dbread

fadb_test: src/fadb_test.o src/dbread.lib.o src/dbwrite.lib.o
	g++ src/fadb_test.o src/dbread.lib.o src/dbwrite.lib.o -o fadb_test

fqdb_test: src/fqdb_test.o src/dbread.lib.o src/dbwrite.lib.o
	g++ src/fqdb_test.o src/dbread.lib.o src/dbwrite.lib.o -o fqdb_test

fadb_test.o: src/fadb_test.cpp src/dbread.h src/dbwrite.h
	${DOB} src/fadb_test.cpp

fqdb_test.o: src/fqdb_test.cpp src/dbread.h src/dbwrite.h
	${DOB} src/fqdb_test.cpp

fqdbm.drv.o: src/fqdmb.drv.cpp src/dbwrite.h
	${DOB} src/fqdbm.drv.cpp

fadbm.drv.o: src/fadbm.drv.cpp src/dbwrite.h
	${DOB} src/fadbm.drv.cpp

dbwrite.lib.o: src/dbwrite.lib.cpp src/dbwrite.h
	${DOB} src/dbwrite.lib.cpp

dbread.drv.o: src/dbread.drv.cpp src/dbread.h
	${DOB} src/dbread.drv.cpp

dbread.lib.o: src/dbread.lib.cpp src/dbread.h
	${DOB} src/dbread.lib.cpp

test:
	./fadb_test
	./fqdb_test
	rm tests/*.seqdb2*
