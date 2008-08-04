#include "dbwrite.h"
#include "dbread.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <assert.h>

using namespace std;

int main(){
	fstream theFile;
	string filename = "tests/test.fastq";
	string name, dna, accu;
	dbwrite db(filename);
	char endck;
	long long filepos;

	if(!db.is_open()){
		cerr << "ERROR: DATABASE FILES ARE NOT OPEN\n";
		exit(1);
	}
	theFile.open(filename.c_str(), fstream::in);
	while(!theFile.eof()){

		//Read in the data line by line and store it in the relevant
		//character array
		filepos = theFile.tellg();
		theFile.seekg(filepos+1);
		theFile >> name >> ws;

		theFile >> dna >> ws;
		theFile.ignore(2); // Ignores the +\n characters

		theFile >> accu >> ws;

		endck = theFile.peek();

		db.writeFirst();
		db.writeLine(name);
		db.writeLine(accu);
		db.writeLine(dna);
		db.writeDelim();
	}

	//Close the open file handles
	theFile.close();
	db.close();

	//Open the newly created database and make sure everything was created
	//properly
	filename.append(".seqdb2");
	dbread thedb(filename);

	thedb.getRecord(0);
	assert(string(thedb.getType(0)) ==
			"HWI-EAS_4_PE-FC20GCB:1:1:62:922/1");
	assert(string(thedb.getType(1)) ==
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAADDDAAD");
	assert(string(thedb.getType(2)) ==
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	thedb.getRecord(12);
	assert(string(thedb.getType(0)) ==
			"HWI-EAS_4_PE-FC20GCB:1:1:63:978/1");
	assert(string(thedb.getType(1)) ==
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAA");
	assert(string(thedb.getType(2)) ==
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	thedb.getRecord(47);
	assert(string(thedb.getType(0)) ==
			"HWI-EAS_4_PE-FC20GCB:1:1:899:619/1");
	assert(string(thedb.getType(1)) ==
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAA<?;>>?8");
	assert(string(thedb.getType(2)) ==
			"TTCAAGATTCGACCCAATACCATTTTAACCAGGAGG");

	thedb.getRecord(48);
	assert(string(thedb.getType(0)) ==
			"HWI-EAS_4_PE-FC20GCB:1:1:57:519/1");
	assert(string(thedb.getType(1)) ==
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	assert(string(thedb.getType(2)) ==
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	return 0;
}
