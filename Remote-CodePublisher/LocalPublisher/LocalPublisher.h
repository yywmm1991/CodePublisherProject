#ifndef LOCALPUBLISHER_H
#define LOCALPUBLISHER_H
/////////////////////////////////////////////////////////////////////
// LocalPublisher.h - Organize and Executive for publishing html   //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #4         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////
/*
*  Module Operations
* ====================
*  - LocalPublisher inherits from Executive class, extends
*    the base class functionalities to direct and organize
*    Type Analysis, Dependency Analysis and generating of html files
*
*  Public Interface
* ==================
*  LocalPublisher publisher(TypeTable&)                     create a LocalPublisher instance
*  publisher.publish()                                      publish html files
*  publisher.getDependencies()                              return dependencies on analyzed directory
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - LocalPublisher.h, LocalPublisher.cpp, Executive.h            //
//    TypeAnalyzer.h, DependencyAnalyzer.h, TypeTable.h            //
//    HtmlFileMgr.h, HtmlGenerator.h                               //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_EXECUTIVE in properties.    //
//    Set LocalPublisher as the startup project.                    //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv LocalPublisher.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 30 Apr 17
*  - first release
*/

#include "../Executive/Executive.h"
#include "../TypeTable/TypeTable.h"
#include "../TypeAnalysis/TypeAnalyzer.h"
#include "../DependencyAnalysis/DependencyAnalyzer.h"
#include "../HtmlFileMgr/HtmlFileMgr.h"
#include "../HtmlGenerator/HtmlGenerator.h"
#include "../LineCounter/LineCounter.h"

namespace CodePublisher {

	using namespace CodeAnalysis;

	///////////////////////////////////////////////////////////////////////////
	// LocalPublisher:
	// - Provide executive code for type analysis and dependency analysis
	// - Provide functionality to publish html files

	class LocalPublisher : public CodePublisherExecutive {
	public:

		using Path = std::string;
		using HtmlFiles = std::vector<std::string>;
		using Map = std::unordered_map<std::string, std::set<std::string>>;

		LocalPublisher(TypeTable& table) : table_(table) {}     // promotion constructor
		~LocalPublisher();                                      // destructor
		
		void publish();                              // publish html files to server
		Map getDependencies();                       // get dependency relationships as map

	private:

		void typeAnalysis();                         // conduct type analysis
		void depAnalysis();                          // conduct dependency analysis
		void getHtmlFiles();                         // access to html files for displaying in browser
		std::string processFileName(Path file);      // process the file name
		void lanuchPage(File file);                  // lanuch html pages in browser
		void generateMainPage();                     // generate main page
		TypeTable& table_;
		DepAnalyzer* depAnal;
		Map *pDep;
		HtmlFiles *pHtmlFiles;
		Path displayFile_;
		Path outPath_ = "../Repository/HtmlFiles";
		Path cssPath_ = "../Repository/WebFiles/style_sheet.css";
		Path jsPath_ = "../Repository/WebFiles/script.js";

	};
}
#endif // !LOCALPUBLISHER_H



