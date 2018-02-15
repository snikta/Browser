// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE. 
// 
// Copyright (c) Microsoft Corporation. All rights reserved 

#include <windows.h> 
#include <wincodec.h> 
#include <commdlg.h> 
#include <d2d1.h> 
#include "WICViewerD2D.h" 

template <typename T>
inline void SafeRelease(T *&p)
{
	if (nullptr != p)
	{
		p->Release();
		p = nullptr;
	}
}

/******************************************************************
*  Application entrypoint                                         *
******************************************************************/

/*int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR pszCmdLine,
	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(pszCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr))
	{
		{
			DemoApp app;
			hr = app.Initialize(hInstance);

			if (SUCCEEDED(hr))
			{
				BOOL fRet;
				MSG msg;

				// Main message loop: 
				while ((fRet = GetMessage(&msg, nullptr, 0, 0)) != 0)
				{
					if (fRet == -1)
					{
						break;
					}
					else
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
			}
		}
		CoUninitialize();
	}

	return 0;
}*/

/******************************************************************
*  Initialize member data                                         *
******************************************************************/

DemoApp::DemoApp()
	:
	m_pD2DBitmap(nullptr),
	m_pConvertedSourceBitmap(nullptr),
	m_pIWICFactory(nullptr),
	m_pD2DFactory(nullptr),
	m_pRT(nullptr)
{
}

/******************************************************************
*  Tear down resources                                            *
******************************************************************/

DemoApp::~DemoApp()
{
	SafeRelease(m_pD2DBitmap);
	SafeRelease(m_pConvertedSourceBitmap);
	SafeRelease(m_pIWICFactory);
	SafeRelease(m_pD2DFactory);
	SafeRelease(m_pRT);
}