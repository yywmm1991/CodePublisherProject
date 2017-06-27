#ifndef LINECOUNTER_H
#define LINECOUNTER_H
/////////////////////////////////////////////////////////////////////
// LineCounter.h - Obtain line counts of types                     //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #3         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////
/*
*  Class Operations
* ====================
*  - This package contains one class: LineCounter
*  - LineCounter: 
*  - provide access to obtain start and end line counts to certain types
*  - provide access to line count collections
*
*  Public Interface
* ==================
*  LineCounter lc(Path file, Types types)                    creat LineCounter instance
*  lc.getLineCounts()                                        analysis line count information 
*  lc.getStrtLines()                                         access to start line collection
*  lc.getEndLines()                                          access to end line collection
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - LineCounter.h, LineCounter.cpp, ActionsAndRules.h            //
//    AbstrSynTree.h                                               //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_LINECOUNTER in properties.  //
//    Set LineCounter as the startup project.                      //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv LineCounter.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 11 Apr 17
*  - first release
*
*/

#include "../Parser/ActionsAndRules.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"

#include <set>

namespace CodePublisher {

	using namespace CodeAnalysis;

	///////////////////////////////////////////////////////////////////////////
	// LineCounter:
	// - Traverse the Abstract Syntax Tree
	// - obtain the start and line count collections for specified file and types

	class LineCounter {

	public:

		using Type   = std::string;
		using Types  = std::set<std::string>;
		using Name   = std::string;
		using Path   = std::string;
		using Counts = std::set<int>;

		LineCounter(Path file, Types types);                           // promotion constructor
		~LineCounter() {}                                              // destructor

		void getLineCounts();                                          // traverse the AST to get line counts
		Counts getStrtLines() { return strtLines_; }                   // access to start line collections
		Counts getEndLines() { return endLines_; }                     // access to end line collections

	private:

		void DFSTraverse(ASTNode* pNode);                              // DFS traversal to AST
		AbstrSynTree& ASTref_;                                         // reference to generated Abstract Syntax Tree
		Path file_;                                                    // file to be processed
		Types types_;                                                  // checked types
		Counts strtLines_;                                             // start line collections
		Counts endLines_;                                              // end line collections

	};


}

#endif // !LINECOUNTER_H