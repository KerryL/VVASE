/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  vvaseApplication.h
// Created:  5/17/2008
// Author:  K. Loux
// Description:  The application class.
// History:
//	11/23/2009	- Renamed from cardesignerapp_class.h, K. Loux.

#ifndef _VVASEAPP_H_
#define _VVASEAPP_H_

// wxWidgets headers
#include <wx/wx.h>

// wxWidgets forward declarations
class wxSingleInstanceChecker;
class wxServer;

// VVASE forward declarations
class MainFrame;

// The application class
class VVASEApp : public wxApp
{
public:
	// Initialization function
	bool OnInit();
	int OnExit();

	// Private data accessor
	MainFrame *GetMainFrame(void) { return mainFrame; };

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

	// Deletes dynamic memory
	void DeleteDynamicMemory(void);
};

// Declare the application object (have wxWidgets create the wxGetApp() function)
DECLARE_APP(VVASEApp);

#endif// _VVASEAPP_H_