/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  ipcServer.cpp
// Created:  10/23/2010
// Author:  K. Loux
// Description:  Inter-process communication server class.  Used for passing file names
//				 between instances of this application to prevent multiple instances
//				 from starting up when a user opens multiple files from a file browser.
//				 wxWidgets implements IPC through a Dynamic Data Exchange process under
//				 MSW and through TCP on other platforms.
// History:

#include <wx/wx.h>

// VVASE headers
#include "gui/ipcServer.h"
#include "gui/ipcConnection.h"
#include "application/vvaseApplication.h"

//==========================================================================
// Class:			IPC_SERVER
// Function:		IPC_SERVER
//
// Description:		Constructor for IPC_SERVER class.
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
IPC_SERVER::IPC_SERVER() : wxServer()
{
	// Initialize the connection poiner to NULL
	Connection = NULL;
}

//==========================================================================
// Class:			IPC_SERVER
// Function:		~IPC_SERVER
//
// Description:		Destructor for IPC_SERVER class.
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
IPC_SERVER::~IPC_SERVER()
{
	// Kill the current connection
	Disconnect();
}

//==========================================================================
// Class:			IPC_SERVER
// Function:		Disconnect
//
// Description:		Kills the current connection.
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
void IPC_SERVER::Disconnect(void)
{
	if (Connection)
	{
		Connection->Disconnect();
		delete Connection;
		Connection = NULL;
	}
}

//==========================================================================
// Class:			IPC_SERVER
// Function:		OnAcceptConnection
//
// Description:		Responds to new connections.
//
// Input Arguments:
//		topic	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
wxConnectionBase *IPC_SERVER::OnAcceptConnection(const wxString& topic)
{
	// Check to see if we recognize the topic
	if (topic.CompareTo(VVASEApp::ConnectionTopic) == 0)
    {
		// Create and return a new connection
        Connection = new IPC_CONNECTION();
		
        return Connection;
    }
	
	// We don't recognize the topic
	return NULL;
}