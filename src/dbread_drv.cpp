#include "dbread.h"
#include <iostream>
#include <stdlib.h>


using namespace std;

int main(int argc, char *argv[]){
	dbread db(argv[1]);
	long long index, i;
	char type[100], hi;
	string result;

	if(db.is_open() == false){
		cerr << "ERROR: DATABASE FILES NOT OPENED\n";
		exit(1);
	}
    cout << "Hashing or by index? (h/i)";
    cin >> hi;
	while(hi == 'i'){
		cout << "Enter an index location to retrieve (0-" <<
			(db.getSize()-1) << "): ";
		cin >> index;
		if(cin.eof()){
			cout << endl;
			break;
		}
		db.getRecord(index);
		if(db.fail() == true){
			cout << db.theError() << endl;
			db.clear();
			continue;
		}
		cout << "Possible types are:\n";
		for(i=0;i<db.getTypesize();i++){
			cout << db.getTypekey(i) << endl;
		}
		cout << "Enter one to retrieve: ";
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
        cout << "Enter a typekey index to retrieve: ";
        cin >> i;
        cout << db.getTypekey(i) << endl;
        if(db.fail() == true){
            cout << db.theError() << endl;
            db.clear();
        }
        continue;
	}
    while(hi == 'h'){
        cout << "Enter a record name to retrieve: ";
        cin >> type;
        if(cin.eof()){
            break;
        }
        db.getHashRecord(type);
        if(db.fail() == true){
            cerr << db.theError() << endl;
            db.clear();
            continue;
        }
		cout << "Possible types are:\n";
		for(i=0;i<db.getTypesize();i++){
			cout << db.getTypekey(i) << endl;
		}
        cout << "Enter a type to retrieve: ";
        cin >> type;
        cout << db.getType(type) << endl;
        if(db.fail() == true){
            cerr << db.theError() << endl;
            db.clear();
            continue;
        }
    }

	return 0;
}
