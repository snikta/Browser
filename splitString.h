#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>

void splitString(string str, char delim, vector<string> &words)
{
	int i = 0, len = str.length();
	string el;
	while (i < len) {
		if (str[i] == delim) {
			words.push_back(el);
			el = "";
		}
		else {
			el += str[i];
		}
		i++;
	}
	words.push_back(el);
}