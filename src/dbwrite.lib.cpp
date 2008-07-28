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
dbwrite::dbwrite(string fname){
	open = true;
	fname.append(".seqdb2");
	dbFile.open(fname.c_str(), fstream::out);
	fname.append(".idx");
	idxFile.open(fname.c_str(), fstream::out);

	if((!dbFile.is_open()) || (!idxFile.is_open())){
			open = false;
	}
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
bool dbwrite::addrecord(char name[], char desc[], char accu[], char dna[],
		int nmsiz, int desiz, int acsiz, long long dnsiz){

	if(open == false){ // One of the database files didn't open
		return false;

	}
	
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

	return true;
}

/*------------------------------------------
 * Name: close
 * Explicitly closes the file handles
------------------------------------------*/
void dbwrite::close(){
	dbFile.close();
	idxFile.close();
}
