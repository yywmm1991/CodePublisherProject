/////////////////////////////////////////////////////////////////////
// MockChannel.h - Mock Channel for Project #4                     //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #4         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
// Reference:   Jim Fawcett, CST 4-187, Syracuse University        //
//              jfawcett@twcny.rr.com, (315) 443-3948              //
/////////////////////////////////////////////////////////////////////

#define IN_DLL
#include "MockChannel.h"
#include "Cpp11-BlockingQueue.h"
#include "../Client/Client.h"

#include <string>
#include <thread>
#include <iostream>

using BQueue = Async::BlockingQueue < Message >;

/////////////////////////////////////////////////////////////////////////////
// MockChannel class
// - reads messages from Sendr and writes messages to Recvr
//
class MockChannel : public IMockChannel
{
public:
	using IP = std::string;

	MockChannel(IP ip, size_t port);                                    // promotion constructor
	void start();
	void stop();
	void postMessage(HttpMessage& msg) { client_.postMessage(msg); }
	HttpMessage getMessage() { return client_.getMessage(); }
private:
	std::thread thread_;
	CodePublisher::Client client_;
	bool stop_ = false;
};

//----< promotion constructor for MockChannel >---------------------------------------

MockChannel::MockChannel(IP ip, size_t port)
	:client_(ip, port)
{}

//----< creates thread to read from sendQ and echo back to the recvQ >----------------

void MockChannel::start()
{
	std::cout << "\n  MockChannel starting up";
	client_.startSending();
	client_.startRecving();
}
//----< signal server thread to stop >---------------------------------------

void MockChannel::stop() { stop_ = true; }

//----< factory functions >--------------------------------------------------

//ISendr* ObjectFactory::createSendr() { return new Sendr; }

//IRecvr* ObjectFactory::createRecvr() { return new Recvr; }

IMockChannel* ObjectFactory::createMockChannel(std::string ip, size_t port)
{
	return new MockChannel(ip, port);
}

#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
  ObjectFactory objFact;
  ISendr* pSendr = objFact.createSendr();
  IRecvr* pRecvr = objFact.createRecvr();
  IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
  pMockChannel->start();
  pSendr->postMessage("Hello World");
  pSendr->postMessage("CSE687 - Object Oriented Design");
  Message msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  pSendr->postMessage("stopping");
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  pMockChannel->stop();
  pSendr->postMessage("quit");
  std::cin.get();
}
#endif
