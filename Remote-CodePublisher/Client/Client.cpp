/////////////////////////////////////////////////////////////////////
// Client.h - Create client for communication channel              //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #4         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue,  yuyue@syr.edu                         //
/////////////////////////////////////////////////////////////////////

#include "Client.h"
#include "../Utilities/Utilities.h"

using namespace CodePublisher;

//----< promotion constructor of Client >---------------------------------------------

Client::Client(const IP& ip, size_t port)
	:ip_(ip),
	 port_(port)
{
	pRecvr = new Recvr(recvQ_);
	pSendr = new Sendr();
}

//----< destructor >------------------------------------------------------------------

Client::~Client()
{
	delete pSendr;
	delete pRecvr;
}

//----< process on command arguments >------------------------------------------------

bool Client::ProcessArguments(int argc, char ** argv)
{
	if (argc == 1)
		return true;
	if (argc > 1 && argv[1]) {
		std::string clientAddr = argv[1];
		EPInfo temp = cvtEndPoint(clientAddr);
		if (temp.size() == 2)
		{
			ip_ = temp[0];
			port_ = Utilities::Converter<size_t>::toValue(temp[1]);
		}
	}
	else {
		std::cout << "\n\n Incorrect Command Arguments! \n\n";
		return false;
	}
	return true;
}

//----< start sending thread >--------------------------------------------------------

void Client::startSending()
{
	std::thread sendThread(&Client::execSendr, this);
	sendThread.detach();
	std::cout << "\n--- Client send thread started!\n";
}

//----< start recving thread >--------------------------------------------------------

void Client::startRecving()
{
	std::thread recvThread(&Client::execRecvr, this);
	recvThread.detach();
	std::cout << "\n--- Client receive thread started!\n";
}

//----< post message to sendr >-------------------------------------------------------

void Client::postMessage(HttpMessage& msg)
{
	sendQ_.enQ(msg);
}

//----< get message from recvr >------------------------------------------------------

HttpMessage Client::getMessage()
{
	return recvQ_.deQ();
}

//----< convert End Point information >-----------------------------------------------

Client::EPInfo Client::cvtEndPoint(std::string ep)
{
	EPInfo epInfo;
	size_t found = ep.find(':');
	epInfo.push_back(ep.substr(0, found));
	epInfo.push_back(ep.substr(found + 1));
	return epInfo;
}

//----< execute sending tasks >-------------------------------------------------------

void Client::execSendr()
{
	try {
		SocketSystem ss;
		SocketConnecter si;
		while (true) {
			HttpMessage sendMsg = sendQ_.deQ();
			if (sendMsg.findValue("Command") == "SHUTDOWN_CLIENT")
				break;
			std::string toAddr = sendMsg.findValue("ToAddr");
			if (connecting(si, toAddr))
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

void Client::execRecvr()
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

bool Client::connecting(SocketConnecter& si, EndPoint ep)
{
	EPInfo temp = cvtEndPoint(ep);
	IP destIP = temp[0];
	size_t destPort = Utilities::Converter<size_t>::toValue(temp[1]);

	while (!si.connect(destIP, destPort)) {
		std::cout << "\n--- Client waiting to connect.....\n";
		std::cout << destIP;
		std::cout << destPort;
		::Sleep(100);
	}
	std::cout << "\n--- Client1 connected on port number : " << std::to_string(destPort) << std::endl;
	return true;
}

//----< test code for Client >--------------------------------------------------------

void Client::test()
{
	HttpMessage msg;
	startRecving();
	msg.addAttribute(HttpMessage::attribute("Command", "POST"));
	msg.addAttribute(HttpMessage::attribute("ToAddr", "localhost:9090"));
	msg.addAttribute(HttpMessage::attribute("FromAddr", "localhost:8080"));
	msg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::attribute("content_length", "10"));
	std::string body = "Test Body";
	msg.addBody(body);
	sendQ_.enQ(msg);
	startSending(); 
}

//----< test stub >-------------------------------------------------------------------

#ifdef TEST_CLIENT

int main(int argc, char* argv[])
{
	Client client;
	if (client.ProcessArguments(argc, argv))
	{
		client.test();
	} 
	return 0;
}
#endif // TEST_CLIENT


