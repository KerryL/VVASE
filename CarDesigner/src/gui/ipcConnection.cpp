/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  ipcConnection.cpp
// Created:  10/23/2010
// Author:  K. Loux
// Description:  Inter-process communication client class.  Used for passing file names
//				 between instances of this application to prevent multiple instances
//				 from starting up when a user opens multiple files from a file browser.
//				 wxWidgets implements IPC through a Dynamic Data Exchange process under
//				 MSW and through TCP on other platforms.
// History:

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "gui/ipcConnection.h"
#include "gui/components/mainFrame.h"
#include "application/vvaseApplication.h"

//==========================================================================
// Class:			IPCConnection
// Function:		IPCConnection
//
// Description:		Constructor for the IPCConnection class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
IPCConnection::IPCConnection() : wxConnection()
{
}

//==========================================================================
// Class:			IPCConnection
// Function:		~IPCConnection
//
// Description:		Destructor for the IPCConnection class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
IPCConnection::~IPCConnection()
{
}

//==========================================================================
// Class:			IPCConnection
// Function:		OnPoke
//
// Description:		Handled by the server object.  Responds to data sent by
//					the client object.
//
// Input Arguments:
//		topic	= const wxString&
//		item	= const wxString& containing the data (file name to load)
//		data	= wxChar *
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
bool IPCConnection::OnPoke(const wxString &topic, const wxString &item, wxChar* WXUNUSED(data),
							int WXUNUSED(size), wxIPCFormat WXUNUSED(format))
{
	// Make sure we recognize the topic
	if (topic.CompareTo(VVASEApp::connectionTopic) == 0)
		// Tell the main frame to load the file
		return wxGetApp().GetMainFrame()->LoadFile(item);

	return false;
}