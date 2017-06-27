/////////////////////////////////////////////////////////////////////
// LocalPublisher.h - Organize and Executive for publishing html   //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #4         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////
#include "LocalPublisher.h"
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <fstream>

#include "../Parser/Parser.h"
#include "../FileSystem/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../TypeAnalysis/TypeAnalyzer.h"
#include "../TypeTable/TypeTable.h"
#include "../Display/Display.h"

using namespace CodePublisher;

//----< destructor for LocalPublisher >------------------------------------------------

LocalPublisher::~LocalPublisher() {
	delete depAnal;
	delete pDep;
	delete pHtmlFiles;
}

//----< test TypeAnalysis >-----------------------------------------------------------

void LocalPublisher::typeAnalysis() {
	TypeAnalyzer ta(table_);
	ta.startAnalysis();
}

//----< test Dependency Analysis >----------------------------------------------------

void LocalPublisher::depAnalysis() {
	depAnal = new DepAnalyzer(cppHeaderFiles(), cppImplemFiles(), table_);
	depAnal->processHFiles();
	depAnal->processImpleFiles();
	pDep = new std::unordered_map<std::string, std::set<std::string>>(depAnal->getMap());
}

//----< publish html files >----------------------------------------------------------

void LocalPublisher::publish() 
{
	typeAnalysis();
	depAnalysis();
	HtmlFileMgr mgr(cppHeaderFiles(), cppImplemFiles(), *pDep, outPath_ + "/");
	mgr.writeFiles();
}

//----< get dependency relationships >------------------------------------------------

LocalPublisher::Map LocalPublisher::getDependencies()
{
	return *pDep;
}

//----< process file name to certain pattern >----------------------------------------

std::string LocalPublisher::processFileName(Path file) {
	std::size_t found = file.find_last_of("/\\");
	return file.substr(found + 1);
}

//----< access to generated html files >----------------------------------------------

void LocalPublisher::getHtmlFiles() {
	pHtmlFiles = new HtmlFiles(FileSystem::Directory::getFiles(outPath_, "*.htm"));
}

//----< lanuch html page in browser >-------------------------------------------------

void LocalPublisher::lanuchPage(File file) {
	std::string browser{ "start " };
	browser += outPath_ + "/" + file;
	//browser += file;
	std::system(browser.c_str());
}

//----< generate main page >----------------------------------------------------------

void LocalPublisher::generateMainPage() {
	std::fstream html;
	html.open(outPath_ + "/" + "MainPage.htm", std::fstream::out | std::fstream::trunc);
	MainPageGenerator mpg("MainPage.htm", *pHtmlFiles);
	mpg.generateHtml();
	html << mpg.getHtml();
	html.close();
}

//----< test stub >-------------------------------------------------------------------

#ifdef TEST_LOCALPUBLISHER

#include <fstream>

int main(int argc, char* argv[])
{
	TypeTable testTable;
	LocalPublisher exec(testTable);

	try {
		bool succeeded = exec.ProcessCommandLine(argc, argv);
		if (!succeeded) { return 1; }
		exec.getSourceFiles();
		exec.processSourceCode(true);
		exec.complexityAnalysis();

		exec.publish();
		LocalPublisher::Map temp = exec.getDependencies();
		for (auto dep : temp)
		{
			std::cout << "File: " << dep.first << std::endl;
			for (auto file : dep.second)
				std::cout << "  " << file << std::endl;
		}
	}
	catch (std::exception& except)
	{
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
		return 1;
	}
	return 0;
}

#endif // !TEST_LOCALPUBLISHER