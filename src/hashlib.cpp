#include "hashlib.h"

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
