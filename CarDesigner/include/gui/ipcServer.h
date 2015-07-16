/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  ipcServer.h
// Created:  10/23/2010
// Author:  K. Loux
// Description:  Inter-process communication server class.  Used for passing file names
//				 between instances of this application to prevent multiple instances
//				 from starting up when a user opens multiple files from a file browser.
//				 wxWidgets implements IPC through a Dynamic Data Exchange process under
//				 MSW and through TCP on other platforms.
// History:

#ifndef IPC_SERVER_H_
#define IPC_SERVER_H_

// wxWidgets headers
#include <wx/ipc.h>

// VVASE forward declarations
class IPCConnection;

class IPCServer : public wxServer
{
public:
	IPCServer();
	~IPCServer();
	
	// Connection handling functions
	void Disconnect();
	bool IsConnected() const { return connection != NULL; }
	
	// Accessors
	IPCConnection *GetConnection() { return connection; }
	
	// Overridden method for handling connection initializations
	wxConnectionBase *OnAcceptConnection(const wxString &topic);
	
private:
	IPCConnection *connection;
};

#endif// IPC_SERVER_H_