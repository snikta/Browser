#pragma once
/* "parser.h" / (c) 2018 Snikta(tm) / Josh Atkins */

#include <iostream>
#include <string>
#include <string.h>
#include <map>
#include <vector>
#include <utility>
#include "DOMNode.h"

using namespace std;

class Parser
{
private:
	string location = "C:\\c++\\Browser\\layout.html";
public:
	bool set = true;
	DOMNode *rootNode;
	string output;
	string cssFilename;
	map<string, map<string, string>> styles;
	//vector<DOMNode*> selectedChildNodes;
	void set_location(string newLocation);
	string get_location();
	string printElementAttributes(DOMNode& node);
	string printChildTagNames(DOMNode& node, int level, bool last);
};
extern Parser myParser;