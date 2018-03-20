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
#include "text.h"
#include "parser.h"
#include "parseCSS.h"
#include "parseHTML.h"
#include "SafeRelease.h"
#include "LoadBitmapFromFile.h"
#include "drawDOMNode.h"
#include "RedBlackTree.h"
#include "SlabDecomposition.h"
#include "deleteShape.h"

std::ofstream ffile;

class MainWindow : public BaseWindow<MainWindow>
{
	IWICImagingFactory     *m_pWICFactory;
	ID2D1Factory            *pFactory;
	ID2D1HwndRenderTarget   *pRenderTarget;
	ID2D1SolidColorBrush    *pBrush;
	ID2D1SolidColorBrush    *redBrush;
	D2D1_ELLIPSE            ellipse;
	D2D1_POINT_2F           ptMouse;
	SlabContainer			mySlabContainer;
	SlabContainer			scToolbar;

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	void    DiscardGraphicsResources();
	void    OnPaint();
	void    Resize();

public:
	Region *selRegion;
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

	MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL), redBrush(NULL)/*,
		ellipse(D2D1::Ellipse(D2D1::Point2F(), 0, 0))*/,
		ptMouse(D2D1::Point2F())
	{
		string fname = "layout.html";
		string src;
		int len = readTextFile(fname, src);

		string emptyStr = "";
		string root = "root";

		myParser.rootNode = new DOMNode(root, emptyStr, 0, len, 0);
		(myParser.rootNode)->set_parent_node(*(myParser.rootNode));

		parseHTML(*(myParser.rootNode), *(myParser.rootNode), src, 0, len, emptyStr);
		parseCSS(myParser.cssFilename);
	}

	PCWSTR  ClassName() const { return L"Browser Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

// Recalculate drawing layout when the size of the window changes.

void MainWindow::CalculateLayout()
{
	/*if (pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		const float x = size.width / 2;
		const float y = size.height / 2;
		const float radius = min(x, y);
		ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
	}*/
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

					D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.0, 0.0);

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

			vector<Shape*> shapesToPreprocess;
			int x = 0, xDiff = 107 * viewportScaleX;
			for (int i = 0, len = 5; i < len; i++)
			{
				int shapeId = mySlabContainer.NextAvailableShapeId++;
				int x1, x2, y1, y2;
				Shape *newShape = new Shape;
				newShape->id = shapeId;
				x1 = newShape->x1 = x;
				x2 = newShape->x2 = x + xDiff;
				y1 = newShape->y1 = 0;
				y2 = newShape->y2 = 107 * viewportScaleY;

				shapesToPreprocess.push_back(newShape);

				x += xDiff;
			}
			//mySlabContainer.preprocessSubdivision(iconShapes, 'x', nilSlab);

			//...

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

			drawDOMNode(*myParser.rootNode, pRenderTarget, pBrush, MainWindow::newHeight, MainWindow::origHeight, MainWindow::newWidth, MainWindow::origWidth, 0);

			myParser.output = myParser.printChildTagNames(*(myParser.rootNode), 0, false);
			std::wstring widestr = std::wstring(myParser.output.begin(), myParser.output.end());
			pRenderTarget->DrawText(
				widestr.c_str(),
				myParser.output.length(),
				m_pTextFormat,
				D2D1::RectF(renderTargetSize.width * viewportScaleX, 0, renderTargetSize.width, renderTargetSize.height),
				pBrush
			);

			if (!mySlabContainer.ShapeMembers.size())
			{
				vector<string> outputByLine;

				splitString(myParser.output, '\n', outputByLine);
				int y = 0;
				for (int i = 0, len = outputByLine.size(); i < len; i++)
				{
					int shapeId = mySlabContainer.NextAvailableShapeId++;
					int x1, x2, y1, y2;
					Shape *newShape = new Shape;
					newShape->id = shapeId;
					x1 = newShape->x1 = renderTargetSize.width * viewportScaleX;
					x2 = newShape->x2 = renderTargetSize.width;
					y1 = newShape->y1 = y;
					y2 = newShape->y2 = y + yDiff;

					shapesToPreprocess.push_back(newShape);

					y += yDiff;
				}
				mySlabContainer.preprocessSubdivision(shapesToPreprocess, 'x', nilSlab);

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

			/*map<int, Shape*>::iterator ShapeIt;
			for (ShapeIt = mySlabContainer.ShapeMembers.begin(); ShapeIt != mySlabContainer.ShapeMembers.end(); ++ShapeIt)
			{
				pRenderTarget->FillRectangle(*(ShapeIt->second->rect), pBrush);
			}*/

			if (MainWindow::success)
			{
				float sX = newWidth / origWidth, sY = newHeight / origHeight;
				D2D1_RECT_F *rect1 = &D2D1::RectF(slabLeft * sX, regionTop * sY, slabRight * sX, regionBottom * sY);
				pRenderTarget->DrawRectangle(rect1, redBrush);
			}
		}

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
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
	if (!win.Create(L"Snikta Browser 2018", WS_OVERLAPPEDWINDOW))
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
	SetCapture(m_hwnd);
	ptMouse = DPIScale::PixelsToDips(pixelX, pixelY);
	dX = ptMouse.x - x1;
	dY = ptMouse.y - y1;

	InvalidateRect(m_hwnd, NULL, FALSE);
}

void MainWindow::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
	//if (flags & MK_LBUTTON)
	//{
		const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);
		const float x1 = -dX + dips.x;
		const float y1 = -dY + dips.y;

		MainWindow::x1 = x1;
		MainWindow::y1 = y1;

		MainWindow::x2 = dips.x / (newWidth / origWidth);
		MainWindow::y2 = dips.y / (newHeight / origHeight);

		RedBlackTree *rbt = &(mySlabContainer.RBTSlabLines);
		RedBlackNode *node = rbt->closest(x2);
		Slab *slab = &nilSlab;
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
				MainWindow::selRegion = region;

				MainWindow::slabLeft = slab->leftX;
				MainWindow::slabRight = slab->rightX;

				MainWindow::regionTop = region->topY;
				MainWindow::regionBottom = region->bottomY;

				MainWindow::success = true;
			}
		}
	//}

	InvalidateRect(m_hwnd, NULL, FALSE);
}

void MainWindow::OnLButtonUp()
{
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
		OnPaint();
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
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		OnPaint();
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

