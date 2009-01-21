#include "dbwrite.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>

#define linsiz 1000

using namespace std;

int main(int argc, char *argv[]){
	fstream theFile;
    streampos pre, post;
	char a;
    char line[linsiz];
	string dna;
    int multiplier;

    multiplier = 2; // Determines how many big hash file will be to index file
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
        int temp;
        for(temp=0;line[temp]!= '\0';temp++){}
        db.writeFirst(line, static_cast<unsigned>((post - pre)));
        db.writeLine(line, static_cast<unsigned long long>((post - pre)));
        theFile >> ws;

        // Read and write the description`
        pre = theFile.tellg();
        theFile.getline(line, linsiz);
        post = theFile.tellg();
        if(post - pre >= 1000){
            cerr << "ERROR: Description line larger than buffer, closing db\n";
            db.close();
            exit(1);
        }
        db.writeLine(line,
                static_cast<unsigned long long>((post - pre-1)));

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
        char seq[dna.size()];
        for(unsigned long long i=0;i<dna.size();i++){
            seq[i] = dna[i];
        }

		db.writeLine(seq, dna.size());
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
