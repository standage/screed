#include "dbread.h"
#include <ctime>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <new>

using namespace std;

/*-----------------------------------------------
 * Simple driver file to report the length of
 * time the reader takes to iterate through
 * a database.
 * Usage: ./timing <path_to_filename>
---------------------------------------------*/
int main(int argc, char *argv[]){
    clock_t start, finish;
    float time;
    char filename[50];

    if(argc != 2){
        cerr << "USAGE: ./timing <filename>\n";
        exit(1);
    }

    sprintf(filename, "%s_screed", argv[1]);
    dbread db(filename);
    char **keys;
    unsigned *lens;
    keys = new(nothrow) char *[db.getSize()];
    lens = new(nothrow) unsigned [db.getSize()];

    // Collect the keys
    for(index_type i=0;i<db.getSize();++i){
        db.getRecord(i);
        keys[i] = db.getAttributeValue("name");
        lens[i] = strlen(keys[i]);
    }

    // Run through db by hashing
    start = clock();
    for(index_type i=0;i<db.getSize();++i){
        db.getHashRecord(keys[i], lens[i]);
    }
    finish = clock();

    time = (float(finish)-float(start))/CLOCKS_PER_SEC;

    cout << time << " seconds\n";
    
    delete [] lens;
    delete [] keys;
    return 0;
}
