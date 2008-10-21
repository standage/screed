long long hashFunc(std::string, long long);

/*-----------------------------------------
 * hashFunct
 * Takes in a std::string type and a
 * long long type as arguments and computes
 * a long long hash value
-----------------------------------------*/
long long hashFunct(std::string toHash, long long hashSize){
    long long result;
    unsigned j;
    result = 0;
    for(unsigned i=0;i<(toHash.size()-2);i++){
        result += int(toHash[i]) * int(toHash[i+1]) * int(toHash[i+2]);
    }
    j = toHash.size()-1;
    for(unsigned i=0;i<j;i++){
        result += int(toHash[i]) * int(toHash[j]);
        j--;
    }
    result = result % hashSize;
    return result;
}

