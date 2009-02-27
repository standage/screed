// Copyright 2008-2009 Michigan State University. All rights reserved.

#ifndef TOOLS_
#define TOOLS_
#include <fstream>

using namespace std;

unsigned long long hashFunct(char *, unsigned, unsigned long long);
unsigned long long gline(fstream&, char*, unsigned);
unsigned long long pullthrough(fstream&, char*, unsigned);

#endif // TOOLS_
