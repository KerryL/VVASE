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
// Class:			IPC_CONNECTION
// Function:		IPC_CONNECTION
//
// Description:		Constructor for the IPC_CONNECTION class.
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
IPC_CONNECTION::IPC_CONNECTION() : wxConnection()
{
}

//==========================================================================
// Class:			IPC_CONNECTION
// Function:		~IPC_CONNECTION
//
// Description:		Destructor for the IPC_CONNECTION class.
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
IPC_CONNECTION::~IPC_CONNECTION()
{
}

//==========================================================================
// Class:			IPC_CONNECTION
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
bool IPC_CONNECTION::OnPoke(const wxString &topic, const wxString &item, wxChar* WXUNUSED(data),
							int WXUNUSED(size), wxIPCFormat WXUNUSED(format))
{
	// Make sure we recognize the topic
	if (topic.CompareTo(VVASEApp::ConnectionTopic) == 0)
		// Tell the main frame to load the file
		return wxGetApp().GetMainFrame()->LoadFile(item);

	return false;
}