#pragma once
#include <map>
#include <vector>
#include <string>
#include "JSExec.h"
/* "DOMNode.h" */
using namespace std;

class DOMNode
{
private:
	string id;
	string tagName;
	string textContent = "";
	int start;
	int end;
	int idx;
	int childCount = 0;
	int zIndex = 1;

public:
	map<string, ASTNode*>* ptrASTArray = nullptr;
	map<string, string> attributes;
	map<string, string> style;
	vector<string> classList;
	DOMNode *firstChild = nullptr; // these should
	DOMNode *lastChild = nullptr; // all
	DOMNode *previousSibling = nullptr; // be
	DOMNode *nextSibling = nullptr; // private -- invert case (camelCase becomes _ and vice versa)
	DOMNode *parentNode;
	string type;
	bool expand = true;
	double width = 0.0;
	double height = 0.0;
	double x = 0.0;
	double y = 0.0;
	double marginX = 0.0;
	double marginY = 0.0;
	bool x_set = false;
	bool y_set = false;
	bool width_set = false;
	bool height_set = false;
	DOMNode(string tagName = "", string textContent = "", int start = 0, int end = 0, int idx = 0) : tagName(tagName), textContent(textContent), start(start), end(end), idx(0)
	{
	};

	void set_parent_node(DOMNode &node)
	{
		parentNode = &node;
	}

	void set_tag_name(string newTagName)
	{
		tagName = newTagName;
	};

	void set_zindex(int newZIndex) {
		zIndex = newZIndex;
	};

	int get_zindex() {
		return zIndex;
	};

	string get_tag_name()
	{
		return tagName;
	};

	string get_text_content()
	{
		return textContent;
	};

	void clear_text_content()
	{
		textContent = "";
	};

	void set_id(string newId)
	{
		id = newId;
	};

	string get_id()
	{
		return id;
	};

	int get_start()
	{
		return start;
	};

	int get_end()
	{
		return end;
	};

	int get_idx()
	{
		return idx;
	};

	void set_idx(int newIdx)
	{
		idx = newIdx;
	};

	void set_start(int newStart)
	{
		start = newStart;
	};

	void set_end(int newEnd)
	{
		end = newEnd;
	};

	void set_text_content(string newTextContent) {
		textContent = newTextContent;
	};

	void append_text_content_char(char newChar)
	{
		textContent += newChar;
	};

	void appendChild(DOMNode &childNode)
	{
		if (childCount)
		{
			childNode.previousSibling = lastChild;
			childNode.nextSibling = nullptr;
			lastChild->nextSibling = &childNode;
			lastChild = &childNode;
			childNode.set_idx(childCount);
		}
		else
		{
			firstChild = &childNode;
			lastChild = &childNode;
			childNode.set_idx(0);
		}

		childCount++;
	};

	int get_child_count()
	{
		return childCount;
	};

	void set_child_count(int newChildCount) {
		childCount = newChildCount;
	}
};

extern map<string, DOMNode*> elsById;
extern map<string, vector<DOMNode*>> elsByTagName;
extern map<string, vector<DOMNode*>> elsByClassName;