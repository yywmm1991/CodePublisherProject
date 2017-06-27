#ifndef EXECUTIVE_H
#define EXECUTIVE_H
/////////////////////////////////////////////////////////////////////
// Executive.h - Organize the manage code analysis                 //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #3         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
// Reference:   Jim Fawcett, CST 4-187, Syracuse University        //
//              jfawcett@twcny.rr.com, (315) 443-3948              //
/////////////////////////////////////////////////////////////////////
/*
*  Module Operations
* ====================
*  Executive uses Parser, ScopeStack, and AbstrSynTree packages to
*  provide static code analysis for C++ files contained in a
*  specified directory tree.  
*
*  It finds files for analysis using FileMgr and, for each file found,
*  uses parser rules to detect:
*  - namespace scopes
*  - class scopes
*  - struct scopes
*  - control scopes
*  - function definition scopes
*  - data declarations
*  and uses rule actions to build an Abstract Sytax Tree (AST) 
*  representing the entire code set's static structure.  Each detected
*  scope becomes a node in the AST which spans all of the files analyzed, 
*  rooted in a top-level Global Namespace scope.
*
*  Because much of the important static structure information is contained
*  in the AST, it is relatively easy to extend the application to evaluate
*  additional information, such as class relationships, dependency network,
*  and static design flaws.
*
*  Public Interface
* ==================
*  CodePublisherExecutive exec()                       create a CodePublisherExecutive instance
*  exec.ProcessCommandLine(argc, argv)                 process command line arguments
*  exec.getSourceFiles()                               obtain all C++ source files
*  exec.processSourceCode(true)                        process and parse all source files
*  exec.complexityAnalysis()                           perform complexity analysis
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - Executive.h, Executive.cpp                                   //
//  - Parser.h, Parser.cpp, ActionsAndRules.h, ActionsAndRules.cpp //
//  - ConfigureParser.h, ConfigureParser.cpp, ScopeStack.h         //
//  - ScopeStack.cpp, AbstrSynTree.h, AbstrSynTree.cpp             //
//  - ITokenCollection.h, SemiExp.h, SemiExp.cpp                   //      
//  - IFileMgr.h, FileMgr.h, FileMgr.cpp                           //
//  - Logger.h, Logger.cpp, Utilities.h, Utilities.cpp             //
//  - Tokenizer.h, Tokenizer.cpp, FileSystem.h, FileSystem.cpp     //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_EXECUTIVE in properties.    //
//    Set Executive as the startup project.                        //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv Executive.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 14 Apr 17
*  - first release
*
*/

#include <string>
#include <vector>
#include <unordered_map>
#include <iosfwd>
#include <set>

#include "../Parser/Parser.h"
#include "../FileMgr/FileMgr.h"
#include "../Parser/ConfigureParser.h"
#include "../Utilities/Utilities.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"

namespace CodePublisher
{
	using Utils = Utilities::StringHelper;
	using namespace CodeAnalysis;

	///////////////////////////////////////////////////////////////////
	// AnalFileMgr class derives from FileManager::FileMgr
	// - provides application specific handling of file and dir events

	class AnalFileMgr : public FileManager::FileMgr
	{

	public:

		using Path = std::string;
		using File = std::string;
		using Files = std::vector<File>;
		using Pattern = std::string;
		using Ext = std::string;
		using FileMap = std::unordered_map<Pattern, Files>;

		AnalFileMgr(const Path& path, FileMap& fileMap);
		virtual void file(const File& f);
		virtual void dir(const Dir& d);
		virtual void done();
		size_t numFiles();
		size_t numDirs();

	private:

		Path d_;
		FileMap& fileMap_;
		bool display_;
		size_t numFiles_;
		size_t numDirs_;
	};

	///////////////////////////////////////////////////////////////////
	// CodeAnalysisExecutive class directs C++ and C# code analysis

	class CodePublisherExecutive
	{

	public:

		using Path = std::string;
		using Pattern = std::string;
		using Patterns = std::vector<Pattern>;
		using File = std::string;
		using Files = std::vector<File>;
		using Ext = std::string;
		using Options = std::vector<char>;
		using FileMap = std::unordered_map<Pattern, Files>;
		using FileNodes = std::vector<std::pair<File, ASTNode*>>;
		using Slocs = size_t;
		using SlocMap = std::unordered_map<File, Slocs>;
		using HtmlFiles = std::vector<std::string>;
		using Map = std::unordered_map<std::string, std::set<std::string>>;

		CodePublisherExecutive();
		virtual ~CodePublisherExecutive();

		CodePublisherExecutive(const CodePublisherExecutive&) = delete;
		CodePublisherExecutive& operator=(const CodePublisherExecutive&) = delete;

		bool ProcessCommandLine(int argc, char* argv[]);
		FileMap& getFileMap();
		std::string getAnalysisPath();
		virtual void getSourceFiles();
		virtual void processSourceCode(bool showActivity);
		void complexityAnalysis();
		std::vector<File>& cppHeaderFiles();
		std::vector<File>& cppImplemFiles();
		std::vector<File>& csharpFiles();
		Slocs fileSLOCs(const File& file);
		size_t numFiles();
		size_t numDirs();
		std::string systemTime();
		void setLogFile(const File& file);

	private:

		void setLanguage(const File& file);
		std::string showData(const Scanner::ITokCollection* ptc);
		Parser* pParser_;
		ConfigParseForCodeAnal configure_;
		Repository* pRepo_;
		Path path_;
		Patterns patterns_;
		Options options_;
		FileMap fileMap_;
		FileNodes fileNodes_;
		std::vector<File> cppHeaderFiles_;
		std::vector<File> cppImplemFiles_;
		std::vector<File> csharpFiles_;
		size_t numFiles_;
		size_t numDirs_;
		SlocMap slocMap_;
		bool displayProc_ = false;
		std::ofstream* pLogStrm_ = nullptr;
	};
}
#endif // !EXECUTIVE_H



