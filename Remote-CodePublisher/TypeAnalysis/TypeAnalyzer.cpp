/////////////////////////////////////////////////////////////////////
// TypeAnalyzer.h - Conduct Type Analysis                          //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #2         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////

#include "TypeAnalyzer.h"

using namespace CodeAnalysis;

//----< default constructor for TypeAnalyzer >----------------------------------------

TypeAnalyzer::TypeAnalyzer(TypeTable& table)
	:ASTref_(Repository::getInstance()->AST()),
	scopeStack_(Repository::getInstance()->scopeStack()),
	toker_(*(Repository::getInstance()->Toker())),
	table_(table)
	//gbTable_(gbTable)
{}

//----< promotion constructor for test only >-----------------------------------------

TypeAnalyzer::TypeAnalyzer(AbstrSynTree ASTref, ScopeStack<ASTNode*> scopeStack,
	Scanner::Toker* toker, TypeTable& table)
	:ASTref_(ASTref),
	 scopeStack_(scopeStack),
	 toker_(*toker),
	 table_(table)
{}

//----< Start type analysis on AST >--------------------------------------------------

void TypeAnalyzer::startAnalysis() {
	//std::cout << "\n  starting type analysis:\n";
	//std::cout << "\n  Analyzing Files:";
	//std::cout << "\n ------------------";
	ASTNode* pRoot = ASTref_.root();
	DFSTraverse(pRoot, pRoot->name_);
}

//----< traverse the AST and store entries into type table >--------------------------

void TypeAnalyzer::DFSTraverse(ASTNode* pNode, const Namespace& nspace) {
	static std::string path = "";
	if (pNode->path_ != path)
	{
		//std::cout << "\n    -- " << pNode->path_ << "\\" << pNode->package_;
		path = pNode->path_;
	}
	std::string newNspace = nspace;
	if (nspace == "Global Namespace")
		newNspace = "";
	if (pNode->type_ == "namespace")
		addGlobal(pNode, newNspace);
	// check typedef and aliases
	if (isAliases(pNode))
		addAliases(pNode, nspace);
	// check type definitions
	else if (isType(pNode)) {
		addType(pNode, newNspace);
	}
	for (auto pChild : pNode->children_) {
		if (newNspace == "")
			DFSTraverse(pChild, pNode->name_);
		else
			DFSTraverse(pChild, newNspace + "::" + pNode->name_);
	}
}

//----< check if current node is typedef or aliase definition >-----------------------

bool TypeAnalyzer::isAliases(ASTNode* pNode) {
	std::string checkList[] = { "alias", "typedef" };
	for (std::string type : checkList) {
		if (pNode->type_ == type)
			return true;
	}
	return false;
}

//----< add typedef or aliases into table_ >------------------------------------------

void TypeAnalyzer::addAliases(ASTNode* pNode, const Namespace& nspace) {
	Content newContent;
	newContent.setType(pNode->type_);
	// split the targetName and original name
	size_t sepaIndex = pNode->name_.find('=');
	newContent.setFullName("Global Namespace::" + nspace + "::" + pNode->name_.substr(0, sepaIndex));
	newContent.setPath(pNode->path_);
	newContent.setNamespace("Global Namespace::" + nspace);
	table_.addType(pNode->name_.substr(0, sepaIndex), newContent);
}

//----< check if current node is type definition >------------------------------------------

bool TypeAnalyzer::isType(ASTNode* pNode) {
	std::string checkList[] = {
		"class", "struct", "enum"
	};
	for (std::string type : checkList)
	{
		if (pNode->type_ == type)
			return true;
	}
	return false;
}

//----< add type definitions into table_ >------------------------------------------

void TypeAnalyzer::addType(ASTNode* pNode, const Namespace& nspace) {
	Content newContent;
	newContent.setType(pNode->type_);
	if (nspace == "")
		newContent.setFullName(pNode->name_);
	else
		newContent.setFullName("Global Namespace::" + nspace + "::" + pNode->name_);
	newContent.setPath(pNode->path_);
	newContent.setNamespace("Global Namespace::" + nspace);
	table_.addType(pNode->name_, newContent);
}

//----< add global function or data into table >--------------------------------------

void TypeAnalyzer::addGlobal(ASTNode* pNode, const Namespace& nspace) {
	if (pNode->type_ != "namespace")
		return;
	// add global functions
	std::string newNspace = nspace;
	if (nspace == "")
		newNspace = "Global Namespace";
	else
		newNspace  = "Global Namespace::" + nspace;
	ASTNode* firstChild = *(pNode->children_.begin());
	std::string path = firstChild->path_;
	for (auto cNode : pNode->children_) {
		path = cNode->path_;
		if (cNode->type_ == "function" && cNode->name_ != "main") {
			Content newContent;
			newContent.setType("global function");
			newContent.setFullName(newNspace + "::" + cNode->name_);
			newContent.setPath(cNode->path_);
			newContent.setNamespace(newNspace);
			table_.addType(cNode->name_, newContent);
		}
	}
	// add global data
	for (auto dNode : pNode->decl_) {
		if (dNode.declType_ == dataDecl) {
			Content newContent;
			newContent.setType("global data");
			size_t nameIndex = dNode.pTc->find("=") - 1;
			std::string name = (*dNode.pTc)[nameIndex];
			newContent.setFullName(newNspace + "::" + name);
			newContent.setPath(path);
			newContent.setNamespace(newNspace);
			table_.addType(name, newContent);
		}
	}
}

//----< display analysis result in string >-------------------------------------------

std::string TypeAnalyzer::showResult() {
	std::ostringstream out;
	out << table_.showTable();
	out << std::endl;
	return out.str();
}

//----< test stub >-------------------------------------------------------------------

#ifdef TEST_TYPEANALYSIS

//----< generate ASTree for test >----------------------------------------------------

AbstrSynTree generateASTree(ScopeStack<ASTNode*>& stack) {
	AbstrSynTree ast(stack);
	ASTNode* pX = new ASTNode("class", "X");
	ast.add(pX);                                        // add X scope
	ASTNode* pf1 = new ASTNode("function", "f1");
	ast.add(pf1);                                       // add f1 scope
	ASTNode* pc1 = new ASTNode("control", "if");
	ast.add(pc1);                                       // add c1 scope
	ast.pop();                                          // end c1 scope
	ast.pop();                                          // end f1 scope
	ASTNode* pf2 = new ASTNode("function", "f2");
	ast.add(pf2);                                       // add f2 scope
	ast.pop();                                          // end f2 scope
	ast.pop();                                          // end X scope
	return ast;
}

int main() {
	using Utils = Utilities::StringHelper;
	Utils::Title("Testing Type Analysis");

	ScopeStack<ASTNode*> stack;
	Scanner::Toker* toker = nullptr;
	TypeTable table;
	AbstrSynTree& ast = generateASTree(stack);
	TypeAnalyzer ta(ast, stack, toker, table);

	// start analysis
	ta.startAnalysis();
	std::cout << table.showTable();
}
#endif // !TEST_TYPEANALYSIS