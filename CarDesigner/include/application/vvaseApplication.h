/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  vvaseApplication.h
// Created:  5/17/2008
// Author:  K. Loux
// Description:  The application class.
// History:
//	11/23/2009	- Renamed from cardesignerapp_class.h, K. Loux.

#ifndef VVASEAPP_H_
#define VVASEAPP_H_

// wxWidgets headers
#include <wx/wx.h>

// wxWidgets forward declarations
class wxSingleInstanceChecker;
class wxServer;

// VVASE forward declarations
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

#endif// VVASEAPP_H_