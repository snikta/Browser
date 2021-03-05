#pragma once
#include <windows.h>
#include <wincodec.h> 
#include <commdlg.h> 
#include <Windowsx.h>
#include <fstream>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "basewin.h"
#include "WICViewerD2D.h"
#include "readTextFile.h"
#include "parser.h"
#include "parseCSS.h"
#include "parseHTML.h"
#include "SafeRelease.h"
#include "LoadBitmapFromFile.h"
#include "drawDOMNode.h"
#include "RedBlackTree.h"
#include "SlabDecomposition.h"
#include "fileWatcher.h"
#include "JSExec.h"
#include <cstdlib>
#include <ctime>

SlabContainer mySlabContainer;
Region* selRegion;

map<string, DOMNode*> elsById;
map<string, vector<DOMNode*>> elsByTagName;
map<string, vector<DOMNode*>> elsByClassName;

vector<vector<ASTNode>> eventListenersToBindArgs;
vector<Scope> eventListenersToBindScopes;
vector<ParseNode> scriptsToRunOnLoad;
vector<DOMNode*> nodesInOrder;
bool pageLoaded = false;

IDWriteFactory* m_pDWriteFactory;
IDWriteTextFormat* m_pTextFormat;
ID2D1Factory* pFactory = NULL;

IWICBitmap* pWICBitmap = NULL;
ID2D1RenderTarget* pRenderTarget2 = NULL;
ID2D1SolidColorBrush* m_pBlackBrush;

Parser myParser;

std::ofstream ffile;
bool firstWatch = true;

void testJSExec() {
	globalVariables.ScopeArray["Object"] = ASTNode();
	globalVariables.ScopeArray["me"] = ASTNode();
	(*globalVariables.ScopeArray["me"].ASTArray)["firstName"] = new ASTNode(string("Josh"));
	(*globalVariables.ScopeArray["me"].ASTArray)["lastName"] = new ASTNode(string("Atkins"));
	(*globalVariables.ScopeArray["me"].ASTArray)["middleNames"] = new ASTNode();
	(*(*globalVariables.ScopeArray["me"].ASTArray)["middleNames"]->ASTArray)["first"] = new ASTNode(string("Benjamin"));
	(*(*globalVariables.ScopeArray["me"].ASTArray)["middleNames"]->ASTArray)["second"] = new ASTNode(string("Mark"));
	(*globalVariables.ScopeArray["me"].ASTArray)["dateOfBirth"] = new ASTNode();
	(*(*globalVariables.ScopeArray["me"].ASTArray)["dateOfBirth"]->ASTArray)["date"] = new ASTNode(string("15"));
	(*(*globalVariables.ScopeArray["me"].ASTArray)["dateOfBirth"]->ASTArray)["monthName"] = new ASTNode(string("April"));
	(*(*globalVariables.ScopeArray["me"].ASTArray)["dateOfBirth"]->ASTArray)["year"] = new ASTNode(string("1994"));

	globalVariables.ScopeArray["currentYear"] = ASTNode(string("2020"));
	globalVariables.ScopeArray["yearOfBirth"] = ASTNode(string("1994"));
	globalVariables.ScopeArray["spacemission"] = ASTNode(string("Apollo 11"));
	globalVariables.ScopeArray["missionsByYear"] = ASTNode();
	(*globalVariables.ScopeArray["missionsByYear"].ASTArray)["1961"] = new ASTNode();
	(*(*globalVariables.ScopeArray["missionsByYear"].ASTArray)["1961"]->ASTArray)["0"] = new ASTNode(string("Gagarin"));
	(*(*globalVariables.ScopeArray["missionsByYear"].ASTArray)["1961"]->ASTArray)["1"] = new ASTNode(string("Shepard"));
	(*globalVariables.ScopeArray["missionsByYear"].ASTArray)["1968"] = new ASTNode();
	(*(*globalVariables.ScopeArray["missionsByYear"].ASTArray)["1968"]->ASTArray)["0"] = new ASTNode(string("Apollo 7"));
	(*(*globalVariables.ScopeArray["missionsByYear"].ASTArray)["1968"]->ASTArray)["1"] = new ASTNode(string("Apollo 8"));
	(*globalVariables.ScopeArray["missionsByYear"].ASTArray)["1969"] = new ASTNode();
	(*(*globalVariables.ScopeArray["missionsByYear"].ASTArray)["1969"]->ASTArray)["0"] = new ASTNode(string("Apollo 11"));
	(*(*globalVariables.ScopeArray["missionsByYear"].ASTArray)["1969"]->ASTArray)["1"] = new ASTNode(string("Apollo 12"));
	(*globalVariables.ScopeArray["missionsByYear"].ASTArray)["1970"] = new ASTNode();
	(*(*globalVariables.ScopeArray["missionsByYear"].ASTArray)["1970"]->ASTArray)["0"] = new ASTNode(string("Apollo 13"));

	/*globalVariables.ScopeArray["k"] = ASTNode(37.0L);
	globalVariables.ScopeArray["j"] = ASTNode(22.0L);
	globalVariables.ScopeArray["i"] = ASTNode(15.0L);*/
}

void loadPage(string url) {
	pageLoaded = false;
	myParser.set_location(url);

	if (true)//(myParser.rootNode == nullptr)
	{
		string fname = myParser.get_location();
		string src;
		int len = readTextFile(fname, src);

		string emptyStr = "";
		string root = "body";

		mySlabContainer = *(new SlabContainer);

		myParser.rootNode = new DOMNode(root, emptyStr, 0, len, 0);
		(myParser.rootNode)->set_parent_node(*(myParser.rootNode));

		vector<DOMNode*> htmlNodes = {};
		parseHTML(*(myParser.rootNode), *(myParser.rootNode), src, 0, len, emptyStr, htmlNodes);

		std::reverse(htmlNodes.begin(), htmlNodes.end());
		for (int i = 0, len = htmlNodes.size(); i < len; i++) {
			DOMNode* node = htmlNodes[i];
			if (node == node->parentNode) {
				continue;
			}
			if ((node->get_tag_name() == "dd" || node->get_tag_name() == "dt") && node->parentNode && node->parentNode != nullptr) {
				DOMNode* pNode = node;
				while (pNode && pNode != nullptr && pNode != pNode->parentNode && pNode->get_tag_name() != "dl") {
					pNode = pNode->parentNode;
				}
				if (pNode->get_tag_name() == "dl") {
					if (pNode->firstChild && pNode->firstChild != nullptr) {
						node->nextSibling = pNode->firstChild;
						pNode->firstChild->previousSibling = node;
					}
					else {
						node->nextSibling = nullptr;
					}
					node->parentNode = pNode;
					pNode->firstChild = node;
					if (node->previousSibling && node->previousSibling != nullptr) {
						node->previousSibling->nextSibling = nullptr;
					}
				}
			}
		}
		src = tagHTML(*myParser.rootNode);
		root = "root";
		myParser.rootNode = new DOMNode(root, emptyStr, 0, len, 0);
		(myParser.rootNode)->set_parent_node(*(myParser.rootNode));
		scriptsToRunOnLoad.clear();
		htmlNodes.clear();
		elsById.clear();
		elsByTagName.clear();
		elsByClassName.clear();
		myParser.cssFilename = "";
		parseHTML(*(myParser.rootNode), *(myParser.rootNode), src, 0, len, emptyStr, htmlNodes);

		string strCern = "www\\cern.css";
		if (myParser.cssFilename == "") {
			myParser.cssFilename = strCern;
		}
		parseCSS(myParser.cssFilename);

		myParser.rootNode->set_zindex(0);

		nodesInOrder.clear();
		nodesInOrder = {};
		//drawDOMNode(*myParser.rootNode, pRenderTarget, pBrush, MainWindow::newHeight, MainWindow::origHeight, MainWindow::newWidth, MainWindow::origWidth, 0, nodesInOrder, 0);

	}
}

class MainWindow : public BaseWindow<MainWindow>
{
	IWICImagingFactory     *m_pWICFactory;
	ID2D1HwndRenderTarget   *pRenderTarget;
	ID2D1BitmapRenderTarget* pCompatibleRenderTarget = NULL;
	ID2D1SolidColorBrush    *pBrush;
	ID2D1SolidColorBrush    *redBrush;
	D2D1_ELLIPSE            ellipse;
	D2D1_POINT_2F           ptMouse;
	SlabContainer			scToolbar;

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	void    DiscardGraphicsResources();
	void    OnPaint();
	void    Resize();

public:
	DOMNode *nodeToExpand;
	bool success;
	float slabLeft, slabRight, regionTop, regionBottom;
	float x1, y1, x2, y2;
	float dX, dY;
	float origWidth = 0.0, origHeight = 0.0, newWidth, newHeight;
	void    OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void    OnLButtonUp();
	void    OnMouseMove(int pixelX, int pixelY, DWORD flags);
	void    visualize(RedBlackTree *rbt, RedBlackNode &x, int level, bool last)
	{
		if (&x == rbt->nil)
		{
			return;
		}

		//RedBlackTree *rbt = &mySlabContainer.RBTSlabLines;
		string str = "";
		for (int i = 0; i < level; i++)
		{
			ffile << "  ";
		}
		ffile << to_string(level) << ":" << to_string(x.key) << endl;

		if (x.left != rbt->nil)
		{
			visualize(rbt, *(x.left), level + 1, x.right != rbt->nil);
		}
		if (x.right != rbt->nil)
		{
			visualize(rbt, *(x.right), level + 1, true);
		}
	};

	MainWindow() : pRenderTarget(NULL), pBrush(NULL), redBrush(NULL)/*,
																					ellipse(D2D1::Ellipse(D2D1::Point2F(), 0, 0))*/,
		ptMouse(D2D1::Point2F())
	{
		std::srand(std::time(nullptr));
		testJSExec();

		loadPage("C:\\c++\\Browser\\draw.html");
	}

	PCWSTR  ClassName() const { return L"Browser Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

// Recalculate drawing layout when the size of the window changes.

void MainWindow::CalculateLayout()
{
}

class DPIScale
{
	static float scaleX;
	static float scaleY;

public:
	static void Initialize(ID2D1Factory *pFactory)
	{
		FLOAT dpiX, dpiY;
		pFactory->GetDesktopDpi(&dpiX, &dpiY);
		scaleX = dpiX / 96.0f;
		scaleY = dpiY / 96.0f;
	}

	template <typename T>
	static D2D1_POINT_2F PixelsToDips(T x, T y)
	{
		return D2D1::Point2F(static_cast<float>(x) / scaleX, static_cast<float>(y) / scaleY);
	}
};

float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;

HRESULT MainWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;

	/*// Create WIC factory
	hr = CoCreateInstance(
	CLSID_WICImagingFactory,
	nullptr,
	CLSCTX_INPROC_SERVER,
	IID_PPV_ARGS(&m_pWICFactory)
	);*/

	if (pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);

		IWICImagingFactory* pWICFactory = NULL;
		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pWICFactory)
		);

		if (SUCCEEDED(hr))
		{
			hr = pWICFactory->CreateBitmap(
				400,
				400,
				GUID_WICPixelFormat32bppRGB,
				WICBitmapCacheOnLoad,
				&pWICBitmap
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFactory->CreateWicBitmapRenderTarget(
				pWICBitmap,
				D2D1::RenderTargetProperties(),
				&pRenderTarget2
			);
		}

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
			const D2D1_COLOR_F color2 = D2D1::ColorF(255, 0, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color2, &redBrush);

			if (SUCCEEDED(hr))
			{
				CalculateLayout();
			}
		}
	}
	return hr;
}

void MainWindow::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	//SafeRelease(m_pBitmap);
	SafeRelease(&pBrush);
	SafeRelease(&redBrush);
}

void setZIndexes(DOMNode& node) {
	if (node.parentNode == nullptr) {
		node.set_zindex(0);
	}
	else {
		node.set_zindex(node.parentNode->get_zindex() + 1);
	}
	DOMNode* childNode = node.firstChild;
	while (childNode != nullptr) {
		setZIndexes(*childNode);
		childNode = childNode->nextSibling;
	}
}

void MainWindow::OnPaint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		hr = DemoApp::CreateDeviceIndependentResources();
		if (SUCCEEDED(hr))
		{
			/*string str;
			for (auto const &ent_i : myParser.styles)
			{
			auto const &outer_key = ent_i.first;
			auto const &inner_map = ent_i.second;
			for (auto const &ent_j : inner_map) {
			auto const &inner_key = ent_j.first;
			auto const &inner_value = ent_j.second;

			str += "styles[" + outer_key + "][" + inner_key + "] = " + inner_value + "\n";
			}
			str += "\n";
			}*/

			/*string str;
			string className = "firstName";
			vector<DOMNode*> *ptrElsOfTag;
			if (elsByClassName.find(className) != elsByClassName.end())
			{
			ptrElsOfTag = &(elsByClassName[className]);

			for (int i = 0, len = ptrElsOfTag->size(); i < len; i++)
			{
			str += (*ptrElsOfTag)[i]->get_tag_name() + "\n";
			str += myParser.printElementAttributes(*((*ptrElsOfTag)[i]));
			}
			}*/

			D2D1_SIZE_F renderTargetSize = pRenderTarget->GetSize();

			if (origWidth == 0.0)
			{
				origWidth = renderTargetSize.width;
			}
			if (origHeight == 0.0)
			{
				origHeight = renderTargetSize.height;
			}
			newWidth = renderTargetSize.width;
			newHeight = renderTargetSize.height;

			viewportScaleX = 0.8;
			viewportScaleY = 0.8;

			//...

			// Create a bitmap from a file.
			IWICImagingFactory     *m_pWICFactory;
			ID2D1Bitmap            *m_pBitmap;

			HRESULT hr = CoCreateInstance(
				CLSID_WICImagingFactory,
				nullptr,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&m_pWICFactory)
			);

			if (SUCCEEDED(hr))
			{
				hr = LoadBitmapFromFile(
					pRenderTarget,
					m_pWICFactory,
					L"icons.png",
					0,
					0,
					&m_pBitmap
				);

				if (SUCCEEDED(hr))
				{
					// Retrieve the size of the bitmap.
					D2D1_SIZE_F size = m_pBitmap->GetSize();

					D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.0, 15.0);

					// Draw a bitmap.
					pRenderTarget->DrawBitmap(
						m_pBitmap,
						D2D1::RectF(
							upperLeftCorner.x,
							upperLeftCorner.y,
							upperLeftCorner.x + size.width * viewportScaleX * newWidth / origWidth,
							upperLeftCorner.y + size.height * viewportScaleY * newHeight / origHeight)
					);

					SafeRelease(&m_pBitmap);
				}
			}

			float yDiff = 10 * newHeight / origHeight;
			hr = m_pDWriteFactory->CreateTextFormat(
				L"Verdana",
				NULL,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				yDiff,
				L"", //locale
				&m_pTextFormat
			);

			if (SUCCEEDED(hr))
			{
				//std::wstring widestr = std::wstring(myParser.get_location().begin(), myParser.get_location().end());
				pRenderTarget->DrawText(
					stringToLPCWSTR(myParser.get_location()),//widestr.c_str(),
					myParser.get_location().length(),
					m_pTextFormat,
					D2D1::RectF(0 * viewportScaleX, 0 * 0.6, renderTargetSize.width * viewportScaleX, 15 * viewportScaleY),
					pBrush,
					D2D1_DRAW_TEXT_OPTIONS_CLIP
				);
			}
 
			//mySlabContainer.preprocessSubdivision(iconShapes, 'x', nilSlab);

			//...

			if (myParser.rootNode == nullptr) {
				return;
			}

			myParser.rootNode->set_zindex(0);

			nodesInOrder.clear();
			nodesInOrder = {};
			drawDOMNode(*myParser.rootNode, pRenderTarget, pBrush, MainWindow::newHeight, MainWindow::origHeight, MainWindow::newWidth, MainWindow::origWidth, 0, nodesInOrder, 0);

			if (!pageLoaded) {
				for (int i = 0, len = scriptsToRunOnLoad.size(); i < len; i++) {
					execAST(scriptsToRunOnLoad[i], globalVariables);
				}
				drawDOMNode(*myParser.rootNode, pRenderTarget, pBrush, MainWindow::newHeight, MainWindow::origHeight, MainWindow::newWidth, MainWindow::origWidth, 0, nodesInOrder, 0);
			}

			setZIndexes(*myParser.rootNode);

			int y = 0;

			/*vector <Shape*> shapesToPreprocess;
			for (int i = 0, len = nodesInOrder.size(); i < len; i++) {
				DOMNode* node = nodesInOrder[i];

				if (node->SlabDecompShape == nullptr && node->get_tag_name() != "TextNode") {
					Shape* shapeShape = new Shape;
					shapeShape->id = mySlabContainer.NextAvailableShapeId++;
					shapeShape->x1 = rint(node->_x);
					shapeShape->x2 = rint(node->_x) + rint(node->_width);
					shapeShape->y1 = rint(node->_y);
					shapeShape->y2 = rint(node->_y) + rint(node->_height);
					shapeShape->node = node;
					node->SlabDecompShape = shapeShape;

					shapesToPreprocess.push_back(shapeShape);
				}
			}
			mySlabContainer.preprocessSubdivision(shapesToPreprocess, 'x', nilSlab);*/

			if (!MainWindow::nodeToExpand)
			{
				MainWindow::nodeToExpand = myParser.rootNode;
			}

			DOMNode *dNode;
			dNode = nodeToExpand;
			do
			{
				dNode->expand = true;
			} while (dNode->parentNode != dNode && (dNode = dNode->parentNode));

			myParser.output = myParser.printChildTagNames(*(myParser.rootNode), 0, false);
			std::wstring widestr = std::wstring(myParser.output.begin(), myParser.output.end());
			pRenderTarget->DrawText(
				widestr.c_str(),
				myParser.output.length(),
				m_pTextFormat,
				D2D1::RectF(renderTargetSize.width * viewportScaleX, 0, renderTargetSize.width, renderTargetSize.height * 0.6),
				pBrush,
				D2D1_DRAW_TEXT_OPTIONS_CLIP
			);


			/*map<int, Shape*>::iterator ShapeIt;
			for (ShapeIt = mySlabContainer.ShapeMembers.begin(); ShapeIt != mySlabContainer.ShapeMembers.end(); ++ShapeIt)
			{
			pRenderTarget->FillRectangle(*(ShapeIt->second->rect), pBrush);
			}*/

			if (MainWindow::success && selRegion != nullptr)
			{
				float sX = newWidth / origWidth, sY = newHeight / origHeight;
				int largestZIndex = -1;
				for (int i = 0, len = selRegion->shapes.size(); i < len; i++)
				{
					DOMNode *node = selRegion->shapes[i]->node;
					if (node)
					{
						if (node->get_zindex() > largestZIndex) {
							largestZIndex = node->get_zindex();
						}
					}
				}

				for (int i = 0, len = selRegion->shapes.size(); i < len; i++)
				{
					DOMNode *node = selRegion->shapes[i]->node;
					if (node && node != nullptr)
					{

						if (node->get_zindex() >= (largestZIndex - 1)) {
							D2D1_RECT_F *rect1 = &D2D1::RectF(node->x * sX, node->y * sY, (node->x + node->width) * sX, (node->y + node->height) * sY);
							pRenderTarget->DrawRectangle(rect1, redBrush);

							/*color2 = D2D1::ColorF(255, 0, 0, 1.0);
							hr = pRenderTarget->CreateSolidColorBrush(color2, &redBrush);*/

							D2D1_COLOR_F color2 = D2D1::ColorF(255, 0, 0, 0.5);
							hr = pRenderTarget->CreateSolidColorBrush(color2, &redBrush);

							//D2D1_RECT_F *nodeBorder = &D2D1::RectF(node->x, node->y, (node->x + node->width), (node->y + node->height));

							if (node->get_zindex() >= largestZIndex) {
								//pRenderTarget->FillRectangle(rect1, redBrush);
							}
						}

						//MessageBox(NULL, stringToLPCWSTR(node->get_text_content()), L"tag_name", MB_OK);

						ID2D1SolidColorBrush* whitebrush;
						D2D1_RECT_F* rectwhite = &D2D1::RectF(renderTargetSize.width * viewportScaleX, renderTargetSize.height * 0.6, renderTargetSize.width, renderTargetSize.height);

						/*color2 = D2D1::ColorF(255, 0, 0, 1.0);
						hr = pRenderTarget->CreateSolidColorBrush(color2, &redBrush);*/

						D2D1_COLOR_F colorwhite = D2D1::ColorF(255, 255, 255, 1.0);
						hr = pRenderTarget->CreateSolidColorBrush(colorwhite, &whitebrush);

						pRenderTarget->FillRectangle(rectwhite, whitebrush);

						myParser.output = myParser.printElementAttributes(*node);
						std::wstring widestr = std::wstring(myParser.output.begin(), myParser.output.end());
						pRenderTarget->DrawText(
							widestr.c_str(),
							myParser.output.length(),
							m_pTextFormat,
							D2D1::RectF(renderTargetSize.width * viewportScaleX, renderTargetSize.height * 0.6, renderTargetSize.width, renderTargetSize.height),
							pBrush,
							D2D1_DRAW_TEXT_OPTIONS_CLIP
						);
					}
				}
			}

			pageLoaded = true;
		}

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);

		//MessageBox(NULL, L"Hello", L"World", MB_OK);
		if (firstWatch) {
			firstWatch = false;
			//WatchDirectory(LPTSTR(_T("C:/c++/browser_win/Browser/Release/")));
		}
		//InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

void MainWindow::Resize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	MainWindow win;
	if (!win.Create(L"Snikta Browser 2021", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	// Run the message loop.

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

void MainWindow::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
	D2D1_SIZE_F renderTargetSize = pRenderTarget->GetSize();
	SetCapture(m_hwnd);
	ptMouse = DPIScale::PixelsToDips(pixelX, pixelY);
	dX = ptMouse.x - x1;
	dY = ptMouse.y - y1;

	const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);
	const float x1 = -dX + dips.x;
	const float y1 = -dY + dips.y;

	MainWindow::x1 = x1;
	MainWindow::y1 = y1;

	if (MainWindow::success && selRegion != nullptr)
	{
		int largestZIndex = -1;
		for (int i = 0, len = selRegion->shapes.size(); i < len; i++)
		{
			DOMNode *node = selRegion->shapes[i]->node;
			if (node)
			{
				if (node->get_zindex() > largestZIndex) {
					largestZIndex = node->get_zindex();
				}

				if (nodeToExpand == node)
				{
					nodeToExpand = node->parentNode;
				}
				else {
					nodeToExpand = node;
				}
			}
		}

		for (int i = 0, len = selRegion->shapes.size(); i < len; i++)
		{
			if (i >= selRegion->shapes.size()) {
				break;
			}
			DOMNode *node = selRegion->shapes[i]->node;
			vector<ASTNode> clickFuncs = selRegion->shapes[i]->eventHandlers["mousedown"];
			for (int j = 0, jLen = clickFuncs.size(); j < jLen; j++) {
				ASTNode astFunc = resolveRuntimeObject(clickFuncs[j]);
				Scope myScope;
				myScope.__parent = &globalVariables;
				myScope.ScopeArray["this"] = runtimeObjects[node->astRuntimeId];
				myScope.ScopeArray["pageX"] = ASTNode((long double)MainWindow::x1);
				myScope.ScopeArray["pageY"] = ASTNode((long double)MainWindow::y1 - 107);
				execAST(*astFunc.ASTNodeFunc, myScope);
				return;
			}
			if (node && node != nullptr)
			{
				if (node->get_tag_name() == "a" && node->get_zindex() >= (largestZIndex - 1)) {
					MessageBox(NULL, stringToLPCWSTR("Navigating to \"" + node->attributes["href"] + "\""), L"<a>.href", MB_OK);
					loadPage("C:\\c++\\Browser\\www\\" + node->attributes["href"]);
				}
			}
		}
	}

	for (int j = 0, jLen = eventListeners["mousedown"].size(); j < jLen; j++) {
		ASTNode astFunc = resolveRuntimeObject(eventListeners["mousedown"][j]);
		Scope myScope;
		myScope.__parent = astFunc.ASTNodeFunc->arguments;
		myScope.ScopeArray["pageX"] = ASTNode((long double)MainWindow::x1);
		myScope.ScopeArray["pageY"] = ASTNode((long double)MainWindow::y1 - 107);
		if (astFunc.ASTNodeFunc == nullptr) {
			continue;
		}
		execAST(*astFunc.ASTNodeFunc, myScope);
	}

	InvalidateRect(m_hwnd, NULL, FALSE);
}

void MainWindow::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
	if (pRenderTarget == nullptr) {
		OutputDebugStringW(L"pRenderTarget was nullptr");
		return;
	}
	D2D1_SIZE_F renderTargetSize = pRenderTarget->GetSize();

	globalVariables.ScopeArray["innerWidth"] = ASTNode((long double)renderTargetSize.width * viewportScaleX);
	globalVariables.ScopeArray["innerHeight"] = ASTNode((long double)renderTargetSize.height * viewportScaleY);

	//if (flags & MK_LBUTTON)
	//{
	const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);
	const float x1 = -dX + dips.x;
	const float y1 = -dY + dips.y;

	MainWindow::x1 = x1;
	MainWindow::y1 = y1;

	MainWindow::x2 = dips.x / (newWidth / origWidth);
	MainWindow::y2 = dips.y / (newHeight / origHeight);
	
	if (mySlabContainer.ShapeMembers.size() < 6)
	{
		vector<Shape*> shapesToPreprocess;
		int x = 0, xDiff = 107 * viewportScaleX;
		for (int i = 0, len = 5; i < len; i++)
		{
			int shapeId = mySlabContainer.NextAvailableShapeId++;
			int x1, x2, y1, y2;
			Shape* newShape = new Shape;
			newShape->id = shapeId;
			x1 = newShape->x1 = x;
			x2 = newShape->x2 = x + xDiff;
			y1 = newShape->y1 = 0;
			y2 = newShape->y2 = 107 * viewportScaleY;

			shapesToPreprocess.push_back(newShape);

			x += xDiff;
		}

		//if (!mySlabContainer.ShapeMembers.size())
		{
			vector<string> outputByLine;

			splitString(myParser.output, '\n', outputByLine);
			int y = 0;
			for (int i = 0, len = nodesInOrder.size(); i < len; i++)
			{
				if (nodesInOrder[i]->get_tag_name() == "TextNode") {
					continue;
				}
				int shapeId = mySlabContainer.NextAvailableShapeId++;
				int x1, x2, y1, y2;

				IDWriteTextLayout* pTextLayout_;
				string tName = nodesInOrder[i]->get_tag_name();
				std::wstring widestr = std::wstring(tName.begin(), tName.end());

				HRESULT hr = m_pDWriteFactory->CreateTextLayout(
					widestr.c_str(),      // The string to be laid out and formatted.
					tName.length(),  // The length of the string.
					m_pTextFormat,  // The text format to apply to the string (contains font information, etc).
					renderTargetSize.width,         // The width of the layout box.
					renderTargetSize.height,        // The height of the layout box.
					&pTextLayout_  // The IDWriteTextLayout interface pointer.
				);

				DWRITE_TEXT_METRICS metrics;
				pTextLayout_->GetMetrics(&metrics);

				Shape* newShape = new Shape;
				newShape->id = shapeId;
				x1 = newShape->x1 = renderTargetSize.width * viewportScaleX;
				x2 = newShape->x2 = renderTargetSize.width;
				y1 = newShape->y1 = y;
				y2 = newShape->y2 = y + metrics.height;
				newShape->node = nodesInOrder[i];

				Shape* shapeShape = new Shape;
				shapeShape->id = mySlabContainer.NextAvailableShapeId++;
				shapeShape->x1 = nodesInOrder[i]->x;
				shapeShape->x2 = nodesInOrder[i]->x + nodesInOrder[i]->width;
				shapeShape->y1 = nodesInOrder[i]->y;
				shapeShape->y2 = nodesInOrder[i]->y + nodesInOrder[i]->height;
				shapeShape->node = nodesInOrder[i];

				nodesInOrder[i]->SlabDecompShape = shapeShape;

				shapesToPreprocess.push_back(shapeShape);
				shapesToPreprocess.push_back(newShape);

				y += metrics.height;
			}
			mySlabContainer.preprocessSubdivision(shapesToPreprocess, 'x', nilSlab);

			/*if (mySlabContainer.ShapeMembers.size() > 5) {
				pageLoaded = true;
				for (int i = 0, len = eventListenersToBindArgs.size(); i < len; i++) {
					predefinedFunctions["addEventListener"](eventListenersToBindArgs[i], eventListenersToBindScopes[i]);
				}
			}*/

			/*				ffile.open("RBT.txt", std::ios_base::in | std::ios_base::trunc);
			MainWindow::visualize(&mySlabContainer.RBTSlabLines, *(mySlabContainer.RBTSlabLines.root), 0, true);
			map<int, Slab*>::iterator SlabIt;
			for (SlabIt = mySlabContainer.SlabLinesByLeft.begin(); SlabIt != mySlabContainer.SlabLinesByLeft.end(); ++SlabIt)
			{
			//int _x1 = mySlabContainer.ShapeMembers[i + 1]->x1;
			//if (mySlabContainer.SlabLinesByLeft.find(_x1) != mySlabContainer.SlabLinesByLeft.end())
			//{
			MainWindow::visualize(SlabIt->second->RBTRegions, *(SlabIt->second->RBTRegions->root), 0, true);
			ffile << endl << "-------" << endl;
			//}
			}
			ffile.close();*/
		}
	}

	RedBlackTree* rbt = &(mySlabContainer.RBTSlabLines);
	RedBlackNode* node = rbt->closest(x2);
	Slab* slab = &nilSlab;
	bool success = false, slabExists = false;

	MainWindow::success = false;

	if (node->key > x2)
	{
		int nodeKey = rbt->predecessor(node)->key;
		slabExists = mySlabContainer.SlabLinesByLeft.find(nodeKey) != mySlabContainer.SlabLinesByLeft.end();
		if (slabExists)
		{
			slab = mySlabContainer.SlabLinesByLeft[nodeKey];
		}
	}
	else
	{
		slabExists = mySlabContainer.SlabLinesByLeft.find(node->key) != mySlabContainer.SlabLinesByLeft.end();
		if (slabExists)
		{
			slab = mySlabContainer.SlabLinesByLeft[node->key];
		}
	}

	if (slabExists && x2 >= slab->leftX && x2 <= slab->rightX)
	{
		rbt = slab->RBTRegions;
		node = rbt->closest(y2);

		bool regionExists;
		Region *region = &nilRegion;
		if (node->key > y2)
		{
			int regionKey = rbt->predecessor(node)->key;
			regionExists = slab->RegionsByTop.find(regionKey) != slab->RegionsByTop.end();
			if (regionExists)
			{
				region = slab->RegionsByTop[regionKey];
			}
		}
		else
		{
			regionExists = slab->RegionsByTop.find(node->key) != slab->RegionsByTop.end();
			if (regionExists)
			{
				region = slab->RegionsByTop[node->key];
			}
		}

		if (regionExists && y2 >= region->topY && y2 <= region->bottomY)
		{
			selRegion = region;

			MainWindow::slabLeft = slab->leftX;
			MainWindow::slabRight = slab->rightX;

			MainWindow::regionTop = region->topY;
			MainWindow::regionBottom = region->bottomY;

			MainWindow::success = true;
		}
	}
	//}

	for (int j = 0, jLen = eventListeners["mousemove"].size(); j < jLen; j++) {
		ASTNode astFunc = resolveRuntimeObject(eventListeners["mousemove"][j]);
		Scope myScope;
		myScope.__parent = astFunc.ASTNodeFunc->arguments;
		myScope.ScopeArray["pageX"] = ASTNode((long double)MainWindow::x1);
		myScope.ScopeArray["pageY"] = ASTNode((long double)MainWindow::y1 - 107);
		execAST(*astFunc.ASTNodeFunc, myScope);
	}

	InvalidateRect(m_hwnd, NULL, FALSE);
}

void MainWindow::OnLButtonUp()
{
	for (int j = 0, jLen = eventListeners["mouseup"].size(); j < jLen; j++) {
		ASTNode astFunc = resolveRuntimeObject(eventListeners["mouseup"][j]);
		Scope myScope;
		myScope.__parent = astFunc.ASTNodeFunc->arguments;
		if (astFunc.ASTNodeFunc == nullptr) {
			continue;
		}
		execAST(*astFunc.ASTNodeFunc, myScope);
	}

	ReleaseCapture();
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;  // Fail CreateWindowEx.
		}

		DPIScale::Initialize(pFactory);
		return 0;

	case WM_DESTROY:
		DiscardGraphicsResources();
		SafeRelease(m_pWICFactory);
		SafeRelease(&pFactory);
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		//OnPaint();
		return 0;

	case WM_SIZE:
		Resize();
		return 0;

	case WM_LBUTTONDOWN:
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;

	case WM_LBUTTONUP:
		OnLButtonUp();
		return 0;

	case WM_MOUSEMOVE:
		/*selRegion = nullptr;
		mySlabContainer.RBTSlabLines.deleteAllNodes();
		for (auto it = mySlabContainer.SlabLinesByLeft.begin(); it != mySlabContainer.SlabLinesByLeft.end(); it++) {
			it->second->RBTRegions->deleteAllNodes();
			for (auto itRegion = it->second->RegionsByTop.begin(); itRegion != it->second->RegionsByTop.end(); itRegion++) {
				delete itRegion->second;
			}
		}
		mySlabContainer.SlabLinesByLeft.clear();
		for (auto it = mySlabContainer.ShapeMembers.begin(); it != mySlabContainer.ShapeMembers.end(); it++) {
			delete it->second->rect;
			delete it->second;
		}
		mySlabContainer.ShapeMembers.clear();*/
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		OnPaint();
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
