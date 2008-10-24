#include "dbread.h"
#include <string>
#include <assert.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[]){
	dbread db(argv[1]);
	string testcase(argv[4]);
	int index;

	index = atoi(argv[2]);

	db.getRecord(index);
	assert(string(db.getType(argv[3])) == testcase);

	return 0;
}
