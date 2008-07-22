#include "dbread.h"
#include <iostream>
#include <fstream>
#include <new>
#include <string>

using namespace std;

dbread::dbread(){
	dbread::Node * Curr;
	dbread::Node * Prev;
	string command, dblist, idxlist;
	fstream dir;
	unsigned fnamelen = 100;
	char a, dbname[fnamelen], idxname[fnamelen];
	int lsize = 1000;
	int i;
	char line[lsize];

	dblist = "dblist.txt";
	command = "ls *.seqdb2 > ";
	command.append(dblist);
	system(command.c_str());

	idxlist = "idxlist.txt";
	command = "ls *.indx > ";
	command.append(idxlist);
	system(command.c_str());

	dir.open(dblist.c_str(), fstream::in);
	dir.getline(dbname, fnamelen);
	dir.close();
	dir.open(idxlist.c_str(), fstream::in);
	dir.getline(idxname, fnamelen);
	dir.close();

	command = "rm ";
	command.append(dblist);
	command.append(" ");
	command.append(idxlist);
	system(command.c_str());

	Head = new (nothrow) Node;
	Head->Next = NULL;
	Prev = Head;
	idxFile.open(idxname, fstream::in);
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

	index = new (nothrow) unsigned[size];

	Prev = Head;
	Curr = Head->Next;
	for(unsigned i=0;i<size;i++){
		index[i] = Curr->data;
		Prev = Curr;
		Curr = Curr->Next;
		delete Prev;
	}

	dbFile.open(dbname, fstream::in);

	lastquery = 0;
	dbFile.seekg(index[lastquery]);
	dbFile.getline(line, lsize);
	for(i=0;line[i]!='\0';i++){}
	i++;
	name = new (nothrow) char[i];
	for(int j=0;j<i;j++){
		name[j] = line[j];
	}

	dbFile.getline(line, lsize);
	for(i=0;line[i]!='\0';i++){}
	i++;
	dna = new (nothrow) char[i];
	for(int j=0;j<i;j++){
		dna[j] = line[j];
	}

	dbFile.getline(line, lsize);
	for(i=0;line[i]!='\0';i++){}
	i++;
	accu = new (nothrow) char[i];
	for(int j=0;j<i;j++){
		accu[j] = line[j];
	}
}

dbread::~dbread(){
	dbFile.close();
	delete Head;
	delete [] index;
	delete [] name;
	delete [] dna;
	delete [] accu;
}

string dbread::query(unsigned idx, int type){
	int lsize = 1000;
	int i;
	char line[lsize];

	if(idx == lastquery){
		switch(type){
			case 0:
				return string(name);
			case 1:
				return string(dna);
			case 2:
				return string(accu);
		}
	}

	delete [] name;
	delete [] dna;
	delete [] accu;

	dbFile.seekg(index[idx]);
	dbFile.getline(line, lsize);
	for(i=0;line[i]!='\0';i++){}
	i++;
	name = new (nothrow) char[i];
	for(int j=0;j<i;j++){
		name[j] = line[j];
	}

	dbFile.getline(line, lsize);
	for(i=0;line[i]!='\0';i++){}
	i++;
	dna = new (nothrow) char[i];
	for(int j=0;j<i;j++){
		dna[j] = line[j];
	}

	dbFile.getline(line, lsize);
	for(i=0;line[i]!='\0';i++){}
	i++;
	accu = new (nothrow) char[i];
	for(int j=0;j<i;j++){
		accu[j] = line[j];
	}

	lastquery = idx;

	if(type == 0){
		return string(name);
	}
	else if(type == 1){
		return string(dna);
	}
	else if(type == 2){
		return string(accu);
	}

	//Gets rid of g++ warning of control reaching end of non-void function.
	//Execution should never reach this line
	string k;
	return k;
}
