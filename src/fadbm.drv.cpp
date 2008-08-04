#include "dbwrite.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>

using namespace std;

int main(int argc, char *argv[]){
	fstream theFile;
	dbwrite db(argv[1]);
	char endck, a;
	string line, name, desc, dna;
	long long filepos;

	if(!db.is_open()){
		cerr << "ERROR: DATABASE FILES ARE NOT OPEN\n";
		exit(1);
	}
	theFile.open(argv[1], fstream::in);
	while(!theFile.eof()){

		//Increment the file position one to skip the '>' in front of
		//the name
		filepos = theFile.tellg();
		theFile.seekg(filepos+1);
		theFile >> name >> ws; // Pull in the line and skip whitespace

		theFile >> desc;
		theFile.get(a);
		while(a != '\n'){ // Appends the rest of description
			desc.push_back(a);
			theFile.get(a);
		}

		dna.clear();
		endck = '0';
		while((endck != '>') && (!theFile.eof())){
			a = '0';
			theFile >> line >> ws;
			dna.append(line);
			endck = theFile.peek();
		}

		db.writeFirst();
		db.writeLine(name);
		db.writeLine(desc);
		db.writeLine(dna);
		db.writeDelim();
	}
	cout << "Database saved in " << argv[1] << ".seqdb2\n";
	cout << "Index saved in " << argv[1] << ".seqdb2.idx\n";

	theFile.close();
	return 0;
}
