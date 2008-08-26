#include <string>
#include <fstream>
#include <map>
#include <exception>

const char delimiter = '-';

class dbread{
	private:
		struct Node{
			long long data;
			Node * Next;
		};
		Node *Head;
		char **Typekeys;
		char **Types;

		typedef std::map<std::string, unsigned> maptype;
		maptype Typeassc;
		char empty;
		long long size, lastquery, dnalines;
		long long * index;
		unsigned Typesize;
		std::ifstream idxFile, dbFile;
		bool open, failbit;
	public:
		dbread(std::string);

		~dbread();

		void getRecord(long long=0);

		char* getType(char[]);

		bool is_open() const{return open;}

		bool fail() const{return failbit;}

		void clear();

		long long getSize() const{return size;}

		unsigned getTypesize() const{return Typesize;}

		char * getTypekey(unsigned i) const {return Typekeys[i];};
};

class dbread_exception: public std::exception {
 public:
  virtual const char * what() const throw()
    {
      return "dbread exception";
    }
};

class dbread_index_exception: public dbread_exception {
 public:
    virtual const char * what() const throw()
    {
      return "index out of range";
    }
};
