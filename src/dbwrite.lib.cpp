#include "dbwrite.h"
#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

/*-------------------------------------------------
 * Constructor
 * Opens .seqdb2 and .seqdb2.idx files in the same
 * directory as the filename passed in using the
 * dbFile and idxFile file handles
-------------------------------------------------*/
dbwrite::dbwrite(string fname, char type){
	open = true;
	fname.append("_seqdb2");
	dbFile.open(fname.c_str(), ios::out | ios::binary);
	fname.append("_idx");
	idxFile.open(fname.c_str(), ios::out | ios::binary);

	if((!dbFile.is_open()) || (!idxFile.is_open())){
			open = false;
	}
	writeTop(type);
}

/*-----------------------------------------------
 * Destructor
 * Closes the database and index file handles
-----------------------------------------------*/
dbwrite::~dbwrite(){
	dbFile.close();
	idxFile.close();
}

/*------------------------------------------
 * Name: close
 * Explicitly closes the file handles
------------------------------------------*/
void dbwrite::close(){
	dbFile.close();
	idxFile.close();
}

/*-----------------------------------------
 * writeFirst
 * Write the put pointer's location to the
 * index file
----------------------------------------*/
bool dbwrite::writeFirst(){
	idxFile.write((char*)&dbFile.tellp(), sizeof(dbFile.tellp()));
	idxFile.write(&newline, 1);
	return !(idxFile.fail());
}

/*---------------------------------------
 * writeLine
 * Writes the given line to the database
 * file. First write the size of the
 * line and then the line itself
---------------------------------------*/
bool dbwrite::writeLine(string theLine){
	long long lsize;
	lsize = theLine.size() + 1;
	dbFile.write((char*)&(lsize), sizeof(lsize));
	dbFile.write(&space, 1);
	dbFile.write(theLine.c_str(), theLine.size());
	dbFile.write(&newline, 1);
	return !(dbFile.fail());
}

/*-------------------------------------
 * writeDelim
 * Write the record deliminator to the
 * database file
-------------------------------------*/
bool dbwrite::writeDelim(){
	dbFile.write(&delim, 1);
	return !(dbFile.fail());	
}

/*---------------------------------------
 * writeTop
 * Writes the types of fields in a small
 * section at the top of the db file
---------------------------------------*/
void dbwrite::writeTop(char a){
	string name, sequence, middle;
	name = "name";
	sequence = "sequence";
	if(a == 'a'){ // sequence is a fasta file
		middle = "description";
	}
	else if(a == 'q'){ // sequence is a fastq file
		middle = "accuracy";
	}

	dbFile.write(name.c_str(), name.size());
	dbFile.write(&newline, 1);
	dbFile.write(middle.c_str(), middle.size());
	dbFile.write(&newline, 1);
	dbFile.write(sequence.c_str(), sequence.size());
	dbFile.write(&newline, 1);
	dbFile.write(&delim, 1);
	dbFile.write(&newline, 1);
}
