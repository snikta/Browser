#pragma once
/* "readTextFile.h"
 *
 * Usage:

	string fname = "someFile.txt";
	int len = getFileLength( fname, is );
	char src[ len ];
	char *ptrSrc = src;

	cout << readTextFile( fname, len, is, ptrSrc ) << endl;
 */
 
#include <iostream>
#include <fstream>
 
using namespace std;
 
int getFileLength(string filename);
int readTextFile(string fname, string& src);