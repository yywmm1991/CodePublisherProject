#ifndef NEWACTIONSANDRULES_H
#define NEWACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
// NewActionsAndRules.h - extends actions and rules                //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #2         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////
/*
*  Module Operations
* ====================
*  This module defines several rule and action classes which derived
*  from IRule and IAction. These actions and rules are used to extend
*  the parsing ability to detect enum, typedefs and aliases
*
*  Public Interface
* ==================
* Rule1 r1;                       // create instance of a derived Rule class
* Action1 a1;                     // create a derived action
* r1.addAction(&a1);              // register action with the rule
* parser.addRule(&r1);            // register rule with parser
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required files                                                 //
//  - Parser.h, Parser.cpp, ScopeStack.h, ScopeStack.cpp,          //
//    NewActionsAndRules.h, NewActionsAndRules.cpp,                //
//    ConfigureParser.cpp, ItokCollection.h, SemiExpression.h,     //
//    SemiExpression.cpp, tokenizer.h, tokenizer.cpp               //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_NEWACTIONSANDRULES          //
//    in properties.                                               //
//    Set NewActionsAndRules as the startup project.               //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv NewActionsAndRules.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 16 Mar 17
*  - first release
*
*/

#include "../Parser/ActionsAndRules.h"

namespace CodeAnalysis {

	///////////////////////////////////////////////////////////////
	// EnumDefinition: 
	// - Provide rule to detect enum declaration

	class EnumDefinition : public IRule
	{
	public:
		bool doTest(const Scanner::ITokCollection* pTc) override
		{
			GrammarHelper::showParseDemo("Test enum definition: ", *pTc);

			const Scanner::ITokCollection& tc = *pTc;
			if (tc[tc.length() - 1] == "{")
			{
				size_t len = tc.find("enum");
				if (len < tc.length())
				{
					doActions(pTc);
					return IRule::Stop;
				}
			}
			return IRule::Continue;
		}
	};

	///////////////////////////////////////////////////////////////
	// HandleEnumDefinition:
	// - Provide action to add enum info to scope stack top

	class HandleEnumDefinition : public IAction
	{
		Repository* p_Repos;

	public:
		HandleEnumDefinition(Repository* pRepos)
		{
			p_Repos = pRepos;
		}

		void doAction(const Scanner::ITokCollection* pTc) override
		{
			GrammarHelper::showParseDemo("Handle enum definition: ", *pTc);

			//p_Repos->currentAccess() = Access::priv;
			ASTNode* top = p_Repos->scopeStack().top();
			size_t typeIndex = pTc->find("enum");
			if (typeIndex < pTc->length())
			{
				size_t nameIndex = typeIndex + 1;
				std::string name = (*pTc)[nameIndex];
				top->type_ = "enum";
				top->name_ = name;
				top->package_ = p_Repos->package();
				p_Repos->AST().typeMap()[name] = top;
				GrammarHelper::showParse("enum def", *pTc);
			}
		}
	};

	///////////////////////////////////////////////////////////////
	// DetectTypeDef: 
	// - Provide rule to detect typedefs

	class DetectTypeDef : public IRule
	{
	public:
		bool doTest(const Scanner::ITokCollection* pTc) override
		{
			GrammarHelper::showParseDemo("Detect Typedef: ", *pTc);

			const Scanner::ITokCollection& tc = *pTc;
			//if (tc[tc.length() - 1] == ";")
			//{
				size_t len = tc.find("typedef");
				if (len < tc.length())
				{
					doActions(pTc);
					return IRule::Stop;
				}
			//}
			return IRule::Continue;
		}
	};

	///////////////////////////////////////////////////////////////
	// HandleTypeDef:
	// - Provide action to add typedef info to scope stack top

	class HandleTypeDef : public IAction
	{
		Repository* p_Repos;

	public:
		HandleTypeDef(Repository* pRepos)
		{
			p_Repos = pRepos;
		}

		void doAction(const Scanner::ITokCollection* pTc) override
		{
			GrammarHelper::showParseDemo("Handle typedefs: ", *pTc);

			ASTNode* top = p_Repos->scopeStack().top();
			size_t typeIndex = pTc->find("typedef");
			if (typeIndex < pTc->length())
			{
				size_t nameIndex = pTc->find(";") - 1;
				std::string tdName = (*pTc)[nameIndex];
				std::string fullName = "";
				for (size_t i = typeIndex + 1; i < nameIndex; i++) {
					fullName += (*pTc)[i];
					if (i + 1 < nameIndex)
						fullName += " ";
				}
				ASTNode* tdNode = new ASTNode;
				tdNode->type_ = "typedef";
				tdNode->name_ = tdName + "=" + fullName;
				tdNode->parentType_ = top->type_;
				tdNode->package_ = p_Repos->package();
				tdNode->startLineCount_ = p_Repos->lineCount();
				tdNode->endLineCount_ = p_Repos->lineCount();
				tdNode->path_ = p_Repos->currentPath();
				top->children_.push_back(tdNode);
				p_Repos->AST().typeMap()[tdName] = tdNode;
				GrammarHelper::showParse("typedef", *pTc);
			}
		}
	};

	///////////////////////////////////////////////////////////////
	// DetectAliases: 
	// - Provide rule to detect aliases

	class DetectAliases : public IRule
	{
	public:
		bool doTest(const Scanner::ITokCollection* pTc) override
		{
			GrammarHelper::showParseDemo("Detect Typedef: ", *pTc);

			const Scanner::ITokCollection& tc = *pTc;
			size_t len = tc.find("using");
			size_t eqLen = tc.find("=");
			if (len < tc.length() && eqLen < tc.length())
			{
				doActions(pTc);
				return IRule::Stop;
			}
			return IRule::Continue;
		}
	};

	///////////////////////////////////////////////////////////////
	// HandleAliases:
	// - Provide action to add aliases info to scope stack top

	class HandleAliases : public IAction
	{
		Repository* p_Repos;

	public:
		HandleAliases(Repository* pRepos)
		{
			p_Repos = pRepos;
		}

		void doAction(const Scanner::ITokCollection* pTc) override
		{
			GrammarHelper::showParseDemo("Handle typedefs: ", *pTc);

			ASTNode* top = p_Repos->scopeStack().top();
			size_t typeIndex = pTc->find("using");
			if (typeIndex < pTc->length())
			{
				size_t equalIndex = pTc->find("=");
				size_t endIndex = pTc->find(";");
				std::string aliasName = (*pTc)[equalIndex-1];
				std::string fullName = "";
				for (size_t i = equalIndex + 1; i < endIndex; i++) {
					fullName += (*pTc)[i];
				}
				ASTNode* usingNode = new ASTNode;
				usingNode->type_ = "alias";
				usingNode->name_ = aliasName + "=" + fullName;
				usingNode->parentType_ = top->type_;
				usingNode->package_ = p_Repos->package();
				usingNode->startLineCount_ = p_Repos->lineCount();
				usingNode->endLineCount_ = p_Repos->lineCount();
				usingNode->path_ = p_Repos->currentPath();
				top->children_.push_back(usingNode);
				p_Repos->AST().typeMap()[aliasName] = usingNode;
				GrammarHelper::showParse("aliase", *pTc);
			}
		}
	};

	
}
#endif // !TYPEANALYZER_H
