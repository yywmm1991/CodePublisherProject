/////////////////////////////////////////////////////////////////////
// HtmlFileMgr.cpp - Organize generating of html files             //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #3         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 8.1, Visual Studio 2015                        //
// Author:      WENYU ZHANG, 233508014                             //
//              wzhan102@syr.edu                                   //
/////////////////////////////////////////////////////////////////////

#include "HtmlFileMgr.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace CodePublisher;

//----< promotion constructor for HtmlFileMgr >---------------------------------------

HtmlFileMgr::HtmlFileMgr(const Files hFile, const Files impleFile, Map map, Path dir)
	:hFiles_(hFile),
	 impleFiles_(impleFile),
	 dep_(map),
	 dir_(dir)
{}

//----< destructor for HtmlFileMgr >--------------------------------------------------

HtmlFileMgr::~HtmlFileMgr()
{
	delete pHtml; 
	delete pCount;
	delete pLC;
}

//----< write html file >-------------------------------------------------------------

void HtmlFileMgr::writeFiles() {
	// Write all header files
	for (auto file : hFiles_) {
		std::fstream html;
		html.open(dir_ + processFileName(file) + ext_, std::fstream::out | std::fstream::trunc);
		html << getHtml(file);
		html.close();
	}
	// Write all implementation files
	for (auto file : impleFiles_) {
		std::fstream html;
		html.open(dir_ + processFileName(file) + ext_, std::fstream::out | std::fstream::trunc);
		html << getHtml(file);
		html.close();
		//std::cout << getHtml(file);
	}
}

//----< process full specific file name >---------------------------------------------

HtmlFileMgr::File HtmlFileMgr::processFileName(File file) {
	std::size_t found = file.find_last_of("/\\");
	return file.substr(found + 1);
}

//----< generate html file >----------------------------------------------------------

HtmlFileMgr::Html HtmlFileMgr::getHtml(File file) {
	pCount = new int(1);
	// Get line counts
	pLC = new LineCounter(file, types_);
	pLC->getLineCounts();
	std::set<int> strts = pLC->getStrtLines();
	std::set<int> ends = pLC->getEndLines();

	FileSystem::File code(file);
	code.open(FileSystem::File::in);
	std::string temp = "";
	while (code.isGood()) 
	{
		std::string line = code.getLine();
		line = processCode(line);
		if (strts.find(*pCount) != strts.end())
			line = addLeftHide(line);
		if (ends.find(*pCount) != ends.end())
			line = addRightHide(line);
		temp = temp + line + "\n";
		(*pCount)++;
	}
	code.close();
	pHtml = new HtmlGenerator(processFileName(file), temp, dep_[file]);
	pHtml->generateHtml();
	return pHtml->getHtml();
}

//----< process source code >----------------------------------------------------------

HtmlFileMgr::Code HtmlFileMgr::processCode(Code code) {
	size_t found = code.find('<');
	while (found != std::string::npos) {
		code.replace(found, 1, "&lt");
		found = code.find('<');
	}
	found = code.find('>');
	while (found != std::string::npos) {
		code.replace(found, 1, "&gt");
		found = code.find('>');
	}

	return code;
}

//----< add hide script for left brace >---------------------------------------------

HtmlFileMgr::Code HtmlFileMgr::addLeftHide(Code code)
{
	std::string hideStart = "{<button onclick=\"myFunction(" + std::to_string(*pCount) + ")\">-</button><bre id=\"" + std::to_string(*pCount) + "\">";
	size_t found = code.find('{');
	if (found != std::string::npos)
	{
		code.replace(found, 1, hideStart);
	}
	return code;
}

//----< add hide script for right brace >----------------------------------------------

HtmlFileMgr::Code HtmlFileMgr::addRightHide(Code code)
{
	std::string hideEnd = "</bre>}";
	std::string hideEndSemi = "</bre>};";
	size_t found = code.find('}');
	size_t semiFound = code.find(';');
	if (found != std::string::npos)
	{
		if (semiFound == found + 1)
			code.replace(found, 2, hideEndSemi);
		else
			code.replace(found, 1, hideEnd);
	}
	else
		return code + "</bre>";
	return code;
}

//----< test stub >-------------------------------------------------------------------

#ifdef TEST_HTMLFILEMGR

#include <iostream>
#include "../Display/Display.h"

int main() {
	makeTitle("Test HtmlFileMgr:");
	std::vector<std::string> hFile{ "TestFolder/Test1.h" };
	std::vector<std::string> impleFile{ "TestFolder/Test1.cpp", "TestFolder/Test2.cpp" };
	HtmlFileMgr mgr(hFile, impleFile);
	mgr.writeFiles();
}


#endif // TEST_HTMLFILEMGR