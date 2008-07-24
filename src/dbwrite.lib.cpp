#include "dbwrite.h"
#include <string>
#include <fstream>
#include <iostream>
#include <new>
#include <stdlib.h>

using namespace std;

/*-------------------------------------------------
 * Constructor
 * Opens .seqdb2 and .seqdb2.idx files in the same
 * directory as the filename passed in using the
 * dbFile and idxFile file handles
-------------------------------------------------*/
dbwrite::dbwrite(char f[]){
	string fname(f);
	fname.append(".seqdb2");
	dbFile.open(fname.c_str(), fstream::out);
	fname.append(".idx");
	idxFile.open(fname.c_str(), fstream::out);
}

/*-----------------------------------------------
 * Destructor
 * Closes the database and index file handles
-----------------------------------------------*/
dbwrite::~dbwrite(){
	dbFile.close();
	idxFile.close();
}

/*-----------------------------------------------
 * addrecord
 * Takes the character arrays and lengths of all
 * attributes of the database and writes a single
 * record with them
-----------------------------------------------*/
void dbwrite::addrecord(char name[], char desc[], char accu[], char dna[],
		int nmsiz, int desiz, int acsiz, long long dnsiz){
	
	idxFile << dbFile.tellp() << endl;	

	dbFile.write(name, nmsiz-1);
	dbFile << endl;
	dbFile.write(desc, desiz-1);
	dbFile << endl;
	dbFile.write(accu, acsiz-1);
	dbFile << endl;
	dbFile.write(dna, dnsiz-1);
	dbFile << endl;
	dbFile << '-' << endl;
}
