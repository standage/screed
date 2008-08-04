#include <fstream>
#include <string>

const char delim = '-';

class dbwrite{
	private:
		std::fstream dbFile, idxFile;

		bool open;
	
	public:
		dbwrite(std::string);

		~dbwrite();

		bool writeFirst();

		bool writeLine(std::string);

		bool writeDelim();

		bool is_open() const {return open;}

		void close();
};
