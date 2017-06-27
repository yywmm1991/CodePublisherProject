/////////////////////////////////////////////////////////////////////
// LineCounter.h - Obtain line counts of types                     //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #3         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////

#include "LineCounter.h"

using namespace CodePublisher;

//----< promotion constructor for LineCounter >---------------------------------------

LineCounter::LineCounter(Path file, Types types)
	:ASTref_(Repository::getInstance()->AST()),
	 file_(file),
	 types_(types)
{
	std::cout << "Testing File: " << file << std::endl;
}

//----< get line counts  >------------------------------------------------------------

void LineCounter::getLineCounts() 
{
	ASTNode* pRoot = ASTref_.root();
	DFSTraverse(pRoot);
}

//----< DFS traversal to AST  >-------------------------------------------------------

void LineCounter::DFSTraverse(ASTNode* pNode)
{
	if (pNode->path_ == file_ && types_.find(pNode->type_) != types_.end()) 
	{
		if (pNode->startLineCount_ != pNode->endLineCount_)
		{
			strtLines_.insert(pNode->startLineCount_);
			endLines_.insert(pNode->endLineCount_);
		}
	}
	for (auto pChild : pNode->children_) 
	{
		DFSTraverse(pChild);
	}
}

//----< test stub >-------------------------------------------------------------------

#ifdef TEST_LINECOUNTER

#include "../Display/Display.h"
#include <iostream>

int main() {
	makeTitle("Test LineCounter:");
	std::set<std::string> types{ "function", "class", "struct" };
	LineCounter lc("HtmlFileMgr.h", types);
	lc.getLineCounts();
	std::vector<int> start = lc.getStrtLines();
	for (auto count : start) 
	{
		std::cout << count << std::endl;
	}

}
#endif // !TEST_LINECOUNTER
