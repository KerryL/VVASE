/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  ipcServer.h
// Date:  10/23/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Inter-process communication server class.  Used for passing file names
//        between instances of this application to prevent multiple instances
//        from starting up when a user opens multiple files from a file browser.
//        wxWidgets implements IPC through a Dynamic Data Exchange process under
//        MSW and through TCP on other platforms.

#ifndef IPC_SERVER_H_
#define IPC_SERVER_H_

// wxWidgets headers
#include <wx/ipc.h>

namespace VVASE
{

// Local forward declarations
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

}// namespace VVASE

#endif// IPC_SERVER_H_