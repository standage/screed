// Copyright 2008-2009 Michigan State University. All rights reserved.

#ifndef TOOLS_
#define TOOLS_

#ifndef INDEX_TYPE_
#define INDEX_TYPE_
typedef unsigned long long index_type;
#endif

#include <fstream>

#ifdef __APPLE__
#include <machine/endian.h>
#else
#include <endian.h>
#endif

using namespace std;

index_type hashFunct(char *, unsigned, index_type);
index_type gline(fstream&, char*, unsigned);
index_type pullthrough(fstream&, char*, unsigned);

template <class int_type>
/*------------------------------------------
 * endian_swap
 * Takes an integer reference as an argument
 * and swaps it to the opposite endian type.
 * Returns nothing. Only does the swapping
 * if the machine is of Big Endian Type
-------------------------------------------*/
void endian_swap(int_type *toswap){
#ifdef __BYTE_ORDER
    #if __BYTE_ORDER == __LITTLE_ENDIAN
    return;
    #endif
#endif
    int_type result = 0;
    int_type size = sizeof(*toswap);
    int_type msk = 0xff;

    for(unsigned i=0;i<size/2;++i){
        int_type inmsk = msk << i*8;
        int_type blanked = inmsk & *toswap;
        blanked = blanked << (size - 2*i -1)*8;
        result = result | blanked;
    }

    for(unsigned i=0;i<size/2;++i){
        int_type inmsk = msk << (size -i -1)*8;
        int_type blanked = inmsk & *toswap;
        blanked = blanked >> (size - 2*i -1)*8;
        result = result | blanked;
    }
    *toswap = result;
}

#endif // TOOLS_
