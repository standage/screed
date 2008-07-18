#include <string>

class seqdb2{
	private:
		struct Node{
			char *name, *dna, *accu;
			seqdb2::Node *Next;
			short nmsiz, dnsiz, acsiz;
		};
		seqdb2::Node * Head;
		unsigned size;
		seqdb2::Node **index;

	public:
		seqdb2(char[]);
		~seqdb2();
		std::string getattr(int, int);
}; 
