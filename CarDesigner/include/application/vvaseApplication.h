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

#ifndef _VVASEAPP_CLASS_H_
#define _VVASEAPP_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// wxWidgets forward declarations
class wxSingleInstanceChecker;
class wxServer;

// VVASE forward declarations
class MAIN_FRAME;

// The application class
class VVASEApp : public wxApp
{
public:
	// Initialization function
	bool OnInit();
	int OnExit();

	// Private data accessor
	MAIN_FRAME *GetMainFrame(void) { return MainFrame; };

	// Constants required for generating data exchange objects
	static const wxString HostName;
	static const wxString ServiceName;
	static const wxString ConnectionTopic;

private:
	// The main class for the VVASE application - this object is the parent
	// for all other VVASE objects
	MAIN_FRAME *MainFrame;

	// To see if there is already another instance open
	wxSingleInstanceChecker *SingleInstanceChecker;

	// Server object to pass data between instances of this application
	wxServer *DataExchangeServer;

	// Deletes dynamic memory
	void DeleteDynamicMemory(void);
};

// Declare the application object (have wxWidgets create the wxGetApp() function)
DECLARE_APP(VVASEApp);

#endif// _VVASEAPP_CLASS_H_