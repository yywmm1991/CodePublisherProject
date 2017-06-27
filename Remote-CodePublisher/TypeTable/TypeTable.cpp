/////////////////////////////////////////////////////////////////////
// TypeTable.h - define Type Table                                 //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #2         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////

#include "TypeTable.h"
#include "../Utilities/Utilities.h"

using namespace CodeAnalysis;

//----< if two instances are equal bt members >---------------------------------------

bool Content::equal(Content content) {
	if (this->type_ == content.type_ && this->path_ == content.path_
		&& this->fullName_ == content.fullName_ && this->namespace_ == content.namespace_)
		return true;
	return false;
}

//----< display content in string >---------------------------------------------------

std::string Content::showContent() {
	std::ostringstream out;
	out << "\n    " << std::setw(10) << "Type" << " : " << type_;
	out << "\n    " << std::setw(10) << "FullName" << " : " << fullName_;
	out << "\n    " << std::setw(10) << "Path" << " : " << path_;
	out << "\n    " << std::setw(10) << "Namespace" << " : " << namespace_;
	out << std::endl;
	return out.str();
}

//----< index operator for TypeTable >------------------------------------------------

TypeTable::Contents& TypeTable::operator[](const Name& name) {
	return table_[name];
}

//----< add new type information into table >-----------------------------------------

void TypeTable::addType(Name typeName, Content content) {

	// typeName already exists in table_
	// check if there is duplicate
	if (table_.find(typeName) != table_.end()) {
		for (auto element : table_[typeName]) {
			if (element.equal(content))
				return;
		}
		table_[typeName].push_back(content);
	}
	// typeName not exist in table_ 
	else {
		Contents newValue({ content });
		auto entry = std::make_pair(typeName, newValue);
		table_.insert(entry);
	}
}

//----< display table information in string >-----------------------------------------

std::string TypeTable::showTable() {
	std::ostringstream out;
	for (auto entry : table_) {
		out << "\n-----< TypeName: " << entry.first << " >-----" << std::endl;
		out << showContents(entry.second);
	}
	out << std::endl;
	return out.str();
}

//----< display table information in format >-----------------------------------------

std::string TypeTable::showTableFormat() {
	std::ostringstream out;
	out << "\n " << std::left << std::setw(25) << "Name";
	out << std::setw(15) << "Type" << std::setw(20) << "Namespace" << "\n";
	return out.str();
}

//----< display contents in string >--------------------------------------------------

std::string TypeTable::showContents(Contents contents) {
	std::ostringstream out;
	for (auto content : contents) {
		out << content.showContent();
		out << std::endl;
	}
	return out.str();
}


//----< test stub >-------------------------------------------------------------------

#ifdef TEST_TYPETABLE

void testContent() {
	Utilities::StringHelper::Title("Test Content Class");
	Content content1;
	content1.setType("Test Type for content1");
	content1.setPath("Test Path for content1");
	content1.setFullName("Test FullName for content1");
	content1.setNamespace("Test Namespace for content1");
	std::cout << content1.showContent() << std::endl;
}

void testTypeTable() {
	Utilities::StringHelper::Title("Test TypeTable Class");
	std::cout << std::endl;
	Content content1;
	content1.setType("Test Type for content1");
	content1.setPath("Test Path for content1");
	content1.setFullName("Test FullName for content1");
	content1.setNamespace("Test Namespace for content1");
	Content content2;
	content2.setType("Test Type for content2");
	content2.setPath("Test Path for content2");
	content2.setFullName("Test FullName for content2");
	content2.setNamespace("Test Namespace for content2");
	Utilities::StringHelper::title("Add Two Contents into Same Key");
	std::cout << std::endl;
	TypeTable tb1;
	tb1.addType("Same Type", content1);
	tb1.addType("Same Type", content1);
	tb1.addType("Same Type", content2);
	std::cout << tb1.showTable() << std::endl;
	Utilities::StringHelper::title("Add Two Contents into Different Keys");
	std::cout << std::endl;
	TypeTable tb2;
	tb2.addType("Type #1", content1);
	tb2.addType("Type #2", content2);
	std::cout << tb2.showTable() << std::endl;
}

int main() {
	// Test Content class
	testContent();
	// Test TypeTable class
	testTypeTable();
}

#endif // !TEST_TYPETABLE