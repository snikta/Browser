#pragma once
// "SlabDecomposition.h"
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include "DOMNode.h"
#include "RedBlackTree.h"
#include <vector>
#include <d2d1.h>
using std::vector;
using std::map;
using std::cout;
using std::cin;
using std::endl;
using std::string;

bool ijSort(int i, int j);

struct horizontalLineCheckResponse
{
	bool inExistingRegion;
	bool hasTop = false;
	bool hasBottom = false;
	int yOfTopLine;
	int yOfBottomLine;
};

class Shape;
class Region;
class Slab
{
public:
	int leftX;
	int rightX;
	RedBlackTree *RBTRegions = new RedBlackTree;
	map<int, Region*> RegionsByTop;
	vector<int> shapeIds;
	int shapeCount = 0;
	Region *makeRegion(int topY, int bottomY, vector<Shape*> &shapesToCopy);
	horizontalLineCheckResponse checkIfHorizontalLineLiesInExistingRegion(int lineY);
	void deleteRegion(int topY);
	void splitRegion(int topY, int splitY);
	void prepareBoundLineGivenYPoint(int lineY, bool isTopOfBBox, int edgeWeMightNeed, int shapeId);
	void addShape(Shape &shape);
};
extern Slab nilSlab;

class Shape
{
public:
	vector<Slab*> slabs; // must delete Slabs later
	int id, x1, x2, y1, y2;
	D2D1_RECT_F *rect;
	DOMNode *node;
};

class Region
{
public:
int topY;
int bottomY;
vector<Shape*> shapes; // must delete shapes later
};
extern Region nilRegion;

enum SlabLineType
{
	EXISTS_AS_SLAB_LINE,
	NONEXISTENT_BUT_LIES_INSIDE_EXISTING_SLAB,
	NONEXISTENT_AND_LIES_IN_WHITESPACE
};
class SlabContainer
{
public:
	RedBlackTree RBTSlabLines;
	map<int, Slab*> SlabLinesByLeft;
	map<int, Shape*> ShapeMembers;
	int NextAvailableShapeId = 1;

	void mergeSlabs(Slab &slab1, Slab &slab2);
	void deleteShape(Shape &shape);
	void preprocessSubdivision(vector<Shape*>& shapesToAdd, char oType, Slab& slab);
	void addShape(Shape& shape);
	void copyRegions(RedBlackTree* src, RedBlackTree* dest);
	Slab* cloneSlab(int leftX, int rightX, Slab& oldSlab);
	Slab* makeSlab(int leftX, int rightX, map<int, Region*>& regionsToPaste, int shapeCount, vector<int>& shapeIds);
	void deleteSlab(int leftX);//, bool keepRegions)
	void splitSlab(int leftX, int splitX);
	struct verticalLineCheckResponse;
	verticalLineCheckResponse checkIfVerticalLineLiesInExistingSlab(int lineX);
	void prepareBoundLineGivenXPoint(int lineX, bool isLeftOfBBox, int edgeWeMightNeed, int shapeId);
};

extern SlabContainer mySlabContainer;