#include "dbread.h"
#include <fstream>
#include <new>
#include <string>

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
	int i, j;
	char a, line[LSIZE];

	open = true;
	failbit = false;
	idxname = dbname + ".idx";
	lastquery = 1;

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
		idxFile.peek();
		/*idxFile >> a;
		if(!idxFile.eof()){
			idxFile.putback(a);
		}*/
	}
	idxFile.close();

	index = new (nothrow) long long[size];

	Prev = Head;
	Curr = Head->Next;
	for(long long i=0;i<size;i++){
		index[i] = Curr->data;
		Prev = Curr;
		Curr = Curr->Next;
		delete Prev;
	}// End copying of index file

	dbFile.open(dbname.c_str(), fstream::in);
	if(!dbFile.is_open()){
		open = false;
		return;
	}

	name = new (nothrow) char[2];
	accu = new (nothrow) char[2];
	desc = new (nothrow) char[2];
	dna = new (nothrow) char[2];
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
	delete [] name;
	delete [] desc;
	delete [] accu;
	delete [] dna;
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
	if((idx > size) || (idx < 0)){
		failbit = true;
		return;
	}
	else if(idx == lastquery){
		return;
	}

	char line[LSIZE];
	int i, j;

	delete [] name;
	delete [] desc;
	delete [] accu;
	delete [] dna;

	//Write the new records for the database into the cache
	dbFile.seekg(index[idx]);
	dbFile.getline(line, LSIZE);
	for(i=0;line[i]!='\0';i++){}
	i++;
	name = new (nothrow) char[i];
	for(j=0;j<i;j++){
		name[j] = line[j];
	}

	dbFile.getline(line, LSIZE);
	for(i=0;line[i]!='\0';i++){}
	i++;
	desc = new (nothrow) char[i];
	for(j=0;j<i;j++){
		desc[j] = line[j];
	}

	dbFile.getline(line, LSIZE);
	for(i=0;line[i]!='\0';i++){}
	i++;
	accu = new (nothrow) char[i];
	for(j=0;j<i;j++){
		accu[j] = line[j];
	}

	dbFile.getline(line, LSIZE);
	for(i=0;line[i]!='\0';i++){}
	i++;
	dna = new (nothrow) char[i];
	for(j=0;j<i;j++){
		dna[j] = line[j];
	}
	
	lastquery = idx;
}

/*------------------------------------------
 * getType
 * Returns referenced cached data. Mode is:
 * 1:Name, 2:description, 3:accuracy, 4:dna
-------------------------------------------*/
char* dbread::getType(unsigned type) const{
	switch(type){
		case 1:
			return name;
		case 2:
			return desc;
		case 3:
			return accu;
		case 4:
			return dna;
	}

	//If execution gets to here, the user typed an invalid type
	//failbit = true;
	//return "";
}
