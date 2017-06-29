/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  ipcServer.cpp
// Date:  10/23/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Inter-process communication server class.  Used for passing file names
//        between instances of this application to prevent multiple instances
//        from starting up when a user opens multiple files from a file browser.
//        wxWidgets implements IPC through a Dynamic Data Exchange process under
//        MSW and through TCP on other platforms.

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "gui/ipcServer.h"
#include "gui/ipcConnection.h"
#include "application/vvaseApplication.h"

namespace VVASE
{

//==========================================================================
// Class:			IPCServer
// Function:		IPCServer
//
// Description:		Constructor for IPCServer class.
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
IPCServer::IPCServer() : wxServer()
{
	// Initialize the connection poiner to NULL
	connection = NULL;
}

//==========================================================================
// Class:			IPCServer
// Function:		~IPCServer
//
// Description:		Destructor for IPCServer class.
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
IPCServer::~IPCServer()
{
	// Kill the current connection
	Disconnect();
}

//==========================================================================
// Class:			IPCServer
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
void IPCServer::Disconnect()
{
	if (connection)
	{
		connection->Disconnect();
		delete connection;
		connection = NULL;
	}
}

//==========================================================================
// Class:			IPCServer
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
wxConnectionBase *IPCServer::OnAcceptConnection(const wxString& topic)
{
	// Check to see if we recognize the topic
	if (topic.CompareTo(VVASEApp::connectionTopic) == 0)
    {
		// Create and return a new connection
        connection = new IPCConnection();

        return connection;
    }

	// We don't recognize the topic
	return NULL;
}

}// namespace VVASE
