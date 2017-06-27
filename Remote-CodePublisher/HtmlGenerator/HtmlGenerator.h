#ifndef HTMLGENERATOR_H
#define HTMLGENERATOR_H
/////////////////////////////////////////////////////////////////////
// HtmlGenerator.h - Generate Html code on C++ source codes        //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #3         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 8.1, Visual Studio 2015                        //
// Author:      WENYU ZHANG, 233508014                             //
//              wzhan102@syr.edu                                   //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations
* ====================
*  - This package contains three class: HtmlTag, HtmlGenerator and MainPageGenerator
*  - HtmlTag: create a data structure for html tags
*  - provide methods to generate start tag and end tag on input information
*  - HtmlGenerator: generate html source code on C++ source code
*  - provide method to generate html code
*  - provide access on generated html code
*  - MainPageGenerator: generate html code for main page
*  - provide method to generate html code
*  - provide access on generated html code
*
*  Public Interface
* ==================
*  HtmlTag tag(string tagname, string attr, size_t indent)              create a HtmlTag instance
*  tag.getName()                                                        access to tag name
*  tag.getAttribute()                                                   access to attribute
*  tag.generateStartTag()                                               generate start tag in string
*  tag.generateEndTag()                                                 generate end tag in string
*
*  HtmlGenerator hg(File file, Code cppCode, Dependency deps)           create a HtmlGenerator instance
*  hg.generateHtml()                                                    begin the process to generate html code
*  hg.getHtml()                                                         access generated html in string
*
*  MainPageGenerator mpg(File file, Dependency deps)                    create a MainPageGenerator instance
*  mpg.generateHtml()                                                   begin the process to generate html code
*  mpg.getHtml()                                                        access generated html in string
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - HtmlGenerator.h, HtmlGenerator.cpp                           //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_HTMLGENERATOR in properties.//
//    Set HtmlGenerator as the startup project.                    //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv HtmlGenerator.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 03 Mar 17
*  - first release
*
*/

#include <string>
#include <stack>
#include <set>
#include <vector>

namespace CodePublisher {
	
	///////////////////////////////////////////////////////////////////////////
	// HtmlTag:
	// - define data structure for Html tags
	// - provide method to generate start tag and end tag on input information

	class HtmlTag {

	public:

		using TagName = std::string;
		using Attribute = std::string;
		using Indent = std::string;

		HtmlTag(TagName tagname, Attribute attribute="", size_t indent = 0);  // promotion constructor
		~HtmlTag() {}                                                         // destructor

		TagName getName() { return tagname_; }                                // provide access to tagname_
		Attribute getAttribute() { return attribute_; }                       // provide access to attribute_
		std::string generateStartTag();                                       // generate string-type start tag
		std::string generateEndTag();                                         // generate string-type end tag

	private:

		TagName tagname_;
		Attribute attribute_;
		Indent indent_;
	};

	///////////////////////////////////////////////////////////////////////////
	// HtmlGenerator:
	// - Generate Html code regarding C++ source code

	class HtmlGenerator {

	public:

		using File = std::string;
		using Dependency = std::set<std::string>;
		using Code = std::string;
		using Html = std::string;
		using Stack = std::stack<HtmlTag>;

		HtmlGenerator(File file, Code cppCode, Dependency deps);   // promotion constructor
		~HtmlGenerator() {}                                        // destructor

		void generateHtml();                                       // main method to generate html 
		Html getHtml() { return html_; }                           // return generated html content in string

	private:
		 
		Code processCode(Code code);                               // process code and replace certain characters
		void getDate();                                            // get the processed date of current file
		void addPrologue();                                        // add prologue into html code
		void addHtml(Html attr = "");                              // add <html> tag
		void addHead(Html attr = "");                              // add <head> tag
		void addCSS();                                             // add link to CSS sheet
		void addJS();                                              // add link to Javascript
		void addBody(Html attr = "");                              // add <body> tag
		void addHeading(File, Html, size_t, Html attr = "");       // add file name as heading
		void addHRule(size_t indent = 0);                          // add horizonal rule
		void addDependency(size_t indent = 0);                     // add dependency block
		void addLink(File dep, size_t indent);                     // add dependency link
		Html processFileName(File file);                           // process file name to certain pattern
		void addCode(size_t indent);                               // add source file content
		void pushHtmlTag(HtmlTag tag);                             // push HtmlTag into stack
		void popHtmlTag();                                         // pop HtmlTag out of stack
		void addFunction();                                        // add javascript function
		File file_;
		File css_ = "./WebFiles/style_sheet.css";
		File js_ = "./WebFiles/script.js";
		Dependency deps_;
		Code code_;                                       
		Html html_;
		Html author_ = "WENYU ZHANG";
		Html subject_ = "CSE687 - Object Oriented Design, Spring 2017";
		Html description_ = "Source Code Page for Project #3";
		Html date_;
		Stack stack_;
	};

	///////////////////////////////////////////////////////////////////////////
	// MainPageGenerator:
	// - Generate Html code for main page

	class MainPageGenerator {

	public:

		using File = std::string;
		using Dependency = std::vector<std::string>;
		using Html = std::string;
		using Stack = std::stack<HtmlTag>;

		MainPageGenerator(File file, Dependency deps);             // promotion constructor
		~MainPageGenerator() {}                                    // destructor

		void generateHtml();                                       // main method to generate html 
		Html getHtml() { return html_; }                           // return generated html content in string

	private:

		void addPrologue();                                        // add prologue into html code
		void getDate();                                            // get the processed date of current file
		void addHtml(Html attr = "");                              // add <html> tag
		void addHead(Html attr = "");                              // add <head> tag
		void pushHtmlTag(HtmlTag tag);                             // push HtmlTag into stack
		void popHtmlTag();                                         // pop HtmlTag out of stack
		void addCSS();                                             // add link to CSS sheet
		void addBody(Html attr = "");                              // add <body> tag
		void addHeading(File, Html, size_t, Html attr = "");       // add file name as heading
		void addHRule(size_t indent = 0);                          // add horizonal rule
		void addDependency(size_t indent = 0);                     // add dependency block
		void addLink(File dep, size_t indent);                     // add dependency link


		File file_;
		Dependency deps_;
		Html html_;
		Stack stack_;
		Html date_;
		File css_ = "style_sheet.css";
		Html author_ = "WENYU ZHANG";
		Html subject_ = "CSE687 - Object Oriented Design, Spring 2017";
		Html description_ = "Main Page for Indexing other pages";
	};
}



#endif // !HTMLGENERATOR_H