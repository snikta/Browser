#pragma once
#include <string>
#include "parser.h"
#include "splitString.h"
/* "parseHTML.h" */

void parseHTML(DOMNode &node, DOMNode &parentNode, string &src, int start, int end, string tagName);