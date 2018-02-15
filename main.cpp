#pragma once
#include <windows.h>
#include <wincodec.h> 
#include <commdlg.h> 
#include <Windowsx.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "basewin.h"
#include "WICViewerD2D.h"
#include "text.h"
#include "parser.h"
#include "parseCSS.h"
#include "parseHTML.h"
#include "drawDOMNode.h"

template <typename T>
inline void SafeRelease(T *&p)
{
	if (nullptr != p)
	{
		p->Release();
		p = nullptr;
	}
}

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

HRESULT LoadBitmapFromFile(
	ID2D1RenderTarget *pRenderTarget,
	IWICImagingFactory *pIWICFactory,
	PCWSTR uri,
	UINT destinationWidth,
	UINT destinationHeight,
	ID2D1Bitmap **ppBitmap
)
{
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}


	if (SUCCEEDED(hr))
	{

		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);

	}


	if (SUCCEEDED(hr))
	{

		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
	}

	if (SUCCEEDED(hr))
	{

		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
		);
	}

	SafeRelease(&pDecoder);
	SafeRelease(&pSource);
	SafeRelease(&pStream);
	SafeRelease(&pConverter);
	SafeRelease(&pScaler);

	return hr;
}

class MainWindow : public BaseWindow<MainWindow>
{
	ID2D1Factory            *pFactory;
	ID2D1HwndRenderTarget   *pRenderTarget;
	ID2D1SolidColorBrush    *pBrush;
	D2D1_ELLIPSE            ellipse;
	D2D1_POINT_2F           ptMouse;
	IWICImagingFactory     *m_pWICFactory;
	ID2D1Bitmap            *m_pBitmap;

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	void    DiscardGraphicsResources();
	void    OnPaint();
	void    Resize();

public:
	
	float x1, y1;
	float dX, dY;
	void    OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void    OnLButtonUp();
	void    OnMouseMove(int pixelX, int pixelY, DWORD flags);

	MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL)/*,
		ellipse(D2D1::Ellipse(D2D1::Point2F(), 0, 0))*/,
		ptMouse(D2D1::Point2F())
	{
	}

	PCWSTR  ClassName() const { return L"Circle Window Class"; }
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

	// Create WIC factory 
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_pWICFactory)
	);

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
	SafeRelease(m_pBitmap);
	SafeRelease(&pBrush);
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
			// don't do this every paint
			string fname = "layout.html";
			string src;
			int len = readTextFile(fname, src);

			string emptyStr = "";
			string root = "root";

			DOMNode rootNode(root, emptyStr, 0, len, 0);
			rootNode.set_parent_node(rootNode);

			parseHTML(rootNode, rootNode, src, 0, len, emptyStr);
			parseCSS(myParser.cssFilename);

			// h/t https://stackoverflow.com/a/27344958
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

			string str = myParser.printChildTagNames(rootNode, 0, false);
			std::wstring widestr = std::wstring(str.begin(), str.end());
			static const wchar_t* sc_text = widestr.c_str();
			D2D1_SIZE_F renderTargetSize = pRenderTarget->GetSize();

			pRenderTarget->DrawText(
				sc_text,
				str.length(),
				m_pTextFormat,
				D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
				pBrush
			);
		}


		/*// Create a bitmap from a file.
		hr = LoadBitmapFromFile(
			pRenderTarget,
			m_pWICFactory,
			L"apollo.jpg",
			0,
			0,
			&m_pBitmap
		);

		// Retrieve the size of the bitmap.
		D2D1_SIZE_F size = m_pBitmap->GetSize();

		D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(MainWindow::x1, MainWindow::y1);

		// Draw a bitmap.
		pRenderTarget->DrawBitmap(
			m_pBitmap,
			D2D1::RectF(
				upperLeftCorner.x,
				upperLeftCorner.y,
				upperLeftCorner.x + size.width,
				upperLeftCorner.y + size.height)
		);*/

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
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
	if (flags & MK_LBUTTON)
	{
		const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);
		const float x1 = -dX + dips.x;
		const float y1 = -dY + dips.y;

		MainWindow::x1 = x1;
		MainWindow::y1 = y1;

		MainWindow::OnPaint();

		InvalidateRect(m_hwnd, NULL, FALSE);
	}
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
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

