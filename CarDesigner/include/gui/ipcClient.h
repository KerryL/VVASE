/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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

#ifndef IPC_CLIENT_H_
#define IPC_CLIENT_H_

// wxWidgets headers
#include <wx/ipc.h>

// VVASE forward declarations
class IPCConnection;

// Client class declaration
class IPCClient : public wxClient
{
public:
	IPCClient();
	~IPCClient();
	
	// Connection handling functions
	bool Connect(const wxString &host, const wxString &service, const wxString &topic);
	void Disconnect();
	
	// Overridden method for handling connection initializations
	wxConnectionBase *OnMakeConnection();
	
	// Accessors
	bool IsConnected() const { return connection != NULL; }
	IPCConnection *GetConnection() { return connection; }
	
private:
	IPCConnection *connection;
};

#endif// VVASE_IPC_CLIENT_H_