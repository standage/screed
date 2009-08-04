// Copyright 2008-2009 Michigan State University. All rights reserved.

#define PRELOAD_INDEX 1

#include "dbread.h"
#include <fstream>
#include <new>
#include <string>
#include <iostream>
#include <math.h>

#define fieldsize 100

using namespace std;

/*----------------------------------------
 * Constructor
 * Takes a string file name and opens
 * the correct _screed and _idx files.
 * Also reads the first record into cache
----------------------------------------*/
dbread::dbread(string dbname){
	dbread::Node *Curr;
	dbread::Node *Prev;
	string idxname, hashname;
	unsigned i, j;
	char fieldname[fieldsize];
	char a;

	Head = NULL;

	open = true;
	failbit = false;
	idxname = dbname + "_idx";
	hashname = dbname + "_hash";
	lastquery = -19;
	empty = ' ';
	NumberOfAttributes = 1;
	errmsg = "";

	idxFile.open(idxname.c_str(), ios::in | ios::binary);
	hashFile.open(hashname.c_str(), ios::in | ios::binary);

	if(!idxFile.is_open() || !hashFile.is_open()){
		open = false;
		errmsg = "Invalid database filename";
		return;
	}

	Head = new (nothrow) Node;

	idxFile.close();

	//

	idxFile.open(idxname.c_str(), ios::in | ios::binary);

	idxFile.seekg(0, ios_base::end);
	size = idxFile.tellg();
	//	std::cout << "END AT: " << size << "\n";
	size = size / sizeof(index_type);
	//	std::cout << "SIZE: " << size << "\n";

	idxFile.seekg(0);
	index = new (nothrow) index_type[size];
	idxFile.read((char *)index, (streamsize) (sizeof(index_type) * size));
	idxFile.close();

	
	idxFile.open(idxname.c_str(), ios::in | ios::binary);
	for(index_type li=0; li < size; li++) {
	  endian_swap(&index[li]);
	  //	  std::cout << "POS " << li << " VAL " << index[li] << "\n";

#if PRELOAD_INDEX
	  index_type i;
	  idxFile.seekg(li * sizeof(index_type));
	  idxFile.read((char *) &i, (streamsize) sizeof(index_type));
	  endian_swap(&i);

	  std::cout << "ATPOS: " << li << " VALA " << index[li] << " VALB " << i << "\n";
	  assert(i == index[li]);
#endif

	}
	
	//

	dbFile.open(dbname.c_str(), ios::in | ios::binary);
	if(!dbFile.is_open()){
        delete [] index;
		open = false;
		return;
	}

	// Gets the hashMultiplier
	dbFile.read((char*)&hashMultiplier, sizeof(hashMultiplier));
    endian_swap(&hashMultiplier);
	//Determine the amount and names of individual types per record
	for(a='0';a!='\n';NumberOfAttributes++){
		dbFile.getline(fieldname, fieldsize);
		AttributeMap[string(fieldname)] = NumberOfAttributes;
		a = dbFile.peek();
	}
	dbFile.ignore(1); // Ignore the \n char seperating the sections
	NumberOfAttributes--;

	//Create the publically-accessable RecordAttributes object for the user to
	//view the keys of the map with
	RecordAttributes = new (nothrow) char*[NumberOfAttributes];
	i = 0;
	for(maptype::iterator it=AttributeMap.begin();it!=AttributeMap.end();it++){
		RecordAttributes[i] = new (nothrow) char[(it->first).size()+1];
		for(j=0;j<(it->first).size();j++){
			RecordAttributes[i][j] = (it->first).at(j);
		}
		RecordAttributes[i][j] = '\0';
		i++;
	}

	//Create the LoadedAttributes object
	LoadedAttributes = new (nothrow) char *[NumberOfAttributes];
	for(i=0;i<NumberOfAttributes;i++){
		LoadedAttributes[i] = new (nothrow) char[2];
	}
}

/*--------------------------------------
 * Destructor
 * Calls close to do the closing of
 * files and deleting of allocated
 * memory
--------------------------------------*/
dbread::~dbread(){
    close();
}

/*------------------------------------------
 * Closes the database files and deletes all
 * memory allocated. Called by destructor
------------------------------------------*/
void dbread::close(){
    if (Head == NULL ) {    // failure in opening or some such.
        return;
    }

	dbFile.close();
	hashFile.close();
    open = false;

	delete Head;
    Head = NULL;
	delete [] index;
	for(unsigned i=0;i<NumberOfAttributes;i++){
		delete [] LoadedAttributes[i];
		delete [] RecordAttributes[i];
	}
	delete [] RecordAttributes;
	delete [] LoadedAttributes;
}

/*-------------------------------------------------
 * getRecord
 * Loads the record indexed by the unsigned long
 * long variable 'index' into memory from the
 * database.
-------------------------------------------------*/
void dbread::getRecord(index_type idx){
	if(open == false){
        errmsg = "Database files not open";
		failbit = true;
		return;
	}
	if((idx >= size) || (idx < 0)){
        errmsg = "Invalid query";
        failbit = true;
        return;
	}
	else if(idx == lastquery){
		return;
	}

	char a;
	unsigned i;
	index_type linelen;

	for(i=0;i<NumberOfAttributes;i++){
		delete [] LoadedAttributes[i];
	}

	index_type offset; // = index[idx];

#if 0
	idxFile.seekg(idx * sizeof(index_type), ios_base::beg);
	idxFile.read((char *) &offset, (streamsize) sizeof(index_type));
	endian_swap(&offset);
#endif

	//	std::cout << "PREOFF: " << offset;
	  offset = index[idx];
	  // std::cout << " POSTOFF: " << offset << "\n";

	dbFile.seekg(offset);

    if(dbFile.fail()){
        errmsg = "database does not extend to stream position " +
            string((char*)&(offset));
        failbit = true;
        return;
    }
	// Read new records into the LoadedAttributes array
	a = '0';
	for(i=0;i<NumberOfAttributes;i++){
		//Read the linelength integer in
		dbFile.read((char*)&linelen, sizeof(linelen));
        endian_swap(&linelen);
		LoadedAttributes[i] = new (nothrow) char[linelen+1];
		dbFile.read(LoadedAttributes[i], linelen); // Read in the line data
		LoadedAttributes[i][linelen] = '\0'; // Set the NULL character at end
	}
    // i must equal NumberOfAttributes when the loop exits
	if(i != NumberOfAttributes){
        errmsg = "Database files corrupted";
		failbit = true;
	}
	lastquery = idx;
    return;
}

/*------------------------------------------------------------
 * getAttributeValue
 * Returns a certain c-string from the LoadedAttributes array.
 * The c-string to be returned is referenced by the
 * 'wanted' c-string passed in and the association is
 * made with the AttributeMap map
------------------------------------------------------------*/
char* dbread::getAttributeValue(char wanted[]){
	unsigned i;
	i = AttributeMap[string(wanted)];
	if(i == 0){
        errmsg = "Invalid typename query";
		failbit = true;
		return &(empty);
	}
	return LoadedAttributes[i-1];
}

/*-----------------------------------------------------
 * getAttributeByNumber
 * Returns data from LoadedAttributes by direct indexing rather
 * by name as in getAttributeValue.
-----------------------------------------------------*/
char* dbread::getAttributeByNumber(unsigned idx){
        return getAttributeValue(getAttributeName(idx));
}

/*---------------------------------------------
 * getAttributeName
 * Returns the c-string stored the in the
 * double-pointer variable RecordAttributes. First
 * though, it checks to make sure that the 
 * referenced index is a valid one. This
 * function can be thought of as the inverse
 * of getAttributeValue
---------------------------------------------*/
char * dbread::getAttributeName(unsigned idx){
    if((idx >= NumberOfAttributes) || (idx < 0)){
        failbit = true;
        errmsg = "Bad typekey request";
        return &(empty);
    }
    return RecordAttributes[idx];
}

/*------------------------------------
 * clear
 * Clears the database error flag and
 * the database file error flags
------------------------------------*/
void dbread::clear(){
	failbit = false;
	idxFile.clear();
	dbFile.clear();
    hashFile.clear();
}

/*------------------------------------------
 * getHashRecord
 * Takes in the name of the record as a
 * string, uses hashFunct to turn the string
 * into an integer and then uses getRecord
 * to load the record into memory
------------------------------------------*/
index_type dbread::getHashRecord(char* RecordName, unsigned RCRDsize){
    index_type hashdResult, rcrdIdx;
    unsigned nameTypeint;
    int collisions;
    index_type hashFilelen = size*hashMultiplier;
    nameTypeint = AttributeMap["name"]-1;
    hashdResult = hashFunct(RecordName, RCRDsize, hashFilelen);
    hashdResult *= sizeof(index_type);
    collisions = 0;

    while(1){
        hashFile.seekg(hashdResult); // Go to the possible stream location
        hashFile.read((char*)&(rcrdIdx), sizeof(index_type));
        endian_swap(&rcrdIdx);
        if(rcrdIdx == 0){
            failbit = true;
            errmsg = "No named record in database";
            break;
        }
        else if(hashFile.fail()){
            failbit = true;
            errmsg = "Corrupted hash file";
            break;
        }

        rcrdIdx = rcrdIdx - 1; // Re-Correction for db writer necessary offset
        getRecord(rcrdIdx);
        // Compare retrieved record name to name passed in
        if(string(LoadedAttributes[nameTypeint]) == string(RecordName)){
            return rcrdIdx;
        }
        // Record was incorrect (a collision), continue searching
        collisions++;
        hashdResult = hashdResult + sizeof(index_type)*(static_cast<unsigned>(
            pow(static_cast<float>(2), collisions))-1);
        hashdResult = hashdResult % (hashFilelen*sizeof(index_type));
    }
    return rcrdIdx; // Only executed if an error occurred above
}
