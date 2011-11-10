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

#ifndef _IPC_SERVER_CLASS_H_
#define _IPC_SERVER_CLASS_H_

// wxWidgets headers
#include <wx/ipc.h>

// VVASE forward declarations
class IPC_CONNECTION;

// Server class declaration
class IPC_SERVER : public wxServer
{
public:
	// Constructor
	IPC_SERVER();
	
	// Destructor
	~IPC_SERVER();
	
	// Connection handling functions
	void Disconnect(void);
	bool IsConnected(void) const { return Connection != NULL; };
	
	// Accessors
	IPC_CONNECTION *GetConnection(void) { return Connection; };
	
	// Overridden method for handling connection initializations
	wxConnectionBase *OnAcceptConnection(const wxString &topic);
	
private:
	// The connection object
	IPC_CONNECTION *Connection;
};

#endif// _IPC_SERVER_CLASS_H_