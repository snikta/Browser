#pragma once
#include <iostream>
#include <string>
#include "parser.h"
using namespace std;

class cssStream {
	string src;
	int pos;
	int length;

public:

	cssStream(string url);
	char get(int readlen);
};
void parseCSS(string &filename);