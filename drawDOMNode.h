#pragma once
// "drawDOMNode.h"
#include <windows.h>
#include <wincodec.h> 
#include <commdlg.h> 
#include <Windowsx.h>
#include <fstream>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "basewin.h"
#include "WICViewerD2D.h"

#include <string>
#include <vector>
#include "DOMNode.h"
#include "substrReplace.h"
#include "SafeRelease.h"
#include "splitString.h"
#include "parser.h"
#include "LoadBitmapFromFile.h"

using namespace std;

extern float viewportScaleX, viewportScaleY;

std::wstring s2ws(const std::string& s);
LPCWSTR stringToLPCWSTR(string str);

void drawDOMNode(DOMNode& node, ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush, int newHeight, int origHeight, int newWidth, int origWidth, int xy, vector<DOMNode*>& nodesInOrder, int level);