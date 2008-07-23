#include <string>
#include <fstream>
#include <iostream>
#include <new>
#include <stdlib.h>

using namespace std;

struct Dna{
	char *strand;
	unsigned dnsiz;
	Dna *dnNext;
};
struct Node{
	char *name, *desc;
	Dna *dnHead;
	Node *Next;
	unsigned nmsiz, desiz, dnalen;
	long long dnalines;
};

Node * Head;
long long size;

int main(int argc, char *argv[]){
	Node *Prev, *Curr;
	Dna *dnPrev, *dnCurr;
	fstream theFile, db, dbindx;
	filebuf *pbuf;
	string dbnam;
	int BUFSIZE = 1000;
	unsigned i, j, nmsiz, dnsiz, desiz, dnareads;
	long long idxpos, streamloc;
	char nambuf[BUFSIZE], dnabuf[BUFSIZE], desbuf[BUFSIZE];
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

	//Create the file name from the .fasta filename
	j=0;
	for(i=0;argv[1][i]!='\0';i++){
		if(argv[1][i] == '/'){
			j = 0;
			continue;
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
	dnareads = 0;

	//Read through the file using the filebuffer object, it.
	//Read line by line and store the results in the internal db, both the
	//data and the pointers to the data
	while(!it.eof()){
		//Create the next Node
		Curr = new (nothrow) Node;
		Curr->Next = NULL;
		Prev->Next = Curr;
		Prev = Curr;
		Curr->dnHead = new (nothrow) Dna;
		dnPrev = Curr->dnHead;
		Curr->dnalen = 0;
		Curr->dnalines = 0;

		//Skips the > in front of the name
		streamloc = it.tellg();
		it.seekg(streamloc+1);

		//Extract the dna file data into the buffers
		it >> nambuf;
		it >> ws;
		it.getline(desbuf, BUFSIZE);

		//Pulls all the lines of dna strands into their respective
		//Dna structs
		while(!it.eof()){
			dnCurr = new (nothrow) Dna;
			dnCurr->dnNext = NULL;
			dnPrev->dnNext = dnCurr;
			dnPrev = dnCurr;

			it.getline(dnabuf, BUFSIZE);
			for(dnsiz=0;dnabuf[dnsiz]!='\0';dnsiz++){}
			dnsiz++;
			try{ // Create a data container for the DNA strand
				dnCurr->strand = new (nothrow) char[dnsiz];
			}
			catch(std::bad_alloc){
				cerr << "\n***UNABLE TO ALLOCATE MEMORY***\n";
				exit(1);
			}

			// Copy the buffer into the data element
			for(i=0;i<dnsiz;i++){
				dnCurr->strand[i] = dnabuf[i];
			}
			dnCurr->dnsiz = dnsiz;
			Curr->dnalen++;
			dnareads++;
			(Curr->dnalines)++;
			
			it.get(a);
			if(a == '>'){
				it.putback(a);
				break;
			}
			it.putback(a);
		}

		//Find length of buffers
		for(nmsiz=0;nambuf[nmsiz]!='\0';nmsiz++){}
		for(desiz=0;desbuf[desiz]!='\0';desiz++){}
		nmsiz++;
		desiz++;

		Curr->nmsiz = nmsiz;
		Curr->desiz = desiz;

		//Create data containers just large enough to hold the text
		try{
			Curr->name = new (nothrow) char[nmsiz];
			Curr->desc = new (nothrow) char[desiz];
		}
		catch(std::bad_alloc){
			cerr << "\n*** UNABLE TO ALLOCATE MORE MEMORY ***\n";
			exit(1);
		}
		for(i=0;i<nmsiz;i++){
			Curr->name[i] = nambuf[i];
		}
		for(i=0;i<desiz;i++){
			Curr->desc[i] = desbuf[i];
		}
		size++;

		//Write the data in memory to disk, deleting nodes along the
		//way
		if((size >= 1500000) || (dnareads >= 2000000)){
			Prev = Head;
			Curr = Head->Next;
			while(Curr != NULL){
				dnPrev = Curr->dnHead;
				dnCurr = dnPrev->dnNext;
				dbindx << idxpos << '\n';
				db.write(Curr->name, Curr->nmsiz-1);
				db.write(delim, 1);
				db.write(Curr->desc, Curr->desiz-1);
				db.write(delim, 1);
				//Empty space for accuracy
				db.write(delim, 1);
				db << Curr->dnalines << endl;
				//Write the dna strand data, line by line
				while(dnCurr != NULL){
					db.write(dnCurr->strand,
							dnCurr->dnsiz-1);
					db.write(delim, 1);
					dnPrev = dnCurr;
					dnCurr = dnCurr->dnNext;
					delete [] dnPrev->strand;
					delete dnPrev;
				}
				db << '-' << endl;
				idxpos = db.tellp();
				Prev = Curr;
				Curr = Curr->Next;
				delete [] Prev->name;
				delete [] Prev->desc;
				delete Prev->dnHead;
				delete Prev;
			}
			Prev = Head;
			Prev->Next = NULL;
			size = 0;
			dnareads = 0;
		}
	}

	//Do final write to files
	Prev = Head;
	Curr = Head->Next;
	while(Curr != NULL){
		dnPrev = Curr->dnHead;
		dnCurr = dnPrev->dnNext;
		dbindx << idxpos << '\n';
		db.write(Curr->name, Curr->nmsiz-1);
		db.write(delim, 1);
		db.write(Curr->desc, Curr->desiz-1);
		db.write(delim, 1);
		// Empty space for accuracy
		db.write(delim, 1);
		db << Curr->dnalines << endl;
		// Write the dna strand data, line by line
		while(dnCurr != NULL){
			db.write(dnCurr->strand, dnCurr->dnsiz-1);
			db.write(delim, 1);
			dnPrev = dnCurr;
			dnCurr = dnCurr->dnNext;
			delete [] dnPrev->strand;
			delete dnPrev;
		}
		db << '-' << endl;
		idxpos = db.tellp();
		Prev = Curr;
		Curr = Curr->Next;
		delete [] Prev->name;
		delete [] Prev->desc;
		delete Prev->dnHead;
		delete Prev;
	}
	delete Head;
	size = 0;

	db.close();
	dbindx.close();
	delete delim;
	pbuf->close();

	return 0;
}
