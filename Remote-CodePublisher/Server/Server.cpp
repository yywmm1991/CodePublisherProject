/////////////////////////////////////////////////////////////////////
// Server.h - Create server for communication channel              //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #4         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////

#include "Server.h"
#include "../Utilities/Utilities.h"
#include "../FileSystem/FileSystem.h"
#include "../Display/Display.h"

#include <string>
#include <windows.h>
#include <ShellApi.h>


using namespace CodePublisher;

//----< promotion constructor of Server >---------------------------------------------

Server::Server(const IP& ip, size_t port)
	:ip_(ip),
	 port_(port)
{
	pRecvr = new Recvr(recvQ_);
	pSendr = new Sendr();
	webFiles_.push_back(repository_ + webFileFolder_ + "/style_sheet.css");
	webFiles_.push_back(repository_ + webFileFolder_ + "/script.js");
}

//----< destructor >------------------------------------------------------------------

Server::~Server()
{
	delete pSendr;
	delete pRecvr;
	delete pPublisher;
}

//----< start sending thread >--------------------------------------------------------

void Server::startSending()
{
	std::thread sendThread(&Server::execSendr, this);
	sendThread.detach();
	std::cout << "\n--- Server send thread started!\n";
}

//----< start recving thread >--------------------------------------------------------

void Server::startRecving()
{
	std::thread recvThread(&Server::execRecvr, this);
	recvThread.detach();
	std::cout << "\n--- Server receive thread started!\n";
}

//----< start recving thread >--------------------------------------------------------

void Server::ProcessRequest()
{
	std::cout << "\n-----< Server starts processing requests >-----\n";
	while (true)
	{
		Msg req = recvQ_.deQ();
		int reqType = getMsgType(req);
		switch (reqType)
		{
			// Handle request to download css and js files
		case 1:
			handleDownloadWebFiles(req);
			break;
			// Handle response message to client that file uploaded successfully
		case 2:
			handleUploadAck(req);
			break;
			// Handle request to browse repository
		case 3:
			handleBrowseRepo(req);
			break;
			// Handle request to display web page
		case 4:
			handleDisplayPage(req);
			break;
			// Handle request to delete file
		case 6:
			handleDeleteFile(req);
			break;
			// Handle request to publish html files
		case 7:
			handlePublish(req);
			break;
		default:
			break;
		}
	}
}

//----< convert End Point information >-----------------------------------------------

Server::EPInfo Server::cvtEndPoint(std::string ep)
{
	EPInfo epInfo;
	size_t found = ep.find(':');
	epInfo.push_back(ep.substr(0, found));
	epInfo.push_back(ep.substr(found + 1, std::string::npos));
	return epInfo;
}

//----< execute sending tasks >-------------------------------------------------------

void Server::execSendr()
{
	try {
		SocketSystem ss;
		SocketConnecter si;
		while (true) {
			HttpMessage sendMsg = sendQ_.deQ();
			if (sendMsg.findValue("Command") == "SHUTDOWN_CLIENT")
				break;
			if (connecting(si, sendMsg.findValue("ToAddr")))
				pSendr->sendMessage(sendMsg, si);
			si.shutDown();
		}
	}
	catch (std::exception& e) {
		std::cout << " \n--- Exception caught:";
		std::cout << e.what() << "\n\n";
	}
}

//----< execute recving tasks >-------------------------------------------------------

void Server::execRecvr()
{
	try {
		SocketSystem ss;
		SocketListener sl(port_, Socket::IP6);
		sl.start(*pRecvr);
		std::cout.flush();
		std::cin.get();
	}
	catch (std::exception& e) {
		std::cout << "\n Exception Caught:";
		std::cout << e.what() << std::endl;
	}
}

//----< connect to server through socket >--------------------------------------------

bool Server::connecting(SocketConnecter& si, const EndPoint& ep)
{
	EPInfo temp = cvtEndPoint(ep);
	IP destIP = temp[0];
	size_t destPort = Utilities::Converter<size_t>::toValue(temp[1]);

	while (!si.connect(destIP, destPort)) {
		std::cout << "\n--- Server waiting to connect.....\n";
		std::cout << destIP;
		std::cout << destPort;
		::Sleep(100);
	}
	std::cout << "\n--- Server connected on port number : " << std::to_string(destPort) << std::endl;
	return true;
}

//----< find out message type >-------------------------------------------------------

int Server::getMsgType(Msg msg)
{
	// If request to download css and js files as initialization
	if (msg.findValue("Command") == "DOWNLOAD_WEBFILES")
		return 1;
	// If request to upload source code to server
	if (msg.findValue("Command") == "UPLOAD_ACK")
		return 2;
	// If request to browse server's repository
	if (msg.findValue("Command") == "BROWSE_REPO")
		return 3;
	// If request to download web pages from server
	if (msg.findValue("Command") == "DISPLAY_PAGE")
		return 4;
	// If request to shutdown server
	if (msg.findValue("Command") == "SHUTDOWN_SERVER")
		return 5;
	// If request to delete selected file
	if (msg.findValue("Command") == "DELETE_FILE")
		return 6;
	// If received message to inform end of uploading
	if (msg.findValue("Command") == "FINISH_UPLOAD")
		return 7;
	return 0;
}

//----< obtain all categories under repository >--------------------------------------

Server::Categories Server::getCategories()
{
	Categories dirs = FileSystem::Directory::getDirectories(repository_, "*");
	Categories categories;
	for (auto cate : dirs)
	{
		if (cate[0] != '.' && cate != "WebFiles")
			categories.push_back(cate);
	}
	return categories;
}

//----< obtain all categories and their files as map >--------------------------------

Server::Map Server::getCategoryMap()
{
	Map categoryMap;
	Categories categories = getCategories();
	std::vector<std::string> patterns({ "*.h", "*.cpp", "*.htm" });
	for (auto cate : categories)
	{
		//std::cout << cate;
		Path path = FileSystem::Path::getFullFileSpec(repository_ + "/" + cate + "/");
		std::vector<std::string> files;
		categoryMap[cate] = std::vector<std::string>();
		if (!FileSystem::Directory::exists(path))
		{
			std::cout << "\n\n  path \"" << path << "\" does not exist\n\n";
			continue;
		}
		for (auto patt : patterns)
		{
			files = std::move(FileSystem::Directory::getFiles(path, patt));
			if (files.size() > 0)
			{
				categoryMap[cate] = files;
				break;
			}
		}
	}
	return categoryMap;
}

//----< convert map contents to string >----------------------------------------------

std::string Server::makeMapString(Map cMap)
{
	std::string mapStr = "";
	int count = 1;
	for (auto cate : cMap)
	{
		if (count == 1)
			mapStr = mapStr + cate.first;
		else
			mapStr = mapStr + "+" + cate.first;
		mapStr += "|";
		for (auto file : cate.second)
			mapStr = mapStr + file + ",";
		if (cate.second.size() > 0)
			mapStr = mapStr.substr(0, mapStr.size() - 1);
		++count;
	}
	return mapStr;
}

//----< initialize folders for client >-----------------------------------------------

void Server::createClientFolder(Msg req)
{
	// create client folder
	std::string clientFolder = "Client" + req.findValue("FromAddr");
	FileSystem::Directory::create(repository_ + "/" + clientFolder);
	// create source code folders under client folder
	FileSystem::Directory::create(repository_ + "/" + clientFolder + "/" + headerFolder_);
	FileSystem::Directory::create(repository_ + "/" + clientFolder + "/" + impleFolder_);
	// create web pages folder
	FileSystem::Directory::create(repository_ + "/" + clientFolder + "/" + htmlFolder_);
}

//----< publish html files to repository >--------------------------------------------

bool Server::publishWebPages(int argc, char* argv[])
{
	TypeTable table;
	pPublisher = new LocalPublisher(table);
	try {
		bool succeeded = pPublisher->ProcessCommandLine(argc, argv);
		if (!succeeded) { return false; }
		pPublisher->getSourceFiles();
		pPublisher->processSourceCode(true);
		pPublisher->complexityAnalysis();
		pPublisher->publish();
		std::cout << "\n-----< Finish Publish Message >-----\n";
		Dep temp = pPublisher->getDependencies();
		for (auto dep : temp)
		{
			std::set<std::string> vTemp;
			for (auto file : dep.second)
				vTemp.insert(changePath(file));
			dep_[changePath(dep.first)] = vTemp;
		}
		
	}
	catch (std::exception& except)
	{
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
		return false;
	}
	return true;
}

//----< utility function to obtain dependency set >-----------------------------------

void Server::findFileSet(std::string file, Set& fileSet)
{
	if (fileSet.find(file) != fileSet.end())
		return;
	fileSet.insert(file);
	for (auto dep : dep_[file])
		findFileSet(dep, fileSet);
}

//----< get pure file name from file path string >------------------------------------

std::string Server::findFileName(std::string filePath)
{
	size_t found = filePath.find_last_of("/\\");
	if (found != std::string::npos)
		return filePath.substr(found + 1);
	return filePath;
}

//----< source code path to corresponding html path >---------------------------------

std::string Server::changePath(std::string filePath)
{
	return FileSystem::Path::getFullFileSpec(repository_ + htmlFolder_ + "/" + findFileName(filePath) + ".htm");
}

//----< handle request to download css and js files >---------------------------------

bool Server::handleDownloadWebFiles(Msg req)
{
	//createClientFolder(req);
	for (auto file : webFiles_) {
		HttpMessage msg;
		msg.addAttribute(Msg::Attribute("Command", "DOWNLOAD_RESPONSE"));
		msg.addAttribute(Msg::Attribute("Mode", "OneWay"));
		msg.addAttribute(Msg::Attribute("ToAddr", req.findValue("FromAddr")));
		msg.addAttribute(Msg::Attribute("FromAddr", req.findValue("ToAddr")));
		FileSystem::FileInfo fi(file);
		size_t fileSize = fi.size();
		std::string sizeString = Utilities::Converter<size_t>::toString(fileSize);
		msg.addAttribute(Msg::Attribute("content-length", sizeString));
		msg.addAttribute(Msg::Attribute("FilePath", file));
		sendQ_.enQ(msg);
	}
	// send response
	HttpMessage msg;
	msg.addAttribute(Msg::Attribute("Command", "RESPONSE"));
	msg.addAttribute(Msg::Attribute("Mode", "OneWay"));
	msg.addAttribute(Msg::Attribute("ToAddr", req.findValue("FromAddr")));
	msg.addAttribute(Msg::Attribute("FromAddr", req.findValue("ToAddr")));
	std::string response = "CSS and JS files downloaded successfully!";
	msg.addAttribute(Msg::Attribute("Message", response));
	sendQ_.enQ(msg);
	return true;
}

//----< handle request send response for uploaded file >------------------------------

bool Server::handleUploadAck(Msg req)
{
	// Return response to client
	HttpMessage msg;
	msg.addAttribute(Msg::Attribute("Command", "RESPONSE"));
	msg.addAttribute(Msg::Attribute("Mode", "OneWay"));
	msg.addAttribute(Msg::Attribute("ToAddr", req.findValue("FromAddr")));
	msg.addAttribute(Msg::Attribute("FromAddr", req.findValue("ToAddr")));
	std::string response = "File " + req.findValue("FileName") + " uploaded successfully!";
	msg.addAttribute(Msg::Attribute("Message", response));
	sendQ_.enQ(msg);
	return true;
}

//----< handle request to browse repository >-----------------------------------------

bool Server::handleBrowseRepo(Msg req)
{
	Map categoryMap = getCategoryMap();
	HttpMessage msg;
	msg.addAttribute(Msg::Attribute("Command", "BROWSE_RESPONSE"));
	msg.addAttribute(Msg::Attribute("Mode", "OneWay"));
	msg.addAttribute(Msg::Attribute("ToAddr", req.findValue("FromAddr")));
	msg.addAttribute(Msg::Attribute("FromAddr", req.findValue("ToAddr")));
	std::string mapStr = makeMapString(categoryMap);
	//std::cout << "-----< Test MapStr >-----\n" << mapStr;
	msg.addAttribute(Msg::Attribute("Map", mapStr));
	std::string response = "Browse Repository Response!";
	msg.addAttribute(Msg::Attribute("Message", response));
	sendQ_.enQ(msg);
	return true;
}

//----< handle request to dispkay web pages >-----------------------------------------

bool Server::handleDisplayPage(Msg req)
{
	// FilePath format: FolderName/fileName
	std::set<std::string> fileSet;
	std::string requestFile = FileSystem::Path::getFullFileSpec(repository_ + "/" + req.findValue("FilePath"));
	fileSet.insert(requestFile);
	for (auto file : dep_[requestFile])
	{
		findFileSet(file, fileSet);
	}
	// Make response message
	for (auto file : fileSet) {
		HttpMessage msg;
		msg.addAttribute(Msg::Attribute("Command", "DISPLAY_RESPONSE"));
		msg.addAttribute(Msg::Attribute("Mode", "OneWay"));
		msg.addAttribute(Msg::Attribute("ToAddr", req.findValue("FromAddr")));
		msg.addAttribute(Msg::Attribute("FromAddr", req.findValue("ToAddr")));
		FileSystem::FileInfo fi(file);
		size_t fileSize = fi.size();
		std::string sizeString = Utilities::Converter<size_t>::toString(fileSize);
		msg.addAttribute(Msg::Attribute("content-length", sizeString));
		msg.addAttribute(Msg::Attribute("FilePath", file));
		sendQ_.enQ(msg);
	}
	// Make message to inform displaying
	HttpMessage msg;
	msg.addAttribute(Msg::Attribute("Command", "BEGIN_DISPLAY"));
	msg.addAttribute(Msg::Attribute("Mode", "OneWay"));
	msg.addAttribute(Msg::Attribute("ToAddr", req.findValue("FromAddr")));
	msg.addAttribute(Msg::Attribute("FromAddr", req.findValue("ToAddr")));
	std::string fileName = findFileName(req.findValue("FilePath"));
	msg.addAttribute(Msg::Attribute("FileName", fileName));
	std::string response = "Web pages downloaded successfully!";
	msg.addAttribute(Msg::Attribute("Message", response));
	sendQ_.enQ(msg);
	return true;
}

//----< handle request to delete selected file >--------------------------------------

bool Server::handleDeleteFile(Msg req)
{
	// FilePath format: FolderName/fileName
	std::string filePath = repository_ + "/" + req.findValue("FilePath");
	if (FileSystem::File::exists(filePath))
		FileSystem::File::remove(filePath);

	// Send response message
	HttpMessage msg;
	msg.addAttribute(Msg::Attribute("Command", "RESPONSE"));
	msg.addAttribute(Msg::Attribute("Mode", "OneWay"));
	msg.addAttribute(Msg::Attribute("ToAddr", req.findValue("FromAddr")));
	msg.addAttribute(Msg::Attribute("FromAddr", req.findValue("ToAddr")));
	std::string response = "File deleted successfully!";
	msg.addAttribute(Msg::Attribute("Message", response));
	sendQ_.enQ(msg);
	return true;
}

//----< handle request to publish html files >----------------------------------------

bool Server::handlePublish(Msg req)
{
	int argc = 4;
	char* argv[] = { "", "../Repository", "*.h", "*.cpp" };
	publishWebPages(argc, argv);
	return true;
}

//----< test class Server >-----------------------------------------------------------

void Server::testServer()
{
	std::string fpath = "../Client localhost9090/HtmlGenerator.cpp.htm";
	std::string command("start \"\" \"" + fpath + "\"");
	std::system(command.c_str());

}

//----< provide demonstration for requirements >--------------------------------------

void Server::testExecutive()
{
	testRequirement2();
	testRequirement3();
	testRequirement4();
	testRequirement5();
	testRequirement6();
	testRequirement7();
	testRequirement8();
	testRequirement9();
	testRequirement10();
	testRequirement11();
}

//----< test Requirement #2 >---------------------------------------------------------

void Server::testRequirement2() 
{
	makeTitle("Demonstrate Requirement #2");
	makeComment("This project only use C++ standard library's stream for all I/O.");
	makeComment("All Heap-based memory are approriately managed.");
	std::cout << std::endl;
}

//----< test Requirement #3 >---------------------------------------------------------

void Server::testRequirement3() 
{
	makeTitle("Demonstrate Requirement #3");
	makeComment("All Repository Publishing Functionality are provided by Server class.");
	makeComment("Publishing is performed after every uploading operation automatically.");
	std::cout << std::endl;
}

//----< test Requirement #4 >---------------------------------------------------------

void Server::testRequirement4() 
{
	makeTitle("Demonstrate Requirement #4");
	makeComment("The Publishing process satisfies all requirements of Project #3.");
	makeComment("All publishing functionalities are embedded under Server class.");
	makeComment("Publishing will automatically begin after each Uploading operation.");
	std::cout << std::endl;
}

//----< test Requirement #5 >---------------------------------------------------------

void Server::testRequirement5() 
{
	makeTitle("Demonstrate Requirement #5");
	makeComment("The ClientGUI provides functionalities as described in Purpose section.");
	makeComment("On starting, a client folder will be created under project's folder.");
	makeComment("And all CSS and JS files will be downloaded to client folder on starting.");
	makeComment("All uploaded files will be located in Repository folder under project's folder.");
	std::cout << std::endl;
}

//----< test Requirement #6 >---------------------------------------------------------

void Server::testRequirement6()
{
	makeTitle("Demonstrate Requirement #6");
	makeComment("This project implements a One-Way communication system based on Sockets.");
	makeComment("Clients access server's functionalities through such communication system.");
	std::cout << std::endl;
}

//----< test Requirement #7 >---------------------------------------------------------

void Server::testRequirement7()
{
	makeTitle("Demonstrate Requirement #7");
	makeComment("This project implements HTTP style request/response over One-Way communication system.");
	makeComment("HTTP requests and responses are displayed separately in Message tab of GUI.");
	std::cout << std::endl;
}

//----< test Requirement #8 >---------------------------------------------------------

void Server::testRequirement8()
{
	makeTitle("Demonstrate Requirement #8");
	makeComment("This project support sending and receiving streams of bytes.");
	makeComment("Sending and receiving byte streams are used to send and receive files in binary format.");
	std::cout << std::endl;
}

//----< test Requirement #9 >---------------------------------------------------------

void Server::testRequirement9()
{
	makeTitle("Demonstrate Requirement #9");
	makeComment("Requirements #2 -- #8 already demonstrated above");
	makeComment("Transmission of files can be demonstrated when uploading or downloading files through GUI.");
	std::cout << std::endl;
}

//----< test Requirement #10 >---------------------------------------------------------

void Server::testRequirement10()
{
	makeTitle("Demonstrate Requirement #10");
	makeComment("Lazy downloading will be performed when clicking Display button of GUI's Repository tab.");
	makeComment("Please make sure the RadioButton Lazy Download is checked before displaying.");
	makeComment("The downloaded html files will be located under client's folder.");
	std::cout << std::endl;
}

//----< test Requirement #11 >---------------------------------------------------------

void Server::testRequirement11()
{
	makeTitle("Demonstrate Requirement #11");
	makeComment("IIS will be performed when clicking Display button of GUI's Repository tab.");
	makeComment("Please make sure the RadioButton IIS is checked before displaying.");
	std::cout << std::endl;
	std::cout << std::endl;
}

//----< test stub >-------------------------------------------------------------------

#ifdef TEST_SERVER

int main()
{
	Server s1;
	//s1.testServer();
	s1.testExecutive();
	s1.startRecving();
	s1.startSending();
	s1.ProcessRequest();
}

#endif // !TEST_SERVER
