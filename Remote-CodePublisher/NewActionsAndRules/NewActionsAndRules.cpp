/////////////////////////////////////////////////////////////////////
// NewActionsAndRules.h - extends actions and rules                //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #2         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////

#include "NewActionsAndRules.h"
#include "../Utilities/Utilities.h"

using namespace CodeAnalysis;

Repository* Repository::instance;

#ifdef TEST_NEWACTIONSANDRULES

#include <iostream>
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"

//----< add action and rule to detect enum >------------------------------------------

void addEnumActionRule(Parser* pParser, Repository* pRepo) {
	EnumDefinition *pEnum = new EnumDefinition;
	HandleEnumDefinition *pHandleEnumDefinition = new HandleEnumDefinition(pRepo);
	pEnum->addAction(pHandleEnumDefinition);
	pParser->addRule(pEnum);
}

//----< add action and rule to detect typedef >------------------------------------------

void addTypedefActionRule(Parser* pParser, Repository* pRepo) {
	DetectTypeDef *pTypedef = new DetectTypeDef;
	HandleTypeDef *pHandleEnumDefinition = new HandleTypeDef(pRepo);
	pTypedef->addAction(pHandleEnumDefinition);
	pParser->addRule(pTypedef);
}

int main(int argc, char* argv[])
{
	std::cout << "\n  Testing ActionsAndRules class\n "
		<< std::string(30, '=') << std::endl;

	try
	{
		Utilities::StringHelper::Title(
			"Test Extended Actions and Rules"
		);

		Repository* pRepo;
		Scanner::Toker toker;
		Scanner::SemiExp se(&toker);
		Parser parser(&se);
		addEnumActionRule(&parser, pRepo);
		addTypedefActionRule(&parser, pRepo);
		while(se.get())
		  parser.parse();
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  " << ex.what() << "\n\n";
	}
}


#endif // !TEST_NEWACTIONSANDRULES