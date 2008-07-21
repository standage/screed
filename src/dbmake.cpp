#include <string>
#include <fstream>
#include <iostream>
#include <new>
#include <stdlib.h>

using namespace std;
struct Node{
	char *name, *dna, *accu;
	Node *Next;
	short nmsiz, dnsiz, acsiz;
};
Node * Head;
unsigned size;

int main(int argc, char *argv[]){
	Node *Prev, *Curr;
	fstream theFile, db, dbindx;
	filebuf *pbuf;
	string dbnam;
	int BUFSIZE = 256;
	unsigned i, j, idxpos;
	short nmsiz, dnsiz, acsiz;
	char nambuf[BUFSIZE], dnabuf[BUFSIZE], accbuf[BUFSIZE];
	char a;
	char * delim = new (nothrow) char;
	char dbnamch[100];

	*delim = '\n';
	size = 0;
	Head = new (nothrow) Node;
	Head->Next = NULL;
	Prev = Head;

	pbuf = theFile.rdbuf();
	pbuf->open(argv[1], fstream::in);
	if(!(pbuf->is_open())){
		cerr << "ERROR: Bad Filename\n";
		exit(1);
	}
	istream it(pbuf);

	//Create the file name from the .fastq filename
	j=0;
	for(i=0;argv[1][i]!='\0';i++){
		if(argv[1][i] == '/'){
			j = 0;
			continue;
		}
		else if(argv[1][i] == '.'){
			break;
		}
		dbnamch[j] = argv[1][i];
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
	idxpos = 0;

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

		//Skip lines that are too small
		if(nmsiz < 10){
			continue;
		}
		else if(dnsiz < 10){
			continue;
		}
		else if(acsiz < 10){
			continue;
		}
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

		//Write the data in memory to disk, deleting nodes along the
		//way
		if(size/1500000 == 1){
			Prev = Head;
			Curr = Head->Next;
			while(Curr != NULL){
				dbindx << idxpos << '\n';
				db.write(Curr->name, Curr->nmsiz);
				db.write(delim, 1);
				db.write(Curr->dna, Curr->dnsiz);
				db.write(delim, 1);
				db.write(Curr->accu, Curr->acsiz);
				db.write(delim, 1);
				idxpos = idxpos + Curr->nmsiz + Curr->dnsiz
					+ Curr->acsiz + 3;
				Prev = Curr;
				Curr = Curr->Next;
				delete [] Prev->name;
				delete [] Prev->dna;
				delete [] Prev->accu;
				delete Prev;
			}
			Prev = Head;
			size = 0;
		}
	}

	//Do final write to files
	Prev = Head;
	Curr = Head->Next;
	while(Curr != NULL){
		dbindx << idxpos << '\n';
		db.write(Curr->name, Curr->nmsiz);
		db.write(delim, 1);
		db.write(Curr->dna, Curr->dnsiz);
		db.write(delim, 1);
		db.write(Curr->accu, Curr->acsiz);
		db.write(delim, 1);
		idxpos = idxpos + Curr->nmsiz + Curr->dnsiz
			+ Curr->acsiz + 3;
		Prev = Curr;
		Curr = Curr->Next;
		delete [] Prev->name;
		delete [] Prev->dna;
		delete [] Prev->accu;
		delete Prev;
	}
	//Head->Next = Curr;
	delete Head;
	size = 0;

	db.close();
	dbindx.close();
	delete delim;
	pbuf->close();

	return 0;
}
