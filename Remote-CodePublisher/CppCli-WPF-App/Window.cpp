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
#include "Window.h"
using namespace CppCliWindows;

//----< constructor for ClientGUI >---------------------------------------------------

ClientGUI::ClientGUI(array<System::String^>^ args)
{
	// Process Command Arguments
	if (!processCommandArgument(args))
		return;

	// set up channel
	std::string ip = toStdString(clientIP_);
	size_t port = Utilities::Converter<size_t>::toValue(toStdString(clientPort_));
	ObjectFactory* pObjFact = new ObjectFactory;
	pChann_ = pObjFact->createMockChannel(ip, port);
	pChann_->start();
	delete pObjFact;

	// client's receive thread
	recvThread = gcnew Thread(gcnew ThreadStart(this, &ClientGUI::getMessage));
	recvThread->Start();

	setButtonEvents();

	// set Window properties
	this->Title = "Client  " + clientIP_ + ":" + clientPort_;
	this->Width = 800;
	this->Height = 600;

	// attach dock panel to Window
	this->Content = hDockPanel;
	hDockPanel->Children->Add(hStatusBar);
	hDockPanel->SetDock(hStatusBar, Dock::Bottom);
	hDockPanel->Children->Add(hGrid);

	// setup Window controls and views
	setUpTabControl();
	setUpStatusBar();
	setUpSendMessageView();
	setUpLocalFileView();
	setUpRepoView();

	// Set up local repository
	setUpLocalRepo();

	// On starting, download script files
	downloadWebFiles();
}

//----< process command line argujments >---------------------------------------------

bool ClientGUI::processCommandArgument(array<System::String^>^ args)
{
	if (args->Length == 1)
	{
		String^ clientEP = args[0];
		std::string temp = toStdString(clientEP);
		size_t found = temp.find(":");
		clientIP_ = toSystemString(temp.substr(0, found));
		clientPort_ = toSystemString(temp.substr(found + 1));
		return true;
	}
	else
	{
		std::cout << "\n No Correct Arguments! \n\n";
		return false;
	}
}

//----< set up button events >--------------------------------------------------------

void ClientGUI::setButtonEvents()
{
	// set event handlers
	this->Loaded +=
		gcnew System::Windows::RoutedEventHandler(this, &ClientGUI::OnLoaded);
	this->Closing +=
		gcnew CancelEventHandler(this, &ClientGUI::Unloading);

	// Set button events
	hShutdownServerButton->Click += gcnew RoutedEventHandler(this, &ClientGUI::shutdownServer);
	hClearButton->Click += gcnew RoutedEventHandler(this, &ClientGUI::clear);
	hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &ClientGUI::browseForFolder);
	hShowItemsButton->Click += gcnew RoutedEventHandler(this, &ClientGUI::getItemsFromList);
	hUploadButton->Click += gcnew RoutedEventHandler(this, &ClientGUI::uploadFiles);
	hBrowseRepoButton->Click += gcnew RoutedEventHandler(this, &ClientGUI::browseRepo);
	hViewFileButton->Click += gcnew RoutedEventHandler(this, &ClientGUI::viewFolder);
	hDisplayButton->Click += gcnew RoutedEventHandler(this, &ClientGUI::displayPage);
	hDeleteButton->Click += gcnew RoutedEventHandler(this, &ClientGUI::deletePage);
}

//----< destructor for ClientGUI >----------------------------------------------------

ClientGUI::~ClientGUI()
{
	delete pChann_;
}

//----< set up status bar >-----------------------------------------------------------

void ClientGUI::setUpStatusBar()
{
	hStatusBar->Items->Add(hStatusBarItem);
	hStatus->Text = "Ready for new operations!";
	hStatusBarItem->Content = hStatus;
	hStatusBar->Padding = Thickness(10, 2, 10, 2);
}

//----< set up tab control >----------------------------------------------------------

void ClientGUI::setUpTabControl()
{
	hGrid->Children->Add(hTabControl);
	hSendMessageTab->Header = "Message";
	hLocalFileTab->Header = "Local File";
	hRepoTab->Header = "Repository";
	hTabControl->Items->Add(hSendMessageTab);
	hTabControl->Items->Add(hLocalFileTab);
	hTabControl->Items->Add(hRepoTab);
}

//----< set up client folder >--------------------------------------------------------

void ClientGUI::setUpLocalRepo()
{
	// create client folder
	std::string clientFolder = "../Client " + toStdString(clientIP_) + toStdString(clientPort_);
	FileSystem::Directory::create(clientFolder);
	clientFolder_ = toSystemString(clientFolder);
	// create web pages folder
	std::string scriptFolder = clientFolder + "/WebFiles";
	FileSystem::Directory::create(scriptFolder);
	scriptFolder_ = toSystemString(scriptFolder);
}

//----< download script files on starting >-------------------------------------------

void ClientGUI::downloadWebFiles()
{
	HttpMessage msg;
	msg.addAttribute(HttpMessage::Attribute("Command", "DOWNLOAD_WEBFILES"));
	msg.addAttribute(HttpMessage::Attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::Attribute("FromAddr", toStdString(clientIP_) + ":" + toStdString(clientPort_)));
	msg.addAttribute(HttpMessage::Attribute("ToAddr", toStdString(serverIP_) + ":" + toStdString(serverPort_)));
	//hTextBlock1->Text += "\nMESSAGE DOWNLOAD WEB SCRIPTS SEND\n\n";
	hTextBlock1->Text += toSystemString(msg.toString());
	pChann_->postMessage(msg);
	hStatus->Text = "Download scripts!";
}

///////////////////////////////////////////////////////////////////////////
// Set up for Message tab

//----< set up text blocks of Message tab >-------------------------------------------

void ClientGUI::setTextBlockProperties()
{
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hSendMessageGrid->RowDefinitions->Add(hRow1Def);
	ColumnDefinition^ hCol1Def = gcnew ColumnDefinition();
	hSendMessageGrid->ColumnDefinitions->Add(hCol1Def);
	// Set up window for sent messages
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hTextBlock1;
	hTextBlock1->Padding = Thickness(10);
	hTextBlock1->Text = "";
	hTextBlock1->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hTextBlock1->FontWeight = FontWeights::Bold;
	hTextBlock1->FontSize = 12;
	hScrollViewer1->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewer1->Content = hBorder1;
	hSendMessageGrid->SetRow(hScrollViewer1, 1);
	hSendMessageGrid->SetColumn(hScrollViewer1, 0);
	hSendMessageGrid->Children->Add(hScrollViewer1);

	// Set up window for response
	ColumnDefinition^ hCol11Def = gcnew ColumnDefinition();
	hSendMessageGrid->ColumnDefinitions->Add(hCol11Def);
	Border^ hBorder2 = gcnew Border();
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hTextBlock2;
	hTextBlock2->Padding = Thickness(10);
	hTextBlock2->Text = "";
	hTextBlock2->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hTextBlock2->FontWeight = FontWeights::Bold;
	hTextBlock2->FontSize = 12;
	hScrollViewer2->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewer2->Content = hBorder2;
	hSendMessageGrid->SetRow(hScrollViewer2, 1);
	hSendMessageGrid->SetColumn(hScrollViewer2, 1);
	hSendMessageGrid->Children->Add(hScrollViewer2);
}

//----< set up button properties of Message tab >-------------------------------------

void ClientGUI::setMessageButtonsProperties()
{
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(100);
	hSendMessageGrid->RowDefinitions->Add(hRow2Def);
	hShutdownServerButton->Content = "Shutdown Client";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hShutdownServerButton;
	hClearButton->Content = "Clear Message List";
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hClearButton;
	hStackPanel1->Children->Add(hBorder3);
	hStackPanel1->Orientation = Orientation::Horizontal;
	hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hSendMessageGrid->SetRow(hStackPanel1, 2);
	hSendMessageGrid->SetColumn(hStackPanel1, 0);
	hSendMessageGrid->Children->Add(hStackPanel1);

	hStackPanel11->Children->Add(hBorder2);
	hStackPanel11->Orientation = Orientation::Horizontal;
	hStackPanel11->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hSendMessageGrid->SetRow(hStackPanel11, 2);
	hSendMessageGrid->SetColumn(hStackPanel11, 1);
	hSendMessageGrid->Children->Add(hStackPanel11);
}

//----< set up Message tab view >-----------------------------------------------------

void ClientGUI::setUpSendMessageView()
{
	Console::Write("\n  setting up sendMessage view");
	hSendMessageGrid->Margin = Thickness(20);
	hSendMessageTab->Content = hSendMessageGrid;
	hMessageLabel->Content = "HttpMessage Requests: ";
	hMessageLabel->FontSize = 18;
	hMessageLabel->Height = 40;
	hMessageLabel->Width = 250;
	hMessageLabel->FontWeight = FontWeights::Bold;
	hMessageLabel1->Content = "HttpMessage Response: ";
	hMessageLabel1->FontSize = 18;
	hMessageLabel1->Height = 40;
	hMessageLabel1->Width = 250;
	hMessageLabel1->FontWeight = FontWeights::Bold;
	RowDefinition^ rw = gcnew RowDefinition();
	rw->Height = GridLength(120);
	hSendMessageGrid->RowDefinitions->Add(rw);
	hSendMessageGrid->SetRow(hMessageLabel, 0);
	hSendMessageGrid->SetColumn(hMessageLabel, 0);
	hSendMessageGrid->SetRow(hMessageLabel1, 0);
	hSendMessageGrid->SetColumn(hMessageLabel1, 1);
	hSendMessageGrid->Children->Add(hMessageLabel);
	hSendMessageGrid->Children->Add(hMessageLabel1);
	setTextBlockProperties();
	setMessageButtonsProperties();
}

std::string ClientGUI::toStdString(String^ pStr)
{
	std::string dst;
	for (int i = 0; i < pStr->Length; ++i)
		dst += (char)pStr[i];
	return dst;
}

String^ ClientGUI::toSystemString(std::string& str)
{
	StringBuilder^ pStr = gcnew StringBuilder();
	for (size_t i = 0; i < str.size(); ++i)
		pStr->Append((Char)str[i]);
	return pStr->ToString();
}

//----< add text to request message window >-----------------------------------------

void ClientGUI::addText(String^ msg)
{
	hTextBlock1->Text += msg + "\n\n";
}

//----< add text to response message window >-----------------------------------------

void ClientGUI::addResponseText(String^ msg)
{
	hStatus->Text = "Ready for new operations!";
	hTextBlock2->Text += msg + "\n\n";
}

//----< add text to response message window >-----------------------------------------

void ClientGUI::UpdateListBox1(String^ msg)
{
	Dictionary<String^, array<String^>^>::KeyCollection^ keyColl = repoCate_->Keys;
	hTextBlock2->Text += msg + "\n\n";
	hListBox1->Items->Clear();
	hStatus->Text = "Ready for new operations!";
	for each (String^ cate in keyColl)
	{
		hListBox1->Items->Add(cate);
	}
}

//----< get message from recvr queue through mock channel >---------------------------

void ClientGUI::getMessage()
{
	while (true)
	{
		HttpMessage msg = pChann_->getMessage();
		std::string command = msg.findValue("Command");
		String^ sMsg = toSystemString(msg.toString());
		// If receive response message
		if (command == "RESPONSE")
		{
			String^ args = sMsg;
			Action<String^>^ act = gcnew Action<String^>(this, &ClientGUI::addResponseText);
			Dispatcher->Invoke(act, args);
		}
		else if (command == "BROWSE_RESPONSE")
		{
			parseBrowseResponse(msg);
			String^ args = sMsg;
			Action<String^>^ act = gcnew Action<String^>(this, &ClientGUI::UpdateListBox1);
			Dispatcher->Invoke(act, args);
		}
		else if (command == "BEGIN_DISPLAY")
		{
			lanuchPage(msg);
			String^ args = sMsg;
			Action<String^>^ act = gcnew Action<String^>(this, &ClientGUI::addResponseText);
			Dispatcher->Invoke(act, args);
		}
		else
			continue;
	}
}

//----< parse browse response and update repoCate_ >----------------------------------

void ClientGUI::parseBrowseResponse(HttpMessage msg)
{
	// body format: folder1|file1,file2+folder2|file3,file4+folder3|
	Map cateMap;
	std::string strMap = msg.findValue("Map");
	size_t foundCate = strMap.find_first_of("+");
	while (foundCate != std::string::npos)
	{
		std::string folder = strMap.substr(0, foundCate);
		strMap = strMap.substr(foundCate + 1);
		size_t foundFile = folder.find('|');
		std::string cateName = folder.substr(0, foundFile);
		std::string files = folder.substr(foundFile + 1);
		cateMap[cateName] = parseFiles(files);
		foundCate = strMap.find_first_of("+");
	}
	size_t foundFile = strMap.find('|');
	std::string cateName = strMap.substr(0, foundFile);
	cateMap[cateName] = parseFiles(strMap.substr(foundFile + 1));
	toDictionary(cateMap);
}

//----< lanuch html file using browser >----------------------------------

void ClientGUI::lanuchPage(HttpMessage msg)
{
	std::string filePath = toStdString(clientFolder_) + "/" + msg.findValue("FileName");
	std::string command("start \"\" \"" + filePath + "\"");
	std::system(command.c_str());
}

//----< parse files string into vector >----------------------------------------------

std::vector<std::string> ClientGUI::parseFiles(std::string files)
{
	std::vector<std::string> vFiles;
	if (files.size() == 0)
		return vFiles;
	else
	{
		size_t foundSep = files.find_first_of(',');
		while (foundSep != std::string::npos)
		{
			vFiles.push_back(files.substr(0, foundSep));
			files = files.substr(foundSep + 1);
			foundSep = files.find_first_of(',');
		}
		vFiles.push_back(files);
	}
	return vFiles;
}

//----< clear the message history lists >---------------------------------------------

void ClientGUI::clear(Object^ object, RoutedEventArgs^ args)
{
	Console::Write("\n  Clear message text");
	hStatus->Text = "Clear message history";
	hTextBlock1->Text = "";
	hTextBlock2->Text = "";
}

//----< handle server shutdown button >-----------------------------------------------

void ClientGUI::shutdownServer(Object^ object, RoutedEventArgs^ args)
{
	HttpMessage msg;
	msg.addAttribute(HttpMessage::Attribute("Command", "SHUTDOWN_CLIENT"));
	msg.addAttribute(HttpMessage::Attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::Attribute("FromAddr", toStdString(clientIP_) + ":" + toStdString(clientPort_)));
	msg.addAttribute(HttpMessage::Attribute("ToAddr", toStdString(serverIP_) + ":" + toStdString(serverPort_)));
	hTextBlock1->Text += toSystemString(msg.toString());
	pChann_->postMessage(msg);
	Console::Write("\n  <command>  Shutdown Server  </command>");
	hStatus->Text = "Shutdown Client!";
}

///////////////////////////////////////////////////////////////////////////
// Set up for File List tab

void ClientGUI::getItemsFromList(Object^ object, RoutedEventArgs^ args)
{
	int index = 0;
	int count = hListBox->SelectedItems->Count;
	hStatus->Text = "Show Selected Items";
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hListBox->SelectedItems)
		{
			//Console::Write(item);
			items[index++] = item;
		}
	}

	hListBox->Items->Clear();
	if (count > 0) {
		for each (String^ item in items)
		{
			hListBox->Items->Add(item);
		}
	}
}

//----< set up button properties of Local File tab >----------------------------------

void ClientGUI::setLocalFileButtonProperties()
{

	hUploadButton->Content = "Upload Selected Files";
	Border^ hBorder1 = gcnew Border();
	hBorder1->Width = 120;
	hBorder1->Height = 30;
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hShowItemsButton->Content = "Show Selected Items";
	hBorder1->Child = hUploadButton;
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hShowItemsButton;
	hStackPanel2->Children->Add(hBorder1);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 100;
	hStackPanel2->Children->Add(hSpacer);
	hStackPanel2->Children->Add(hBorder2);
	hStackPanel2->Orientation = Orientation::Horizontal;
	hStackPanel2->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hLocalFileGrid->SetRow(hStackPanel2, 2);
	hLocalFileGrid->Children->Add(hStackPanel2);
}

//----< set up button properties of upload tab >----------------------------------

void ClientGUI::setUploadButtonProperties()
{
	// Upload button
	hFolderBrowseButton->Content = "Select Directory";
	hFolderBrowseButton->Height = 30;
	hFolderBrowseButton->Width = 120;
	hFolderBrowseButton->BorderThickness = Thickness(2);
	hFolderBrowseButton->BorderBrush = Brushes::Black;
	hStackPanel21->Children->Add(hFolderBrowseButton);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 100;
	hStackPanel21->Children->Add(hSpacer);
	hHeaderBox->Content = ".h";
	hHeaderBox->IsChecked = "true";
	hHeaderBox->Height = 20;
	hHeaderBox->Width = 50;
	hHeaderBox->BorderThickness = Thickness(2);
	hHeaderBox->BorderBrush = Brushes::Black;
	hStackPanel21->Children->Add(hHeaderBox);
	TextBlock^ hSpacer1 = gcnew TextBlock();
	hSpacer1->Width = 100;
	hStackPanel21->Children->Add(hSpacer1);
	hCppBox->Content = ".cpp";
	hCppBox->IsChecked = "true";
	hCppBox->Height = 20;
	hCppBox->Width = 50;
	hCppBox->BorderThickness = Thickness(2);
	hCppBox->BorderBrush = Brushes::Black;
	hStackPanel21->Children->Add(hCppBox);
	hStackPanel21->Orientation = Orientation::Horizontal;
	hStackPanel21->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hLocalFileGrid->SetRow(hStackPanel21, 0);
	hLocalFileGrid->Children->Add(hStackPanel21);
}

//----< set up Local File tab view >--------------------------------------------------

void ClientGUI::setUpLocalFileView()
{
	Console::Write("\n  setting up Local File view");
	hLocalFileGrid->Margin = Thickness(20);
	hLocalFileTab->Content = hLocalFileGrid;

	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(100);
	hLocalFileGrid->RowDefinitions->Add(hRow1Def);
	RowDefinition^ hRow1Def2 = gcnew RowDefinition();
	hLocalFileGrid->RowDefinitions->Add(hRow1Def2);
	RowDefinition^ hRow1Def3 = gcnew RowDefinition();
	hRow1Def3->Height = GridLength(100);
	hLocalFileGrid->RowDefinitions->Add(hRow1Def3);

	// Set up file list
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hListBox->SelectionMode = SelectionMode::Multiple;
	hBorder1->Child = hListBox;
	hLocalFileGrid->SetRow(hBorder1, 1);
	hLocalFileGrid->Children->Add(hBorder1);
	hFolderBrowserDialog->ShowNewFolderButton = false;
	hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();

	// Set up buttons
	setLocalFileButtonProperties();
	setUploadButtonProperties();
}

//----< handle select directory button >----------------------------------------------

void ClientGUI::browseForFolder(Object^ object, RoutedEventArgs^ args)
{
	Console::Write("\n\n <command>  Browsing for folder  </command>");
	hListBox->Items->Clear();
	System::Windows::Forms::DialogResult result;
	result = hFolderBrowserDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hFolderBrowserDialog->SelectedPath;
		std::cout << "\n\n <command>  Opening folder \"" << toStdString(path) << "\"  </command>";
		array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
		for (int i = 0; i < dirs->Length; ++i)
			hListBox->Items->Add(L"<> " + dirs[i]);
		array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
		if (hHeaderBox->IsChecked) {
			for (int i = 0; i < files->Length; ++i) 
			{
				if (files[i]->Contains(toSystemString(std::string(".h"))))
					hListBox->Items->Add(files[i]);

			}
		}
		if (hCppBox->IsChecked) {
			for (int i = 0; i < files->Length; ++i)
			{
				if (files[i]->Contains(toSystemString(std::string(".cpp"))))
					hListBox->Items->Add(files[i]);

			}
		}
	}
	hStatus->Text = "Select Directory";
}

//----< handle upload button >--------------------------------------------------------

void ClientGUI::uploadFiles(Object^ object, RoutedEventArgs^ args)
{
	int index = 0;
	int count = hListBox->SelectedItems->Count;
	hStatus->Text = "Uploading...";
	if (count > 0) {
		for each (String^ item in hListBox->SelectedItems)
		{
			postUploadMessage(item);
		}
	}
	postUploadFinishMessage();
	hListBox->Items->Clear();
}

//----< post message to upload file >-------------------------------------------------

void ClientGUI::postUploadMessage(String^ file)
{
	std::cout << "<command> Uploading File: " << toStdString(file) << " </command>";
	HttpMessage msg;
	msg.addAttribute(HttpMessage::Attribute("Command", "UPLOAD_FILE"));
	msg.addAttribute(HttpMessage::Attribute("FilePath", toStdString(file)));
	msg.addAttribute(HttpMessage::Attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::Attribute("FromAddr", toStdString(clientIP_) + ":" + toStdString(clientPort_)));
	msg.addAttribute(HttpMessage::Attribute("ToAddr", toStdString(serverIP_) + ":" + toStdString(serverPort_)));
	hTextBlock1->Text += toSystemString(msg.toString());
	pChann_->postMessage(msg);
}

//----< post message to inform finishing uploading >----------------------------------

void ClientGUI::postUploadFinishMessage()
{
	HttpMessage msg;
	msg.addAttribute(HttpMessage::Attribute("Command", "FINISH_UPLOAD"));
	msg.addAttribute(HttpMessage::Attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::Attribute("FromAddr", toStdString(clientIP_) + ":" + toStdString(clientPort_)));
	msg.addAttribute(HttpMessage::Attribute("ToAddr", toStdString(serverIP_) + ":" + toStdString(serverPort_)));
	pChann_->postMessage(msg);
}

///////////////////////////////////////////////////////////////////////////
// Set up for Repository tab

//----< set up Repository tab view >--------------------------------------------------

void ClientGUI::setUpRepoView()
{
	Console::Write("\n  setting up Repository view");
	hRepoGrid->Margin = Thickness(20);
	hRepoTab->Content = hRepoGrid;

	// Set up browse repository button
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(100);
	hRepoGrid->RowDefinitions->Add(hRow1Def);
	hBrowseRepoButton->Content = "Browse Repository";
	hBrowseRepoButton->Height = 30;
	hBrowseRepoButton->Width = 120;
	hBrowseRepoButton->BorderThickness = Thickness(2);
	hBrowseRepoButton->BorderBrush = Brushes::Black;
	hStackPanel3->Children->Add(hBrowseRepoButton);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 150;
	hStackPanel3->Children->Add(hSpacer);
	hViewFileButton->Content = "View Folder Contents";
	hViewFileButton->Height = 30;
	hViewFileButton->Width = 120;
	hViewFileButton->BorderThickness = Thickness(2);
	hViewFileButton->BorderBrush = Brushes::Black;
	hStackPanel3->Children->Add(hViewFileButton);

	hStackPanel3->Orientation = Orientation::Horizontal;
	hStackPanel3->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hRepoGrid->SetRow(hStackPanel3, 0);
	hRepoGrid->Children->Add(hStackPanel3);
	// Set up two repository lists
	setRepoCateLists();

	// Set up file operation buttons
	setSelectedFileOperations();

	// Set up radiobuttons
	setDisplayPattern();
}

//----< set up Repository categories lists >------------------------------------------

void ClientGUI::setRepoCateLists()
{
	// Set up Folder list
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(250);
	hRepoGrid->RowDefinitions->Add(hRow2Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hListBox1->SelectionMode = SelectionMode::Single;
	hListBox1->Width = 250;
	hBorder1->Child = hListBox1;
	hStackPanel31->Children->Add(hBorder1);

	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 20;
	hStackPanel31->Children->Add(hSpacer);
	// Set up File list
	Border^ hBorder2 = gcnew Border();
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hListBox2->SelectionMode = SelectionMode::Single;
	hListBox2->Width = 350;
	hBorder2->Child = hListBox2;
	hStackPanel31->Children->Add(hBorder2);

	hStackPanel31->Orientation = Orientation::Horizontal;
	hStackPanel31->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hRepoGrid->SetRow(hStackPanel31, 1);
	hRepoGrid->Children->Add(hStackPanel31);
}

//----< set up selected file operations buttons >-------------------------------------

void ClientGUI::setSelectedFileOperations()
{
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(80);
	hRepoGrid->RowDefinitions->Add(hRow3Def);

	hDisplayButton->Content = "Display Selected File";
	hDisplayButton->Height = 30;
	hDisplayButton->Width = 120;
	hDisplayButton->BorderThickness = Thickness(2);
	hDisplayButton->BorderBrush = Brushes::Black;
	hStackPanel32->Children->Add(hDisplayButton);
	TextBlock^ hSpacer1 = gcnew TextBlock();
	hSpacer1->Width = 100;
	hStackPanel32->Children->Add(hSpacer1);

	hDeleteButton->Content = "Delete Selected File";
	hDeleteButton->Height = 30;
	hDeleteButton->Width = 120;
	hDeleteButton->BorderThickness = Thickness(2);
	hDeleteButton->BorderBrush = Brushes::Black;
	hStackPanel32->Children->Add(hDeleteButton);

	hStackPanel32->Orientation = Orientation::Horizontal;
	hStackPanel32->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hRepoGrid->SetRow(hStackPanel32, 2);
	hRepoGrid->Children->Add(hStackPanel32);
}

//----< set radio buttons for display pattern >---------------------------------------

void ClientGUI::setDisplayPattern()
{
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	//hRow3Def->Height = GridLength(50);
	hRepoGrid->RowDefinitions->Add(hRow3Def);

	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 185;
	hStackPanel33->Children->Add(hSpacer);

	// Set up lazy download button
	hLazyDownload->Content = "Lazy Download";
	hLazyDownload->IsChecked = "True";
	hStackPanel33->Children->Add(hLazyDownload);

	TextBlock^ hSpacer1 = gcnew TextBlock();
	hSpacer1->Width = 10;
	hStackPanel33->Children->Add(hSpacer1);

	// Set up IIS button
	hIIS->Content = "IIS";
	hStackPanel33->Children->Add(hIIS);
	
	hStackPanel33->Orientation = Orientation::Horizontal;
	hStackPanel33->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;
	hRepoGrid->SetRow(hStackPanel33, 3);
	hRepoGrid->Children->Add(hStackPanel33);
}

//----< handle browse repository button >---------------------------------------------

void ClientGUI::browseRepo(Object^ object, RoutedEventArgs^ args)
{
	hListBox1->Items->Clear();
	hListBox2->Items->Clear();
	HttpMessage msg;
	msg.addAttribute(HttpMessage::Attribute("Command", "BROWSE_REPO"));
	msg.addAttribute(HttpMessage::Attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::Attribute("FromAddr", toStdString(clientIP_) + ":" + toStdString(clientPort_)));
	msg.addAttribute(HttpMessage::Attribute("ToAddr", toStdString(serverIP_) + ":" + toStdString(serverPort_)));
	//hTextBlock1->Text += "\nMESSAGE BROWSE REPOSITORY SEND\n";
	hTextBlock1->Text += toSystemString(msg.toString());
	pChann_->postMessage(msg);
	Console::Write("\n  <command>  Browse Repository  </command>");
	hStatus->Text = "Browse Repository";
}

//----< handle view folder contents button >---------------------------------------------

void ClientGUI::viewFolder(Object^ object, RoutedEventArgs^ args)
{
	int index = 0;
	int count = hListBox1->SelectedItems->Count;
	//std::cout << "\n Count: " << count << std::endl;
	hStatus->Text = "View Category Contents";
	hListBox2->Items->Clear();
	if (count == 1) {
		String^ cate = hListBox1->SelectedItems[0]->ToString();
		array<String^>^ files;
		if (repoCate_->TryGetValue(cate, files))
		{
			for each (String^ file in files)
			{
				hListBox2->Items->Add(file);
			}
		}
	}

}

//----< convert map into dictionary >-------------------------------------------------

void ClientGUI::toDictionary(Map cateMap)
{
	repoCate_->Clear();
	for (auto cate : cateMap)
	{
		String^ key = toSystemString(std::string(cate.first));
		array<String^>^ value = toArray(cate.second);
		try
		{
			repoCate_->Add(key, value);
		}
		catch (ArgumentException^)
		{
			Console::WriteLine("An element with Key = \"txt\" already exists.");
		}
	}
}

//----< convert vector into array >---------------------------------------------------

array<String^>^ ClientGUI::toArray(std::vector<std::string> vStr)
{
	int size = vStr.size();
	int count = 0;
	array<String^>^ temp = gcnew array<String^>(size);
	for (auto str : vStr)
	{
		temp[count++] = toSystemString(str);
	}
	return temp;
}

//----< lazy download and display the selected page >---------------------------------

void ClientGUI::displayPage(Object^ object, RoutedEventArgs^ args)
{
	int index = 0;
	int count = hListBox2->SelectedItems->Count;
	hStatus->Text = "Display Selected File";
	if (count > 0) {
		String^ folder = hListBox1->SelectedItem->ToString();
		for each (String^ file in hListBox2->SelectedItems)
		{
			std::string filePath = toStdString(folder) + "/" + processDisplayedFilename(file);
			std::cout << "\n File Path: " << filePath;
			postDisplayPageMessage(filePath);
		}
	}
}

//----< process selected file name to display >---------------------------------------

std::string ClientGUI::processDisplayedFilename(String^ fileName)
{
	std::string processedName = toStdString(fileName);
	size_t found = processedName.find_last_of('.');
	if (processedName.substr(found + 1) == "htm")
		return processedName;
	else
		return processedName.append(".htm");
}

//----< make and post message to send DOWNLOAD_PAGE request >-------------------------

void ClientGUI::postDisplayPageMessage(std::string filePath)
{
	HttpMessage msg;
	msg.addAttribute(HttpMessage::Attribute("Command", "DISPLAY_PAGE"));
	msg.addAttribute(HttpMessage::Attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::Attribute("FromAddr", toStdString(clientIP_) + ":" + toStdString(clientPort_)));
	msg.addAttribute(HttpMessage::Attribute("ToAddr", toStdString(serverIP_) + ":" + toStdString(serverPort_)));
	msg.addAttribute(HttpMessage::Attribute("FilePath", filePath));
	//hTextBlock1->Text += "\nMESSAGE DISPLAY PAGE SEND\n";
	hTextBlock1->Text += toSystemString(msg.toString());
	pChann_->postMessage(msg);
	Console::Write("\n  <command>  Display Selected Page  </command>");
}

//----< delete selected file from repository >----------------------------------------

void ClientGUI::deletePage(Object^ object, RoutedEventArgs^ args)
{
	int index = 0;
	int count = hListBox2->SelectedItems->Count;
	hStatus->Text = "Delete Selected File";
	if (count > 0) {
		String^ folder = hListBox1->SelectedItem->ToString();
		for each (String^ file in hListBox2->SelectedItems)
		{
			std::string filePath = toStdString(folder) + "/" + processDisplayedFilename(file);
			postDeleteMessageMessage(filePath);

			// Need to modify to display deleted list
			/*String^ key = folder;
			array<String^>^ value;
			repoCate_->TryGetValue(key, value);*/
		}
		hListBox2->Items->Clear();
	}
}

//----< send request to publish html files >------------------------------------------

void ClientGUI::publishHtml(Object^ object, RoutedEventArgs^ args)
{
	// Send message to denote finishing uploading
	// Thus server can begin publishing
	HttpMessage msg;
	msg.addAttribute(HttpMessage::Attribute("Command", "PUBLISH"));
	msg.addAttribute(HttpMessage::Attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::Attribute("FromAddr", toStdString(clientIP_) + ":" + toStdString(clientPort_)));
	msg.addAttribute(HttpMessage::Attribute("ToAddr", toStdString(serverIP_) + ":" + toStdString(serverPort_)));
	hTextBlock1->Text += toSystemString(msg.toString());
	pChann_->postMessage(msg);
}

//----< make and post message to send DELETE_FILE request >---------------------------

void ClientGUI::postDeleteMessageMessage(std::string filePath)
{
	HttpMessage msg;
	msg.addAttribute(HttpMessage::Attribute("Command", "DELETE_FILE"));
	msg.addAttribute(HttpMessage::Attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::Attribute("FromAddr", toStdString(clientIP_) + ":" + toStdString(clientPort_)));
	msg.addAttribute(HttpMessage::Attribute("ToAddr", toStdString(serverIP_) + ":" + toStdString(serverPort_)));
	msg.addAttribute(HttpMessage::Attribute("FilePath", filePath));
	//hTextBlock1->Text += "\nMESSAGE DELETE FILE SEND\n";
	hTextBlock1->Text += toSystemString(msg.toString());
	pChann_->postMessage(msg);
	Console::Write("\n  <command>  Delete Selected Page  </command>");
}

void ClientGUI::OnLoaded(Object^ object, RoutedEventArgs^ args)
{
	Console::Write("\n  Window loaded");
}
void ClientGUI::Unloading(Object^ object, System::ComponentModel::CancelEventArgs^ args)
{
	Console::Write("\n  Window closing");
}

[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
	Console::WriteLine(L"\n Starting ClientGUI");

	Application^ app = gcnew Application();
	app->Run(gcnew ClientGUI(args));
	Console::WriteLine(L"\n\n");

}