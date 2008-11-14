#include <fstream>
#include <string>
#include <queue>
#include <list>
#include <fstream>

const char newline = '\n';

class dbwrite{
	private:
        std::queue<std::string, std::list<std::string> > Names4Hash;
		std::ofstream dbFile, idxFile;
        std::fstream hashFile;
        long long Recordlen; // The number of records read in
        unsigned hashMultiplier; // Defaults to 2X

		void writeTop(char);

		bool open, failbit;

        long long hashFunct(std::string, long long);
        long long quadProb(long long);

	public:
		dbwrite(std::string, char, unsigned = 2);

		~dbwrite();

		void close();

		bool fail() const {return failbit;}

        bool hash2Disk();

		bool is_open() const {return open;}

		bool writeFirst(std::string);

		bool writeLine(std::string);
};
