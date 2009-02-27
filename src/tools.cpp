// Copyright 2008-2009 Michigan State University. All rights reserved.

#include "tools.h"

using namespace std;

unsigned long long hashFunct(char* toHash, unsigned size,
        unsigned long long hashSize){
    unsigned long long result;
    unsigned a, b;
    a = 3412;
    result = a;
    b = 5;
    for(unsigned i=0;i<size;i++){
        result = ((result << b) + result) ^
            static_cast<unsigned long long>(toHash[i]);
    }
    result = result % hashSize;
    return result;
}

/*-------------------------------------------------
 * Reads a line from the file stream, storing the
 * characters in ary and returning the length
------------------------------------------------*/
unsigned long long gline(fstream& F, char* ary, unsigned maxlen){
	unsigned long long i;
	for(i=0;i<maxlen-1;++i){
		F.get(ary[i]);
		if(ary[i] == '\n'){
			break;
		}
	}
	ary[i] = '\0';
	return i;
}

/*-------------------------------------------------------------
 * Reads through spaces/tabs and pulls characters until another
 * patch of whitespace is found, returning the length of the
 * data read in
-------------------------------------------------------------*/
unsigned long long pullthrough(fstream& F, char* ary, unsigned maxlen){
    char temp;
    unsigned long long i = 0;
    bool first = false;
    while(i<maxlen-1){
        F.get(temp);
        if((temp == ' ') || (temp == '\n') || (temp == '\t') || (temp == '\r')){
            if(first == true){
                F.putback(temp);
                break;
            }
            continue;
        }
        first = true;
        ary[i] = temp;
        ++i;
    }
    ary[i] = '\0';
    return i;
}
