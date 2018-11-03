#pragma once
// "drawDOMNode.h"
#include <string>
#include <string.h>
#include <vector>
#include "DOMNode.h"
#include "substrReplace.h"
#include "LoadBitmapFromFile.h"
const vector<string> styleSkipList = { "box-shadow", "margin", "position", "display", "left", "top", "width", "height" };

float viewportScaleX, viewportScaleY;
int yiy = 0;
void drawDOMNode(DOMNode &node, ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush, int newHeight, int origHeight, int newWidth, int origWidth, int xy, vector<DOMNode*> &nodesInOrder, int level) {
	DOMNode *parentNode = node.parentNode;
	bool x_set = false, y_set = false, width_set = false, height_set = false;
	double totalWidth, totalHeight;

	//node.expand = false;
	node.x = 0;
	node.y = 107 * newHeight / origHeight;
	node.width = 0;
	node.height = 0;
	node.width_set = false;
	node.height_set = false;
	node.x_set = false;
	node.y_set = false;

	if (node.get_tag_name() != "root" && node.get_tag_name() != "TextNode")
	{
		nodesInOrder.push_back(&node);
	}

	string tagName = node.get_tag_name();
	D2D1_SIZE_F renderTargetSize = pRenderTarget->GetSize();

	//ctx.shadowBlur = ctx.shadowOffsetX = ctx.shadowOffsetY = 0;
	//ctx.shadowColor = "transparent";

	if (tagName == "head") {
		return;
	}

	if (parentNode->get_tag_name() != "root") {
		if (node.get_idx() == 0)
		{
			node.x = parentNode->x;
			node.y = parentNode->y;
		}
		else
		{
			node.x = node.previousSibling->x;
			//if (node.previousSibling->style["display"].find("inline") != -1)
			{
				string s1 = to_string(node.previousSibling->width);
				std::wstring widestr = std::wstring(s1.begin(), s1.end());
				OutputDebugStringW(widestr.c_str());
				node.x += node.previousSibling->width;
			}
			/*if (node.get_tag_name() == "TextNode")
			{
			node.x_set = true;
			node.y_set = true;
			}*/
			node.y = node.previousSibling->y;
			if ((node.style["display"] == "block") || (node.previousSibling->style["display"] == "block"))
			{
				node.y += node.previousSibling->height;
			}
		}

		node.x_set = node.y_set = true;

		if (tagName == "body") {
			node.width = node.parentNode->width;
			node.height = node.parentNode->height;
			node.width_set = node.height_set = true;
		}

		node.style["display"] = "inline";

		if (true || (node.attributes).size()) {
			map<string, string>::iterator it;
			map<string, string> *elStyles;

			elStyles = &(myParser.styles[node.get_tag_name()]);
			for (it = elStyles->begin(); it != elStyles->end(); ++it) {
				node.style[it->first] = it->second;
			}

			elStyles = &(myParser.styles[node.get_id()]);
			for (it = elStyles->begin(); it != elStyles->end(); ++it) {
				node.style[it->first] = it->second;
			}

			if (node.attributes.find("class") != node.attributes.end()) {
				vector<string> *classList = &(node.classList);
				for (auto className = classList->begin(); className != classList->end(); ++className)
				{
					if (myParser.styles.find(*className) != myParser.styles.end())
					{
						elStyles = &(myParser.styles[*className]);
						for (it = elStyles->begin(); it != elStyles->end(); ++it) {
							node.style[it->first] = it->second;
						}
					}
				}
			}

			if (node.style["display"] == "block") {
				//if (!(x_set && y_set))
				//{
				node.x = parentNode->x + stod(substrReplace(node.style["left"] == "" ? "0" : node.style["left"], "%", "")) / 100 * parentNode->width;
				if (node.style["position"] == "absolute") {
					node.y = parentNode->y + stod(substrReplace(node.style["top"] == "" ? "0" : node.style["top"], "%", "")) / 100 * parentNode->height;
				}
				//}
				//if (!width_set)
				//{
				node.width = stod(substrReplace(node.style["width"] == "" ? "100" : node.style["width"], "%", "")) / 100 * parentNode->width;
				//}
				//if (!height_set)
				//{
				node.height = node.style["height"] == "" ? 12 : (stod(substrReplace(node.style["height"] == "" ? "0" : node.style["height"], "%", "")) / 100 * parentNode->height);
				//}
				node.x_set = node.y_set = node.width_set = node.height_set = true;
			}
		}

		for (auto const &ent_i : parentNode->style)
		{
			if (find(styleSkipList.begin(), styleSkipList.end(), ent_i.first) == styleSkipList.end()) {
				//if (key != "margin" || node.style["display"] == "block") {
				if (node.style[ent_i.first] == "") {
					node.style[ent_i.first] = ent_i.second;
				}
				//}
			}
		}
	}
	else {
		node.x = 0;
		node.y = 0;
		node.width = renderTargetSize.width * viewportScaleX;
		node.height = renderTargetSize.height * viewportScaleY;
		node.x_set = node.y_set = node.width_set = node.height_set = true;
	}

	if (!node.x_set) { node.x = parentNode->x; }
	if (!node.y_set) { node.y = parentNode->y; }
	if (!node.width_set) { node.width = parentNode->width; }
	if (!node.height_set) { node.height = parentNode->height; }

	if (node.style["display"].find("block") != string::npos && node.style["margin"] != "") {
		node.marginX = node.marginX || stod(substrReplace(node.style["margin"] == "" ? "0" : node.style["margin"], "%", "")) / 100 * node.parentNode->width;
		node.marginY = node.marginY || stod(substrReplace(node.style["margin"] == "" ? "0" : node.style["margin"], "%", "")) / 100 * node.parentNode->height;
	}

	if (node.get_tag_name() == "div") {
		string bg = node.style["background"];
		ID2D1LinearGradientBrush *m_pLinearGradientBrush;
		if (bg != "")
		{
			int len = bg.length();
			string lg = "linear-gradient(";
			D2D1_RECT_F rect1 = D2D1::RectF(
				node.x,
				node.y,
				node.x + node.width,
				node.y + node.height
			);
			if (bg.find(lg) == string::npos) {
				vector<string> rgbValues;
				splitString(node.style["background"].substr(0, node.style["background"].length() - 1).substr(4), ',', rgbValues);
				D2D1_COLOR_F color = D2D1::ColorF(stof(rgbValues[0]) / 255, stof(rgbValues[1]) / 255, stof(rgbValues[2]) / 255);
				pRenderTarget->CreateSolidColorBrush(color, &pBrush);
				pRenderTarget->FillRectangle(&rect1, pBrush);
			}
			else {
				bg = bg.substr(0, bg.length() - 1).substr(lg.length());

				int bgI = 0, bgLen = bg.length();
				vector<string> colors;
				string color;
				while (bgI < bgLen) {
					if ((bgI == bgLen - 1) || (bg[bgI] == ',' && !isdigit(bg[bgI - 1])))
					{
						if (bgI == bgLen - 1)
						{
							color += bg[bgI];
						}
						colors.push_back(color);
						color = "";
					}
					else {
						color += bg[bgI];
					}
					bgI++;
				}
				len = colors.size();
				ID2D1GradientStopCollection *pGradientStops = NULL;
				D2D1_GRADIENT_STOP *gradStops = new D2D1_GRADIENT_STOP[len - 1]; // delete later
				for (int i = 1; i < len; i++)
				{
					vector<string> rgbValues;
					splitString(colors[i].substr(0, colors[i].length() - 1).substr(4), ',', rgbValues);
					gradStops[i - 1].color = D2D1::ColorF(stof(rgbValues[0]) / 255, stof(rgbValues[1]) / 255, stof(rgbValues[2]) / 255);
					gradStops[i - 1].position = (float(i) - 1) / (float(len) - 2);
				}
				HRESULT hr = pRenderTarget->CreateGradientStopCollection(
					gradStops,
					len - 1,
					D2D1_GAMMA_2_2,
					D2D1_EXTEND_MODE_CLAMP,
					&pGradientStops
				);
				if (SUCCEEDED(hr))
				{
					hr = pRenderTarget->CreateLinearGradientBrush(
						D2D1::LinearGradientBrushProperties(
							D2D1::Point2F(node.x, node.y),
							D2D1::Point2F(node.x + node.width, node.y)),
						pGradientStops,
						&m_pLinearGradientBrush
					);
					//m_pLinearGradientBrush->Release();
					pRenderTarget->FillRectangle(&rect1, m_pLinearGradientBrush);
				}
				SafeRelease(pGradientStops);
				SafeRelease(m_pLinearGradientBrush);
				delete[] gradStops;
			}
		}
	}

	if (node.get_tag_name() == "TextNode") {
		float yDiff = 10 * newHeight / origHeight;
		HRESULT hr = m_pDWriteFactory->CreateTextFormat(
			L"Verdana",
			NULL,
			node.style["font-weight"] == "bold" ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
			node.style["font-style"] == "italic" ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			yDiff,
			L"", //locale
			&m_pTextFormat
		);

		D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0, 1.0f);
		pRenderTarget->CreateSolidColorBrush(color, &pBrush);

		string txt = node.get_text_content();
		std::wstring widestr = std::wstring(txt.begin(), txt.end());
		pRenderTarget->DrawText(
			widestr.c_str(),
			txt.length(),
			m_pTextFormat,
			D2D1::RectF(node.x, node.y, node.x + node.width, node.y + node.height),
			pBrush
		);
	}
	else if (node.get_tag_name() == "img") {

		// Create a bitmap from a file.
		std::wstring widestr = std::wstring(node.attributes["src"].begin(), node.attributes["src"].end());

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
				widestr.c_str(),
				0,
				0,
				&m_pBitmap
			);

			if (SUCCEEDED(hr))
			{
				// Retrieve the size of the bitmap.
				D2D1_SIZE_F size = m_pBitmap->GetSize();

				D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(node.x, node.y);

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
	}
	else if (node.get_child_count())
	{
		if (node.parentNode->get_tag_name() != "root")
		{
			if (node.style["display"] == "block" || node.style["display"] == "inline-block") {
				node.width = stod(substrReplace(node.style["width"] == "" ? "100" : node.style["width"], "%", "")) / 100 * node.parentNode->width;
				node.width_set = true;
			}
			if (node.style["display"] == "block")
			{
				node.height = stod(substrReplace(node.style["height"] == "" ? "100" : node.style["height"], "%", "")) / 100 * node.parentNode->height;
				node.height_set = true;
			}
		}
		else
		{
			node.width = renderTargetSize.width * viewportScaleX;
			node.height = renderTargetSize.height * viewportScaleY;
			node.width_set = node.height_set = true;
		}

		if (node.get_child_count())
		{
			DOMNode *child = node.firstChild;
			int xy = 0;
			while (child)
			{
				if (child->style["width"] != "") {
					child->width = stod(substrReplace(child->style["width"] == "" ? "0" : child->style["width"], "%", "")) / 100 * node.width;
					child->width_set = true;
				}
				if (child->style["height"] != "") {
					child->height = stod(substrReplace(child->style["height"] == "" ? "0" : child->style["height"], "%", "")) / 100 * node.height;
					child->height_set = true;
				}
				drawDOMNode(*child, pRenderTarget, pBrush, newHeight, origHeight, newWidth, origWidth, xy, nodesInOrder, level + 1);
				xy += child->width;
				child = child->nextSibling;
			}
		}
	}
	if (node.style["width"] == "" || node.style["height"] == "") {
		totalWidth = 0;
		totalHeight = 0;
		//console.log(node.tagName,node.width);
		if (node.get_child_count())
		{
			DOMNode *child = node.firstChild;
			int idx = 0;
			while (child)
			{
				totalWidth += child->width;
				if (child->style["display"] == "block" || idx == 0 || child->get_tag_name() == "TextNode")
				{
					totalHeight += child->height;
				}
				idx++;
				child = child->nextSibling;
			}
		}
		if (node.style["display"] == "inline")
		{
			node.width = totalWidth;
			node.width_set = true;
		}
		node.height = totalHeight;
		node.height_set = true;
		//console.log(node.tagName,node.width);
		//console.log('\n');

		if (node.get_tag_name() == "TextNode") {
			// Create a text layout using the text format.
			string s3 = node.get_text_content();
			std::wstring widestr = std::wstring(s3.begin(), s3.end());

			IDWriteTextLayout *pTextLayout_;

			HRESULT hr = m_pDWriteFactory->CreateTextLayout(
				widestr.c_str(),      // The string to be laid out and formatted.
				s3.length(),  // The length of the string.
				m_pTextFormat,  // The text format to apply to the string (contains font information, etc).
				renderTargetSize.width,         // The width of the layout box.
				renderTargetSize.height,        // The height of the layout box.
				&pTextLayout_  // The IDWriteTextLayout interface pointer.
			);

			DWRITE_TEXT_METRICS metrics;
			pTextLayout_->GetMetrics(&metrics);
			node.width = metrics.widthIncludingTrailingWhitespace;
			node.height = metrics.height;

			SafeRelease(pTextLayout_);
		}
	}
}