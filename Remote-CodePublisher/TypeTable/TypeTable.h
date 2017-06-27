#pragma once
/////////////////////////////////////////////////////////////////////
// TypeTable.h - define Type Table                                 //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #2         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////
/*
*  Module Operations
* ====================
*  - This module contains two classes: Content and TypeTable
*  - Content: provide type definition for value of TypeTable,
*  - Composed information including absolutely path, type, namespace
*  - provide access to all three attributes
*
*  - TypeTable: provide map-type data structure for storing information 
*               obtained from TypaAnalysis, whose value type is Content
*  - Provide access to add new Key/Value entry
*  - Provide iterator to traverse the table
*  - Provide method to display table contents
*
*
*  Public Interface
* ==================
*  Content content()                                default constuctor for Content class
*  content.setPath(std::string)                     set path_ attribute
*  content.setType(std::string)                     set type_ attribute
*  content.setNamespace(std::string)                set namespace_ attribute
*  content.getPath()                                get path_ attribute
*  content.getType()                                get type_ attribute
*  content.getNamespace()                           get namespace_ attribute
*  content.equal(Content c1)                        check if equal by members
*
*  TypeTable table()                                default constructor for TypeTable class
*  table.addType(std::string, Content)              add new entry
*  table.begin()                                    provide iterator pointing to first entry
*  table.end()                                      provide iterator indicating table end
*  table.find(const std::string&)                   provide iterator finding certain key
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - TypeTable.h, TypeTable.cpp, Utilities.h                      //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_TYPETABLE in properties.    //
//    Set TypeTable as the startup project.                        //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv TypeTable.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 16 Mar 17
*  - first release
*
*/

#include <unordered_map>
#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

namespace CodeAnalysis {
	
	///////////////////////////////////////////////////////////////////////////
	// Content:
	// - Provide contents value for type table

	class Content {

	public:

		using Path = std::string;
		using Namespace = std::string;
		using FullName = std::string;
		using Type = std::string;

		void setPath(Path path) { path_ = path; }                  // set path_ attribute
		void setFullName(FullName name) { fullName_ = name; }      // set fullName_ attribute
		void setType(Type type) { type_ = type; }                  // set type_ attribute
		void setNamespace(Namespace name) { namespace_ = name; }   // set namespace_ attribute

		Path getPath() { return path_; }                           // get path_ attribute
		FullName getFullName() { return fullName_; }               // get fullName_ attribute
		Type getType() { return type_; }                           // get type_ attribute
		Namespace getNamespace() { return namespace_; }            // get namespace_ attribute
		bool equal(Content content);                               // check if two instances are equal by members

		std::string showContent();                                 // Display the content information in string

	private:

		FullName fullName_;                          // fully captured name
		Path path_;                                  // path information              
		Type type_;                                  // type information
		Namespace namespace_;                        // namespace information
	};

	///////////////////////////////////////////////////////////////////////////
	// TypeTable:
	// - Provides STL container for type entries
	// - Provides iterators traversing the table

	class TypeTable {
		
	public:

		using Name = std::string;
		using Path = std::string;
		using FullName = std::string;
		using Type = std::string;
		using Contents = std::vector<Content>;
		using Table = std::unordered_map<Name, Contents>;
		using Iterator = std::unordered_map<Name, Contents>::iterator;

		TypeTable() {}                                                 // default constructor
		~TypeTable() {}                                                // destructor
		Contents& operator[](const Name& name);                        // indexing operator

		void addType(Name typeName, Content content);                  // add new type entry into table
		Iterator begin() { return table_.begin(); }                    // provide iterator pointing to first entry
		Iterator end() { return table_.end(); }                        // provide iterator pointing to one position past last entry
		Iterator find(const Name& name) { return table_.find(name); }  // find certain key in TypeTable
		std::string showTable();                                       // display table information in string
		std::string showTableFormat();                                 // display table information in format

	private:

		Table table_;                                      // Type table
		std::string showContents(Contents contents);       // display contents information in string         
	};
}