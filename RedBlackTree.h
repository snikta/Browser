#pragma once
#pragma once
#include <iostream>
#include <string>
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::to_string;

enum RedBlackColor { red, black };
class RedBlackNode
{
public:
	RedBlackColor color = black;
	RedBlackNode *left;
	RedBlackNode *right;
	RedBlackNode *parent;
	int key;

	RedBlackNode(RedBlackNode &nil, int newKey)
	{
		this->key = newKey;
		this->left = &nil;
		this->right = &nil;
		this->parent = &nil;
	};
	RedBlackNode() { };
};

class RedBlackTree
{
public:
	RedBlackNode *root;
	RedBlackNode *nil = new RedBlackNode;
	RedBlackTree()
	{
		this->root = this->nil;
	}
	RedBlackNode *closest(int key)
	{
		return search(key, true);
	};
	RedBlackNode *search(int key)
	{
		return search(key, false);
	};
	RedBlackNode *search(int key, bool closest)
	{
		RedBlackNode *x = this->root;
		RedBlackNode *lastGood = x;
		while (x != this->nil && key != x->key)
		{
			lastGood = x;
			if (key < x->key)
			{
				x = x->left;
			}
			else
			{
				x = x->right;
			}
		}
		if (closest && x == this->nil)
		{
			return lastGood;
		}
		else
		{
			return x;
		}
	};
	void leftRotate(RedBlackNode &x)
	{
		RedBlackNode *y = x.right;
		x.right = y->left;
		if (y->left != this->nil)
		{
			y->left->parent = &x;
		}
		y->parent = x.parent;
		if (x.parent == this->nil)
		{
			this->root = y;
		}
		else
		{
			if (&x == x.parent->left)
			{
				x.parent->left = y;
			}
			else
			{
				x.parent->right = y;
			}
		}
		y->left = &x;
		x.parent = y;
	};
	void rightRotate(RedBlackNode &x)
	{
		RedBlackNode *y = x.left;
		x.left = y->right;
		if (y->right != this->nil)
		{
			y->right->parent = &x;
		}
		y->parent = x.parent;
		if (x.parent == this->nil)
		{
			this->root = y;
		}
		else
		{
			if (&x == x.parent->right)
			{
				x.parent->right = y;
			}
			else
			{
				x.parent->left = y;
			}
		}
		y->right = &x;
		x.parent = y;
	};
	RedBlackNode *binaryInsert(int newKey)
	{
		RedBlackNode *z = new RedBlackNode(*(this->nil), newKey); // delete later?
		RedBlackNode *y = this->nil;
		RedBlackNode *x = this->root;
		while (x != this->nil)
		{
			y = x;
			if (z->key < x->key)
			{
				x = x->left;
			}
			else
			{
				x = x->right;
			}
		}
		z->parent = y;
		if (y == this->nil)
		{
			this->root = z;
		}
		else {
			if (z->key < y->key)
			{
				y->left = z;
			}
			else
			{
				y->right = z;
			}
		}
		return z;
	};
	RedBlackNode *insert(int newKey)
	{
		if (this->search(newKey, false) != this->nil)
		{
			return this->nil;
		}
		RedBlackNode *x = binaryInsert(newKey);
		RedBlackNode *newNode = x;
		RedBlackNode *y;
		x->color = red;
		while (x != root && x->parent->color == red)
		{
			if (x->parent == x->parent->parent->left)
			{
				y = x->parent->parent->right;
				if (y->color == red)
				{
					x->parent->color = black;
					y->color = black;
					x->parent->parent->color = red;
					x = x->parent->parent;
				}
				else
				{
					if (x == x->parent->right)
					{
						x = x->parent;
						this->leftRotate(*x);
					}
					x->parent->color = black;
					x->parent->parent->color = red;
					this->rightRotate(*(x->parent->parent));
				}
			}
			else
			{
				y = x->parent->parent->left;
				if (y->color == red)
				{
					x->parent->color = black;
					y->color = black;
					x->parent->parent->color = red;
					x = x->parent->parent;
				}
				else
				{
					if (x == x->parent->left)
					{
						x = x->parent;
						this->rightRotate(*x);
					}
					x->parent->color = black;
					x->parent->parent->color = red;
					this->leftRotate(*(x->parent->parent));
				}
			}
		}
		this->root->color = black;
		return newNode;
	};
	RedBlackNode *minimum(RedBlackNode *x)
	{
		while (x->left != this->nil)
		{
			x = x->left;
		}
		return x;
	};
	RedBlackNode *maximum(RedBlackNode *x)
	{
		while (x->right != this->nil)
		{
			x = x->right;
		}
		return x;
	};
	RedBlackNode *predecessor(RedBlackNode *x)
	{
		RedBlackNode *y;
		if (x->left != this->nil)
		{
			return this->maximum(x->left);
		}
		y = x->parent;
		while (y != this->nil && x == y->left)
		{
			x = y;
			y = y->parent;
		}
		return y;
	};
	RedBlackNode *successor(RedBlackNode *x)
	{
		RedBlackNode *y;
		if (x->right != this->nil)
		{
			return this->minimum(x->right);
		}
		y = x->parent;
		while (y != this->nil && x == y->right)
		{
			x = y;
			y = y->parent;
		}
		return y;
	};
	void deleteNodeFixup(RedBlackNode *x)
	{
		RedBlackNode *w;
		while (x != this->root && x->color == black)
		{
			if (x == x->parent->left)
			{
				w = x->parent->right;
				if (w->color == red)
				{
					w->color = black;
					x->parent->color = red;
					this->leftRotate(*(x->parent));
					w = x->parent->right;
				}
				if (w->left->color == black && w->right->color == black)
				{
					w->color = red;
					x = x->parent;
				}
				else
				{
					if (w->right->color == black)
					{
						w->left->color = black;
						w->color = red;
						this->rightRotate(*w);
						w = x->parent->right;
					}
					w->color = x->parent->color;
					x->parent->color = black;
					w->right->color = black;
					this->leftRotate(*(x->parent));
					x = this->root;
				}
			}
			else
			{
				w = x->parent->left;
				if (w->color == red)
				{
					w->color = black;
					x->parent->color = red;
					this->rightRotate(*(x->parent));
					w = x->parent->left;
				}
				if (w->right->color == black && w->left->color == black)
				{
					w->color = red;
					x = x->parent;
				}
				else
				{
					if (w->left->color == black)
					{
						w->right->color = black;
						w->color = red;
						this->leftRotate(*w);
						w = x->parent->left;
					}
					w->color = x->parent->color;
					x->parent->color = black;
					w->left->color = black;
					this->rightRotate(*(x->parent));
					x = this->root;
				}
			}
		}
		x->color = black;
	};
	RedBlackNode *deleteNode(RedBlackNode &z)
	{
		RedBlackNode *x;
		RedBlackNode *y;
		if (&z == this->nil) {
			return this->nil;
		}
		if (z.left == this->nil || z.right == this->nil)
		{
			y = &z;
		}
		else
		{
			y = successor(&z);
		}
		if (y->left == this->nil)
		{
			x = y->right;
		}
		else
		{
			x = y->left;
		}
		x->parent = y->parent;
		if (y->parent == this->nil)
		{
			this->root = x;
		}
		else
		{
			if (y == y->parent->left)
			{
				y->parent->left = x;
			}
			else
			{
				y->parent->right = x;
			}
		}
		if (y != &z)
		{
			z.key = y->key;
			// copy any other fields
		}
		if (y->color == black)
		{
			deleteNodeFixup(x);
		}
		return y;
	};
};