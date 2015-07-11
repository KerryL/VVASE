/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mainFrame.h
// Created:  3/7/2008
// Author:  K. Loux
// Description:  Contains the class declaration for the MainFrame class.  Uses wxWidgets
//				 for the GUI components.  Also contains the application class definition
//				 and functionality at the bottom of this file (minimal).
// History:
//	3/9/2008	- Changed the structure of the Debugger class so one object can debug the
//				  entire application and we can print the outputs to OutputPane, K. Loux.
//  5/13/2008	- Added variables and functions to the class for managing GuiCar objects.
//				  Functionality includes maintaining a list of open objects to prevent
//				  memory leaks, K. Loux.
//	1/24/2009	- Major application structure change - MainFrame uses GuiObject instead of
//				  GuiCar.  GuiObject changed to only contain either GuiCar or Iteration
//				  objects, K. Loux.
//	1/28/2009	- Changed structure of GUI components so context menu creation for all
//				  objects is handled by this class, K. Loux.
//	2/10/2009	- Added EditPanel to application, K. Loux.
//	3/24/2009	- Created one, application level Kinematics object, K. Loux.
//	6/7/2009	- Changed GetFilenameFromUser() to make use of wx functions for checking for file
//				  existence and selecting multiple files to open, K. Loux.
//	10/18/2010	- Added undo/redo stack object, K. Loux.

#ifndef _MAIN_FRAME_H_
#define _MAIN_FRAME_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/aui/aui.h>

// VVASE headers
#include "vUtilities/managedList.h"
#include "vSolver/physics/kinematics.h"
#include "gui/undoRedoStack.h"
#include "gui/guiObject.h"	// Can't use a forward declaration here because
							// ManagedList<GuiObject> can't compile without a definition

// wxWidgets forward declarations
class wxFileHistory;

// VVASE forward declarations
class KinematicOutputs;
class MainNotebook;
class MainTree;
class EditPanel;
class OutputPanel;
class JobQueue;
class ThreadJob;

// The main frame class
class MainFrame : public wxFrame
{
public:
	// Constructor
	MainFrame();

	// Destructor
	~MainFrame();

	// Public class level constant - the name of the config file
	static const wxString pathToConfigFile;

	// Method for opening files
	bool LoadFile(wxString pathAndFileName);

	// For managing gui objects - these functions are the accessors
	// for the private list
	int AddObjectToList(GuiObject *objectToAdd);
	void RemoveObjectFromList(int index);
	inline int GetObjectCount(void) const { return openObjectList.GetCount(); };
	inline GuiObject *GetObjectByIndex(int index) const { return openObjectList[index]; };

	// Controls the object that is currently "active"
	void SetActiveIndex(int index, bool selectNotebookTab = true);
	inline int GetActiveIndex(void) const { return activeIndex; };

	// Accessor to deletion flag
	inline bool ObjectIsBeingDeleted(void) const { return beingDeleted; };

	// Accessor to reference to other GUI components
	inline MainTree *GetSystemsTree(void) const { return systemsTree; };
	inline MainNotebook *GetNotebook(void) const { return notebook; };
	inline EditPanel *GetEditPanel(void) const { return editPanel; };

	// Adds/removes files to/from the recent history list
	void AddFileToHistory(wxString pathAndFileName);
	void RemoveFileFromHistory(wxString pathAndFileName);

	// For displaying a menu that was created by this form
	// NOTE:  When calculating the Position to display this context menu,
	// consider that the coordinates for the calling object might be different
	// from the coordinates for this object!
	void CreateContextMenu(int objectIndex, wxPoint position, bool allowClosing = true);

	// For getting a open/save file name from the user
	wxArrayString GetFileNameFromUser(wxString dialogTitle, wxString defaultDirectory,
		wxString defaultFileName, wxString wildcard, long style);

	// Updated the kinematics outputs for each car
	void UpdateAnalysis(void);

	// Updates the output panel with current car output information 
	// (should be called following an analysis update)
	void UpdateOutputPanel(void);

	// Add a job to the queue to be processed by a worker thread
	void AddJob(ThreadJob &newJob);
	JobQueue& GetJobQueue(void) { return *jobQueue; };

	// Returns the current inputs for the kinematics analysis
	inline Kinematics::Inputs GetInputs(void) const { return kinematicInputs; };
	inline bool GetUseRackTravel(void) const { return useRackTravel; };
	inline void SetUseRackTravel(bool _useRackTravel) { useRackTravel = _useRackTravel; };

	// Returns true if there are any outstanding jobs
	bool JobsPending(void) const;

	// Returns the number of worker threads available for processing
	inline unsigned int GetNumberOfThreads(void) const { return numberOfThreads; };
	void SetNumberOfThreads(unsigned int _numberOfThreads);

	// Returns a reference to the undo/redo stack object
	inline UndoRedoStack& GetUndoRedoStack(void) { return undoRedo; };

	// Accessors to enable/disable the Undo/Redo menu items
	void EnableUndo(void) { menuBar->FindItem(IdMenuEditUndo)->Enable(true); };
	void EnableRedo(void) { menuBar->FindItem(IdMenuEditRedo)->Enable(true); };
	void DisableUndo(void) { menuBar->FindItem(IdMenuEditUndo)->Enable(false); };
	void DisableRedo(void) { menuBar->FindItem(IdMenuEditRedo)->Enable(false); };
	
	const wxFont& GetOutputFont(void) { return outputFont; };
	const wxFont& GetPlotFont(void) { return plotFont; };
	
	void SetOutputFont(const wxFont &font);
	void SetPlotFont(const wxFont &font);

private:
	// The form object manager - this controls docking, sub-frame locations, etc.
	wxAuiManager manager;

	// Object to manage recently viewed file list
	wxFileHistory *recentFileManager;

	// This object manages the worker threads
	JobQueue *jobQueue;

	// The actual number of active threads
	unsigned short activeThreads;

	// The number of worker threads in our thread pool
	unsigned short numberOfThreads;

	// The number of un-closed jobs
	unsigned int openJobCount;

	// For creating controls
	void CreateMenuBar(void);
	void CreateKinematicAnalysisToolbar(void);

	// Functions that do some of the frame initialization and control positioning
	void DoLayout(void);
	void SetProperties(void);
	void InitializeSolver(void);

	// Sets the notebook page with index Index to be the top page
	void SetNotebookPage(int index);

	// For object management we need an index specifying the active car
	int activeIndex;

	// Flag set to true during the deletion process
	bool beingDeleted;

	// Asks for user confirmation and checks to make sure the cars are saved
	bool CloseThisForm(void);

	// Read and writes the configuration information to the registry (called on load and exit)
	void ReadConfiguration(void);
	void WriteConfiguration(void);

	// Update the object specific menu (called any time the ActiveIndex changes)
	void UpdateActiveObjectMenu(void);

	// For the context menus we need to store the index of the object
	// for which the menu was created - it might not be the active car!
	int objectOfInterestIndex;

	// For dynamically changing the menu bar
	wxMenu *CreateCarMenu(void);
	wxMenu *CreateIterationMenu(void);

	// The input parameters for the kinematic analyses
	Kinematics::Inputs kinematicInputs;
	bool useRackTravel;// if false, we use steering wheel angle

	// Maximum number of recent files to store
	static const int maxRecentFiles = 9;

	// The event IDs
	enum MenuEventId
	{
		// Menu bar
		IdMenuFileNewCar = 100 + wxID_HIGHEST,
		IdMenuFileNewIteration,
		IdMenuFileNewOptimization,
		IdMenuFileOpen,
		IdMenuFileClose,
		IdMenuFileCloseAll,
		IdMenuFileSave,
		IdMenuFileSaveAs,
		IdMenuFileSaveAll,
		IdMenuFileWriteImageFile,
		IdMenuFileExit,

		IdMenuFileRecentStart,// For recent file history
		IdMenuFileRecentLast = IdMenuFileRecentStart + maxRecentFiles,
		IdMenuFileOpenAllRecent,

		IdMenuEditUndo,
		IdMenuEditRedo,
		IdMenuEditCut,
		IdMenuEditCopy,
		IdMenuEditPaste,

		IdMenuCar,
		IdMenuCarAppearanceOptions,

		IdMenuIteration,
		IdMenuIterationShowAssociatedCars,
		IdMenuIterationAssociatedWithAllCars,
		IdMenuIterationExportDataToFile,
		IdMenuIterationXAxisPitch,
		IdMenuIterationXAxisRoll,
		IdMenuIterationXAxisHeave,
		IdMenuIterationXAxisRackTravel,

		IdMenuViewToolbarsKinematic,
		IdMenuViewClearOutput,

		IdMenuToolsDoE,
		IdMenuToolsDynamic,
		IdMenuToolsGA,
		IdMenuToolsOptions,

		IdMenuHelpManual,
		IdMenuHelpAbout,

		// Kinematic Analysis toolbar
		IdToolbarKinematicPitch,
		IdToolbarKinematicRoll,
		IdToolbarKinematicHeave,
		IdToolbarKinematicSteer
	};

	// Event handlers-----------------------------------------------------
	// Frame top level
	void WindowCloseEvent(wxCloseEvent &event);
	void OnSizeEvent(wxSizeEvent &event);

	// For the menu bar
	void FileNewCarEvent(wxCommandEvent &event);
	void FileNewIterationEvent(wxCommandEvent &event);
	void FileNewOptimizationEvent(wxCommandEvent &event);
	void FileOpenEvent(wxCommandEvent &event);
	void FileCloseEvent(wxCommandEvent &event);
	void FileCloseAllEvent(wxCommandEvent &event);
	void FileSaveEvent(wxCommandEvent &event);
	void FileSaveAsEvent(wxCommandEvent &event);
	void FileSaveAllEvent(wxCommandEvent &event);
	void FileOpenAllRecentEvent(wxCommandEvent &event);
	void FileWriteImageFileEvent(wxCommandEvent &event);
	void FileExitEvent(wxCommandEvent &event);

	void OtherMenuClickEvents(wxCommandEvent &event);

	void EditUndoEvent(wxCommandEvent &event);
	void EditRedoEvent(wxCommandEvent &event);
	void EditCutEvent(wxCommandEvent &event);
	void EditCopyEvent(wxCommandEvent &event);
	void EditPasteEvent(wxCommandEvent &event);

	void CarAppearanceOptionsEvent(wxCommandEvent &event);

	void IterationShowAssociatedCarsClickEvent(wxCommandEvent &event);
	void IterationAssociatedWithAllCarsClickEvent(wxCommandEvent &event);
	void IterationExportDataToFileClickEvent(wxCommandEvent &event);
	void IterationXAxisPitchClickEvent(wxCommandEvent &event);
	void IterationXAxisRollClickEvent(wxCommandEvent &event);
	void IterationXAxisHeaveClickEvent(wxCommandEvent &event);
	void IterationXAxisRackTravelClickEvent(wxCommandEvent &event);

	void ViewToolbarsKinematicEvent(wxCommandEvent &event);
	void ViewClearOutputEvent(wxCommandEvent &event);

	void ToolsDoEEvent(wxCommandEvent &event);
	void ToolsDynamicEvent(wxCommandEvent &event);
	void ToolsOptionsEvent(wxCommandEvent &event);

	void HelpManualEvent(wxCommandEvent &event);
	void HelpAboutEvent(wxCommandEvent &event);

	// Toolbars
	// Static Analysis
	void KinematicToolbarPitchChangeEvent(wxCommandEvent &event);
	void KinematicToolbarRollChangeEvent(wxCommandEvent &event);
	void KinematicToolbarHeaveChangeEvent(wxCommandEvent &event);
	void KinematicToolbarSteerChangeEvent(wxCommandEvent &event);

	// Threads
	void ThreadCompleteEvent(wxCommandEvent &event);
	void DebugMessageEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// The main areas of the frame
	MainTree *systemsTree;
	MainNotebook *notebook;
	EditPanel *editPanel;
	OutputPanel *outputPanel;
	wxTextCtrl *debugPane;

	// The menu bar
	wxMenuBar *menuBar;

	// Toolbars
	wxToolBar *kinematicToolbar;

	// The list of open objects
	ManagedList<GuiObject> openObjectList;

	// The undo/redo object
	UndoRedoStack undoRedo;
	
	// The font objects
	wxFont outputFont;
	wxFont plotFont;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _MAIN_FRAME_H_
