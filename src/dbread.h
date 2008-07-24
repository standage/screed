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

		char *name, *desc, *accu, *dna;

	public:
		dbread(std::string);

		~dbread();

		std::string query(long long=0, int=1);		
};
