#seqdb2 makefile

makedb: src/dbmake.cpp
	g++ -Wall -g src/dbmake.cpp -o makedb
