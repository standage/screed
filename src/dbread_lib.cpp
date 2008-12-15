#include "dbread.h"
#include <fstream>
#include <new>
#include <string>
#include <iostream>
#include <math.h>

using namespace std;

/*----------------------------------------
 * Constructor
 * Takes a string file name and opens
 * the correct _seqdb2 and _idx files.
 * Also reads the first record into cache
----------------------------------------*/
dbread::dbread(string dbname){
	dbread::Node *Curr;
	dbread::Node *Prev;
	string idxname, hashname;
	unsigned i, j;
	unsigned fieldsize = 100;
	char fieldname[fieldsize];
	char a;

	Head = NULL;

	open = true;
	failbit = false;
	idxname = dbname + "_idx";
	hashname = dbname + "_hash";
	lastquery = -19;
	empty = ' ';
	Typesize = 1;
	errmsg = "";

	idxFile.open(idxname.c_str(), ios::in | ios::binary);
	hashFile.open(hashname.c_str(), ios::in | ios::binary);

	if(!idxFile.is_open() || !hashFile.is_open()){
		open = false;
		errmsg = "Invalid database filename";
		return;
	}

	Head = new (nothrow) Node;
	Head->Next = NULL;
	Prev = Head;

	//Copy the index file completly into memory so results can be retrieved
	//faster
	for(size=0;!idxFile.eof();size++){
		Curr = new (nothrow) Node;
		Prev->Next = Curr;
		Curr->Next = NULL;
		Prev = Curr;
		idxFile.read((char*)&(Curr->data), sizeof(Curr->data));
		idxFile.peek();
	}
	idxFile.close();

	dbFile.open(dbname.c_str(), ios::in | ios::binary);
	if(!dbFile.is_open()){
	        // @CTB remember to dealloc linked list.
	          
		open = false;
		return;
	}

	index = new (nothrow) long long[size];
	Prev = Head;
	Curr = Head->Next;
	for(long long li=0;li<size;li++){
		index[li] = Curr->data;
		Prev = Curr;
		Curr = Curr->Next;
		delete Prev;
	}// End copying of index file

	// Gets the hashMultiplier
	dbFile.read((char*)&hashMultiplier, sizeof(hashMultiplier));
	//Determine the amount and names of individual types per record
	for(a='0';a!='\n';Typesize++){
		dbFile.getline(fieldname, fieldsize);
		Typeassc[string(fieldname)] = Typesize;
		a = dbFile.peek();
	}
	dbFile.ignore(1); // Ignore the \n char seperating the sections
	Typesize--;

	//Create the publically-accessable Typekeys object for the user to
	//view the keys of the map with
	Typekeys = new (nothrow) char*[Typesize];
	i = 0;
	for(maptype::iterator it=Typeassc.begin();it!=Typeassc.end();it++){
		Typekeys[i] = new (nothrow) char[(it->first).size()+1];
		for(j=0;j<(it->first).size();j++){
			Typekeys[i][j] = (it->first).at(j);
		}
		Typekeys[i][j] = '\0';
		i++;
	}

	//Create the Types object
	Types = new (nothrow) char *[Typesize];
	for(i=0;i<Typesize;i++){
		Types[i] = new (nothrow) char[2];
	}
}

/*--------------------------------------
 * Destructor
 * Deletes the Head node and all the
 * character arrays stored in the cache
--------------------------------------*/
dbread::~dbread(){
        if (Head == NULL ) {	// failure in opening or some such.
                return;
        }

	unsigned i;
	dbFile.close();
	hashFile.close();

	delete Head;		// @CTB do we need to delete linked list too?
	delete [] index;
	for(i=0;i<Typesize;i++){
		delete [] Types[i];
		delete [] Typekeys[i];
	}
	delete [] Typekeys;
	delete [] Types;
}

/*-------------------------------------------------
 * getRecord
 * Loads the record indexed by the long long
 * variable 'index' into memory from the database
-------------------------------------------------*/
void dbread::getRecord(long long idx){
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
	long long linelen;

	for(i=0;i<Typesize;i++){
		delete [] Types[i];
	}

	dbFile.seekg(index[idx]);
    if(dbFile.fail()){
        errmsg = "database does not extend to stream position " +
            string((char*)&(index[idx]));
        failbit = true;
        return;
    }
	// Read new records into the Types array
	a = '0';
	for(i=0;i<Typesize;i++){
		//Read the linelength integer in
		dbFile.read((char*)&linelen, sizeof(linelen));
		Types[i] = new (nothrow) char[linelen+1];
		dbFile.read(Types[i], linelen); // Read in the line data
		Types[i][linelen] = '\0'; // Set the NULL character at end
	}
	if(i != Typesize){ // i must equal Typesize when the loop exits
        errmsg = "Database files corrupted";
		failbit = true;
	}
	lastquery = idx;
}

/*-----------------------------------------------------
 * getType
 * Returns integer representation of data in the map
 * object Typeassc. Typeassc is set in the constructor
 * and provides the types which are accessable in the
 * database
 * @CTB this description seems incorrect...?
-----------------------------------------------------*/
char* dbread::getType(char wanted[]){
	unsigned i;
	i = Typeassc[string(wanted)];
	if(i == 0){
        errmsg = "Invalid typename query";
		failbit = true;
		return &(empty);
	}
	return Types[i-1];
}

/*-----------------------------------------------------
 * getTypeByIndex
 * Returns data from Types by direct indexing rather
 * by name as in getType.
-----------------------------------------------------*/
char* dbread::getTypeByIndex(unsigned idx){
        return getType(getTypekey(idx));
}

/*---------------------------------------------
 * getTypekey
 * Returns the c-string stored the in the
 * double-pointer variable Typekeys. First
 * though, it checks to make sure that the 
 * referenced index is a valid one. This
 * function can be thought of as the inverse
 * of getType
---------------------------------------------*/
char * dbread::getTypekey(unsigned idx){
    if((idx >= Typesize) || (idx < 0)){
        failbit = true;
        errmsg = "Bad typekey request";
        return &(empty);
    }
    return Typekeys[idx];
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
void dbread::getHashRecord(std::string RecordName){
    long long hashdResult, rcrdIdx;
    unsigned nameTypeint, collisions;
    nameTypeint = Typeassc["name"]-1;
    std::string test;
    hashdResult = hashFunct(RecordName, size*hashMultiplier);
    hashdResult = hashdResult * 8;
    collisions = 0;

    while(1){
        hashFile.seekg(hashdResult); // Go to the possible stream location
        hashFile.read((char*)&(rcrdIdx), 8);
        if(hashFile.eof()){
            failbit = true;
            errmsg = "No named record in database";
            return;
        }
        else if(hashFile.fail()){
            failbit = true;
            errmsg = "Corrupted hash file";
            return;
        }
        // Re-Correction for db writer necessary offset
        rcrdIdx = rcrdIdx - 1;
        getRecord(rcrdIdx);
        test = Types[nameTypeint];
        if(RecordName == test){ // Compare the retrieved name to the input one
            return;
        }
        collisions++;
        hashdResult = hashdResult + 8*(pow(2, collisions)-1);
    }
    return;
}

/*-----------------------------------------
 * hashFunct
 * Takes in a std::string type and a
 * long long type as arguments and computes
 * a long long hash value
-----------------------------------------*/
long long dbread::hashFunct(std::string toHash, long long hashSize){
    unsigned long long result, a, b;
    result = 0;
    a = 63689;
    b = 378551;
    for(unsigned i=0;i<toHash.size();i++){
        result = result*a + int(toHash[i]);
        a = a * b;
    }
    result = result % hashSize;
    return result;
}
