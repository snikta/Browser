#pragma once
#include <string>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "whspace.h"
#include "parseCSS.h"
#include "splitString.h"
#include "parser.h"
/* "parseHTML.h" */

void parseHTML(DOMNode &node, DOMNode &parentNode, string &src, int start, int end, string tagName)
{
	int charIndex, attrIndex, attrLen, firstSpace;
	vector<string> classList;
	char chr, prevChr, attrChr;
	bool closing, newNodeSet;
	string openingTag, attributeSource, attrName, attrValue;

	node.set_zindex(parentNode.get_zindex() + 1);

	for (charIndex = start; charIndex < end; charIndex++)
	{
		chr = src[charIndex];

		newNodeSet = false;
		if (chr == '<')
		{
			DOMNode *newNode;
			if (node.get_text_content() != "")
			{
				newNode = new DOMNode("TextNode", node.get_text_content(), charIndex + 1, end, 0);
				newNode->set_idx(node.get_child_count());
				node.appendChild(*newNode);
				node.clear_text_content();
				newNode->set_parent_node(node);
			}
			newNode = new DOMNode("", "", charIndex + 1, end, 0);
			newNode->set_parent_node(node);
			newNodeSet = true;
			prevChr = chr;
			openingTag = "";
			chr = src[++charIndex];
			closing = false;
			while (charIndex < end && chr != '>')
			{
				if (prevChr == '<' && chr == '/')
				{
					closing = true;
					prevChr = chr;
					chr = src[++charIndex];
					continue;
				}
				openingTag += chr;
				prevChr = chr;
				chr = src[++charIndex];
			};
			int firstSpace = openingTag.find(" ");
			//if (!closing)
			//{
			//}
			if (firstSpace != string::npos)
			{
				attributeSource = openingTag.substr(firstSpace + 1);
				openingTag = openingTag.substr(0, firstSpace);
				for (attrIndex = 0, attrLen = attributeSource.length(); attrIndex < attrLen; attrIndex++)
				{
					attrName = "";
					attrValue = "";
					while ((attrChr = attributeSource[attrIndex]) != '"')
					{
						if (whspace(attrChr) || attrChr == '=')
						{
							attrIndex++;
							continue;
						}
						attrName += attrChr;
						attrIndex++;
					}
					attrIndex++;
					while ((attrChr = attributeSource[attrIndex]) != '"')
					{
						if (whspace(attrChr) && attrChr != ' ')
						{
							attrIndex++;
							continue;
						}
						attrValue += attrChr;
						attrIndex++;
					}
					newNode->attributes[attrName] = attrValue;
				}
			}

			if (closing)
			{
				if ((openingTag == tagName))
				{
					node.set_end(charIndex);
					node.clear_text_content();
					return;
				}
			}
			else {
				// CAREFUL!!!
				parseHTML(*newNode, node, src, charIndex + 1, end, openingTag);
				node.appendChild(*newNode);
				newNode->set_idx(node.get_child_count() - 1);
				newNode->set_tag_name(lcase(openingTag));

				elsByTagName[newNode->get_tag_name()].push_back(newNode);

				if (newNode->attributes.find("id") != newNode->attributes.end())
				{
					newNode->set_id(newNode->attributes["id"]);
					elsById[newNode->get_id()] = newNode;
				}

				if (newNode->attributes.find("class") != newNode->attributes.end())
				{
					splitString(newNode->attributes["class"], ' ', newNode->classList);
					for (auto className = classList.begin(); className != classList.end(); ++className)
					{
						elsByClassName[*className].push_back(newNode);
					}
				}

				charIndex = newNode->get_end();

				if (newNode->get_tag_name() == "link" && newNode->attributes["rel"] == "stylesheet") {
					myParser.cssFilename = newNode->attributes["href"];
				}
			}
		}
		else if (!(chr == '\r' || chr == '\n' || chr == '\t'))
		{
			node.append_text_content_char(chr);
		}
	}
}