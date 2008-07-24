#include <fstream>

const unsigned LINESIZE = 1000;

class dbwrite{
	private:
		std::fstream dbFile, idxFile;
	
	public:
		dbwrite(char[]);

		~dbwrite();

		void addrecord(char[], char[], char[], char[], int, int,
				int, long long);
};
