#include "seqdb2.h"
#include <string>
#include <fstream>
#include <iostream>
#include <new>
#include <stdlib.h>

using namespace std;
seqdb2::seqdb2(char fn[]){
	seqdb2::Node *Prev, *Curr;
	fstream theFile, db, dbindx;
	filebuf *pbuf;
	string dbnam;
	int BUFSIZE = 256;
	unsigned i, j;
	short nmsiz, dnsiz, acsiz;
	char nambuf[BUFSIZE], dnabuf[BUFSIZE], accbuf[BUFSIZE];
	char a;
	char * delim = new (nothrow) char;
	char dbnamch[100];

	size = 0;
	Head = new (nothrow) Node;
	Head->Next = NULL;
	Prev = Head;

	pbuf = theFile.rdbuf();
	pbuf->open(fn, fstream::in);
	if(!(pbuf->is_open())){
		cerr << "ERROR: Bad Filename\n";
		exit(1);
	}
	istream it(pbuf);

	//Read through the file using the filebuffer object, it.
	//Read line by line and store the results in the internal db, both the
	//data and the pointers to the data
	while(!it.eof()){
		//Create the next Node
		Curr = new (nothrow) Node;
		Curr->Next = NULL;
		Prev->Next = Curr;
		Prev = Curr;

		//Extract the file data to the buffers
		it.getline(nambuf, BUFSIZE);
		it.getline(dnabuf, BUFSIZE);
		it.getline(accbuf, BUFSIZE);
		it.getline(accbuf, BUFSIZE);

		//Removes first @ character in the name
		for(i=0;nambuf[i] != '\0';i++){
			nambuf[i] = nambuf[i+1];
		}

		//Find length of buffers
		for(nmsiz=0;nambuf[nmsiz]!='\0';nmsiz++){}
		for(dnsiz=0;dnabuf[dnsiz]!='\0';dnsiz++){}
		for(acsiz=0;accbuf[acsiz]!='\0';acsiz++){}
		Curr->nmsiz = nmsiz;
		Curr->dnsiz = dnsiz;
		Curr->acsiz = acsiz;

		//Create data containers just large enough to hold the text
		try{
			Curr->name = new (nothrow) char[nmsiz+1];
			Curr->dna = new (nothrow) char[dnsiz+1];
			Curr->accu = new (nothrow) char[acsiz+1];
		}
		catch(std::bad_alloc){
			cerr << "\n*** UNABLE TO ALLOCATE MORE MEMORY ***\n";
			exit(1);
		}
		for(i=0;i<unsigned(nmsiz)+1;i++){
			Curr->name[i] = nambuf[i];
		}
		for(i=0;i<unsigned(dnsiz)+1;i++){
			Curr->dna[i] = dnabuf[i];
		}
		for(i=0;i<unsigned(acsiz)+1;i++){
			Curr->accu[i] = accbuf[i];
		}
		it.get(a);
		if(!it.eof()){
			it.putback(a);
		}

		size++;
	}

	j=0;
	for(i=0;fn[i]!='\0';i++){
		if(fn[i] == '/'){
			j = 0;
			continue;
		}
		else if(fn[i] == '.'){
			break;
		}
		dbnamch[j] = fn[i];
		j++;
	}
	if(j > 99){
		j=99;
	}
	dbnamch[j] = '\0';
	dbnam = dbnamch;

	dbnam.append(".seqdb2");
	db.open(dbnam.c_str(), fstream::out);
	dbnam.append(".indx");
	dbindx.open(dbnam.c_str(), fstream::out);
	
	Prev = Head;
	Curr = Head->Next;
	*delim = '\n';
	while(Curr != NULL){
		db.write(Curr->name, Curr->nmsiz);
		db.write(delim, 1);
		db.write(Curr->dna, Curr->dnsiz);
		db.write(delim, 1);
		db.write(Curr->accu, Curr->acsiz);
		db.write(delim, 1);
		Curr = Curr->Next;
	}

	db.close();
	dbindx.close();

	delete delim;
	index = new (nothrow) seqdb2::Node *[size];
	Prev = Head;
	Curr = Head->Next;
	i=0;
	while(Curr != NULL){
		index[i] = Curr;
		Curr = Curr->Next;
		i++;
	}

	pbuf->close();
}

seqdb2::~seqdb2(){
	seqdb2::Node *Curr, *Prev;
	Curr = Head->Next;
	while(Curr != NULL){
		Prev = Curr;
		Curr = Curr->Next;
		delete [] Prev->name;
		delete [] Prev->dna;
		delete [] Prev->accu;
		delete Prev;
	}

	delete Head;
	delete [] index;
	index = NULL;
	Head = NULL;
	size = 0;
}

string seqdb2::getattr(int ind, int type){
	std::string a;
	seqdb2::Node * Temp;
	
	Temp = index[ind];
	switch(type){
		case 1:
			a = Temp->name;
			break;
		case 2:
			a = Temp->dna;
			break;
		case 3:
			a = Temp->accu;
			break;
	}
	return a;
}
