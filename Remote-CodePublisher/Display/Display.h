#ifndef DISPLAY_H
#define DISPLAY_H
/////////////////////////////////////////////////////////////////////
// Display.h - Generally useful helper functions                   //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #2         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
// Reference:   Jim Fawcett, CST 4-187, Syracuse University        //
//              jfawcett@twcny.rr.com, (315) 443-3948              //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations
* ====================
*  - This package provides some simple but generally useful functions
*    needed by this project
*  - The functions inside this package have high reusability
*  - Provide methods mainly for displaying or demonstrating
*
*  Public Interface
* ==================
*  makeTitle(std::string)                         display a string-type title
*  makeSubTitle(std::string)                      display a string-type subtitle
*  makeComment(std::string)                       display a string0type comment
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - Display.h, Display.cpp                                       //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_DISPLAY in properties.      //
//    Set Display as the startup project.                          //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv Display.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 15 Feb 17
*  - first release
*
*/
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <functional>

namespace CodePublisher {

	/*----< Helper function to make title >------------------*/

	inline void makeTitle(const std::string& title, char underlineChar = '=')
	{
		std::cout << "\n " << std::string(16, underlineChar);
		std::cout << " " << title;
		std::cout << " " << std::string(16, underlineChar) << std::endl;
	}

	/*----< Helper function to make subtitle >------------------*/

	inline void makeSubTitle(const std::string& subTitle, char underlineChar = '=') {
		std::cout << "\n  " << subTitle;
		std::cout << "\n " << std::string(subTitle.size() + 2, underlineChar);
	}

	/*----< Helper function to make comment for demo >------------------*/

	inline void makeComment(const std::string& comment, char underlineChar = '-')
	{
		std::cout << "\n " << std::string(8, underlineChar);
		std::cout << " " << comment;
		std::cout << " " << std::string(8, underlineChar) << std::endl;
	}
}
#endif // !DISPLAY_H

