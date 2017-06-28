/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  ipcConnection.h
// Date:  10/23/2010
// Author:  K. Loux
// Desc:  Inter-process communication connection class.  Used for passing file names
//        between instances of this application to prevent multiple instances
//        from starting up when a user opens multiple files from a file browser.
//        wxWidgets implements IPC through a Dynamic Data Exchange process under
//        MSW and through TCP on other platforms.

#ifndef IPC_CONNECTION_H_
#define IPC_CONNECTION_H_

// wxWidgets headers
#include <wx/ipc.h>

namespace VVASE
{

class IPCConnection : public wxConnection
{
public:
	IPCConnection();
	~IPCConnection();

	// Client sends data to the server via wxConnection::Poke()

	// Method by which the server processes and acknowledges receipt of the data
	virtual bool OnPoke(const wxString &topic, const wxString &item, wxChar *data,
		int size, wxIPCFormat format);

	// Overridden method to alert derived classes that the connection doesn't exist
	// Only reason to override this method is so this class, the derived class, knows
	// that the connection is no longer available.  See wxWidgets docmentation for
	// more detail.
	virtual bool OnDisconnect() { return true; }
};

}// namespace VVASE

#endif// IPC_CONNECTION_H_
