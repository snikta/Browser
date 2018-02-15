#pragma once
#include <string>
#include <string.h>
#include <vector>
#include "DOMNode.h"
#include "substrReplace.h"
//const vector<string> styleSkipList = { "box-shadow", "margin", "position", "display", "left", "top", "width", "height" };
vector<string> styleSkipList;

void drawDOMNode(DOMNode &node) {
	DOMNode *parentNode = node.parentNode;
	bool x_set = false, y_set = false, width_set = false, height_set = false;
	double totalWidth, totalHeight;

	string tagName = node.get_tag_name();

	//ctx.shadowBlur = ctx.shadowOffsetX = ctx.shadowOffsetY = 0;
	//ctx.shadowColor = "transparent";

	if (tagName == "head") {
		return;
	}

	if (parentNode->get_tag_name() != "root") {
		/*if (node.id=="sidebar"){
		console.log(node.parentNode->x);
		}*/
		node.x = ((&node == node.parentNode->firstChild ? node.parentNode : node.previousSibling)->x || 0);// + ((node.previousSibling && node.previousSibling.style && node.previousSibling.style["display"].indexOf("inline") != -1 && node.previousSibling.width) || 0);
		node.y = ((&node == node.parentNode->firstChild ? node.parentNode : node.previousSibling)->y || 0);// + ((node.previousSibling && ((node.style && node.style["display"] == "block") || (node.previousSibling.style && node.previousSibling.style["display"] == "block")) && node.previousSibling.height) || 0);

		x_set = y_set = true;

		if (tagName == "body") {
			node.width = node.parentNode->width;
			node.height = node.parentNode->height;
			width_set = height_set = true;
		}

		//node.style = {display: "inline"};

		if ((node.attributes).size()) {
			map<string, string>::iterator it;
			map<string, string> *elStyles;

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

			elStyles = &(myParser.styles[node.get_tag_name()]);
			for (it = elStyles->begin(); it != elStyles->end(); ++it) {
				node.style[it->first] = it->second;
			}

			elStyles = &(myParser.styles["#" + node.get_id()]);
			for (it = elStyles->begin(); it != elStyles->end(); ++it) {
				node.style[it->first] = it->second;
			}

			//if (node.style["display"] == "block") {
			node.x = parentNode->x + stod(substrReplace(node.style["left"] == "" ? "0" : node.style["left"], "%", "")) / 100 * parentNode->width;
			if (node.style["position"] == "absolute") {
				node.y = parentNode->y + stod(substrReplace(node.style["top"] == "" ? "0" : node.style["top"], "%", "")) / 100 * parentNode->height;
			}
			node.width = stod(substrReplace(node.style["width"] == "" ? "0" : node.style["width"], "%", "")) / 100 * parentNode->width;
			node.height = (node.style["height"] == "" ? 12 : stod(substrReplace(node.style["height"] == "" ? "0" : node.style["height"], "%", "")) / 100 * parentNode->height);
			x_set = y_set = width_set = height_set = true;
			//}
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

		for (auto const &ent_i : node.style)
		{
			node.style[ent_i.first] = ent_i.second;
		}
	}
	else {
		node.x = 0;
		node.y = 0;
		node.width = 1366.0;
		node.height = 768.0;
	}

	if (!x_set) { node.x = parentNode->x; }
	if (!y_set) { node.y = parentNode->y; }
	if (!width_set) { node.width = parentNode->width; }
	if (!height_set) { node.height = parentNode->height; }

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
	 drawDOMNode(header,"main");
	 }*/
	node.x += node.marginX || 0;
	node.y += node.marginY || 0;

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
	}

	if (node.get_tag_name() == "TextNode") {
		/*ctx.font = (node.style['font-weight'] == "bold" ? 'bold ' : '') + (node.style['font-style'] == "italic" ? 'italic ' : '') + ' 10pt Arial';
		ctx.fillStyle = node.style["color"] || '#000000';
		ctx.fillText(node.textContent, node.x, node.y + 12);*/
		//node.width = ctx.measureText(node.textContent).width;
		//node.height = 12;
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
				node.width = stod(substrReplace(node.style["width"] = "" ? "0" : node.style["width"], "%", "")) / 100 * node.parentNode->width;
			}
			if (node.style["display"] == "block") {
				node.height = stod(substrReplace(node.style["height"] == "" ? "0" : node.style["height"], "%", "")) / 100 * node.parentNode->height;
			}
		}
		else {
			node.width = 1366.0;
			node.height = 768.0;
		}

		if (node.get_child_count())
		{
			DOMNode *child = node.firstChild;
			while (child)
			{
				if (width_set && child->style["width"] != "") {
					//console.log(child.tagName);
					//console.log(child.width);
					child->width = stod(substrReplace(node.style["width"] == "" ? "0" : node.style["width"], "%", "")) / 100 * node.width;
					//console.log(child.width);
					//console.log('\n');
				}
				if (height_set && child->style["height"] != "") {
					child->height = stod(substrReplace(node.style["height"] == "" ? "0" : node.style["height"], "%", "")) / 100 * node.height;
				}
			}
			drawDOMNode(*child);
			child = child->nextSibling;
		}
	}
	if (!(width_set && height_set)) {
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
				if (child->style["display"] == "block" || idx == 0 || child->type == "TextNode") {
					totalHeight += child->height;
				}
				idx++;
				child = child->nextSibling;
			}
		}
		if (node.style["display"] == "inline") {
			node.width = totalWidth;
		}
		node.height = totalHeight;
		//console.log(node.tagName,node.width);
		//console.log('\n');
	}
}