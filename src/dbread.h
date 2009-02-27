// Copyright 2008-2009 Michigan State University. All rights reserved.

#include <string>
#include <fstream>
#include <map>
#include <exception>
#include "tools.h"

class dbread{
	private:
		struct Node{
			unsigned long long data;
			Node * Next;
		};
		Node *Head;
		char **Typekeys;
		char **Types;

		typedef std::map<std::string, unsigned> maptype;
		maptype Typeassc;
		char empty;
		unsigned long long size, lastquery, dnalines;
		unsigned long long * index;
		unsigned Typesize;
        int hashMultiplier; // Defaults to 2
		std::ifstream idxFile, dbFile, hashFile;
		bool open, failbit;
        std::string errmsg;
        
        bool cmpCstrs(char*, unsigned, const char*, unsigned);

    public:

		dbread(std::string);

		~dbread();

        void close();

        void getHashRecord(char*, unsigned);

		void getRecord(unsigned long long=0);

		char* getType(char[]);
		char* getTypeByIndex(unsigned);

		bool is_open() const{return open;}

		bool fail() const{return failbit;}

		void clear();

		unsigned long long getSize() const{return size;}

		unsigned getTypesize() const{return Typesize;}

		char * getTypekey(unsigned);

        const char * theError() const {return errmsg.c_str();}
};
