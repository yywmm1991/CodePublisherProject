/////////////////////////////////////////////////////////////////////
// ChannelParts.h - Provides necessary parts for communication     //
//                  channel                                        //
//                                                                 //
// Application: CSE687 - Object Oriented Design Project #4         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Win 10, Visual Studio 2015                         //
// Author:      Yuewen Yue, yuyue@syr.edu                          //
/////////////////////////////////////////////////////////////////////

#include "ChannelParts.h"
#include "../Utilities/Utilities.h"
#include "../FileSystem/FileSystem.h"

using namespace Channel;

///////////////////////////////////////////////////////////////////////////
// Sendr class

//----< send message through socket >-------------------------------------------------

void Sendr::sendMessage(Msg msg, Socket& sc)
{
	int msgType = getMsgType(msg);
	switch (msgType)
	{
		// send request to download web files
		case 1:
			downWebFiles(msg, sc);
			break;
		// send request to upload source file
		case 2:
			uploadFile(msg, sc);
			break;
		// send request to browse repository
		case 3:
			browseRepo(msg, sc);
			break;
		// send response
		default:
			directSendMessage(msg, sc);
	}
}

//----< find out message type >-------------------------------------------------------

int Sendr::getMsgType(Msg msg)
{
	// If request to download css and js files as initialization
	if (msg.findValue("Command") == "DOWNLOAD_WEBFILES")
		return 1;
	// If request to upload source code to server
	if (msg.findValue("Command") == "UPLOAD_FILE" || msg.findValue("Command") == "DOWNLOAD_RESPONSE" || msg.findValue("Command") == "DISPLAY_RESPONSE")
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
	// If send out a response
	if (msg.findValue("Command") == "RESPONSE")
		return 6;
	// If send out a repository browse response
	if (msg.findValue("Command") == "BROWSE_RESPONSE")
		return 7;
	// If send out request to delete selected file
	if (msg.findValue("Command") == "DELETE_FILE")
		return 8;
	// If send out a display response
	if (msg.findValue("Command") == "DISPLAY_RESPONSE")
		return 9;
	// If send out a begin display response
	if (msg.findValue("Command") == "BEGIN_DISPLAY")
		return 9;
	// If send out a finish upload message
	if (msg.findValue("Command") == "FINISH_UPLOAD")
		return 10;
	return 0;
}

//----< send request to download web files >------------------------------------------

bool Sendr::downWebFiles(Msg msg, Socket& sc)
{
	std::string msgString = msg.toString();
	//sc.send(msgString.size(), (Socket::byte*)msgString.c_str());
	sc.sendString(msgString);
	return true;
}

//----< send request to upload source file >------------------------------------------

bool Sendr::uploadFile(Msg msg, Socket& sc)
{
	std::string filePath = msg.findValue("FilePath");
	FileSystem::FileInfo fi(filePath);
	size_t fileSize = fi.size();
	std::string sizeString = Utilities::Converter<size_t>::toString(fileSize);
	FileSystem::File file(filePath);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	// Send out the request message
	msg.removeAttribute("FilePath");
	msg.addAttribute(HttpMessage::attribute("content-length", sizeString));
	msg.addAttribute(HttpMessage::attribute("FileName", getFileName(filePath)));
	std::string msgString = msg.toString();
	sc.send(msgString.size(), (Socket::byte*)msgString.c_str());	
	//sc.sendString(msgString);

	// Send out file
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		sc.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}

//----< send request to browse server repository >------------------------------------

bool Sendr::browseRepo(Msg msg, Socket& sc)
{
	std::string msgString = msg.toString();
	sc.send(msgString.size(), (Socket::byte*)msgString.c_str());
	return true; 
}

//----< directly send request to server >---------------------------------------------

bool Sendr::directSendMessage(Msg msg, Socket& sc)
{
	std::string msgString = msg.toString();
	sc.send(msgString.size(), (Socket::byte*)msgString.c_str());
	return true;
}

//----< extract file name from local file path >--------------------------------------

Sendr::File Sendr::getFileName(Path filePath)
{
	size_t found = filePath.find_last_of("/\\");
	if (found != std::string::npos)
		return filePath.substr(found + 1);
	return filePath;
}

///////////////////////////////////////////////////////////////////////////
// Recvr class

//----< promotion constructor for Recvr class >---------------------------------------

Recvr::Recvr(BQueue& recvQ)
{
	pRecvQ = &recvQ;
}

//----< reload operator for Recvr >---------------------------------------------------

void Recvr::operator()(Socket socket)
{
	while (true) {
		Msg recvMsg = readMessage(socket);
		if (connectionClosed_)
		{
			std::cout << "\n\n  Reciver thread is terminating \n";
			break;
		}
		handleRequest(recvMsg, socket);
		Sleep(50);
	}

	std::cout << "\n\n  Reciver socket connection closing \n";
	socket.shutDown();
	socket.close();
	std::cout << "\n  Reciver thread terminating\n";
}

//----< read message from socket >----------------------------------------------------

Recvr::Msg Recvr::readMessage(Socket& socket)
{
	connectionClosed_ = false;
	HttpMessage msg;
	while (true)
	{
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1)
		{
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else
		{
			break;
		}
	}
	if (msg.findValue("Command") == "SHUTDOWN_CLIENT")
		connectionClosed_ = true;
	return msg;
}

//----< find out message type >-------------------------------------------------------

int Recvr::getMsgType(Msg msg)
{
	// If request to download css and js files as initialization
	if (msg.findValue("Command") == "DOWNLOAD_WEBFILES")
		return 1;
	// If request to upload source code to server
	if (msg.findValue("Command") == "UPLOAD_FILE")
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
	// If receive a response
	if (msg.findValue("Command") == "RESPONSE")
		return 6;
	// If receive a repository browse response
	if (msg.findValue("Command") == "BROWSE_RESPONSE")
		return 7;
	// If receive request to delete selected file
	if (msg.findValue("Command") == "DELETE_FILE")
		return 8;
	// If receive a display response
	if (msg.findValue("Command") == "DISPLAY_RESPONSE")
		return 9;
	// If receive a download response response
	if (msg.findValue("Command") == "DOWNLOAD_RESPONSE")
		return 10;
	// If receive a begin display response
	if (msg.findValue("Command") == "BEGIN_DISPLAY")
		return 11;
	// If receive a finish upload message
	if (msg.findValue("Command") == "FINISH_UPLOAD")
		return 12;
	return 0;
}

//----< handle request from recvr >---------------------------------------------------

void Recvr::handleRequest(Msg msg, Socket& socket)
{
	int msgType = getMsgType(msg);
	switch (msgType)
	{
		// handle request to upload source file
		case 2:
			handleUploadFile(msg, socket);
			break;
		// handle display response
		case 9:
			handleDisplayResponse(msg, socket);
			break;
		// handle download response
		case 10:
			handleDownloadResponse(msg, socket);
			break;
		// handle other request by directly enque messages
		default:
			pRecvQ->enQ(msg);
			break;
	}
}

//----< handle request to upload source file >----------------------------------------

bool Recvr::handleUploadFile(Msg msg, Socket& socket)
{
	size_t fileSize = Utilities::Converter<size_t>::toValue(msg.findValue("content-length"));
	std::string fileName = msg.findValue("FileName");
	Path filePath = findFilePath(fileName);
	FileSystem::File file(filePath);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		std::cout << "\n\n  can't open file " + fileName;
		return false;
	}
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];

	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	// Enqueue response message
	msg.removeAttribute("Command");
	msg.addAttribute(HttpMessage::attribute("Command", "UPLOAD_ACK"));
	pRecvQ->enQ(msg);

	return true;
}

//----< handle display response >-----------------------------------------------------

bool Recvr::handleDisplayResponse(Msg msg, Socket& socket)
{
	size_t fileSize = Utilities::Converter<size_t>::toValue(msg.findValue("content-length"));
	std::string fileName = msg.findValue("FileName");
	size_t found = msg.findValue("ToAddr").find(":");
	Path filePath = client_ + msg.findValue("ToAddr").erase(found, 1) + "/" + fileName;
	filePath = FileSystem::Path::getFullFileSpec(filePath);
	FileSystem::File file(filePath);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		std::cout << "\n\n  can't open file " + fileName;
		return false;
	}

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];

	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}

//----< handle download response >----------------------------------------------------

bool Recvr::handleDownloadResponse(Msg msg, Socket& socket)
{
	size_t fileSize = Utilities::Converter<size_t>::toValue(msg.findValue("content-length"));
	std::string fileName = msg.findValue("FileName");
	size_t found = msg.findValue("ToAddr").find(":");
	Path filePath = client_ + msg.findValue("ToAddr").erase(found, 1) + "/" + "WebFiles/" + fileName;
	filePath = FileSystem::Path::getFullFileSpec(filePath);
	FileSystem::File file(filePath);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		std::cout << "\n\n  can't open file " + fileName;
		return false;
	}

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];

	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}

//----< find path for source file >---------------------------------------------------

Recvr::Path Recvr::findFilePath(File file)
{
	size_t found = file.find_last_of('.');
	if (found != std::string::npos)
	{
		std::string post = file.substr(found + 1);
		if (post == "h")
			return serverRepo_ + headerFolder_ + file;
		if (post == "cpp")
			return serverRepo_ + impleFolder_ + file;
		else
			return serverRepo_;
	}
	return serverRepo_;
}


//----< test stub >-------------------------------------------------------------------

#ifdef TEST_CHANNELPARTS

using namespace Utilities;

void testParsingMessages()
{
	StringHelper::Title("Test Parse Messages", '-');

	// Construct HttpMessage
	HttpMessage msg;
	msg.addAttribute(HttpMessage::attribute("Command", "GetFiles"));
	msg.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
	msg.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8081"));
	msg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::attribute("content_length", "10"));
	std::string body = "Test Body";
	msg.addBody(body);
	std::cout << "Original Messages:\n" << msg.toString();
	putline();

	// Reconstruct HttpMessage
	HttpMessage newMsg = Recvr::parseMessage(msg.toString());
	std::cout << "Reconstructed Command:" << newMsg.findValue("Command") << std::endl;
	std::cout << "Reconstructed ToAddr:" << newMsg.findValue("ToAddr") << std::endl;
	std::cout << "Reconstructed Body:" << newMsg.bodyString() << std::endl;
}

HttpMessage makeTestMessage()
{
	HttpMessage msg;
	msg.addAttribute(HttpMessage::attribute("Command", "POST"));
	msg.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
	msg.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8081"));
	msg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::attribute("content_length", "10"));
	std::string body = "Test Body";
	msg.addBody(body);
	return msg;
}

HttpMessage quitSendrMessage()
{
	HttpMessage msg;
	msg.addAttribute(HttpMessage::attribute("Command", "STOP_SENDING"));
	return msg;
}

HttpMessage quitRecvrMessage()
{
	HttpMessage msg;
	msg.addAttribute(HttpMessage::attribute("Command", "STOP_RECIVING"));
	return msg;
}

void executeChannel()
{
	Async::BlockingQueue<HttpMessage> sendrQ;
	Async::BlockingQueue<HttpMessage> recvrQ;
	try {
		SocketSystem ss;
		SocketConnecter si;
		SocketListener sl(9090, Socket::IP6);
		Recvr recvr(recvrQ);
		sl.start(recvr);

		while (!si.connect("localhost", 9090))
		{
			std::cout << "\n-----------client waiting to connect\n";
			::Sleep(100);
		}

		Sendr sendr();
		sendrQ.enQ(makeTestMessage());
		sendrQ.enQ(makeTestMessage());
		sendrQ.enQ(makeTestMessage());
		si.shutDownSend();
	}
	catch (std::exception& exc)
	{
		std::cout << "\n  Exeception caught: ";;
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		std::cout << exMsg;
	}
}

int main() {
	
	testParsingMessages();
	executeChannel();
}
#endif // !TEST_CHANNELPARTS