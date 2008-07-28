#include <fstream>
#include <string>

const unsigned LINESIZE = 1000;

class dbwrite{
	private:
		std::fstream dbFile, idxFile;

		bool open;
	
	public:
		dbwrite(std::string);

		~dbwrite();

		bool addrecord(char[], char[], char[], char[], int, int,
				int, long long);

		bool is_open() const {return open;}

		void close();
};
