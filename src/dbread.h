#include <string>
#include <fstream>

const int LSIZE = 1000;

class dbread{
	private:
		struct Node{
			unsigned data;
			Node * Next;
		};
		Node *Head;

		long long size, lastquery, dnalines;
		long long * index;
		std::fstream idxFile, dbFile;
		bool open, failbit;

	public:
		char *name, *desc, *accu, *dna;

		dbread(std::string);

		~dbread();

		void getRecord(long long=0);

		char* getType(unsigned=1);

		bool is_open() const{return open;}
		bool fail() const{return failbit;}

		void clear(){failbit = false;}
};
