#ifndef SERVER_H
#define SERVER_H
/////////////////////////////////////////////////////////////////////
// Server.h - Create server for communication channel              //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #4         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations
* ====================
*  - This package contains one class: Server
*  - Server: Provide Server functionality for communication channel
*  - Use separate threads to deal with sending and receiving
*  - Process received messages and perform corresponding operations
*
*  Public Interface
* ==================
*  Server server(IP, size_t)                       create a Server instance
*  server.startSending()                           start sending thread
*  server.startRecving()                           start recving thread
*  server.ProcessRequest()                         use main thread to keep processing received messages
*  server.testExecutive()                          demonstrate requirements
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - Server.h, Server.cpp,                                        //
//    ChannelParts.h, ChannelParts.cpp, HttpMessage.h,             //
//    HttpMessage.cpp, Sockets.h, Sockets.cpp,                     //
//    Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp               //
//    FileSystem.h, FileSystem.cpp, Utilities.h, Utilities.cpp     //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_SERVER in properties.       //
//    Set Server as the startup project.                           //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv Server.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 1 May 17
*  - first release
*/

#include "../ChannelParts/ChannelParts.h"
#include "../TypeTable/TypeTable.h"
#include "../TypeAnalysis/TypeAnalyzer.h"
#include "../DependencyAnalysis/DependencyAnalyzer.h"
#include "../HtmlFileMgr/HtmlFileMgr.h"
#include "../HtmlGenerator/HtmlGenerator.h"
#include "../LineCounter/LineCounter.h"
#include "../LocalPublisher/LocalPublisher.h"

#include <unordered_map>

namespace CodePublisher
{
	using namespace Channel;

	///////////////////////////////////////////////////////////////////////////
	// Server:
	// - provide functionalities for server of communication channel
	// - receive request from clients
	// - response back to clients

	class Server
	{

	public:

		using Msg = HttpMessage;
		using BQueue = Async::BlockingQueue<Msg>;
		using EndPoint = std::string; 
		using IP = std::string;
		using EPInfo = std::vector<std::string>;
		using Repo = std::string;
		using Path = std::string;
		using WebFiles = std::vector<std::string>;
		using Category = std::string;
		using Categories = std::vector<Category>;
		using Map = std::unordered_map<std::string, std::vector<std::string>>;
		using Dep = std::unordered_map<std::string, std::set<std::string>>;
		using Set = std::set<std::string>;


		Server(const IP& ip = "localhost", size_t port = 8080);     // promotion constructor
		~Server();                                                  // destructor

		void startSending();                                        // start sending thread
		void startRecving();                                        // start receiving thread
		void ProcessRequest();                                      // process received Http request

		void testServer();                                          // test Server class
		void testExecutive();                                       // provide desmonstration for all requirements

	private:

		EPInfo cvtEndPoint(std::string ep);                         // convert string EP into ip and port
		void execSendr();                                           // execute sending tasks
		void execRecvr();                                           // execute recving tasks
		bool connecting(SocketConnecter& si, const EndPoint& ep);   // connect to server through socket
		int getMsgType(Msg msg);                                    // find out message type
		Categories getCategories();                                 // obtain all categories under repository
		Map getCategoryMap();                                       // obtain all categories and their files as map
		std::string makeMapString(Map cMap);                        // convert map contents to string
		void createClientFolder(Msg req);                           // initialize folders for client
		bool publishWebPages(int argc, char* argv[]);               // publish html files
		void findFileSet(std::string file, Set& fileSet);           // utility function to obtain dependency set
		std::string findFileName(std::string filePath);             // get pure file name from file path string
		std::string changePath(std::string filePath);               // source code path to corresponding html path

		bool handleDownloadWebFiles(Msg req);                       // handle request to download css and js files
		bool handleUploadAck(Msg req);                              // handle request to send response to client that file successfully uploaded
		bool handleBrowseRepo(Msg req);                             // handle request to browse repository
		bool handleDisplayPage(Msg req);                            // handle request to download web pages
		bool handleDeleteFile(Msg req);                             // handle request to delete selected file
		bool handlePublish(Msg req);                                // handle request to publish html files

		void testRequirement2();                                  // test code for Requirement #2
		void testRequirement3();                                  // test code for Requirement #3
		void testRequirement4();                                  // test code for Requirement #4
		void testRequirement5();                                  // test code for Requirement #5
		void testRequirement6();                                  // test code for Requirement #6
		void testRequirement7();                                  // test code for Requirement #7
		void testRequirement8();                                  // test code for Requirement #8
		void testRequirement9();                                  // test code for Requirement #9
		void testRequirement10();                                 // test code for Requirement #10
		void testRequirement11();                                 // test code for Requirement #11
		 
		
		LocalPublisher* pPublisher;
		Dep dep_;
		Sendr *pSendr;
		Recvr *pRecvr;
		BQueue sendQ_;
		BQueue recvQ_;
		IP ip_;
		size_t port_;
		Repo repository_ = "../Repository";
		Path impleFolder_ = "/CppFiles";
		Path headerFolder_ = "/HeaderFiles";
		Path htmlFolder_ = "/HtmlFiles";
		Path webFileFolder_ = "/WebFiles";
		WebFiles webFiles_;

	};
}

#endif // !SERVER_H
