#pragma once
#include <string>
#include <string.h>
#include <vector>
#include "DOMNode.h"
#include "substrReplace.h"
const vector<string> styleSkipList = { "box-shadow", "margin", "position", "display", "left", "top", "width", "height" };

void drawDOMNode(DOMNode &node, ID2D1HwndRenderTarget *pRenderTarget, ID2D1SolidColorBrush *pBrush) {
	DOMNode *parentNode = node.parentNode;
	bool x_set = false, y_set = false, width_set = false, height_set = false;
	double totalWidth, totalHeight;

	string tagName = node.get_tag_name();

	D2D1_SIZE_F renderTargetSize = pRenderTarget->GetSize();

	//ctx.shadowBlur = ctx.shadowOffsetX = ctx.shadowOffsetY = 0;
	//ctx.shadowColor = "transparent";

	if (tagName == "head") {
		return;
	}

	if (parentNode->get_tag_name() != "root") {
		/*if (node.id=="sidebar"){
		console.log(node.parentNode->x);
		}*/

		/*if (node.get_id() == "main")
		{
			myParser.output = to_string(node.get_idx()) + " " + node.style["display"] + " " + node.previousSibling->style["display"];
		}*/
		if (node.get_idx() == 0)
		{
			node.x = parentNode->x;
			node.y = parentNode->y;
		}
		else
		{
			node.x = node.previousSibling->x;
			if (node.previousSibling->style["display"].find("inline") != -1)
			{
				node.x += node.previousSibling->width;
			}
			node.y = node.previousSibling->y;
			/*string mS = node.previousSibling->style["display"] + ',' + node.get_id() + ',' + to_string(node.previousSibling->height);
			std::wstring widestr = std::wstring(mS.begin(), mS.end());
			static const wchar_t* sc_text = widestr.c_str();
			MessageBox(NULL, sc_text, L"", MB_OK);*/
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

		if ((node.attributes).size()) {
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

			/*for (auto const &ent_i : node.style)
			{
				myParser.output += ent_i.first + ":" + ent_i.second + ";";
			}*/

			if (node.style["display"] == "block") {
			/*string str3 = "Width:" + node.style["width"];
			std::wstring widestr = std::wstring(str3.begin(), str3.end());
			static const wchar_t* sc_text = widestr.c_str();
			MessageBox(NULL, sc_text, L"", MB_OK);*/
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
		node.width = renderTargetSize.width;
		node.height = renderTargetSize.height;
		node.x_set = node.y_set = node.width_set = node.height_set = true;
	}

	if (!node.x_set) { node.x = parentNode->x; }
	if (!node.y_set) { node.y = parentNode->y; }
	if (!node.width_set) { node.width = parentNode->width; }
	if (!node.height_set) { node.height = parentNode->height; }

	if (node.style["display"].find("block") != string::npos && node.style["margin"] != "") {
		node.marginX = node.marginX || stod(substrReplace(node.style["margin"] == "" ? "0" : node.style["margin"], "%", "")) / 100 * node.parentNode->width;
		node.marginY = node.marginY || stod(substrReplace(node.style["margin"] == "" ? "0" : node.style["margin"], "%", "")) / 100 * node.parentNode->height;
	}/* else {
	 node.marginX = node.marginY = 0;
	 }*/

	 /*if (node.id == "header") {
	 if (!(arguments[1]&&arguments[1]=="main")){return;}
	 }*/
	 /*if (node.id == "main") {
	 var header=getElementById("header");
	 ctx.clearRect(header.x,header.y,header.width,header.height);
	 header.marginY+=node.marginY;
	 node.marginY*=3;
	 }*/
	 //drawDOMNode(header,"main");
	//node.x += node.marginX || 0;
	//node.y += node.marginY || 0;

	if (node.get_tag_name() == "div") {
		/*var i = 0, bg = node.style && node.style["background"], len = bg && bg.length, gradColors, lg = 'linear-gradient(', colors, el, grad;
		if (bg) {
		while (i < len && bg[i] == lg[i]) {
		i++;
		}
		if (i == lg.length) {
		colors = [];
		el = '';
		while (i < len) {
		if (bg[i] == ',') {
		colors.push(el);
		el = '';
		} else {
		el += bg[i];
		}
		i++;
		}
		colors.push(el.slice(0, -1));

		if (colors[0].trim() == 'to right') {
		grad = ctx.createLinearGradient(node.x, node.y, node.x + node.width, node.y);
		} else if (colors[0].trim() == 'to bottom') {
		grad = ctx.createLinearGradient(node.x, node.y, node.x, node.y + node.height);
		}
		colors.shift();
		colors.forEach(function (color, idx) {
		grad.addColorStop(idx / (colors.length - 1), color);
		});
		}
		}
		var boxShadow = node.style['box-shadow'];
		if (boxShadow && boxShadow != "none") {
		boxShadow = boxShadow.split(' ');
		ctx.shadowOffsetX = Number(boxShadow[0].replace("px", ''));
		ctx.shadowOffsetY = Number(boxShadow[1].replace("px", ''));
		ctx.shadowBlur = Number(boxShadow[2].replace("px", ''));
		ctx.shadowColor = boxShadow[3];
		}
		ctx.fillStyle = grad || node.style["background"] || "transparent";
		ctx.fillRect(node.x, node.y, node.width, node.height);*/
		
		/*string s = to_string(node.x);
		std::wstring widestr = std::wstring(s.begin(), s.end());
		static const wchar_t* sc_text = widestr.c_str();*/

		/*pRenderTarget->DrawText(
			sc_text,
			s.length(),
			m_pTextFormat,
			D2D1::RectF(0, 0, 100,100),
			pBrush
		);*/

		D2D1_RECT_F rect1 = D2D1::RectF(
			node.x,
			node.y,
			node.x + node.width,
			node.y + node.height
		);
		vector<string> rgbValues;

		if (node.style["background"] == "")
		{
			node.style["background"] = "rgb(255,255,255)";
		}

		splitString(node.style["background"].substr(0, node.style["background"].length() - 1).substr(4), ',', rgbValues);
		//myParser.output += rgbValues[0] + ':' + rgbValues[1] + ':' + rgbValues[2] + ';';
		D2D1_COLOR_F color = D2D1::ColorF( stof(rgbValues[0]) / 255, stof(rgbValues[1]) / 255, stof(rgbValues[2]) / 255);
		//pRenderTarget->CreateSolidColorBrush(color, &pBrush);
		//pRenderTarget->DrawRectangle(&rect1, pBrush,5.0f,NULL);
		//D2D1_COLOR_F color2 = D2D1::ColorF(255, 0, 0);
		pRenderTarget->CreateSolidColorBrush(color, &pBrush);
		pRenderTarget->FillRectangle(&rect1, pBrush);
	}

	if (node.get_tag_name() == "TextNode") {
		/*ctx.font = (node.style['font-weight'] == "bold" ? 'bold ' : '') + (node.style['font-style'] == "italic" ? 'italic ' : '') + ' 10pt Arial';
		ctx.fillStyle = node.style["color"] || '#000000';
		ctx.fillText(node.textContent, node.x, node.y + 12);*/
		node.width = 100;// ctx.measureText(node.textContent).width;
		node.height = 12;
	}/* else if (node.tagName == "img") {
	 img = new Image();
	 img.src = node.attributes.src;
	 img.x1 = node.x;
	 img.y1 = node.y;
	 node.width = Math.min(Number(node.attributes.width), node.parentNode->width);
	 node.height = Number(node.attributes.height);
	 img.x2 = node.x + node.width;
	 img.y2 = node.y + node.height;
	 img.onload = function () {
	 ctx.beginPath();
	 ctx.moveTo(this.x1, this.y1);
	 ctx.lineTo(this.x2, this.y1);
	 ctx.lineTo(this.x2, this.y2);
	 ctx.lineTo(this.x1, this.y2);
	 ctx.closePath();
	 ctx.clip();
	 ctx.drawImage(this, this.x1, this.y1);
	 }
	 } */
	else if (node.get_child_count()) {
		if (node.parentNode->get_tag_name() != "root") {
			if (node.style["display"] == "block" || node.style["display"] == "inline-block") {
				node.width = stod(substrReplace(node.style["width"] == "" ? "100" : node.style["width"], "%", "")) / 100 * node.parentNode->width;
				node.width_set = true;
			}
			if (node.style["display"] == "block") {
				node.height = stod(substrReplace(node.style["height"] == "" ? "100" : node.style["height"], "%", "")) / 100 * node.parentNode->height;
				node.height_set = true;
			}
		}
		else {
			node.width = renderTargetSize.width;
			node.height = renderTargetSize.height;
			node.width_set = node.height_set = true;
		}
		
		if (node.get_child_count())
		{
			DOMNode *child = node.firstChild;
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
				drawDOMNode(*child, pRenderTarget, pBrush);
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
				if (child->style["display"] == "block" || idx == 0 || child->get_tag_name() == "TextNode") {
					totalHeight += child->height;
				}
				idx++;
				child = child->nextSibling;
			}
		}
		if (node.style["display"] == "inline") {
			node.width = totalWidth;
			node.width_set = true;
		}
		node.height = totalHeight;
		node.height_set = true;
		//console.log(node.tagName,node.width);
		//console.log('\n');
	}
}