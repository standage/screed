#include "lib/seqdb2.h"
#include <assert.h>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char * argv[]){
  char theFile[] = "tests/test.fastq";

  seqdb2 db(theFile);

  assert(db.getattr(1, 1) == "HWI-EAS_4_PE-FC20GCB:1:1:62:914/1");
  assert(db.getattr(1, 2) == "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
  assert(db.getattr(1, 3) == "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAADADAAD");
  
  return 0;
}
