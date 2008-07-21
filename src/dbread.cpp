#include <iostream>
#include <fstream>
#include <new>

using namespace std;

struct Node{
	unsigned data;
	Node * Next;
};
Node * Head;
unsigned size;

int main(int argc, char *argv[] ){
	fstream idxFile, dbFile;
	Node * Curr;
	Node * Prev;
	char a;
	char name[100], dna[100], accu[100];
	unsigned * index;
	unsigned query;

	Head = new (nothrow) Node;
	Head->Next = NULL;
	Prev = Head;
	idxFile.open(argv[1], fstream::in);
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

	dbFile.open(argv[2], fstream::in);
	while(1){
		cout << "Enter an index to query (1-" << size << "): ";
		cin >> query;
		if(cin.eof()){
			cout << endl;
			break;
		}
		dbFile.seekg(index[query]);
		dbFile.getline(name, 100);
		dbFile.getline(dna, 100);
		dbFile.getline(accu, 100);
		
		cout << "Name: " << name << endl;
		cout << "DNA: " << dna << endl;
		cout << "Accuracy: " << accu << endl << endl;
	}

	dbFile.close();
	delete Head;
	delete [] index;

	return 0;
}
