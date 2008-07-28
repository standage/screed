#include "dbread.h"
#include <iostream>
#include <stdlib.h>


using namespace std;

int main(int argc, char *argv[]){
	dbread db(argv[1]);
	long long index, type;
	string result;

	if(db.is_open() == false){
		cerr << "ERROR: DATABASE FILES NOT OPENED\n";
		exit(1);
	}
	while(1){
		cout << "Enter a query: ";
		cin >> index >> type;
		if(cin.eof()){
			cout << endl;
			break;
		}
		cout << db.query(index, type) << endl;
		if(db.fail() == true){
			cerr << "ERROR: BAD QUERY\n";
			db.clear();
		}
	}

	return 0;
}
