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
	string printElementAttributes(DOMNode &node)
	{
		map<string, string> *ptrAttributes = &(node.attributes);
		vector<string> *classList = &(node.classList);
		//string str = to_string(node.get_idx())+")"+node.get_tag_name() + ":" + node.get_text_content() + ";";
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

		string str;
		/*for (auto beg = node.style.begin(), end = node.style.end(); beg != end; ++beg)
		{
			str += "[" + beg->first + "] = " + beg->second;// +"\n";
		}*/

		if (node.get_tag_name() != "root" && node.get_tag_name() != "TextNode")
		{
			return "<" + node.get_tag_name() + ":" + to_string(node.get_idx()) +">\n";
		}
		else
		{
			return "";
		}

		/*if (node.get_tag_name() == "div") {
			return "<" + node.get_tag_name() + (node.get_id() == "" ? "" : " id=" + '"' + node.get_id() + '"') + ">\n";
			//return node.get_tag_name() + to_string(node.get_idx()) + '=' + node.get_id() + str + ',' + node.style["display"] + '>' + to_string((int)node.x) + ',' + to_string((int)node.y) + ',' + to_string((int)node.width) + ',' + to_string((int)node.height) + '\n';
		}
		else if (node.get_tag_name() != "root" && node.get_tag_name() != "TextNode") {
			//return node.get_tag_name() + to_string(node.get_idx()) + '=' + node.get_id() + str + ',' + node.style["display"] + '>' + node.style["font-weight"] + ";" + node.style["font-style"] + to_string((int)node.x) + ',' + to_string((int)node.y) + ',' + to_string((int)node.width) + ',' + to_string((int)node.height) + '\n';
			return "<" + node.get_tag_name() + (node.get_id() == "" ? "" : " id=" + '"' + node.get_id() + '"') + ">\n";
		}
		else
		{
			return "";
		}*/
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
		tagTree += printElementAttributes(node);
		if (node.get_child_count())
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