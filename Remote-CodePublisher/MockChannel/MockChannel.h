#ifndef MOCKCHANNEL_H
#define MOCKCHANNEL_H
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
/*
*  Package Operations
* ====================
*  - This package contains one interface: IMockChannel
*  - IMockChannel: Read from sendQ and write to recvQ
*  - Communicate between GUI and Client instance
*
*  Public Interface
* ==================
*  Application^ app = gcnew Application()                         create an Application object
*  app->Run(gcnew ClientGUI(array<System::String^>^ args))        start a ClientGUI
*
/////////////////////////////////////////////////////////////////////
// Build Process                                                   //
/////////////////////////////////////////////////////////////////////
//  Required Files:                                                //
//  - HttpMessage.h, HttpMessage.cpp                               //
//    MockChannel.h, MockChannel.cpp                               //
//                                                                 //
//  Build Process:                                                 //
//    Set MockChannel as the startup project.                      //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv MockChannel.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 1 May 17
*  - first release
*/
#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif

#include <string>
#include"../HttpMessage/HttpMessage.h"

using Message = HttpMessage;

struct IMockChannel
{
public:
  virtual void postMessage(HttpMessage& msg) = 0;
  virtual HttpMessage getMessage() = 0;
  virtual void start() = 0;
  virtual void stop() = 0;
};

extern "C" {
  struct ObjectFactory
  {
    DLL_DECL IMockChannel* createMockChannel(std::string ip, size_t port);
  };
}

#endif


