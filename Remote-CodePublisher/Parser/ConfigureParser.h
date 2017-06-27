#ifndef CONFIGUREPARSER_H
#define CONFIGUREPARSER_H
/////////////////////////////////////////////////////////////////////
// ConfigureParser.h - builds and configures parsers               //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #2         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 8.1, Visual Studio 2015                        //
// Author:      WENYU ZHANG, 233508014                             //
//              wzhan102@syr.edu                                   //
// Reference:   Jim Fawcett, CST 4-187, Syracuse University        //
//              jfawcett@twcny.rr.com, (315) 443-3948              //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module builds and configures parsers.  It builds the parser
  parts and configures them with application specific rules and actions.

  - Modification: 
    Add new actions and rules into ConfigureParser. New actions and rules are 
    defined in NewActionsAndRules.h.
    New added actions and rules pointers are new and delete appropriately.

  Public Interface:
  =================
  ConfigParseForCodeAnal config;
  config.Build();
  config.Attach(someFileName);

  Build Process:
  ==============
  Required files
    - ConfigureParser.h, ConfigureParser.cpp, Parser.h, Parser.cpp,
      ActionsAndRules.h, ActionsAndRules.cpp,
	  NewActionsAndRules.h, NewActionsAndRules.cpp,
      SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv Project1HelpS06.sln
    - cl /EHsc /DTEST_PARSER ConfigureParser.cpp parser.cpp \
         ActionsAndRules.cpp \
         semiexpression.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.0 : 16 Mar 17
   - first release
*/

#include <fstream>
#include "Parser.h"
#include "ActionsAndRules.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "../NewActionsAndRules/NewActionsAndRules.h"

namespace CodeAnalysis
{
  ///////////////////////////////////////////////////////////////
  // build parser that writes its output to console

  class ConfigParseForCodeAnal : IBuilder
  {
  public:
    ConfigParseForCodeAnal() : pIn(nullptr) {};
    ~ConfigParseForCodeAnal();
    bool Attach(const std::string& name, bool isFile = true);
    Parser* Build();

  private:
    // Builder must hold onto all the pieces

    std::ifstream* pIn;
    Scanner::Toker* pToker;
    Scanner::SemiExp* pSemi;
    Parser* pParser;
    Repository* pRepo;

	void handleScope();
	void handleDefinition();

    // add Rules and Actions

    BeginScope* pBeginScope = nullptr;
    HandleBeginScope* pHandleBeginScope = nullptr;

    EndScope* pEndScope = nullptr;
    HandleEndScope* pHandleEndScope = nullptr;

    PreprocStatement* pPreprocStatement = nullptr;
    HandlePreprocStatement* pHandlePreprocStatement = nullptr;

    NamespaceDefinition* pNamespaceDefinition = nullptr;
    HandleNamespaceDefinition* pHandleNamespaceDefinition = nullptr;

    ClassDefinition* pClassDefinition = nullptr;
    HandleClassDefinition* pHandleClassDefinition = nullptr;

    StructDefinition* pStructDefinition = nullptr;
    HandleStructDefinition* pHandleStructDefinition = nullptr;

    CppFunctionDefinition* pCppFunctionDefinition = nullptr;
    HandleCppFunctionDefinition* pHandleCppFunctionDefinition = nullptr;
    //PrintFunction* pPrintFunction;

    CSharpFunctionDefinition* pCSharpFunctionDefinition = nullptr;
    HandleCSharpFunctionDefinition* pHandleCSharpFunctionDefinition = nullptr;

    ControlDefinition* pControlDefinition = nullptr;
    HandleControlDefinition* pHandleControlDefinition = nullptr;

    CppDeclaration* pCppDeclaration = nullptr;
    HandleCppDeclaration* pHandleCppDeclaration = nullptr;

    CSharpDeclaration* pCSharpDeclaration = nullptr;
    HandleCSharpDeclaration* pHandleCSharpDeclaration = nullptr;

    CppExecutable* pCppExecutable = nullptr;
    HandleCppExecutable* pHandleCppExecutable = nullptr;

    CSharpExecutable* pCSharpExecutable = nullptr;
    HandleCSharpExecutable* pHandleCSharpExecutable = nullptr;

    Default* pDefault = nullptr;
    HandleDefault* pHandleDefault = nullptr;

    // prohibit copies and assignments

    ConfigParseForCodeAnal(const ConfigParseForCodeAnal&) = delete;
    ConfigParseForCodeAnal& operator=(const ConfigParseForCodeAnal&) = delete;


	// Add rule and action to detect enum
	EnumDefinition* pEnumDefinition = nullptr;
	HandleEnumDefinition* pHandleEnumDefinition = nullptr;

	// Add rule and action to detect typedef
	DetectTypeDef* pTypeDef = nullptr;
	HandleTypeDef* pHandleTypeDef = nullptr;

	// Add rule and action to detect aliases
	DetectAliases* pAlias = nullptr;
	HandleAliases* pHandleAlias = nullptr;
  };
}
#endif
