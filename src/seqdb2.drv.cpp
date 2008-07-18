#include "lib/seqdb2.h"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char * argv[]){
//  char * theFile = argv[1];
	char theFile[] = "/home/alex/Desktop/s_1_1_sequence.fastq";
	int ind, attr;
	seqdb2 db(theFile);

	while(!cin.eof()){
		cout << "Enter an index and type to lookup (1=name, 2="
			<< "strand, 3=accuracy)\n";
		cin >> ind;
		cin >> attr;
		cout << db.getattr(ind, attr) << endl;
	}


	return 0;
}
