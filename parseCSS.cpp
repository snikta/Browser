#pragma once
#include <iostream>
#include <string>
#include <istream>
#include <fstream>
#include <map>
#include <vector>
#include "parser.h"
#include "whspace.h"
/* "parseCSS.cpp" */
using namespace std;

void parseCSS(string &filename)
{
	char chr;
	ifstream source(filename);
	while ((chr = source.get()) != EOF)
	{
		if (whspace(chr))
		{
			continue;
		}
		string selector;
		selector += chr;
		vector<string> selectorList;
		while ((chr = source.get()) != EOF && chr != '{')
		{
			if (whspace(chr) || chr == ',')
			{
				if (chr == ',') {
					selectorList.push_back(selector);
					selector = "";
				}
				continue;
			}
			selector += chr;
		}
		selectorList.push_back(selector);
		
		chr = source.get();
		string propertyName, propertyValue;
		bool parsingPropName;
		map<string,string> declaration;
		while (chr && chr != EOF && chr != '}')
		{
			if (whspace(chr)) {
				chr = source.get();
				continue;
			}
			propertyName = "";
			propertyValue = "";
			parsingPropName = true;
			do
			{
				if (whspace(chr))
				{
					continue;
				}
				if (parsingPropName)
				{
					do
					{
						propertyName += chr;
					} while ((chr = source.get()) != EOF && chr != ':');
					parsingPropName = false;
				}
				else
				{
					propertyValue += chr;
				}
			} while ((chr = source.get()) && chr != EOF && chr != ';');
			declaration[propertyName] = propertyValue;
			if (chr && chr != EOF && chr != '}')
			{
				chr = source.get();
			}
		}
		
		chr = source.get();

		for (int i = 0, len = selectorList.size(); i < len; i++)
		{
			string sel = selectorList[i];
			if (sel.substr(0, 1) == "." || sel.substr(0, 1) == "#") {
				sel = sel.substr(1);
			}
			map<string, string>::iterator it;
			for (it = declaration.begin(); it != declaration.end(); it++)
			{
				myParser.styles[sel][it->first] = it->second;
			}
		}
	}
}