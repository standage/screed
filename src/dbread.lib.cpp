#include "dbread.h"
#include <iostream>
#include <fstream>
#include <new>
#include <string>

using namespace std;

dbread::dbread(){
	dbread::Node *Curr;
	dbread::Node *Prev;
	string command, dblist, idxlist;
	fstream dir;
	unsigned fnamelen = 100;
	char a, dbname[fnamelen], idxname[fnamelen];
	int lsize = 1000;
	int i, j;
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

	index = new (nothrow) long long[size];

	Prev = Head;
	Curr = Head->Next;
	for(long long i=0;i<size;i++){
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
	for(j=0;j<i;j++){
		name[j] = line[j];
	}
	
	dbFile.getline(line, lsize);
	for(i=0;line[i]!='\0';i++){}
	i++;
	desc = new (nothrow) char[i];
	for(j=0;j<i;j++){
		desc[j] = line[j];
	}

	dbFile.getline(line, lsize);
	for(i=0;line[i]!='\0';i++){}
	i++;
	accu = new (nothrow) char[i];
	for(j=0;j<i;j++){
		accu[j] = line[j];
	}

	//Get the number of lines of the DNA strands
	dbFile >> dnalines;
	dbFile.ignore(1);
	dna = new (nothrow) char *[dnalines];

	//The lines of DNA can vary, ergo the loop
	for(i=0;i<dnalines;i++){
		dbFile.getline(line, lsize);
		for(j=0;line[j]!='\0';j++){}
		j++;
		dna[i] = new (nothrow) char[j];
		for(int p=0;p<j;p++){
			dna[i][p] = line[p];
		}
	}
}

dbread::~dbread(){
	dbFile.close();
	delete Head;
	delete [] index;
	delete [] name;
	delete [] desc;
	delete [] accu;
	for(int i=0;i<dnalines;i++){
		delete [] dna[i];
	}
	delete [] dna;
}

string dbread::query(long long idx, int type){
	int lsize = 1000;
	int i, j;
	char line[lsize];

	if(idx == lastquery){
		switch(type){
			case 1:
				return string(name);
			case 2:
				return string(desc);
			case 3:
				return string(accu);
			case 4:
				return string(dna[0]);
		}
	}

	delete [] name;
	delete [] desc;
	delete [] accu;
	for(i=0;i<dnalines;i++){
		delete [] dna[i];
	}
	delete [] dna;

	dbFile.seekg(index[idx]);
	dbFile.getline(line, lsize);
	for(i=0;line[i]!='\0';i++){}
	i++;
	name = new (nothrow) char[i];
	for(j=0;j<i;j++){
		name[j] = line[j];
	}

	dbFile.getline(line, lsize);
	for(i=0;line[i]!='\0';i++){}
	i++;
	desc = new (nothrow) char[i];
	for(j=0;j<i;j++){
		desc[j] = line[j];
	}

	dbFile.getline(line, lsize);
	for(i=0;line[i]!='\0';i++){}
	i++;
	accu = new (nothrow) char[i];
	for(j=0;j<i;j++){
		accu[j] = line[j];
	}

	dbFile >> dnalines;
	dbFile.ignore(1);
	dna = new (nothrow) char *[dnalines];

	for(i=0;i<dnalines;i++){
		dbFile.getline(line, lsize);
		for(j=0;line[j]!='\0';j++){}
		j++;
		dna[i] = new (nothrow) char[j];
		for(int p=0;p<j;p++){
			dna[i][p] = line[p];
		}
	}

	lastquery = idx;

	if(type == 1){
		return string(name);
	}
	else if(type == 2){
		return string(desc);
	}
	else if(type == 3){
		return string(accu);
	}
	else if(type == 4){
		return string(dna[0]);
	}

	//Gets rid of g++ warning of control reaching end of non-void function.
	//Execution should never reach this line
	string k;
	return k;
}
