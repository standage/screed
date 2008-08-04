#include <string>
#include <fstream>

const char delimiter = '-';

class dbread{
	private:
		struct Node{
			long long data;
			Node * Next;
		};
		Node *Head;

		char empty;
		long long size, lastquery, dnalines;
		long long * index;
		unsigned Typesize;
		std::ifstream idxFile, dbFile;
		bool open, failbit;
	public:
		char **Types;

		dbread(std::string);

		~dbread();

		void getRecord(long long=0);

		char* getType(unsigned=1);

		bool is_open() const{return open;}

		bool fail() const{return failbit;}

		void clear();

		long long getSize() const{return size;}

		unsigned getTypesize() const{return Typesize;}
};
