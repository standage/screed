// Copyright 2008-2009 Michigan State University. All rights reserved.

#ifndef DBREAD_
#define DBREAD_

#include <string>
#include <fstream>
#include <map>
#include <exception>
#include "tools.h"

class dbread{
	private:
		struct Node{
			index_type data;
			Node * Next;
		};
		Node *Head;
		char **Typekeys;
		char **Types;

		typedef std::map<std::string, unsigned> maptype;
		maptype Typeassc;
		char empty;
		index_type size, lastquery, dnalines;
		index_type * index;
		unsigned Typesize;
        index_type hashMultiplier; // Defaults to 2
		std::ifstream idxFile, dbFile, hashFile;
		bool open, failbit;
        std::string errmsg;
        
    public:

		dbread(std::string);

		~dbread();

        void close();

        void getHashRecord(char*, unsigned);

		void getRecord(index_type=0);

		char* getType(char[]);
		char* getTypeByIndex(unsigned);

		bool is_open() const{return open;}

		bool fail() const{return failbit;}

		void clear();

		index_type getSize() const{return size;}

		unsigned getTypesize() const{return Typesize;}

		char * getTypekey(unsigned);

        const char * theError() const {return errmsg.c_str();}
};

#endif // DBREAD_
