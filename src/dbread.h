#include <string>
#include <fstream>

class dbread{
	private:
		struct Node{
			unsigned data;
			Node * Next;
		};
		Node * Head;
		unsigned size, lastquery;
		unsigned * index;
		std::fstream idxFile, dbFile;

		char * name, * dna, * accu;

	public:
		dbread();

		~dbread();

		std::string query(unsigned=0, int=0);		


};
