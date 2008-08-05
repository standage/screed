#include "dbwrite.h"
#include "dbread.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include <string>
#include <stdlib.h>

using namespace std;

int main(){
	fstream theFile;
	char endck, a;
	char nam[] = "name";
	char seq[] = "sequence";
	char des[] = "description";
	string filename = "tests/test.fa";
	string line, name, desc, dna; 
	long long filepos;
	dbwrite db(filename, 'a');

	if(!db.is_open()){
		cerr << "ERROR: DATABASE FILES ARE NOT OPEN\n";
		exit(1);
	}
	theFile.open(filename.c_str(), fstream::in);
	while(!theFile.eof()){

		//Increment the file position one to skip the '>' in front of
		//the name
		filepos = theFile.tellg();
		theFile.seekg(filepos+1);
		theFile >> name >> ws; // Pull in the line and skip whitespace

		theFile >> desc;
		theFile.get(a);
		while(a != '\n'){
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

	//Closes reading and writing files
	theFile.close();
	db.close();

	//Do the testing to make sure everything still works
	filename.append("_seqdb2");
	dbread thedb(filename);

	thedb.getRecord(0);
	assert(string(thedb.getType(nam)) == "ENSMICT00000012722");
	assert(string(thedb.getType(des)) == "cdna:pseudogene scaffold:micMur1:scaffold_185008:9:424:1 gene:ENSMICG00000012730");
	assert(string(thedb.getType(seq)) == "TGCAGAAAATATCAAGAGTCAGCAGAAAAACTATACAAGGGCTGGTATTTTGATTATTCTATAAAAATTCACTTTTTGCTCAGTGTCTTTCATCTGGGCCTGGCCTCCTCTCTTGCAAGCCCTGGATTCATAACATCTATAATAATTTTTATATGTGGTAGAGTAATATTAGCTGATTCCTTTGCCTCCTGTTCCTTCCCCTCATTCAGGCAGCTGGCCAGGTTTGTGCTCCTTATCTCGCAGAAGAGATGTGATAGCAGGCAGAGAATTAAAGTCTTCCTGGCTTTTGGTTTCAGAAGCTGCCTTGGGAAGGAAGCAAACAAACATGCCACAGATAAAATATTTGAAAGAAAAGATAATGAAAGTAGAAAAGGGTTCCCTGTTCTTGTGGGGAGGAAGTGA");

	thedb.getRecord(2);
	assert(string(thedb.getType(nam)) == "ENSMICT00000004627");
	assert(string(thedb.getType(des)) == "cdna:novel scaffold:micMur1:scaffold_184569:3:516:-1 gene:ENSMICG00000004628");
	assert(string(thedb.getType(seq)) == "GCGAGCTCAGGCCGCCCTGGTTCACTCGTGTACCTCATGACCGCCCTCAAGGAAGACAACCTGCGCCGCGCCTCGCCTGACTCGCGGTGGCACGTCTTAGAGAAATACGGGCGCATCGGGGACGAGTACAGCCCGCGGGTCTACTCCAAGGCGATCGGGTTCGCCTTCCTCCGCTTCCCCCACAGACAGGACGCCCAGGCCCGCCAGTACGCCCTGGGCGGGGGCCCTCCAGCAGCCCGCGACCTGCGCGTGCACAGGGCGCGCAGCCGCCCTCGGGTCTCCCAGCGCGGCCTCCCCCACAGGTCCCGGTGCAGCTACACACGCTGCAGCCACTGGTCCTGCACTGGCTCTCCAGGCCCCAAATCCACAAGGGCGCGAAAGTGCAAGTCCCCATCAGGGTCCCGATGCCTCTCCAGGGCCAGGGCCACGTCCCTGTCCAGG");

	thedb.getRecord(21);
	assert(string(thedb.getType(nam)) == "ENSMICT00000003880");
	assert(string(thedb.getType(des)) == "cdna:novel scaffold:micMur1:scaffold_175819:130:631:1 gene:ENSMICG00000003884");
	assert(string(thedb.getType(seq)) == "ATGCTGCCTAAGTTTGACCCCAACGCGATCAAAGTCATGTACCTGAGGTGCACGGGTGGCGAAGTCAGTGCCGTGTCTGCCCTGGCTCCCAAGATCAGCCCCCTGGGTCTGTTGATTGAAGTGGTACCTTCTGCCTCTGCCCTGATCATCAAAGCTCTCAAGGAACCACTAGGAGACAGAAAGAAACAGAAAAACATTAAACACAACAGAAACATCACTTTTCATAAGATTGTCAACACTGCCAGACAGATGTGGCATCCATCTTTAGCCAGAGAACTCTCCAAAACCATTAAATATATCCTGGGGACTGCCTCTGTGGGATGCAACATTGATGGCCACCATCTTCATGCCATTATAGATGACATCAACAGTGGTGCAGTGGCATGCCCAGCTAGTTAA");
	
	return 0;
}
