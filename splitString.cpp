#pragma once
#include <iostream>
#include <vector>
#include <string>

void splitString(std::string str, char delim, std::vector<std::string>& words)
{
	int i = 0, len = str.length();
	std::string el;
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