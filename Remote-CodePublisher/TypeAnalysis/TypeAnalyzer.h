#ifndef TYPEANALYZER_H
#define TYPEANALYZER_H
/////////////////////////////////////////////////////////////////////
// TypeAnalyzer.h - Conduct Type Analysis                          //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #2         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////
/*
*  Class Operations
* ====================
*  - This package contains one class: TypeAnalyzer
*  - TypeAnalyzer: provide method to traverse AST and update TypeTable contents
*  - Provide method to detect global functions and global data from AST
*  - Provide method to add type definition into TypeTable
*
*  Public Interface
* ==================
*  TypeAnalyzer tb(TypeTable&)                       default constructor for TypeAnalyzer
*  tb.startAnalysis()                                start analyzing
*  tb.showResult()                                   display the result of analysis
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - TypeAnalyzer.h, TypeAnalyzer.cpp, Utilities.h                //
//    AnstrSynTree.h, Logger.h, TypeTable.h                        //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_TYPEANALYSIS in properties. //
//    Set TypeAnalysis as the startup project.                     //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv TypeAnalysis.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 16 Mar 17
*  - first release
*
*/

#include <iostream>
#include <functional>

#include "../Parser/ActionsAndRules.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Logger/Logger.h"
#include "../TypeTable/TypeTable.h"

namespace CodeAnalysis
{
	class TypeAnalyzer
	{
	public:

		enum Type {type, gFunction, gData};
		using SPtr = std::shared_ptr<ASTNode*>;
		using Namespace = std::string;

		TypeAnalyzer(TypeTable& table);                             // promotion constructor
		TypeAnalyzer(AbstrSynTree ASTref, ScopeStack<ASTNode*> scopeStack,
			Scanner::Toker* toker, TypeTable& table);               // promotion constructor for test only
		void startAnalysis();                                       // start analysis on AST
		std::string showResult();                                   // display the result of analysis

	private:

		void DFSTraverse(ASTNode* pNode, const Namespace& nspace);  // traverse the AST and store entries into type table
		bool isAliases(ASTNode* pNode);                             // if ASTNode is a type definition
		void addAliases(ASTNode* pNode, const Namespace& nspace);   // add typedef and aliases into table_
		void addGlobal(ASTNode* pNode, const Namespace& nspace);    // add global function or data into table_
		bool isType(ASTNode* pNode);                                // if ASTNode is type definition
		void addType(ASTNode* pNode, const Namespace& nspace);      // add type definition into table_
		AbstrSynTree& ASTref_;                                      // reference to generated Abstract Syntax Tree
		ScopeStack<ASTNode*> scopeStack_;                           // scopeStack
		Scanner::Toker& toker_;                                     // reference to toker_
		TypeTable& table_;                                          // type table
	};
}
#endif // !TYPEANALYZER_H

