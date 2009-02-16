// Copyright 2008-2009 Michigan State University. All rights reserved.

#include <fstream>
#include <string>
#include <queue>
#include <list>
#include <fstream>
#include <new>
#include "hashlib.h"

const char newline = '\n';
struct Node{
    char* data;
    unsigned len;
    Node * Next;
};

class dbwrite{
	private:
//        std::queue<std::string, std::list<std::string> > Names4Hash;
		std::ofstream dbFile, idxFile;
        std::fstream hashFile;
        unsigned long long Recordlen; // The number of records read in
        int hashMultiplier; // Defaults to 2X

		void writeTop(char);

		bool open, failbit;

        Node * Head;
        Node * Prev;
        Node * Curr;

	public:
		dbwrite(std::string, char, int = 2);

		~dbwrite();

		void close();

		bool fail() const {return failbit;}

        bool hash2Disk();

		bool is_open() const {return open;}

		bool writeFirst(char*, unsigned);

		bool writeLine(char *, unsigned long long);
};
