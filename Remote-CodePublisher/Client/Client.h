#ifndef CLIENT_H
#define CLIENT_H
/////////////////////////////////////////////////////////////////////
// Client.h - Create client for communication channel              //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #4         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue,  yuyue@syr.edu                         //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations
* ====================
*  - This package contains one class: Client
*  - Client: Provide Client functionality for communication channel
*  - Use separate threads for sending and recving
*  - Communicate to GUI through MockChannel
*
*  Public Interface
* ==================
*  Client client(IP, size_t)                       create a Client instance
*  client.startSending()                           start sending thread
*  client.startRecving()                           start recving thread
*  client.postMessage()                            send message to server
*  client.getMessage()                             send message to GUI
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - Client.h, Client.cpp,                                        //             
//    ChannelParts.h, ChannelParts.cpp, HttpMessage.h,             //
//    HttpMessage.cpp, Sockets.h, Sockets.cpp,                     //
//    Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp               //
//    FileSystem.h, FileSystem.cpp, Utilities.h, Utilities.cpp     //
//                                                                 //
//  Build Process:                                                 //
//    Set preprocessor definition TEST_CLIENT in properties.       //
//    Set Client as the startup project.                           //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv Client.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 1 May 17
*  - first release
*/

#include "../ChannelParts/ChannelParts.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include <vector>

namespace CodePublisher 
{
	using namespace Channel;

	///////////////////////////////////////////////////////////////////////////
	// Client:
	// - provide functionalities for client of communication channel
	// - send messages to server
	// - receive response from server

	class Client
	{

	public:

		using BQueue = Async::BlockingQueue<HttpMessage>;
		using EndPoint = std::string;
		using IP = std::string;
		using EPInfo = std::vector<std::string>;

		Client(const IP& ip = "localhost", size_t port = 9090);     // promotion constructor
		~Client();                                                  // destructor

		bool ProcessArguments(int argc, char ** argv);              // process command arguments
		void startSending();                                        // start sending thread
		void startRecving();                                        // start receiving thread
		void postMessage(HttpMessage& msg);                         // post message to sendr
		HttpMessage getMessage();                                   // get message from recvr

		void test();                                                // test code for Client class

	private:

		EPInfo cvtEndPoint(std::string ep);                         // convert string EP into ip and port
		void execSendr();                                           // execute sending tasks
		void execRecvr();                                           // execute recving tasks
		bool connecting(SocketConnecter& si, EndPoint ep);          // connect to server through socket

		Sendr *pSendr;
		Recvr *pRecvr;
		BQueue sendQ_;
		BQueue recvQ_;
		IP ip_;
		size_t port_;
	};
}

#endif // CLIENT_H

