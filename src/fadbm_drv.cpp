// Copyright 2008-2009 Michigan State University. All rights reserved.

#include "dbwrite.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <new>

#define linsiz 1000

using namespace std;

int main(int argc, char *argv[]){
	fstream theFile;
    streampos pre, post, len;
	char a;
    char line[linsiz];
	string dna;
    int multiplier;

    if((argc < 2) || (argc > 3)){
        cerr << "ERROR: USAGE IS: " << argv[0] << " <dbfilename>\n";
        exit(1);
    }
    multiplier = 2; // Determines how big hash file will be to index file
    if(argc == 3){
        multiplier = atol(argv[2]);
        if(multiplier <= 0){
            cerr << "ERROR: CAN'T CREATE DB OF 0 OR NEGATIVE HASH LENGTH\n";
            exit(1);
        }
    }
	dbwrite db(argv[1], 'a', multiplier);
	if(!db.is_open()){
		cerr << "ERROR: DATABASE FILES ARE NOT OPEN\n";
		exit(1);
	}
	theFile.open(argv[1], fstream::in);
	while(!theFile.eof()){

		//Increment the file position one to skip the '>' in front of
		//the name
        theFile.ignore(1);
        // Read and write the name
        pre = theFile.tellg();
		theFile >> line;
        post = theFile.tellg();
        len = post - pre;
        db.writeFirst(line, static_cast<unsigned>((len)));
        db.writeLine(line, static_cast<unsigned long long>((len)));
//        theFile >> ws;

        if(theFile.peek() == '\n'){ // No description
            theFile >> ws;
            line[0] = ' ';
            line[1] = '\0';
            len = streampos(1);
        }
        else{
            // Read and write the description
            theFile >> ws;
            pre = theFile.tellg();
            theFile.getline(line, linsiz);
            post = theFile.tellg();
            len = post - pre - static_cast<streampos>(1);
        }
        db.writeLine(line,
                static_cast<unsigned long long>((len)));

        // Read and write the sequence
		a = '0';
		while((a != '>') && (!theFile.eof())){
			// Another file-validity check
			if(!theFile.good()){
				cerr << "ERROR: The stream is corrupt. " <<
					"Did you open the correct file?\n" <<
					"Check to make sure the file isn't " <<
					"empty or an invalid fasta file\n";
				db.close();
				exit(1);
			}
            theFile.getline(line, linsiz);
			dna.append(line);
			a = theFile.peek();
		}
        char * seq = new(nothrow) char[dna.size()];
        for(unsigned long long i=0;i<dna.size();i++){
            seq[i] = dna[i];
        }

		db.writeLine(seq, dna.size());
        delete [] seq;
        dna.clear();
		if(db.fail() == true){
			cerr << "ERROR: One of the database file streams is "<<
				"corrupt. Do you have enough diskspace?\n";
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
