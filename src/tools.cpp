// Copyright 2008-2009 Michigan State University. All rights reserved.

#include "tools.h"

using namespace std;

index_type hashFunct(char* toHash, unsigned size,
        index_type hashSize){
    index_type result;
    unsigned a, b;
    a = 3412;
    result = a;
    b = 5;
    for(unsigned i=0;i<size;i++){
        result = ((result << b) + result) ^
            static_cast<index_type>(toHash[i]);
    }
    result = result % hashSize;
    return result;
}

/*-------------------------------------------------
 * Reads a line from the file stream, storing the
 * characters in ary and returning the length
------------------------------------------------*/
index_type gline(fstream& F, char* ary, unsigned maxlen){
	index_type i;
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
index_type pullthrough(fstream& F, char* ary, unsigned maxlen){
    char temp;
    index_type i = 0;
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
