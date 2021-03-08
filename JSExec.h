#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include "DOMNode.h"
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <wincodec.h> 
#include <commdlg.h> 
#include <Windowsx.h>
#include <fstream>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "basewin.h"
#include "WICViewerD2D.h"

class DOMNode;

using std::cout;
using std::endl;
using std::vector;
using std::map;
using std::string;

void LOut(string str);
string trim(string str);
bool isNaN(string val);

extern const vector<string> operators;
bool isOperator(string op);

class ScopeContainer;
struct ParseNode;

enum ASTNodeType { ASTStringNode, ASTNumberNode, ASTBoolNode, ASTFuncNode, ASTInstanceNode };
class ASTNode {
public:
	string* data = nullptr;
	DOMNode* ptrDOMNode = nullptr;
	int runtimeId = -1;
	ASTNodeType ASTNodeType = ASTBoolNode;
	map<string, ASTNode*>* ASTArray = new map<string, ASTNode*>;
	string funcName = "";
	string origFuncName = "";
	string ASTNodeString = "";
	long double ASTNodeNumber = 0.0;
	ParseNode* ASTNodeFunc = nullptr;
	bool ASTNodeBool = false;
	ASTNode* parent = nullptr;
	vector<ASTNode*> childNodes;
	ScopeContainer* scopeBox = nullptr;

	ASTNode(long double ASTNodeNumber);
	ASTNode(string ASTNodeString);
	ASTNode(bool ASTNodeBool);
	ASTNode(ParseNode* ASTNodeFunc);
	ASTNode();
	int getType();
	string getString();
	long double getNumber();
	bool getBool();
	bool operator==(const ASTNode& rhs);
};

typedef vector<ASTNode> AbstractSyntaxTree;
//typedef map<string, ASTNode> Scope;
struct Scope {
	map<string, ASTNode> ScopeArray;
	Scope* __parent = nullptr;
};

void printAST(AbstractSyntaxTree& ast);

class ScopeContainer {
public:
	Scope* scope__ = nullptr;
	ASTNode op1__;
	ASTNode op2__;
	ScopeContainer();
};

ASTNode parseParens(string expr, Scope& args);

extern Scope globalVariables;

string resolveString(string str);

extern vector<ASTNode> runtimeObjects;
ASTNode resolveRuntimeObject(ASTNode astRef);

ASTNode resolve(ASTNode& op, Scope& scope);
ASTNode resolveFunc(ScopeContainer* scopeBox, bool newvalExists, ASTNode newval);

ASTNode* parseASTNode(ASTNode* node);

ASTNode resolve(ASTNode& op, Scope& scope);

bool isNumeric(char val);
bool isIdentifier(char val);

AbstractSyntaxTree parseExpr(string expr, Scope& scope);

typedef ASTNode(*operatorFunction)(ASTNode op1, ASTNode op2, Scope& args, AbstractSyntaxTree& ast);
ASTNode Add(ASTNode op1, ASTNode op2, Scope& args, AbstractSyntaxTree& ast);
ASTNode Subtract(ASTNode op1, ASTNode op2, Scope& args, AbstractSyntaxTree& ast);
ASTNode Multiply(ASTNode op1, ASTNode op2, Scope& args, AbstractSyntaxTree& ast);
ASTNode Divide(ASTNode op1, ASTNode op2, Scope& args, AbstractSyntaxTree& ast);

void printParseNode(ParseNode* node, string indent);
void printNode(ASTNode& node);
ASTNode PlusEquals(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast);
ASTNode MinusEquals(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast);
ASTNode TimesEquals(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast);
ASTNode DivideEquals(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast);
ASTNode NewOperator(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast);
ASTNode Let(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast);
ASTNode LessThanOrEqualTo(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast);
ASTNode GreaterThanOrEqualTo(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast);
ASTNode EqualsEquals(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast);
ASTNode CommaOperator(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast);
ASTNode Dot(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast);
ASTNode Brackets(ASTNode op1, ASTNode op2, Scope& scope, AbstractSyntaxTree& ast);
ASTNode InOperator(ASTNode op2, ASTNode op1, Scope& scope, AbstractSyntaxTree& ast);
extern map<string, operatorFunction> operatorFunctions;

typedef ASTNode(*predefinedFunction)(vector<ASTNode> args, Scope& scope);

ASTNode PredefinedAdd(vector<ASTNode> args, Scope& scope);
ASTNode PredefinedMultiply(vector<ASTNode> args, Scope& scope);
ASTNode PredefinedSubtract(vector<ASTNode> args, Scope& scope);
ASTNode PredefinedDivide(vector<ASTNode> args, Scope& scope);
ASTNode PredefinedLog(vector <ASTNode> args, Scope& scope);
ASTNode PredefinedIsset(vector<ASTNode> args, Scope& scope);
ASTNode PredefinedCreatePrototype(vector<ASTNode> args, Scope& scope);

extern map<string, predefinedFunction> predefinedFunctions;

struct OperatorListNode {
	string operatorName;
	ASTNode op1;
	ASTNode op2;
	ASTNode value;
	OperatorListNode* prev = nullptr;
	OperatorListNode* next = nullptr;
	int idx;
	OperatorListNode(string operatorName, ASTNode op1, ASTNode op2, int idx);
};

void evaluateOperatorListNode(OperatorListNode* obj, Scope& args, AbstractSyntaxTree& ast);

extern map<string, int> precedenceTable;
extern map<string, string> associativityTable;

bool OperatorListSort(OperatorListNode* a, OperatorListNode* b);

typedef vector<OperatorListNode*> OperatorList;

ASTNode evaluate(AbstractSyntaxTree ast, Scope& args);

vector<ASTNode> parseArguments(string argstring, Scope& scope);

enum ParseNodeType { FunctionNode, WhileNode, DoWhileNode, IfNode, ElseNode, ElseIfNode, SwitchNode, SwitchCaseNode, VarNode, ReturnNode, ForNode, OtherNode, BreakNode, ContinueNode, ObjectLiteralNode };
struct ParseNode {
	Scope* arguments = nullptr;
	ParseNode* parent = nullptr;
	vector<ParseNode*> childNodes;
	ParseNodeType type = OtherNode;
	string expr;
	ParseNode* exprNode = nullptr;
	vector<ParseNode*> condition;
	string statement;
	string name;
	vector<string> parameters;
	string switchCase;
	vector<ParseNode*> elseIfs;
	vector<ParseNode*> cases;
	ParseNode* elseNode = nullptr;
	ParseNode* forNode = nullptr;
	string line = "";
	ASTNode retval;
	ParseNode(string line);
	ParseNode();
};

extern IWICBitmap* pWICBitmap;
extern ID2D1Factory* pFactory;
extern ID2D1RenderTarget* pRenderTarget2;
extern map<string, ParseNode*> userDefinedFunctions;
void printParseNode(ParseNode* node, string indent);
string parseBracketedLine(string line, int i, int len);
ParseNode* parseLine(string line);
bool matchWord(string line, string word);
ParseNode execAST(ParseNode& ast, Scope& args);
ParseNode generateAST(string src);
ASTNode* parseObjectLiteral(string expr, int& i);
ASTNode* parseArrayLiteral(string expr, int& i);
ASTNode* parseNode(ASTNode* node, Scope& args);
ParseNode* parseFunction(string expr, int& i);
ASTNode parseParens(string expr, Scope& args);
vector<ASTNode> parseArguments(string argstring, Scope& scope);
extern map<string, vector<ASTNode>> eventListeners;
extern vector<ParseNode> scriptsToRunOnLoad;
extern vector<vector<ASTNode>> eventListenersToBindArgs;
extern vector<Scope> eventListenersToBindScopes;
extern bool pageLoaded;