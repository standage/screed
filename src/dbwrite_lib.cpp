// Copyright 2008-2009 Michigan State University. All rights reserved.

#include "dbwrite.h"
#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

/*-------------------------------------------------
 * Constructor
 * Opens .seqdb2 and .seqdb2.idx files in the same
 * directory as the filename passed in using the
 * dbFile and idxFile file handles
-------------------------------------------------*/
dbwrite::dbwrite(string fname, char type, int multi){
	open = true;
	failbit = false;
    Recordlen = 0;
    string dbName, hashName;
    dbName = fname;
	dbName.append("_seqdb2");
	dbFile.open(dbName.c_str(), ios::out | ios::binary);
    hashName = dbName;
	dbName.append("_idx");
	idxFile.open(dbName.c_str(), ios::out | ios::binary);
    hashName.append("_hash");
    hashFile.open(hashName.c_str(), ios::out | ios::binary);
    hashMultiplier = multi;

    Head = new(nothrow) Node;
    Head->Next = NULL;
    Prev = Head;
    Curr = Head->Next;

	if((!dbFile.is_open()) || (!idxFile.is_open()) || (!hashFile.is_open())){
        open = false;
	}
    writeTop(type);
}

/*-----------------------------------------------
 * Destructor
 * Closes the database and index file handles
-----------------------------------------------*/
dbwrite::~dbwrite(){
    close();
}

/*------------------------------------------
 * Name: close
 * Explicitly closes the file handles
------------------------------------------*/
void dbwrite::close(){
	dbFile.close();
	idxFile.close();
    hashFile.close();

    Node * Curr;
    Node * Prev;
    Prev = Head;
    Curr = Head->Next;
    while(Curr != NULL){
        Prev = Curr;
        Curr = Curr->Next;
        delete [] Prev->data;
        delete Prev;
    }
}

/*-----------------------------------------
 * writeFirst
 * Write the put pointer's location to the
 * index file
 * Also push the name onto
 * a growing list of 2-entry list
----------------------------------------*/
bool dbwrite::writeFirst(char* name, unsigned len){
    char * data = new(nothrow) char[len+1];
    for(unsigned i=0;i<len;i++){
        data[i] = name[i];
    }
    data[len] = '\0';
    index_type pLoc;
    pLoc = dbFile.tellp();
	idxFile.write((char*)&(pLoc), sizeof(pLoc));

    Curr = new(nothrow) Node;
    Prev->Next = Curr;
    Prev = Curr;
    Curr->Next = NULL;

    Prev->data = data;
    Prev->len = len;
    Recordlen++;
	if(!idxFile.good()){
		failbit = true;
	}
	return !(idxFile.fail());
}

/*---------------------------------------
 * writeLine
 * Writes the given line to the database
 * file. First write the size of the
 * line and then the line itself
---------------------------------------*/
bool dbwrite::writeLine(char* theLine, index_type lsize){
	dbFile.write((char*)&(lsize), sizeof(lsize));
	dbFile.write(theLine, lsize);
	//cout << "LSIZE: " << lsize << endl;
	if(!dbFile.good()){
		failbit = true;
	}
	return !(dbFile.fail());
}

/*---------------------------------------
 * writeTop
 * Writes the multiplier used on the hash
 * file. Also writes the types of fields
 * in a small section at the top of the
 * db file.
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
    
    dbFile.write((char*)&(hashMultiplier), sizeof(hashMultiplier));
	dbFile.write(name.c_str(), name.size());
	dbFile.write(&newline, 1);
	dbFile.write(middle.c_str(), middle.size());
	dbFile.write(&newline, 1);
	dbFile.write(sequence.c_str(), sequence.size());
	dbFile.write(&newline, 1);
	dbFile.write(&newline, 1);
	if(!dbFile.good()){
		failbit = true;
	}
}

/*--------------------------------------------------
 * hash2Disk
 * Pop entries off the FIFO queue Names4Hash and
 * store their index number (0, 1, 2, etc...) at the
 * file stream position in hashFile determinded by
 * the hash function
--------------------------------------------------*/
bool dbwrite::hash2Disk(){
    bool result;
    index_type hashFilelen;
    hashFilelen = Recordlen * hashMultiplier;
    index_type hashdResult, totalCollisions, i;
    int collisions;
    index_type* hashArray;

    // Declare and zero out the array
    hashArray = new(nothrow) index_type[hashFilelen];
    for(i=0;i<hashFilelen;i++){
        hashArray[i] = 0;
    }

    result = true;
    totalCollisions = 0;
    Prev = Head;
    Curr = Head->Next;
    // Build the in-memory hash
    for(index_type streamPos=1;streamPos<=Recordlen;streamPos++){
        Prev = Curr;
        Curr = Curr->Next;
        hashdResult = hashFunct(Prev->data, Prev->len, hashFilelen);
        collisions = 0;
        // Find an empty block to write to
        while(1){
            if(hashArray[hashdResult] == 0){
                break;
            }
            collisions++;
            totalCollisions++;
            // Block was occupied. Do a quadratic probe for next area
            hashdResult = hashdResult + static_cast<unsigned>(
                pow(static_cast<float>(2), collisions))-1;
	    if(hashdResult >= hashFilelen){ // Makes the data wrap around
		hashdResult = hashdResult - hashFilelen;
            }
	}
	hashArray[hashdResult] = streamPos;
    }
    // Write the in-memory array to disk
    for(i=0; i < hashFilelen; i++){
    	if(hashArray[hashdResult] == 0){
	    continue;
	}
	hashFile.seekp(i*sizeof(index_type));
	hashFile.write((char*)&(hashArray[i]), sizeof(index_type));
    }
    // Write extra data to end so eof isn't encountered when reading last entry
    hashFile.seekp(0, ios_base::end);
    hashFile.write((char*)&(collisions), sizeof(index_type));
    
    // Delete the array
    delete [] hashArray;

    //!!!DEBUGGING CODE!!!
    cout << totalCollisions << " collisions with average = " <<
        static_cast<double>(totalCollisions)/Recordlen <<
        " collisions per write\n" << Recordlen << " records total\n\n";
    
    return result;
}
