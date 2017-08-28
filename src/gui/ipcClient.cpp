/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  ipcClient.cpp
// Date:  10/23/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Inter-process communication client class.  Used for passing file names
//        between instances of this application to prevent multiple instances
//        from starting up when a user opens multiple files from a file browser.
//        wxWidgets implements IPC through a Dynamic Data Exchange process under
//        MSW and through TCP on other platforms.

#include <wx/wx.h>

// Local headers
#include "ipcClient.h"
#include "ipcConnection.h"

namespace VVASE
{

//==========================================================================
// Class:			IPCClient
// Function:		IPCClient
//
// Description:		Constructor for IPCClient class.
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
IPCClient::IPCClient() : wxClient()
{
}

//==========================================================================
// Class:			IPCClient
// Function:		~IPCClient
//
// Description:		Destructor for IPCClient class.
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
IPCClient::~IPCClient()
{
	if (connection)
		connection->Disconnect();
}

//==========================================================================
// Class:			IPCClient
// Function:		Connect
//
// Description:		Attempts to connect to the specified service on the
//					specified host.
//
// Input Arguments:
//		host	= const wxString& specifying the host to which we want to connect
//		service	= const wxString& specifying the name of the service
//		topic	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for connection made, false otherwise
//
//==========================================================================
bool IPCClient::Connect(const wxString &host, const wxString &service, const wxString &topic)
{
	// Attempt to make a connection to the specified host
	connection = std::unique_ptr<IPCConnection>(dynamic_cast<IPCConnection*>(MakeConnection(host, service, topic)));
	return connection != nullptr;
}

//==========================================================================
// Class:			IPCClient
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
wxConnectionBase *IPCClient::OnMakeConnection()
{
	return new IPCConnection();
}

}// namespace VVASE
