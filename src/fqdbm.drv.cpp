#include "dbwrite.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[]){
	fstream theFile;
	dbwrite db(argv[1]);
	unsigned i;
	char endck;
	char name[LINESIZE], dna[LINESIZE], accu[LINESIZE];
	long long filepos, nmsiz, acsiz, dnsiz;

	if(!db.is_open()){
		cerr << "ERROR: DATABASE FILES ARE NOT OPEN\n";
		exit(1);
	}
	theFile.open(argv[1], fstream::in);
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
		db.writeFirst();
		db.writeLine(name, nmsiz);
		db.writeLine(accu, acsiz);
		db.writeLine(dna, dnsiz);
		db.writeDelim();
	}

	cout << "Database saved in " << argv[1] << ".seqdb2\n";
	cout << "Index saved in " << argv[1] << ".seqdb2.idx\n";

	theFile.close();
	return 0;
}
