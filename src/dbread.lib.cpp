#include "dbread.h"
#include <iostream>
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

	idxname = dbname + ".idx";

	Head = new (nothrow) Node;
	Head->Next = NULL;
	Prev = Head;
	idxFile.open(idxname.c_str(), fstream::in);
	if(!idxFile.is_open()){
		cerr << "ERROR: UNABLE TO OPEN INDEX FILE. BAD FILENAME\n";
		exit(1);
	}
	//Copy the index file completly into memory so results can be retrieved
	//faster
	for(size=0;!idxFile.eof();size++){
		Curr = new (nothrow) Node;
		Prev->Next = Curr;
		Curr->Next = NULL;
		Prev = Curr;
		idxFile >> Curr->data;
		idxFile >> a;
		if(!idxFile.eof()){
			idxFile.putback(a);
		}
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
		cerr << "ERROR: UNABLE TO OPEN DATABASE FILE. BAD FILENAME\n";
		exit(1);
	}

	//Setup the caching of the 0th query. Caching ensures faster results
	//when the user is querying the same record again and again
	lastquery = 0;
	dbFile.seekg(index[lastquery]);
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

/*----------------------------------------
 * query
 * Accepts a long long int as index and
 * a long integer representing the type
 * of data to lookup in the record. If
 * the current query matches the previous
 * one, the data in the cache is returned.
 * If not, new data is read in first
----------------------------------------*/
string dbread::query(long long idx, int type){
	int i, j;
	char line[LSIZE];

	//If the user queried the same index they did last time, simply return
	//the cached record
	if(idx == lastquery){
		switch(type){
			case 1:
				return string(name);
			case 2:
				return string(desc);
			case 3:
				return string(accu);
			case 4:
				return string(dna);
			default:
				cerr << "ERROR: INVALID QUERY TYPE: " << type
					<< ". ENTER A QUERY 1-4\n";
				return "0";
		}
	}
	else if((idx > size) || (idx < 0)){
		cerr << "ERROR: QUERIED SIZE OF: " << idx << " OUTSIDE OF "<<
			"RANGE OF DATABASE: 0 - " << size << endl;
		return "0";

	}

	delete [] name;
	delete [] desc;
	delete [] accu;
	delete [] dna;

	//Rebuild the cache to hold a new record from the database
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

	switch(type){
		case 1:
			return string(name);
		case 2:
			return string(desc);
		case 3:
			return string(accu);
		case 4:
			return string(dna);
		default:
			cerr << "ERROR: INVALID QUERY TYPE: " << type
				<< ". ENTER A QUERY 1-4\n";
			return "0";
	}
	//Gets rid of g++ warning of control reaching end of non-void function.
	//Execution should never reach this line
	string k;
	return k;
}
