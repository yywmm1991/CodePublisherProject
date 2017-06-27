#pragma once
/////////////////////////////////////////////////////////////////////
// ChannelParts.h - Provides necessary parts for communication     //
//                  channel                                        //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #4         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations
* ====================
*  - This package contains functionalities of sending and recving for 
*    communication channel
*  - Sendr: Provide sending functionality for channel
*           Deal with sending file tasks
*  - Recvr: Provide reciving functionality for channel
*           Deal with recving file tasks
*
*  Public Interface
* ==================
*  Sendr sendr                                    create a Sendr instance
*  sendr.sendMessage()                            send HttpMessage based on message type
* 
*  Recvr recvr(BQueue&)                           create a recvr instance
*  sl.start(recvr)                                begin to receive message 
*  
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - ChannelParts.h, ChannelParts.cpp, HttpMessage.h,             //
//    HttpMessage.cpp, Sockets.h, Sockets.cpp,                     //
//    Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp               //
//    FileSystem.h, FileSystem.cpp, Utilities.h, Utilities.cpp     //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_CHANNELPARTS in properties. //
//    Set ChannelParts as the startup project.                     //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv ChannelParts.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 1 May 17
*  - first release
*/

#include "../Sockets/Sockets.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../HttpMessage/HttpMessage.h"

namespace Channel {

	///////////////////////////////////////////////////////////////////////////
	// Sendr:
	// - provide functionalities to send messages through channel
	// - create http messages

	class Sendr
	{

	public:

		using Msg = HttpMessage;
		using File = std::string;
		using Path = std::string;

		Sendr() {}                               // default constructor
		~Sendr() {}                              // destructor
		void sendMessage(Msg msg, Socket& sc);   // send message through SocketConnecter

	private:

		int getMsgType(Msg msg);                       // based on contents of message to choose sending method
		bool downWebFiles(Msg msg, Socket& sc);        // send request to download web files
		bool uploadFile(Msg msg, Socket& sc);          // send request to upload source file
		bool browseRepo(Msg msg, Socket& sc);          // send request to browse repository
		bool directSendMessage(Msg msg, Socket& sc);   // directly send request to server without other operations
		File getFileName(Path filePath);               // extract file name from local file path

	};


	///////////////////////////////////////////////////////////////////////////
	// Recvr:
	// - Provide functionalities to receive messages from channel
	// - evoke SendrHandler to handle messages

	class Recvr
	{

	public:

		using Msg = HttpMessage;
		using BQueue = Async::BlockingQueue<Msg>;
		using Path = std::string;
		using File = std::string;

		Recvr(BQueue& recvQ);                                // promotion constructor
		~Recvr() { pRecvQ = nullptr; }                       // destructor
		void operator()(Socket socket);                      // reload operator 
		void shutdownRecvr() { connectionClosed_ = true; }   // stop recving messages

	private:
		
		int getMsgType(Msg msg);                             // based on contents of message to choose sending method
		Msg readMessage(Socket& socket);

		void handleRequest(Msg msg, Socket& socket);          // handle request from recvr
		bool handleUploadFile(Msg msg, Socket& socket);       // handle request to upload source file
		bool handleDisplayResponse(Msg msg, Socket& socket);  // handle display response 
		bool handleDownloadResponse(Msg msg, Socket& socket); // handle download response 
		Path findFilePath(File file);                         // find out the path of source code in server's repository

		Path serverRepo_ = "../Repository";
		Path impleFolder_ = "/CppFiles/";
		Path headerFolder_ = "/HeaderFiles/";
		Path client_ = "../Client ";

		bool connectionClosed_;
		BQueue* pRecvQ;
	};

}