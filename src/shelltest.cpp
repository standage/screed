#include "dbread.h"
#include <string>
#include <assert.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[]){
	dbread db(argv[1]);
	string testcase(argv[4]);
	int index, type;

	index = atoi(argv[2]);
	type = atoi(argv[3]);

	cout << index << endl;
	cout << type << endl;
	cout << testcase << endl;

	assert(db.query(index, type) == testcase);

	return 0;
}
