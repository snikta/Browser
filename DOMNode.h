#pragma once
/* "DOMNode.h" */

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

public:
	map<string, string> attributes;
	map<string, string> style;
	vector<string> classList;
	DOMNode *firstChild; // these should
	DOMNode *lastChild; // all
	DOMNode *previousSibling; // be
	DOMNode *nextSibling; // private -- invert case (camelCase becomes _ and vice versa)
	DOMNode *parentNode;
	string type;
	double width = 0;
	double height = 0;
	double x = 0;
	double y = 0;
	double marginX = 0;
	double marginY = 0;
	bool x_set = false;
	bool y_set = false;
	bool width_set = false;
	bool height_set = false;
	DOMNode(string tagName, string textContent, int start, int end, int idx) : idx(0)
	{
		//this->parentNode = &parentNode;
		this->tagName = tagName;
		this->textContent = textContent;
		this->start = start;
		this->end = end;
		//this->idx = idx;
	};

	void set_parent_node(DOMNode &node)
	{
		this->parentNode = &node;
	}

	void set_tag_name(string newTagName)
	{
		this->tagName = newTagName;
	};

	string get_tag_name()
	{
		return this->tagName;
	};

	string get_text_content()
	{
		return this->textContent;
	};

	void clear_text_content()
	{
		this->textContent = "";
	};

	void set_id(string newId)
	{
		this->id = newId;
	};

	string get_id()
	{
		return this->id;
	};

	int get_start()
	{
		return this->start;
	};

	int get_end()
	{
		return this->end;
	};

	int get_idx()
	{
		return this->idx;
	};

	void set_idx(int newIdx)
	{
		this->idx = newIdx;
	};

	void set_end(int newEnd)
	{
		this->end = newEnd;
	};

	void append_text_content_char(char newChar)
	{
		this->textContent += newChar;
	};

	void appendChild(DOMNode &childNode)
	{
		if (this->childCount)
		{
			childNode.previousSibling = this->lastChild;
			childNode.nextSibling = NULL;
			this->lastChild->nextSibling = &childNode;
			this->lastChild = &childNode;
		}
		else
		{
			this->firstChild = &childNode;
			this->lastChild = &childNode;
		}

		this->childCount++;
	};

	int get_child_count()
	{
		return this->childCount;
	};
};

map<string, DOMNode*> elsById;
map<string, vector<DOMNode*>> elsByTagName;
map<string, vector<DOMNode*>> elsByClassName;