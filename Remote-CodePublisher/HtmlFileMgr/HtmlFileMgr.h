#ifndef HTMLFILEMGR_H
#define HTMLFILEMGR_H
/////////////////////////////////////////////////////////////////////
// HtmlFileMgr.h - Organize generating of html files               //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #3         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 8.1, Visual Studio 2015                        //
// Author:      WENYU ZHANG, 233508014                             //
//              wzhan102@syr.edu                                   //
/////////////////////////////////////////////////////////////////////
/*
*  Class Operations
* ====================
*  - This package contains one class: HtmlFileMgr
*  - HtmlFileMgr: provide method to organize and manage generation of html files
*  - Accepts files to be processed
*  - Accepts path to store generated files
*  - Write html files to corresponding path
*
*  Public Interface
* ==================
*  HtmlFileMgr mgr(Files hfiles, Files cppFiles, 
*                  unordered_map map, std::string)                create a HtmlFileMgr instance
*  mgr.write()                                                    write generated html files
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - HtmlFileMgr.h, HtmlFileMgr.cpp, HtmlGenerator.h              //
//    FileSystem.h                                                 //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_HTMLFILEMGR in properties.  //
//    Set HtmlFileMgr as the startup project.                      //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv HtmlFileMgr.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 03 Apr 2017
*  - first release
*  ver 1.1 : 13 Apr 2017
*  - make hide and unhide only effective for classes, methods and functions
*/

#include "../FileSystem/FileSystem.h"
#include "../HtmlGenerator/HtmlGenerator.h"
#include "../LineCounter/LineCounter.h"

#include <unordered_map>
#include <set>

namespace CodePublisher {

	///////////////////////////////////////////////////////////////////////////
	// HtmlFileMgr:
	// - Organize generating of html files
	// - write html files to predefined path

	class HtmlFileMgr {

	public:

		using File      = std::string;
		using Files     = std::vector<File>;
		using Path      = std::string;
		using Html      = std::string;
		using Code      = std::string;
		using Types     = std::set<std::string>;
		using Extention = std::string;
		using Map       = std::unordered_map<std::string, std::set<std::string>>;

		HtmlFileMgr(const Files, const Files, Map, Path);          // promotion constructor
		~HtmlFileMgr();                                            // destructor
		void writeFiles();                                         // write html files

	private:

		File processFileName(File file);                           // process full file name
		Html getHtml(File file);                                   // generate html using HtmlGenerator
		Code processCode(Code code);                               // process source code
		Code addLeftHide(Code code);                               // add html script for left brace
		Code addRightHide(Code code);                              // add html script for right brace
		Files hFiles_;
		Files impleFiles_;
		Map dep_;
		HtmlGenerator* pHtml;
		LineCounter* pLC;
		Path dir_;
		Types types_ = Types{ "class", "struct", "function" };
		Extention ext_ = ".htm";
		int *pCount;
	};

	
}

#endif // !HTMLFILEMGR_H