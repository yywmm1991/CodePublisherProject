/////////////////////////////////////////////////////////////////////
// HtmlGenerator.cpp - Generate Html code on C++ source codes      //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #3         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 8.1, Visual Studio 2015                        //
// Author:      WENYU ZHANG, 233508014                             //
//              wzhan102@syr.edu                                   //
/////////////////////////////////////////////////////////////////////

#include "HtmlGenerator.h"

#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip> 
#include <fstream>

using namespace CodePublisher;

//----< promotion constructor for HtmlTag >-------------------------------------------

HtmlTag::HtmlTag(TagName tagname, Attribute attribute, size_t indent)
	:tagname_(tagname),
	 attribute_(attribute),
	 indent_(std::string(indent, ' '))
{}

//----< generate start tag in string >------------------------------------------------

std::string HtmlTag::generateStartTag() {
	std::string temp = "\n" + indent_ + "<" + this->getName();
	if (this->getAttribute() != "")
		temp = temp + " " + this->getAttribute();
	return temp + ">";
}

//----< generate end tag in string >------------------------------------------------

std::string HtmlTag::generateEndTag() {
	return "\n" + indent_ + "</" + this->getName() + ">";
}

//----< promotion constructor for HtmlGenerator >-------------------------------------

HtmlGenerator::HtmlGenerator(File file, Code cppCode, Dependency deps)
	:file_(file),
	 code_(cppCode),
	 deps_(deps),
	 html_("")
{

	getDate();
}

//----< generate html regarding source code >-----------------------------------------

void HtmlGenerator::generateHtml() {
	// Add prologure
	addPrologue();
	// Deal with <html> block
	addHtml();
	// Deal with <head> block
	addHead();
	popHtmlTag();
	// Deal with <body> block
	addBody();
	// Deal with file name heading
	addHeading(file_, "h3", 2);
	addHRule(2);
	// Deal with dependency
	addDependency(4);
	addHRule(4);
	addCode(0);
	//addFunction();
	popHtmlTag();
	popHtmlTag();
}

//----< process source code >---------------------------------------------------------

HtmlGenerator::Code HtmlGenerator::processCode(Code code) {
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

//----< get processed date for generated html file >----------------------------------

void HtmlGenerator::getDate() {
	time_t t = time(nullptr);
	tm tm;
	localtime_s(&tm, &t);
	std::stringstream out;
	out << std::put_time(&tm, "%d %B %Y");
	date_ =  out.str();
}

//----< add prologue into html code >-------------------------------------------------

void HtmlGenerator::addPrologue() {
	Html startTag = "<!" + std::string(76, '-');
	html_ = html_ + startTag;
	html_ = html_ + "\n" + processFileName(file_) + ".htm" + " - " + description_;
	html_ = html_ + "\n" + "Published " + date_;
	html_ = html_ + "\n" + author_ + ", " + subject_;
	Html endTag = std::string(78, '-') + ">";
	html_ = html_ + "\n" + endTag;
}

//----< push tag into stack >---------------------------------------------------------

void HtmlGenerator::pushHtmlTag(HtmlTag tag) {
	html_ = html_ + tag.generateStartTag();
	stack_.push(tag);
}

//----< add <style> tag >--------------------------------------------------------------

void HtmlGenerator::popHtmlTag() {
	if (stack_.size() > 0) {
		HtmlTag tag = stack_.top();
		html_ = html_ + tag.generateEndTag();
		stack_.pop();
	}
}

//----< add <html> tag >--------------------------------------------------------------

void HtmlGenerator::addHtml(Html attr) {
	HtmlTag html("html", attr);
	pushHtmlTag(html);
}

//----< add <head> tag >--------------------------------------------------------------

void HtmlGenerator::addHead(Html attr) {
	HtmlTag head("head", attr);
	pushHtmlTag(head);
	addCSS();
	addJS();
}

//----< add link to CSS file >--------------------------------------------------------

void HtmlGenerator::addCSS() {
	html_ = html_ + "\n<link rel = \"stylesheet\" type = \"text/css\" href = \"" + css_ + "\">" + "\n";
}

//----< add link to Javascript >------------------------------------------------------

void HtmlGenerator::addJS() {
	html_ = html_ + "<script type=\"text/javascript\" src=\"" + js_ + "\"></script>";
}

//----< add <body> tag >--------------------------------------------------------------

void HtmlGenerator::addBody(Html attr) {
	HtmlTag html("body", attr);
	pushHtmlTag(html);
}

//----< add file name as heading >----------------------------------------------------

void HtmlGenerator::addHeading(File file, Html size, size_t indent, Html attr) {
	HtmlTag html(size, attr,indent);
	pushHtmlTag(html);
	html_ = html_ + file;
	popHtmlTag();
}

//----< add horizonal rule >----------------------------------------------------------

void HtmlGenerator::addHRule(size_t indent) {
	std::string ind(indent, ' ');
	html_ = html_ + "\n" + ind + "<hr />";
}

//----< add dependency block >--------------------------------------------------------

void HtmlGenerator::addDependency(size_t indent) {
	HtmlTag div("div", "class = \"indent\"", indent);
	pushHtmlTag(div);
	addHeading("Dependencies:", "h4", indent+2);
	if (deps_.size() == 0)
		addHeading("Such file has no dependency!", "h4", indent + 2, "class = \"nodep\"");
	for (auto dep : deps_) {
		addLink(dep, indent + 2);
		html_ = html_ + "\n  <br>";
	}
	popHtmlTag();
}

//----< add dependency links >--------------------------------------------------------

void HtmlGenerator::addLink(File dep, size_t indent) {
	HtmlTag link("a", "href = \"" + processFileName(dep) + ".htm\"", indent);
	pushHtmlTag(link);
	html_ = html_ + processFileName(dep);
	popHtmlTag();
}

//----< process file name to certain pattern >----------------------------------------

HtmlGenerator::Html HtmlGenerator::processFileName(File file) {
	std::size_t found = file.find_last_of("/\\");
	return file.substr(found + 1);
}

//----< add javascript function >-----------------------------------------------------

void HtmlGenerator::addFunction() {
	HtmlTag html("script");
	pushHtmlTag(html);
	std::string temp;
	std::fstream script;
	script.open(file_, std::fstream::out | std::fstream::trunc);
	script >> temp;
	html_ = html_ + temp;
	popHtmlTag();
}

//----< add source file content >-----------------------------------------------------

void HtmlGenerator::addCode(size_t indent) {
	HtmlTag code("pre", "", indent);
	pushHtmlTag(code);
	html_ = html_ + "\n" + code_;
	popHtmlTag();
}

//----< promotion constructor for MainPageGenerator >---------------------------------

MainPageGenerator::MainPageGenerator(File file, Dependency deps)
	:file_(file),
	 deps_(deps)
{
	getDate();
}

//----< promotion constructor for MainPageGenerator >---------------------------------

void MainPageGenerator::generateHtml() {
	// Add prologure
	addPrologue();
	// Deal with <html> block
	addHtml();
	// Deal with <head> block
	addHead();
	popHtmlTag();
	// Deal with <body> block
	addBody();
	// Deal with file name heading
	addHeading("This Page displays links to all generated pages", "h3", 2);
	addHRule(2);
	// Deal with dependency
	addDependency(4);
	addHRule(4);

	popHtmlTag();
	popHtmlTag();
}

//----< add prologue to html code >---------------------------------------------------

void MainPageGenerator::addPrologue() {
	Html startTag = "<!" + std::string(76, '-');
	html_ = html_ + startTag;
	html_ = html_ + "\n" + file_ + ".htm" + " - " + description_;
	html_ = html_ + "\n" + "Published " + date_;
	html_ = html_ + "\n" + author_ + ", " + subject_;
	Html endTag = std::string(78, '-') + ">";
	html_ = html_ + "\n" + endTag;
}

//----< get processed date for generated html file >----------------------------------

void MainPageGenerator::getDate() {
	time_t t = time(nullptr);
	tm tm;
	localtime_s(&tm, &t);
	std::stringstream out;
	out << std::put_time(&tm, "%d %B %Y");
	date_ = out.str();
}

//----< add <html> tag >--------------------------------------------------------------

void MainPageGenerator::addHtml(Html attr) {
	HtmlTag html("html", attr);
	pushHtmlTag(html);
}

//----< add <head> tag >--------------------------------------------------------------

void MainPageGenerator::addHead(Html attr) {
	HtmlTag head("head", attr);
	pushHtmlTag(head);
	addCSS();
}

//----< push tag into stack >---------------------------------------------------------

void MainPageGenerator::pushHtmlTag(HtmlTag tag) {
	html_ = html_ + tag.generateStartTag();
	stack_.push(tag);
}

//----< add <style> tag >--------------------------------------------------------------

void MainPageGenerator::popHtmlTag() {
	if (stack_.size() > 0) {
		HtmlTag tag = stack_.top();
		html_ = html_ + tag.generateEndTag();
		stack_.pop();
	}
}

//----< add link to CSS file >--------------------------------------------------------

void MainPageGenerator::addCSS() {
	html_ = html_ + "\n<link rel = \"stylesheet\" type = \"text/css\" href = \"" + css_ + "\">";
}

//----< add <body> tag >--------------------------------------------------------------

void MainPageGenerator::addBody(Html attr) {
	HtmlTag html("body", attr);
	pushHtmlTag(html);
}

//----< add file name as heading >----------------------------------------------------

void MainPageGenerator::addHeading(File file, Html size, size_t indent, Html attr) {
	HtmlTag html(size, attr, indent);
	pushHtmlTag(html);
	html_ = html_ + file;
	popHtmlTag();
}

//----< add horizonal rule >----------------------------------------------------------

void MainPageGenerator::addHRule(size_t indent) {
	std::string ind(indent, ' ');
	html_ = html_ + "\n" + ind + "<hr />";
}

//----< add dependency block >--------------------------------------------------------

void MainPageGenerator::addDependency(size_t indent) {
	HtmlTag div("div", "class = \"indent\"", indent);
	pushHtmlTag(div);
	addHeading("Links:", "h4", indent + 2);
	if (deps_.size() == 0)
		addHeading("No page generated!", "h4", indent + 2, "class = \"nodep\"");
	for (auto dep : deps_) {
		addLink(dep, indent + 2);
		html_ = html_ + "\n  <br>";
	}
	popHtmlTag();
}

//----< add dependency links >--------------------------------------------------------

void MainPageGenerator::addLink(File dep, size_t indent) {
	HtmlTag link("a", "href = \"" + dep + "\"", indent);
	pushHtmlTag(link);
	html_ = html_ + dep;
	popHtmlTag();
}

//----< test stub >-------------------------------------------------------------------

#ifdef TEST_HTMLGENERATOR

#include <iostream>
#include "../Display/Display.h"

void testHtmlTag() {

	// Test Get() methods of HtmlTag
	makeTitle("Test HtmlTag:");
	makeSubTitle("Test Get() methods on HtmlTag without attribute:");
	HtmlTag html("html");
	std::cout << "\n\n  Tag Name: " << html.getName();
	std::cout << "\n Attribute: " << html.getAttribute() << std::endl;

	makeSubTitle("Test Get() methods on HtmlTag with attribute:");
	HtmlTag htmlWithAttr("html", "lang=\"en-US\"");
	std::cout << "\n  Tag Name: " << htmlWithAttr.getName();
	std::cout << "\n Attribute: " << htmlWithAttr.getAttribute() << std::endl;

	// Test tag generator methods of HtmlTag
	makeTitle("Test tag generator methods:");
	makeSubTitle("Test tag generator methods on HtmlTag without attribute:");
	std::cout << "\n\n  Start Tag: " << html.generateStartTag();
	std::cout << "\n    End Tag: " << html.generateEndTag() << std::endl;

	makeSubTitle("Test tag generator methods on HtmlTag with attribute:");
	std::cout << "\n  Start Tag: " << htmlWithAttr.generateStartTag();
	std::cout << "\n    End Tag: " << htmlWithAttr.generateEndTag() << std::endl;
}

void testHtmlGenerator() {
	makeTitle("Test HtmlGenerator:");
	std::set<std::string> deps{ "Test.cpp", "Test1.h" };
	HtmlGenerator hg("Test.h", "Test {}", deps);
	hg.generateHtml();
	std::cout << hg.getHtml() << std::endl;
}

void testMainPageGenerator() {
	makeTitle("Test HtmlGenerator:");
	std::vector<std::string> deps{ "Test.cpp", "Test1.h" };
	MainPageGenerator mpg("Test.h", deps);
	mpg.generateHtml();
	std::cout << mpg.getHtml() << std::endl;
}

int main() {
	testHtmlTag();
	testHtmlGenerator();
	testMainPageGenerator();
}


#endif // TEST_HTMLGENERATOR
