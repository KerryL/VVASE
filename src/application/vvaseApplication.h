/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  vvaseApplication.h
// Date:  5/17/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  The application class.

#ifndef VVASEAPP_H_
#define VVASEAPP_H_

// wxWidgets headers
#include <wx/wx.h>

// wxWidgets forward declarations
class wxSingleInstanceChecker;
class wxServer;

namespace VVASE
{

// Local forward declarations
class MainFrame;

class VVASEApp : public wxApp
{
public:
	// Initialization function
	bool OnInit();
	int OnExit();

	MainFrame *GetMainFrame() { return mainFrame; }

	// Constants required for generating data exchange objects
	static const wxString hostName;
	static const wxString serviceName;
	static const wxString connectionTopic;

private:
	// The main class for the VVASE application - this object is the parent
	// for all other VVASE objects
	MainFrame *mainFrame;

	// To see if there is already another instance open
	wxSingleInstanceChecker *singleInstanceChecker;

	// Server object to pass data between instances of this application
	wxServer *dataExchangeServer;

	void DeleteDynamicMemory();
};

// Declare the application object (have wxWidgets create the wxGetApp() function)
DECLARE_APP(VVASEApp);

}// namespace VVASE

#endif// VVASEAPP_H_
