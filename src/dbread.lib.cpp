#include "dbread.h"
#include <fstream>
#include <new>
#include <string>
#include <iostream>

using namespace std;

/*----------------------------------------
 * Constructor
 * Takes a string file name and opens
 * the correct .seqdb2 and .idx files.
 * Also reads the first record into cache
----------------------------------------*/
dbread::dbread(string dbname){
	dbread::Node *Curr;
	dbread::Node *Prev;
	string idxname;
	char a;
	unsigned i;

	open = true;
	failbit = false;
	idxname = dbname + ".idx";
	lastquery = 1;
	empty = ' ';

	Head = new (nothrow) Node;
	Head->Next = NULL;
	Prev = Head;
	idxFile.open(idxname.c_str(), fstream::in);
	if(!idxFile.is_open()){
		open = false;
		return;
	}
	//Copy the index file completly into memory so results can be retrieved
	//faster
	for(size=0;!idxFile.eof();size++){
		Curr = new (nothrow) Node;
		Prev->Next = Curr;
		Curr->Next = NULL;
		Prev = Curr;
		idxFile >> Curr->data;
		idxFile.ignore(1); // Ignores the newline character
		idxFile.peek();
	}
	idxFile.close();

	index = new (nothrow) long long[size];

	Prev = Head;
	Curr = Head->Next;
	for(long long li=0;li<size;li++){
		index[li] = Curr->data;
		Prev = Curr;
		Curr = Curr->Next;
		delete Prev;
	}// End copying of index file

	dbFile.open(dbname.c_str(), fstream::in);
	if(!dbFile.is_open()){
		open = false;
		return;
	}

	//Determine the amount of individual types per record
	for(Typesize=0;a!=delimiter;Typesize++){
		while(1){
			dbFile.get(a);
			if(a == '\n'){ // Each type is newline seperated
				break;
			}
		}
		a = dbFile.peek();
	}
	dbFile.seekg(0);

	//Create the Types object
	Types = new (nothrow) char *[Typesize];
	for(i=0;i<Typesize;i++){
		Types[i] = new (nothrow) char[2];
	}

	//Setup the caching of the 0th query. Caching ensures faster results
	//when the user is querying the same record again and again
	getRecord(0);
}

/*--------------------------------------
 * Destructor
 * Deletes the Head node and all the
 * character arrays stored in the cache
--------------------------------------*/
dbread::~dbread(){
	dbFile.close();
	delete Head;
	delete [] index;
	for(unsigned i=0;i<Typesize;i++){
		delete [] Types[i];
	}
	delete [] Types;
}

/*-------------------------------------------------
 * getRecord
 * Loads the record indexed by the long long
 * variable 'index' into memory from the database
-------------------------------------------------*/
void dbread::getRecord(long long idx){

	if(open == false){
		failbit = true;
		return;
	}
	if((idx >= size) || (idx < 0)){
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
	//Read new records into the Types array
	a = '0';
	for(i=0;a!=delimiter;i++){
		if(i == Typesize){
			failbit = true;
			return;
		}
		dbFile >> linelen >> ws;
		Types[i] = new (nothrow) char[linelen];
		dbFile.getline(Types[i], linelen);
		a = dbFile.peek();
	}
	if(i != Typesize){ // i must equal Typesize when the loop exits
		failbit = true;
	}
	lastquery = idx;
}

/*------------------------------------------
 * getType
 * Returns referenced cached data
-------------------------------------------*/
char* dbread::getType(unsigned wanted){
	if((wanted < 0) || (wanted >= Typesize)){
		failbit = true;
		return &(empty);
	}
	return Types[wanted];
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
}
