#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include "DOMNode.h"
#include "JSExec.h"
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <wincodec.h> 
#include <commdlg.h> 
#include <Windowsx.h>
#include <fstream>
#include <d2d1.h>
#include "SafeRelease.h"
#pragma comment(lib, "d2d1")

#include "basewin.h"
#include "WICViewerD2D.h"

void LOut(string str) {
	string s1 = str + "\n";
	std::wstring widestr = std::wstring(s1.begin(), s1.end());
	OutputDebugStringW(widestr.c_str());
}

using std::cout;
using std::endl;
using std::vector;
using std::map;
using std::string;

string trim(string str) {
	int i = 0;
	int len = str.size();
	while (i < len && (str[i] == ' ' || str[i] == '\r' || str[i] == '\n' || str[i] == '\t')) {
		i++;
	}
	str = str.substr(i);
	len = str.size();
	i = len - 1;
	while (i >= 0 && (str[i] == ' ' || str[i] == '\r' || str[i] == '\n' || str[i] == '\t')) {
		i--;
	}
	str = str.substr(0, i + 1);
	return str;
}

bool isNaN(string val) {
	if (val == "") {
		return false;
	}
	if (val == ".") {
		return true;
	}
	for (int i = 0, len = val.size(); i < len; i++) {
		if (!((val[i] >= '0' && val[i] <= '9') || val[i] == '.' || val[i] == '+' || val[i] == '-' || val[i] == 'e') || (i > 0 && (val[i] == '+' || val[i] == '-')) || val[i] == '*' || val[i] == '/') {
			return true;
		}
	}
	return false;
}

const vector<string> operators = { "==","=","+=","-=","*=","/=","[]",".","+","-","*","/","%","<","<=",">",">=","&&","||",","," in ","new" };
bool isOperator(string op) {
	return find(operators.begin(), operators.end(), op) != operators.end();
}

ASTNode::ASTNode(long double ASTNodeNumber) : ASTNodeNumber(ASTNodeNumber), ASTNodeType(ASTNumberNode) {};
ASTNode::ASTNode(string ASTNodeString) {
	if (ASTNodeString == "" || isNaN(ASTNodeString) || ASTNodeString == "-" || ASTNodeString == "+" || ASTNodeString == ".") {
		this->ASTNodeString = ASTNodeString;
		this->ASTNodeType = ASTStringNode;
	}
	else {
		this->ASTNodeNumber = stold(ASTNodeString);
		this->ASTNodeType = ASTNumberNode;
	}
};
ASTNode::ASTNode(bool ASTNodeBool) : ASTNodeBool(ASTNodeBool), ASTNodeType(ASTBoolNode) {};
ASTNode::ASTNode(ParseNode *ASTNodeFunc) : ASTNodeFunc(ASTNodeFunc), ASTNodeType(ASTFuncNode) {};
ASTNode::ASTNode() {};
int ASTNode::getType() {
	return ASTNodeType;
}
string ASTNode::getString() {
	switch (ASTNodeType) {
	case ASTStringNode:
		return this->ASTNodeString;
		break;
	case ASTNumberNode:
	{
		if (this->ASTNodeNumber == int(this->ASTNodeNumber)) {
			return std::to_string(int(this->ASTNodeNumber));
		}
		string retval = std::to_string(this->ASTNodeNumber);
		int len = retval.size(), i = len - 1;
		while (i >= 0 && retval[i] == '0') {
			i--;
		}
		return retval.substr(0, i + 1);
	}
	break;
	case ASTBoolNode:
		return this->ASTNodeBool ? "true" : "false";
		break;
	default:
		return "";
	}
};
long double ASTNode::getNumber() {
	switch (ASTNodeType) {
	case ASTStringNode:
		if (this->ASTNodeString[0] == '"' && this->ASTNodeString[this->ASTNodeString.size() - 1] == '"') {
			return stold(this->ASTNodeString.substr(1, this->ASTNodeString.size() - 2));
		}
		if (this->ASTNodeString == "") {
			return 0.0L;
		}
		return stold(this->ASTNodeString);
		break;
	case ASTNumberNode:
		return this->ASTNodeNumber;
		break;
	case ASTBoolNode:
		return this->ASTNodeBool ? 1.0 : 0.0;
		break;
	}
	return ASTNodeNumber;
}
bool ASTNode::getBool() {
	if (ASTNodeType == ASTNumberNode) {
		return this->ASTNodeNumber != 0.0;
	}
	return ASTNodeBool;
}
bool ASTNode::operator==(const ASTNode& rhs) {
	if (ASTNodeType != rhs.ASTNodeType) {
		return false;
	}
	switch (ASTNodeType) {
	case ASTStringNode:
		return ASTNodeString == rhs.ASTNodeString;
		break;
	case ASTNumberNode:
		return ASTNodeNumber == rhs.ASTNodeNumber;
		break;
	case ASTBoolNode:
		return ASTNodeBool == rhs.ASTNodeBool;
		break;
	}
};

//typedef vector<ASTNode> AbstractSyntaxTree;
//typedef map<string, ASTNode> Scope;

void printAST(AbstractSyntaxTree& ast) {
	for (int i = 0, len = ast.size(); i < len; i++) {
		switch (ast[i].getType()) {
		case ASTFuncNode:
			cout << "ASTFuncNode()" << endl;
			break;
		case ASTStringNode:
			cout << "ASTStringNode: \"" << ast[i].getString() << "\"" << endl;
			break;
		case ASTNumberNode:
			cout << "ASTNumberNode: " << ast[i].getNumber() << endl;
			break;
		case ASTBoolNode:
			cout << "ASTBoolNode: " << (ast[i].getBool() ? "true" : "false") << endl;
			break;
		}
	}
}

ScopeContainer::ScopeContainer() {};

ASTNode parseParens(string expr, Scope& args);

Scope globalVariables;

string resolveString(string str) {
	if (str[0] == '"' && str[str.size() - 1] == '"') {
		return str.substr(1, str.size() - 2);
	}
	return str;
}

vector<ASTNode> runtimeObjects;
ASTNode resolveRuntimeObject(ASTNode astRef) {
	if (astRef.getString() != "" && astRef.ASTNodeString.find("<RuntimeObject#") != string::npos) {
		string runtimeObjectId = astRef.ASTNodeString.substr(astRef.ASTNodeString.find("<RuntimeObject#") + string("<RuntimeObject#").size());
		runtimeObjectId = runtimeObjectId.substr(0, runtimeObjectId.size() - 1);
		return runtimeObjects[stoi(runtimeObjectId)];
	}
	return astRef;
}

ASTNode resolve(ASTNode& op, Scope& scope);
ASTNode resolveFunc(ScopeContainer* scopeBox, bool newvalExists, ASTNode newval)
{
	ASTNode op1 = scopeBox->op1__;
	ASTNode op2 = scopeBox->op2__;

	string op1Str = resolveString(op1.getString());
	string op2Str = resolveString(op2.getString());

	Scope* ptrScope = scopeBox->scope__;

	/*if (op1Str == "this") {
		while (ptrScope != nullptr && ptrScope->__this__ == nullptr) {
			ptrScope = ptrScope->__parent;
		}
		if (newvalExists) {
			return *((*ptrScope->__this__)[op2Str] = new ASTNode(newval));
		}
		else {
			return *((*ptrScope->__this__)[op2Str]);
		}
	}*/

	op1 = resolveRuntimeObject(op1);
	if (op1.ASTArray->size()) {
		if (!newvalExists) {
			ASTNode* ptrRetval = (*op1.ASTArray)[op2Str];
			if (ptrRetval != nullptr) {
				return *ptrRetval;
			}
		}
	}

	do {
		if (ptrScope->ScopeArray.find(op1Str) != ptrScope->ScopeArray.end()) {
			if (ptrScope->__parent == nullptr) {
			}
			if (!newvalExists) {
				ASTNode astFound = resolveRuntimeObject(ptrScope->ScopeArray[op1Str]);
				do {
					if ((*astFound.ASTArray).find(op2Str) != (*astFound.ASTArray).end()) {
						return resolveRuntimeObject(*((*astFound.ASTArray)[op2Str]));
					}
					if ((*astFound.ASTArray).find("prototype") != (*astFound.ASTArray).end()) {
						astFound = *((*astFound.ASTArray)["prototype"]);
					}
					else {
						break;
					}
				} while (true);
			}
			return *(((*resolveRuntimeObject(ptrScope->ScopeArray[op1Str]).ASTArray)[op2Str]) = new ASTNode(newval));
		}
	} while (ptrScope->__parent != nullptr && (ptrScope = ptrScope->__parent));

	return *((*(resolveRuntimeObject(op1).ASTArray))[op2Str] = new ASTNode(newval));
}

ASTNode* parseASTNode(ASTNode* node) {
	if (node->childNodes.size() == 1) {
		return parseASTNode(node->childNodes[0]);
	}
	return node;
}

ASTNode resolve(ASTNode& op, Scope& scope) {
	if (op.scopeBox != nullptr) {
		return resolveFunc(op.scopeBox, false, ASTNode());
	}
	if (op.getString()[0] == '"' && op.getString()[op.getString().size() - 1] == '"') {
		return ASTNode(string(resolveString(op.getString())));
	}
	if (op.ASTArray->size()) {
		return op;
	}

	Scope* ptrScope = &scope;
	do {
		if (ptrScope->ScopeArray.find(op.getString()) != ptrScope->ScopeArray.end()) {
			if (ptrScope->__parent == nullptr) {
			}
			return resolveRuntimeObject(ptrScope->ScopeArray[op.getString()]);
		}
	} while (ptrScope->__parent != nullptr && (ptrScope = ptrScope->__parent));

	if (globalVariables.ScopeArray.find(op.getString()) != globalVariables.ScopeArray.end()) {
		return resolveRuntimeObject(globalVariables.ScopeArray[op.getString()]);
	}

	return resolveRuntimeObject(op);
	/*if (typeof op == 'function') {
		return op()
	}
	if (typeof op == 'number') {
		return op
	}
	if (typeof op == 'object' && op.string) {
		return op.string.toString()
	}
	if (scope) {
		do {
			if (op in scope) {
				return typeof scope[op] == 'object' && ('string' in scope[op]) ? scope[op].string : scope[op]
			}
		} while (scope = scope.__parent)
	}
	if (op in JSExec.variables) {
		op = JSExec.variables[op]
	}
	return typeof op == 'object' && ('string' in op) ? op.string : op*/
}

bool isNumeric(char val) {
	if (!((val >= '0' && val <= '9') || val == '.')) {
		return false;
	}
	return true;
}
bool isIdentifier(char val) {
	return (val >= 'A' && val <= 'Z') || (val >= 'a' && val <= 'z') || val == '_';
}

AbstractSyntaxTree parseExpr(string expr, Scope& scope) {
	/*if (typeof expr == "function") {
		return expr()
	}
	if (typeof expr == "object" && expr.string) {
		return expr
	}
	expr = expr.toString()*/
	AbstractSyntaxTree ast;
	string cur = "";
	bool num = false;
	for (int i = 0, len = expr.size(); i < len; i++) {
		if (expr.substr(i, 4) == "new ") {
			ast.push_back(ASTNode(string("new")));
			i += 3;
		}
		else if (expr.substr(i, string("<RuntimeObject#").size()) == "<RuntimeObject#") {
			int pos = i + string("<RuntimeObject#").size();
			int endPos = expr.find_first_of('>', pos);;
			ast.push_back(ASTNode(string(expr.substr(i, endPos + 1 - i))));
			i = endPos + 1;
		}
		else if (i > 0 && expr[i] == '.' && ast.size() >= 2 && ast[ast.size() - 2].getString() == "[]") {
			ast.push_back(ASTNode(string(".")));
			cur = "";
			continue;
		}
		else if (i > 0 && expr[i] == '.' && isIdentifier(cur[0])) {
			ast.push_back(ASTNode(cur));
			ast.push_back(ASTNode(string(".")));
			cur = "";
			continue;
		}
		else if (isNumeric(expr[i]) && !(i > 0 && isIdentifier(expr[i - 1]))) {
			if (!num) {
				num = true;
				if (cur != "") {
					if (isNaN(cur)) {
						ast.push_back(ASTNode(string(cur)));
					}
					else {
						ast.push_back(ASTNode(stold(cur)));
					}
				}
				cur = "";
			}
		}
		else if (num && expr[i] == 'e' && (expr[i + 1] == '+' || expr[i + 1] == '-')) {
			cur = cur + "e" + expr[i + 1];
			i += 2;
			while (i < len && isNumeric(expr[i])) {
				cur += expr[i];
				i++;
			}
			i--;
			continue;
		}
		else {
			bool numWasTrue = num;
			if (num) {
				ast.push_back(ASTNode(string(cur)));
			}
			num = false;
			if (i >= 3 && expr.substr(i - 3, 3) == " in" && expr[i] == ' ') {
				cur = " in";
			}
			else if (expr[i] == ' ') {
				continue;
			}
			if (expr[i] == '"' || expr[i] == '\'') {
				char quotChar = expr[i];
				string val = "";
				i++;
				string slashes = "";
				while (i < len && expr[i] != quotChar) {
					if (expr[i] == '\\') {
						slashes += expr[i];
					}
					val += expr[i];
					if (i < len - 1 && (expr[i + 1] == '"' || expr[i + 1] == '\'' || expr[i + 1] == 'n') && (slashes.size() % 2 != 0)) {
						slashes = "";
						val += expr[i + 1];
						i++;
					}
					i++;
				}
				ast.push_back(ASTNode(string(quotChar + val + quotChar)));
				cur = "";
				continue;
			}
			else if (expr[i] == '[') {
				string val = "";
				i++;
				while (i < len && expr[i] != ']') {
					val += expr[i];
					i++;
				}
				if (cur != "") {
					ast.push_back(ASTNode(string(cur)));
				}
				if (val == "") {
					ast.push_back(ASTNode()); // fix: should be empty array (ASTArray)
				}
				else {
					ast.push_back(ASTNode(string("[]")));
					ast.push_back(ASTNode(string(val)));
				}
				cur = "";
				continue;
			}
			else {
				string cct = cur + expr[i];
				if (cur != "" && isOperator(cct)) {
					ast.push_back(ASTNode(string(cct)));
					cur = "";
					continue;
				}
				else if (i < len - 1 && isOperator(string() + expr[i] + expr[i + 1])) {
					if (cur != "") {
						ast.push_back(ASTNode(string(cur)));
					}
					ast.push_back(ASTNode(string() + expr[i] + expr[i + 1]));
					cur = "";
					i++;
					continue;
				}
				else {
					if (isOperator(string() + expr[i])) {
						if (ast.size() && ast[ast.size() - 1].getString() == ".") {
							ast.push_back(ASTNode(string(cur)));
							ast.push_back(ASTNode(string() + expr[i]));
							cur = "";
							continue;
						}
						if (expr[i] == '=' && cur != "") {
							ast.push_back(ASTNode(string(cur)));
						}
						else if (cur != "" && isNaN(cur)) { // && ((args && (cur in args)) || (cur in JSExec.variables))) {
							ast.push_back(ASTNode(cur));
						}
						else if (cur != "") {
							if (!numWasTrue) {
								ast.push_back(ASTNode(string(cur)));
							}
						}
						ast.push_back(ASTNode(string() + expr[i]));
						cur = "";
						continue;
					}
					else {
						cur += expr[i];
						continue;
					}
				}
			}
			cur = "";
		}
		if (i < len) {
			cur += expr[i];
		}
	}
	if (cur != "") {
		/*if (isNaN(cur) && (((args && (cur in args)) || (cur in JSExec.variables)))) {
			let value = args && (cur in args) ? args[cur] : JSExec.variables[cur];
			if (typeof value == "string") {
				ast.push_back(ASTNode(value));
			}
			else {
				ast.push_back(ASTNode(value));
			}
		}
		else {*/
		/*if (isNaN(cur) && (scope.ScopeArray.find(cur) != scope.ScopeArray.end() || globalVariables.ScopeArray.find(cur) != globalVariables.ScopeArray.end())) {
			ASTNode value = scope.ScopeArray.find(cur) != scope.ScopeArray.end() ? scope.ScopeArray[cur] : globalVariables.ScopeArray[cur];
			ast.push_back(value);
		}
		else {*/
		if (isNaN(cur)) {
			ast.push_back(ASTNode(string(cur)));
		}
		else {
			ast.push_back(ASTNode(stold(cur)));
		}
		//}
	}
	return ast;
}

typedef ASTNode(*operatorFunction)(ASTNode op1, ASTNode op2, Scope& args, AbstractSyntaxTree& ast);
ASTNode Add(ASTNode op1, ASTNode op2, Scope& args, AbstractSyntaxTree& ast) {
	bool op1IsString = op1.getString()[0] == '"' && op1.getString()[op1.getString().size() - 1] == '"';
	bool op2IsString = op2.getString()[0] == '"' && op2.getString()[op2.getString().size() - 1] == '"';
	op1 = resolve(op1, args);
	op2 = resolve(op2, args);
	bool op1IsNumber = !op1IsString && !isNaN(op1.getString());
	bool op2IsNumber = !op2IsString && !isNaN(op2.getString());
	if (op1IsNumber && op2IsNumber) {
		return op1.getNumber() + op2.getNumber();
	}
	return "\"" + resolveString(op1.getString()) + resolveString(op2.getString()) + "\"";
}
ASTNode Subtract(ASTNode op1, ASTNode op2, Scope& args, AbstractSyntaxTree& ast) {
	op1 = resolve(op1, args);
	op2 = resolve(op2, args);
	return op1.getNumber() - op2.getNumber();
}
ASTNode Multiply(ASTNode op1, ASTNode op2, Scope& args, AbstractSyntaxTree& ast) {
	op1 = resolve(op1, args);
	op2 = resolve(op2, args);
	return op1.getNumber() * op2.getNumber();
}
ASTNode Divide(ASTNode op1, ASTNode op2, Scope& args, AbstractSyntaxTree& ast) {
	op1 = resolve(op1, args);
	op2 = resolve(op2, args);
	return op1.getNumber() / op2.getNumber();
}

void printParseNode(ParseNode* node, string indent);
void printNode(ASTNode& node) {
	if (node.ASTNodeFunc != nullptr) {
		printParseNode(node.ASTNodeFunc, "");
		return;
	}
	if (node.ASTArray->size() == 0) {
		cout << node.getString() << endl;
		return;
	}
	for (auto it = node.ASTArray->begin(); it != node.ASTArray->end(); it++) {
		cout << it->first << ": ";
		printNode(*it->second);
	}
	return;
}
ASTNode PlusEquals(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	return (globalVariables.ScopeArray[op1.getString()].ASTNodeNumber += op2.getNumber());
}
ASTNode MinusEquals(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	return (globalVariables.ScopeArray[op1.getString()].ASTNodeNumber -= op2.getNumber());
}
ASTNode TimesEquals(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	return (globalVariables.ScopeArray[op1.getString()].ASTNodeNumber *= op2.getNumber());
}
ASTNode DivideEquals(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	return (globalVariables.ScopeArray[op1.getString()].ASTNodeNumber /= op2.getNumber());
}
ASTNode NewOperator(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	return op2;
}
ASTNode Let(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	/*if (typeof op2 == 'function') {
		op2 = op2()
	}*/

	op2 = resolve(op2, scope);
	if (op1.scopeBox != nullptr) {
		ASTNode retval = resolveFunc(op1.scopeBox, true, op2);
		return retval;
	}

	string op1Str = resolveString(op1.getString());

	Scope* ptrOrigScope = &scope;
	Scope* ptrScope = &scope;
	bool found = false;
	do {
		if (ptrScope->ScopeArray.find(op1Str) != ptrScope->ScopeArray.end()) {
			found = true;
			break;
		}
	} while (ptrScope->__parent != nullptr && (ptrScope = ptrScope->__parent));

	if (found) {
		ptrScope->ScopeArray[op1Str] = op2;
		return ptrScope->ScopeArray[op1Str];
	}
	else if (globalVariables.ScopeArray.find(op1Str) != globalVariables.ScopeArray.end()) {
		return globalVariables.ScopeArray[op1Str] = op2;
	}
	else {
		return (ptrOrigScope->ScopeArray[op1Str] = op2);
	}
}
ASTNode LessThanOrEqualTo(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	op1 = resolve(op1, scope);
	op2 = resolve(op2, scope);
	return ASTNode(op1.getNumber() <= op2.getNumber() ? 1.0L : 0.0L);
}
ASTNode GreaterThan(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	op1 = resolve(op1, scope);
	op2 = resolve(op2, scope);
	return ASTNode(op1.getNumber() > op2.getNumber() ? 1.0L : 0.0L);
}
ASTNode LessThan(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	op1 = resolve(op1, scope);
	op2 = resolve(op2, scope);
	return ASTNode(op1.getNumber() < op2.getNumber() ? 1.0L : 0.0L);
}
ASTNode GreaterThanOrEqualTo(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	op1 = resolve(op1, scope);
	op2 = resolve(op2, scope);
	return ASTNode(op1.getNumber() >= op2.getNumber() ? 1.0L : 0.0L);
}
ASTNode EqualsEquals(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	op1 = resolve(op1, scope);
	op2 = resolve(op2, scope);
	return ASTNode(op1.getString() == op2.getString() ? 1.0L : 0.0L);
}
ASTNode CommaOperator(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	op1 = parseParens(op1.getString(), scope);
	op2 = parseParens(op2.getString(), scope);
	return op2;
}
ASTNode Dot(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	if (op1.scopeBox != nullptr) {
		op1 = resolveFunc(op1.scopeBox, false, ASTNode());
	}

	//op1 = resolveRuntimeObject(op1);
	//op2 = parseParens(op2.getString(), scope);

	ASTNode retval;

	//if (op1.ASTArray->find(resolveString(op2.getString())) == op1.ASTArray->end() || op1.ASTNodeString != "") {
		retval.scopeBox = new ScopeContainer;
		retval.scopeBox->scope__ = &scope;
		retval.scopeBox->op1__ = op1;
		retval.scopeBox->op2__ = op2;
	/*}
	else {
		retval = *((*op1.ASTArray)[resolveString(op2.getString())]);
	}*/

	return retval;
}
ASTNode Brackets(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	if (op1.scopeBox != nullptr) {
		op1 = resolveFunc(op1.scopeBox, false, ASTNode());
	}

	op1 = resolveRuntimeObject(op1);
	op2 = parseParens(op2.getString(), scope);

	ASTNode retval;

	if (op1.ASTArray->find(resolveString(op2.getString())) == op1.ASTArray->end() || op1.ASTNodeString != "") {
		retval.scopeBox = new ScopeContainer;
		retval.scopeBox->scope__ = &scope;
		retval.scopeBox->op1__ = op1;
		retval.scopeBox->op2__ = op2;
	}
	else {
		retval = *((*op1.ASTArray)[resolveString(op2.getString())]);
	}

	return retval;

	/*if (op1.scopeBox != nullptr) {
		op1 = resolveFunc(op1.scopeBox, false, ASTNode());
	}

	op2 = parseParens(op2.getString(), scope);

	ASTNode retval;

	if (op1.ASTNodeString != "") {
		retval.scopeBox = new ScopeContainer;
		retval.scopeBox->scope__ = &scope;
		retval.scopeBox->op1__ = op1;
		retval.scopeBox->op2__ = op2;
	}
	else {
		retval = *((*op1.ASTArray)[resolveString(op2.getString())]);
	}

	return retval;*/
}
ASTNode InOperator(ASTNode op2, ASTNode op1, Scope& scope, AbstractSyntaxTree& ast) {
	op1 = ASTNode(resolveString(op1.getString()));
	op2 = ASTNode(resolveString(op2.getString()));
	op1 = resolve(op1, scope);
	op2 = resolve(op2, scope);
	return ASTNode(op1.ASTArray->find(op2.getString()) != op1.ASTArray->end() ? 1.0L : 0.0L);
}
ASTNode Remainder(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	return ASTNode((long double) fmod(op1.getNumber(), op2.getNumber()));
}
ASTNode LogicalAnd(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	op1 = ASTNode(resolveString(op1.getString()));
	op2 = ASTNode(resolveString(op2.getString()));
	op1 = resolve(op1, scope);
	op2 = resolve(op2, scope);
	return ASTNode((op1.getNumber() == 1.0L && op2.getNumber() == 1.0L) ? 1.0L : 0.0L);
}
ASTNode LogicalOr(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast) {
	op1 = ASTNode(resolveString(op1.getString()));
	op2 = ASTNode(resolveString(op2.getString()));
	op1 = resolve(op1, scope);
	op2 = resolve(op2, scope);
	return ASTNode((op1.getNumber() == 1.0L || op2.getNumber() == 1.0L) ? 1.0L : 0.0L);
}
map<string, operatorFunction> operatorFunctions = {
	{"+", &Add},
	{"-", &Subtract},
	{"*", &Multiply},
	{"%", &Remainder},
	{"/", &Divide},
	{"[]", &Brackets},
	{".", &Dot},
	{"=", &Let},
	{"+=", &PlusEquals},
	{"-=", &MinusEquals},
	{"*=", &TimesEquals},
	{"/=", &DivideEquals},
	{"&&", &LogicalAnd},
	{"||", &LogicalOr},
	{"<=", &LessThanOrEqualTo},
	{">=", &GreaterThanOrEqualTo},
	{"<", &LessThan},
	{">", &GreaterThan},
	{"==", &EqualsEquals},
	{",", &CommaOperator},
	{" in ", &InOperator},
	{"new", &NewOperator}
};

typedef ASTNode(*predefinedFunction)(vector<ASTNode> args, Scope& scope);

ASTNode PredefinedAdd(vector<ASTNode> args, Scope &scope) {
	AbstractSyntaxTree emptyAst;
	ASTNode retval(0.0L);
	for (int i = 0, len = args.size(); i < len; i++) {
		retval = Add(retval, args[i], scope, emptyAst);
	}
	return retval;
}
ASTNode PredefinedMultiply(vector<ASTNode> args, Scope& scope) {
	long double retval = parseASTNode(&args[0])->getNumber();
	for (int i = 1, len = args.size(); i < len; i++) {
		retval *= parseASTNode(&args[i])->getNumber();
	}
	return retval;
}
ASTNode PredefinedSubtract(vector<ASTNode> args, Scope& scope) {
	return parseASTNode(&args[0])->getNumber() - parseASTNode(&args[1])->getNumber();
}
ASTNode PredefinedDivide(vector<ASTNode> args, Scope& scope) {
	return parseASTNode(&args[0])->getNumber() / parseASTNode(&args[1])->getNumber();
}
ASTNode PredefinedAlert(vector <ASTNode> args, Scope& scope) {
	string retval = "";
	for (int i = 0, iLen = args.size(); i < iLen; i++) {
		string strToLog = resolve(args[i], scope).getString();
		string replacedString = "";
		for (int j = 0, len = strToLog.size(); j < len; j++) {
			if (j < len - 1 && strToLog[j] == '\\' && strToLog[j + 1] == 'n') {
				replacedString += "\n";
				j++;
			}
			else {
				replacedString += strToLog[j];
			}
		}
		retval += replacedString;
	}
	string s1 = retval + "\n";
	std::wstring widestr = std::wstring(s1.begin(), s1.end());
	MessageBox(NULL, widestr.c_str(), L"Alert", MB_OK);
	return ASTNode();
}
map<string, vector<ASTNode>> eventListeners;
ASTNode PredefinedRandom(vector <ASTNode> args, Scope& scope) {
	long double randomNumber = ((double) std::rand() / (RAND_MAX));
	return ASTNode(randomNumber);
}
ASTNode PredefinedRemoveEventListener(vector <ASTNode> args, Scope& scope) {
	if (args.size() == 2) {
		string handlerName = resolveString(args[0].getString());
		while (true) {
			bool found = false;
			for (int i = 0, len = eventListeners[handlerName].size(); i < len; i++) {
				if (eventListeners[handlerName][i].ASTNodeString == args[1].ASTNodeString) {
					found = true;
					eventListeners[handlerName].erase(eventListeners[handlerName].begin() + i);
					break;
				}
			}
			if (!found) {
				break;
			}
		}
	}
	return ASTNode();
}
ASTNode PredefinedAddEventListener(vector <ASTNode> args, Scope& scope) {
	/*if (pageLoaded == false) {
		eventListenersToBindArgs.push_back(args);
		eventListenersToBindScopes.push_back(scope);
		return ASTNode();
	}*/
	if (args.size() == 2) {
		string handlerName = resolveString(args[0].getString());
		if (eventListeners.find(handlerName) == eventListeners.end()) {
			eventListeners[handlerName] = {};
		}
		eventListeners[handlerName].push_back(args[1]);
	}
	else if (args.size() == 3) {
		ASTNode astHTMLElement = resolveRuntimeObject(args[0]);
		Shape* ptrShape = astHTMLElement.ptrDOMNode->SlabDecompShape;
		string handlerName = resolveString(args[1].getString());
		if (ptrShape->eventHandlers.find(handlerName) == ptrShape->eventHandlers.end()) {
			ptrShape->eventHandlers[handlerName] = {};
		}
		ptrShape->eventHandlers[handlerName].push_back(args[2]);
	}
	return ASTNode();
}
ASTNode PredefinedLog(vector <ASTNode> args, Scope& scope) {
	string retval = "";
	for (int i = 0, iLen = args.size(); i < iLen; i++) {
		string strToLog = resolve(args[i], scope).getString();
		string replacedString = "";
		for (int j = 0, len = strToLog.size(); j < len; j++) {
			if (j < len - 1 && strToLog[j] == '\\' && strToLog[j + 1] == 'n') {
				replacedString += "\n";
				j++;
			}
			else {
				replacedString += strToLog[j];
			}
		}
		retval += replacedString;
	}
	LOut(retval);
	return 0.0L;
}

ASTNode PredefinedIsset(vector<ASTNode> args, Scope& scope) {
	string arrname = resolveString(args[0].getString());
	string key = resolveString(args[1].getString());
	bool retval = globalVariables.ScopeArray[arrname].ASTArray->find(key) != globalVariables.ScopeArray[arrname].ASTArray->end();
	return retval ? 1.0L : 0.0L;
}

ASTNode PredefinedCreatePrototype(vector<ASTNode> args, Scope& scope) {
	ASTNode retval;
	ASTNode astProto = resolveRuntimeObject(args[0]);
	(*retval.ASTArray)["prototype"] = new ASTNode;
	(*retval.ASTArray)["prototype"]->ASTArray = astProto.ASTArray;
	(*retval.ASTArray)["prototype"]->runtimeId = runtimeObjects.size();
	runtimeObjects.push_back(*(*retval.ASTArray)["prototype"]);
	int runtimeObjId = runtimeObjects.size();
	retval.runtimeId = runtimeObjId;
	runtimeObjects.push_back(retval);
	return ASTNode(string("<RuntimeObject#" + std::to_string(runtimeObjId) + '>'));
}

ASTNode PredefinedCreateElement(vector<ASTNode> args, Scope& scope) {
	string tagName = resolveString(args[0].getString());
	DOMNode* newElementNode = new DOMNode(tagName, "", 0, 0, 0);
	ASTNode astHTMLElement;
	newElementNode->SlabDecompShape = new Shape;
	newElementNode->ptrASTArray = astHTMLElement.ASTArray;
	newElementNode->set_parent_node(*elsByTagName["body"][0]);
	elsByTagName["body"][0]->appendChild(*newElementNode);
	(*astHTMLElement.ASTArray)["style"] = new ASTNode;
	int runtimeObjId = runtimeObjects.size();
	astHTMLElement.runtimeId = runtimeObjId;
	astHTMLElement.ptrDOMNode = newElementNode;
	newElementNode->astRuntimeId = runtimeObjId;
	runtimeObjects.push_back(astHTMLElement);
	return ASTNode(string("<RuntimeObject#" + std::to_string(runtimeObjId) + '>'));
}

ASTNode PredefinedSetPixel(vector<ASTNode> args, Scope& scope) {
	DOMNode* canvasEl = resolveRuntimeObject(args[0]).ptrDOMNode;
	int x = int(resolveRuntimeObject(args[1]).getNumber());
	int y = int(resolveRuntimeObject(args[2]).getNumber());
	int idx = y * 400 * 4 + x * 4;
	canvasEl->bitmap[idx] = (BYTE)255;
	canvasEl->bitmap[idx + 1] = (BYTE)0;
	canvasEl->bitmap[idx + 2] = (BYTE)0;
	canvasEl->bitmap[idx + 3] = (BYTE)255;
	return ASTNode();
}

ASTNode PredefinedDrawLine(vector<ASTNode> args, Scope& scope) {
	DOMNode* canvasEl = resolveRuntimeObject(args[0]).ptrDOMNode;
	float x1 = float(resolveRuntimeObject(args[1]).getNumber());
	float y1 = float(resolveRuntimeObject(args[2]).getNumber());
	float x2 = float(resolveRuntimeObject(args[3]).getNumber());
	float y2 = float(resolveRuntimeObject(args[4]).getNumber());
	
	D2D1_SIZE_U size = D2D1::SizeU(400, 400);

	pRenderTarget2->BeginDraw();
	pRenderTarget2->Clear(D2D1::ColorF(D2D1::ColorF::White));

	if (canvasEl->bitmap == nullptr) {
		canvasEl->bitmap = new BYTE[400 * 400 * 4];
	}

	ID2D1Bitmap* pBitmap = NULL;
	HRESULT hr = pRenderTarget2->CreateBitmap(D2D1::SizeU(400, 400), canvasEl->bitmap, 400 * 4, D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &pBitmap);

	// Draw a bitmap.
	pRenderTarget2->DrawBitmap(
		pBitmap,
		D2D1::RectF(
			0,
			0,
			400,
			400
		),
		1.0
	);

	const D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0);
	ID2D1SolidColorBrush* pBrush;
	hr = pRenderTarget2->CreateSolidColorBrush(color, &pBrush);
	pRenderTarget2->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), pBrush);
	pRenderTarget2->EndDraw();
	
	if (SUCCEEDED(hr))
	{
		if (canvasEl->bitmap && canvasEl->bitmap != nullptr) {
			delete[] canvasEl->bitmap;
			canvasEl->bitmap = nullptr;
		}
		canvasEl->bitmap = new BYTE[400 * 400 * 4];
		pWICBitmap->CopyPixels(NULL, 400 * 4, 400 * 400 * 4, canvasEl->bitmap);
	}

	SafeRelease(&pBrush);
	SafeRelease(&pBitmap);

	return ASTNode();
}

ASTNode PredefinedAppendChild(vector<ASTNode> args, Scope& scope) {
	DOMNode* newChild = resolveRuntimeObject(args[1]).ptrDOMNode;
	DOMNode* newParent = resolveRuntimeObject(args[0]).ptrDOMNode;
	if (newChild->previousSibling != nullptr) {
		newChild->previousSibling->nextSibling = newChild->nextSibling;
	}
	if (newChild->nextSibling != nullptr) {
		newChild->nextSibling->previousSibling = newChild->previousSibling;
	}
	newChild->previousSibling = nullptr;
	newChild->nextSibling = nullptr;
	newChild->parentNode->set_child_count(newChild->parentNode->get_child_count() - 1);
	newChild->set_parent_node(*newParent);
	newParent->appendChild(*newChild);
	return ASTNode();
}

ASTNode PredefinedGetWidth(vector<ASTNode> args, Scope& scope) {
	ASTNode astHTMLElement = resolveRuntimeObject(args[0]);
	return ASTNode((long double)astHTMLElement.ptrDOMNode->width);
}

ASTNode PredefinedGetHeight(vector<ASTNode> args, Scope& scope) {
	ASTNode astHTMLElement = resolveRuntimeObject(args[0]);
	return ASTNode((long double)astHTMLElement.ptrDOMNode->height);
}

ASTNode PredefinedGetLeft(vector<ASTNode> args, Scope& scope) {
	ASTNode astHTMLElement = resolveRuntimeObject(args[0]);
	return ASTNode((long double) astHTMLElement.ptrDOMNode->x);
}

ASTNode PredefinedGetTop(vector<ASTNode> args, Scope& scope) {
	ASTNode astHTMLElement = resolveRuntimeObject(args[0]);
	return ASTNode((long double) astHTMLElement.ptrDOMNode->y);
}

ASTNode PredefinedAbs(vector<ASTNode> args, Scope& scope) {
	return ASTNode(abs(args[0].getNumber()));
}

ASTNode PredefinedMin(vector<ASTNode> args, Scope& scope) {
	double min = args[0].getNumber();
	for (int i = 1, len = args.size(); i < len; i++) {
		double num = args[i].getNumber();
		if (num < min) {
			min = num;
		}
	}
	return ASTNode((long double)min);
}

ASTNode PredefinedMax(vector<ASTNode> args, Scope& scope) {
	double max = args[0].getNumber();
	for (int i = 1, len = args.size(); i < len; i++) {
		double num = args[i].getNumber();
		if (num > max) {
			max = num;
		}
	}
	return ASTNode((long double)max);
}

ASTNode PredefinedRound(vector<ASTNode> args, Scope& scope) {
	return ASTNode((long double)round(args[0].getNumber()));
}

ASTNode PredefinedLength(vector<ASTNode> args, Scope& scope) {
	ASTNode astNode = resolveRuntimeObject(args[0]);
	if (astNode.getType() == ASTStringNode) {
		return ASTNode((long double) astNode.ASTNodeString.size());
	}
	return ASTNode((long double) astNode.ASTArray->size());
}

ASTNode PredefinedToHex(vector<ASTNode> args, Scope& scope) {
	std::stringstream ss;
	ss << std::hex << (int) args[0].getNumber(); // int decimal_value
	std::string res(ss.str());
	return ASTNode(string(res));
}

map<string, predefinedFunction> predefinedFunctions = {
	{"abs", &PredefinedAbs},
	{"min", &PredefinedMin},
	{"max", &PredefinedMax},
	{"round", &PredefinedRound},
	{"Add", &PredefinedAdd},
	{"Multiply", &PredefinedMultiply},
	{"Subtract", &PredefinedSubtract},
	{"Divide", &PredefinedDivide},
	{"Log", &PredefinedLog},
	{"isset", &PredefinedIsset},
	{"createPrototype", &PredefinedCreatePrototype},
	{"createElement", &PredefinedCreateElement},
	{"SetPixel", &PredefinedSetPixel},
	{"DrawLine", &PredefinedDrawLine},
	{"appendChild", &PredefinedAppendChild},
	{"Alert", &PredefinedAlert},
	{"addEventListener", &PredefinedAddEventListener},
	{"removeEventListener", &PredefinedRemoveEventListener},
	{"random", &PredefinedRandom},
	{"getLeft", &PredefinedGetLeft},
	{"getTop", &PredefinedGetTop},
	{"getWidth", &PredefinedGetWidth},
	{"getHeight", &PredefinedGetHeight},
	{"length", &PredefinedLength},
	{"toHex", &PredefinedToHex}
};

OperatorListNode::OperatorListNode(string operatorName, ASTNode op1, ASTNode op2, int idx) : operatorName(operatorName), op1(op1), op2(op2), idx(idx) {};

void evaluateOperatorListNode(OperatorListNode* obj, Scope& args, AbstractSyntaxTree& ast) {
	if (obj->operatorName != " in ") {
		obj->operatorName = trim(obj->operatorName);
	}
	if (obj->op1.ASTNodeString != "") {
		obj->op1.ASTNodeString = trim(obj->op1.ASTNodeString);
	}
	if (obj->op2.ASTNodeString != "") {
		obj->op2.ASTNodeString = trim(obj->op2.ASTNodeString);
	}
	ASTNode val = operatorFunctions[obj->operatorName](obj->op1, obj->op2, args, ast);
	/*if (val.scopeBox != nullptr) {
		val = resolveFunc(val.scopeBox, false, ASTNode());
		LOut("hmm: " + val.getString());
	}*/
	obj->value = val;
	if (obj->prev) {
		obj->prev->op2 = val;
		if (obj->next) {
			obj->prev->next = obj->next;
		}
	}
	if (obj->next) {
		obj->next->op1 = val;
		if (obj->prev) {
			obj->next->prev = obj->prev;
		}
	}
	//int idx = unorderedAst.find(obj)
	//unorderedAst.erase(unorderedAst.begin() + idx)
}

map<string, int> precedenceTable = { { "[]", 20 }, { ".", 20 }, { "*", 15 }, { "\/", 15 }, { "%", 15 }, { "+", 14 }, { "-", 14 }, { "<", 12 }, { "<=", 12 }, { ">", 12 }, { ">=", 12 }, {" in ", 12} , { "==", 11 }, { "&&", 6 }, { "||", 5 }, { "=", 3 }, { "+=", 3 }, { "-=", 3 }, { "**=", 3 }, { "*=", 3 }, { "/=", 3 }, {",",1} };
map<string, string> associativityTable = { {"[]", "ltr"}, {".", "ltr"}, {"*", "ltr"}, {"\/", "ltr"}, {"%", "ltr"}, {"+", "ltr"}, {"-", "ltr"}, {"<", "ltr"}, {"<=", "ltr"}, {">", "ltr"}, {">=", "ltr"}, {"==", "ltr"}, {"&&", "ltr"}, {"||", "ltr"}, {"=", "rtl"}, {"+=", "rtl"}, {"-=", "rtl"}, {"**=", "rtl"}, {"*=", "rtl"}, {"/=", "rtl"}, {"%=", "rtl"}, {"<<=", "rtl"}, {">>=", "rtl"}, {"&=", "rtl"}, {"^=", "rtl"}, {"|=", "rtl"}, {",", "ltr"}, {" in ", "ltr"} };

bool OperatorListSort(OperatorListNode* a, OperatorListNode* b) {
	int bprec = precedenceTable[b->operatorName];
	int aprec = precedenceTable[a->operatorName];
	if (bprec == aprec) {
		if (associativityTable[b->operatorName] == "rtl") {
			return b->idx < a->idx;
		}
		return a->idx < b->idx;
	}
	return bprec < aprec;
}

typedef vector<OperatorListNode*> OperatorList;

ASTNode evaluate(AbstractSyntaxTree ast, Scope& args) {
	/*if (typeof ast == 'object' && Array.isArray(ast) && ast.length == 0) {
		return ast
	}
	if (typeof ast == 'object' && Array.isArray(ast) && ast.length == 1 && typeof ast[0] == 'object' && ('string' in ast[0])) {
		return ast[0].string
	}
	if (typeof ast == 'object' && ('string' in ast)) {
		return ast.string
	}
	if (typeof ast == 'number') {
		return ast
	}*/

	if (ast.size() == 0) {
		return "";
	}

	if (ast.size() == 1 && ast[0].getType() == ASTNumberNode) {
		return ast[0];
	}

	if (ast.size() == 1 && ast[0].ASTNodeString.find("<RuntimeObject#") != string::npos) {
		return ast[0].ASTNodeString;
	}

	if (ast.size() == 1 && ast[0].ASTNodeString != "" && ast[0].ASTNodeString[0] == '"' && ast[0].ASTNodeString[ast[0].ASTNodeString.size() - 1] == '"') {
		return ast[0].ASTNodeString;
	}

	if (ast.size() == 1 && ast[0].ASTNodeString == "this") {
		return ASTNode(string("<RuntimeObject#" + std::to_string(resolve(ast[0], args).runtimeId) + '>'));
	}

	if (ast.size() && (ast[0].getString()[0] == '+' || ast[0].getString()[0] == '-')) {
		ast.insert(ast.begin(), ASTNode(string("+")));
		ast.insert(ast.begin(), ASTNode(string("0")));
	}

	for (int len = ast.size(), i = len - 1; i >= 0; i--) {
		if ((/*ast[i].getType() == ASTNumberNode || */!isNaN(ast[i].getString())) && i > 0 && ast[i - 1].getString() == "!") {
			/*if (ast[i].getType() == ASTNumberNode) {
				ast[i - 1] = ASTNode(ast[i].getNumber() * -1);
			}
			else {
				ast[i - 1] = ASTNode(std::to_string(stold(ast[i].getString()) * -1));
			}*/
			ast[i - 1] = ast[i - 1].getNumber() == 1.0 ? ASTNode(0.0L) : ASTNode(1.0L);
			ast.erase(ast.begin() + i);
		}
		if (i >= 2 && trim(ast[i].getString()) != "" && !isNaN(ast[i].getString()) && ast[i - 1].getString() == "+" && ast.size() >= 2 && isOperator(ast[i - 2].getString())) {
			ast[i - 1] = ASTNode(+ast[i].getNumber());
			ast.erase(ast.begin() + i);
		}
		else if (i >= 2 && trim(ast[i].getString()) != "" && !isNaN(ast[i].getString()) && ast[i - 1].getString() == "-" && ast.size() >= 2 && isOperator(ast[i - 2].getString())) {
			ast[i - 1] = ASTNode(-ast[i].getNumber());
			ast.erase(ast.begin() + i);
		}
	}
	if (ast.size() == 1) {
		ASTNode retval = resolve(ast[0], args);
		if (retval.ASTNodeFunc != nullptr || retval.getType() == ASTFuncNode || retval.getType() == ASTInstanceNode || retval.runtimeId != -1) {
			int runtimeObjId = runtimeObjects.size();
			if (retval.runtimeId == -1) {
				retval.runtimeId = runtimeObjId;
				runtimeObjects.push_back(retval);
			}
			return ASTNode(string("<RuntimeObject#" + std::to_string(retval.runtimeId) + '>'));
		}
		return retval;
	}

	OperatorList unorderedAst;
	int idx = 0;

	for (int i = 0, len = ast.size(); i < len; i++) {
		if (isOperator(ast[i].getString())) {
			OperatorListNode* cur = new OperatorListNode(ast[i].getString(), i == 0 ? ASTNode() : ast[i - 1], ast[i + 1], idx++);
			if (unorderedAst.size()) {
				OperatorListNode* prev = unorderedAst[unorderedAst.size() - 1];
				if (prev && prev != nullptr) {
					cur->prev = prev;
					prev->next = cur;
				}
			}
			unorderedAst.push_back(cur);
		}
	}
	OperatorList orderedAst = unorderedAst;
	std::sort(orderedAst.begin(), orderedAst.end(), OperatorListSort);

	ASTNode retval;
	while (orderedAst.size() > 1) {
		evaluateOperatorListNode(orderedAst[0], args, ast);
		retval = orderedAst[0]->value;
		orderedAst.erase(orderedAst.begin());
	}
	if (orderedAst.size()) {
		evaluateOperatorListNode(orderedAst[0], args, ast);
		retval = orderedAst[0]->value;
	}

	if (retval.scopeBox != nullptr) {
		retval = resolveFunc(retval.scopeBox, false, ASTNode());
		if (retval.runtimeId != -1) {
			return "<RuntimeObject#" + std::to_string(retval.runtimeId) + '>';
		}
		return retval;
	}

	return isNaN(retval.getString()) ? resolve(retval, args).getString() : std::to_string(retval.getNumber());
	//return std::to_string(retval.getNumber());

	string strRetval = resolve(retval, args).getString();

	/*if (retval.string) {
		return '"' + retval.string + '"'
	}*/
	return strRetval;
}

vector<ASTNode> parseArguments(string argstring, Scope& scope);

ParseNode::ParseNode(string line) : line(line) {};
ParseNode::ParseNode() {};

map<string, ParseNode*> userDefinedFunctions;

void printParseNode(ParseNode* node, string indent) {
	LOut(indent + "ParseNode(type=" + std::to_string(node->type));
	if (node->name != "") {
		LOut(",name=" + node->name);
	}
	if (node->expr != "") {
		LOut(",expr=" + node->expr);
	}
	if (node->exprNode != nullptr) {
		LOut(",exprNode=\n");
		printParseNode(node->exprNode, indent + "    ");
		LOut(indent);
	}
	if (node->statement != "") {
		LOut(",statement=" + node->statement);
	}
	if (node->switchCase != "") {
		LOut(",switchCase=" + node->switchCase);
	}
	LOut(")\n");
	if (node->line != "") {
		LOut(indent + "    \"" + node->line + "\"\n");
	}
	if (node->parameters.size()) {
		LOut(indent + "    parameters:\n");
		for (int i = 0, len = node->parameters.size(); i < len; i++) {
			LOut(indent + "    " + node->parameters[i] + "\n");
		}
	}
	if (node->childNodes.size()) {
		LOut(indent + "    childNodes:\n");
		for (int i = 0, len = node->childNodes.size(); i < len; i++) {
			printParseNode(node->childNodes[i], indent + "        ");
		}
	}
	if (node->cases.size()) {
		LOut(indent + "    cases:\n");
		for (int i = 0, len = node->cases.size(); i < len; i++) {
			printParseNode(node->cases[i], indent + "        ");
		}
	}
	if (node->elseIfs.size()) {
		LOut(indent + "    elseIfs:\n");
		for (int i = 0, len = node->elseIfs.size(); i < len; i++) {
			printParseNode(node->elseIfs[i], indent + "        ");
		}
	}
	if (node->condition.size()) {
		LOut(indent + "    condition:\n");
		for (int i = 0, len = node->condition.size(); i < len; i++) {
			printParseNode(node->condition[i], indent + "        ");
		}
	}
	if (node->elseNode != nullptr) {
		LOut(indent + "    elseNode:" + "\n");
		printParseNode(node->elseNode, indent + "        ");
	}
}

string parseBracketedLine(string line, int i, int len) {
	while (i < len && (line[i] == ' ' || line[i] == '\t')) {
		i++;
	}
	if (line[i] != '(') {
		return "";
	}
	i++;
	int exprStartPos = i;
	i = len - 2;
	while (i >= 0 && (line[i] == ' ' || line[i] == '\t')) {
		i--;
	}
	if (line[i] != ')') {
		return "";
	}
	return line.substr(exprStartPos, i - exprStartPos);
}

ParseNode* parseLine(string line) {
	ParseNode* retval = new ParseNode();
	ParseNode* retval_other = new ParseNode();
	line = trim(line);
	retval_other->type = OtherNode;
	retval_other->line = line;
	size_t functionMatch = line.rfind("function");
	if (functionMatch != string::npos) {
		int i = functionMatch + string("function").size();
		int len = line.size();
		while (i < len && (line[i] == ' ' || line[i] == '\t')) {
			i++;
		}
		string functionName = "";
		while (i < len && (line[i] >= 'A' && line[i] <= 'Z') || (line[i] >= 'a' && line[i] <= 'z') || (line[i] >= '0' && line[i] <= '9') || line[i] == '_') {
			functionName += line[i];
			i++;
		}
		while (i < len && (line[i] == ' ' || line[i] == '\t')) {
			i++;
		}
		if (line[i] != '(') {
			return retval_other;
		}
		i++;
		string curarg = "";
		while (i < len && line[i] != ')') {
			if (line[i] == ',') {
				curarg = trim(curarg);
				if (curarg.find(' ') != string::npos) {
					return retval_other;
				}
				retval->parameters.push_back(curarg);
				curarg = "";
			}
			else {
				curarg += line[i];
			}
			i++;
		}
		curarg = trim(curarg);
		if (curarg.find(' ') != string::npos) {
			return retval_other;
		}
		retval->parameters.push_back(curarg);
		if (line[i] != ')') {
			return retval_other;
		}
		i++;
		while (i < len && (line[i] == ' ' || line[i] == '\t')) {
			i++;
		}
		if (line[i] != '{') {
			return retval_other;
		}
		retval->type = FunctionNode;
		retval->name = functionName;
	}
	else if (line.substr(0, 4) == "for ") {
		retval->type = ForNode;
	}
	else if (line.substr(0, 7) == "switch ") {
		retval->type = SwitchNode;
		int i = string("switch").size();
		int len = line.size();

		retval->expr = parseBracketedLine(line, i, len);
		if (retval->expr == "") {
			return retval_other;
		}
	}
	else if (line.substr(0, 5) == "case ") {
		retval->type = SwitchCaseNode;
		retval->switchCase = trim(line.substr(4));
		retval->switchCase = retval->switchCase.substr(0, retval->switchCase.size() - 1);
	}
	else if (line.substr(0, 3) == "if ") {
		retval->type = IfNode;
		int i = string("if").size();
		int len = line.size();

		retval->condition = { new ParseNode(parseBracketedLine(line, i, len)) };
		if (retval->condition[0]->line == "") {
			return retval_other;
		}
	}
	else if (line.substr(0, 7) == "else if") {
		retval->type = ElseIfNode;
		int i = string("else if").size();
		int len = line.size();

		retval->condition = { new ParseNode(parseBracketedLine(line, i, len)) };
		if (retval->condition[0]->line == "") {
			return retval_other;
		}
	}
	else if (line.substr(0, 5) == "else ") {
		retval->type = ElseNode;
	}
	else if (line.substr(0, 4) == "var ") {
		retval->type = VarNode;
		line = trim(line.substr(4));
		int eqIdx = line.find('=');
		string varName = trim(line.substr(0, eqIdx));
		retval->name = varName;
		retval->expr = trim(line.substr(eqIdx + 1));
	}
	else if (line.substr(0, 7) == "return ") {
		retval->type = ReturnNode;
		retval->statement = trim(line.substr(6));
	}
	else if (line.substr(0, 3) == "do ") {
		retval->type = DoWhileNode;
	}
	else if (line.substr(0, 6) == "while ") {
		retval->type = WhileNode;
	}
	else {
		retval->type = OtherNode;
		retval->line = line;
	}
	return retval;
}

bool matchWord(string line, string word) {
	if (line[line.size() - 1] != '(') {
		return false;
	}
	int len = line.size();
	int i = len - 2;
	while (i >= 0 && line[i] == ' ') {
		i--;
	}
	i++;
	if (i < word.size()) {
		return false;
	}
	if (line.substr(i - word.size(), word.size()) == word) {
		return true;
	}
	return false;
}

ParseNode generateAST(string src);

ParseNode execAST(ParseNode& ast, Scope& args) {
	vector<ParseNode*> childNodes;
	if (ast.childNodes.size()) {
		childNodes = ast.childNodes;
	}
	else {
		childNodes = { &ast };
	}
	if (childNodes.size() == 1 && (childNodes[0]->type == BreakNode || childNodes[0]->type == ContinueNode)) {
		return *childNodes[0];
	}
	for (int i = 0, len = childNodes.size(); i < len; i++) {
		ParseNode* node = childNodes[i];
		switch (node->type) {
		case BreakNode:
		{
			ParseNode retval;
			retval.type = ReturnNode;
			retval.retval = node;
			return retval;
		}
		break;
		case ContinueNode:
		{
			ParseNode retval;
			retval.type = ReturnNode;
			retval.retval = node;
			return retval;
		}
		break;
		case DoWhileNode:
		{
			string condition = node->condition[0]->line;
			Scope* scope = new Scope;
			scope->__parent = &args;
			do {
				ParseNode retval = execAST(*node, *scope);
				if (retval.type == BreakNode) {
					break;
				}
				else if (retval.type == ContinueNode) {

				}
			} while (parseParens(condition, *scope).getBool());
		}
		break;
		case WhileNode:
		{
			string condition = node->condition[0]->line;
			Scope* scope = new Scope;
			scope->__parent = &args;
			while (parseParens(condition, *scope).getBool()) {
				ParseNode retval = execAST(*node, *scope);
				if (retval.type == BreakNode) {
					break;
				}
				else if (retval.type == ContinueNode) {

				}
			}
		}
		break;
		case ForNode:
		{
			Scope* scope = new Scope;
			scope->__parent = &args;
			if (node->condition.size() == 1) {
				string condition = trim(node->condition[0]->line);
				bool var = false;
				size_t pos = 0;
				if (condition.substr(0, 4) == "var ") {
					var = true;
					pos = 4;
				}

				size_t len = condition.size();
				while (pos < len && condition[pos] == ' ') {
					pos++;
				}

				string identifier = "";
				while (pos < len && condition[pos] != ' ') {
					identifier += condition[pos];
					pos++;
				}

				while (pos < len && condition[pos] == ' ') {
					pos++;
				}

				string inMatch = "";
				while (pos < len && condition[pos] != ' ') {
					inMatch += condition[pos];
					pos++;
				}

				while (pos < len && condition[pos] == ' ') {
					pos++;
				}

				string obj = "";
				while (pos < len && condition[pos] != ' ') {
					obj += condition[pos];
					pos++;
				}

				AbstractSyntaxTree parsedExpr = parseExpr(obj, *scope);
				ASTNode astObj = evaluate(parsedExpr, *scope);
				//astObj = resolveFunc(astObj.scopeBox, false, ASTNode());

				for (auto it = astObj.ASTArray->begin(); it != astObj.ASTArray->end(); it++) {
					ParseNode rootNode = generateAST((var ? "var " : "") + identifier + " = \"" + it->first + "\"\n");
					execAST(rootNode, *scope);
					ParseNode retval = execAST(*node, *scope);
					if (retval.type == BreakNode) {
						break;
					}
					else if (retval.type == ContinueNode) {

					}
				}
			}
			else {
				string condition = node->condition[1]->line;
				execAST(*node->condition[0], *scope);
				if (condition[condition.size() - 1] == ';') {
					condition = condition.substr(0, condition.size() - 1);
				}
				while (parseParens(condition, *scope).getNumber() == 1.0) {
					ParseNode retval = execAST(*node, *scope);
					if (retval.type == BreakNode) {
						break;
					}
					else if (retval.type == ContinueNode) {

					}
					execAST(*node->condition[2], *scope);
				}
			}
		}
		break;
		case SwitchNode:
		{
			string expr = node->expr;
			/*for (let argName in args) {
				expr = expr.replace(argName, '"' + args[argName] + '"')
			}*/
			expr = parseParens(expr, args).getString();
			for (int i = 0, len = node->cases.size(); i < len; i++) {
				if (node->cases[i]->switchCase == expr) {
					execAST(*node->cases[i], args);
					break;
				}
			}
		}
		break;
		case IfNode:
		{
			string condition = node->condition[0]->line;
			Scope* scope = new Scope;
			scope->__parent = &args;
			bool evalCondition = parseParens(condition, *scope).getBool();
			if (evalCondition) {
				ParseNode retval = execAST(*node, *scope);
				if (retval.type == BreakNode) {
					break;
				}
				else if (retval.type == ContinueNode) {

				}
				else if (retval.type == ReturnNode) {
					return retval;
				}
				goto AfterSwitch;
			}
			for (int i = 0, len = node->elseIfs.size(); i < len; i++) {
				string condition = node->elseIfs[i]->condition[0]->line;
				bool evalCondition = parseParens(condition, *scope).getBool();
				if (evalCondition) {
					ParseNode retval = execAST(*(node->elseIfs[i]), *scope);
					if (retval.type == BreakNode) {
						break;
					}
					else if (retval.type == ContinueNode) {

					}
					else if (retval.type == ReturnNode) {
						return retval;
					}
					goto AfterSwitch;
				}
			}
			if (node->elseNode) {
				ParseNode retval = execAST(*(node->elseNode), *scope);
				if (retval.type == BreakNode) {
					break;
				}
				else if (retval.type == ContinueNode) {

				}
				else if (retval.type == ReturnNode) {
					return retval;
				}
				goto AfterSwitch;
			}
		}
		break;
		case FunctionNode:
		{
			if (node->name != "") {
				ASTNode exprFunc;
				int runtimeObjId = runtimeObjects.size();
				node->arguments = &args;
				(*exprFunc.ASTArray)["prototype"] = new ASTNode;
				(*exprFunc.ASTArray)["prototype"]->runtimeId = runtimeObjId;
				runtimeObjects.push_back(*(*exprFunc.ASTArray)["prototype"]);
				exprFunc.ASTNodeType = ASTFuncNode;
				exprFunc.ASTNodeFunc = node;
				runtimeObjId = runtimeObjects.size();
				exprFunc.runtimeId = runtimeObjId;
				runtimeObjects.push_back(exprFunc);
				args.ScopeArray[node->name] = exprFunc;
				//userDefinedFunctions[node->name] = node;
			}
		}
		break;
		case VarNode:
		{
			string expr = node->expr;
			if (node->exprNode != nullptr) {
				node->exprNode->arguments = &args;
				ASTNode exprFunc;
				int runtimeObjId = runtimeObjects.size();
				(*exprFunc.ASTArray)["prototype"] = new ASTNode;
				(*exprFunc.ASTArray)["prototype"]->runtimeId = runtimeObjId;
				runtimeObjects.push_back(*(*exprFunc.ASTArray)["prototype"]);
				exprFunc.ASTNodeType = ASTFuncNode;
				exprFunc.ASTNodeFunc = node->exprNode;
				runtimeObjId = runtimeObjects.size();
				exprFunc.runtimeId = runtimeObjId;
				runtimeObjects.push_back(exprFunc);
				args.ScopeArray[node->name] = exprFunc;
			}
			else {
				if (expr[expr.size() - 1] == ';') {
					expr = expr.substr(0, expr.size() - 1);
				}
				args.ScopeArray[node->name] = parseParens(expr, args);
			}
		}
		break;
		case ReturnNode:
		{
			string stmt = node->statement;
			Scope* scope = new Scope;
			scope->__parent = &args;
			ParseNode retval;
			retval.type = ReturnNode;
			retval.retval = parseParens(stmt, *scope);
			return retval;
		}
		break;
		default:
		{
			string line = node->line;
			if (line[line.size() - 1] == ';') {
				line = line.substr(0, line.size() - 1);
			}
			parseParens(line, args);
		}
		break;
		}
	AfterSwitch: {};
	}
	return *(new ParseNode());
}

ParseNode generateAST(string src) {
	string curLine = "";
	ParseNode* curParent = new ParseNode;
	ParseNode* curParenParent = new ParseNode;
	ParseNode* root = curParent;
	for (int i = 0, len = src.size(); i < len; i++) {
		if (src[i] == '\t') {
			continue;
		}
		curLine += src[i];
		if (src[i] == ';' && curParenParent->type == ForNode && curParent->type == ForNode) {
			ParseNode* forConditionParseNode = new ParseNode(curLine);
			forConditionParseNode->type = OtherNode;
			curParent->childNodes.push_back(forConditionParseNode);
			/*let newParent = {
				parent: curParent,
				childNodes: [],
			}
			curParent.childNodes.push(newParent)
			curParent = newParent*/
			curLine = "";
		}
		else if (src[i] == '(') {
			ParseNode* newParenParent = new ParseNode(curLine);
			newParenParent->parent = curParenParent;
			curParenParent->childNodes.push_back(newParenParent);
			curParenParent = newParenParent;
		}
		if (i > 0 && src[i] == '*' && src[i - 1] == '\/') {
			string comment = string() + src[i - 1];
			while (i < len - 1 && !(src[i] == '*' && src[i + 1] == '\/')) {
				comment += src[i];
				i++;
			}
			comment += "*/";
			i++;
			curLine = "";
		}
		else if (i > 0 && trim(curLine) == "\/\/" && src[i] == '\/' && src[i - 1] == '\/') {
			string comment = string() + src[i - 1];
			while (i < len && src[i] != '\n') {
				comment += src[i];
				i++;
			}
			curParent->childNodes.push_back(new ParseNode(comment));
			curLine = "";
		}
		else if (src[i] == '{') {
			ParseNode* start;
			start = parseLine(curLine);
			ParseNode* newParent = new ParseNode;
			newParent->parent = curParent;
			newParent->type = start->type;
			if (start->type == FunctionNode) {
				newParent->name = start->name;
				newParent->parameters = start->parameters;
				newParent->parent = curParent;
				curParent->childNodes.push_back(newParent);
				curParent = newParent;
				if (curLine != "") {
					string trimmed = trim(curLine.substr(0, curLine.find("function ")));
					if (trimmed.find("var ") != string::npos) {
						ParseNode* trimmedParseNode = parseLine(trimmed);
						trimmedParseNode->exprNode = curParent;
						curParent->parent->childNodes.pop_back();
						curParent->parent->childNodes.push_back(trimmedParseNode);
					}
					else if (trimmed.find_first_of('=') != string::npos) {
						ParseNode* curBraceNode = new ParseNode;
						string func = "";
						while (i < len) {
							func += src[i];
							if (src[i] == '{') {
								curBraceNode = new ParseNode;
								curBraceNode->parent = curBraceNode;
							}
							else if (src[i] == '}') {
								curBraceNode = curBraceNode->parent;
								if (!(curBraceNode != nullptr && curBraceNode->parent != nullptr)) {
									break;
								}
							}
							i++;
						}
						i--;
						ParseNode* curLineParseNode = new ParseNode;
						curLineParseNode->type = OtherNode;
						curLineParseNode->line = curLine + func.substr(1);
						curParent->parent->childNodes.pop_back();
						curParent->parent->childNodes.push_back(curLineParseNode);
						continue;
						//curParent.parent.childNodes.splice(curParent.parent.childNodes.length - 1, 1, [trimmed, newParent])
						/*trimmed.expr = curParent
						curParent.parent.childNodes.pop()
						curParent.parent.childNodes.push(trimmed)*/
					}
					/*else {
						curParent->parent->childNodes.erase(std::remove(curParent->parent->childNodes.begin(), curParent->parent->childNodes.end(), newParent));
						ParseNode* curBraceNode = new ParseNode;
						string func = curLine.substr(0, curLine.size() - 1);
						while (i < len) {
							func += src[i];
							if (src[i] == '{') {
								curBraceNode = new ParseNode;
								curBraceNode->parent = curBraceNode;
							}
							else if (src[i] == '}') {
								curBraceNode = curBraceNode->parent;
								if (!(curBraceNode != nullptr && curBraceNode->parent != nullptr)) {
									break;
								}
							}
							i++;
						}
						i--;
						ParseNode* ptrFuncLine = new ParseNode;
						ptrFuncLine->type = OtherNode;
						ptrFuncLine->line = func;
						curParent->parent->childNodes.push_back(ptrFuncLine);
						curLine = "";
					}*/
				}
			}
			else if (start->type == SwitchNode) {
				newParent->expr = start->expr;
				newParent->parent = curParent;
				curParent->childNodes.push_back(newParent);
				curParent = newParent;
			}
			else if (start->type == IfNode) {
				newParent->condition = start->condition;
				newParent->parent = curParent;
				curParent->childNodes.push_back(newParent);
				curParent = newParent;
			}
			else if (start->type == ElseNode) {
				curParent->childNodes[curParent->childNodes.size() - 1]->elseNode = newParent;
				curParent = newParent;
			}
			else if (start->type == ElseIfNode) {
				newParent->condition = start->condition;
				newParent->parent = newParent->parent->type == IfNode ? newParent->parent : curParent;
				newParent->condition = start->condition;
				curParent->childNodes[curParent->childNodes.size() - 1]->elseIfs.push_back(newParent);
				curParent = newParent;
			}
			else if (start->type == DoWhileNode) {
				newParent->type = DoWhileNode;
				curParent->childNodes.push_back(newParent);
				curParent = newParent;
			}
			else {
				if (curParent->type == WhileNode && curParenParent->childNodes[curParenParent->childNodes.size() - 1]->type == WhileNode) {

				}
				else if (curParent->type == ForNode && curParenParent->childNodes[curParenParent->childNodes.size() - 1]->type == ForNode) {

				}
				else {
					//start->type = ObjectLiteralNode;

					ASTNode* curBraceNode = new ASTNode;
					ASTNode* rootBraceNode = curBraceNode;
					string curBraceStr = "";
					i++;
					int len = src.size();
					string blockContents = "";
					while (i < len) {
						blockContents += src[i];
						if (src[i] == '{') {
							ASTNode* newBraceNode = new ASTNode;
							newBraceNode->parent = curBraceNode;
							curBraceNode = newBraceNode;
						}
						else if (src[i] == '}') {
							if (curBraceNode->parent != nullptr) {
								curBraceNode = curBraceNode->parent;
							}
							else {
								break;
							}
						}
						i++;
					}
					curLine += blockContents;
					continue;
				}
			}
			curLine = "";
		}
		else if (matchWord(curLine, "while") && src[i] == '(') {
			ParseNode start;
			ParseNode* newParent = new ParseNode;
			newParent->type = WhileNode;
			newParent->parent = curParent;
			curParenParent->type = WhileNode;
			curParent->childNodes.push_back(newParent);
			curParent = newParent;
			curLine = "";
		}
		else if (matchWord(curLine, "for") && src[i] == '(') {
			ParseNode start;
			ParseNode* newParent = new ParseNode;
			newParent->type = ForNode;
			newParent->parent = curParent;
			curParenParent->type = ForNode;
			//curParenParent->forNode = newParent; // ?
			curParent->childNodes.push_back(newParent);
			curParent = newParent;
			curLine = "";
		}
		else if ((curParenParent->type == WhileNode || curParenParent->type == ForNode) && (curParent->type == WhileNode || curParent->type == ForNode) && src[i] == ')') {
			ParseNode* loopConditionNode = new ParseNode(curLine.substr(0, curLine.size() - 1));
			loopConditionNode->type = OtherNode;
			curParent->childNodes.push_back(loopConditionNode);
			curParent->condition = curParent->childNodes;
			curParent->childNodes = {};
		}
		else if (trim(curLine) == "break") {
			ParseNode* breakNode = new ParseNode;
			breakNode->type = BreakNode;
			breakNode->parent = curParent;
			curParent->childNodes.push_back(breakNode);
			curLine = "";
		}
		else if (trim(curLine) == "continue") {
			ParseNode* continueNode = new ParseNode;
			continueNode->type = ContinueNode;
			continueNode->parent = curParent;
			curParent->childNodes.push_back(continueNode);
			curLine = "";
		}
		else if (src[i] == '\n' || src[i] == '}') {
			if (trim(curLine) != "") {
				if (src[i] == '}') {
					curLine = trim(curLine.substr(0, curLine.size() - 1));
				}
				ParseNode* start = parseLine(curLine);
				if (start->type == SwitchCaseNode) {
					ParseNode* newParent = new ParseNode;
					newParent->type = SwitchCaseNode;
					newParent->switchCase = start->switchCase;
					newParent->parent = curParent->type == SwitchNode ? curParent : curParent->parent;
					newParent->parent->cases.push_back(newParent);
					curParent = newParent;
				}
				else {
					//if (start->type != "") {
					curParent->childNodes.push_back(start);
					//}
				}
			}
			curLine = "";
			if (src[i] == '}') {
				curParent = curParent->parent;
			}
		}

		if (src[i] == ')') {
			curParenParent = curParenParent->parent;
			if (curParent->type == WhileNode) {
				ParseNode* prev = curParent->parent->childNodes[curParent->parent->childNodes.size() - 2];
				if (prev && prev->type == DoWhileNode) {
					prev->condition = curParent->condition;
					curParent->parent->childNodes.pop_back();
					curParent = curParent->parent;
					curLine = "";
				}
			}
		}
	}
	return *root;
}

ASTNode* parseArrayLiteral(string expr, int& i);

ASTNode* parseObjectLiteral(string expr, int& i) {
	ASTNode* curBraceNode = new ASTNode;
	ASTNode* rootBraceNode = curBraceNode;
	ASTNode* curParenNode = new ASTNode;
	ASTNode* rootParenNode = curParenNode;
	string curBraceStr = "";
	i++;
	int len = expr.size();
	string key = "";
	ASTNode* value = new ASTNode;
	string key_or_value = "key";
	while (i < len) {
		if (expr[i] == '(') {
			ASTNode* newNode = new ASTNode;
			newNode->parent = curParenNode;
			curParenNode = newNode;
			value->ASTNodeString += expr[i];
			i++;
			while (i < len) {
				if (expr[i] == ')') {
					value->ASTNodeString += expr[i];
					curParenNode = curParenNode->parent;
					if (curParenNode->parent == nullptr) {
						break;
					}
				}
				else if (expr[i] == '(') {
					value->ASTNodeString += expr[i];
					ASTNode* newNode = new ASTNode;
					newNode->parent = curParenNode;
					curParenNode = newNode;
				}
				else {
					value->ASTNodeString += expr[i];
				}
				i++;
			}
		}
		else if (expr[i] == '[') {
			value = parseArrayLiteral(expr, i);
		}
		else if (expr[i] == '{') {
			ASTNode* newBraceNode = new ASTNode;
			newBraceNode->parent = curBraceNode;
			(*curBraceNode->ASTArray)[/*trim*/key] = newBraceNode;
			key = "";
			value = new ASTNode;
			key_or_value = "key";
			curBraceNode = newBraceNode;
		}
		else if (expr[i] == '}') {
			if (key != "") {
				if (value->ASTArray->size() != 0) {
					(*curBraceNode->ASTArray)[key] = value;
				}
				else if (!isNaN(value->ASTNodeString)) {
					(*curBraceNode->ASTArray)[key] = new ASTNode((long double)stod(value->ASTNodeString));
				}
				else if (/*trim(*/value->getString() != "") {
					Scope args;
					(*curBraceNode->ASTArray)[key] = new ASTNode(parseParens(value->getString(), args));
				}
			}
			key = "";
			value = new ASTNode;
			key_or_value = "key";
			if (curBraceNode->parent != nullptr) {
				curBraceNode = curBraceNode->parent;
			}
			else {
				break;
			}
		}
		else if (key_or_value == "key" && (expr[i] == '\'' || expr[i] == '\"')) {
			char quot = expr[i];
			i++;
			key = quot;
			while (i < len && expr[i] != quot) {
				key += expr[i];
				i++;
			}
			key += quot;
		}
		else if (expr[i] == ':') {
			key_or_value = "value";
		}
		else if (expr[i] == ',') {
			if (key != "") {
				if (value->ASTArray->size() != 0) {
					(*curBraceNode->ASTArray)[key] = value;
				}
				else if (!isNaN(value->ASTNodeString)) {
					(*curBraceNode->ASTArray)[key] = new ASTNode((long double) stod(value->ASTNodeString));
				}
				else if (/*trim(*/value->getString() != "") {
					Scope args;
					(*curBraceNode->ASTArray)[key] = new ASTNode(parseParens(value->getString(), args));
				}
			}
			key_or_value = "key";
			key = "";
			value = new ASTNode;
		}
		else {
			if (key_or_value == "key") {
				key += expr[i];
			}
			else if (key_or_value == "value") {
				value->ASTNodeString += expr[i];
			}
		}
		i++;
	}
	return rootBraceNode;
}
ASTNode* parseArrayLiteral(string expr, int& i) {
	ASTNode* curBracketNode = new ASTNode;
	ASTNode* rootBracketNode = curBracketNode;
	i++;
	int len = expr.size();
	ASTNode* curElement = new ASTNode;
	while (i < len) {
		if ((expr[i] == '\'' || expr[i] == '\"')) {
			char quot = expr[i];
			i++;
			curElement->ASTNodeString = quot;
			while (i < len && expr[i] != quot) {
				curElement->ASTNodeString += expr[i];
				i++;
			}
			curElement->ASTNodeString += quot;
		}
		else if (expr[i] == '{') {
			(*curBracketNode->ASTArray)[std::to_string(curBracketNode->ASTArray->size())] = parseObjectLiteral(expr, i);
		}
		else if (expr[i] == '[') {
			ASTNode* newBracketNode = new ASTNode;
			newBracketNode->parent = curBracketNode;
			(*curBracketNode->ASTArray)[std::to_string(curBracketNode->ASTArray->size())] = newBracketNode;
			curBracketNode = newBracketNode;
		}
		else if (expr[i] == ']') {
			if (curElement->ASTArray->size() != 0) {
				(*curBracketNode->ASTArray)[std::to_string(curBracketNode->ASTArray->size())] = curElement;
			}
			else if (/*trim(*/curElement->getString() != "") {
				Scope args;
				ASTNode retval = parseParens(curElement->ASTNodeString, args);
				if (retval.ASTNodeString != "") {
					retval = ASTNode(resolveString(retval.ASTNodeString));
				}
				(*curBracketNode->ASTArray)[std::to_string(curBracketNode->ASTArray->size())] = new ASTNode(retval);
			}
			curElement = new ASTNode;
			if (curBracketNode->parent != nullptr) {
				curBracketNode = curBracketNode->parent;
			}
			else {
				break;
			}
		}
		else if (expr[i] == ',') {
			if (curElement->ASTArray->size() != 0) {
				(*curBracketNode->ASTArray)[std::to_string(curBracketNode->ASTArray->size())] = curElement;
			}
			else if (/*trim(*/curElement->getString() != "") {
				Scope args;
				ASTNode retval = parseParens(curElement->ASTNodeString, args);
				if (retval.ASTNodeString != "") {
					retval = ASTNode(resolveString(retval.ASTNodeString));
				}
				(*curBracketNode->ASTArray)[std::to_string(curBracketNode->ASTArray->size())] = new ASTNode(retval);
			}
			curElement = new ASTNode;
		}
		else {
			curElement->ASTNodeString += expr[i];
		}
		i++;
	}
	return rootBracketNode;
}

ASTNode* parseNode(ASTNode* node, Scope& args) {
	ASTNode* retval = node;
	/*if ((typeof node == 'object' && node.__fromLiteral) || Array.isArray(node) || (typeof node == 'object' && ('type' in node) && node.type == 'function')) {
		let runtimeObjId = JSExec.nextRuntimeObjectId++
			JSExec.runtimeObjects[runtimeObjId] = node
			return '<RuntimeObject#' + runtimeObjId + '>'
	}
	else {*/
	if (node->ASTNodeFunc != nullptr || node->getType() == ASTFuncNode) {
		int runtimeObjId = runtimeObjects.size();
		node->runtimeId = runtimeObjId;
		runtimeObjects.push_back(*node);
		return new ASTNode(string("<RuntimeObject#" + std::to_string(runtimeObjId) + '>'));
	}
	retval = parseASTNode(retval);
	//if (typeof node=='string') return '"' + node + '"'
	if (retval->scopeBox != nullptr) {
		while (retval->scopeBox != nullptr) {
			retval = &resolveFunc(retval->scopeBox, false, ASTNode());
		}
	}
	retval = parseASTNode(retval);
	if (retval->childNodes.size()) {
		string joined = "";
		for (int i = 0, len = retval->childNodes.size(); i < len; i++) {
			joined += parseNode(retval->childNodes[i], args)->getString();
		}
		string evaluated = evaluate(parseExpr(joined, args), args).getString();
		/*cout << "joined: " << joined << endl;
		cout << "evaluated: " << evaluated << endl;*/
		if (isNaN(evaluated)) {
			return new ASTNode('"' + evaluated + '"');
		}
		return new ASTNode(evaluated);
	}
	//if (typeof retval == 'string' || typeof retval == 'number') {
	if (retval->getType() != ASTBoolNode) {
		string retstr = retval->getString();
		char firstChar = retstr[0];
		char lastChar = retstr[retstr.size() - 1];
		if (((firstChar >= 'A' && firstChar <= 'Z') || (firstChar >= 'a' && firstChar <= 'z') || (firstChar >= '0' && firstChar <= '9') || firstChar == '_') && ((lastChar >= 'A' && lastChar <= 'Z') || (lastChar >= 'a' && lastChar <= 'z') || (lastChar >= '0' && lastChar <= '9') || lastChar == '_')) {
			return new ASTNode(retstr);
		}
	}
	//}
	/*if (typeof retval == 'object' && ('string' in retval)) {
		return '"' + retval.string + '"'
	}*/
	return retval;
}

ParseNode *parseFunction(string expr, int &i) {
	bool syntaxError = true;
	size_t len = expr.size();
	int startPos = i;
	i += string("function").size();
	while (i < len && expr[i] == ' ') {
		i++;
	}
	string funcName = "";
	if (expr[i] != '(') {
		while (i < len && ((expr[i] >= 'A' && expr[i] <= 'Z') || (expr[i] >= 'a' && expr[i] <= 'z') || (expr[i] >= '0' && expr[i] <= '0') || expr[i] == '_'))
		{
			funcName += expr[i];
			i++;
		}
	}
	while (i < len && expr[i] == ' ') {
		i++;
	}
	string args = "";
	if (expr[i] == '(') {
		i++;
		while (i < len && ((expr[i] >= 'A' && expr[i] <= 'Z') || (expr[i] >= 'a' && expr[i] <= 'z') || (expr[i] >= '0' && expr[i] <= '9') || expr[i] == '_' || expr[i] == ',' || expr[i] == ' ')) {
			args += expr[i];
			i++;
		}
		if (expr[i] == ')') {
			i++;
			while (i < len && expr[i] == ' ') {
				i++;
			}
			if (expr[i] == '{') {
				syntaxError = false;
			}
		}
	}
	if (!syntaxError) {
		ParseNode* braceNode = new ParseNode;
		string src = "{";
		i++;
		while (i < len) {
			if (expr[i] == '{') {
				ParseNode *newBraceNode = new ParseNode;
				newBraceNode->parent = braceNode;
				braceNode = newBraceNode;
			}
			else if (expr[i] == '}') {
				braceNode = braceNode->parent;
				if (braceNode == nullptr) {
					ParseNode *funcAST = generateAST(expr.substr(startPos, i + 1 - startPos)).childNodes[0];
					return funcAST;
				}
			}
			src += expr[i];
			i++;
		}
	}
}

ASTNode parseParens(string expr, Scope& args) {
	expr = '(' + trim(expr) + ')';
	ASTNode* curParent = new ASTNode;
	ASTNode* root = curParent;
	string curChild = "";
	for (int i = 0, len = expr.size(); i < len; i++) {
		if (i < len - 1 && expr[i] == ' ' && curChild != "new" && expr.substr(i, 4) != " in " && curChild != " in") {
			continue;
		}
		if (expr.substr(i, string("function").size()) == "function") {
			ASTNode* funcASTNode = new ASTNode(parseFunction(expr, i));
			curParent->childNodes.push_back(funcASTNode);
		}
		else if (expr[i] == '{') {
			if (curChild != "") {
				curParent->childNodes.push_back(new ASTNode(curChild));
			}
			ASTNode* ptrASTArray = parseObjectLiteral(expr, i);
			int runtimeObjId = runtimeObjects.size();
			ptrASTArray->runtimeId = runtimeObjId;
			runtimeObjects.push_back(*ptrASTArray);
			curParent->childNodes.push_back(new ASTNode(string("<RuntimeObject#" + std::to_string(runtimeObjId) + '>')));
			curChild = "";
		}
		else if (expr[i] == '[' && !(expr[i - 1] >= 'A' && expr[i - 1] <= 'Z') && !(expr[i - 1] >= 'a' && expr[i - 1] <= 'z') && !(expr[i - 1] >= '0' && expr[i - 1] <= '9') && expr[i - 1] != '_' && expr[i - 1] != ']') {
			if (curChild != "") {
				curParent->childNodes.push_back(new ASTNode(curChild));
			}
			ASTNode *ptrASTArray = parseArrayLiteral(expr, i);
			int runtimeObjId = runtimeObjects.size();
			ptrASTArray->runtimeId = runtimeObjId;
			runtimeObjects.push_back(*ptrASTArray);
			curParent->childNodes.push_back(new ASTNode(string("<RuntimeObject#" + std::to_string(runtimeObjId) + '>')));
			curChild = "";
		}
		else if (expr[i] == '"' || expr[i] == '\'') {
			char quotChar = expr[i];
			if (curChild != "") {
				curParent->childNodes.push_back(new ASTNode(curChild));
			}
			curChild = "";
			string curStr = "";
			i++;
			string slashes = "";
			while (i < len && expr[i] != quotChar) {
				if (expr[i] == '\\') {
					slashes += expr[i];
				}
				curStr += expr[i];
				if (i < len - 1 && (expr[i + 1] == '"' || expr[i + 1] == '\'' || expr[i + 1] == 'n') && (slashes.size() % 2 != 0)) {
					slashes = "";
					curStr += expr[i + 1];
					i++;
				}
				i++;
			}
			curParent->childNodes.push_back(new ASTNode("\"" + curStr + "\""));
		}
		else if (expr[i] == '(') {
			string funcName = "";
			string origFuncName;
			ASTNode func;
			if (curChild != "") {
				int len = curChild.size();
				int i = len - 1;
				while (i >= 0) {
					if ((curChild[i] >= 'A' && curChild[i] <= 'Z') || (curChild[i] >= 'a' && curChild[i] <= 'z') || (curChild[i] >= '0' && curChild[i] <= '9') || curChild[i] == '_' || curChild[i] == '.') {
						funcName = curChild[i] + funcName;
					}
					else {
						break;
					}
					i--;
				}
				if ((funcName[0] >= 'A' && funcName[0] <= 'Z') || (funcName[0] >= 'a' && funcName[0] <= 'z')) {
					funcName = funcName;
					origFuncName = funcName;
					funcName = parseParens(funcName, args).getString();
				}
				else {
					funcName = "";
				}
				if (func.getString() == "" && (predefinedFunctions.find(funcName) == predefinedFunctions.end()) && (userDefinedFunctions.find(funcName) == userDefinedFunctions.end())) {
					curParent->childNodes.push_back(new ASTNode(curChild));
					funcName = "";
				}
				else {
					if (funcName != "") {
						if (curChild.substr(0, curChild.size() - origFuncName.size()) != "") {
							curParent->childNodes.push_back(new ASTNode(trim(curChild.substr(0, curChild.size() - origFuncName.size()))));
						}
					}
					else {
						curParent->childNodes.push_back(new ASTNode(curChild));
					}
				}
			}
			curChild = "";
			ASTNode* newParent = new ASTNode;
			newParent->parent = curParent;
			if (funcName != "") {
				newParent->funcName = funcName;
				newParent->origFuncName = origFuncName;
			}
			curParent->childNodes.push_back(newParent);
			curParent = newParent;
			newParent = new ASTNode;
			newParent->parent = curParent;
			curParent = newParent;
		}
		else if (expr[i] == ')') {
			if (curChild != "") {
				if (curParent->childNodes.size()) {
					/*if (curParent->childNodes[curParent->childNodes.size() - 1]->getString() == "") {
						curParent->childNodes[curParent->childNodes.size() - 1] = new ASTNode(curParent->childNodes[curParent->childNodes.size() - 1]->getString() + curChild);
					}*/
					curParent->childNodes[curParent->childNodes.size() - 1] = new ASTNode(string(curParent->childNodes[curParent->childNodes.size() - 1]->getString() + curChild));
				}
				else {
					curParent->childNodes.push_back(new ASTNode(string(curChild)));
				}
			}
			if (find(curParent->parent->childNodes.begin(), curParent->parent->childNodes.end(), curParent) == curParent->parent->childNodes.end()) {
				curParent->parent->childNodes.push_back(curParent);
			}
			int idx = std::distance(curParent->parent->childNodes.begin(), find(curParent->parent->childNodes.begin(), curParent->parent->childNodes.end(), curParent));
			int curIdx = std::distance(curParent->parent->parent->childNodes.begin(), find(curParent->parent->parent->childNodes.begin(), curParent->parent->parent->childNodes.end(), curParent->parent));
			ASTNode prevAST;
			
			if (curIdx > 0 && curParent->parent != nullptr && curParent->parent->parent != nullptr) {
				prevAST = *curParent->parent->parent->childNodes[curIdx - 1];
			}

			if (prevAST.getString() != "" && prevAST.getType() != ASTBoolNode) {
				if (prevAST.ASTNodeString.find("<RuntimeObject#") != string::npos) {
					
				}
				else if (trim(prevAST.getString()) == "new") {
					prevAST = ASTNode();
					prevAST.ASTNodeType = ASTInstanceNode;
				}
				else if (prevAST.ASTNodeString != "" && find(operators.begin(), operators.end(), string() + prevAST.getString()[prevAST.getString().size() - 1]) == operators.end()) {
					prevAST = parseParens(prevAST.getString(), args);
				}
				else {
					prevAST = ASTNode();
				}
			}
			else {
				prevAST = ASTNode();
			}
			if (curParent->parent->funcName != "" || (prevAST.getString() != "" && prevAST.getType() != ASTBoolNode) || prevAST.getType() == ASTInstanceNode) {
				Scope* args2 = new Scope();
				for (auto it = args.ScopeArray.begin(); it != args.ScopeArray.end(); it++) {
					args2->ScopeArray[it->first] = it->second;
				}
				args2->__parent = args.__parent;
				string argsToParse = "";
				for (int i = 0, len = curParent->parent->childNodes.size(); i < len; i++) {
					for (int j = 0, jLen = curParent->parent->childNodes[i]->childNodes.size(); j < jLen; j++) { // ???
						ASTNode* node = parseASTNode(curParent->parent->childNodes[i]->childNodes[j]);
						if (node->runtimeId != -1) {
							argsToParse += "<RuntimeObject#" + std::to_string(node->runtimeId) + '>';
						}
						else {
							argsToParse += node->getString();
						}
					}
					argsToParse += ",";
				}
				argsToParse = argsToParse.substr(0, argsToParse.size() - 1);
				idx = std::distance(curParent->parent->parent->childNodes.begin(), find(curParent->parent->parent->childNodes.begin(), curParent->parent->parent->childNodes.end(), curParent->parent));
				
				vector<ASTNode> parsedArgs = parseArguments(argsToParse, *args2);
				ASTNode* output = nullptr;
				if (prevAST.getString() != "" && prevAST.getType() != ASTBoolNode) {
					ASTNode parsedNode;
					if (curParent->parent != nullptr && curParent->parent->parent != nullptr && idx > 0) {
						parsedNode = ASTNode(*curParent->parent->parent->childNodes[idx - 1]);
					}
					if (prevAST.ASTNodeString.find("<RuntimeObject#") != string::npos) {
						parsedNode = prevAST;
					}
					if (parsedNode.getString() != "" && parsedNode.ASTNodeString.find("<RuntimeObject#") != string::npos) {
						string runtimeObjectId = parsedNode.ASTNodeString.substr(parsedNode.ASTNodeString.find("<RuntimeObject#") + string("<RuntimeObject#").size());
						runtimeObjectId = runtimeObjectId.substr(0, runtimeObjectId.size() - 1);
						output = &runtimeObjects[stoi(runtimeObjectId)];
						//output = runtimeObjects[parsedNode.string.match(/ \<RuntimeObject#([0 - 9] + )\> / )[1]]
					}
					else if (prevAST.getType() == FunctionNode) {
						output = &prevAST;
					}
					else {
						output = nullptr;
					}
					if (output != nullptr) {
						if (output->ASTNodeFunc != nullptr) {
							Scope params;
							//vector<ASTNode> args = parsedArgs;
							if (output->ASTNodeFunc->arguments) {
								params.__parent = output->ASTNodeFunc->arguments;
							}
							//params.__parent = &args;
							if (output->ASTNodeFunc != nullptr) {
								for (int i = 0, len = output->ASTNodeFunc->parameters.size(); i < len; i++) {
									if (output->ASTNodeFunc->parameters[i] != "") {
										params.ScopeArray[output->ASTNodeFunc->parameters[i]] = parsedArgs[i];
									}
								}

							}
							output = new ASTNode(execAST(*output->ASTNodeFunc, params).retval);
						}
						curParent->parent->parent->childNodes.erase(curParent->parent->parent->childNodes.begin() + idx - 1);
						idx--;
					}
				}
				if (output == nullptr) {
					ASTNode prev;
					if (curParent->parent != nullptr && curParent->parent->parent != nullptr && curIdx > 0) {
						prev = *parseASTNode(curParent->parent->parent->childNodes[curIdx - 1]);
					}
					if (prev.getString() != "" && trim(prev.getString()) == "new") {
						ParseNode* func;
						ASTNode astFunc;
						
						if (curParent->parent->funcName != "" && curParent->parent->funcName.find("<RuntimeObject#") != string::npos) {
							astFunc = resolveRuntimeObject(ASTNode(string(curParent->parent->funcName)));
							func = astFunc.ASTNodeFunc;
						}
						else {
							func = userDefinedFunctions[curParent->parent->funcName];
						}
						ParseNode* funcToInvoke = func;

						curParent->parent->parent->childNodes.erase(curParent->parent->parent->childNodes.begin() + curIdx - 1);
						idx--;

						if (func->type == FunctionNode) {
							Scope *userDefinedFunctionScope = new Scope;
							userDefinedFunctionScope->__parent = &args;
							vector<string> params = func->parameters;
							if (parsedArgs.size()) {
								for (int paramIdx = 0, paramCount = params.size(); paramIdx < paramCount; paramIdx++) {
									userDefinedFunctionScope->ScopeArray[params[paramIdx]] = parsedArgs[paramIdx];
								}
							}
							ASTNode* objectInstance = new ASTNode;
							objectInstance->ASTNodeType = ASTInstanceNode;
							int runtimeObjId = runtimeObjects.size();
							objectInstance->runtimeId = runtimeObjId;
							(*objectInstance->ASTArray)["prototype"] = (*astFunc.ASTArray)["prototype"];
							userDefinedFunctionScope->ScopeArray["this"] = *objectInstance;
							runtimeObjects.push_back(*objectInstance);
							execAST(*func, *userDefinedFunctionScope);
							output = new ASTNode("<RuntimeObject#" + std::to_string(runtimeObjId) + '>');
							//parseASTNode(curParent->parent->parent->childNodes[curIdx - 1])->ASTNodeString = "new ";
							/*let outerArgs = args
							funcToInvoke = function () {
								let params = {}
								let args = Array.from(arguments)
								for (let key in outerArgs) {
									if (outerArgs.hasOwnProperty(key)) {
										params[key] = outerArgs[key]
									}
								}
								if (func.parameters) {
									func.parameters.forEach(function (param, idx) {
										params[param] = args[idx]
									})
								}
								params['__this__'] = this
								let retval = JSExec.execAST(func.childNodes, params)
								return (typeof retval == 'object' && ('return' in retval)) ? retval.return : retval
							}*/
							//funcToInvoke.prototype = func.prototype
							/*if (typeof func.prototype == 'string') {
								let runtimeObjectMatch = func.prototype.match(/ \<RuntimeObject#([0 - 9] + )\> / )
								if (runtimeObjectMatch) {
									funcToInvoke.prototype = JSExec.runtimeObjects[runtimeObjectMatch[1]]
								}
							}
							funcToInvoke.prototype.constructor = funcToInvoke*/
						}
						/*output = new (Function.prototype.bind.apply(funcToInvoke, parsedArgs))
						output.__this__ = output
						let runtimeObjId = JSExec.nextRuntimeObjectId++
						JSExec.runtimeObjects[runtimeObjId] = output
						output = '<RuntimeObject#' + runtimeObjId + '>'*/
					}
					else if (curParent != nullptr && curParent->parent != nullptr && curParent->parent->funcName != "") {
						ParseNode *func = nullptr;
						if (curParent->parent->funcName != "" && curParent->parent->funcName.find("<RuntimeObject#") != string::npos) {
							func = resolveRuntimeObject(ASTNode(string(curParent->parent->funcName))).ASTNodeFunc;
						}
						if (predefinedFunctions.find(curParent->parent->funcName) != predefinedFunctions.end()) {
							output = new ASTNode(predefinedFunctions[curParent->parent->funcName](parsedArgs, args).getString());
						}
						else if (userDefinedFunctions.find(curParent->parent->funcName) != userDefinedFunctions.end()) {
							Scope userDefinedFunctionScope;
							userDefinedFunctionScope.__parent = &args;
							vector<string> params = userDefinedFunctions[curParent->parent->funcName]->parameters;
							for (int paramIdx = 0, paramCount = params.size(); paramIdx < paramCount; paramIdx++) {
								userDefinedFunctionScope.ScopeArray[params[paramIdx]] = parsedArgs[paramIdx];
							}
							output = new ASTNode(execAST(*userDefinedFunctions[curParent->parent->funcName], userDefinedFunctionScope).retval.getString());
						}
						else {
							/*if (userDefinedFunctions.find(curParent->parent->funcName) != userDefinedFunctions.end()) {
								func = parseParens(curParent->parent->funcName, args);
							}*/
							if (func != nullptr && func->type == FunctionNode) {
								Scope userDefinedFunctionScope;
								//userDefinedFunctionScope.__parent = &args;
								if (func->arguments) {
									userDefinedFunctionScope.__parent = func->arguments;
								}
								vector<string> params = func->parameters;
								if (parsedArgs.size()) {
									for (int paramIdx = 0, paramCount = params.size(); paramIdx < paramCount; paramIdx++) {
										userDefinedFunctionScope.ScopeArray[params[paramIdx]] = parsedArgs[paramIdx];
									}
								}
								size_t lastDot = curParent->parent->origFuncName.find_last_of('.', string::npos);
								ASTNode astThis = parseParens(curParent->parent->origFuncName.substr(0, lastDot), args);
								userDefinedFunctionScope.ScopeArray["this"] = resolveRuntimeObject(parseParens(curParent->parent->origFuncName.substr(0, lastDot), args));
								output = new ASTNode(execAST(*func, userDefinedFunctionScope).retval.getString());
							}
							else {
								/*output = (func || JSExec.functions[curParent.parent.funcName]).apply(null, parsedArgs)*/
							}
						}
					}
				}
				/*
				output->parent = curParent->parent->parent;
				curParent->parent->parent->childNodes[idx] = output;
				curParent = curParent->parent->parent->childNodes[idx];*/
				if (output != nullptr) {
					output = new ASTNode(*output);
					curParent->parent->parent->childNodes[idx] = output;
				}
				curParent = curParent->parent;
			}
			else {
				if (curParent->childNodes.size() == 1) {
					string joined = "";
					for (int i = 0, len = curParent->childNodes.size(); i < len; i++) {
						/*if ((typeof node == 'object' && node.__fromLiteral) || Array.isArray(node) || (typeof node == 'object' && ('type' in node) && node.type == 'function')) {
							let runtimeObjId = JSExec.nextRuntimeObjectId++
								JSExec.runtimeObjects[runtimeObjId] = node
								return '<RuntimeObject#' + runtimeObjId + '>'
						}*/
						ASTNode* node = parseASTNode(curParent->childNodes[i]);
						if (node->ASTNodeFunc != nullptr || node->getType() == ASTFuncNode) {
							int runtimeObjId = runtimeObjects.size();
							node->runtimeId = runtimeObjId;
							runtimeObjects.push_back(*node);
							joined += "<RuntimeObject#" + std::to_string(runtimeObjId) + '>';
						}
						else if (node->scopeBox != nullptr) {
							while (node->scopeBox != nullptr) {
								node = &resolveFunc(node->scopeBox, false, ASTNode());
							}
							joined += parseASTNode(node)->getString();
						}
						else {
							joined += node->getString();
						}
					}
					AbstractSyntaxTree exprAST = parseExpr(joined, args);
					curParent->parent->childNodes[idx] = new ASTNode(evaluate(exprAST, args));
					//curParent.parent.childNodes[idx] = {childNodes: [], ASTNodeString: typeof retval == 'string' && !retval.match(/\<RuntimeObject#[0-9]+\>/) ? {string: retval} : retval}
					//curParent->parent->childNodes[idx]->parent = curParent->parent;
				}
				else if (curParent->childNodes.size()) {
					string joined = parseNode(curParent, args)->getString();
					AbstractSyntaxTree exprAST = parseExpr(joined, args);
					curParent->parent->childNodes[idx] = new ASTNode(evaluate(exprAST, args));
					//curParent.parent.childNodes[idx] = {childNodes: [], ASTNodeString: typeof retval == 'string' && !retval.match(/\<RuntimeObject#[0-9]+\>/) ? {string: retval} : retval}
					//curParent->parent->childNodes[idx]->parent = curParent->parent;
				}
				int idx = std::distance(curParent->parent->parent->childNodes.begin(), find(curParent->parent->parent->childNodes.begin(), curParent->parent->parent->childNodes.end(), curParent->parent));
				AbstractSyntaxTree ast;
				for (int i = 0, len = curParent->parent->childNodes.size(); i < len; i++) {
					ast.push_back(*parseASTNode(curParent->parent->childNodes[i]));
				}
				
				curParent->parent->parent->childNodes[idx] = new ASTNode(evaluate(ast, args));
				curParent->parent->parent->childNodes[idx]->parent = curParent->parent->parent;

				curParent = curParent->parent;
			}
			curChild = "";
			curParent = curParent->parent;
		}
		else {
			curChild += expr[i];
		}
	}

	string retval;

	if (root->childNodes.size() == 0) {
		retval = evaluate(parseExpr(curChild, args), args).getString();
	}
	else if (root->childNodes.size() == 1) {
		AbstractSyntaxTree rootAst;
		rootAst.push_back(*parseASTNode(root));
		retval = evaluate(rootAst, args).getString();
		
	}
	else if (root->childNodes.size()) {
		string exprToParse = "";
		for (int i = 0, len = root->childNodes.size(); i < len; i++) {
			exprToParse += parseASTNode(root->childNodes[i])->getString();
		}
		retval = evaluate(parseExpr(exprToParse, args), args).getString();
	}
	else {
		retval = parseASTNode(root)->getString();
	}

	/*if (retval.string) {
		retval = retval.string
	}

	return typeof retval == = 'undefined' ? root : retval*/

	return retval;
}

vector<ASTNode> parseArguments(string argstring, Scope& scope) {
	vector<ASTNode> args;
	string curarg = "";
	for (int i = 0, len = argstring.size(); i < len; i++) {
		if (argstring[i] == '"') {
			i++;
			curarg += "\"";
			while (i < len && argstring[i] != '"') {
				curarg += argstring[i];
				i++;
			}
			curarg += '"';
		}
		else if (argstring[i] == '\'') {
			i++;
			curarg += "'";
			while (i < len && argstring[i] != '\'') {
				curarg += argstring[i];
				i++;
			}
			curarg += "'";
		}
		else if (argstring[i] == ',') {
			bool isDoubleQuote = curarg[0] == '"' && curarg[curarg.size() - 1] == '"';
			bool isSingleQuote = curarg[0] == '\'' && curarg[curarg.size() - 1] == '\'';
			/*if (isDoubleQuote) {
				args.push_back(ASTNode(curarg));//.slice(1, -1))
			}
			else if (isSingleQuote) {
				args.push_back(ASTNode(curarg));//.slice(1, -1))
			}
			else {*/
			args.push_back(parseParens(curarg, scope));
			//}
			curarg = "";
		}
		else {
			curarg += argstring[i];
		}
	}
	if (curarg != "") {
		bool isDoubleQuote = curarg[0] == '"' && curarg[curarg.size() - 1] == '"';
		bool isSingleQuote = curarg[0] == '\'' && curarg[curarg.size() - 1] == '\'';
		/*if (isDoubleQuote) {
			args.push_back(ASTNode(curarg));//.slice(1, -1))
		}
		else if (isSingleQuote) {
			args.push_back(ASTNode(curarg));//.slice(1, -1))
		}
		else {*/
		args.push_back(parseParens(curarg, scope));
		//}
	}
	return args;
}