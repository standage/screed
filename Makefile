# seqdb2 makefile

CPPFLAGS=-Wall -g

all: fqdbm dbread fadbm fadb_test fqdb_test shelltest

fqdbm: src/fqdbm.drv.o src/dbwrite.lib.o
	g++ src/fqdbm.drv.o src/dbwrite.lib.o -o fqdbm

fadbm: src/dbwrite.lib.o src/fadbm.drv.o
	g++ src/dbwrite.lib.o src/fadbm.drv.o -o fadbm

dbread: src/dbread.drv.o src/dbread.lib.o
	g++ src/dbread.drv.o src/dbread.lib.o -o dbread

shelltest: src/shelltest.cpp src/dbread.lib.o
	g++ src/shelltest.cpp src/dbread.lib.o -o shelltest

fadb_test: src/fadb_test.o src/dbread.lib.o src/dbwrite.lib.o
	g++ src/fadb_test.o src/dbread.lib.o src/dbwrite.lib.o -o fadb_test

fqdb_test: src/fqdb_test.o src/dbread.lib.o src/dbwrite.lib.o
	g++ src/fqdb_test.o src/dbread.lib.o src/dbwrite.lib.o -o fqdb_test

src/fadb_test.o: src/fadb_test.cpp src/dbread.h src/dbwrite.h

src/fqdb_test.o: src/fqdb_test.cpp src/dbread.h src/dbwrite.h

src/fqdbm.drv.o: src/fqdbm.drv.cpp src/dbwrite.h

src/fadbm.drv.o: src/fadbm.drv.cpp src/dbwrite.h

src/dbwrite.lib.o: src/dbwrite.lib.cpp src/dbwrite.h

src/dbread.drv.o: src/dbread.drv.cpp src/dbread.h

src/dbread.lib.o: src/dbread.lib.cpp src/dbread.h

test:
	./fadb_test
	./fqdb_test
	rm -f tests/*.seqdb2*

clean:
	rm -f dbread *test *dbm src/*.o tests/*seqdb2*
	rm -f src/_seqdb2.c src/_seqdb2.cpp src/_seqdb2.so

pymodule:
	pyrexc -+ src/_seqdb2.pyx
	g++ -pthread -fno-strict-aliasing -DNDEBUG -g -O3 -Wall -fPIC -I/usr/include/python2.5 -c src/_seqdb2.cpp -o src/_seqdb2.o
	g++ -pthread -shared src/_seqdb2.o src/dbread.lib.o -o src/_seqdb2.so
	./fadbm tests/test.fa
	cd src && python tst.py
