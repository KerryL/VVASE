/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  ipcClient.h
// Date:  10/23/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Inter-process communication client class.  Used for passing file names
//        between instances of this application to prevent multiple instances
//        from starting up when a user opens multiple files from a file browser.
//        wxWidgets implements IPC through a Dynamic Data Exchange process under
//        MSW and through TCP on other platforms.

#ifndef IPC_CLIENT_H_
#define IPC_CLIENT_H_

// wxWidgets headers
#include <wx/ipc.h>

namespace VVASE
{

// Local forward declarations
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

}// namespace VVASE

#endif// VVASE_IPC_CLIENT_H_
