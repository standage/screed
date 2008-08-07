#include <fstream>
#include <string>

const char newline = '\n';
const char delim = '-';
const char space = ' ';

class dbwrite{
	private:
		std::ofstream dbFile, idxFile;

		void writeTop(char);

		bool open, failbit;
	
	public:
		dbwrite(std::string, char);

		~dbwrite();

		bool writeFirst();

		bool writeLine(std::string);

		bool writeDelim();

		bool is_open() const {return open;}

		bool fail() const {return failbit;}

		void close();
};
