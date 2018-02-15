/* "lcase.h" */

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

string lcase( string str )
{
	string newStr;
	for ( int i = 0, len = str.length( ); i < len; i++ )
	{
		newStr += tolower( str[ i ] );
	}
	return newStr;
}