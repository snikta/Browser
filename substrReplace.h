#pragma once
#include <iostream>
#include <string>
using namespace std;

string substrReplace(string haystack, string needle, string replacement)
{
	int len1 = haystack.length(), len2 = needle.length();
	string newStr;
	for (int i = 0; i < len1; i++)
	{
		int j = 0;
		while ((i + j) < len1 && j < len2 && haystack[i + j] == needle[j])
		{
			j++;
		}
		if (j == 0)
		{
			newStr += haystack[i];
			j = 1;
		}
		else if (j < len2)
		{
			newStr += haystack.substr(i, j);
		}
		else
		{
			newStr += replacement;
		}
		i += j - 1;
	}
	return newStr;
}