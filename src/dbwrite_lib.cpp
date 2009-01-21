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
    unsigned long long pLoc;
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
bool dbwrite::writeLine(char* theLine, unsigned long long lsize){
	dbFile.write((char*)&(lsize), sizeof(lsize));
	dbFile.write(theLine, lsize);
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
    unsigned long long hashdResult, collisions, totalCollisions;
    map<long long, long long> Nums4Hash;
    map<long long, long long>::iterator it;
    result = true;
    totalCollisions = 0;
    Prev = Head;
    Curr = Head->Next;
    // Build the in-memory hash
    for(unsigned long long streamPos=1;streamPos<=Recordlen;streamPos++){
        Prev = Curr;
        Curr = Curr->Next;
        hashdResult =hashFunct(Prev->data, Prev->len, Recordlen*hashMultiplier);
        hashdResult = hashdResult * 8; // Go by blocks of 8 bytes
        collisions = 0;
        // Find an empty block to write to
        while(1){
            if(Nums4Hash[hashdResult] == 0){
                break;
            }
            collisions++;
            totalCollisions++;
            // Block was occupied. Do a quadratic probe for next area
            hashdResult = hashdResult + 8*(pow(2, collisions)-1);
        }
        Nums4Hash[hashdResult] = streamPos;
    }
    // Write the in-memory hash to disk
    for(it=Nums4Hash.begin(); it != Nums4Hash.end(); it++){
        hashFile.seekp((*it).first);
        hashFile.write((char*)&((*it).second), 8);
    }
    // Write extra data to end so eof isn't encountered when reading last entry
    hashFile.seekp(0, ios_base::end);
    hashFile.write((char*)&(collisions), 8);

    //!!!DEBUGGING CODE!!!
    cout << totalCollisions << " collisions with average = " <<
        static_cast<double>(totalCollisions)/Recordlen <<
        " collisions per write\n" << Recordlen << " records total\n\n";
    
    return result;
}

/*-----------------------------------------
 * hashFunct
 * Takes in a char * type and a
 * long long type as arguments and computes
 * a long long hash value
-----------------------------------------*/
unsigned long long dbwrite::hashFunct(char* toHash, unsigned size,
        unsigned long long hashSize){
    unsigned long long result, a, b;
    result = 0;
    a = 63689;
    b = 378551;
    for(unsigned i=0;i<size;i++){
        result = result*a + int(toHash[i]);
        a = a * b;
    }
    result = result % hashSize;
    return result;
}
