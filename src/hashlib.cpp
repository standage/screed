#include "hashlib.h"

using namespace std;

unsigned long long hashFunct(char* toHash, unsigned size,
        unsigned long long hashSize){
    unsigned long long result, a, b;
    result = 0;
    a = 63689;
    b = 378551;
    for(unsigned i=0;i<size;i++){
        result = result*a + int(toHash[i]);
        a = a * b;
    }
    result = result % hashSize;
    return result;
}
