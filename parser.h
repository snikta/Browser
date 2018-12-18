#pragma once
/* "parser.h" / (c) 2018 Snikta(tm) / Josh Atkins */

#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
//#include <istream>
//#include <fstream>
#include <map>
#include <vector>
#include <utility>
//#include "splitString.h"
#include "whspace.h"
#include "lcase.h"
#include "DOMNode.h"
#include "readTextFile.h"

class Parser
{
public:
	DOMNode *rootNode;
	string output;
	string cssFilename;
	map<string, map<string, string>> styles;
	//vector<DOMNode*> selectedChildNodes;
	string printElementAttributes(DOMNode &node)
	{
		map<string, string> *ptrAttributes = &(node.attributes);
		vector<string> *classList = &(node.classList);
		string str;
		/*for (auto className = classList->begin(); className != classList->end(); ++className)
		{
			str += "[" + *className + "],";
			if (styles.find(*className) != styles.end())
			{
				str += *className + ",";
				map<string, string> *elStyles = &(styles[*className]);
				map<string, string>::iterator it;
				for (it = elStyles->begin(); it != elStyles->end(); ++it) {
					node.style[it->first] = it->second;
					str += it->first + " " + it->second + ";";
				}
			}
		}*/

		str += "(" + to_string((int) node.x) + ", " + to_string((int) node.y) + ") -> (" + to_string((int) (node.x + node.width)) + ", " + to_string(int (node.y + node.height)) + ")\n";

		if (node.get_tag_name() == "a") {
			str += "href = " + node.attributes["href"] + "\n";
		}

		for (auto beg = node.style.begin(), end = node.style.end(); beg != end; ++beg)
		{
			if (beg->second != "")
			{
				str += "[" + beg->first + "] = " + beg->second + "\n";
			}
		}

		return str;
	};
	string printChildTagNames(DOMNode &node, int level, bool last)
	{
		string tagTree;
		if (node.get_child_count() && node.get_tag_name() != "root" && node.get_tag_name() != "html")
		{
			for (int i = 0; i < level; i++)
			{
				tagTree += "  ";
			}
		}
		if (node.get_tag_name() != "root" && node.get_tag_name() != "TextNode")
		{
			tagTree += "<" + node.get_tag_name() + ">\n";
		}
		if (node.get_tag_name() != "head" && node.expand && node.get_child_count())
		{
			DOMNode *curChild;
			curChild = node.firstChild;
			int i = 0, len = node.get_child_count();
			string newPath;
			while (i < len)
			{
				tagTree += printChildTagNames(*curChild, level + 1, i == len - 1);
				curChild = curChild->nextSibling;
				i++;
			}
		}
		return tagTree;
	};
};
Parser myParser;