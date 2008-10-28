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
    hashFile.open(hashName.c_str(), ios::out | ios::in | ios::binary |
            ios::trunc);

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
	dbFile.close();
	idxFile.close();
    hashFile.close();
}

/*------------------------------------------
 * Name: close
 * Explicitly closes the file handles
------------------------------------------*/
void dbwrite::close(){
	dbFile.close();
	idxFile.close();
    hashFile.close();
}

/*-----------------------------------------
 * writeFirst
 * Write the put pointer's location to the
 * index file
 * Also push the name and location onto
 * a growing list of 2-entry lists
----------------------------------------*/
bool dbwrite::writeFirst(string name){
	idxFile.write((char*)&dbFile.tellp(), sizeof(dbFile.tellp()));
    Names4Hash.push(name);
    Recordlen++;
	idxFile.write(&newline, 1);
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
bool dbwrite::writeLine(string theLine){
    if(Recordlen == 497){
        cout << "line is:\n" << theLine << endl;
    }
	long long lsize;
	lsize = theLine.size() + 1;
	dbFile.write((char*)&(lsize), sizeof(lsize));
	dbFile.write(&space, 1);
	dbFile.write(theLine.c_str(), theLine.size());
	dbFile.write(&newline, 1);
	if(!dbFile.good()){
		failbit = true;
	}
	return !(dbFile.fail());
}

/*-------------------------------------
 * writeDelim
 * Write the record delimitor to the
 * database file
-------------------------------------*/
bool dbwrite::writeDelim(){
	dbFile.write(&delim, 1);
	if(!dbFile.good()){
		failbit = true;
	}
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
    std::string RecordName;
    bool result;
    long long hashdResult, testBlock, fileEnd;
    result = true;
    fileEnd = 0;
    for(long long streamPos=1;streamPos<=Recordlen;streamPos++){
        RecordName = Names4Hash.front();
        hashdResult = hashFunct(RecordName, (2*Recordlen));
        hashdResult = hashdResult * 8; // Go by blocks of 8 bytes
        // Find an empty block to write to
        while(1){
            hashFile.seekg(hashdResult);
            hashFile.read((char*)&(testBlock), 8);
            if(!hashFile.good()){ // Block area was beyond file, no problem
                hashFile.clear();
                break;
            }
            else if(testBlock == 0){ // Block is empty and is good for data
                break;
            }
            hashdResult = hashdResult + 8; // Block was occupied, go to next
        }
        hashFile.seekp(hashdResult);
        hashFile.write((char*)&(streamPos), 8);
        if(hashFile.tellp() > fileEnd){
            fileEnd = hashFile.tellp();
        }
        hashFile.flush();
        Names4Hash.pop();
    }
    // Write extra data to end so eof isn't encountered when reading last entry
    hashFile.seekp(fileEnd);
    hashFile.write((char*)&(fileEnd), 8);
    
    return result;
}

/*-----------------------------------------
 * hashFunct
 * Takes in a std::string type and a
 * long long type as arguments and computes
 * a long long hash value
-----------------------------------------*/
long long dbwrite::hashFunct(std::string toHash, long long hashSize){
    long long result;
    unsigned j;
    result = 0;
    for(unsigned i=0;i<(toHash.size()-2);i++){
        result += int(toHash[i]) * int(toHash[i+1]) * int(toHash[i+2]);
    }
    j = toHash.size()-1;
    for(unsigned i=0;i<j;i++){
        result += int(toHash[i]) * int(toHash[j]);
        j--;
    }
    result = result % hashSize;
    return result;
}
