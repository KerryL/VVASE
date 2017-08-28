/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  ipcConnection.h
// Date:  10/23/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
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

	// Client sends data to the server via wxConnection::Poke()

	// Method by which the server processes and acknowledges receipt of the data
	/*bool OnPoke(const wxString &topic, const wxString &item, wxChar *data,
		int size, wxIPCFormat format) override;*/

	// Overridden method to alert derived classes that the connection doesn't exist
	// Only reason to override this method is so this class, the derived class, knows
	// that the connection is no longer available.  See wxWidgets docmentation for
	// more detail.
	bool OnDisconnect() override { return true; }
};

}// namespace VVASE

#endif// IPC_CONNECTION_H_
