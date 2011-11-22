/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mainFrame.cpp
// Created:  3/7/2008
// Author:  K. Loux
// Description:  Contains the class functionality (event handlers, etc.) for the
//				 MAIN_FRAME class.  Uses wxWidgets for the GUI components.
// History:
//	1/24/2009	- Major application structure change - MAIN_FRAME uses GUI_OBJECT instead of
//				  GUI_CAR.  GUI_OBJECT changed to only contain either GUI_CAR or ITERATION
//				  objects, K. Loux.
//	1/28/2009	- Changed structure of GUI components so context menu creation for all
//				  objects is handled by this class, K. Loux.
//	2/10/2009	- Added EDIT_PANEL to application, K. Loux.
//	2/17/2009	- Moved the Kinematics object for primary analysis into the GUI_CAR class.
//	6/7/2009	- Changed GetFilenameFromUser() to make use of wx functions for checking for file
//				  existance and selecting multiple files to open, K. Loux.
//	10/14/2010	- Added configuration file for storing application level options, K. Loux.
//	11/28/2010	- Added number of threads to configuration file, K. Loux.

// For difficult debugging problems, use this flag to print messages to file as well as to the output pane
//#define _DEBUG_TO_FILE_

// Standard C++ headers
#ifdef _DEBUG_TO_FILE_
#include <fstream>
#endif

// wxWidgets headers
#include <wx/aboutdlg.h>
#include <wx/datetime.h>
#include <wx/mimetype.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/docview.h>

//#include <wx/platinfo.h>

// CarDesigner headers
#include "application/vvaseConstants.h"
#include "vCar/car.h"
#include "vCar/drivetrain.h"
#include "gui/renderer/carRenderer.h"
#include "gui/renderer/plotRenderer.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/mainFrame.h"
#include "gui/components/mainNotebook.h"
#include "gui/components/mainTree.h"
#include "gui/components/outputPanel.h"
#include "gui/guiObject.h"
#include "gui/guiCar.h"
#include "gui/iteration.h"
#include "gui/geneticOptimization.h"
#include "gui/gaObject.h"
#include "gui/dialogs/optionsDialog.h"
#include "gui/appearanceOptions.h"
#include "gui/dropTarget.h"
#include "vSolver/physics/kinematics.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vSolver/threads/jobQueue.h"
#include "vSolver/threads/workerThread.h"
#include "vSolver/threads/threadEvent.h"
#include "vMath/vector.h"
#include "vUtilities/fontFinder.h"

// *nix Icons
#ifdef __WXGTK__
#include "../res/icons/aavase16.xpm"
#include "../res/icons/aavase32.xpm"
#include "../res/icons/aavase48.xpm"
#endif

//==========================================================================
// Class:			MAIN_FRAME
// Function:		MAIN_FRAME
//
// Description:		Constructor for MAIN_FRAME class.  Initializes the form
//					and creates the controls, etc.
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
MAIN_FRAME::MAIN_FRAME() : wxFrame(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition,
								   wxDefaultSize, wxDEFAULT_FRAME_STYLE),
								   /*MaxRecentFiles(9), */UndoRedo(*this)
{
	// Create the SystemsTree
	SystemsTree = new MAIN_TREE(*this, wxID_ANY, wxDefaultPosition, wxSize(320, 384),
		wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_DEFAULT_STYLE | wxSUNKEN_BORDER
		| wxTR_HIDE_ROOT, debugger);

	// Create the Notebook
	Notebook = new MAIN_NOTEBOOK(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE |
		wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_WINDOWLIST_BUTTON,
		debugger);

	// Create the EditPanel
	EditPanel = new EDIT_PANEL(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize, debugger);

	// Create the OutputPanel
	OutputPanel = new OUTPUT_PANEL(*this, wxID_ANY, wxDefaultPosition, wxSize(350, -1), debugger);

	// Create the OutputPane
	DebugPane = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(-1, 350), wxTE_PROCESS_TAB | wxTE_MULTILINE | wxHSCROLL | wxTE_READONLY
		| wxTE_RICH);

	// Create additional objects
	CreateMenuBar();
	StatusBar = CreateStatusBar(2, 0);

	// Add the toolbars
	KinematicToolbar = NULL;
	CreateKinematicAnalysisToolbar();

	// These need to be in this order - otherwise the centering doesn't work (resize first)
	SetProperties();
	DoLayout();
	InitializeSolver();

	// Load user preferences - done after SetProperties(), etc. because this will overwrite defaults
	// if a setup was previously saved
	ReadConfiguration();

	// Initialize the object management variables
	ActiveIndex = -1;
	BeingDeleted = false;

	debugger.Print(CarDesignerName + _T(" Initialized!"));

	/*debugger.Print(wxPlatformInfo::Get().GetArchName());
	debugger.Print(wxPlatformInfo::Get().GetEndiannessName());

	debugger.Print(wxGetOsDescription());
	wxString temp;
	temp.Printf("%i cores", wxThread::GetCPUCount());
	debugger.Print(temp);*/

	// Some debugging information
	/*debugger.Print(Debugger::PriorityVeryHigh, "sizeof(double): %i", sizeof(double));
	debugger.Print(Debugger::PriorityVeryHigh, "sizeof(short): %i", sizeof(short));
	debugger.Print(Debugger::PriorityVeryHigh, "sizeof(int): %i", sizeof(int));
	debugger.Print(Debugger::PriorityVeryHigh, "sizeof(long): %i", sizeof(long));
	debugger.Print(Debugger::PriorityVeryHigh, "sizeof(bool): %i", sizeof(bool));
	debugger.Print(Debugger::PriorityVeryHigh, "sizeof(DRIVETRAIN::DRIVE_WHEELS): %i",
		sizeof(DRIVETRAIN::DRIVE_WHEELS));
	debugger.Print(Debugger::PriorityVeryHigh, "sizeof(Vector): %i", sizeof(Vector));
	debugger.Print(Debugger::PriorityVeryHigh, "sizeof(Vector_SET): %i", sizeof(Vector_SET));
	debugger.Print(Debugger::PriorityVeryHigh, "sizeof(WHEEL_SET): %i", sizeof(WHEEL_SET));//*/
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		~MAIN_FRAME
//
// Description:		Destructor for MAIN_FRAME class.  Frees memory and
//					releases GUI object managers.
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
MAIN_FRAME::~MAIN_FRAME()
{
	// Delete the file history manager
	delete RecentFileManager;
	RecentFileManager = NULL;

	// Delete the job queue object
	delete jobQueue;
	jobQueue = NULL;

	// Remove all of the cars we're managing that haven't been closed
	while (OpenObjectList.GetCount() > 0)
		RemoveObjectFromList(0);

	// Deinitialize the manager
	Manager.UnInit();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		Constant Declarations
//
// Description:		Constant declarations for the MAIN_FRAME class.
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
#ifdef __WXGTK__
const wxString MAIN_FRAME::PathToConfigFile = _T("vvase.rc");
#else
const wxString MAIN_FRAME::PathToConfigFile = _T("config.ini");
#endif

//==========================================================================
// Class:			MAIN_FRAME
// Function:		DoLayout
//
// Description:		Creates the layout for this window and positions the
//					form on the screen.
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
void MAIN_FRAME::DoLayout(void)
{
	// Tell the frame manager to manage this window
	Manager.SetManagedWindow(this);

	// Add the panes to the manager - this order is important (see below)
	// It's necessary to use the wxAuiPaneInfo() method because we need to assign internal names
	// to make calls to SavePerspective() and LoadPerspective() work properly.
	Manager.AddPane(Notebook, wxAuiPaneInfo().Name(_T("Main Notebook")).CenterPane());
	Manager.AddPane(DebugPane, wxAuiPaneInfo().Name(_T("Debug Pane")).Bottom().Caption(_T("Output")));
	// For some reason, these get reversed under Linux
#ifdef __WXGTK__
	Manager.AddPane(EditPanel, wxAuiPaneInfo().Name(_T("Edit Panel")).Left().Caption(_T("Edit Sub-Systems")));
	Manager.AddPane(SystemsTree, wxAuiPaneInfo().Name(_T("Systems Tree")).Left().Caption(_T("Systems Tree")));
#else
	Manager.AddPane(SystemsTree, wxAuiPaneInfo().Name(_T("Systems Tree")).Left().Caption(_T("Systems Tree")));
	Manager.AddPane(EditPanel, wxAuiPaneInfo().Name(_T("Edit Panel")).Left().Caption(_T("Edit Sub-Systems")));
#endif
	Manager.AddPane(OutputPanel, wxAuiPaneInfo().Name(_T("Output List")).Right().Caption(_T("Output List")));

	// This layer stuff is required to get the desired initial layout (see below)
	Manager.GetPane(DebugPane).Layer(0);
	Manager.GetPane(Notebook).Layer(0);
	Manager.GetPane(SystemsTree).Layer(1);
	Manager.GetPane(EditPanel).Layer(1);
	Manager.GetPane(OutputPanel).Layer(0);

	/* Desired initial layout is as follows:
	=============================================================
	|Toolbars													|
	|-----------------------------------------------------------|
	| Systems	| Notebook								|		|
	| Tree		|										|Output	|
	|			|										|Panel	|
	|			|										|		|
	|			|										|		|
	|			|										|		|
	|			|										|		|
	|-----------|										|		|
	| Edit		|										|		|
	| Panel		|										|		|
	|			|										|		|
	|			|										|		|
	|			|-----------------------------------------------|
	|			| Output Pane									|
	|			|												|
	|			|												|
	|			|												|
	===========================================================*/

	// Have the manager update the layout
	Manager.Update();

	// Configure the output panel to show units
	OutputPanel->FinishUpdate(0);
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		SetProperties
//
// Description:		Sets the window properties for this window.  Includes
//					title, frame size, and default font.
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
void MAIN_FRAME::SetProperties(void)
{
	// MAIN_FRAME properties
	SetTitle(CarDesignerName);
	SetName(CarDesignerName);
	SetSize(1024, 768);
	Center();

	// Add the icons
	wxIconBundle bundle;
#ifdef __WXMSW__
	bundle.AddIcon(wxIcon(_T("ICON_ID_MAIN"), wxBITMAP_TYPE_ICO_RESOURCE, 16, 16));
	bundle.AddIcon(wxIcon(_T("ICON_ID_MAIN"), wxBITMAP_TYPE_ICO_RESOURCE, 32, 32));
	bundle.AddIcon(wxIcon(_T("ICON_ID_MAIN"), wxBITMAP_TYPE_ICO_RESOURCE, 48, 48));
#elif __WXGTK__
	bundle.AddIcon(wxIcon(aavase16_xpm, wxBITMAP_TYPE_XPM));
	bundle.AddIcon(wxIcon(aavase32_xpm, wxBITMAP_TYPE_XPM));
	bundle.AddIcon(wxIcon(aavase48_xpm, wxBITMAP_TYPE_XPM));
#endif
	SetIcons(bundle);

	// StatusBar properties
	int StatusBarWidths[] = {-1, 100};
	StatusBar->SetStatusWidths(2, StatusBarWidths);
	StatusBar->SetStatusText(_T("Ready"), 0);
	StatusBar->SetStatusText(wxEmptyString, 1);

	// Set up the debugger
	debugger.SetTargetOutput(DebugPane);
	debugger.SetDebugLevel(Debugger::PriorityHigh);

	// OutputPane properties
	wxString FontFaceName;
	wxTextAttr OutputAttributes;
	wxFont OutputFont;

	// FIXME:  Give user the option of selecting a font?  Maybe in options dialog?  Then only do this if unset?
	wxArrayString preferredFonts;
	preferredFonts.Add(_T("Monospace"));// GTK preference
	preferredFonts.Add(_T("Courier New"));// MSW preference
	bool foundPreferredFont = FontFinder::GetFontFaceName(wxFONTENCODING_SYSTEM, preferredFonts, true, FontFaceName);

	// As long as we have a font name to use, set the font
	if (!FontFaceName.IsEmpty())
	{
		// Populate the wxFont object
		OutputFont.SetPointSize(9);
		OutputFont.SetFamily(wxFONTFAMILY_MODERN);
		if (!OutputFont.SetFaceName(FontFaceName))
			debugger.Print(_T("Error setting font face to ") + FontFaceName);

		// Assign the font to the window
		OutputAttributes.SetFont(OutputFont);
		if (!DebugPane->SetDefaultStyle(OutputAttributes))
			debugger.Print(_T("Error setting font style"));
	}

	// Tell the user if we're using a font we're unsure of
	if (!foundPreferredFont)
		debugger.Print(_T("Could not find preferred fixed-width font; using ") + FontFaceName);// FIXME:  If we make this a configuration option, tell the user here

	// Also put the cursor at the bottom of the text, so the window scrolls automatically
	// as it updates with text
	// NOTE:  This already works as desired under GTK, issue is with MSW
	// FIXME:  This doesn't work!

	// Add the application level entry to the SystemsTree (this one is hidden, but necessary)
	SystemsTree->AddRoot(_T("Application Level"), -1, -1);

	// Do the disabling/checking of certain controls
	MenuBar->Check(IdMenuViewToolbarsKinematic, true);

	// Disable the undo and redo menu items
	DisableUndo();
	DisableRedo();

	// This section disables all menu items that are not yet implemented
	MenuBar->FindItem(IdMenuEditCut)->Enable(false);
	MenuBar->FindItem(IdMenuEditCopy)->Enable(false);
	MenuBar->FindItem(IdMenuEditPaste)->Enable(false);
	MenuBar->FindItem(IdMenuToolsDoE)->Enable(false);
	MenuBar->FindItem(IdMenuToolsDynamic)->Enable(false);

	// Set up the unit converter
	converter.SetAngleUnits(Convert::DEGREES);
	converter.SetDistanceUnits(Convert::INCH);
	converter.SetAreaUnits(Convert::INCH_SQUARED);
	converter.SetForceUnits(Convert::POUND_FORCE);
	converter.SetPressureUnits(Convert::POUND_FORCE_PER_SQUARE_INCH);
	converter.SetMomentUnits(Convert::FOOT_POUND_FORCE);
	converter.SetMassUnits(Convert::SLUG);
	converter.SetVelocityUnits(Convert::INCHES_PER_SECOND);
	converter.SetAccelerationUnits(Convert::INCHES_PER_SECOND_SQUARED);
	converter.SetInertiaUnits(Convert::SLUG_INCHES_SQUARED);
	converter.SetDensityUnits(Convert::SLUGS_PER_INCH_CUBED);
	converter.SetPowerUnits(Convert::HORSEPOWER);
	converter.SetEnergyUnits(Convert::BRITISH_THERMAL_UNIT);
	converter.SetTemperatureUnits(Convert::FAHRENHEIT);

	// Allow draging-and-dropping of files onto this window to open them
	SetDropTarget(dynamic_cast<wxDropTarget*>(new DropTarget(*this)));
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		InitializeSolver
//
// Description:		Initializes solver settings.
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
void MAIN_FRAME::InitializeSolver(void)
{
	// Initialize the job queue
	jobQueue = new JobQueue(GetEventHandler());

	// Initialize thread-related variables
	ActiveThreads = 0;
	OpenJobCount = 0;
	NumberOfThreads = 0;

	// Pass the debugger information objects that have a static pointer to the debugger
	Suspension::SetDebugger(debugger);
	KinematicOutputs::SetDebugger(debugger);

	// Zero out kinematic inputs
	KinematicInputs.pitch = 0.0;
	KinematicInputs.roll = 0.0;
	KinematicInputs.heave = 0.0;
	KinematicInputs.rackTravel = 0.0;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		SetNumberOfThreads
//
// Description:		Sets the number of worker threads to the specified value.
//					Handles creation or deletion of threads as necessary to
//					ensure the correct number of threads are left.
//
// Input Arguments:
//		NewNumberOfThreads	= unsigned int specifying the number of
//							  threads desired
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::SetNumberOfThreads(unsigned int NewNumberOfThreads)
{
	// Make sure we've got at least one thread
	if (NewNumberOfThreads < 1)
		NewNumberOfThreads = 1;

	// Determine if we need to increase or decrease the number of threads we have
	unsigned int i;
	if (NewNumberOfThreads > NumberOfThreads)
	{
		// Spawn threads for the thread pool
		for (i = NumberOfThreads; i < NewNumberOfThreads; i++)
		{
			// Keep track of jobs by counting them as they're sent to the
			// threads (starting a thread counts as a job)
			OpenJobCount++;

			// These threads will delete themselves after an EXIT job
			WorkerThread *NewThread = new WorkerThread(jobQueue, debugger, i);
			// FIXME:  If we want to set priority, this is where it needs to happen
			NewThread->Run();
		}
	}
	else if (NewNumberOfThreads < NumberOfThreads)
	{
		// Kill the necessary number of threads
		for (i = NumberOfThreads; i > NewNumberOfThreads; i--)
			jobQueue->AddJob(ThreadJob(ThreadJob::CommandThreadExit), JobQueue::PriorityVeryHigh);
	}

	// Update the class member for the desired number of threads
	NumberOfThreads = NewNumberOfThreads;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		CreateMenuBar
//
// Description:		Creates the menu bar and all of the sub-menus.
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
void MAIN_FRAME::CreateMenuBar(void)
{
	MenuBar = new wxMenuBar();

	// Car menu
	wxMenu *mnuFile = new wxMenu();
	wxMenu *mnuFileNew = new wxMenu();
	mnuFileNew->Append(IdMenuFileNewCar, _T("&Car\tCtrl+N"), _T("Create new car file"), wxITEM_NORMAL);
	mnuFileNew->Append(IdMenuFileNewIteration, _T("&Static Iteration\tCtrl+I"),
		_T("Create new static iteration analysis"), wxITEM_NORMAL);
	mnuFileNew->Append(IdMenuFileNewOptimization, _T("&Genetic Optimization\tCtrl+G"),
		_T("Create new genetic algorithm optimization"), wxITEM_NORMAL);
	mnuFile->AppendSubMenu(mnuFileNew, _T("New"));
	mnuFile->AppendSeparator();
	mnuFile->Append(IdMenuFileOpen, _T("&Open\tCtrl+O"), _T("Open saved files"), wxITEM_NORMAL);
	mnuFile->AppendSeparator();
	mnuFile->Append(IdMenuFileClose, _T("&Close"), _T("Close current car file"), wxITEM_NORMAL);
	mnuFile->Append(IdMenuFileCloseAll, _T("Close All"), _T("Close all files"), wxITEM_NORMAL);
	mnuFile->AppendSeparator();
	mnuFile->Append(IdMenuFileSave, _T("&Save\tCtrl+S"), _T("Save current file"), wxITEM_NORMAL);
	mnuFile->Append(IdMenuFileSaveAs, _T("Save &As"), _T("Save current file as new file"), wxITEM_NORMAL);
	mnuFile->Append(IdMenuFileSaveAll, _T("Save A&ll"), _T("Save all open files"), wxITEM_NORMAL);
	mnuFile->AppendSeparator();
	mnuFile->Append(IdMenuFileWriteImageFile, _T("&Write Image File\tCtrl+W"),
		_T("Save window contents to image file"), wxITEM_NORMAL);
	mnuFile->AppendSeparator();
	wxMenu *mnuRecentFiles = new wxMenu();
	mnuFile->AppendSubMenu(mnuRecentFiles, _T("&Recent Files"));
	mnuFile->Append(IdMenuFileOpenAllRecent, _T("Open All Recent Files"),
		_T("Open all files in the Recent Files list"), wxITEM_NORMAL);
	mnuFile->AppendSeparator();
	mnuFile->Append(IdMenuFileExit, _T("E&xit\tAlt+F4"), _T("Exit ") + CarDesignerName, wxITEM_NORMAL);
	MenuBar->Append(mnuFile, _T("&File"));

	// Edit menu
	wxMenu *mnuEdit = new wxMenu();
	mnuEdit->Append(IdMenuEditUndo, _T("&Undo\tCtrl+Z"), _T("Undo last action"), wxITEM_NORMAL);
	mnuEdit->Append(IdMenuEditRedo, _T("&Redo\tCtrl+Y"), _T("Redo last previously undone action"), wxITEM_NORMAL);
	mnuEdit->AppendSeparator();
	mnuEdit->Append(IdMenuEditCut, _T("&Cut\tCtrl+X"), _T("Cut selected to clipboard"), wxITEM_NORMAL);
	mnuEdit->Append(IdMenuEditCopy, _T("C&opy\tCtrl+C"), _T("Copy selected to clipboard"), wxITEM_NORMAL);
	mnuEdit->Append(IdMenuEditPaste, _T("&Paste\tCtrl+V"), _T("Paste from clipboard"), wxITEM_NORMAL);
	MenuBar->Append(mnuEdit, _T("&Edit"));

	// View menu
	wxMenu *mnuView = new wxMenu();
	wxMenu *mnuViewToolbars = new wxMenu();
	mnuViewToolbars->AppendCheckItem(IdMenuViewToolbarsKinematic, _T("Kinematic Analysis"));
	mnuView->AppendSubMenu(mnuViewToolbars, _T("Toolbars"));
	mnuView->AppendSeparator();
	mnuView->Append(IdMenuViewClearOutput, _T("&Clear Output Text"),
		_T("Clear all text from the output pane"), wxITEM_NORMAL);
	MenuBar->Append(mnuView, _T("&View"));

	// Tools menu
	wxMenu *mnuTools = new wxMenu();
	mnuTools->Append(IdMenuToolsDoE, _T("Design of &Experiment"),
		_T("Open design of experiments tool"), wxITEM_NORMAL);
	mnuTools->Append(IdMenuToolsDynamic, _T("&Dynamic Analysis"),
		_T("Start Dynamic Analysis Wizard"), wxITEM_NORMAL);
	mnuTools->AppendSeparator();
	mnuTools->Append(IdMenuToolsOptions, _T("&Options"), _T("Edit application preferences"), wxITEM_NORMAL);
	MenuBar->Append(mnuTools, _T("&Tools"));

	// Help menu
	wxMenu *mnuHelp = new wxMenu();
	mnuHelp->Append(IdMenuHelpManual, _T("&User's Manual\tF1"), _T("Display user's manual"), wxITEM_NORMAL);
	mnuHelp->AppendSeparator();
	mnuHelp->Append(IdMenuHelpAbout, _T("&About"), _T("Show About dialog"), wxITEM_NORMAL);
	MenuBar->Append(mnuHelp, _T("&Help"));

	// Add the manager for the recently used file menu
	RecentFileManager = new wxFileHistory(MaxRecentFiles, IdMenuFileRecentStart);
	RecentFileManager->UseMenu(mnuRecentFiles);

	// Now make it official
	SetMenuBar(MenuBar);
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		CreateKinematicAnalysisToolbar
//
// Description:		Creates the toolbar and adds the buttons and icons. Also
//					adds the toolbar to the frame in the appropriate
//					position.
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
void MAIN_FRAME::CreateKinematicAnalysisToolbar(void)
{
	// Make sure we don't already have one of these
	if (KinematicToolbar != NULL)
		return;

	// Create the toolbar
	KinematicToolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxTB_FLAT | wxTB_NODIVIDER);

	// Create the controls
	wxStaticText *PitchLabel = new wxStaticText(KinematicToolbar, wxID_ANY, _T("Pitch"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxTextCtrl *PitchSet = new wxTextCtrl(KinematicToolbar, IdToolbarKinematicPitch, _T("0"), wxDefaultPosition,
		wxSize(40, -1));
	PitchSet->SetMaxLength(5);

	wxStaticText *RollLabel = new wxStaticText(KinematicToolbar, wxID_ANY, _T("Roll"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxTextCtrl *RollSet = new wxTextCtrl(KinematicToolbar, IdToolbarKinematicRoll, _T("0"), wxDefaultPosition,
		wxSize(40, -1));
	RollSet->SetMaxLength(5);

	wxStaticText *HeaveLabel = new wxStaticText(KinematicToolbar, wxID_ANY, _T("Heave"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxTextCtrl *HeaveSet = new wxTextCtrl(KinematicToolbar, IdToolbarKinematicHeave, _T("0"), wxDefaultPosition,
		wxSize(40, -1));
	HeaveSet->SetMaxLength(5);

	wxStaticText *SteerLabel = new wxStaticText(KinematicToolbar, wxID_ANY, _T("Steer"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxTextCtrl *SteerSet = new wxTextCtrl(KinematicToolbar, IdToolbarKinematicSteer, _T("0"), wxDefaultPosition,
		wxSize(40, -1));
	SteerSet->SetMaxLength(5);

	// Adjust the spacing a little bit

	// Add the controls to the toolbar
	KinematicToolbar->AddControl(PitchLabel);
	KinematicToolbar->AddControl(PitchSet);
	KinematicToolbar->AddSeparator();
	KinematicToolbar->AddControl(RollLabel);
	KinematicToolbar->AddControl(RollSet);
	KinematicToolbar->AddSeparator();
	KinematicToolbar->AddControl(HeaveLabel);
	KinematicToolbar->AddControl(HeaveSet);
	KinematicToolbar->AddSeparator();
	KinematicToolbar->AddControl(SteerLabel);
	KinematicToolbar->AddControl(SteerSet);

	// Make the toolbar come alive
	KinematicToolbar->Realize();

	// And add it to the wxAui Manager
	Manager.AddPane(KinematicToolbar, wxAuiPaneInfo().Name(_T("KinematicToolbar")).
		Caption(_T("Kinematic Analysis")).ToolbarPane().Top().Row(1).Position(1).
		LeftDockable(false).RightDockable(false));
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
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
BEGIN_EVENT_TABLE(MAIN_FRAME, wxFrame)
	// Frame top level
	EVT_CLOSE(MAIN_FRAME::WindowCloseEvent)
	EVT_SIZE(MAIN_FRAME::OnSizeEvent)

	// Menu bar
	EVT_MENU(IdMenuFileNewCar,					MAIN_FRAME::FileNewCarEvent)
	EVT_MENU(IdMenuFileNewIteration,			MAIN_FRAME::FileNewIterationEvent)
	EVT_MENU(IdMenuFileNewOptimization,			MAIN_FRAME::FileNewOptimizationEvent)
	EVT_MENU(IdMenuFileOpen,					MAIN_FRAME::FileOpenEvent)
	EVT_MENU(IdMenuFileClose,					MAIN_FRAME::FileCloseEvent)
	EVT_MENU(IdMenuFileCloseAll,				MAIN_FRAME::FileCloseAllEvent)
	EVT_MENU(IdMenuFileSave,					MAIN_FRAME::FileSaveEvent)
	EVT_MENU(IdMenuFileSaveAs,					MAIN_FRAME::FileSaveAsEvent)
	EVT_MENU(IdMenuFileSaveAll,					MAIN_FRAME::FileSaveAllEvent)
	EVT_MENU(IdMenuFileWriteImageFile,			MAIN_FRAME::FileWriteImageFileEvent)
	EVT_MENU(IdMenuFileOpenAllRecent,			MAIN_FRAME::FileOpenAllRecentEvent)
	EVT_MENU(IdMenuFileExit,					MAIN_FRAME::FileExitEvent)
	EVT_MENU(wxID_ANY,							MAIN_FRAME::OtherMenuClickEvents)

	EVT_MENU(IdMenuEditUndo,					MAIN_FRAME::EditUndoEvent)
	EVT_MENU(IdMenuEditRedo,					MAIN_FRAME::EditRedoEvent)
	EVT_MENU(IdMenuEditCut,						MAIN_FRAME::EditCutEvent)
	EVT_MENU(IdMenuEditCopy,					MAIN_FRAME::EditCopyEvent)
	EVT_MENU(IdMenuEditPaste,					MAIN_FRAME::EditPasteEvent)

	EVT_MENU(IdMenuCarAppearanceOptions,		MAIN_FRAME::CarAppearanceOptionsEvent)

	EVT_MENU(IdMenuIterationShowAssociatedCars,	MAIN_FRAME::IterationShowAssociatedCarsClickEvent)
	EVT_MENU(IdMenuIterationAssociatedWithAllCars,	MAIN_FRAME::IterationAssociatedWithAllCarsClickEvent)
	EVT_MENU(IdMenuIterationExportDataToFile,	MAIN_FRAME::IterationExportDataToFileClickEvent)
	EVT_MENU(IdMenuIterationXAxisPitch,			MAIN_FRAME::IterationXAxisPitchClickEvent)
	EVT_MENU(IdMenuIterationXAxisRoll,			MAIN_FRAME::IterationXAxisRollClickEvent)
	EVT_MENU(IdMenuIterationXAxisHeave,			MAIN_FRAME::IterationXAxisHeaveClickEvent)
	EVT_MENU(IdMenuIterationXAxisRackTravel,	MAIN_FRAME::IterationXAxisRackTravelClickEvent)

	EVT_MENU(IdMenuViewToolbarsKinematic,		MAIN_FRAME::ViewToolbarsKinematicEvent)
	EVT_MENU(IdMenuViewClearOutput,				MAIN_FRAME::ViewClearOutputEvent)

	EVT_MENU(IdMenuToolsDoE,					MAIN_FRAME::ToolsDoEEvent)
	EVT_MENU(IdMenuToolsDynamic,				MAIN_FRAME::ToolsDynamicEvent)
	EVT_MENU(IdMenuToolsOptions,				MAIN_FRAME::ToolsOptionsEvent)

	EVT_MENU(IdMenuHelpManual,					MAIN_FRAME::HelpManualEvent)
	EVT_MENU(IdMenuHelpAbout,					MAIN_FRAME::HelpAboutEvent)

	// Toolbars
	// Static Analysis
	EVT_TEXT(IdToolbarKinematicPitch,			MAIN_FRAME::KinematicToolbarPitchChangeEvent)
	EVT_TEXT(IdToolbarKinematicRoll,			MAIN_FRAME::KinematicToolbarRollChangeEvent)
	EVT_TEXT(IdToolbarKinematicHeave,			MAIN_FRAME::KinematicToolbarHeaveChangeEvent)
	EVT_TEXT(IdToolbarKinematicSteer,			MAIN_FRAME::KinematicToolbarSteerChangeEvent)

	// Threads
	EVT_COMMAND(wxID_ANY, EVT_THREAD,			MAIN_FRAME::ThreadCompleteEvent)
	EVT_COMMAND(wxID_ANY, EVT_DEBUG,			MAIN_FRAME::DebugMessageEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			MAIN_FRAME
// Function:		FileNewCarEvent
//
// Description:		Generates a new GUI_CAR object and adds the car to the
//					list of managed objects.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::FileNewCarEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create a new GUI_OBJECT with type TYPE_CAR
	GUI_OBJECT *TempObject = new GUI_CAR(*this, debugger);

	// Make the new object active
	SetActiveIndex(TempObject->GetIndex());
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		FileNewIterationEvent
//
// Description:		Generates a new ITERATION object and adds the it to the
//					list of managed objects
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::FileNewIterationEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create a new GUI_OBJECT with type TYPE_ITERATION
	GUI_OBJECT *TempObject = new ITERATION(*this, debugger);

	// Make the new object active or remove it from the list (user canceled)
	if (TempObject->IsInitialized())
		SetActiveIndex(TempObject->GetIndex());
	else
		RemoveObjectFromList(TempObject->GetIndex());
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		FileNewOptimizationEvent
//
// Description:		Generates a new GENETIC_OPTIMIZATION object and adds the
//					it to the list of managed objects
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::FileNewOptimizationEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create a new GUI_OBJECT with type TYPE_ITERATION
	GUI_OBJECT *TempObject = new GENETIC_OPTIMIZATION(*this, debugger, converter);

	// Make the new object active
	SetActiveIndex(TempObject->GetIndex());

}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		FileOpenEvent
//
// Description:		Displays a dialog asking the user to specify the file to
//					read from.  Creates a new GUI_OBJECT, loading the
//					contents from the specified file name.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::FileOpenEvent(wxCommandEvent& WXUNUSED(event))
{
	// Set up the wildcard specifications
	// (Done here for readability)
	wxString Wildcard("VVASE files (*.car; *.iteration; *.ga)|*.car;*.iteration;*.ga|");
	Wildcard.append("Car files (*.car)|*.car");
	Wildcard.append("|Iteration files (*.iteration)|*.iteration");
	Wildcard.append("|Optimization files (*.ga)|*ga");

	// Get the file name to open from the user
	wxArrayString PathsAndFileNames = GetFileNameFromUser(_T("Open"), wxEmptyString, wxEmptyString,
		Wildcard, wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);

	// Make sure the user didn't cancel
	if (PathsAndFileNames.GetCount() == 0)
		return;

	// Loop to make sure we open all selected files
	unsigned int File;
	for (File = 0; File < PathsAndFileNames.GetCount(); File++)
		LoadFile(PathsAndFileNames[File]);
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		FileCloseEvent
//
// Description:		Calls the object of interests's close method.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::FileCloseEvent(wxCommandEvent& WXUNUSED(event))
{
	// Check to make sure there is an object to close
	if (OpenObjectList.GetCount() > 0)
		// Close the object of interest
		OpenObjectList[ObjectOfInterestIndex]->Close();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		FileCloseAllEvent
//
// Description:		Calls all of the open GUI_OBJECTs' close methods.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::FileCloseAllEvent(wxCommandEvent& WXUNUSED(event))
{
	// Close all of the objects
	int IndexToDelete = 0;
	while (OpenObjectList.GetCount() > IndexToDelete)
	{
		// If the user chooses not to close a particular object, we need to
		// increment our target index so that we don't keep asking about the same object.
		if (!OpenObjectList[IndexToDelete]->Close())
			IndexToDelete++;
	}
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		FileSaveEvent
//
// Description:		Calls the object of interest's save method.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::FileSaveEvent(wxCommandEvent& WXUNUSED(event))
{
	// Check to make sure there is an object to save
	if (OpenObjectList.GetCount() > 0)
		// Save the object of interest
		OpenObjectList[ObjectOfInterestIndex]->SaveToFile();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		FileSaveAsEvent
//
// Description:		Calls the active object's save method and asks for a new
//					file name.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::FileSaveAsEvent(wxCommandEvent& WXUNUSED(event))
{
	// Check to make sure there is an object to save
	if (OpenObjectList.GetCount() > 0)
		// Save the object of interest
		OpenObjectList[ObjectOfInterestIndex]->SaveToFile(true);
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		FileSaveAllEvent
//
// Description:		Calls all of the open GUI_OBJECTs' save methods
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::FileSaveAllEvent(wxCommandEvent& WXUNUSED(event))
{
	// Save all of the objects
	int IndexToSave = 0;
	while (OpenObjectList.GetCount() > IndexToSave)
	{
		// Save the object with index IndexToSave
		if (!OpenObjectList[IndexToSave]->SaveToFile())
			break;

		// Increment our save index
		IndexToSave++;
	}
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		FileOpenAllRecentEvent
//
// Description:		Opens all files in the Recent Files list.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::FileOpenAllRecentEvent(wxCommandEvent& WXUNUSED(event))
{
	// Try to open every file in the list
	unsigned int i;
	for (i = 0; i < RecentFileManager->GetCount(); i++)
		LoadFile(RecentFileManager->GetHistoryFile(i));
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		FileWriteImageFileEvent
//
// Description:		Calls the object of interest's write image file method.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::FileWriteImageFileEvent(wxCommandEvent& WXUNUSED(event))
{
	// Check to make sure there is an object open
	if (OpenObjectList.GetCount() < 1)
		return;

	// Get the file name to open from the user
	wxArrayString PathAndFileName = GetFileNameFromUser(_T("Save Image File"), wxEmptyString, wxEmptyString,
		_T("Bitmap Image (*.bmp)|*.bmp|JPEG Image (*.jpg)|*.jpg|PNG Image (*.png)|*.png|TIFF Image (*.tif)|*.tif"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	// Make sure the user didn't cancel
	if (PathAndFileName.IsEmpty())
		return;

	// Call the object's write image file method
	if (OpenObjectList[ObjectOfInterestIndex]->WriteImageToFile(PathAndFileName[0]))
		debugger.Print(Debugger::PriorityHigh, "Image file written to %s", PathAndFileName[0].c_str());
	else
		debugger.Print(_T("Image file NOT written!"), Debugger::PriorityHigh);
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		FileExitEvent
//
// Description:		Attempts to close this form.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::FileExitEvent(wxCommandEvent& WXUNUSED(event))
{
	// Shut down this application
	// User confirmation, etc. is handled by the CloseEvent method,
	// which is called when the form tries to close.  If we put our own
	// code here, the user is asked for confirmation twice.
	Close(true);
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		OtherMenuClickEvents
//
// Description:		Handles menu events not specifically caught by other functions.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::OtherMenuClickEvents(wxCommandEvent &event)
{
	// If the ID isn't in the right range, skip the event and return
	if (event.GetId() < IdMenuFileRecentStart || event.GetId() > IdMenuFileRecentLast)
	{
		event.Skip();
		return;
	}

	// Make sure the index isn't greater than the number of files we actually have in the list
	if ((unsigned int)(event.GetId() - IdMenuFileRecentStart) >= RecentFileManager->GetCount())
		return;

	// Attempt to load the specified file
	LoadFile(RecentFileManager->GetHistoryFile(event.GetId() - IdMenuFileRecentStart));
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		EditUndoEvent
//
// Description:		Event handler for the Edit menu's Undo item.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::EditUndoEvent(wxCommandEvent& WXUNUSED(event))
{
	// Undo the last operation
	UndoRedo.Undo();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		EditRedoEvent
//
// Description:		Event handler for the Edit menu's Redo item.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::EditRedoEvent(wxCommandEvent& WXUNUSED(event))
{
	// Redo the last undone operation
	UndoRedo.Redo();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		EditCutEvent
//
// Description:		Event handler for the Edit menu's Cut item.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::EditCutEvent(wxCommandEvent &event)
{
	event.Skip();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		EditCopyEvent
//
// Description:		Event handler for the Edit menu's Copy item.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::EditCopyEvent(wxCommandEvent &event)
{
	event.Skip();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		EditPasteEvent
//
// Description:		Event handler for the Edit menu's Paste item.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::EditPasteEvent(wxCommandEvent &event)
{
	event.Skip();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		CarAppearanceOptionsEvent
//
// Description:		Calls the ShowAppearanceOptionsDialog() function if the
//					object of interest is a TYPE_CAR.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::CarAppearanceOptionsEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object of interest is a car
	if (OpenObjectList[ObjectOfInterestIndex]->GetType() != GUI_OBJECT::TYPE_CAR)
		return;

	// Show the appearance options dialog for the car
	static_cast<GUI_CAR*>(OpenObjectList[ObjectOfInterestIndex])
		->GetAppearanceOptions().ShowAppearanceOptionsDialog();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		IterationShowAssociatedCarsClickEvent
//
// Description:		For ITERATION objects - calls the method that displays
//					a dialog allowing the user to select the associated cars.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::IterationShowAssociatedCarsClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object is TYPE_ITERATION
	if (OpenObjectList[ObjectOfInterestIndex]->GetType() != GUI_OBJECT::TYPE_ITERATION)
		return;

	// Call the ShowAssociatedCarsDialog() method
	static_cast<ITERATION*>(OpenObjectList[ObjectOfInterestIndex])->ShowAssociatedCarsDialog();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		IterationAssociatedWithAllCarsClickEvent
//
// Description:		For ITERATION objects - toggles the auto-associate function
//					for the object of interest.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::IterationAssociatedWithAllCarsClickEvent(wxCommandEvent &event)
{
	// Make sure the object is TYPE_ITERATION
	if (OpenObjectList.GetObject(ObjectOfInterestIndex)->GetType() != GUI_OBJECT::TYPE_ITERATION)
		return;

	// If this object is checked, set the auto-associate flag to true, otherwise
	// set it to false
	if (event.IsChecked())
		static_cast<ITERATION*>(OpenObjectList.GetObject(ObjectOfInterestIndex))->SetAutoAssociate(true);
	else
		static_cast<ITERATION*>(OpenObjectList.GetObject(ObjectOfInterestIndex))->SetAutoAssociate(false);
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		IterationExportDataToFileClickEvent
//
// Description:		For ITERATION objects.  Calls a method that exports the
//					kinematic output data to a user-specified file.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::IterationExportDataToFileClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object is TYPE_ITERATION
	if (OpenObjectList.GetObject(ObjectOfInterestIndex)->GetType() != GUI_OBJECT::TYPE_ITERATION)
		return;

	// Get the file name to export to
	wxArrayString PathAndFileName = GetFileNameFromUser(_T("Save As"), wxGetHomeDir(), wxEmptyString,
		_T("Tab delimited (*.txt)|*.txt|Comma Separated Values (*.csv)|*.csv"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	// Make sure the user didn't cancel
	if (PathAndFileName.IsEmpty())
		return;

	// Call the ExportDataToFile() method
	static_cast<ITERATION*>(OpenObjectList.GetObject(ObjectOfInterestIndex))
		->ExportDataToFile(PathAndFileName.Item(0));
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		IterationXAxisPitchClickEvent
//
// Description:		Calls the ObjectOfInterest's save method.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::IterationXAxisPitchClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object is TYPE_ITERATION
	if (OpenObjectList.GetObject(ObjectOfInterestIndex)->GetType() != GUI_OBJECT::TYPE_ITERATION)
		return;

	// Set the X axis to pitch
	static_cast<ITERATION*>(OpenObjectList.GetObject(ObjectOfInterestIndex))
		->SetXAxisType(ITERATION::AxisTypePitch);

	// Uncheck all other X axis options
	if (MenuBar->FindItem(IdMenuIterationXAxisPitch) != NULL)
	{
		MenuBar->Check(IdMenuIterationXAxisRoll, false);
		MenuBar->Check(IdMenuIterationXAxisHeave, false);
		MenuBar->Check(IdMenuIterationXAxisRackTravel, false);
	}
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		IterationXAxisRollClickEvent
//
// Description:		Calls the ObjectOfInterest's save method.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::IterationXAxisRollClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object is TYPE_ITERATION
	if (OpenObjectList.GetObject(ObjectOfInterestIndex)->GetType() != GUI_OBJECT::TYPE_ITERATION)
		return;

	// Set the X axis to roll
	static_cast<ITERATION*>(OpenObjectList.GetObject(ObjectOfInterestIndex))
		->SetXAxisType(ITERATION::AxisTypeRoll);

	// Uncheck all other X axis options
	if (MenuBar->FindItem(IdMenuIterationXAxisRoll) != NULL)
	{
		MenuBar->Check(IdMenuIterationXAxisPitch, false);
		MenuBar->Check(IdMenuIterationXAxisHeave, false);
		MenuBar->Check(IdMenuIterationXAxisRackTravel, false);
	}
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		IterationXAxisHeaveClickEvent
//
// Description:		Calls the ObjectOfInterest's save method.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::IterationXAxisHeaveClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object is TYPE_ITERATION
	if (OpenObjectList.GetObject(ObjectOfInterestIndex)->GetType() != GUI_OBJECT::TYPE_ITERATION)
		return;

	// Set the X axis to heave
	static_cast<ITERATION*>(OpenObjectList.GetObject(ObjectOfInterestIndex))
		->SetXAxisType(ITERATION::AxisTypeHeave);

	// Uncheck all other X axis options
	if (MenuBar->FindItem(IdMenuIterationXAxisHeave) != NULL)
	{
		MenuBar->Check(IdMenuIterationXAxisPitch, false);
		MenuBar->Check(IdMenuIterationXAxisRoll, false);
		MenuBar->Check(IdMenuIterationXAxisRackTravel, false);
	}
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		IterationXAxisRackTravelClickEvent
//
// Description:		Calls the ObjectOfInterest's save method.
//
// Input Arguments:
//		event	= wxContextMenuEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::IterationXAxisRackTravelClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object is TYPE_ITERATION
	if (OpenObjectList.GetObject(ObjectOfInterestIndex)->GetType() != GUI_OBJECT::TYPE_ITERATION)
		return;

	// Set the X axis to rack travel
	static_cast<ITERATION*>(OpenObjectList.GetObject(ObjectOfInterestIndex))
		->SetXAxisType(ITERATION::AxisTypeRackTravel);

	// Uncheck all other X axis options
	if (MenuBar->FindItem(IdMenuIterationXAxisRackTravel) != NULL)
	{
		MenuBar->Check(IdMenuIterationXAxisPitch, false);
		MenuBar->Check(IdMenuIterationXAxisRoll, false);
		MenuBar->Check(IdMenuIterationXAxisHeave, false);
	}
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		ViewToolbarsKinematicEvent
//
// Description:		Event handler for the View menu's Kinematic Toolbar item.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::ViewToolbarsKinematicEvent(wxCommandEvent &event)
{
	// Determine if we are supposed to hide or show the menu
	// FIXME:  This is still a little funky...
	if (event.IsChecked())
		CreateKinematicAnalysisToolbar();
	else if (KinematicToolbar != NULL)
	{
		Manager.DetachPane(KinematicToolbar);

		delete KinematicToolbar;
		KinematicToolbar = NULL;
	}
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		ViewClearOutputEvent
//
// Description:		Clears all of the text in the OutputPane.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::ViewClearOutputEvent(wxCommandEvent& WXUNUSED(event))
{
	// Set the text in the DebugPane to an empty string
	DebugPane->ChangeValue(_T(""));
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		ToolsDoEEvent
//
// Description:		Event handler for the Tools menu's Design of Experiments item.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::ToolsDoEEvent(wxCommandEvent &event)
{
	event.Skip();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		ToolsDynamicsEvent
//
// Description:		Event handler for the Tools menu's Dynamics item.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::ToolsDynamicEvent(wxCommandEvent &event)
{
	event.Skip();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		ToolsOptionsEvent
//
// Description:		Displays the option dialog, allowing the user to specify
//					preferences.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::ToolsOptionsEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create the dialog box
	OPTIONS_DIALOG OptionsDialog(*this, converter, KinematicInputs, wxID_ANY, wxDefaultPosition, debugger);

	// Display the dialog
	if (OptionsDialog.ShowModal() == wxOK)
	{
		// FIXME:  Write the updated options to the registry

		// Update the edit panel
		EditPanel->UpdateInformation();

		// Update the analyses
		UpdateAnalysis();
		UpdateOutputPanel();

		// Make sure we have an object to update before we try to update it
		if (OpenObjectList.GetCount() > 0)
			OpenObjectList.GetObject(ActiveIndex)->UpdateData();
	}
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		HelpManualEvent
//
// Description:		Event handler for the Help menu's Manual item.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::HelpManualEvent(wxCommandEvent& WXUNUSED(event))
{
	// Call the shell to display the user's manual
	wxString OpenPDFManualCommand;
	wxString ManualFileName(_T("VVASE Manual.pdf"));
	wxMimeTypesManager MimeManager;

	// In Linux, we need to put the file name in quotes
#ifdef __WXGTK__
	ManualFileName.Prepend(_T("'"));
	ManualFileName.Append(_T("'"));
#endif

	wxFileType *PDFFileType = MimeManager.GetFileTypeFromExtension(_T("pdf"));
	if (!PDFFileType->GetOpenCommand(&OpenPDFManualCommand,
		wxFileType::MessageParameters(ManualFileName)))
		debugger.Print(_T("ERROR:  Could not determine how to open .pdf files!"));
	else
	{
		if (wxExecute(OpenPDFManualCommand) == 0)
			debugger.Print(_T("ERROR:  Could not find 'VVASE Manual.pdf'!"));// FIXME:  Use ManualFileName
	}
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		HelpAboutEvent
//
// Description:		Displays an about message box with some information
//					about the application.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::HelpAboutEvent(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo AppInfo;

	// Fill in the information
	AppInfo.SetName(CarDesignerLongName);
	AppInfo.SetVersion(CarDesignerVersion);
	AppInfo.SetDescription(_T("\n\
A work in progress...\n\
This is a vehicle design and analysis tool.  Please see the\n\
readme.txt file for licensing and other information."));
	AppInfo.SetCopyright(_T("(C) 2008-2010 Kerry Loux"));

	// Display the information
	wxAboutBox(AppInfo);
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		UpdateAnalysis
//
// Description:		Updates the information associated with each object.
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
void MAIN_FRAME::UpdateAnalysis(void)
{
	// For every object we've got open, call the update display method
	int i;
	for (i = 0; i < OpenObjectList.GetCount(); i++)
		// Update the display (this performs the kinematic analysis)
		OpenObjectList.GetObject(i)->UpdateData();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		UpdateOutputPanel
//
// Description:		Updates the output dispaly with the information currently
//					in each car object.
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
void MAIN_FRAME::UpdateOutputPanel(void)
{
	// For every object we've got open, call the update display method
	int i, CarCount = 0;
	for (i = 0; i < OpenObjectList.GetCount(); i++)
	{
		// Update the kinematics information (ONLY if this is a CAR)
		if (OpenObjectList.GetObject(i)->GetType() == GUI_OBJECT::TYPE_CAR)
		{
			// Increment the number of cars we have
			CarCount++;

			// Update the information for this car
			OutputPanel->UpdateInformation(static_cast<GUI_CAR*>(OpenObjectList.GetObject(i))->GetKinematicOutputs(),
				static_cast<GUI_CAR*>(OpenObjectList.GetObject(i))->GetWorkingCar(),
				CarCount, OpenObjectList.GetObject(i)->GetCleanName());
		}
	}

	// Make sure the output panel doesn't have more data columns than there are cars
	OutputPanel->FinishUpdate(CarCount);
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		AddJob
//
// Description:		Adds a job to the job queue to be handled by the thread pool.
//
// Input Arguments:
//		newJob	= ThreadJob& containg in the information about the new job to
//				  be performed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::AddJob(ThreadJob &newJob)
{
	// Make sure we have a thread to handle the job
	assert(ActiveThreads > 0);

	// Add the job to the queue
	jobQueue->AddJob(newJob, JobQueue::PriorityNormal);

	// Increment the open job counter
	OpenJobCount++;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		KinematicToolbarPitchChangeEvent
//
// Description:		Event that fires when the pitch text box changes value.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::KinematicToolbarPitchChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Get a pointer to the pitch text box
	wxTextCtrl *TextBox = static_cast<wxTextCtrl*>(KinematicToolbar->FindControl(IdToolbarKinematicPitch));

	// Make sure the text box was found (may not be found on startup)
	if (TextBox == NULL)
		return;

	// Get the value from the text box and convert it to a double
	double Value;
	if (!TextBox->GetValue().ToDouble(&Value))
		// The value was non-numeric - don't do anything
		return;

	// Set the value for the kinematic analysis object
	KinematicInputs.pitch = converter.ReadAngle(Value);

	// Update the analysis
	UpdateAnalysis();
	UpdateOutputPanel();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		KinematicToolbarRollChangeEvent
//
// Description:		Event that fires when the roll text box changes value.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::KinematicToolbarRollChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Get a pointer to the roll text box
	wxTextCtrl *TextBox = static_cast<wxTextCtrl*>(KinematicToolbar->FindControl(IdToolbarKinematicRoll));

	// Make sure the text box was found (may not be found on startup)
	if (TextBox == NULL)
		return;

	// Get the value from the text box and convert it to a double
	double Value;
	if (!TextBox->GetValue().ToDouble(&Value))
		// The value was non-numeric - don't do anything
		return;

	// Set the value for the kinematic analysis object
	KinematicInputs.roll = converter.ReadAngle(Value);

	// Update the analysis
	UpdateAnalysis();
	UpdateOutputPanel();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		KinematicToolbarHeaveChangeEvent
//
// Description:		Event that fires when the heave text box changes value.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::KinematicToolbarHeaveChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Get a pointer to the heave text box
	wxTextCtrl *TextBox = static_cast<wxTextCtrl*>(KinematicToolbar->FindControl(IdToolbarKinematicHeave));

	// Make sure the text box was found (may not be found on startup)d
	if (TextBox == NULL)
		return;

	// Get the value from the text box and convert it to a double
	double Value;
	if (!TextBox->GetValue().ToDouble(&Value))
		// The value was non-numeric - don't do anything
		return;

	// Set the value for the kinematic analysis object
	KinematicInputs.heave = converter.ReadDistance(Value);

	// Update the analysis
	UpdateAnalysis();
	UpdateOutputPanel();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		KinematicToolbarSteerChangeEvent
//
// Description:		Event that fires when the steer text box changes value.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::KinematicToolbarSteerChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Get a pointer to the steer text box
	wxTextCtrl *TextBox = static_cast<wxTextCtrl*>(KinematicToolbar->FindControl(IdToolbarKinematicSteer));

	// Make sure the text box was found (may not be found on startup)
	if (TextBox == NULL)
		return;

	// Get the value from the text box and convert it to a double
	double Value;
	if (!TextBox->GetValue().ToDouble(&Value))
		// The value was non-numeric - don't do anything
		return;

	// Set the value for the kinematic analysis object depending on what the steering input represents
	if (UseRackTravel)
		KinematicInputs.rackTravel = converter.ReadDistance(Value);
	else
		KinematicInputs.rackTravel = converter.ReadAngle(Value) * 1.0;// * RackRatio;// FIXME:  Use rack ratio

	// Update the analysis
	UpdateAnalysis();
	UpdateOutputPanel();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		ThreadCompleteEvent
//
// Description:		Handles events when threads complete their jobs.  Depending
//					on the type of event, we send the results to different places.
//
// Input Arguments:
//		event	= &wxCommandEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::ThreadCompleteEvent(wxCommandEvent &event)
{
	int CarCount(0), i;

	// Perform different operations depending on the type of job that has completed
	switch (event.GetInt())
	{
	case ThreadJob::CommandThreadExit:
		// Decrement the number of active threads
		ActiveThreads--;
		debugger.Print(Debugger::PriorityLow, "Thread %i exited", event.GetId());

		// If there are no more active threads, it is now safe to kill this window
		if (ActiveThreads == 0)
		{
			// Kill the window
			Destroy();
			return;
		}
		break;

	case ThreadJob::CommandThreadStarted:
		// Increment the number of active threads
		ActiveThreads++;
		debugger.Print(Debugger::PriorityLow, "Thread %i started", event.GetId());
		break;

	case ThreadJob::CommandThreadKinematicsNormal:
		// Get the car count for this car (number of objects before this in the list that are also cars)
		for (i = 0; i <= event.GetExtraLong(); i++)
		{
			// Iterate through the open objects up to the selected object, and if it is a car,
			// increment the car count
			if (OpenObjectList.GetObject(i)->GetType() == GUI_OBJECT::TYPE_CAR)
				CarCount++;
		}

		// Update the information for this car
		OutputPanel->UpdateInformation(static_cast<GUI_CAR*>(OpenObjectList.GetObject(
			event.GetExtraLong()))->GetKinematicOutputs(),
			static_cast<GUI_CAR*>(OpenObjectList.GetObject(event.GetExtraLong()))->GetWorkingCar(),
			CarCount, OpenObjectList[event.GetExtraLong()]->GetCleanName());

		// Call the 3D display update method
		OpenObjectList[event.GetExtraLong()]->UpdateDisplay();
		break;

	case ThreadJob::CommandThreadKinematicsIteration:
		// Decrement the semaphore for the current iteration
		static_cast<ITERATION*>(OpenObjectList[event.GetExtraLong()])->MarkAnalysisComplete();
		break;

	case ThreadJob::CommandThreadKinematicsGA:
		// Tell the GA manager thread that an analysis is done (decrement the count of pending analyses)
		static_cast<GENETIC_OPTIMIZATION*>(OpenObjectList[event.GetExtraLong()])->MarkAnalysisComplete();
		break;

	case ThreadJob::CommandThreadGeneticOptimization:
		// Update the optimized car and mark the optimization object as complete
		static_cast<GENETIC_OPTIMIZATION*>(OpenObjectList[event.GetExtraLong()])->CompleteOptimization();

		// Update the displays
		UpdateAnalysis();
		UpdateOutputPanel();
		break;

	case ThreadJob::CommandThreadNull:
	default:
		break;
	}

	// Decrement the job counter
	OpenJobCount--;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		DebugMessageEvent
//
// Description:		Prints debug messages when the debugger object posts messages.
//
// Input Arguments:
//		event	= &wxCommandEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::DebugMessageEvent(wxCommandEvent &event)
{
	// Print a message to the output pane
	DebugPane->AppendText(event.GetString());

#ifdef _DEBUG_TO_FILE_
	// Useful for cases where the application crashes and text can no longer be viewed in the output pane
	// FIXME:  Make this part of the debugger class?
	std::ofstream file("debug.txt", std::ios::app);
	file << event.GetString();
	file.close();
#endif
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		AddObjectToList
//
// Description:		Add a GUI_OBJECT object to the list of managed cars.  This
//					function should be called immediately after creation of
//					a new GUI_OBJECT.  The usual syntax and calling
//					sequence looks something like this (it is also important
//					to assign the index back to the GUI_OBJECT for future use):
//						GUI_OBJECT *NewObject = new GUI_OBJECT(this, debugger);
//						 NewObject->SetIndex(AddObjectToList(NewObject));
//
// Input Arguments:
//		ObjectToAdd	= *GUI_OBJECT
//
// Output Arguments:
//		None
//
// Return Value:
//		integer = new index for the car that was just added to the list
//
//==========================================================================
int MAIN_FRAME::AddObjectToList(GUI_OBJECT *ObjectToAdd)
{
	// Add the object to the list (must be done before the menu persmissions are updated)
	return OpenObjectList.Add(ObjectToAdd);
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		RemoveObjectFromList
//
// Description:		Removes a GUI_OBJECT object from the list of managed cars.
//					Should be done instead of deleting the GUI_OBJECT object.
//					This function will handle the deletion of the GUI_OBJECT
//					internally.
//
// Input Arguments:
//		Index	= integer specifying the object to be removed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::RemoveObjectFromList(int Index)
{
	// Set the deletion status flag
	BeingDeleted = true;

	// Delete the specified object from the list
	OpenObjectList.Remove(Index);

	// Reset the cars' indicies and check to see what types we have available
	// Also refresh the car's displays
	int i;
	for (i = 0; i < OpenObjectList.GetCount(); i++)
	{
		// Re-set the index
		OpenObjectList[i]->SetIndex(i);

		// Update the data and displays - data first, because in some cases data is
		// dependent on other open objects, and we may have just closed one
		OpenObjectList[i]->UpdateData();
		OpenObjectList[i]->UpdateDisplay();
	}

	// Reset the deletion status flag
	BeingDeleted = false;

	// Reset the active index - if there is still an open object, show the one with index zero
	// Otherwise, set the index to an invalid number
	if (OpenObjectList.GetCount() > 0)
		SetActiveIndex(0);
	else
		SetActiveIndex(-1);

	// Update the output display
	UpdateOutputPanel();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		SetNotebookPage
//
// Description:		Sets the notebook page to the specified index.  The
//					index here should start at 0, just like the
//					ActiveIndex.
//
// Input Arguments:
//		Index	= integer specifying the notebook page to activate
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::SetNotebookPage(int Index)
{
	// Make sure the index could possibly be a valid index
	if (Index >= 0 && Index < (signed int)OpenObjectList.GetCount())
	{
		// Bring the desired notebook page to the front
		Notebook->SetSelection(Index);

		// Update the active object's display
		// (haven't identified the root cause, but sometimes the car disappears if this isn't done)
		// FIXME:  This works as a workaround for disappearing cars/plots as happens sometimes, but
		// casues intermittent crashes (access violations) for cars and always crashes new iterations
		// in FormatPlot() (Z values are NaN)
		//OpenObjectList[ActiveIndex]->UpdateDisplay();
	}
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		WindowCloseEvent
//
// Description:		Calls CloseThisForm and depending on whether or not the
//					user confirms the close, it allows or prevents the form
//					closing.
//
// Input Arguments:
//		event	= &wxCloseEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::WindowCloseEvent(wxCloseEvent& WXUNUSED(event))
{
	// Write the application configuration information to the registry
	WriteConfiguration();

	// Kill this window if there aren't any more worker threads (assumes we've already
	// been through this once, and issued the EXIT command to all of the threads -
	// no need to ask for confirmation again!).  In practice, this should never happen,
	// as the window will be destroyed when the last thread exits.
	if (ActiveThreads == 0)
	{
		// Kill this window
		Destroy();

		return;
	}

	// Get the user confirmation
	if (!CloseThisForm())
		// Returning without skipping the event will prevent the window from closing
		return;

	// Delete all of the threads in the thread pool
	int i;
	for (i = 0; i < ActiveThreads; i++)
		jobQueue->AddJob(ThreadJob(ThreadJob::CommandThreadExit), JobQueue::PriorityVeryHigh);
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		OnSizeEvent
//
// Description:		Event handler for re-sizing events.  When this event triggers,
//					we need to update the displays for all of our GUI_OBJECTS.
//					This is particularly important for plots, which do not
//					automatically re-size with the main window.
//
// Input Arguments:
//		event	= &wxSizeEvent (UNUSED)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::OnSizeEvent(wxSizeEvent& WXUNUSED(event))
{
	// For each open object, update the display
	/*int i;
	for (i = 0; i < OpenObjectList.GetCount(); i++)
		OpenObjectList[i]->UpdateDisplay();*/
	// FIXME:  This is out-of-phase with the actualy re-sizing!
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		CloseThisForm
//
// Description:		Starts a process to close the application.  If no cars
//					are open, or all of the open cars are saved, the user
//					is asked for confirmation to close.  Otherwise, a save
//					confirmation appears for each unsaved car, with the
//					option to cancel and abort the close.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the user confirms the close, false otherwise
//
//==========================================================================
bool MAIN_FRAME::CloseThisForm(void)
{
	// Cycle through all of the cars and close them
	while (OpenObjectList.GetCount() > 0)
	{
		// Attempt to close each object
		if (!OpenObjectList.GetObject(OpenObjectList.GetCount() - 1)->Close(false))
			// If  the user canceled, we should cancel here, too
			return false;
	}

	// If we have not yet returned false, then return true
	return true;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		ReadConfiguration
//
// Description:		Reads the application configuration information from
//					file.
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
void MAIN_FRAME::ReadConfiguration(void)
{
	// Create a configuration file object
	wxFileConfig *ConfigurationFile = new wxFileConfig(_T(""), _T(""),
		wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() + PathToConfigFile, _T(""),
		wxCONFIG_USE_RELATIVE_PATH);

	// Read UNITS configuration from file
	converter.SetAccelerationUnits(Convert::UnitsOfAcceleration(
		ConfigurationFile->Read(_T("/Units/Acceleration"), 0l)));
	converter.SetAngleUnits(Convert::UnitsOfAngle(
		ConfigurationFile->Read(_T("/Units/Angle"), 1l)));
	converter.SetAreaUnits(Convert::UnitsOfArea(
		ConfigurationFile->Read(_T("/Units/Area"), 0l)));
	converter.SetDensityUnits(Convert::UnitsOfDensity(
		ConfigurationFile->Read(_T("/Units/Density"), 0l)));
	converter.SetDistanceUnits(Convert::UnitsOfDistance(
		ConfigurationFile->Read(_T("/Units/Distance"), 0l)));
	converter.SetEnergyUnits(Convert::UnitsOfEnergy(
		ConfigurationFile->Read(_T("/Units/Energy"), 0l)));
	converter.SetForceUnits(Convert::UnitsOfForce(
		ConfigurationFile->Read(_T("/Units/Force"), 0l)));
	converter.SetInertiaUnits(Convert::UnitsOfInertia(
		ConfigurationFile->Read(_T("/Units/Inertia"), 0l)));
	converter.SetMassUnits(Convert::UnitsOfMass(
		ConfigurationFile->Read(_T("/Units/Mass"), 0l)));
	converter.SetMomentUnits(Convert::UnitsOfMoment(
		ConfigurationFile->Read(_T("/Units/Moment"), 0l)));
	converter.SetPowerUnits(Convert::UnitsOfPower(
		ConfigurationFile->Read(_T("/Units/Power"), 0l)));
	converter.SetPressureUnits(Convert::UnitsOfPressure(
		ConfigurationFile->Read(_T("/Units/Pressure"), 0l)));
	converter.SetTemperatureUnits(Convert::UnitsOfTemperature(
		ConfigurationFile->Read(_T("/Units/Temperature"), 0l)));
	converter.SetVelocityUnits(Convert::UnitsOfVelocity(
		ConfigurationFile->Read(_T("/Units/Velocity"), 0l)));

	// Read NUMBER FORMAT configuration from file
	converter.SetNumberOfDigits(ConfigurationFile->Read(_T("/NumberFormat/NumberOfDigits"), 3l));
	bool TempBool = converter.GetUseScientificNotation();
	ConfigurationFile->Read(_T("/NumberFormat/UseScientificNotation"), &TempBool);
	converter.SetUseScientificNotation(TempBool);
	TempBool = converter.GetUseSignificantDigits();
	ConfigurationFile->Read(_T("/NumberFormat/UseSignificantDigits"), &TempBool);
	converter.SetUseSignificantDigits(TempBool);

	// Read KINEMATICS configuration from file
	double TempDouble = 0.0;
	ConfigurationFile->Read(_T("/Kinematics/CenterOfRotationX"), &TempDouble);
	KinematicInputs.centerOfRotation.x = TempDouble;
	TempDouble = 0.0;
	ConfigurationFile->Read(_T("/Kinematics/CenterOfRotationY"), &TempDouble);
	KinematicInputs.centerOfRotation.y = TempDouble;
	TempDouble = 0.0;
	ConfigurationFile->Read(_T("/Kinematics/CenterOfRotationZ"), &TempDouble);
	KinematicInputs.centerOfRotation.z = TempDouble;
	KinematicInputs.firstRotation = (Vector::Axis)ConfigurationFile->Read(
		_T("/Kinematics/FirstRotation"), 0l);
	ConfigurationFile->Read(_T("/Kinematics/UseRackTravel"), &UseRackTravel, true);

	// Read DEBUGGING configuration from file
	debugger.SetDebugLevel(Debugger::DebugLevel(ConfigurationFile->Read(_T("/Debugging/DebugLevel"), 1l)));

	// Read GUI configuration from file
	wxString LayoutString;
	if (ConfigurationFile->Read(_T("/GUI/LayoutString"), &LayoutString))
		Manager.LoadPerspective(LayoutString);
	TempBool = false;
	ConfigurationFile->Read(_T("/GUI/IsMaximized"), &TempBool);
	if (TempBool)
		Maximize(TempBool);
	else
	{
		SetSize(ConfigurationFile->Read(_T("/GUI/SizeX"), 1024l),
			ConfigurationFile->Read(_T("/GUI/SizeY"), 768l));
		int XPosition = 0, YPosition = 0;
		if (ConfigurationFile->Read(_T("/GUI/PositionX"), &XPosition)
			&& ConfigurationFile->Read(_T("/GUI/PositionY"), &YPosition))
			SetPosition(wxPoint(XPosition, YPosition));
		else
			Center();
	}

	// Read SOLVER configuration from file
	SetNumberOfThreads(ConfigurationFile->Read(_T("/SOLVER/NumberOfThreads"), wxThread::GetCPUCount() * 2));

	// Read recent file history
	RecentFileManager->Load(*ConfigurationFile);

	// Delete file object
	delete ConfigurationFile;
	ConfigurationFile = NULL;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		WriteConfiguration
//
// Description:		Writes the application configuration information to file.
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
void MAIN_FRAME::WriteConfiguration(void)
{
	// Create a configuration file object
	wxFileConfig *ConfigurationFile = new wxFileConfig(_T(""), _T(""),
		wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() + PathToConfigFile, _T(""),
		wxCONFIG_USE_RELATIVE_PATH);

	// Write UNITS configuration to file
	ConfigurationFile->Write(_T("/Units/Acceleration"), converter.GetAccelerationUnits());
	ConfigurationFile->Write(_T("/Units/Angle"), converter.GetAngleUnits());
	ConfigurationFile->Write(_T("/Units/Area"), converter.GetAreaUnits());
	ConfigurationFile->Write(_T("/Units/Density"), converter.GetDensityUnits());
	ConfigurationFile->Write(_T("/Units/Distance"), converter.GetDistanceUnits());
	ConfigurationFile->Write(_T("/Units/Energy"), converter.GetEnergyUnits());
	ConfigurationFile->Write(_T("/Units/Force"), converter.GetForceUnits());
	ConfigurationFile->Write(_T("/Units/Inertia"), converter.GetInertiaUnits());
	ConfigurationFile->Write(_T("/Units/Mass"), converter.GetMassUnits());
	ConfigurationFile->Write(_T("/Units/Moment"), converter.GetMomentUnits());
	ConfigurationFile->Write(_T("/Units/Power"), converter.GetPowerUnits());
	ConfigurationFile->Write(_T("/Units/Pressure"), converter.GetPressureUnits());
	ConfigurationFile->Write(_T("/Units/Temperature"), converter.GetTemperatureUnits());
	ConfigurationFile->Write(_T("/Units/Velocity"), converter.GetVelocityUnits());

	// Write NUMBER FORMAT configuration to file
	ConfigurationFile->Write(_T("/NumberFormat/NumberOfDigits"), converter.GetNumberOfDigits());
	ConfigurationFile->Write(_T("/NumberFormat/UseScientificNotation"), converter.GetUseScientificNotation());
	ConfigurationFile->Write(_T("/NumberFormat/UseSignificantDigits"), converter.GetUseSignificantDigits());

	// Write KINEMATICS configuration to file
	ConfigurationFile->Write(_T("/Kinematics/CenterOfRotationX"), KinematicInputs.centerOfRotation.x);
	ConfigurationFile->Write(_T("/Kinematics/CenterOfRotationY"), KinematicInputs.centerOfRotation.y);
	ConfigurationFile->Write(_T("/Kinematics/CenterOfRotationZ"), KinematicInputs.centerOfRotation.z);
	ConfigurationFile->Write(_T("/Kinematics/FirstRotation"), KinematicInputs.firstRotation);
	ConfigurationFile->Write(_T("/Kinematics/UseRackTravel"), UseRackTravel);

	// Write DEBUGGING configuration to file
	ConfigurationFile->Write(_T("/Debugging/DebugLevel"), debugger.GetDebugLevel());

	// Write GUI configuration to file
	ConfigurationFile->Write(_T("/GUI/LayoutString"), Manager.SavePerspective());
	ConfigurationFile->Write(_T("/GUI/IsMaximized"), IsMaximized());
	ConfigurationFile->Write(_T("/GUI/SizeX"), GetSize().GetX());
	ConfigurationFile->Write(_T("/GUI/SizeY"), GetSize().GetY());
	ConfigurationFile->Write(_T("/GUI/PositionX"), GetPosition().x);
	ConfigurationFile->Write(_T("/GUI/PositionY"), GetPosition().y);

	// Write SOLVER configuration to file
	ConfigurationFile->Write(_T("/SOLVER/NumberOfThreads"), NumberOfThreads);

	// Write recent file history
	RecentFileManager->Save(*ConfigurationFile);

	// Delete file object
	delete ConfigurationFile;
	ConfigurationFile = NULL;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		UpdateActiveObjectMenu
//
// Description:		Updates the active object-specific menu to the new
//					active object's type.
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
void MAIN_FRAME::UpdateActiveObjectMenu(void)
{
	// Try to get a handle to object specific menus
	int CarMenuIndex = MenuBar->FindMenu(_T("Car"));
	int IterationMenuIndex = MenuBar->FindMenu(_T("Iteration"));
	int GAMenuIndex = MenuBar->FindMenu(_T("GA"));

	// Store the type of the active object
	GUI_OBJECT::ITEM_TYPE ActiveType;

	// Make sure the active index is valid
	if (ActiveIndex < 0)
		ActiveType = GUI_OBJECT::TYPE_NONE;
	else
		ActiveType = OpenObjectList[ActiveIndex]->GetType();

	switch (ActiveType)
	{
	case GUI_OBJECT::TYPE_CAR:
		// Check to see if the car menu already exists
		if (CarMenuIndex == wxNOT_FOUND)
		{
			// Check to see if we first need to remove the iteration menu
			if (IterationMenuIndex == wxNOT_FOUND)
				// Just insert a new menu
				MenuBar->Insert(3, CreateCarMenu(), _T("&Car"));
			else
				// Replace the iteration menu with a new car menu
				delete MenuBar->Replace(IterationMenuIndex, CreateCarMenu(), _T("&Car"));
		}
		break;

	case GUI_OBJECT::TYPE_ITERATION:
		// Check to see if the iteration menu already exists
		if (IterationMenuIndex == wxNOT_FOUND)
		{
			// Check to see if we first need to remove the car menu
			if (CarMenuIndex == wxNOT_FOUND)
				// Just insert a new menu
				MenuBar->Insert(3, CreateIterationMenu(), _T("&Iteration"));
			else
				// Replace the car menu with a new iteration menu
				delete MenuBar->Replace(CarMenuIndex, CreateIterationMenu(), _T("&Iteration"));
		}
		else
			// For iteration objects, we need to update the menu for every object
			// Just having the menu there already isn't good enough (checks, etc.)
			// So we replace the existing Iteration menu with a new one
			delete MenuBar->Replace(IterationMenuIndex, CreateIterationMenu(), _T("&Iteration"));
		break;

	// Unused cases
	case GUI_OBJECT::TYPE_NONE:
	case GUI_OBJECT::TYPE_OPTIMIZATION:
		break;

	// Fail on unknown types to prevent forgetting any
	default:
		// Remove all menus
		if (CarMenuIndex != wxNOT_FOUND)
			MenuBar->Remove(CarMenuIndex);
		else if (IterationMenuIndex != wxNOT_FOUND)
			MenuBar->Remove(IterationMenuIndex);
		else if (GAMenuIndex != wxNOT_FOUND)
			MenuBar->Remove(GAMenuIndex);

		break;
	}
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		SetActiveIndex
//
// Description:		Sets the active index to the specified value and brings
//					the associated notebook page to the front.
//
// Input Arguments:
//		Index				= integer specifying the current active object
//		SelectNotebookTab	= bool indicated whether or not to change the current
//							  notebook page (optional)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::SetActiveIndex(int Index, bool SelectNotebookTab)
{
	// Make sure the index is valid
	if (Index >= (signed int)OpenObjectList.GetCount())
		return;

	// Set the active index
	ActiveIndex = Index;

	// Also update the object of interest index
	ObjectOfInterestIndex = ActiveIndex;

	// Bring the appropriate notebook page to the top and render the scene (if desired)
	if (SelectNotebookTab)
		SetNotebookPage(ActiveIndex);

	if (ActiveIndex >= 0)
	{
		// Highlight the column in the output panel that corresponds to this car
		OutputPanel->HighlightColumn(OpenObjectList.GetObject(ActiveIndex)->GetCleanName());

		// Update the EditPanel
		EditPanel->UpdateInformation(OpenObjectList.GetObject(ActiveIndex));
	}
	else
		EditPanel->UpdateInformation(NULL);

	// If the active object is not selected in the SystemsTree, select it now
	// Get the selected item's ID
	wxTreeItemId SelectedId;
	SystemsTree->GetSelectedItem(&SelectedId);

	// Make sure there was an item selected (ID is valid?)
	if (SelectedId.IsOk() && ActiveIndex >= 0)
	{
		// Check to see if the ID belongs to our active object
		if (!OpenObjectList.GetObject(ActiveIndex)->IsThisObjectSelected(SelectedId))
			// If it does not, then select the root item for the newly active object
			OpenObjectList.GetObject(ActiveIndex)->SelectThisObjectInTree();
	}

	// Update the object specific menus
	UpdateActiveObjectMenu();
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		CreateContextMenu
//
// Description:		Displays a context menu that is customized for the object
//					specified by ObjectIndex.
//
// Input Arguments:
//		ObjectIndex		= integer specifying the object in the OpenObjectList
//						  that this menu is being created for
//		Position		= wxPoint specifying the position to display the menu
//		AllowClosing	= bool, specifying whether or not we should add the
//						  "Close" item to the menu
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::CreateContextMenu(int ObjectIndex, wxPoint Position, bool AllowClosing)
{
	// Make sure the index is valid before continuing
	if (ObjectIndex < 0 || ObjectIndex >= (signed int)OpenObjectList.GetCount())
		return;

	// Set the object of interest to the specified object
	ObjectOfInterestIndex = ObjectIndex;

	// Declare the menu variable and get the position of the cursor
	wxMenu *ContextMenu(NULL);

	// Depending on the type of the item, we might want additional options (or none at all)
	switch (OpenObjectList.GetObject(ObjectIndex)->GetType())
	{
	case GUI_OBJECT::TYPE_CAR:
		ContextMenu = CreateCarMenu();
		break;

	case GUI_OBJECT::TYPE_ITERATION:
		ContextMenu = CreateIterationMenu();
		break;

	// Unused types
	case GUI_OBJECT::TYPE_OPTIMIZATION:
	case GUI_OBJECT::TYPE_NONE:
		break;

	// Fail on unknown types to avoid forgetting any
	default:
		assert(0);
		return;
	}

	// Start building the context menu

	// Genetic algorithms do not have image files
	if (OpenObjectList.GetObject(ObjectIndex)->GetType() != GUI_OBJECT::TYPE_OPTIMIZATION)
	{
		ContextMenu->PrependSeparator();
		ContextMenu->Prepend(IdMenuFileWriteImageFile, _T("&Write Image File"));
	}

	// Only add the close item if it was specified that we should
	if (AllowClosing)
		ContextMenu->Prepend(IdMenuFileClose, _T("&Close"));

	ContextMenu->Prepend(IdMenuFileSave, _T("&Save"));

	// Show the menu
	PopupMenu(ContextMenu, Position);

	// Delete the context menu object
	delete ContextMenu;
	ContextMenu = NULL;

	// The event handlers for whatever was clicked get called here
	// Reset the object index to the active object index
	ObjectOfInterestIndex = ActiveIndex;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		CreateCarMenu
//
// Description:		Creates a drop-down menu for TYPE_CAR objects.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxMenu* pointing to the newly created menu
//
//==========================================================================
wxMenu *MAIN_FRAME::CreateCarMenu(void)
{
	// Car menu
	wxMenu *mnuCar = new wxMenu();
	mnuCar->Append(IdMenuCarAppearanceOptions, _T("Appearance Options"));

	return mnuCar;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		CreateIterationMenu
//
// Description:		Creates a drop-down menu for TYPE_ITERATION objects.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxMenu* pointing to the newly created menu
//
//==========================================================================
wxMenu *MAIN_FRAME::CreateIterationMenu(void)
{
	// Iteration menu
	wxMenu *mnuIteration = new wxMenu();

	// Make sure the active object is a TYPE_ITERATION object
	if (OpenObjectList.GetObject(ActiveIndex)->GetType() != GUI_OBJECT::TYPE_ITERATION)
		return mnuIteration;

	// Allocate the sub-menus
	wxMenu *AssociatedCarsMenu = new wxMenu();
	wxMenu *XAxisMenu = new wxMenu();

	mnuIteration->Append(IdMenuIterationExportDataToFile, _T("Export Data"));

	// Create and append the associated cars sub-menu
	AssociatedCarsMenu->Append(IdMenuIterationShowAssociatedCars, _T("Choose Associated Cars"));
	AssociatedCarsMenu->AppendSeparator();
	AssociatedCarsMenu->AppendCheckItem(IdMenuIterationAssociatedWithAllCars, _T("Associate With All Cars"));
	mnuIteration->AppendSubMenu(AssociatedCarsMenu, _T("Associated Cars"));

	// Create and append the x-axis sub-menu
	XAxisMenu->AppendCheckItem(IdMenuIterationXAxisPitch, _T("Pitch"));
	XAxisMenu->AppendCheckItem(IdMenuIterationXAxisRoll, _T("Roll"));
	XAxisMenu->AppendCheckItem(IdMenuIterationXAxisHeave, _T("Heave"));
	XAxisMenu->AppendCheckItem(IdMenuIterationXAxisRackTravel, _T("Rack Travel"));
	mnuIteration->AppendSubMenu(XAxisMenu, _T("Set X-Axis"));

	// Determine which items need to be checked
	if (static_cast<ITERATION*>(OpenObjectList.GetObject(ActiveIndex))->GetAutoAssociate())
		AssociatedCarsMenu->Check(IdMenuIterationAssociatedWithAllCars, true);

	switch (static_cast<ITERATION*>(OpenObjectList.GetObject(ActiveIndex))->GetXAxisType())
	{
	case ITERATION::AxisTypePitch:
		XAxisMenu->Check(IdMenuIterationXAxisPitch, true);
		break;

	case ITERATION::AxisTypeRoll:
		XAxisMenu->Check(IdMenuIterationXAxisRoll, true);
		break;

	case ITERATION::AxisTypeHeave:
		XAxisMenu->Check(IdMenuIterationXAxisHeave, true);
		break;

	case ITERATION::AxisTypeRackTravel:
		XAxisMenu->Check(IdMenuIterationXAxisRackTravel, true);
		break;

	case ITERATION::AxisTypeUnused:
		// Take no action
		break;

	default:
		assert(0);
		break;
	}

	return mnuIteration;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		JobsPending
//
// Description:		Returns true if there are any outstanding jobs.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for outstanding jobs, false for all caught up
//
//==========================================================================
bool MAIN_FRAME::JobsPending(void) const
{
	// If there are any jobs in the queue, return false
	if (jobQueue->PendingJobs() > 0)
		return true;

	// If there are any open jobs, return false
	if (OpenJobCount > 0)
		return true;

	// Otherwise, return true
	return false;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		GetFileNameFromUser
//
// Description:		Displays a dialog asking the user to specify a file name.
//					Arguments allow this to be for opening or saving files,
//					with different options for the wildcards.
//
// Input Arguments:
//		DialogTitle			= wxString containing the title for the dialog
//		DefaultDirectory	= wxString specifying the initial directory
//		DefaultFileName		= wxString specifying the default file name
//		Wildcard			= wxString specifying the list of file types to
//							  allow the user to select
//		Style				= long integer specifying the type of dialog
//							  (this is usually wxFD_OPEN or wxFD_SAVE)
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString containing the paths and file names of the specified files,
//		or and empty array if the user cancels
//
//==========================================================================
wxArrayString MAIN_FRAME::GetFileNameFromUser(wxString DialogTitle, wxString DefaultDirectory,
										 wxString DefaultFileName, wxString Wildcard, long Style)
{
	// Initialize the return variable
	wxArrayString PathsAndFileNames;

	// Step 1 is to ask the user to specify the file name
	wxFileDialog Dialog(this, DialogTitle, DefaultDirectory, DefaultFileName,
		Wildcard, Style);

	// Set the dialog to display center screen at the user's home directory
	Dialog.CenterOnParent();

	// Display the dialog and make sure the user clicked OK
	if (Dialog.ShowModal() == wxID_OK)
	{
		// If this was an open dialog, we want to get all of the selected paths,
		// otherwise, just get the one selected path
		if (Style & wxFD_OPEN)
			Dialog.GetPaths(PathsAndFileNames);
		else
			PathsAndFileNames.Add(Dialog.GetPath());
	}

	// Return the path and file name
	return PathsAndFileNames;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		LoadFile
//
// Description:		Public method for loading a single object from file.
//
// Input Arguments:
//		PathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool MAIN_FRAME::LoadFile(wxString PathAndFileName)
{
	int StartOfExtension;
	wxString FileExtension;
	GUI_OBJECT *TempObject = NULL;

	// Decipher the file name to figure out what kind of object this is
	StartOfExtension = PathAndFileName.Last('.') + 1;
	FileExtension = PathAndFileName.Mid(StartOfExtension);

	// Create the appropriate object
	if (FileExtension.CmpNoCase("car") == 0)
		TempObject = new GUI_CAR(*this, debugger, PathAndFileName);
	else if (FileExtension.CmpNoCase("iteration") == 0)
		TempObject = new ITERATION(*this, debugger, PathAndFileName);
	else if (FileExtension.CmpNoCase("ga") == 0)
		TempObject = new GENETIC_OPTIMIZATION(*this, debugger, converter, PathAndFileName);
	else
	{
		// Print error message to screen
		debugger.Print(_T("ERROR:  Unrecognized file extension: '") + FileExtension + _T("'"));
		return false;
	}

	// If the object is not initialized, remove it from the list
	// (user cancelled or errors occured)
	if (!TempObject->IsInitialized())
	{
		RemoveObjectFromList(TempObject->GetIndex());
		return false;
	}
	else
		// Make the last object loaded the active
		SetActiveIndex(TempObject->GetIndex());

	return true;
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		AddFileToHistory
//
// Description:		Adds the specified file to the recent history list.
//
// Input Arguments:
//		PathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::AddFileToHistory(wxString PathAndFileName)
{
	// Add the file to the manager
	RecentFileManager->AddFileToHistory(PathAndFileName);
}

//==========================================================================
// Class:			MAIN_FRAME
// Function:		RemoveFileFromHistory
//
// Description:		Removes the specified file from the recent file list.
//					Looks up the object index based on the file name.
//
// Input Arguments:
//		PathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_FRAME::RemoveFileFromHistory(wxString PathAndFileName)
{
	// Identify the entry that matches the specified path and file nam
	unsigned int i;
	for (i = 0; i < RecentFileManager->GetCount(); i++)
	{
		// If we find the specified file, remove it from the list
		if (RecentFileManager->GetHistoryFile(i).CompareTo(PathAndFileName) == 0)
		{
			RecentFileManager->RemoveFileFromHistory(i);
			break;
		}
	}
}
