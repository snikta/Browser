#pragma once
// "deleteShape.h"
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include "RedBlackTree.h"
#include "SlabDecomposition.h"
using std::vector;
using std::map;
using std::cout;
using std::cin;
using std::endl;
using std::string;

bool slabSort(Slab *slab1, Slab *slab2) { return slab1->leftX < slab2->leftX; }

void SlabContainer::mergeSlabs(Slab &slab1, Slab &slab2)
{
	slab1.rightX = slab2.rightX;
	this->deleteSlab(slab2.leftX);
	this->RBTSlabLines.insert(slab1.rightX);
}

void SlabContainer::deleteShape(Shape &shape)
{
	RedBlackTree *rbt = &this->RBTSlabLines;
	vector<Slab*> oldSlabs;

	for (int i = 0, len = shape.slabs.size(); i < len; i++)
	{
		Slab *rightSlab = &nilSlab;
		Slab *slab = shape.slabs[i];
		vector<int>::iterator it = find(slab->shapeIds.begin(), slab->shapeIds.end(), shape.id);

		if (this->SlabLinesByLeft.find(slab->leftX) == this->SlabLinesByLeft.end())
		{
			continue;
		}

		if (it != slab->shapeIds.end())
		{
			slab->shapeIds.erase(it);
			slab->shapeCount--;
			for (int j = 0, jLen = slab->shapeCount; j < jLen; j++)
			{
				Shape *sh = this->ShapeMembers[slab->shapeIds[j]];
				vector<Slab*>::iterator it2 = find(sh->slabs.begin(), sh->slabs.end(), slab);
				if (it2 != sh->slabs.end())
				{
					sh->slabs.erase(it2);
				}
			}
		}

		if (slab->shapeCount == 0)
		{
			RedBlackNode *pred = rbt->predecessor(rbt->search(slab->leftX));
			Slab *predSlab = &nilSlab;
			bool allAreTrue = false;
			if (pred != rbt->nil)
			{
				map<int, Slab*>::iterator predSlabIt = this->SlabLinesByLeft.find(pred->key);
				if (predSlabIt != this->SlabLinesByLeft.end())
				{
					predSlab = this->SlabLinesByLeft[pred->key];

					if (predSlab->rightX == slab->leftX)
					{
						allAreTrue = true;
					}
				}
			}
			if (!allAreTrue)
			{
				rbt->deleteNode(*(rbt->search(slab->leftX)));
			}
			this->SlabLinesByLeft.erase(slab->leftX);
		}
		else
		{
			Slab *mySlab = new Slab;

			mySlab->leftX = slab->leftX;
			mySlab->rightX = slab->rightX;
			mySlab->shapeIds = slab->shapeIds;
			mySlab->shapeCount = slab->shapeCount;

			oldSlabs.push_back(mySlab);
		}
		
		map<int, Slab*>::iterator SlabLinesIt = this->SlabLinesByLeft.find(slab->rightX);
		bool rightSlabHasShapes = false;
		if (SlabLinesIt != this->SlabLinesByLeft.end())
		{
			rightSlab = this->SlabLinesByLeft[slab->rightX];
			if (rightSlab->shapeCount)
			{
				rightSlabHasShapes = true;
			}
		}
		if (!rightSlabHasShapes)
		{
			rbt->deleteNode(*(rbt->search(slab->rightX)));
		}

		vector<Slab*> newSlabs;
		Slab *oSlab;
		for (int i = 0, len = oldSlabs.size(); i < len; i++)
		{
			oSlab = oldSlabs[i];
			map<int, Region*> emptyRegions;
			vector<int> emptyShapeIds;
			Slab *mySlab = this->makeSlab(oSlab->leftX, oSlab->rightX, emptyRegions, 0, emptyShapeIds);

			for (int j = 0, jLen = oSlab->shapeIds.size(); j < jLen; j++)
			{
				mySlab->addShape(*(this->ShapeMembers[oSlab->shapeIds[j]]));
				this->ShapeMembers[oSlab->shapeIds[j]]->slabs.push_back(mySlab);
			}

			newSlabs.push_back(mySlab);
		}

		if (oldSlabs.size())
		{
			std::sort(newSlabs.begin(), newSlabs.end(), slabSort);
			SlabLinesIt = this->SlabLinesByLeft.find(newSlabs[newSlabs.size() - 1]->rightX);

			if (SlabLinesIt != this->SlabLinesByLeft.end())
			{
				newSlabs.push_back(this->SlabLinesByLeft[newSlabs[newSlabs.size() - 1]->rightX]);
			}

			// merge slabs if necessary
			Slab *lastSlab;
			vector<Slab*>::iterator newSlabsIt;

			for (newSlabsIt = newSlabs.begin(); newSlabsIt != newSlabs.end(); ++newSlabsIt)
			{
				Slab *thisSlab = *newSlabsIt;
				RedBlackNode *prevNode = rbt->predecessor(rbt->search(thisSlab->leftX));
				Slab *prevSlab = &nilSlab;
				int topY, id;
				bool sameShapes;

				if (prevNode == rbt->nil || !thisSlab->shapeCount)
				{
					continue;
				}

				SlabLinesIt = this->SlabLinesByLeft.find(prevNode->key);
				if (SlabLinesIt == this->SlabLinesByLeft.end())
				{
					continue;
				}

				prevSlab = this->SlabLinesByLeft[prevNode->key];
				sameShapes = true;

				vector<int>::iterator shapeIdsIt;
				
				for (int m = 0, mLen = thisSlab->shapeIds.size(); m < mLen; m++)
				{
					int id = thisSlab->shapeIds[m];
					shapeIdsIt = find(prevSlab->shapeIds.begin(), prevSlab->shapeIds.end(), id);
					if (shapeIdsIt == prevSlab->shapeIds.end())
					{
						sameShapes = false;
					}
				}

				for (int m = 0, mLen = prevSlab->shapeIds.size(); m < mLen; m++)
				{
					int id = prevSlab->shapeIds[m];
					shapeIdsIt = find(thisSlab->shapeIds.begin(), thisSlab->shapeIds.end(), id);
					if (shapeIdsIt == thisSlab->shapeIds.end())
					{
						sameShapes = false;
					}
				}

				if (sameShapes)
				{
					this->mergeSlabs(*prevSlab, *thisSlab);
					lastSlab = prevSlab;
				}
			}
		}
	}

	this->ShapeMembers.erase(shape.id);
}