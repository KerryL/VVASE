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
//				 MainFrame class.  Uses wxWidgets for the GUI components.
// History:
//	1/24/2009	- Major application structure change - MainFrame uses GuiObject instead of
//				  GuiCar.  GuiObject changed to only contain either GuiCar or Iteration
//				  objects, K. Loux.
//	1/28/2009	- Changed structure of GUI components so context menu creation for all
//				  objects is handled by this class, K. Loux.
//	2/10/2009	- Added EditPanel to application, K. Loux.
//	2/17/2009	- Moved the Kinematics object for primary analysis into the GUI_CAR class.
//	6/7/2009	- Changed GetFilenameFromUser() to make use of wx functions for checking for file
//				  existence and selecting multiple files to open, K. Loux.
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
#include "vUtilities/debugger.h"

// *nix Icons
#ifdef __WXGTK__
#include "../res/icons/aavase16.xpm"
#include "../res/icons/aavase32.xpm"
#include "../res/icons/aavase48.xpm"
#endif

//==========================================================================
// Class:			MainFrame
// Function:		MainFrame
//
// Description:		Constructor for MainFrame class.  Initializes the form
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
MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition,
								   wxDefaultSize, wxDEFAULT_FRAME_STYLE),
								   /*MaxRecentFiles(9), */undoRedo(*this)
{
	// Create the SystemsTree
	systemsTree = new MainTree(*this, wxID_ANY, wxDefaultPosition, wxSize(320, 384),
		wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_DEFAULT_STYLE | wxSUNKEN_BORDER
		| wxTR_HIDE_ROOT);

	// Create the Notebook
	notebook = new MainNotebook(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE |
		wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_WINDOWLIST_BUTTON);

	// Create the EditPanel
	editPanel = new EditPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	// Create the OutputPanel
	outputPanel = new OutputPanel(*this, wxID_ANY, wxDefaultPosition, wxSize(350, -1));

	// Create the OutputPane
	debugPane = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(-1, 350), wxTE_PROCESS_TAB | wxTE_MULTILINE | wxHSCROLL | wxTE_READONLY
		| wxTE_RICH);

	// Create additional objects
	CreateMenuBar();

	// Add the toolbars
	kinematicToolbar = NULL;
	CreateKinematicAnalysisToolbar();

	// These need to be in this order - otherwise the centering doesn't work (resize first)
	DoLayout();
	InitializeSolver();
	SetProperties();// Includes reading configuration file

	// Initialize the object management variables
	activeIndex = -1;
	beingDeleted = false;

	Debugger::GetInstance().Print(carDesignerName + _T(" Initialized!"));

	// Some machine information
	/*Debugger::GetInstance().Print(wxPlatformInfo::Get().GetArchName());
	Debugger::GetInstance().Print(wxPlatformInfo::Get().GetEndiannessName());

	Debugger::GetInstance().Print(wxGetOsDescription());
	wxString temp;
	temp.Printf("%i cores", wxThread::GetCPUCount());
	Debugger::GetInstance().Print(temp);//*/

	// Some debugging information
	/*Debugger::GetInstance().Print(Debugger::PriorityVeryHigh, "sizeof(double): %i", sizeof(double));
	Debugger::GetInstance().Print(Debugger::PriorityVeryHigh, "sizeof(short): %i", sizeof(short));
	Debugger::GetInstance().Print(Debugger::PriorityVeryHigh, "sizeof(int): %i", sizeof(int));
	Debugger::GetInstance().Print(Debugger::PriorityVeryHigh, "sizeof(long): %i", sizeof(long));
	Debugger::GetInstance().Print(Debugger::PriorityVeryHigh, "sizeof(bool): %i", sizeof(bool));
	Debugger::GetInstance().Print(Debugger::PriorityVeryHigh, "sizeof(Drivetrain::DriveWheels): %i",
		sizeof(Drivetrain::DriveWheels));
	Debugger::GetInstance().Print(Debugger::PriorityVeryHigh, "sizeof(Vector): %i", sizeof(Vector));
	Debugger::GetInstance().Print(Debugger::PriorityVeryHigh, "sizeof(VectorSet): %i", sizeof(VectorSet));
	Debugger::GetInstance().Print(Debugger::PriorityVeryHigh, "sizeof(WheelSet): %i", sizeof(WheelSet));//*/
}

//==========================================================================
// Class:			MainFrame
// Function:		~MainFrame
//
// Description:		Destructor for MainFrame class.  Frees memory and
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
MainFrame::~MainFrame()
{
	// Delete the file history manager
	delete recentFileManager;
	recentFileManager = NULL;

	// Delete the job queue object
	delete jobQueue;
	jobQueue = NULL;

	// Remove all of the cars we're managing that haven't been closed
	while (openObjectList.GetCount() > 0)
		RemoveObjectFromList(0);

	// De-initialize the manager
	manager.UnInit();
}

//==========================================================================
// Class:			MainFrame
// Function:		Constant Declarations
//
// Description:		Constant declarations for the MainFrame class.
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
const wxString MainFrame::pathToConfigFile = _T("vvase.rc");
#else
const wxString MainFrame::pathToConfigFile = _T("config.ini");
#endif

//==========================================================================
// Class:			MainFrame
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
void MainFrame::DoLayout(void)
{
	// Tell the frame manager to manage this window
	manager.SetManagedWindow(this);

	// Add the panes to the manager - this order is important (see below)
	// It's necessary to use the wxAuiPaneInfo() method because we need to assign internal names
	// to make calls to SavePerspective() and LoadPerspective() work properly.
	manager.AddPane(notebook, wxAuiPaneInfo().Name(_T("Main Notebook")).CenterPane());
	manager.AddPane(debugPane, wxAuiPaneInfo().Name(_T("Debug Pane")).Bottom().Caption(_T("Output")));
	// For some reason, these get reversed under Linux
#ifdef __WXGTK__
	manager.AddPane(editPanel, wxAuiPaneInfo().Name(_T("Edit Panel")).Left().Caption(_T("Edit Sub-Systems")));
	manager.AddPane(systemsTree, wxAuiPaneInfo().Name(_T("Systems Tree")).Left().Caption(_T("Systems Tree")));
#else
	manager.AddPane(systemsTree, wxAuiPaneInfo().Name(_T("Systems Tree")).Left().Caption(_T("Systems Tree")));
	manager.AddPane(editPanel, wxAuiPaneInfo().Name(_T("Edit Panel")).Left().Caption(_T("Edit Sub-Systems")));
#endif
	manager.AddPane(outputPanel, wxAuiPaneInfo().Name(_T("Output List")).Right().Caption(_T("Output List")));

	// This layer stuff is required to get the desired initial layout (see below)
	manager.GetPane(debugPane).Layer(0);
	manager.GetPane(notebook).Layer(0);
	manager.GetPane(systemsTree).Layer(1);
	manager.GetPane(editPanel).Layer(1);
	manager.GetPane(outputPanel).Layer(0);

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
	manager.Update();

	// Configure the output panel to show units
	outputPanel->FinishUpdate(0);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::SetProperties(void)
{
	// MainFrame properties
	SetTitle(carDesignerName);
	SetName(carDesignerName);
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

	// Set up the debugger
	Debugger::GetInstance().SetTargetOutput(debugPane);
	Debugger::GetInstance().SetDebugLevel(Debugger::PriorityHigh);

	// Add the application level entry to the SystemsTree (this one is hidden, but necessary)
	systemsTree->AddRoot(_T("Application Level"), -1, -1);

	// Do the disabling/checking of certain controls
	menuBar->Check(IdMenuViewToolbarsKinematic, true);

	// Disable the undo and redo menu items
	DisableUndo();
	DisableRedo();

	// This section disables all menu items that are not yet implemented
	menuBar->FindItem(IdMenuEditCut)->Enable(false);
	menuBar->FindItem(IdMenuEditCopy)->Enable(false);
	menuBar->FindItem(IdMenuEditPaste)->Enable(false);
	menuBar->FindItem(IdMenuToolsDoE)->Enable(false);
	menuBar->FindItem(IdMenuToolsDynamic)->Enable(false);

	// Set up the unit converter
	Convert::GetInstance().SetDistanceUnits(Convert::INCH);
	Convert::GetInstance().SetAreaUnits(Convert::INCH_SQUARED);
	Convert::GetInstance().SetForceUnits(Convert::POUND_FORCE);
	Convert::GetInstance().SetPressureUnits(Convert::POUND_FORCE_PER_SQUARE_INCH);
	Convert::GetInstance().SetMomentUnits(Convert::FOOT_POUND_FORCE);
	Convert::GetInstance().SetMassUnits(Convert::SLUG);
	Convert::GetInstance().SetVelocityUnits(Convert::INCHES_PER_SECOND);
	Convert::GetInstance().SetAccelerationUnits(Convert::INCHES_PER_SECOND_SQUARED);
	Convert::GetInstance().SetInertiaUnits(Convert::SLUG_INCHES_SQUARED);
	Convert::GetInstance().SetDensityUnits(Convert::SLUGS_PER_INCH_CUBED);
	Convert::GetInstance().SetPowerUnits(Convert::HORSEPOWER);
	Convert::GetInstance().SetEnergyUnits(Convert::BRITISH_THERMAL_UNIT);
	Convert::GetInstance().SetTemperatureUnits(Convert::FAHRENHEIT);
	
	// Read defaults from config file, if it exists
	ReadConfiguration();
	
	// OutputPane properties
	wxString fontFaceName;
	
	// Check to see if we read the output font preference from the config file
	if (outputFont.IsNull() || !outputFont.IsOk())
	{
		wxArrayString preferredFonts;
		preferredFonts.Add(_T("Monospace"));// GTK preference
		preferredFonts.Add(_T("Courier New"));// MSW preference
		bool foundPreferredFont = FontFinder::GetFontFaceName(
			wxFONTENCODING_SYSTEM, preferredFonts, true, fontFaceName);

		// As long as we have a font name to use, set the font
		if (!fontFaceName.IsEmpty())
		{
			// Populate the wxFont object
			outputFont.SetPointSize(9);
			outputFont.SetFamily(wxFONTFAMILY_MODERN);
			if (!outputFont.SetFaceName(fontFaceName))
				Debugger::GetInstance().Print(_T("Error setting font face to ") + fontFaceName);
		}
		
		// Tell the user if we're using a font we're unsure of
		if (!foundPreferredFont)
		{
			Debugger::GetInstance().Print(_T("Could not find preferred fixed-width font; using ")
				+ fontFaceName);
			Debugger::GetInstance().Print(_T("This can be changed in Tools->Options->Fonts"));
		}
	}

	// Set the font
	SetOutputFont(outputFont);

	// Check the plot font
	if (plotFont.IsNull() || !plotFont.IsOk())
	{
		wxArrayString preferredFonts;

		preferredFonts.Add(_T("DejaVu Sans"));// GTK preference
		preferredFonts.Add(_T("Arial"));// MSW preference

		wxString fontFile;
		bool foundFont = FontFinder::GetPreferredFontFileName(wxFONTENCODING_SYSTEM,
			preferredFonts, false, fontFile);

		// Tell the user if we're unsure of the font
		if (!foundFont)
		{
			if (!fontFile.IsEmpty())
				Debugger::GetInstance().Print(_T("Could not find preferred plot font; using ") + fontFile);
			else
				Debugger::GetInstance().Print(_T("Could not find any *.ttf files - cannot generate plot fonts"));
		}
		else
		{
			// Store what we found in the MainFrame configuration
			wxString fontName;
			if (FontFinder::GetFontName(fontFile, fontName))
			{
				if (plotFont.SetFaceName(fontName))
					SetPlotFont(plotFont);
			}
		}
	}

	// Also put the cursor at the bottom of the text, so the window scrolls automatically
	// as it updates with text
	// NOTE:  This already works as desired under GTK, issue is with MSW
	// FIXME:  This doesn't work!

	// Allow dragging-and-dropping of files onto this window to open them
	SetDropTarget(dynamic_cast<wxDropTarget*>(new DropTarget(*this)));
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::InitializeSolver(void)
{
	// Initialize the job queue
	jobQueue = new JobQueue(GetEventHandler());

	// Initialize thread-related variables
	activeThreads = 0;
	openJobCount = 0;
	numberOfThreads = 0;

	// Zero out kinematic inputs
	kinematicInputs.pitch = 0.0;
	kinematicInputs.roll = 0.0;
	kinematicInputs.heave = 0.0;
	kinematicInputs.rackTravel = 0.0;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetNumberOfThreads
//
// Description:		Sets the number of worker threads to the specified value.
//					Handles creation or deletion of threads as necessary to
//					ensure the correct number of threads are left.
//
// Input Arguments:
//		newNumberOfThreads	= unsigned int specifying the number of
//							  threads desired
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetNumberOfThreads(unsigned int newNumberOfThreads)
{
	// Make sure we've got at least one thread
	if (newNumberOfThreads < 1)
		newNumberOfThreads = 1;

	// Determine if we need to increase or decrease the number of threads we have
	unsigned int i;
	if (newNumberOfThreads > numberOfThreads)
	{
		// Spawn threads for the thread pool
		for (i = numberOfThreads; i < newNumberOfThreads; i++)
		{
			// Keep track of jobs by counting them as they're sent to the
			// threads (starting a thread counts as a job)
			openJobCount++;

			// These threads will delete themselves after an EXIT job
			WorkerThread *newThread = new WorkerThread(jobQueue, i);
			// FIXME:  If we want to set priority, this is where it needs to happen
			newThread->Run();
		}
	}
	else if (newNumberOfThreads < numberOfThreads)
	{
		// Kill the necessary number of threads
		for (i = numberOfThreads; i > newNumberOfThreads; i--)
			jobQueue->AddJob(ThreadJob(ThreadJob::CommandThreadExit), JobQueue::PriorityVeryHigh);
	}

	// Update the class member for the desired number of threads
	numberOfThreads = newNumberOfThreads;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetNumberOfThreads
//
// Description:		Sets the font to use for text output and assigns it to
//					the panel.
//
// Input Arguments:
//		font	= const wxFont&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetOutputFont(const wxFont& font)
{
	if (!font.IsNull() && font.IsOk())
	{
		outputFont = font;
		
		// Assign the font to the window
		wxTextAttr outputAttributes;
		outputAttributes.SetFont(outputFont);
		if (!debugPane->SetDefaultStyle(outputAttributes))
			Debugger::GetInstance().Print(_T("Error setting font style"));
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		SetPlotFont
//
// Description:		Sets the font to use for plots.
//
// Input Arguments:
//		font	= const wxFont&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetPlotFont(const wxFont& font)
{
	if (!font.IsNull() && font.IsOk())
		plotFont = font;
	/*else
		Debugger::GetInstance().Print(_T("Error setting plot font"));*/// Sometimes we just want
	// this to sort out whether or not the font is valid without telling the user.
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::CreateMenuBar(void)
{
	menuBar = new wxMenuBar();

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
	mnuFile->Append(IdMenuFileExit, _T("E&xit\tAlt+F4"), _T("Exit ") + carDesignerName, wxITEM_NORMAL);
	menuBar->Append(mnuFile, _T("&File"));

	// Edit menu
	wxMenu *mnuEdit = new wxMenu();
	mnuEdit->Append(IdMenuEditUndo, _T("&Undo\tCtrl+Z"), _T("Undo last action"), wxITEM_NORMAL);
	mnuEdit->Append(IdMenuEditRedo, _T("&Redo\tCtrl+Y"), _T("Redo last previously undone action"), wxITEM_NORMAL);
	mnuEdit->AppendSeparator();
	mnuEdit->Append(IdMenuEditCut, _T("&Cut\tCtrl+X"), _T("Cut selected to clipboard"), wxITEM_NORMAL);
	mnuEdit->Append(IdMenuEditCopy, _T("C&opy\tCtrl+C"), _T("Copy selected to clipboard"), wxITEM_NORMAL);
	mnuEdit->Append(IdMenuEditPaste, _T("&Paste\tCtrl+V"), _T("Paste from clipboard"), wxITEM_NORMAL);
	menuBar->Append(mnuEdit, _T("&Edit"));

	// View menu
	wxMenu *mnuView = new wxMenu();
	wxMenu *mnuViewToolbars = new wxMenu();
	mnuViewToolbars->AppendCheckItem(IdMenuViewToolbarsKinematic, _T("Kinematic Analysis"));
	mnuView->AppendSubMenu(mnuViewToolbars, _T("Toolbars"));
	mnuView->AppendSeparator();
	mnuView->Append(IdMenuViewClearOutput, _T("&Clear Output Text"),
		_T("Clear all text from the output pane"), wxITEM_NORMAL);
	menuBar->Append(mnuView, _T("&View"));

	// Tools menu
	wxMenu *mnuTools = new wxMenu();
	mnuTools->Append(IdMenuToolsDoE, _T("Design of &Experiment"),
		_T("Open design of experiments tool"), wxITEM_NORMAL);
	mnuTools->Append(IdMenuToolsDynamic, _T("&Dynamic Analysis"),
		_T("Start Dynamic Analysis Wizard"), wxITEM_NORMAL);
	mnuTools->AppendSeparator();
	mnuTools->Append(IdMenuToolsOptions, _T("&Options"), _T("Edit application preferences"), wxITEM_NORMAL);
	menuBar->Append(mnuTools, _T("&Tools"));

	// Help menu
	wxMenu *mnuHelp = new wxMenu();
	mnuHelp->Append(IdMenuHelpManual, _T("&User's Manual\tF1"), _T("Display user's manual"), wxITEM_NORMAL);
	mnuHelp->AppendSeparator();
	mnuHelp->Append(IdMenuHelpAbout, _T("&About"), _T("Show About dialog"), wxITEM_NORMAL);
	menuBar->Append(mnuHelp, _T("&Help"));

	// Add the manager for the recently used file menu
	recentFileManager = new wxFileHistory(maxRecentFiles, IdMenuFileRecentStart);
	recentFileManager->UseMenu(mnuRecentFiles);

	// Now make it official
	SetMenuBar(menuBar);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::CreateKinematicAnalysisToolbar(void)
{
	// Make sure we don't already have one of these
	if (kinematicToolbar != NULL)
		return;

	// Create the toolbar
	kinematicToolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxTB_FLAT | wxTB_NODIVIDER);

	// Create the controls
	wxStaticText *pitchLabel = new wxStaticText(kinematicToolbar, wxID_ANY, _T("Pitch"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxTextCtrl *pitchSet = new wxTextCtrl(kinematicToolbar, IdToolbarKinematicPitch, _T("0"), wxDefaultPosition,
		wxSize(40, -1));
	pitchSet->SetMaxLength(5);

	wxStaticText *rollLabel = new wxStaticText(kinematicToolbar, wxID_ANY, _T("Roll"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxTextCtrl *rollSet = new wxTextCtrl(kinematicToolbar, IdToolbarKinematicRoll, _T("0"), wxDefaultPosition,
		wxSize(40, -1));
	rollSet->SetMaxLength(5);

	wxStaticText *heaveLabel = new wxStaticText(kinematicToolbar, wxID_ANY, _T("Heave"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxTextCtrl *heaveSet = new wxTextCtrl(kinematicToolbar, IdToolbarKinematicHeave, _T("0"), wxDefaultPosition,
		wxSize(40, -1));
	heaveSet->SetMaxLength(5);

	wxStaticText *steerLabel = new wxStaticText(kinematicToolbar, wxID_ANY, _T("Steer"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxTextCtrl *steerSet = new wxTextCtrl(kinematicToolbar, IdToolbarKinematicSteer, _T("0"), wxDefaultPosition,
		wxSize(40, -1));
	steerSet->SetMaxLength(5);

	// Adjust the spacing a little bit

	// Add the controls to the toolbar
	kinematicToolbar->AddControl(pitchLabel);
	kinematicToolbar->AddControl(pitchSet);
	kinematicToolbar->AddSeparator();
	kinematicToolbar->AddControl(rollLabel);
	kinematicToolbar->AddControl(rollSet);
	kinematicToolbar->AddSeparator();
	kinematicToolbar->AddControl(heaveLabel);
	kinematicToolbar->AddControl(heaveSet);
	kinematicToolbar->AddSeparator();
	kinematicToolbar->AddControl(steerLabel);
	kinematicToolbar->AddControl(steerSet);

	// Make the toolbar come alive
	kinematicToolbar->Realize();

	// And add it to the wxAui Manager
	manager.AddPane(kinematicToolbar, wxAuiPaneInfo().Name(_T("KinematicToolbar")).
		Caption(_T("Kinematic Analysis")).ToolbarPane().Top().Row(1).Position(1).
		LeftDockable(false).RightDockable(false));
}

//==========================================================================
// Class:			MainFrame
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
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	// Frame top level
	EVT_CLOSE(MainFrame::WindowCloseEvent)
	EVT_SIZE(MainFrame::OnSizeEvent)

	// Menu bar
	EVT_MENU(IdMenuFileNewCar,					MainFrame::FileNewCarEvent)
	EVT_MENU(IdMenuFileNewIteration,			MainFrame::FileNewIterationEvent)
	EVT_MENU(IdMenuFileNewOptimization,			MainFrame::FileNewOptimizationEvent)
	EVT_MENU(IdMenuFileOpen,					MainFrame::FileOpenEvent)
	EVT_MENU(IdMenuFileClose,					MainFrame::FileCloseEvent)
	EVT_MENU(IdMenuFileCloseAll,				MainFrame::FileCloseAllEvent)
	EVT_MENU(IdMenuFileSave,					MainFrame::FileSaveEvent)
	EVT_MENU(IdMenuFileSaveAs,					MainFrame::FileSaveAsEvent)
	EVT_MENU(IdMenuFileSaveAll,					MainFrame::FileSaveAllEvent)
	EVT_MENU(IdMenuFileWriteImageFile,			MainFrame::FileWriteImageFileEvent)
	EVT_MENU(IdMenuFileOpenAllRecent,			MainFrame::FileOpenAllRecentEvent)
	EVT_MENU(IdMenuFileExit,					MainFrame::FileExitEvent)
	EVT_MENU(wxID_ANY,							MainFrame::OtherMenuClickEvents)

	EVT_MENU(IdMenuEditUndo,					MainFrame::EditUndoEvent)
	EVT_MENU(IdMenuEditRedo,					MainFrame::EditRedoEvent)
	EVT_MENU(IdMenuEditCut,						MainFrame::EditCutEvent)
	EVT_MENU(IdMenuEditCopy,					MainFrame::EditCopyEvent)
	EVT_MENU(IdMenuEditPaste,					MainFrame::EditPasteEvent)

	EVT_MENU(IdMenuCarAppearanceOptions,		MainFrame::CarAppearanceOptionsEvent)

	EVT_MENU(IdMenuIterationShowAssociatedCars,	MainFrame::IterationShowAssociatedCarsClickEvent)
	EVT_MENU(IdMenuIterationAssociatedWithAllCars,	MainFrame::IterationAssociatedWithAllCarsClickEvent)
	EVT_MENU(IdMenuIterationExportDataToFile,	MainFrame::IterationExportDataToFileClickEvent)
	EVT_MENU(IdMenuIterationXAxisPitch,			MainFrame::IterationXAxisPitchClickEvent)
	EVT_MENU(IdMenuIterationXAxisRoll,			MainFrame::IterationXAxisRollClickEvent)
	EVT_MENU(IdMenuIterationXAxisHeave,			MainFrame::IterationXAxisHeaveClickEvent)
	EVT_MENU(IdMenuIterationXAxisRackTravel,	MainFrame::IterationXAxisRackTravelClickEvent)

	EVT_MENU(IdMenuViewToolbarsKinematic,		MainFrame::ViewToolbarsKinematicEvent)
	EVT_MENU(IdMenuViewClearOutput,				MainFrame::ViewClearOutputEvent)

	EVT_MENU(IdMenuToolsDoE,					MainFrame::ToolsDoEEvent)
	EVT_MENU(IdMenuToolsDynamic,				MainFrame::ToolsDynamicEvent)
	EVT_MENU(IdMenuToolsOptions,				MainFrame::ToolsOptionsEvent)

	EVT_MENU(IdMenuHelpManual,					MainFrame::HelpManualEvent)
	EVT_MENU(IdMenuHelpAbout,					MainFrame::HelpAboutEvent)

	// Toolbars
	// Static Analysis
	EVT_TEXT(IdToolbarKinematicPitch,			MainFrame::KinematicToolbarPitchChangeEvent)
	EVT_TEXT(IdToolbarKinematicRoll,			MainFrame::KinematicToolbarRollChangeEvent)
	EVT_TEXT(IdToolbarKinematicHeave,			MainFrame::KinematicToolbarHeaveChangeEvent)
	EVT_TEXT(IdToolbarKinematicSteer,			MainFrame::KinematicToolbarSteerChangeEvent)

	// Threads
	EVT_COMMAND(wxID_ANY, EVT_THREAD,			MainFrame::ThreadCompleteEvent)
	EVT_COMMAND(wxID_ANY, EVT_DEBUG,			MainFrame::DebugMessageEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			MainFrame
// Function:		FileNewCarEvent
//
// Description:		Generates a new GuiCar object and adds the car to the
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
void MainFrame::FileNewCarEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create a new GuiObject with type TypeCar
	GuiObject *tempObject = new GuiCar(*this);

	// Make the new object active
	SetActiveIndex(tempObject->GetIndex());
}

//==========================================================================
// Class:			MainFrame
// Function:		FileNewIterationEvent
//
// Description:		Generates a new Iteration object and adds the it to the
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
void MainFrame::FileNewIterationEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create a new GuiObject with type TypeIteration
	GuiObject *tempObject = new Iteration(*this);

	// Make the new object active or remove it from the list (user canceled)
	if (tempObject->IsInitialized())
		SetActiveIndex(tempObject->GetIndex());
	else
		RemoveObjectFromList(tempObject->GetIndex());
}

//==========================================================================
// Class:			MainFrame
// Function:		FileNewOptimizationEvent
//
// Description:		Generates a new GeneticOptimization object and adds the
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
void MainFrame::FileNewOptimizationEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create a new GuiObject with type TypeOptimization
	GuiObject *tempObject = new GeneticOptimization(*this);

	// Make the new object active
	SetActiveIndex(tempObject->GetIndex());

}

//==========================================================================
// Class:			MainFrame
// Function:		FileOpenEvent
//
// Description:		Displays a dialog asking the user to specify the file to
//					read from.  Creates a new GuiObject, loading the
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
void MainFrame::FileOpenEvent(wxCommandEvent& WXUNUSED(event))
{
	// Set up the wildcard specifications
	// (Done here for readability)
	wxString wildcard("VVASE files (*.car; *.iteration; *.ga)|*.car;*.iteration;*.ga|");
	wildcard.append("Car files (*.car)|*.car");
	wildcard.append("|Iteration files (*.iteration)|*.iteration");
	wildcard.append("|Optimization files (*.ga)|*ga");

	// Get the file name to open from the user
	wxArrayString pathsAndFileNames = GetFileNameFromUser(_T("Open"), wxEmptyString, wxEmptyString,
		wildcard, wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);

	// Make sure the user didn't cancel
	if (pathsAndFileNames.GetCount() == 0)
		return;

	// Loop to make sure we open all selected files
	unsigned int file;
	for (file = 0; file < pathsAndFileNames.GetCount(); file++)
		LoadFile(pathsAndFileNames[file]);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::FileCloseEvent(wxCommandEvent& WXUNUSED(event))
{
	// Check to make sure there is an object to close
	if (openObjectList.GetCount() > 0)
		// Close the object of interest
		openObjectList[objectOfInterestIndex]->Close();
}

//==========================================================================
// Class:			MainFrame
// Function:		FileCloseAllEvent
//
// Description:		Calls all of the open GuiObject' close methods.
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
void MainFrame::FileCloseAllEvent(wxCommandEvent& WXUNUSED(event))
{
	// Close all of the objects
	int indexToDelete = 0;
	while (openObjectList.GetCount() > indexToDelete)
	{
		// If the user chooses not to close a particular object, we need to
		// increment our target index so that we don't keep asking about the same object.
		if (!openObjectList[indexToDelete]->Close())
			indexToDelete++;
	}
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::FileSaveEvent(wxCommandEvent& WXUNUSED(event))
{
	// Check to make sure there is an object to save
	if (openObjectList.GetCount() > 0)
		// Save the object of interest
		openObjectList[objectOfInterestIndex]->SaveToFile();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::FileSaveAsEvent(wxCommandEvent& WXUNUSED(event))
{
	// Check to make sure there is an object to save
	if (openObjectList.GetCount() > 0)
		// Save the object of interest
		openObjectList[objectOfInterestIndex]->SaveToFile(true);
}

//==========================================================================
// Class:			MainFrame
// Function:		FileSaveAllEvent
//
// Description:		Calls all of the open GuiObjects' save methods
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
void MainFrame::FileSaveAllEvent(wxCommandEvent& WXUNUSED(event))
{
	// Save all of the objects
	int indexToSave = 0;
	while (openObjectList.GetCount() > indexToSave)
	{
		// Save the object with index IndexToSave
		if (!openObjectList[indexToSave]->SaveToFile())
			break;

		// Increment our save index
		indexToSave++;
	}
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::FileOpenAllRecentEvent(wxCommandEvent& WXUNUSED(event))
{
	// Try to open every file in the list
	unsigned int i;
	for (i = 0; i < recentFileManager->GetCount(); i++)
		LoadFile(recentFileManager->GetHistoryFile(i));
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::FileWriteImageFileEvent(wxCommandEvent& WXUNUSED(event))
{
	// Check to make sure there is an object open
	if (openObjectList.GetCount() < 1)
		return;

	// Get the file name to open from the user
	wxArrayString pathAndFileName = GetFileNameFromUser(_T("Save Image File"), wxEmptyString, wxEmptyString,
		_T("Bitmap Image (*.bmp)|*.bmp|JPEG Image (*.jpg)|*.jpg|PNG Image (*.png)|*.png|TIFF Image (*.tif)|*.tif"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	// Make sure the user didn't cancel
	if (pathAndFileName.IsEmpty())
		return;

	// Call the object's write image file method
	if (openObjectList[objectOfInterestIndex]->WriteImageToFile(pathAndFileName[0]))
		Debugger::GetInstance().Print(Debugger::PriorityHigh, "Image file written to %s", pathAndFileName[0].c_str());
	else
		Debugger::GetInstance().Print(_T("Image file NOT written!"), Debugger::PriorityHigh);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::FileExitEvent(wxCommandEvent& WXUNUSED(event))
{
	// Shut down this application
	// User confirmation, etc. is handled by the CloseEvent method,
	// which is called when the form tries to close.  If we put our own
	// code here, the user is asked for confirmation twice.
	Close(true);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::OtherMenuClickEvents(wxCommandEvent &event)
{
	// If the ID isn't in the right range, skip the event and return
	if (event.GetId() < IdMenuFileRecentStart || event.GetId() > IdMenuFileRecentLast)
	{
		event.Skip();
		return;
	}

	// Make sure the index isn't greater than the number of files we actually have in the list
	if ((unsigned int)(event.GetId() - IdMenuFileRecentStart) >= recentFileManager->GetCount())
		return;

	// Attempt to load the specified file
	LoadFile(recentFileManager->GetHistoryFile(event.GetId() - IdMenuFileRecentStart));
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::EditUndoEvent(wxCommandEvent& WXUNUSED(event))
{
	// Undo the last operation
	undoRedo.Undo();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::EditRedoEvent(wxCommandEvent& WXUNUSED(event))
{
	// Redo the last undone operation
	undoRedo.Redo();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::EditCutEvent(wxCommandEvent &event)
{
	event.Skip();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::EditCopyEvent(wxCommandEvent &event)
{
	event.Skip();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::EditPasteEvent(wxCommandEvent &event)
{
	event.Skip();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::CarAppearanceOptionsEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object of interest is a car
	if (openObjectList[objectOfInterestIndex]->GetType() != GuiObject::TypeCar)
		return;

	// Show the appearance options dialog for the car
	static_cast<GuiCar*>(openObjectList[objectOfInterestIndex])
		->GetAppearanceOptions().ShowAppearanceOptionsDialog();
}

//==========================================================================
// Class:			MainFrame
// Function:		IterationShowAssociatedCarsClickEvent
//
// Description:		For Iteration objects - calls the method that displays
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
void MainFrame::IterationShowAssociatedCarsClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object is TypeIteration
	if (openObjectList[objectOfInterestIndex]->GetType() != GuiObject::TypeIteration)
		return;

	// Call the ShowAssociatedCarsDialog() method
	static_cast<Iteration*>(openObjectList[objectOfInterestIndex])->ShowAssociatedCarsDialog();
}

//==========================================================================
// Class:			MainFrame
// Function:		IterationAssociatedWithAllCarsClickEvent
//
// Description:		For Iteration objects - toggles the auto-associate function
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
void MainFrame::IterationAssociatedWithAllCarsClickEvent(wxCommandEvent &event)
{
	// Make sure the object is TypeIteration
	if (openObjectList.GetObject(objectOfInterestIndex)->GetType() != GuiObject::TypeIteration)
		return;

	// If this object is checked, set the auto-associate flag to true, otherwise
	// set it to false
	if (event.IsChecked())
		static_cast<Iteration*>(openObjectList.GetObject(objectOfInterestIndex))->SetAutoAssociate(true);
	else
		static_cast<Iteration*>(openObjectList.GetObject(objectOfInterestIndex))->SetAutoAssociate(false);
}

//==========================================================================
// Class:			MainFrame
// Function:		IterationExportDataToFileClickEvent
//
// Description:		For Iteration objects.  Calls a method that exports the
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
void MainFrame::IterationExportDataToFileClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object is TypeIteration
	if (openObjectList.GetObject(objectOfInterestIndex)->GetType() != GuiObject::TypeIteration)
		return;

	// Get the file name to export to
	wxArrayString pathAndFileName = GetFileNameFromUser(_T("Save As"), wxGetHomeDir(), wxEmptyString,
		_T("Tab delimited (*.txt)|*.txt|Comma Separated Values (*.csv)|*.csv"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	// Make sure the user didn't cancel
	if (pathAndFileName.IsEmpty())
		return;

	// Call the ExportDataToFile() method
	static_cast<Iteration*>(openObjectList.GetObject(objectOfInterestIndex))
		->ExportDataToFile(pathAndFileName.Item(0));
}

//==========================================================================
// Class:			MainFrame
// Function:		IterationXAxisPitchClickEvent
//
// Description:		Event handler for setting iteration x-axis to pitch.
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
void MainFrame::IterationXAxisPitchClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object is TypeIteration
	if (openObjectList.GetObject(objectOfInterestIndex)->GetType() != GuiObject::TypeIteration)
		return;

	// Set the X axis to pitch
	static_cast<Iteration*>(openObjectList.GetObject(objectOfInterestIndex))
		->SetXAxisType(Iteration::AxisTypePitch);

	// Uncheck all other X axis options
	if (menuBar->FindItem(IdMenuIterationXAxisPitch) != NULL)
	{
		menuBar->Check(IdMenuIterationXAxisRoll, false);
		menuBar->Check(IdMenuIterationXAxisHeave, false);
		menuBar->Check(IdMenuIterationXAxisRackTravel, false);
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		IterationXAxisRollClickEvent
//
// Description:		Event handler for setting iteration x-axis to roll.
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
void MainFrame::IterationXAxisRollClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object is TypeIteration
	if (openObjectList.GetObject(objectOfInterestIndex)->GetType() != GuiObject::TypeIteration)
		return;

	// Set the X axis to roll
	static_cast<Iteration*>(openObjectList.GetObject(objectOfInterestIndex))
		->SetXAxisType(Iteration::AxisTypeRoll);

	// Uncheck all other X axis options
	if (menuBar->FindItem(IdMenuIterationXAxisRoll) != NULL)
	{
		menuBar->Check(IdMenuIterationXAxisPitch, false);
		menuBar->Check(IdMenuIterationXAxisHeave, false);
		menuBar->Check(IdMenuIterationXAxisRackTravel, false);
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		IterationXAxisHeaveClickEvent
//
// Description:		Event handler for setting iteration x-axis to heave.
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
void MainFrame::IterationXAxisHeaveClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object is TypeIteration
	if (openObjectList.GetObject(objectOfInterestIndex)->GetType() != GuiObject::TypeIteration)
		return;

	// Set the X axis to heave
	static_cast<Iteration*>(openObjectList.GetObject(objectOfInterestIndex))
		->SetXAxisType(Iteration::AxisTypeHeave);

	// Uncheck all other X axis options
	if (menuBar->FindItem(IdMenuIterationXAxisHeave) != NULL)
	{
		menuBar->Check(IdMenuIterationXAxisPitch, false);
		menuBar->Check(IdMenuIterationXAxisRoll, false);
		menuBar->Check(IdMenuIterationXAxisRackTravel, false);
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		IterationXAxisRackTravelClickEvent
//
// Description:		Event handler for setting iteration x-axis to steer.
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
void MainFrame::IterationXAxisRackTravelClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the object is TypeIteration
	if (openObjectList.GetObject(objectOfInterestIndex)->GetType() != GuiObject::TypeIteration)
		return;

	// Set the X axis to rack travel
	static_cast<Iteration*>(openObjectList.GetObject(objectOfInterestIndex))
		->SetXAxisType(Iteration::AxisTypeRackTravel);

	// Uncheck all other X axis options
	if (menuBar->FindItem(IdMenuIterationXAxisRackTravel) != NULL)
	{
		menuBar->Check(IdMenuIterationXAxisPitch, false);
		menuBar->Check(IdMenuIterationXAxisRoll, false);
		menuBar->Check(IdMenuIterationXAxisHeave, false);
	}
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ViewToolbarsKinematicEvent(wxCommandEvent &event)
{
	// Determine if we are supposed to hide or show the menu
	// FIXME:  This is still a little funky...
	if (event.IsChecked())
		CreateKinematicAnalysisToolbar();
	else if (kinematicToolbar != NULL)
	{
		manager.DetachPane(kinematicToolbar);

		delete kinematicToolbar;
		kinematicToolbar = NULL;
	}
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ViewClearOutputEvent(wxCommandEvent& WXUNUSED(event))
{
	// Set the text in the DebugPane to an empty string
	debugPane->ChangeValue(_T(""));
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ToolsDoEEvent(wxCommandEvent &event)
{
	event.Skip();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ToolsDynamicEvent(wxCommandEvent &event)
{
	event.Skip();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ToolsOptionsEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create the dialog box
	OptionsDialog optionsDialog(*this, kinematicInputs, wxID_ANY, wxDefaultPosition);

	// Display the dialog
	if (optionsDialog.ShowModal() == wxOK)
	{
		// FIXME:  Write the updated options to the registry

		// Update the edit panel
		editPanel->UpdateInformation();

		// Update the analyses
		UpdateAnalysis();
		UpdateOutputPanel();

		// Make sure we have an object to update before we try to update it
		if (openObjectList.GetCount() > 0)
			openObjectList.GetObject(activeIndex)->UpdateData();
	}
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::HelpManualEvent(wxCommandEvent& WXUNUSED(event))
{
	// Call the shell to display the user's manual
	wxString openPDFManualCommand;
	wxString manualFileName(_T("VVASE Manual.pdf"));
	wxMimeTypesManager mimeManager;

	// In Linux, we need to put the file name in quotes
#ifdef __WXGTK__
	manualFileName.Prepend(_T("'"));
	manualFileName.Append(_T("'"));
#endif

	wxFileType *pdfFileType = mimeManager.GetFileTypeFromExtension(_T("pdf"));
	if (!pdfFileType->GetOpenCommand(&openPDFManualCommand,
		wxFileType::MessageParameters(manualFileName)))
		Debugger::GetInstance().Print(_T("ERROR:  Could not determine how to open .pdf files!"));
	else
	{
		if (wxExecute(openPDFManualCommand) == 0)
			Debugger::GetInstance().Print(_T("ERROR:  Could not find 'VVASE Manual.pdf'!"));// FIXME:  Use manualFileName
	}
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::HelpAboutEvent(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo appInfo;

	// Fill in the information
	appInfo.SetName(carDesignerLongName);
	appInfo.SetVersion(carDesignerVersion);
	appInfo.SetDescription(_T("\n\
A work in progress...\n\
This is a vehicle design and analysis tool.  Please see the\n\
readme.txt file for licensing and other information."));
	appInfo.SetCopyright(_T("(C) 2008-2010 Kerry Loux"));

	// Display the information
	wxAboutBox(appInfo);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::UpdateAnalysis(void)
{
	// For every object we've got open, call the update display method
	int i;
	for (i = 0; i < openObjectList.GetCount(); i++)
		// Update the display (this performs the kinematic analysis)
		openObjectList.GetObject(i)->UpdateData();
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateOutputPanel
//
// Description:		Updates the output display with the information currently
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
void MainFrame::UpdateOutputPanel(void)
{
	// For every object we've got open, call the update display method
	int i, carCount = 0;
	for (i = 0; i < openObjectList.GetCount(); i++)
	{
		// Update the kinematics information (ONLY if this is a car)
		if (openObjectList.GetObject(i)->GetType() == GuiObject::TypeCar)
		{
			// Increment the number of cars we have
			carCount++;

			// Update the information for this car
			outputPanel->UpdateInformation(static_cast<GuiCar*>(openObjectList.GetObject(i))->GetKinematicOutputs(),
				static_cast<GuiCar*>(openObjectList.GetObject(i))->GetWorkingCar(),
				carCount, openObjectList.GetObject(i)->GetCleanName());
		}
	}

	// Make sure the output panel doesn't have more data columns than there are cars
	outputPanel->FinishUpdate(carCount);
}

//==========================================================================
// Class:			MainFrame
// Function:		AddJob
//
// Description:		Adds a job to the job queue to be handled by the thread pool.
//
// Input Arguments:
//		newJob	= ThreadJob& containing in the information about the new job to
//				  be performed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::AddJob(ThreadJob &newJob)
{
	// Make sure we have a thread to handle the job
	assert(activeThreads > 0);

	// Add the job to the queue
	jobQueue->AddJob(newJob, JobQueue::PriorityNormal);

	// Increment the open job counter
	openJobCount++;
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::KinematicToolbarPitchChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Get a pointer to the pitch text box
	wxTextCtrl *textBox = static_cast<wxTextCtrl*>(kinematicToolbar->FindControl(IdToolbarKinematicPitch));

	// Make sure the text box was found (may not be found on startup)
	if (textBox == NULL)
		return;

	// Get the value from the text box and convert it to a double
	double value;
	if (!textBox->GetValue().ToDouble(&value))
		// The value was non-numeric - don't do anything
		return;

	// Set the value for the kinematic analysis object
	kinematicInputs.pitch = Convert::GetInstance().ReadAngle(value);

	// Update the analysis
	UpdateAnalysis();
	UpdateOutputPanel();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::KinematicToolbarRollChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Get a pointer to the roll text box
	wxTextCtrl *textBox = static_cast<wxTextCtrl*>(kinematicToolbar->FindControl(IdToolbarKinematicRoll));

	// Make sure the text box was found (may not be found on startup)
	if (textBox == NULL)
		return;

	// Get the value from the text box and convert it to a double
	double value;
	if (!textBox->GetValue().ToDouble(&value))
		// The value was non-numeric - don't do anything
		return;

	// Set the value for the kinematic analysis object
	kinematicInputs.roll = Convert::GetInstance().ReadAngle(value);

	// Update the analysis
	UpdateAnalysis();
	UpdateOutputPanel();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::KinematicToolbarHeaveChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Get a pointer to the heave text box
	wxTextCtrl *textBox = static_cast<wxTextCtrl*>(kinematicToolbar->FindControl(IdToolbarKinematicHeave));

	// Make sure the text box was found (may not be found on startup)d
	if (textBox == NULL)
		return;

	// Get the value from the text box and convert it to a double
	double value;
	if (!textBox->GetValue().ToDouble(&value))
		// The value was non-numeric - don't do anything
		return;

	// Set the value for the kinematic analysis object
	kinematicInputs.heave = Convert::GetInstance().ReadDistance(value);

	// Update the analysis
	UpdateAnalysis();
	UpdateOutputPanel();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::KinematicToolbarSteerChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Get a pointer to the steer text box
	wxTextCtrl *textBox = static_cast<wxTextCtrl*>(kinematicToolbar->FindControl(IdToolbarKinematicSteer));

	// Make sure the text box was found (may not be found on startup)
	if (textBox == NULL)
		return;

	// Get the value from the text box and convert it to a double
	double value;
	if (!textBox->GetValue().ToDouble(&value))
		// The value was non-numeric - don't do anything
		return;

	// Set the value for the kinematic analysis object depending on what the steering input represents
	if (useRackTravel)
		kinematicInputs.rackTravel = Convert::GetInstance().ReadDistance(value);
	else
		kinematicInputs.rackTravel = Convert::GetInstance().ReadAngle(value) * 1.0;// * RackRatio;// FIXME:  Use rack ratio

	// Update the analysis
	UpdateAnalysis();
	UpdateOutputPanel();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ThreadCompleteEvent(wxCommandEvent &event)
{
	int carCount(0), i;

	// Perform different operations depending on the type of job that has completed
	switch (event.GetInt())
	{
	case ThreadJob::CommandThreadExit:
		// Decrement the number of active threads
		activeThreads--;
		Debugger::GetInstance().Print(Debugger::PriorityLow, "Thread %i exited", event.GetId());

		// If there are no more active threads, it is now safe to kill this window
		if (activeThreads == 0)
		{
			// Kill the window
			Destroy();
			return;
		}
		break;

	case ThreadJob::CommandThreadStarted:
		// Increment the number of active threads
		activeThreads++;
		Debugger::GetInstance().Print(Debugger::PriorityLow, "Thread %i started", event.GetId());
		break;

	case ThreadJob::CommandThreadKinematicsNormal:
		// Get the car count for this car (number of objects before this in the list that are also cars)
		for (i = 0; i <= event.GetExtraLong(); i++)
		{
			// Iterate through the open objects up to the selected object, and if it is a car,
			// increment the car count
			if (openObjectList.GetObject(i)->GetType() == GuiObject::TypeCar)
				carCount++;
		}

		// Update the information for this car
		outputPanel->UpdateInformation(static_cast<GuiCar*>(openObjectList.GetObject(
			event.GetExtraLong()))->GetKinematicOutputs(),
			static_cast<GuiCar*>(openObjectList.GetObject(event.GetExtraLong()))->GetWorkingCar(),
			carCount, openObjectList[event.GetExtraLong()]->GetCleanName());

		// Call the 3D display update method
		openObjectList[event.GetExtraLong()]->UpdateDisplay();
		break;

	case ThreadJob::CommandThreadKinematicsIteration:
		// Decrement the semaphore for the current iteration
		static_cast<Iteration*>(openObjectList[event.GetExtraLong()])->MarkAnalysisComplete();
		break;

	case ThreadJob::CommandThreadKinematicsGA:
		// Tell the GA manager thread that an analysis is done (decrement the count of pending analyses)
		static_cast<GeneticOptimization*>(openObjectList[event.GetExtraLong()])->MarkAnalysisComplete();
		break;

	case ThreadJob::CommandThreadGeneticOptimization:
		// Update the optimized car and mark the optimization object as complete
		static_cast<GeneticOptimization*>(openObjectList[event.GetExtraLong()])->CompleteOptimization();

		// Update the displays
		UpdateAnalysis();
		UpdateOutputPanel();
		break;

	case ThreadJob::CommandThreadNull:
	default:
		break;
	}

	// Decrement the job counter
	openJobCount--;
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::DebugMessageEvent(wxCommandEvent &event)
{
	// Print a message to the output pane
	debugPane->AppendText(event.GetString());

#ifdef _DEBUG_TO_FILE_
	// Useful for cases where the application crashes and text can no longer be viewed in the output pane
	// FIXME:  Make this part of the Debugger class?
	std::ofstream file("debug.txt", std::ios::app);
	file << event.GetString();
	file.close();
#endif
}

//==========================================================================
// Class:			MainFrame
// Function:		AddObjectToList
//
// Description:		Add a GuiObject object to the list of managed cars.  This
//					function should be called immediately after creation of
//					a new GuiObject.  The usual syntax and calling
//					sequence looks something like this (it is also important
//					to assign the index back to the GuiObject for future use):
//						GuiObject *NewObject = new GuiObject(this);
//						 NewObject->SetIndex(AddObjectToList(NewObject));
//
// Input Arguments:
//		objectToAdd	= *GuiObject
//
// Output Arguments:
//		None
//
// Return Value:
//		integer = new index for the car that was just added to the list
//
//==========================================================================
int MainFrame::AddObjectToList(GuiObject *objectToAdd)
{
	// Add the object to the list (must be done before the menu permissions are updated)
	return openObjectList.Add(objectToAdd);
}

//==========================================================================
// Class:			MainFrame
// Function:		RemoveObjectFromList
//
// Description:		Removes a GuiObject object from the list of managed cars.
//					Should be done instead of deleting the GuiObject object.
//					This function will handle the deletion of the GuiObject
//					internally.
//
// Input Arguments:
//		index	= integer specifying the object to be removed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::RemoveObjectFromList(int index)
{
	// Set the deletion status flag
	beingDeleted = true;

	// Delete the specified object from the list
	openObjectList.Remove(index);

	// Reset the cars' indices and check to see what types we have available
	// Also refresh the car's displays
	int i;
	for (i = 0; i < openObjectList.GetCount(); i++)
	{
		// Re-set the index
		openObjectList[i]->SetIndex(i);

		// Update the data and displays - data first, because in some cases data is
		// dependent on other open objects, and we may have just closed one
		openObjectList[i]->UpdateData();// FIXME:  This spawns threads (or multiple threads) - what happens when we close and multiple objects are deleted one after another?
		openObjectList[i]->UpdateDisplay();
	}

	// Reset the deletion status flag
	beingDeleted = false;

	// Reset the active index - if there is still an open object, show the one with index zero
	// Otherwise, set the index to an invalid number
	if (openObjectList.GetCount() > 0)
		SetActiveIndex(0);
	else
		SetActiveIndex(-1);

	// Update the output display
	UpdateOutputPanel();
}

//==========================================================================
// Class:			MainFrame
// Function:		SetNotebookPage
//
// Description:		Sets the notebook page to the specified index.  The
//					index here should start at 0, just like the
//					ActiveIndex.
//
// Input Arguments:
//		index	= integer specifying the notebook page to activate
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetNotebookPage(int index)
{
	// Make sure the index could possibly be a valid index
	if (index >= 0 && index < (signed int)openObjectList.GetCount())
	{
		// Bring the desired notebook page to the front
		notebook->SetSelection(index);

		// Update the active object's display
		// (haven't identified the root cause, but sometimes the car disappears if this isn't done)
		// FIXME:  This works as a workaround for disappearing cars/plots as happens sometimes, but
		// causes intermittent crashes (access violations) for cars and always crashes new iterations
		// in FormatPlot() (Z values are NaN)
		//openObjectList[ActiveIndex]->UpdateDisplay();
	}
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::WindowCloseEvent(wxCloseEvent& WXUNUSED(event))
{
	// Write the application configuration information to the registry
	WriteConfiguration();

	// Kill this window if there aren't any more worker threads (assumes we've already
	// been through this once, and issued the EXIT command to all of the threads -
	// no need to ask for confirmation again!).  In practice, this should never happen,
	// as the window will be destroyed when the last thread exits.
	if (activeThreads == 0)
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
	for (i = 0; i < activeThreads; i++)
		jobQueue->AddJob(ThreadJob(ThreadJob::CommandThreadExit), JobQueue::PriorityVeryHigh);
}

//==========================================================================
// Class:			MainFrame
// Function:		OnSizeEvent
//
// Description:		Event handler for re-sizing events.  When this event triggers,
//					we need to update the displays for all of our GuiObjectS.
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
void MainFrame::OnSizeEvent(wxSizeEvent& WXUNUSED(event))
{
	// For each open object, update the display
	/*int i;
	for (i = 0; i < openObjectList.GetCount(); i++)
		openObjectList[i]->UpdateDisplay();*/
	// FIXME:  This is out-of-phase with the actualy re-sizing!
}

//==========================================================================
// Class:			MainFrame
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
bool MainFrame::CloseThisForm(void)
{
	// Cycle through all of the cars and close them
	while (openObjectList.GetCount() > 0)
	{
		// Attempt to close each object
		if (!openObjectList.GetObject(openObjectList.GetCount() - 1)->Close(false))
			// If  the user canceled, we should cancel here, too
			return false;
	}

	// If we have not yet returned false, then return true
	return true;
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ReadConfiguration(void)
{
	// Create a configuration file object
	wxFileConfig *configurationFile = new wxFileConfig(_T(""), _T(""),
		wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() + pathToConfigFile, _T(""),
		wxCONFIG_USE_RELATIVE_PATH);

	// Read UNITS configuration from file
	Convert::GetInstance().SetAccelerationUnits(Convert::UnitsOfAcceleration(
		configurationFile->Read(_T("/Units/Acceleration"), 0l)));
	Convert::GetInstance().SetAngleUnits(Convert::UnitsOfAngle(
		configurationFile->Read(_T("/Units/Angle"), 1l)));
	Convert::GetInstance().SetAreaUnits(Convert::UnitsOfArea(
		configurationFile->Read(_T("/Units/Area"), 0l)));
	Convert::GetInstance().SetDensityUnits(Convert::UnitsOfDensity(
		configurationFile->Read(_T("/Units/Density"), 0l)));
	Convert::GetInstance().SetDistanceUnits(Convert::UnitsOfDistance(
		configurationFile->Read(_T("/Units/Distance"), 0l)));
	Convert::GetInstance().SetEnergyUnits(Convert::UnitsOfEnergy(
		configurationFile->Read(_T("/Units/Energy"), 0l)));
	Convert::GetInstance().SetForceUnits(Convert::UnitsOfForce(
		configurationFile->Read(_T("/Units/Force"), 0l)));
	Convert::GetInstance().SetInertiaUnits(Convert::UnitsOfInertia(
		configurationFile->Read(_T("/Units/Inertia"), 0l)));
	Convert::GetInstance().SetMassUnits(Convert::UnitsOfMass(
		configurationFile->Read(_T("/Units/Mass"), 0l)));
	Convert::GetInstance().SetMomentUnits(Convert::UnitsOfMoment(
		configurationFile->Read(_T("/Units/Moment"), 0l)));
	Convert::GetInstance().SetPowerUnits(Convert::UnitsOfPower(
		configurationFile->Read(_T("/Units/Power"), 0l)));
	Convert::GetInstance().SetPressureUnits(Convert::UnitsOfPressure(
		configurationFile->Read(_T("/Units/Pressure"), 0l)));
	Convert::GetInstance().SetTemperatureUnits(Convert::UnitsOfTemperature(
		configurationFile->Read(_T("/Units/Temperature"), 0l)));
	Convert::GetInstance().SetVelocityUnits(Convert::UnitsOfVelocity(
		configurationFile->Read(_T("/Units/Velocity"), 0l)));

	// Read NUMBER FORMAT configuration from file
	Convert::GetInstance().SetNumberOfDigits(configurationFile->Read(_T("/NumberFormat/NumberOfDigits"), 3l));
	bool tempBool = Convert::GetInstance().GetUseScientificNotation();
	configurationFile->Read(_T("/NumberFormat/UseScientificNotation"), &tempBool);
	Convert::GetInstance().SetUseScientificNotation(tempBool);
	tempBool = Convert::GetInstance().GetUseSignificantDigits();
	configurationFile->Read(_T("/NumberFormat/UseSignificantDigits"), &tempBool);
	Convert::GetInstance().SetUseSignificantDigits(tempBool);

	// Read KINEMATICS configuration from file
	double tempDouble = 0.0;
	configurationFile->Read(_T("/Kinematics/CenterOfRotationX"), &tempDouble);
	kinematicInputs.centerOfRotation.x = tempDouble;
	tempDouble = 0.0;
	configurationFile->Read(_T("/Kinematics/CenterOfRotationY"), &tempDouble);
	kinematicInputs.centerOfRotation.y = tempDouble;
	tempDouble = 0.0;
	configurationFile->Read(_T("/Kinematics/CenterOfRotationZ"), &tempDouble);
	kinematicInputs.centerOfRotation.z = tempDouble;
	kinematicInputs.firstRotation = (Vector::Axis)configurationFile->Read(
		_T("/Kinematics/FirstRotation"), 0l);
	configurationFile->Read(_T("/Kinematics/UseRackTravel"), &useRackTravel, true);

	// Read DEBUGGING configuration from file
	Debugger::GetInstance().SetDebugLevel(Debugger::DebugLevel(configurationFile->Read(_T("/Debugging/DebugLevel"), 1l)));

	// Read GUI configuration from file
	wxString layoutString;
	if (configurationFile->Read(_T("/GUI/LayoutString"), &layoutString))
		manager.LoadPerspective(layoutString);
	tempBool = false;
	if (configurationFile->Read(_T("/GUI/IsMaximized"), &tempBool))
		Maximize(tempBool);
	else// FIXME:  This doesn't work as expected under GTK (needs testing under MSW)
	{
		SetSize(configurationFile->Read(_T("/GUI/SizeX"), 1024l),
			configurationFile->Read(_T("/GUI/SizeY"), 768l));
		int xPosition = 0, yPosition = 0;
		if (configurationFile->Read(_T("/GUI/PositionX"), &xPosition)
			&& configurationFile->Read(_T("/GUI/PositionY"), &yPosition))
			SetPosition(wxPoint(xPosition, yPosition));
		else
			Center();
	}

	// Read SOLVER configuration from file
	SetNumberOfThreads(configurationFile->Read(_T("/SOLVER/NumberOfThreads"), wxThread::GetCPUCount() * 2));
	
	// Read FONT configuration from file
	wxFont font;
	font.SetNativeFontInfo(configurationFile->Read(_T("/FONTS/OutputFont"), wxEmptyString));
	SetOutputFont(font);
	font.SetNativeFontInfo(configurationFile->Read(_T("/FONTS/PlotFont"), wxEmptyString));
	SetPlotFont(font);

	// Read recent file history
	recentFileManager->Load(*configurationFile);

	// Delete file object
	delete configurationFile;
	configurationFile = NULL;
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::WriteConfiguration(void)
{
	// Create a configuration file object
	wxFileConfig *configurationFile = new wxFileConfig(_T(""), _T(""),
		wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() + pathToConfigFile, _T(""),
		wxCONFIG_USE_RELATIVE_PATH);

	// Write UNITS configuration to file
	configurationFile->Write(_T("/Units/Acceleration"), Convert::GetInstance().GetAccelerationUnits());
	configurationFile->Write(_T("/Units/Angle"), Convert::GetInstance().GetAngleUnits());
	configurationFile->Write(_T("/Units/Area"), Convert::GetInstance().GetAreaUnits());
	configurationFile->Write(_T("/Units/Density"), Convert::GetInstance().GetDensityUnits());
	configurationFile->Write(_T("/Units/Distance"), Convert::GetInstance().GetDistanceUnits());
	configurationFile->Write(_T("/Units/Energy"), Convert::GetInstance().GetEnergyUnits());
	configurationFile->Write(_T("/Units/Force"), Convert::GetInstance().GetForceUnits());
	configurationFile->Write(_T("/Units/Inertia"), Convert::GetInstance().GetInertiaUnits());
	configurationFile->Write(_T("/Units/Mass"), Convert::GetInstance().GetMassUnits());
	configurationFile->Write(_T("/Units/Moment"), Convert::GetInstance().GetMomentUnits());
	configurationFile->Write(_T("/Units/Power"), Convert::GetInstance().GetPowerUnits());
	configurationFile->Write(_T("/Units/Pressure"), Convert::GetInstance().GetPressureUnits());
	configurationFile->Write(_T("/Units/Temperature"), Convert::GetInstance().GetTemperatureUnits());
	configurationFile->Write(_T("/Units/Velocity"), Convert::GetInstance().GetVelocityUnits());

	// Write NUMBER FORMAT configuration to file
	configurationFile->Write(_T("/NumberFormat/NumberOfDigits"), Convert::GetInstance().GetNumberOfDigits());
	configurationFile->Write(_T("/NumberFormat/UseScientificNotation"), Convert::GetInstance().GetUseScientificNotation());
	configurationFile->Write(_T("/NumberFormat/UseSignificantDigits"), Convert::GetInstance().GetUseSignificantDigits());

	// Write KINEMATICS configuration to file
	configurationFile->Write(_T("/Kinematics/CenterOfRotationX"), kinematicInputs.centerOfRotation.x);
	configurationFile->Write(_T("/Kinematics/CenterOfRotationY"), kinematicInputs.centerOfRotation.y);
	configurationFile->Write(_T("/Kinematics/CenterOfRotationZ"), kinematicInputs.centerOfRotation.z);
	configurationFile->Write(_T("/Kinematics/FirstRotation"), kinematicInputs.firstRotation);
	configurationFile->Write(_T("/Kinematics/UseRackTravel"), useRackTravel);

	// Write DEBUGGING configuration to file
	configurationFile->Write(_T("/Debugging/DebugLevel"), Debugger::GetInstance().GetDebugLevel());

	// Write GUI configuration to file
	configurationFile->Write(_T("/GUI/LayoutString"), manager.SavePerspective());
	configurationFile->Write(_T("/GUI/IsMaximized"), IsMaximized());
	configurationFile->Write(_T("/GUI/SizeX"), GetSize().GetX());
	configurationFile->Write(_T("/GUI/SizeY"), GetSize().GetY());
	configurationFile->Write(_T("/GUI/PositionX"), GetPosition().x);
	configurationFile->Write(_T("/GUI/PositionY"), GetPosition().y);

	// Write SOLVER configuration to file
	configurationFile->Write(_T("/SOLVER/NumberOfThreads"), numberOfThreads);
	
	// Write FONTS configuration to file
	if (outputFont.IsOk())
		configurationFile->Write(_T("/FONTS/OutputFont"),
			outputFont.GetNativeFontInfoDesc());
	if (plotFont.IsOk())
		configurationFile->Write(_T("/FONTS/PlotFont"),
			plotFont.GetNativeFontInfoDesc());

	// Write recent file history
	recentFileManager->Save(*configurationFile);

	// Delete file object
	delete configurationFile;
	configurationFile = NULL;
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::UpdateActiveObjectMenu(void)
{
	// Try to get a handle to object specific menus
	int carMenuIndex = menuBar->FindMenu(_T("Car"));
	int iterationMenuIndex = menuBar->FindMenu(_T("Iteration"));
	//int gaMenuIndex = menuBar->FindMenu(_T("GA"));

	// Store the type of the active object
	GuiObject::ItemType activeType;

	// Make sure the active index is valid
	if (activeIndex < 0)
		activeType = GuiObject::TypeNone;
	else
		activeType = openObjectList[activeIndex]->GetType();

	switch (activeType)
	{
	case GuiObject::TypeCar:
		// Check to see if the car menu already exists
		if (carMenuIndex == wxNOT_FOUND)
		{
			// Check to see if we first need to remove the iteration menu
			if (iterationMenuIndex == wxNOT_FOUND)
				// Just insert a new menu
				menuBar->Insert(3, CreateCarMenu(), _T("&Car"));
			else
				// Replace the iteration menu with a new car menu
				delete menuBar->Replace(iterationMenuIndex, CreateCarMenu(), _T("&Car"));
		}
		break;

	case GuiObject::TypeIteration:
		// Check to see if the iteration menu already exists
		if (iterationMenuIndex == wxNOT_FOUND)
		{
			// Check to see if we first need to remove the car menu
			if (carMenuIndex == wxNOT_FOUND)
				// Just insert a new menu
				menuBar->Insert(3, CreateIterationMenu(), _T("&Iteration"));
			else
				// Replace the car menu with a new iteration menu
				delete menuBar->Replace(carMenuIndex, CreateIterationMenu(), _T("&Iteration"));
		}
		else
			// For iteration objects, we need to update the menu for every object
			// Just having the menu there already isn't good enough (checks, etc.)
			// So we replace the existing Iteration menu with a new one
			delete menuBar->Replace(iterationMenuIndex, CreateIterationMenu(), _T("&Iteration"));
		break;

	// Unused cases
	case GuiObject::TypeNone:
	case GuiObject::TypeOptimization:
	default:
		// Remove all menus
		if (carMenuIndex != wxNOT_FOUND)
			delete menuBar->Remove(carMenuIndex);
		else if (iterationMenuIndex != wxNOT_FOUND)
			delete menuBar->Remove(iterationMenuIndex);
		/*else if (gaMenuIndex != wxNOT_FOUND)
			delete menuBar->Remove(gaMenuIndex);*/
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		SetActiveIndex
//
// Description:		Sets the active index to the specified value and brings
//					the associated notebook page to the front.
//
// Input Arguments:
//		index				= integer specifying the current active object
//		selectNotebookTab	= bool indicated whether or not to change the current
//							  notebook page (optional)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetActiveIndex(int index, bool selectNotebookTab)
{
	// Make sure the index is valid
	if (index >= (signed int)openObjectList.GetCount())
		return;

	// Set the active index
	activeIndex = index;

	// Also update the object of interest index
	objectOfInterestIndex = activeIndex;

	// Bring the appropriate notebook page to the top and render the scene (if desired)
	if (selectNotebookTab)
		SetNotebookPage(activeIndex);

	if (activeIndex >= 0)
	{
		// Highlight the column in the output panel that corresponds to this car
		outputPanel->HighlightColumn(openObjectList.GetObject(activeIndex)->GetCleanName());

		// Update the EditPanel
		editPanel->UpdateInformation(openObjectList.GetObject(activeIndex));
	}
	else
		editPanel->UpdateInformation(NULL);

	// If the active object is not selected in the SystemsTree, select it now
	// Get the selected item's ID
	wxTreeItemId selectedId;
	systemsTree->GetSelectedItem(&selectedId);

	// Make sure there was an item selected (ID is valid?)
	if (selectedId.IsOk() && activeIndex >= 0)
	{
		// Check to see if the ID belongs to our active object
		if (!openObjectList.GetObject(activeIndex)->IsThisObjectSelected(selectedId))
			// If it does not, then select the root item for the newly active object
			openObjectList.GetObject(activeIndex)->SelectThisObjectInTree();
	}

	// Update the object specific menus
	UpdateActiveObjectMenu();
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateContextMenu
//
// Description:		Displays a context menu that is customized for the object
//					specified by ObjectIndex.
//
// Input Arguments:
//		objectIndex		= integer specifying the object in the openObjectList
//						  that this menu is being created for
//		position		= wxPoint specifying the position to display the menu
//		allowClosing	= bool, specifying whether or not we should add the
//						  "Close" item to the menu
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::CreateContextMenu(int objectIndex, wxPoint position, bool allowClosing)
{
	// Make sure the index is valid before continuing
	if (objectIndex < 0 || objectIndex >= (signed int)openObjectList.GetCount())
		return;

	// Set the object of interest to the specified object
	objectOfInterestIndex = objectIndex;

	// Declare the menu variable and get the position of the cursor
	wxMenu *contextMenu(NULL);

	// Depending on the type of the item, we might want additional options (or none at all)
	switch (openObjectList.GetObject(objectIndex)->GetType())
	{
	case GuiObject::TypeCar:
		contextMenu = CreateCarMenu();
		break;

	case GuiObject::TypeIteration:
		contextMenu = CreateIterationMenu();
		break;

	// Unused types
	case GuiObject::TypeOptimization:
	case GuiObject::TypeNone:
		break;

	// Fail on unknown types to avoid forgetting any
	default:
		assert(0);
		return;
	}

	// Start building the context menu

	// Genetic algorithms do not have image files
	if (openObjectList.GetObject(objectIndex)->GetType() != GuiObject::TypeOptimization)
	{
		contextMenu->PrependSeparator();
		contextMenu->Prepend(IdMenuFileWriteImageFile, _T("&Write Image File"));
	}

	// Only add the close item if it was specified that we should
	if (allowClosing)
		contextMenu->Prepend(IdMenuFileClose, _T("&Close"));

	contextMenu->Prepend(IdMenuFileSave, _T("&Save"));

	// Show the menu
	PopupMenu(contextMenu, position);

	// Delete the context menu object
	delete contextMenu;
	contextMenu = NULL;

	// The event handlers for whatever was clicked get called here
	// Reset the object index to the active object index
	objectOfInterestIndex = activeIndex;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateCarMenu
//
// Description:		Creates a drop-down menu for TypeCar objects.
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
wxMenu *MainFrame::CreateCarMenu(void)
{
	// Car menu
	wxMenu *mnuCar = new wxMenu();
	mnuCar->Append(IdMenuCarAppearanceOptions, _T("Appearance Options"));

	return mnuCar;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateIterationMenu
//
// Description:		Creates a drop-down menu for TypeIteration objects.
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
wxMenu *MainFrame::CreateIterationMenu(void)
{
	// Iteration menu
	wxMenu *mnuIteration = new wxMenu();

	// Make sure the active object is a TYPE_ITERATION object
	if (openObjectList.GetObject(activeIndex)->GetType() != GuiObject::TypeIteration)
		return mnuIteration;

	// Allocate the sub-menus
	wxMenu *associatedCarsMenu = new wxMenu();
	wxMenu *xAxisMenu = new wxMenu();

	mnuIteration->Append(IdMenuIterationExportDataToFile, _T("Export Data"));

	// Create and append the associated cars sub-menu
	associatedCarsMenu->Append(IdMenuIterationShowAssociatedCars, _T("Choose Associated Cars"));
	associatedCarsMenu->AppendSeparator();
	associatedCarsMenu->AppendCheckItem(IdMenuIterationAssociatedWithAllCars, _T("Associate With All Cars"));
	mnuIteration->AppendSubMenu(associatedCarsMenu, _T("Associated Cars"));

	// Create and append the x-axis sub-menu
	xAxisMenu->AppendCheckItem(IdMenuIterationXAxisPitch, _T("Pitch"));
	xAxisMenu->AppendCheckItem(IdMenuIterationXAxisRoll, _T("Roll"));
	xAxisMenu->AppendCheckItem(IdMenuIterationXAxisHeave, _T("Heave"));
	xAxisMenu->AppendCheckItem(IdMenuIterationXAxisRackTravel, _T("Rack Travel"));
	mnuIteration->AppendSubMenu(xAxisMenu, _T("Set X-Axis"));

	// Determine which items need to be checked
	if (static_cast<Iteration*>(openObjectList.GetObject(activeIndex))->GetAutoAssociate())
		associatedCarsMenu->Check(IdMenuIterationAssociatedWithAllCars, true);

	switch (static_cast<Iteration*>(openObjectList.GetObject(activeIndex))->GetXAxisType())
	{
	case Iteration::AxisTypePitch:
		xAxisMenu->Check(IdMenuIterationXAxisPitch, true);
		break;

	case Iteration::AxisTypeRoll:
		xAxisMenu->Check(IdMenuIterationXAxisRoll, true);
		break;

	case Iteration::AxisTypeHeave:
		xAxisMenu->Check(IdMenuIterationXAxisHeave, true);
		break;

	case Iteration::AxisTypeRackTravel:
		xAxisMenu->Check(IdMenuIterationXAxisRackTravel, true);
		break;

	case Iteration::AxisTypeUnused:
		// Take no action
		break;

	default:
		assert(0);
		break;
	}

	return mnuIteration;
}

//==========================================================================
// Class:			MainFrame
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
bool MainFrame::JobsPending(void) const
{
	// If there are any jobs in the queue, return false
	if (jobQueue->PendingJobs() > 0)
		return true;

	// If there are any open jobs, return false
	if (openJobCount > 0)
		return true;

	// Otherwise, return true
	return false;
}

//==========================================================================
// Class:			MainFrame
// Function:		GetFileNameFromUser
//
// Description:		Displays a dialog asking the user to specify a file name.
//					Arguments allow this to be for opening or saving files,
//					with different options for the wildcards.
//
// Input Arguments:
//		dialogTitle			= wxString containing the title for the dialog
//		defaultDirectory	= wxString specifying the initial directory
//		defaultFileName		= wxString specifying the default file name
//		wildcard			= wxString specifying the list of file types to
//							  allow the user to select
//		style				= long integer specifying the type of dialog
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
wxArrayString MainFrame::GetFileNameFromUser(wxString dialogTitle, wxString defaultDirectory,
										 wxString defaultFileName, wxString wildcard, long style)
{
	// Initialize the return variable
	wxArrayString pathsAndFileNames;

	// Step 1 is to ask the user to specify the file name
	wxFileDialog dialog(this, dialogTitle, defaultDirectory, defaultFileName,
		wildcard, style);

	// Set the dialog to display center screen at the user's home directory
	dialog.CenterOnParent();

	// Display the dialog and make sure the user clicked OK
	if (dialog.ShowModal() == wxID_OK)
	{
		// If this was an open dialog, we want to get all of the selected paths,
		// otherwise, just get the one selected path
		if (style & wxFD_OPEN)
			dialog.GetPaths(pathsAndFileNames);
		else
			pathsAndFileNames.Add(dialog.GetPath());
	}

	// Return the path and file name
	return pathsAndFileNames;
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadFile
//
// Description:		Public method for loading a single object from file.
//
// Input Arguments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool MainFrame::LoadFile(wxString pathAndFileName)
{
	int startOfExtension;
	wxString fileExtension;
	GuiObject *tempObject = NULL;

	// Decipher the file name to figure out what kind of object this is
	startOfExtension = pathAndFileName.Last('.') + 1;
	fileExtension = pathAndFileName.Mid(startOfExtension);

	// FIXME:  This could be extension agnostic if we read something from the file header

	// Create the appropriate object
	if (fileExtension.CmpNoCase("car") == 0)
		tempObject = new GuiCar(*this, pathAndFileName);
	else if (fileExtension.CmpNoCase("iteration") == 0)
		tempObject = new Iteration(*this, pathAndFileName);
	else if (fileExtension.CmpNoCase("ga") == 0)
		tempObject = new GeneticOptimization(*this, pathAndFileName);
	else
	{
		// Print error message to screen
		Debugger::GetInstance().Print(_T("ERROR:  Unrecognized file extension: '") + fileExtension + _T("'"));
		return false;
	}

	// If the object is not initialized, remove it from the list
	// (user canceled or errors occurred)
	if (!tempObject->IsInitialized())
	{
		RemoveObjectFromList(tempObject->GetIndex());
		return false;
	}
	else
		// Make the last object loaded the active
		SetActiveIndex(tempObject->GetIndex());

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		AddFileToHistory
//
// Description:		Adds the specified file to the recent history list.
//
// Input Arguments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::AddFileToHistory(wxString pathAndFileName)
{
	// Add the file to the manager
	recentFileManager->AddFileToHistory(pathAndFileName);
}

//==========================================================================
// Class:			MainFrame
// Function:		RemoveFileFromHistory
//
// Description:		Removes the specified file from the recent file list.
//					Looks up the object index based on the file name.
//
// Input Arguments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::RemoveFileFromHistory(wxString pathAndFileName)
{
	// Identify the entry that matches the specified path and file nam
	unsigned int i;
	for (i = 0; i < recentFileManager->GetCount(); i++)
	{
		// If we find the specified file, remove it from the list
		if (recentFileManager->GetHistoryFile(i).CompareTo(pathAndFileName) == 0)
		{
			recentFileManager->RemoveFileFromHistory(i);
			break;
		}
	}
}
