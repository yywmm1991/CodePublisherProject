#ifndef WINDOW_H
#define WINDOW_H
/////////////////////////////////////////////////////////////////////
// Window.h - Client GUI for Project #4                            //
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
*  - This package contains one class: ClientGUI
*  - ClientGUI: Provide necessary functionality for Client GUI of Project #4
*  - Provide complete layout for ClientGUI
*  - Provide functionalities for interacting with server and communication channel
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
//  - Window.h, Window.cpp, HttpMessage.h, HttpMessage.cpp         //
//    MockChannel.h, MockChannel.cpp, FileSystem.h, FileSystem.cpp //
//    Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp               //
//                                                                 //
//  Build Process:                                                 //
//    Set CppCli-WPF-App as the startup project.                   //
//    Compile and run in Visual Studio.                            //
//                                                                 //
/////////////////////////////////////////////////////////////////////
*
*  Build Command
* ===============
*    devenv CppCli-WPF-App.sln /rebuild debug
*
*  Maintenance History
* =====================
*  ver 1.0 : 1 May 17
*  - first release
*/
#include "../FileSystem/FileSystem.h"

using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;             // Dictionary

#include "../MockChannel/MockChannel.h"
#include "../Utilities/Utilities.h"
#include <iostream>
#include <unordered_map>
#include <vector>

namespace CppCliWindows
{
  ref class ClientGUI : Window
  {
    // MockChannel references

    IMockChannel* pChann_;

	// Controls for Window
	DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
	Grid^ hGrid = gcnew Grid();
	TabControl^ hTabControl = gcnew TabControl();
	// Three tabs
	TabItem^ hSendMessageTab = gcnew TabItem();
	TabItem^ hLocalFileTab = gcnew TabItem();
	TabItem^ hRepoTab = gcnew TabItem();

	StatusBar^ hStatusBar = gcnew StatusBar();
	StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
	TextBlock^ hStatus = gcnew TextBlock();

	// Controls for SendMessage View
	Grid^ hSendMessageGrid = gcnew Grid();
	Button^ hClearButton = gcnew Button();
	//Button^ hShutdownClientButton = gcnew Button();
	Button^ hShutdownServerButton = gcnew Button();
	TextBlock^ hTextBlock1 = gcnew TextBlock();
	TextBlock^ hTextBlock2 = gcnew TextBlock();
	ScrollViewer^ hScrollViewer1 = gcnew ScrollViewer();
	ScrollViewer^ hScrollViewer2 = gcnew ScrollViewer();
	StackPanel^ hStackPanel1 = gcnew StackPanel();
	StackPanel^ hStackPanel11 = gcnew StackPanel();
	Label^ hMessageLabel = gcnew Label();
	Label^ hMessageLabel1 = gcnew Label();

	// Controls for local file View
	Grid^ hLocalFileGrid = gcnew Grid();
	Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
	ListBox^ hListBox = gcnew ListBox();
	Button^ hFolderBrowseButton = gcnew Button();
	Button^ hShowItemsButton = gcnew Button();
	Button^ hUploadButton = gcnew Button();
	CheckBox^ hHeaderBox = gcnew CheckBox();
	CheckBox^ hCppBox = gcnew CheckBox();
	StackPanel^ hStackPanel2 = gcnew StackPanel();
	StackPanel^ hStackPanel21 = gcnew StackPanel();

	// Controls for Repository View
	Grid^ hRepoGrid = gcnew Grid();
	ListBox^ hListBox1 = gcnew ListBox();
	ListBox^ hListBox2 = gcnew ListBox();
	Button^ hBrowseRepoButton = gcnew Button();
	Button^ hViewFileButton = gcnew Button();
	Button^ hDisplayButton = gcnew Button();
	Button^ hDeleteButton = gcnew Button();
	RadioButton^ hLazyDownload = gcnew RadioButton();
	RadioButton^ hIIS = gcnew RadioButton();
	StackPanel^ hStackPanel3 = gcnew StackPanel();
	StackPanel^ hStackPanel31 = gcnew StackPanel();
	StackPanel^ hStackPanel32 = gcnew StackPanel();
	StackPanel^ hStackPanel33 = gcnew StackPanel();

	// receive thread
	Thread^ recvThread;

  public:

	  using Map = std::unordered_map<std::string, std::vector<std::string>>;

	  ClientGUI(array<System::String^>^ args);
	  ~ClientGUI();

	  void setUpStatusBar();
	  void setUpTabControl();
	  void setUpSendMessageView();
	  void setUpLocalFileView();
	  void setUpRepoView();
	  void setUpLocalRepo();
	  void setButtonEvents();

	  void addText(String^ msg);
	  void addResponseText(String^ msg);
	  void UpdateListBox1(String^ msg);
	  void getMessage();
	  void clear(Object^ object, RoutedEventArgs^ args);
	  void getItemsFromList(Object^ object, RoutedEventArgs^ args);
	  void browseForFolder(Object^ object, RoutedEventArgs^ args);
	  void uploadFiles(Object^ object, RoutedEventArgs^ args);
	  void browseRepo(Object^ object, RoutedEventArgs^ args);
	  void viewFolder(Object^ object, RoutedEventArgs^ args);
	  void displayPage(Object^ object, RoutedEventArgs^ args);
	  void deletePage(Object^ object, RoutedEventArgs^ args);
	  void publishHtml(Object^ object, RoutedEventArgs^ args);
	  void OnLoaded(Object^ object, RoutedEventArgs^ args);
	  void Unloading(Object^ object, System::ComponentModel::CancelEventArgs^ args);

	  void shutdownServer(Object^ object, RoutedEventArgs^ args);

  private:

	  bool processCommandArgument(array<System::String^>^ args);
	  std::string toStdString(String^ pStr);
	  String^ toSystemString(std::string& str);
	  void setTextBlockProperties();
	  void setMessageButtonsProperties();
	  void setLocalFileButtonProperties();
	  void setUploadButtonProperties();
	  void postUploadMessage(String^ file);
	  void setRepoCateLists();
	  void setSelectedFileOperations();
	  void setDisplayPattern();
	  void toDictionary(Map cateMap);
	  array<String^>^ toArray(std::vector<std::string> vStr);
	  std::string processDisplayedFilename(String^ fileName);
	  void postDisplayPageMessage(std::string filePath);
	  void postDeleteMessageMessage(std::string filePath);
	  void postUploadFinishMessage();
	  void parseBrowseResponse(HttpMessage msg);
	  void lanuchPage(HttpMessage msg);
	  std::vector<std::string> parseFiles(std::string files);
	  void downloadWebFiles();

	  String^ serverIP_ = toSystemString(std::string("localhost"));
	  String^ serverPort_ = toSystemString(Utilities::Converter<size_t>::toString(8080));
	  String^ clientIP_;
	  String^ clientPort_;

	  Dictionary<String^, array<String^>^>^ repoCate_ = gcnew Dictionary<String^, array<String^>^>();
	  String^ clientFolder_;
	  String^ scriptFolder_;
  };
}


#endif
