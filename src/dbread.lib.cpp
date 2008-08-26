#include "dbread.h"
#include <fstream>
#include <new>
#include <string>
#include <iostream>

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
	string idxname;
	unsigned i, j;
	unsigned fieldsize = 100;
	char fieldname[fieldsize];
	char a;

	open = true;
	failbit = false;
	idxname = dbname + "_idx";
	lastquery = -19;
	empty = ' ';
	Typesize = 0;

	Head = new (nothrow) Node;
	Head->Next = NULL;
	Prev = Head;
	idxFile.open(idxname.c_str(), ios::in | ios::binary);
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
		idxFile.read((char*)&(Curr->data), sizeof(Curr->data));
		a = idxFile.peek();
		while(a != '\n'){ // Removes the newline character
			idxFile.get(a);
		}
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

	dbFile.open(dbname.c_str(), ios::in | ios::binary);
	if(!dbFile.is_open()){
		open = false;
		return;
	}

	//Determine the amount and names of individual types per record
	for(a='0';a!=delimiter;Typesize++){
		dbFile.getline(fieldname, fieldsize);
		Typeassc[string(fieldname)] = (Typesize+1);
		a = dbFile.peek();
	}

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
	unsigned i;
	dbFile.close();
	delete Head;
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
		failbit = true;
		return;
	}
	if((idx >= size) || (idx < 0)){
  	        throw dbread_index_exception();
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
		//Read the linelength integer in
		dbFile.read((char*)&linelen, sizeof(linelen));
		dbFile.ignore(1, ' '); // Ignore the space character
		Types[i] = new (nothrow) char[linelen];
		dbFile.read(Types[i], (linelen-1)); // Read in the line data
		Types[i][linelen-1] = '\0'; // Set the NULL character at end
		dbFile.ignore(1, '\n'); // Ignore the newline character
		a = dbFile.peek(); // Checks for delimiter character
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
char* dbread::getType(char wanted[]){
	unsigned i;
	i = Typeassc[string(wanted)];
	if(i == 0){
		failbit = true;
		return &(empty);
	}
	return Types[i-1];
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
