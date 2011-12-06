/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  vvaseApplication.cpp
// Created:  5/17/2008
// Author:  K. Loux
// Description:  The application class.
// History:
//	11/23/2009	- Renamed from cardesignerapp_class.cpp, K. Loux.

// wxWidgets headers
#include <wx/wx.h>
#include <wx/snglinst.h>
#include <wx/ipc.h>

// CarDesigner headers
#include "application/vvaseApplication.h"
#include "gui/components/mainFrame.h"
#include "gui/ipcClient.h"
#include "gui/ipcServer.h"
#include "gui/ipcConnection.h"
#include "vUtilities/debugLog.h"
#include "vUtilities/debugger.h"
#include "vUtilities/convert.h"

// Implement the application (have wxWidgets set up the appropriate entry points, etc.)
IMPLEMENT_APP(VVASEApp);

//==========================================================================
// Class:			VVASEApp
// Function:		Constant Definitions
//
// Description:		Constant definitions for the VVASEApp class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
const wxString VVASEApp::hostName = _T("localhost");
const wxString VVASEApp::serviceName = _T("/tmp/VVASE_DDE");
const wxString VVASEApp::connectionTopic = _T("VVASE: Open File");

//==========================================================================
// Class:			VVASEApp
// Function:		OnInit
//
// Description:		Initializes the VVASE application window.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool = true for successful window initialization, false for error
//
//==========================================================================
bool VVASEApp::OnInit()
{
	// Set the application's name and the vendor's name
	SetAppName(_T("VVASE"));
	SetVendorName(_T("Kerry Loux"));

	// Create the single instance checker
	singleInstanceChecker = new wxSingleInstanceChecker(GetAppName() + _T(":") + wxGetUserId());

	// Initialize the dynamic exchange server to NULL
	dataExchangeServer = NULL;

	// If we have command line arguments (assume they are file names) and
	// another instance of this application is already running - pass the
	// filenames to the other application
	if (VVASEApp::argc > 1 && singleInstanceChecker->IsAnotherRunning())
	{
		// Another instance is already active - pass the file name to that instance to open
		// Create a client object to connect to the server
		IPCClient client;
		if (client.Connect(hostName, serviceName, connectionTopic))
		{
			// Send the file names to the server
			int i;
			for (i = 1; i < argc; i++)
				// Don't bother checking for whether the poke was successfully recieved or not
				client.GetConnection()->Poke(_T(argv[i]), NULL);
		}

		client.Disconnect();

		DeleteDynamicMemory();
		return false;
	}

	// Proceed with actions for a "normal" execution - display the main form, etc.
	// Create the MainFrame object - this is the parent for all VVASE objects
	mainFrame = new MainFrame();

	// Make sure the MainFrame was successfully created
	if (mainFrame == NULL)
		return false;

	// Make the window visible
	mainFrame->Show();

	// Bring the window to the top
	SetTopWindow(mainFrame);

	// If we had any command line arguments (file to open), open them now
	if (argc > 1)
	{
		// Wait for the main frame to finish initialization
		while (mainFrame->JobsPending())
		{
			wxSafeYield();
			wxMilliSleep(50);
		}

		// Load all of the files
		int i;
		for (i = 1; i < argc; i++)
			mainFrame->LoadFile(argv[i]);
	}
	
	// Start the data exchange server
	dataExchangeServer = new IPCServer();
	if (!dataExchangeServer->Create(serviceName))
	{
		// Server creation failed - delete the server object
		delete dataExchangeServer;
		dataExchangeServer = NULL;
	}

	return true;
}

//==========================================================================
// Class:			VVASEApp
// Function:		OnExit
//
// Description:		Initializes the VVASE application window.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool = true for successful window initialization, false for error
//
//==========================================================================
int VVASEApp::OnExit()
{
	DeleteDynamicMemory();
	DebugLog::Kill();
	Debugger::Kill();
	Convert::Kill();

	return 0;
}

//==========================================================================
// Class:			VVASEApp
// Function:		DeleteDynamicMemory
//
// Description:		Frees class-level dynamic memory.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void VVASEApp::DeleteDynamicMemory(void)
{
	// Delete the single instance checker
	delete singleInstanceChecker;
	singleInstanceChecker = NULL;
	
	// Delete the data exchange server
	delete dataExchangeServer;
	dataExchangeServer = NULL;
}