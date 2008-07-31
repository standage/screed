#include <fstream>
#include <string>

const unsigned LINESIZE = 1000;
const char delim = '-';

class dbwrite{
	private:
		std::fstream dbFile, idxFile;

		bool open;
	
	public:
		dbwrite(std::string);

		~dbwrite();

		bool writeFirst();

		bool writeLine(char[], long long);

		bool writeLine(const char[], long long);

		bool writeDelim();

		bool is_open() const {return open;}

		void close();
};
