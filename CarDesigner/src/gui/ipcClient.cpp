/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  ipc_client_class.cpp
// Created:  10/23/2010
// Author:  K. Loux
// Description:  Inter-process communication client class.  Used for passing file names
//				 between instances of this application to prevent multiple instances
//				 from starting up when a user opens multiple files from a file browser.
//				 wxWidgets implements IPC through a Dynamic Data Exchange process under
//				 MSW and through TCP on other platforms.
// History:

#include <wx/wx.h>

// VVASE headers
#include "gui/ipc_client_class.h"
#include "gui/ipc_connection_class.h"

//==========================================================================
// Class:			IPC_CLIENT
// Function:		IPC_CLIENT
//
// Description:		Constructor for IPC_CLIENT class.
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
IPC_CLIENT::IPC_CLIENT() : wxClient()
{
	// Initialize the connection poiner to NULL
	Connection = NULL;
}

//==========================================================================
// Class:			IPC_CLIENT
// Function:		~IPC_CLIENT
//
// Description:		Destructor for IPC_CLIENT class.
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
IPC_CLIENT::~IPC_CLIENT()
{
	// Disconnect from the server
	Disconnect();
}

//==========================================================================
// Class:			IPC_CLIENT
// Function:		Connect
//
// Description:		Attempts to connect to the specified service on the
//					specified host.
//
// Input Arguments:
//		Host	= const wxString& specifying the host to which we want to connect
//		Service	= const wxString& specifying the name of the service
//		Topic	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for connection made, false otherwise
//
//==========================================================================
bool IPC_CLIENT::Connect(const wxString &Host, const wxString &Service, const wxString &Topic)
{
	// Attempt to make a connection to the specified host
	Connection = (IPC_CONNECTION*)MakeConnection(Host, Service, Topic);
	return Connection != NULL;
}

//==========================================================================
// Class:			IPC_CLIENT
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
void IPC_CLIENT::Disconnect(void)
{
	if (Connection)
	{
		Connection->Disconnect();
		delete Connection;
		Connection = NULL;
	}
	
	return;
}

//==========================================================================
// Class:			IPC_CLIENT
// Function:		OnMakeConnection
//
// Description:		Overridden method for responding to successful connections.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxConnectionBase* pointing to the new connection object
//
//==========================================================================
wxConnectionBase *IPC_CLIENT::OnMakeConnection(void)
{
	return new IPC_CONNECTION();
}