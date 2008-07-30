#include "dbread.h"
#include <iostream>
#include <stdlib.h>


using namespace std;

int main(int argc, char *argv[]){
	dbread db(argv[1]);
	long long index;
	unsigned type;
	string result;

	if(db.is_open() == false){
		cerr << "ERROR: DATABASE FILES NOT OPENED\n";
		exit(1);
	}
	while(1){
		cout << "Enter an index location to retrieve (0-" <<
			(db.getSize()-1) << "): ";
		cin >> index;
		if(cin.eof()){
			cout << endl;
			break;
		}
		db.getRecord(index);
		if(db.fail() == true){
			cerr << "ERROR: BAD INDEX\n";
			db.clear();
			continue;
		}
		cout << "Enter a type, 0-" << (db.getTypesize()-1) <<
			" , to retrieve: ";
		cin >> type;
		if(cin.eof()){
			cout << endl;
			break;
		}
		cout << db.getType(type) << endl;
		if(db.fail() == true){
			cerr << "ERROR: BAD TYPE\n";
			db.clear();
			continue;
		}

	}

	return 0;
}
