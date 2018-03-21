#pragma once
// "SlabDecomposition.h"
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include "RedBlackTree.h"
#include <vector>
#include <d2d1.h>
using std::vector;
using std::map;
using std::cout;
using std::cin;
using std::endl;
using std::string;

bool ijSort(int i, int j) { return (i < j); };

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
Slab nilSlab;

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
Region nilRegion;

Region *Slab::makeRegion(int topY, int bottomY, vector<Shape*> &shapesToCopy)
{
	Region *newRegion = new Region;

	newRegion->topY = topY;
	newRegion->bottomY = bottomY;

	this->RBTRegions->insert(topY);
	this->RBTRegions->insert(bottomY);

	for (int i = 0, len = shapesToCopy.size(); i < len; i++)
	{
		newRegion->shapes.push_back(shapesToCopy[i]);
	}

	this->RegionsByTop[topY] = newRegion;

	return newRegion;
}
void Slab::addShape(Shape &shape)
{
	int y1 = shape.y1, y2 = shape.y2, y = y1;

	this->prepareBoundLineGivenYPoint(y1, true, y2, shape.id);
	this->prepareBoundLineGivenYPoint(y2, false, y1, shape.id);

	if (find(this->shapeIds.begin(), this->shapeIds.end(), shape.id) == this->shapeIds.end())
	{
		this->shapeIds.push_back(shape.id);
		this->shapeCount++;
	}

	RedBlackTree *rbt = this->RBTRegions;

	while (y < y2)
	{
		bool regionExists = this->RegionsByTop.find(y) != this->RegionsByTop.end();
		bool regionTouchesNext = false;
		Region *currentRegion = &nilRegion;

		if (regionExists)
		{
			currentRegion = this->RegionsByTop[y];
			RedBlackNode *cur = rbt->search(currentRegion->topY);
			
			if (cur != rbt->nil)
			{
				RedBlackNode *succ = rbt->successor(cur);
				if (succ != rbt->nil && succ != cur)
				{
					regionTouchesNext = true;
				}
			}
		}

		if (!regionTouchesNext)
		{
			RedBlackNode *node = rbt->closest(y);
			regionExists = false;
			int regionKey;
			if (node != rbt->nil)
			{
				if (node->key <= y)
				{
					regionKey = rbt->successor(node)->key;
				}
				else
				{
					regionKey = node->key;
				}
				regionExists = true;
			}

			vector<Shape*> emptyShapes;
			currentRegion = this->makeRegion(y, regionExists ? regionKey : y2, emptyShapes);
		}
		y = currentRegion->bottomY;
		if (find(currentRegion->shapes.begin(), currentRegion->shapes.end(), &shape) == currentRegion->shapes.end())
		{
			currentRegion->shapes.push_back(&shape);
		}
	}
}
horizontalLineCheckResponse Slab::checkIfHorizontalLineLiesInExistingRegion(int lineY)
{
	RedBlackNode *tempLineNode = this->RBTRegions->insert(lineY);
	RedBlackNode *tempPred = this->RBTRegions->predecessor(tempLineNode);
	RedBlackNode *tempSucc = this->RBTRegions->successor(tempLineNode);
	bool functionHasWorkedAtThisPoint = false;
	horizontalLineCheckResponse response;

	if (tempPred != this->RBTRegions->nil && tempSucc != this->RBTRegions->nil)
	{
		map<int, Region*>::iterator it = this->RegionsByTop.find(tempPred->key);
		if (it != this->RegionsByTop.end())
		{
			Region *regionAbove = it->second;
			if (regionAbove->bottomY == tempSucc->key)
			{
				functionHasWorkedAtThisPoint = true;
			}
		}
	}

	response.inExistingRegion = functionHasWorkedAtThisPoint;
	if (tempPred != this->RBTRegions->nil)
	{
		response.hasTop = true;
		response.yOfTopLine = tempPred->key;
	}
	if (tempSucc != this->RBTRegions->nil)
	{
		response.hasBottom = true;
		response.yOfBottomLine = tempSucc->key;
	}
	return response;
};
void Slab::prepareBoundLineGivenYPoint(int lineY, bool isTopOfBBox, int edgeWeMightNeed, int shapeId)
{
	RedBlackTree *rbt = this->RBTRegions;
	if (rbt->search(lineY) == rbt->nil)
	{
		horizontalLineCheckResponse existingSlabCheck = this->checkIfHorizontalLineLiesInExistingRegion(lineY);

		if (existingSlabCheck.inExistingRegion)
		{
			this->splitRegion(existingSlabCheck.yOfTopLine, lineY);
		}
		else
		{
			int otherEdge;
			if (isTopOfBBox)
			{
				if (existingSlabCheck.hasBottom && existingSlabCheck.yOfBottomLine <= edgeWeMightNeed)
				{
					otherEdge = existingSlabCheck.yOfBottomLine;
				}
				else
				{
					otherEdge = edgeWeMightNeed;
				}
			}
			else
			{
				if (existingSlabCheck.hasTop && existingSlabCheck.yOfTopLine >= edgeWeMightNeed)
				{
					otherEdge = existingSlabCheck.yOfTopLine;
				}
				else
				{
					otherEdge = edgeWeMightNeed;
				}
			}

			vector<Shape*> emptyShapes;
			this->makeRegion(min(lineY, otherEdge), max(lineY, otherEdge), emptyShapes);
		}
	}
};
void Slab::deleteRegion(int topY)
{
	int oldRegionBottomY = this->RegionsByTop[topY]->bottomY;

	this->RBTRegions->deleteNode(*(this->RBTRegions->search(topY)));
	this->RBTRegions->deleteNode(*(this->RBTRegions->search(oldRegionBottomY))); // what if oldRegion.bottomY is topY of next region?

	this->RegionsByTop.erase(topY);
}
void Slab::splitRegion(int topY, int splitY)
{
	Region *regionToSplit = this->RegionsByTop[topY];
	int bottomY = regionToSplit->bottomY;
	vector<Shape*> *shapes = &(regionToSplit->shapes);

	this->deleteRegion(topY);
	this->makeRegion(topY, splitY, *shapes);
	this->makeRegion(splitY, bottomY, *shapes);

	// only need to create one new region
}

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
	void preprocessSubdivision(vector<Shape*> &shapesToAdd, char oType, Slab &slab)
	{
		//MessageBox(NULL, L"Invoking preprocessSubdivision", L"", MB_OK);
		map<int, vector<Shape*>> ords;
		vector<int> sortedOrds;
		int edgeOrd, prevO;

		for (int i = 0, len = shapesToAdd.size(); i < len; i++)
		{
			Shape *s = shapesToAdd[i];

			int o1, o2;
			if (oType == 'x')
			{
				o1 = s->x1;
				o2 = s->x2;
			}
			else
			{
				o1 = s->y1;
				o2 = s->y2;
			}

			if (find(sortedOrds.begin(), sortedOrds.end(), o1) == sortedOrds.end())
			{
				sortedOrds.push_back(o1);
			}
			if (find(sortedOrds.begin(), sortedOrds.end(), o2) == sortedOrds.end())
			{
				sortedOrds.push_back(o2);
			}

			ords[o1].push_back(s);
			ords[o2].push_back(s);

			if (oType == 'x')
			{
				s->id = this->NextAvailableShapeId++;
				this->ShapeMembers[s->id] = s;
			}
		}

		std::sort(sortedOrds.begin(), sortedOrds.end(), ijSort);
		vector<int>::iterator it;
		vector<Shape*> activeShapes, slabShapes;
		bool slabStarted = false;
		map<int, Region*> emptyRegions;
		Slab *_slab = &nilSlab;
		for (it = sortedOrds.begin(); it != sortedOrds.end(); ++it)
		{
			edgeOrd = *it;
			vector<Shape*> slabShapes;
			if (slabStarted)
			{
				slabShapes = activeShapes;
				if (oType == 'x')
				{
					vector<int> shapeIds;
					for (int i = 0, len = slabShapes.size(); i < len; i++)
					{
						shapeIds.push_back(slabShapes[i]->id);
					}
					_slab = this->makeSlab(prevO, edgeOrd, emptyRegions, slabShapes.size(), shapeIds);
					this->preprocessSubdivision(slabShapes, 'y', *_slab);
				}
				else
				{
					slab.makeRegion(prevO, edgeOrd, slabShapes);
				}
			}
			bool match = false;
			for (int j = 0, len = ords[edgeOrd].size(); j < len; j++)
			{
				if (oType == 'x')
				{
					if (ords[edgeOrd][j]->x1 == edgeOrd)
					{
						activeShapes.push_back(ords[edgeOrd][j]);
						match = true;
					}
					else if (ords[edgeOrd][j]->x2 == edgeOrd)
					{
						activeShapes.erase(find(activeShapes.begin(), activeShapes.end(), ords[edgeOrd][j]));
					}
				}
				else
				{
					if (ords[edgeOrd][j]->y1 == edgeOrd)
					{
						activeShapes.push_back(ords[edgeOrd][j]);
						match = true;
					}
					else if (ords[edgeOrd][j]->y2 == edgeOrd)
					{
						activeShapes.erase(find(activeShapes.begin(), activeShapes.end(), ords[edgeOrd][j]));
					}
				}
			}
			if (activeShapes.size())
			{
				slabStarted = true;
				prevO = edgeOrd;
			}
			else
			{
				slabStarted = false;
			}
		}
	};
	void addShape(Shape &shape)
	{
		int x1 = shape.x1, x2 = shape.x2;

		// they should now both exist as slab lines we can use as bound lines
		this->prepareBoundLineGivenXPoint(x1, true, x2, shape.id);
		this->prepareBoundLineGivenXPoint(x2, false, x1, shape.id);

		int x = x1;
		bool slabExists = false;

		while (x < x2)
		{
			bool slabExists = this->SlabLinesByLeft.find(x) != this->SlabLinesByLeft.end();
			bool slabTouchesNext = false;
			Slab *currentSlab = &nilSlab;

			if (slabExists)
			{
				currentSlab = this->SlabLinesByLeft[x];
				RedBlackNode *cur = this->RBTSlabLines.search(currentSlab->leftX);
				RedBlackNode *succ = cur == this->RBTSlabLines.nil ? this->RBTSlabLines.nil : this->RBTSlabLines.successor(cur);

				if (cur != this->RBTSlabLines.nil && succ != this->RBTSlabLines.nil && succ != cur)
				{
					if (currentSlab->rightX == succ->key)
					{
						slabTouchesNext = true;

						// there's a slab here; we don't have to create one. woo!
						// but we do have to make sure the Y-values for the top and bottom of the bounding box are included as a region. boo!

						currentSlab->addShape(shape);
						if (find(shape.slabs.begin(), shape.slabs.end(), currentSlab) == shape.slabs.end())
						{
							shape.slabs.push_back(currentSlab);
						}
					}
				}
			}

			if (!slabTouchesNext)
			{
				// no slab here; we need to create one. :-(

				RedBlackNode *node = this->RBTSlabLines.closest(x);
				slabExists = false;
				int slabKey;

				if (node != this->RBTSlabLines.nil)
				{
					if (node->key <= x)
					{
						slabKey = this->RBTSlabLines.successor(node)->key;
					}
					else
					{
						slabKey = node->key;
					}
					slabExists = true; // bug? check successor exists
				}

				int innerX2 = slabExists ? slabKey : x2;
				map<int, Region*> emptyRegions;
				vector<int> shapeIds = { shape.id };
				currentSlab = this->makeSlab(x, innerX2, emptyRegions, 1, shapeIds);
				if (find(shape.slabs.begin(), shape.slabs.end(), currentSlab) == shape.slabs.end())
				{
					shape.slabs.push_back(currentSlab);
				}
				// now we need to insert the Y-values for the top and bottom of the bounding box to form a region
				vector<Shape*> shapesToCopy = { &shape };
				currentSlab->makeRegion(shape.y1, shape.y2, shapesToCopy);
			}
			x = currentSlab->rightX;
		}
	};
	void copyRegions(RedBlackTree *src, RedBlackTree *dest)
	{
		RedBlackNode *x;
		RedBlackNode *min = src->minimum(src->root);
		RedBlackNode *max = src->maximum(src->root);

		for (x = min; x != src->nil; x = src->successor(x))
		{
			dest->insert(x->key);
		}
	};
	Slab *cloneSlab(int leftX, int rightX, Slab &oldSlab)
	{
		Slab *newSlab = new Slab;
		newSlab->shapeCount = oldSlab.shapeCount;
		newSlab->shapeIds = oldSlab.shapeIds; // copy

		for (int i = 0, len = newSlab->shapeIds.size(); i < len; i++)
		{
			ShapeMembers[newSlab->shapeIds[i]]->slabs.push_back(newSlab);
		}

		this->RBTSlabLines.insert(leftX);
		this->RBTSlabLines.insert(rightX);

		map<int, Region*>::iterator it;
		for (it = oldSlab.RegionsByTop.begin(); it != oldSlab.RegionsByTop.end(); ++it) {
			newSlab->makeRegion(oldSlab.RegionsByTop[it->first]->topY, oldSlab.RegionsByTop[it->first]->bottomY, oldSlab.RegionsByTop[it->first]->shapes);
		}

		this->SlabLinesByLeft[leftX] = newSlab;
		newSlab->leftX = leftX;
		newSlab->rightX = rightX;

		return newSlab;
	};
	Slab *makeSlab(int leftX, int rightX, map<int, Region*> &regionsToPaste, int shapeCount, vector<int> &shapeIds)
	{
		Slab *newSlab = new Slab;
		newSlab->shapeCount = shapeCount;
		newSlab->shapeIds = shapeIds; // copy

		for (int i = 0, len = shapeIds.size(); i < len; i++)
		{
			ShapeMembers[shapeIds[i]]->slabs.push_back(newSlab);
		}

		this->RBTSlabLines.insert(leftX);
		this->RBTSlabLines.insert(rightX);

		map<int, Region*>::iterator it;
		for (it = regionsToPaste.begin(); it != regionsToPaste.end(); ++it) {
			newSlab->makeRegion(regionsToPaste[it->first]->topY, regionsToPaste[it->first]->bottomY, regionsToPaste[it->first]->shapes);
		}

		this->SlabLinesByLeft[leftX] = newSlab;
		//newSlab = this->SlabLinesByLeft[leftX];
		newSlab->leftX = leftX;
		newSlab->rightX = rightX;

		return newSlab;
	};
	void deleteSlab(int leftX)//, bool keepRegions)
	{
		if (this->SlabLinesByLeft.find(leftX) == this->SlabLinesByLeft.end())
		{
			return;
		}

		Slab *oldSlab = this->SlabLinesByLeft[leftX];
		Slab *predSlab = &nilSlab;
		RedBlackNode *pred;
		RedBlackTree *rbt = &(this->RBTSlabLines);

		pred = rbt->predecessor(rbt->search(leftX));
		if (pred != rbt->nil)
		{
			if (!(this->SlabLinesByLeft.find(pred->key) != this->SlabLinesByLeft.end() && predSlab->rightX == leftX))
			{
				rbt->deleteNode(*(rbt->search(leftX)));
			}
		}
		RedBlackNode *succ = rbt->search(oldSlab->rightX);
		if (succ != rbt->nil)
		{
			if (!(this->SlabLinesByLeft.find(succ->key) != this->SlabLinesByLeft.end()))
			{
				rbt->deleteNode(*(rbt->search(oldSlab->rightX)));
			}
		}

		for (int i = 0, len = oldSlab->shapeIds.size(); i < len; i++)
		{
			Shape *shape = ShapeMembers[oldSlab->shapeIds[i]];
			
			vector<Slab*>::iterator it;
			it = find(shape->slabs.begin(), shape->slabs.end(), oldSlab);
			if (it != shape->slabs.end())
			{
				shape->slabs.erase(it);
			}
		}

		this->SlabLinesByLeft.erase(leftX);
	};
	void splitSlab(int leftX, int splitX)
	{
		Slab *slabToSplit = this->SlabLinesByLeft[leftX];
		int rightX = slabToSplit->rightX;

		slabToSplit->rightX = splitX;

		this->cloneSlab(splitX, rightX, *slabToSplit);// slabToSplit->RegionsByTop, slabToSplit->shapeCount, slabToSplit->shapeIds);
	};
	struct verticalLineCheckResponse
	{
		bool inExistingSlab;
		bool hasLeft = false;
		bool hasRight = false;
		int xOfLeftLine;
		int xOfRightLine;
	};
	verticalLineCheckResponse checkIfVerticalLineLiesInExistingSlab(int lineX)
	{
		RedBlackNode *tempLineNode = this->RBTSlabLines.insert(lineX);
		RedBlackNode *tempPred = this->RBTSlabLines.predecessor(tempLineNode);
		RedBlackNode *tempSucc = this->RBTSlabLines.successor(tempLineNode);
		bool functionHasWorkedAtThisPoint = false;
		verticalLineCheckResponse response;

		if (tempPred != this->RBTSlabLines.nil && tempSucc != this->RBTSlabLines.nil)
		{
			map<int, Slab*>::iterator it = this->SlabLinesByLeft.find(tempPred->key);
			if (it != this->SlabLinesByLeft.end())
			{
				Slab *slabToLeft = it->second;
				if (slabToLeft->rightX == tempSucc->key)
				{
					functionHasWorkedAtThisPoint = true;
				}
			}
		}

		response.inExistingSlab = functionHasWorkedAtThisPoint;
		if (tempPred != this->RBTSlabLines.nil)
		{
			response.hasLeft = true;
			response.xOfLeftLine = tempPred->key;
		}
		if (tempSucc != this->RBTSlabLines.nil)
		{
			response.hasRight = true;
			response.xOfRightLine = tempSucc->key;
		}
		return response;
	};
	void prepareBoundLineGivenXPoint(int lineX, bool isLeftOfBBox, int edgeWeMightNeed, int shapeId)
	{
		// we need to determine the status of the line "x = lineX"
		// it will either be a) already a slab line, b) nonexistent, but lying in an existing slab, or c) nonexistent, and lying in whitespace

		if (this->RBTSlabLines.search(lineX) == this->RBTSlabLines.nil)
		{
			// either b) or c)

			verticalLineCheckResponse existingSlabCheck = this->checkIfVerticalLineLiesInExistingSlab(lineX);

			if (existingSlabCheck.inExistingSlab)
			{
				// b) lies in an existing slab; we just need to split that slab at xPointOfLine
				this->splitSlab(existingSlabCheck.xOfLeftLine, lineX);
			}
			else
			{
				// c) lies in whitespace; we need to a create a slab from xPointOfLine to the
				// nearest slab line that lies in the direction of the relevant bounding box edge

				int otherEdge;
				if (isLeftOfBBox)
				{
					if (existingSlabCheck.hasRight && existingSlabCheck.xOfRightLine <= edgeWeMightNeed)
					{
						otherEdge = existingSlabCheck.xOfRightLine;
					}
					else
					{
						otherEdge = edgeWeMightNeed;
					}
				}
				else // line given is right line of bounding box
				{
					if (existingSlabCheck.hasLeft && existingSlabCheck.xOfLeftLine >= edgeWeMightNeed)
					{
						otherEdge = existingSlabCheck.xOfLeftLine;
					}
					else
					{
						otherEdge = edgeWeMightNeed;
					}
				}

				map<int, Region*> emptyRegions;
				vector<int> shapeIds = { shapeId };
				this->makeSlab(min(lineX, otherEdge), max(lineX, otherEdge), emptyRegions, 1, shapeIds);
			}
		}
		else
		{
			// a) already a slab line; do nothing as it is already suitable for use as a bound line
		}
	};
};