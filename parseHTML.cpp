#pragma once
#include <string>
#include <vector>
#include "DOMNode.h"
#include "whspace.h"
#include "splitString.h"
#include "lcase.h"
#include "parser.h"
#include "JSExec.h"
#include "readTextFile.h"
/* "parseHTML.cpp" */
using namespace std;

string tagHTML(DOMNode& tag) {
	if (tag.get_tag_name() == "TextNode") {
		return tag.get_text_content();
	}
	string html = "<" + tag.get_tag_name();
	for (auto it = tag.attributes.begin(); it != tag.attributes.end(); it++) {
		html += " " + it->first + "=\"" + it->second + "\"";
	}
	html += ">" + tag.get_text_content();
	DOMNode *node = tag.firstChild;
	while (node && node != nullptr) {
		html += tagHTML(*node);
		node = node->nextSibling;
	}
	html += "</" + tag.get_tag_name() + ">";
	return html;
}

void parseHTML(DOMNode &node, DOMNode &parentNode, string &src, int start, int end, string tagName, vector<DOMNode*> &htmlNodes)
{
	int charIndex, attrIndex, attrLen, firstSpace;
	vector<string> classList;
	char chr, prevChr, attrChr;
	bool closing, newNodeSet;
	string openingTag, attributeSource, attrName, attrValue;

	htmlNodes.push_back(&node);
	node.set_zindex(parentNode.get_zindex() + 1);

	string source = src;

	for (charIndex = start; charIndex < end; charIndex++)
	{
		chr = source[charIndex];

		newNodeSet = false;
		if (chr == '<')
		{
			if ((charIndex + 4) < source.size() && source.substr(charIndex, 4) == "<!--") {
				string comment = "";
				while ((charIndex + 3) < end && source.substr(charIndex, 3) != "-->") {
					comment += source[charIndex];
					charIndex += 1;
				}
				charIndex += 3;
			}
			DOMNode* newNode;
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
			chr = source[++charIndex];
			closing = false;
			while (charIndex < end && chr != '>')
			{
				if (chr == '\t' || chr == '\r' || chr == '\n') {
					chr = ' ';
				}
				if (prevChr == '<' && chr == '/')
				{
					closing = true;
					prevChr = chr;
					chr = source[++charIndex];
					continue;
				}
				openingTag += chr;
				prevChr = chr;
				chr = source[++charIndex];
			};
			size_t firstSpace = openingTag.find(" ");
			//if (!closing)
			//{
			//}
			if (firstSpace != string::npos && lcase(openingTag).find("doctype") == string::npos)
			{
				attributeSource = openingTag.substr(firstSpace + 1);
				openingTag = openingTag.substr(0, firstSpace);
				for (attrIndex = 0, attrLen = attributeSource.length(); attrIndex < attrLen; attrIndex++)
				{
					attrName = "";
					attrValue = "";
					while (attrIndex < attrLen) {
						attrChr = attributeSource[attrIndex];
						if (attrChr == '=') {
							break;
						}
						if (attrChr == '"') {
							attrIndex += 1;
							attrChr = attributeSource[attrIndex];
							while (attrChr != '"' && attrIndex < attrLen) {
								attrName += attrChr;
								attrIndex += 1;
								attrChr = attributeSource[attrIndex];
							}
						}
						else {
							attrName += attrChr;
						}
						attrIndex += 1;
					}
					if (attrIndex < attrLen && attributeSource[attrIndex] == '=') {
						attrIndex += 1;
					}
					while (attrIndex < attrLen) {
						attrChr = attributeSource[attrIndex];
						if (attrChr == '>') {
							break;
						}
						if (attrChr == '"') {
							attrIndex += 1;
							attrChr = attributeSource[attrIndex];
							while (attrChr != '"' && attrIndex < attrLen) {
								attrValue += attrChr;
								attrIndex += 1;
								attrChr = attributeSource[attrIndex];
							}
						}
						else if (whspace(attrChr)) {
							break;
						}
						else {
							attrValue += attrChr;
						}
						attrIndex += 1;
					}
					newNode->attributes[lcase(attrName)] = attrValue;
				}
			}

			openingTag = lcase(openingTag);
			if (closing)
			{
				if ((openingTag == tagName))
				{
					node.set_end(charIndex);
					node.clear_text_content();
					return;
				}
			}
			else if (openingTag == "meta" || openingTag == "nextid") {

			}
			else {
				// CAREFUL!!!
				parseHTML(*newNode, node, source, charIndex + 1, end, openingTag, htmlNodes);
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
				if (newNode->get_tag_name() == "script" && newNode->attributes["src"] != "") {
					string scriptSrc;
					int scriptLen = readTextFile(newNode->attributes["src"], scriptSrc);
					scriptSources.push_back(scriptSrc);
					ParseNode scriptAST = generateAST(scriptSrc);
					scriptsToRunOnLoad.push_back(scriptAST);
					//execAST(scriptAST, globalVariables);
				}
			}
		}
		else if (!(chr == '\r' || chr == '\n' || chr == '\t'))
		{
			node.append_text_content_char(chr);
		}
	}
}