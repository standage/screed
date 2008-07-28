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
	dbwrite db(filename);
	unsigned i, j;
	char endck, desc[] = "";
	char name[LINESIZE], dna[LINESIZE], accu[LINESIZE];
	long long filepos, dnsiz;
	int nmsiz, acsiz;

	if(!db.is_open()){
		cerr << "ERROR: DATABASE FILES ARE NOT OPEN\n";
		exit(1);
	}
	theFile.open(filename.c_str(), fstream::in);
	while(!theFile.eof()){
		nmsiz = 0;
		acsiz = 0;
		dnsiz = 0;

		//Read in the data line by line and store it in the relevant
		//character array
		filepos = theFile.tellg();
		theFile.seekg(filepos+1);
		theFile.getline(name, LINESIZE);
		for(i=0;name[i]!='\0';i++){}
		i++;
		nmsiz = i;

		theFile.getline(dna, LINESIZE);
		for(i=0;dna[i]!='\0';i++){}
		i++;
		dnsiz = i;
		theFile.ignore(2); // Ignores the +\n characters

		theFile.getline(accu, LINESIZE);
		for(i=0;accu[i]!='\0';i++){}
		i++;
		acsiz = i;

		endck = theFile.peek();

		db.addrecord(name, desc, accu, dna, nmsiz, 1, acsiz, dnsiz);
	}

	//Close the open file handles
	theFile.close();
	db.close();

	//Open the newly created database and make sure everything was created
	//properly
	filename.append(".seqdb2");
	dbread thedb(filename);

	assert(thedb.query(0, 1) == "HWI-EAS_4_PE-FC20GCB:1:1:62:922/1");
	assert(thedb.query(0, 3) == "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAADDDAAD");
	assert(thedb.query(0, 4) == "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	assert(thedb.query(12, 1) == "HWI-EAS_4_PE-FC20GCB:1:1:63:978/1");
	assert(thedb.query(12, 3) == "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAA");
	assert(thedb.query(12, 4) == "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	assert(thedb.query(47, 1) == "HWI-EAS_4_PE-FC20GCB:1:1:899:619/1");
	assert(thedb.query(47, 3) == "AAAAAAAAAAAAAAAAAAAAAAAAAAAAA<?;>>?8");
	assert(thedb.query(47, 4) == "TTCAAGATTCGACCCAATACCATTTTAACCAGGAGG");

	assert(thedb.query(48, 1) == "HWI-EAS_4_PE-FC20GCB:1:1:57:519/1");
	assert(thedb.query(48, 3) == "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	assert(thedb.query(48, 4) == "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	return 0;
}
