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

#include "Display.h"
#include <iostream>
#include <string>

#ifdef TEST_DISPLAY

using namespace CodePublisher;

int main()
{
	makeTitle("Test makeTitle()");
	makeSubTitle("Test makeSubTitle()");
	makeComment("Test makeComment()");
}

#endif // TEST_DISPLAY