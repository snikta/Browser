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
 
int getFileLength( string filename )
{
	ifstream is ( filename, ifstream::binary );
    is.seekg( 0, is.end );
    int len = is.tellg();
	is.seekg( 0, is.beg );
	is.close();
   
    return len;
}
 
int readTextFile( string fname, string &src )
{
	ifstream is ( fname );
	char chr;
	int i = 0;
    while ( ( chr = is.get() ) != EOF ) {
		src += chr;
		i++;
	};
	is.close( );
	return i;
}