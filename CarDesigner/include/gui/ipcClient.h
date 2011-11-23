/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  ipcClient.h
// Created:  10/23/2010
// Author:  K. Loux
// Description:  Inter-process communication client class.  Used for passing file names
//				 between instances of this application to prevent multiple instances
//				 from starting up when a user opens multiple files from a file browser.
//				 wxWidgets implements IPC through a Dynamic Data Exchange process under
//				 MSW and through TCP on other platforms.
// History:

#ifndef _IPC_CLIENT_H_
#define _IPC_CLIENT_H_

// wxWidgets headers
#include <wx/ipc.h>

// VVASE forward declarations
class IPCConnection;

// Client class declaration
class IPCClient : public wxClient
{
public:
	// Constructor
	IPCClient();
	
	// Destructor
	~IPCClient();
	
	// Connection handling functions
	bool Connect(const wxString &host, const wxString &service, const wxString &topic);
	void Disconnect(void);
	
	// Overridden method for handling connection initializations
	wxConnectionBase *OnMakeConnection(void);
	
	// Accessors
	bool IsConnected(void) const { return connection != NULL; };
	IPCConnection *GetConnection(void) { return connection; };
	
private:
	// The connection object
	IPCConnection *connection;
};

#endif// _VVASE_IPC_CLIENT_H_