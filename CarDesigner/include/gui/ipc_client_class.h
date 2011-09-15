/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  ipc_client_class.h
// Created:  10/23/2010
// Author:  K. Loux
// Description:  Inter-process communication client class.  Used for passing file names
//				 between instances of this application to prevent multiple instances
//				 from starting up when a user opens multiple files from a file browser.
//				 wxWidgets implements IPC through a Dynamic Data Exchange process under
//				 MSW and through TCP on other platforms.
// History:

#ifndef _IPC_CLIENT_CLASS_H_
#define _IPC_CLIENT_CLASS_H_

// wxWidgets headers
#include <wx/ipc.h>

// VVASE forward declarations
class IPC_CONNECTION;

// Client class declaration
class IPC_CLIENT : public wxClient
{
public:
	// Constructor
	IPC_CLIENT();
	
	// Destructor
	~IPC_CLIENT();
	
	// Connection handling functions
	bool Connect(const wxString &Host, const wxString &Service, const wxString &Topic);
	void Disconnect(void);
	
	// Overridden method for handling connection initializations
	wxConnectionBase *OnMakeConnection(void);
	
	// Accessors
	bool IsConnected(void) const { return Connection != NULL; };
	IPC_CONNECTION *GetConnection(void) { return Connection; };
	
private:
	// The connection object
	IPC_CONNECTION *Connection;
};

#endif// _VVASE_IPC_CLIENT_CLASS_H_