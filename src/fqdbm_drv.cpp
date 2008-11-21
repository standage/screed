#include "dbwrite.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>

using namespace std;

int main(int argc, char *argv[]){
	fstream theFile;
	char endck;
	string name, dna, accu;
	long long filepos;
    int multiplier;

    multiplier = 2;
    if(argc == 3){
        multiplier = atol(argv[2]);
        if(multiplier <= 0){
            cerr << "ERROR: CAN'T CREATE DB OF 0 OR NEGATIVE HASH LENGTH\n";
            exit(1);
        }
    }
    dbwrite db(argv[1], 'q', multiplier);
	if(!db.is_open()){
		cerr << "ERROR: DATABASE FILES ARE NOT OPEN\n";
		exit(1);
	}
	theFile.open(argv[1], fstream::in);
	while(!theFile.eof()){

		//Read in the data line by line and store it in the relevant
		//character array
		filepos = theFile.tellg();
		theFile.seekg(filepos+1);

		theFile >> ws;
		// A file integrity checker
		if(!theFile.good()){
			cerr << "ERROR: There is something wrong with the " <<
				"source file stream\n";
			db.close();
			exit(1);
		}

		theFile >> name >> ws;
		theFile >> dna >> ws;
		theFile.ignore(2); // Ignores the +\n characters
		theFile >> accu >> ws;

		endck = theFile.peek();
		db.writeFirst(name);
		db.writeLine(name);
		db.writeLine(accu);
		db.writeLine(dna);

		// Make sure nothing went wrong when writing to the database
		if(db.fail() == true){
			cerr << "ERROR: Something went wrong while writing the "
				<< "database.\nHave you run out of space?\n";
			db.close();
			exit(1);
		}
	}
	theFile.close();
    db.hash2Disk();
	cout << "Database saved in " << argv[1] << "_seqdb2\n";
	cout << "Index saved in " << argv[1] << "_seqdb2_idx\n";
	cout << "Hash saved in " << argv[1] << "_seqdb2_hash\n";

	return 0;
}
