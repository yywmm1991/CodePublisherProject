#ifndef DEPENDENCYANALYZER_H
#define DEPENDENCYANALYZER_H
/////////////////////////////////////////////////////////////////////
// DependencyAnalyzer.h - Conduct Dependency Analysis              //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #3         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////
/*
*  Class Operations
* ====================
*  - This package contains one class: DepAnalyzer
*  - DepAnalyzer: provide method to analysis dependencies to given files using TypeTable
*  - Store dependencies relationships into NoSql database
*  - Provide access to the resultant database
*  - In the database, key is the file name, children are dependent file names
*  
*  Public Interface
* ==================
*  DepAnalyzer da(Files hfiles, Files cppFiles, TypeTable)        create a dependency analyzer
*  da.processHFiles()                                             process header files and store results into database
*  da.processImpleFiles()                                         process implementation files and store results into database
*  da.getMap()                                                    return analyzed result as an unordered_map
*  da.showDependencies()                                          display the dependency relationships in string
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - DependencyAnalyzer.h, DependencyAnalyzer.cpp, Utilities.h    //
//    AnstrSynTree.h, Logger.h, TypeTable.h, SemiExp.h             //
//    Tokenizer.h                                                  //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_DEPANALYSIS in properties.  //
//    Set DependencyAnalysis as the startup project.               //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv DependencyAnalysis.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 16 Mar 17
*  - first release
*  ver 2.0 : 02 Apr 17
*  - different result type for project 3
*    using unordered_map to store dependency result rather than NoSql database
*/

#include <string>
#include <istream>
#include <fstream>
#include <stack>
#include <vector>
#include <set>

#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "../TypeTable/TypeTable.h"

namespace CodeAnalysis {

	///////////////////////////////////////////////////////////////////////////
	// DepAnalyzer:
	// - Conduct dependency analysis
	// - Store dependency analysis result in NoSql database
	// - Provide access to dependency analysis result by database

	class DepAnalyzer {

	public:
		using File = std::string;
		using Files = std::vector<File>;
		using Name = std::string;
		using Names = std::vector<std::string>;
		using Dependencies = std::set<std::string>;
		using DepTable = std::unordered_map<std::string, Dependencies>;
		using Stack = std::vector<std::string>;
		using Set = std::set<std::string>;
		using Iterator = std::vector<std::string>::iterator;

		DepAnalyzer(const Files, const Files, TypeTable&);             // promotion constructor
		~DepAnalyzer();                                                // destructor
		void processHFiles();                                          // process all header files
		void processImpleFiles();                                      // process all implementation files
		DepTable getMap() { return depResult; }                        // return dependencies result by unordered_map
		std::string showDependencies();                                // display the dependencies

	private:
		bool attachFile(const std::string& file);                      // attach file to toker
		void startAnalysis(const File& file);                          // analysis current attached file
		bool containNspace();                                          // check if contain namespace declaration
		bool containUsing();                                           // check if contain using statement
		bool containStart();                                           // check if contain "{"
		bool containTermi();                                           // check if contain "}"
		void handleNspace();                                           // handle namespace in current semiExpression
		void handleUsing();                                            // handle using statement in current semiExpression
		void handleTermi();                                            // handle "}" in current semiExpression
		void typeMatch(const File& file);                              // handle match keywords in typetable
		void updateSet();                                              // update namespace subsets 
		void setGenerator(std::string record, unsigned int pos);       // utility function for updateSet()
		void formatStack();                                            // generate formattedStack_
		void addMutDep(const File& file, const File& tFile);           // check mutual dependency 
		bool isHFile(const File& file);                                // check if the file is header file
		Name addPrefix(Name name);                                     // add prefix occured in statement
		Files hFiles_;
		Files impleFiles_;
		TypeTable& table_;
		std::ifstream* pIn;
		Scanner::Toker* pToker;
		Scanner::SemiExp* pSemi;
		DepTable depResult;
		Stack stack_;
		Stack formattedStack_;
		Set nspaceSet;
		Dependencies deps;

	};
}

#endif // !DEPENDENCYANALYZER_H

