#include "dbwrite.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[]){
	fstream theFile;
	dbwrite db(argv[1]);
	string dna;
	unsigned i, j;
	char line[LINESIZE], endck;
	char name[LINESIZE], desc[LINESIZE];
	long long filepos, nmsiz, desiz;

	if(!db.is_open()){
		cerr << "ERROR: DATABASE FILES ARE NOT OPEN\n";
		exit(1);
	}
	theFile.open(argv[1], fstream::in);
	while(!theFile.eof()){
		nmsiz = 0; // Initialize the sizes of the relevant c-strings to
		desiz = 0; // 0

		//Increment the file position one to skip the '>' in front of
		//the name
		filepos = theFile.tellg();
		theFile.seekg(filepos+1);
		theFile >> name >> ws; // Pull in the line and skip whitespace
		for(i=0;name[i]!='\0';i++){}
		i++;
		nmsiz = i;

		theFile.getline(desc, LINESIZE);
		for(i=0;desc[i]!='\0';i++){}
		i++;
		desiz = i;

		dna.clear();
		endck = '0';
		while((endck != '>') && (!theFile.eof())){
				theFile.getline(line, LINESIZE);
				dna.append(line);
				endck = theFile.peek();
		}

		db.writeFirst();
		db.writeLine(name, nmsiz);
		db.writeLine(desc, desiz);
		db.writeLine(dna.c_str(), (dna.size()+1));
		db.writeDelim();
	}
	cout << "Database saved in " << argv[1] << ".seqdb2\n";
	cout << "Index saved in " << argv[1] << ".seqdb2.idx\n";

	theFile.close();
	return 0;
}
