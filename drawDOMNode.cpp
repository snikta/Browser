#pragma once
// "drawDOMNode.cpp"
#include "drawDOMNode.h"

using namespace std;

// START: https://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode
wstring s2ws(const string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	wstring r(buf);
	delete[] buf;
	return r;
}
LPCWSTR stringToLPCWSTR(string str) {
	std::wstring stemp = s2ws(str);
	LPCWSTR result = stemp.c_str();
	return result;
}
// END: https://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode*/

const vector<string> styleSkipList = { "box-shadow", "margin", "position", "display", "left", "top", "width", "height" };
float viewportScaleX, viewportScaleY;

double pxToPc(string px, double denominator) {
	if (px.substr(px.size() - 1, 1) == "%") {
		return stod(px.substr(0, px.size() - 1));
	}
	if (px.size() >= 2 && px.substr(px.size() - 2, 2) == "px") {
		return stod(px.substr(0, px.size() - 2));/* / denominator * 100;*/
	}
	return stod(px);
}

void drawBitmap(std::wstring widestr, DOMNode& node, ID2D1HwndRenderTarget* pRenderTarget) {
	IWICImagingFactory* m_pWICFactory;
	ID2D1Bitmap* m_pBitmap;

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

			double aspectRatio = size.width / size.height;
			double origWidth = size.width;
			double origHeight = size.height;
			double newWidth = node.width;
			double newHeight = node.height;
			if (origWidth < newWidth && origHeight < newHeight) {
				newWidth = origWidth;
				newHeight = origHeight;
			}
			else {
				if (newWidth / newHeight > aspectRatio) {
					newWidth = newHeight * aspectRatio;
				}
				else {
					newHeight = newWidth / aspectRatio;
				}
			}

			// Draw a bitmap.
			pRenderTarget->DrawBitmap(
				m_pBitmap,
				D2D1::RectF(
					upperLeftCorner.x,
					upperLeftCorner.y,
					upperLeftCorner.x + newWidth * viewportScaleX,
					upperLeftCorner.y + newHeight * viewportScaleY)
			);

			SafeRelease(&m_pBitmap);
		}
	}
}

void drawDOMNode(DOMNode& node, ID2D1HwndRenderTarget* pRenderTarget, ID2D1SolidColorBrush* pBrush, int newHeight, int origHeight, int newWidth, int origWidth, int xy, vector<DOMNode*>& nodesInOrder, int level) {
	D2D1_SIZE_F renderTargetSize = pRenderTarget->GetSize();
	double innerWidth = renderTargetSize.width * viewportScaleX;
	double innerHeight = renderTargetSize.height * viewportScaleY;
	DOMNode* parentNode = node.parentNode;
	bool x_set = false, y_set = false, width_set = false, height_set = false;
	double totalWidth, totalHeight;
	bool shouldRecreateSlabDecompShape = false;

	if (node.get_tag_name() == "script") {
		return;
	}

	//node.expand = false;
	node.x = 0;
	node.y = 107 * newHeight / origHeight;
	node.width = 0;
	node.height = 0;
	node.width_set = false;
	node.height_set = false;
	node.x_set = false;
	node.y_set = false;

	if (node.get_tag_name() == "canvas") {
		if (!node.bitmap || node.bitmap == nullptr) {
			node.bitmap = new BYTE[400 * 400 * 4];
		}
	}

	if (node.get_child_count()) {
		DOMNode* childNode = node.firstChild;
		while (childNode && childNode != nullptr) {
			childNode->x = 0;
			childNode->y = 107 * newHeight / origHeight;
			childNode->width = 0;
			childNode->height = 0;
			childNode->width_set = false;
			childNode->height_set = false;
			childNode->x_set = false;
			childNode->y_set = false;
			childNode = childNode->nextSibling;
		}
	}

	if (node.ptrASTArray != nullptr) {
		if (node.ptrASTArray->find("id") != node.ptrASTArray->end()) {
			node.set_id((*node.ptrASTArray)["id"]->getString());
		}

		if (node.ptrASTArray->find("src") != node.ptrASTArray->end()) {
			node.attributes["src"] = (*node.ptrASTArray)["src"]->getString();
		}

		if (node.ptrASTArray->find("width") != node.ptrASTArray->end()) {
			node.attributes["width"] = (*node.ptrASTArray)["width"]->getString();
			if (node.attributes["width"].find("px") != string::npos) {
				node.style["width"] = std::to_string(pxToPc(node.attributes["width"], innerWidth) / innerWidth * 100) + "%";
			}
			else {
				node.style["width"] = std::to_string(stod(node.attributes["width"]) / node.parentNode->width * viewportScaleX * 100) + '%';
			}
		}

		if (node.ptrASTArray->find("height") != node.ptrASTArray->end()) {
			node.attributes["height"] = (*node.ptrASTArray)["height"]->getString();
			if (node.attributes["height"].find("px") != string::npos) {
				node.style["height"] = std::to_string(pxToPc(node.attributes["height"], innerHeight) / innerHeight * 100) + "%";
			}
			else {
				node.style["height"] = std::to_string(stod(node.attributes["height"]) / node.parentNode->height * viewportScaleY * 100) + '%';
			}
		}

		if (node.ptrASTArray->find("class") != node.ptrASTArray->end()) {
			node.attributes["class"] = (*node.ptrASTArray)["class"]->getString();
			splitString((*node.ptrASTArray)["class"]->getString(), ' ', node.classList);
		}

		map<string, ASTNode*>* ptrStyle = resolveRuntimeObject(*(*node.ptrASTArray)["style"]).ASTArray;
		string oldLeft = "";
		if (ptrStyle->find("left") != ptrStyle->end()) {
			oldLeft = (*ptrStyle)["left"]->getString();
		}
		string oldTop = "";
		if (ptrStyle->find("top") != ptrStyle->end()) {
			oldTop = (*ptrStyle)["top"]->getString();
		}

		if ((oldLeft != "" && oldLeft != node.style["left"]) || (oldTop != "" && oldTop != node.style["top"])) {
			shouldRecreateSlabDecompShape = true;
		}

		for (auto it = ptrStyle->begin(); it != ptrStyle->end(); it++) {
			node.style[it->first] = resolveString(it->second->getString());
		}

		if (node.ptrASTArray->find("textContent") != node.ptrASTArray->end()) {
			DOMNode* curChild = node.firstChild;
			while (curChild != nullptr) {
				DOMNode* nextSibling = curChild->nextSibling;
				if (nextSibling == nullptr || nextSibling == curChild) {
					delete curChild;
					break;
				}
				delete curChild;
				curChild = nextSibling;
			}
			curChild = nullptr;
			DOMNode* newTextNode = new DOMNode("TextNode", (*node.ptrASTArray)["textContent"]->getString(), 0, 0, 0);
			newTextNode->set_parent_node(node);
			node.set_child_count(0);
			node.appendChild(*newTextNode);
			newTextNode->previousSibling = nullptr;
			newTextNode->nextSibling = nullptr;
			node.firstChild = newTextNode;
			node.lastChild = newTextNode;
		}
	}

	if (node.get_tag_name() != "root" && node.get_tag_name() != "TextNode")
	{
		nodesInOrder.push_back(&node);
	}

	string tagName = node.get_tag_name();

	if (tagName == "head" || tagName == "link" || tagName == "meta" || tagName == "title") {
		return;
	}

	if (parentNode != &node) {
		if (node.get_idx() == 0)
		{
			node.x = parentNode->x + parentNode->marginX;
			node.y = parentNode->y;
		}
		else
		{
			node.x = node.previousSibling->getLeft();
			node.x += node.previousSibling->width;
			node.y = node.previousSibling->getTop();
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
			map<string, string>* elStyles;

			elStyles = &(myParser.styles[node.get_tag_name()]);
			for (it = elStyles->begin(); it != elStyles->end(); ++it) {
				node.style[it->first] = it->second;
			}

			elStyles = &(myParser.styles[node.get_id()]);
			for (it = elStyles->begin(); it != elStyles->end(); ++it) {
				node.style[it->first] = it->second;
			}

			if (node.attributes.find("class") != node.attributes.end()) {
				vector<string>* classList = &(node.classList);
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
				if (node.style["left"].find("px") != string::npos) {
					node.x = pxToPc(node.style["left"], innerWidth);
				}
				else {
					node.x = parentNode->x + parentNode->marginX + stod(substrReplace(node.style["left"] == "" ? "0" : node.style["left"], "%", "")) / 100 * parentNode->width;
				}
				if (node.style["position"] == "absolute") {
					if (node.style["top"].find("px") != string::npos) {
						node.y = pxToPc(node.style["top"], innerHeight);
					}
					else {
						node.y = parentNode->y + stod(substrReplace(node.style["top"] == "" ? "0" : node.style["top"], "%", "")) / 100 * parentNode->height;
					}
				}

				if (node.style["position"] == "fixed") {
					if (node.style["left"].find("px") != string::npos) {
						node.x = pxToPc(node.style["left"], innerWidth);
					}
					else {
						node.x = stod(substrReplace(node.style["left"] == "" ? "0" : node.style["left"], "%", "")) / 100 * renderTargetSize.width * viewportScaleX;
					}
					if (node.style["top"].find("px") != string::npos) {
						node.y = pxToPc(node.style["top"], innerHeight);
					}
					else {
						node.y = stod(substrReplace(node.style["top"] == "" ? "0" : node.style["top"], "%", "")) / 100 * renderTargetSize.height * viewportScaleY;
					}
				}

				//}
				//if (!width_set)
				//{
				if (node.style["width"].find("px") != string::npos) {
					node.width = pxToPc(node.style["width"], innerWidth);
				}
				else {
					node.width = stod(substrReplace(node.style["width"] == "" ? "100" : node.style["width"], "%", "")) / 100 * parentNode->width;
				}
				//}
				//if (!height_set)
				//{
				if (node.style["height"].find("px") != string::npos) {
					node.height = pxToPc(node.style["height"], innerHeight);
				}
				else {
					node.height = node.style["height"] == "" ? 12 : (stod(substrReplace(node.style["height"] == "" ? "0" : node.style["height"], "%", "")) / 100 * parentNode->height);
				}
				//}
				node.x_set = node.y_set = node.width_set = node.height_set = true;
			}
		}

		for (auto const& ent_i : parentNode->style)
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

	if (!node.x_set) { node.x = parentNode->getLeft(); }
	if (!node.y_set) { node.y = parentNode->getTop(); }
	if (!node.width_set) { node.width = parentNode->width; }
	if (!node.height_set) { node.height = parentNode->height; }

	if (node.style["display"].find("block") != string::npos && node.style["margin"] != "") {
		if (node.style["margin-left"] != "") {
			if (node.style["margin-left"].find("px") != string::npos) {
				node.marginX = pxToPc(node.style["margin-left"], innerWidth);
			}
			else {
				node.marginX = node.marginX || stod(substrReplace(node.style["margin-left"] == "" ? "0" : node.style["margin-left"], "%", "")) / 100 * node.parentNode->width;
			}
		}
		if (node.style["margin-top"] != "") {
			if (node.style["margin-top"].find("px") != string::npos) {
				node.marginY = pxToPc(node.style["margin-top"], innerHeight);
			}
			else {
				node.marginY = node.marginY || stod(substrReplace(node.style["margin-top"] == "" ? "0" : node.style["margin-top"], "%", "")) / 100 * node.parentNode->height;
			}
		}
	}

	if (true || node.get_tag_name() == "div") {
		string bg = node.style["background"];
		ID2D1LinearGradientBrush* m_pLinearGradientBrush;
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
				if (bg.find('url(') != string::npos) {
					string src = bg.substr(4);
					src = src.substr(0, src.size() - 1);
					std::wstring widestr = std::wstring(src.begin(), src.end());
					drawBitmap(widestr, node, pRenderTarget);
				}
				vector<string> rgbValues;
				D2D1_COLOR_F color;
				bool hasColor = false;
				if (node.style["background"].find("rgb") != string::npos) {
					splitString(node.style["background"].substr(0, node.style["background"].length() - 1).substr(4), ',', rgbValues);
					color = D2D1::ColorF(stof(rgbValues[0]) / 255, stof(rgbValues[1]) / 255, stof(rgbValues[2]) / 255);
					hasColor = true;
				}
				else if (node.style["background"].find("rgba") != string::npos) {
					splitString(node.style["background"].substr(0, node.style["background"].length() - 1).substr(5), ',', rgbValues);
					color = D2D1::ColorF(stof(rgbValues[0]) / 255, stof(rgbValues[1]) / 255, stof(rgbValues[2]) / 255, stof(rgbValues[3]));
					hasColor = true;
				}
				if (hasColor) {
					pRenderTarget->CreateSolidColorBrush(color, &pBrush);
					pRenderTarget->FillRectangle(&rect1, pBrush);
				}
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
				ID2D1GradientStopCollection* pGradientStops = NULL;
				D2D1_GRADIENT_STOP* gradStops = new D2D1_GRADIENT_STOP[len - 1]; // delete later
				for (int i = 1; i < len; i++)
				{
					vector<string> rgbValues;
					if (colors[i].find("rgba") == string::npos) {
						splitString(colors[i].substr(0, colors[i].length() - 1).substr(4), ',', rgbValues);
						gradStops[i - 1].color = D2D1::ColorF(stof(rgbValues[0]) / 255, stof(rgbValues[1]) / 255, stof(rgbValues[2]) / 255);
					}
					else {
						splitString(colors[i].substr(0, colors[i].length() - 1).substr(5), ',', rgbValues);
						gradStops[i - 1].color = D2D1::ColorF(stof(rgbValues[0]) / 255, stof(rgbValues[1]) / 255, stof(rgbValues[2]) / 255, stof(rgbValues[3]));
					}
					gradStops[i - 1].position = (float(i) - 1) / (float(len) - 2);
				}
				HRESULT hr = pRenderTarget->CreateGradientStopCollection(
					gradStops,
					len - 1,
					D2D1_GAMMA_2_2,
					D2D1_EXTEND_MODE_CLAMP,
					&pGradientStops
				);
				if (SUCCEEDED(hr) && node.width && node.height)
				{
					bool horiz = bg.find("toright") != string::npos;
					hr = pRenderTarget->CreateLinearGradientBrush(
						D2D1::LinearGradientBrushProperties(
							D2D1::Point2F(node.x, node.y + (horiz ? 0 : node.height)),
							D2D1::Point2F(node.x + (horiz ? node.width : 0), node.y)),
						pGradientStops,
						&m_pLinearGradientBrush
					);
					//m_pLinearGradientBrush->Release();
					if (SUCCEEDED(hr)) {
						pRenderTarget->FillRectangle(&rect1, m_pLinearGradientBrush);
					}

					SafeRelease(m_pLinearGradientBrush);
				}
				SafeRelease(pGradientStops);
				delete[] gradStops;
			}
		}
	}
	if (node.get_tag_name() == "canvas") {
		ID2D1Bitmap* pBitmap = NULL;
		HRESULT hr = pRenderTarget->CreateBitmap(D2D1::SizeU(400, 400), node.bitmap, 400 * 4, D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

		// Draw a bitmap.
		pRenderTarget->DrawBitmap(
			pBitmap,
			D2D1::RectF(
				node.x,
				node.y,
				node.x + node.width,
				node.y + node.height
			),
			1.0
		);

		SafeRelease(&pBitmap);

	}

	if (node.get_tag_name() == "img") {

		// Create a bitmap from a file.
		std::wstring widestr = std::wstring(node.attributes["src"].begin(), node.attributes["src"].end());
		drawBitmap(widestr, node, pRenderTarget);
	}

	node._x = node.x;
	node._y = node.y;
	node._width = node.width;
	node._height = node.height;

	if (shouldRecreateSlabDecompShape) {
		selRegion = nullptr;

		Shape* newShape = new Shape;

		if (node.SlabDecompShape != nullptr) {
			newShape->eventHandlers = node.SlabDecompShape->eventHandlers;
			mySlabContainer.deleteShape(*node.SlabDecompShape);
			delete node.SlabDecompShape;
		}

		int newShapeId = mySlabContainer.NextAvailableShapeId++;
		newShape->id = newShapeId;
		newShape->x1 = node.x;
		newShape->x2 = node.x + node.width;
		newShape->y1 = node.y;
		newShape->y2 = node.y + node.height;
		newShape->node = &node;

		mySlabContainer.ShapeMembers[newShapeId] = newShape;
		mySlabContainer.addShape(*newShape);
		node.SlabDecompShape = newShape;
	}

	if (node.get_child_count())
	{
		if (node.parentNode->get_tag_name() != "root")
		{
			if (node.style["display"] == "block" || node.style["display"] == "inline-block") {
				if (node.style["width"].find("px") != string::npos) {
					node.width = pxToPc(node.style["width"], innerWidth);
				}
				else {
					node.width = stod(substrReplace(node.style["width"] == "" ? "100" : node.style["width"], "%", "")) / 100 * node.parentNode->width;
				}
				node.width_set = true;
			}
			if (node.style["display"] == "block")
			{
				if (node.style["height"].find("px") != string::npos) {
					node.height = pxToPc(node.style["height"], innerHeight);
				}
				else {
					node.height = stod(substrReplace(node.style["height"] == "" ? "100" : node.style["height"], "%", "")) / 100 * node.parentNode->height;
				}
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
			DOMNode* child = node.firstChild;
			int xy = 0;
			while (child)
			{
				if (child->style["width"] != "") {
					if (child->style["width"].find("px") != string::npos) {
						child->width = pxToPc(child->style["width"], innerWidth);
					}
					else {
						child->width = stod(substrReplace(child->style["width"] == "" ? "0" : child->style["width"], "%", "")) / 100 * node.width;
					}
					child->width_set = true;
				}
				if (child->style["height"] != "") {
					if (child->style["height"].find("px") != string::npos) {
						child->height = pxToPc(child->style["height"], innerHeight);
					}
					else {
						child->height = stod(substrReplace(child->style["height"] == "" ? "0" : child->style["height"], "%", "")) / 100 * node.height;
					}
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
		vector<double> widths = { 0.0 };
		if (node.get_child_count())
		{
			DOMNode* child = node.firstChild;
			double curWidth = 0.0;
			int idx = 0;
			while (child)
			{
				double childWidth = 0.0;
				childWidth = child->width;
				if ((curWidth + childWidth) >= innerWidth) {
					widths.push_back(curWidth);
					curWidth = 0.0;
				}
				curWidth += childWidth;
				if (totalHeight == 0.0 || child->style["display"] == "block" || idx == 0 || child->get_tag_name() == "TextNode")
				{
					if (child->nextSibling && child->nextSibling != nullptr) {
						totalHeight += child->nextSibling->getTop() - child->getTop();
					}
					else {
						totalHeight += child->height;
					}
				}
				idx++;
				child = child->nextSibling;
			}
			widths.push_back(curWidth);
		}
		if (node.style["display"] == "inline")
		{
			node.width = min(innerWidth, *std::max_element(widths.begin(), widths.end()));
			node.width_set = true;
		}
		node.height = min(innerHeight, totalHeight);
		node.height_set = true;
		
		if (node.get_tag_name() == "TextNode") {
			// Create a text layout using the text format.
			string s3 = node.get_text_content();
			std::wstring widestr = std::wstring(s3.begin(), s3.end());

			IDWriteTextLayout* pTextLayout_;

			HRESULT hr = m_pDWriteFactory->CreateTextFormat(
				L"Verdana",
				NULL,
				node.style["font-weight"] == "bold" ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
				node.style["font-style"] == "italic" ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				10 * newHeight / origHeight,
				L"", //locale
				&m_pTextFormat
			);

			hr = m_pDWriteFactory->CreateTextLayout(
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

			double x1 = node.getLeft();
			double y1 = node.getTop();
			double x2 = node.getLeft() + node.width;
			double y2 = node.getTop() + node.height;

			if ((node.getLeft() + node.width) >= innerWidth) {
				DOMNode* leftNode = node.parentNode;
				DOMNode* origNode = &node;
				if (node.previousSibling && node.previousSibling != nullptr) {
					node.y += node.previousSibling->height;
				}
				else {
					node.y += 12;// metrics.height;
				}
				node.x = 0.0;
				if (!(node.previousSibling && node.previousSibling != nullptr) && !(node.nextSibling && node.nextSibling != nullptr)) {
					origNode = node.parentNode;
				}
				origNode->x = origNode->parentNode->x;
				while (leftNode->style["display"] != "block" && leftNode->parentNode != leftNode) {
					leftNode = leftNode->parentNode;
				}
				double xdiff = leftNode->x + leftNode->marginX;
				node.x += xdiff;
				int offset = 1;
				while (origNode && origNode != nullptr) {
					origNode->x += xdiff * offset;
					origNode->width += xdiff;
					if (origNode->previousSibling && origNode->previousSibling != nullptr) {
						origNode->y += origNode->previousSibling->height;
					}
					else {
						origNode->y += origNode->height;
					}
					origNode = origNode->nextSibling;
					offset += 1;
				}
				node.width = min(innerWidth - node.getLeft(), metrics.width);

				x1 = node.getLeft();
				y1 = node.getTop();
				x2 = node.getLeft() + node.width;
				y2 = node.getTop() + node.height;
			}

			if (node.get_tag_name() == "TextNode") {
				//D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0, 1.0f);
				vector<string> rgbValues;

				//MessageBox(NULL, stringToLPCWSTR(node.style["color"]), L"node.style.color", MB_OK);

				if (node.style.find("color") == node.style.end() || node.style["color"] == "")
				{
					node.style["color"] = "rgb(0,0,0)";
				}

				splitString(node.style["color"].substr(0, node.style["color"].length() - 1).substr(4), ',', rgbValues);
				D2D1_COLOR_F color = D2D1::ColorF(stof(rgbValues[0]) / 255, stof(rgbValues[1]) / 255, stof(rgbValues[2]) / 255);
				pRenderTarget->CreateSolidColorBrush(color, &pBrush);

				string txt = node.get_text_content();
				std::wstring widestr = std::wstring(txt.begin(), txt.end());
				pRenderTarget->DrawText(
					widestr.c_str(),
					txt.length(),
					m_pTextFormat,
					D2D1::RectF(x1, y1, x2, y2),
					pBrush
				);
			}

			SafeRelease(pTextLayout_);
		}
	}

	/*node.x = min(node.x, renderTargetSize.width * viewportScaleX);
	node.y = min(node.y, renderTargetSize.height * viewportScaleY);
	node.width = min(node.width, renderTargetSize.width * viewportScaleX);
	node.height = min(node.height, renderTargetSize.height * viewportScaleY);*/
}