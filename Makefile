#seqdb2 makefile

all: makedb readdb

makedb: src/dbmake.cpp
	g++ -Wall -g src/dbmake.cpp -o makedb

readdb: src/dbread.cpp
	g++ -Wall -g src/dbread.cpp -o readdb
