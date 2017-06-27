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
#include "Executive.h"
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

#include "../Parser/Parser.h"
#include "../FileSystem/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"

using Rslt = Logging::StaticLogger<0>;  // use for application results
using Demo = Logging::StaticLogger<1>;  // use for demonstrations of processing
using Dbug = Logging::StaticLogger<2>;  // use for debug output


/////////////////////////////////////////////////////////////////////
// AnalFileMgr class
// - Derives from FileMgr to make application specific file handler
//   by overriding FileMgr::file(), FileMgr::dir(), and FileMgr::done()

using Path = std::string;
using File = std::string;
using Files = std::vector<File>;
using Pattern = std::string;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, Files>;

using namespace CodePublisher;

//----< initialize application specific FileMgr >--------------------
/*
* - Accepts CodeAnalysisExecutive's path and fileMap by reference
*/
AnalFileMgr::AnalFileMgr(const Path& path, FileMap& fileMap)
	: FileMgr(path), fileMap_(fileMap), numFiles_(0), numDirs_(0) {}

//----< override of FileMgr::file(...) to store found files >------

void AnalFileMgr::file(const File& f)
{
	File fqf = d_ + "\\" + f;
	Ext ext = FileSystem::Path::getExt(fqf);
	Pattern p = "*." + ext;
	fileMap_[p].push_back(fqf);
	++numFiles_;
}
//----< override of FileMgr::dir(...) to save current dir >----------

void AnalFileMgr::dir(const Dir& d)
{
	d_ = d;
	++numDirs_;
}
//----< override of FileMgr::done(), not currently used >------------

void AnalFileMgr::done()
{
}
//----< returns number of matched files from search >----------------

size_t AnalFileMgr::numFiles()
{
	return numFiles_;
}
//----< returns number of dirs searched >----------------------------

size_t AnalFileMgr::numDirs()
{
	return numDirs_;
}

/////////////////////////////////////////////////////////////////////
// CodePublisherExecutive class
// - 
using Path = std::string;
using Pattern = std::string;
using Patterns = std::vector<Pattern>;
using File = std::string;
using Files = std::vector<File>;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, Files>;
using ASTNodes = std::vector<ASTNode*>;
using FileToNodeCollection = std::vector<std::pair<File, ASTNode*>>;

//----< initialize parser, get access to repository >----------------

CodePublisherExecutive::CodePublisherExecutive()
{
	pParser_ = configure_.Build();
	if (pParser_ == nullptr)
	{
		throw std::exception("couldn't create parser");
	}
	pRepo_ = Repository::getInstance();
}
//----< cleanup >----------------------------------------------------

CodePublisherExecutive::~CodePublisherExecutive()
{}

//----< handle command line arguments >------------------------------

bool CodePublisherExecutive::ProcessCommandLine(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Not Enough Arguments!" << std::endl;
		return false;
	}
	try {
		path_ = FileSystem::Path::getFullFileSpec(argv[1]);
		if (!FileSystem::Directory::exists(path_))
		{
			std::cout << "\n\n  path \"" << path_ << "\" does not exist\n\n";
			return false;
		}
		for (int i = 2; i < argc; ++i)
		{
			if (argv[i][0] == '*')
				patterns_.push_back(argv[i]);
		}
		if (patterns_.size() == 0)
		{
			return false;
		}
	}

	catch (std::exception& ex)
	{
		std::cout << "\n\n  command line argument parsing error:";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}
	return true;
}
//----< returns path entered on command line >-------------------

std::string CodePublisherExecutive::getAnalysisPath()
{
	return path_;
}

//----< returns reference to FileMap >---------------------------
/*
* Supports quickly finding all the files found with a give pattern
*/
FileMap& CodePublisherExecutive::getFileMap()
{
	return fileMap_;
}

//----< searches path for files matching specified patterns >----
/*
* - Searches entire diretory tree rooted at path_, evaluated
*   from a command line argument.
* - Saves found files in FileMap.
*/
void CodePublisherExecutive::getSourceFiles()
{
	AnalFileMgr fm(path_, fileMap_);
	for (auto patt : patterns_)
		fm.addPattern(patt);
	fm.search();
	numFiles_ = fm.numFiles();
	numDirs_ = fm.numDirs();
}
//----< helper: is text a substring of str? >--------------------

bool contains(const std::string& str, const std::string& text)
{
	if (str.find(text) < str.length())
		return true;
	return false;
}
//----< retrieve from fileMap all files matching *.h >-----------

std::vector<File>& CodePublisherExecutive::cppHeaderFiles()
{
	cppHeaderFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.h"))
		{
			for (auto file : item.second)
				cppHeaderFiles_.push_back(file);
		}
	}
	return cppHeaderFiles_;
}
//----< retrieve from fileMap all files matching *.cpp >---------

std::vector<File>& CodePublisherExecutive::cppImplemFiles()
{
	cppImplemFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.cpp"))
		{
			for (auto file : item.second)
				cppImplemFiles_.push_back(file);
		}
	}
	return cppImplemFiles_;
}
//----< retrieve from fileMap all files matching *.cs >----------

std::vector<File>& CodePublisherExecutive::csharpFiles()
{
	csharpFiles_.clear();
	for (auto item : fileMap_)
	{
		if (contains(item.first, "*.cs"))
		{
			for (auto file : item.second)
				csharpFiles_.push_back(file);
		}
	}
	return csharpFiles_;
}
//----< report number of Source Lines Of Code (SLOCs) >----------

CodePublisherExecutive::Slocs CodePublisherExecutive::fileSLOCs(const File& file)
{
	return slocMap_[file];
}
//----< report number of files processed >-----------------------

size_t CodePublisherExecutive::numFiles()
{
	return numFiles_;
}
//----< report number of directories searched >------------------

size_t CodePublisherExecutive::numDirs()
{
	return numDirs_;
}

//----< parses code and saves results in AbstrSynTree >--------------
/*
* - Processes C++ header files first to build AST with nodes for
*   all public classes and structs.
* - Then processes C++ implementation files.  Each member function
*   is relocated to its class scope node, not the local scope.
* - Therefore, this ordering is important.
* - C# code has all member functions inline, so we don't need to
*   do any relocation of nodes in the AST.  Therefore, that analysis
*   can be done at any time.
* - If you bore down into the analysis code in ActionsAndRules.h you
*   will find some gymnastics to handle template syntax.  That can
*   get somewhat complicated, so there may be some latent bugs there.
*   I don't know of any at this time.
*/
void CodePublisherExecutive::setLanguage(const File& file)
{
	std::string ext = FileSystem::Path::getExt(file);
	if (ext == "h" || ext == "cpp")
		pRepo_->language() = Language::Cpp;
	else if (ext == "cs")
		pRepo_->language() = Language::CSharp;
}

void CodePublisherExecutive::processSourceCode(bool showProc)
{
	for (auto file : cppHeaderFiles())
	{
		pRepo_->package() = FileSystem::Path::getName(file);

		if (!configure_.Attach(file))
		{
			std::cout << "\n  could not open file " << file << "\n";
			continue;
		}
		// parse file
		pRepo_->language() = Language::Cpp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
		{
			pParser_->parse();
		}

		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;
	}
	for (auto file : cppImplemFiles())
	{
		pRepo_->package() = FileSystem::Path::getName(file);

		if (!configure_.Attach(file))
		{
			std::cout << "\n  could not open file " << file << "\n";
			continue;
		}

		// parse file
		pRepo_->language() = Language::Cpp;
		pRepo_->currentPath() = file;
		while (pParser_->next())
			pParser_->parse();

		Slocs slocs = pRepo_->Toker()->currentLineCount();
		slocMap_[pRepo_->package()] = slocs;
	}

}
//----< evaluate complexities of each AST node >---------------------

void CodePublisherExecutive::complexityAnalysis()
{
	ASTNode* pGlobalScope = pRepo_->getGlobalScope();
	CodeAnalysis::complexityEval(pGlobalScope);
}
//----< comparison functor for sorting FileToNodeCollection >----
/*
* - supports stable sort on extension values
* - displayMetrics(...) uses to organize metrics display
*/
struct CompExts
{
	bool operator()(const std::pair<File, ASTNode*>& first, const std::pair<File, ASTNode*>& second)
	{
		return FileSystem::Path::getExt(first.first) > FileSystem::Path::getExt(second.first);
	}
};
//----< comparison functor for sorting FileToNodeCollection >----
/*
* - supports stable sort on name values
* - displayMetrics(...) uses these functions to organize metrics display
*/
static void removeExt(std::string& name)
{
	size_t extStartIndex = name.find_last_of('.');
	name = name.substr(0, extStartIndex);
}

struct CompNames
{
	bool operator()(const std::pair<File, ASTNode*>& first, const std::pair<File, ASTNode*>& second)
	{
		std::string fnm = FileSystem::Path::getName(first.first);
		removeExt(fnm);
		std::string snm = FileSystem::Path::getName(second.first);
		removeExt(snm);
		return fnm < snm;
	}
};

//----< display lines containing public data declaration >-----------

std::string CodePublisherExecutive::showData(const Scanner::ITokCollection* pTc)
{
	std::string semiExpStr;
	for (size_t i = 0; i<pTc->length(); ++i)
		semiExpStr += (*pTc)[i] + " ";
	return semiExpStr;
}


//----< walk tree of element nodes >---------------------------------

template<typename element>
void TreeWalk(element* pItem, bool details = false)
{
	static std::string path;
	if (path != pItem->path_ && details == true)
	{
		path = pItem->path_;
		Rslt::write("\n" + path);
	}
	static size_t indentLevel = 0;
	std::ostringstream out;
	out << "\n  " << std::string(2 * indentLevel, ' ') << pItem->show();
	Rslt::write(out.str());
	auto iter = pItem->children_.begin();
	++indentLevel;
	while (iter != pItem->children_.end())
	{
		TreeWalk(*iter);
		++iter;
	}
	--indentLevel;
}

//----< comparison functor for sorting SLOC display >----------------

struct compFiles
{
private:
	std::string ChangeFirstCharOfExt(const std::string& fileName) const
	{
		std::string temp = fileName;
		size_t pos = temp.find_last_of('.');
		if (pos < temp.size() - 1)
			if (temp[pos + 1] == 'h')
				temp[pos + 1] = 'a';
		return temp;
	}
public:
	bool operator()(const std::string& fileName1, const std::string& fileName2) const
	{
		return ChangeFirstCharOfExt(fileName1) < ChangeFirstCharOfExt(fileName2);
	}
};

//----< open file stream for logging >-------------------------------
/*
*  - must come after CodeAnalysisExecutive::processCommandLine()
*  - must come before starting any of the loggers
*/
void CodePublisherExecutive::setLogFile(const File& file)
{
	std::string path = getAnalysisPath();
	path += "\\" + file;
	pLogStrm_ = new std::ofstream(path);
	if (pLogStrm_->good())
	{
		Rslt::attach(pLogStrm_);
		Demo::attach(pLogStrm_);
		Dbug::attach(pLogStrm_);
	}
	else
		Rslt::write("\n  couldn't open logFile.txt for writing");
}

std::string CodePublisherExecutive::systemTime()
{
	time_t sysTime = time(&sysTime);
	char buffer[27];
	ctime_s(buffer, 27, &sysTime);
	buffer[24] = '\0';
	std::string temp(buffer);
	return temp;
}

//----< test stub >-------------------------------------------------------------------

#ifdef TEST_EXECUTIVE

#include <fstream>

int main(int argc, char* argv[])
{
	using namespace CodeAnalysis;

	CodePublisherExecutive exec;

	try {
		bool succeeded = exec.ProcessCommandLine(argc, argv);
		if (!succeeded)
		{
			return 1;
		}
		exec.getSourceFiles();
		exec.processSourceCode(true);
		exec.complexityAnalysis();
	}
	catch (std::exception& except)
	{
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
		return 1;
	}
	return 0;
}

#endif // !TEST_EXECUTIVE