/////////////////////////////////////////////////////////////////////
// DependencyAnalyzer.h - Conduct Dependency Analysis              //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #3         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////

#include "DependencyAnalyzer.h"

using namespace CodeAnalysis;

//----< promotion constructor for DepAnalyzer >---------------------------------------

DepAnalyzer::DepAnalyzer(const Files hFiles, const Files impleFiles, TypeTable& table)
	:hFiles_(hFiles),
	 impleFiles_(impleFiles),
	 table_(table)
{
	pToker = new Scanner::Toker;
	pToker->returnComments(false);
	pSemi = new Scanner::SemiExp(pToker);
}

//----< destructor for DepAnalyzer >--------------------------------------------------

DepAnalyzer::~DepAnalyzer() {
	delete pToker;
	delete pIn;
	delete pSemi;
}

//----< process all header files >----------------------------------------------------

void DepAnalyzer::processHFiles() {
	for (auto file : hFiles_) {
		//std::cout << "\n Analyzing File:" << file << std::endl;
		attachFile(file);
		startAnalysis(file);
		depResult[file] = std::move(deps);

		// Method using NoSql
		/*for (auto dep : deps)
			db_.addChild(file, dep);*/

		deps.clear();
	}
}

//----< process all implementation files >--------------------------------------------

void DepAnalyzer::processImpleFiles() {
	for (auto file : impleFiles_) {
		//std::cout << "\n Analyzing File:" << file;
		attachFile(file);
		startAnalysis(file);
		depResult[file] = std::move(deps);

		// Method using NoSql
		/*for (auto dep : deps)
			db_.addChild(file, dep);*/

		deps.clear();
	}
}

//----< display dependencies >--------------------------------------------------------

std::string DepAnalyzer::showDependencies() {
	std::ostringstream out;
	for (auto entry : depResult) {
		out << "\n\n -----< File Name >-----\n";
		out << entry.first << std::endl;
		out << "\n-----< Dependent Files >-----\n";
		for (auto elem : entry.second)
			out << elem << std::endl;
	}
	return out.str();
	//return db_.showDatabase();
}

//----< attach new file to toker >----------------------------------------------------

bool DepAnalyzer::attachFile(const std::string& file) {
	pIn = new std::ifstream(file);
	if (!pIn->good())
		return false;
	else {
		return pToker->attach(pIn);
	}
}

//----< check if contain namespace declaration >--------------------------------------

bool DepAnalyzer::containNspace() {
	size_t nspaceIndex = pSemi->find("namespace");
	size_t termiIndex = pSemi->find("{");
	if (nspaceIndex < pSemi->length() && termiIndex < pSemi->length())
		return true;
	return false;
}

//----< check if contain using statement >--------------------------------------------

bool DepAnalyzer::containUsing() {
	size_t usingIndex = pSemi->find("using");
	size_t equalIndex = pSemi->find("=");
	if (usingIndex < pSemi->length() && !(equalIndex < pSemi->length()))
		return true;
	return false;
}

//----< check if contain "{" character >----------------------------------------------

bool DepAnalyzer::containStart() {
	size_t termiIndex = pSemi->find("{");
	if (termiIndex < pSemi->length())
		return true;
	return false;
}

//----< check if contain "}" character >----------------------------------------------

bool DepAnalyzer::containTermi() {
	size_t termiIndex = pSemi->find("}");
	if (termiIndex < pSemi->length()) {
		return true;
	}
	return false;
}

//----< handle namespace declaration in current semiExpression >----------------------

void DepAnalyzer::handleNspace() {
	size_t nspaceIndex = pSemi->find("namespace");
	size_t termiIndex = pSemi->find("{");
	if (nspaceIndex < pSemi->length() && termiIndex < pSemi->length()) {
		stack_.push_back("{");
		stack_.push_back((*pSemi)[nspaceIndex + 1]);
		updateSet();
	}
}

//----< handle using statement in current semiExpression >----------------------------

void DepAnalyzer::handleUsing() {
	size_t usingIndex = pSemi->find("using");
	size_t equalIndex = pSemi->find("=");
	if (usingIndex < pSemi->length() && !(equalIndex < pSemi->length())) {
		stack_.push_back((*pSemi)[pSemi->find(";") - 1]);
		updateSet();
	}
}

//----< handle "}" in current semiExpression >----------------------------------------

void DepAnalyzer::handleTermi() {
	if (stack_.size() == 0)
		return;
	std::string top = stack_.back();
	stack_.pop_back();
	int count = 0;
	while (top != "{") {
		top = stack_.back();
		stack_.pop_back();
		count++;
	}
	if (count > 0) {
		updateSet();
	}
}

//----< handle type match current semiExpression >------------------------------------

void DepAnalyzer::typeMatch(const File& file) {
	// Handle "{" character
	if (containStart())
		stack_.push_back("{");
	Iterator iter = pSemi->begin();
	while (iter != pSemi->end()) {
		// If there is a match
		if (table_.find(*iter) != table_.end()) {
			// add prefix into name
			std::string prefixedName = addPrefix(*iter);
			// check prefixs in namespace subset
			for (auto nspace : nspaceSet) {
				for (auto content : table_.find(*iter)->second) {
					if ((nspace+*iter) == content.getFullName() && file != content.getPath()) {
						deps.insert(content.getPath());
						if (containStart())
							addMutDep(file, content.getPath());
					}
				}
			}
		}
		iter++;
	}
	if (containUsing()) {
		handleUsing();
	}
}

//----< add mutual dependency between header and implementation files >---------------

void DepAnalyzer::addMutDep(const File& file, const File& tFile) {
	// skip when file is a header file
	for (auto f : hFiles_) {
		if (f == file) {
			return;
		}
	}
	// skip when target file is not a header file
	if (!isHFile(tFile))
		return;
	// add mutual dependency
	depResult[tFile].insert(file);
}

//----< check if the file is header file >--------------------------------------------

bool DepAnalyzer::isHFile(const File& file) {
	for (auto f : hFiles_) {
		if (f == file) {
			return true;
		}
	}
	return false;
}

//----< update namespace subsets >----------------------------------------------------

void DepAnalyzer::updateSet() {
	if (stack_.size() == 0)
		return;
	formatStack();
	std::string nspaceRecord = "Global Namespace::";
	setGenerator(nspaceRecord, 0);
}

//----< utility function for updateSet() >--------------------------------------------

void DepAnalyzer::setGenerator(std::string record, unsigned int pos) {
	if (pos >= formattedStack_.size()) {
		nspaceSet.insert(record);
		return;
	}
	nspaceSet.insert(record);
	for (unsigned int i = pos; i < formattedStack_.size(); i++) {
		std::string newNspace = record;
		newNspace = newNspace + formattedStack_[i] + "::";
		setGenerator(newNspace, pos + 1);
	}
}

//----< generate formattedStack_ >----------------------------------------------------

void DepAnalyzer::formatStack() {
	formattedStack_.clear();
	for (auto elem : stack_)
		if (elem != "{")
			formattedStack_.push_back(elem);
}

//----< update namespace subsets >----------------------------------------------------

DepAnalyzer::Name DepAnalyzer::addPrefix(Name name) {
	Name prefixedName = name;
	size_t index = pSemi->find(name);
	while (index >= 2 && (*pSemi)[index - 1] == "::") {
		Name prefix =  (*pSemi)[index - 2] + (*pSemi)[index - 1];
		prefixedName = prefix + prefixedName;
		index = index - 2;
	}
	return prefixedName;
}

//----< analysis current attached file >----------------------------------------------

void DepAnalyzer::startAnalysis(const File& file) {
	while (pSemi->get()) {
		pSemi->trim();

		// Handle namespace declaration statement
		if (containNspace()) {
			handleNspace();
			continue;
		}

		// Handle "}" statement 
		if (containTermi()) {
			handleTermi();
			continue;
		}
		// Try typeMatch and handle "using namespace" statement
		typeMatch(file);
	}
}

//----< test stub >-------------------------------------------------------------------

#ifdef TEST_DEPANALYSIS

int main() {
	std::cout << "Test Dependency Analysis";
	DepAnalyzer* depAnal = new DepAnalyzer(cppHeaderFiles(), cppImplemFiles(), table_);
	depAnal->processHFiles();
	depAnal->processImpleFiles();
	std::cout << depAnal->showDependencies();
}
#endif // !TEST_DEPANALYSIS
