// Copyright 2008-2009 Michigan State University. All rights reserved.

#ifndef TOOLS_
#define TOOLS_

#ifndef INDEX_TYPE_
#define INDEX_TYPE_
typedef unsigned long long index_type;
#endif

#include <fstream>

using namespace std;

index_type hashFunct(char *, unsigned, index_type);
index_type gline(fstream&, char*, unsigned);
index_type pullthrough(fstream&, char*, unsigned);

#endif // TOOLS_
