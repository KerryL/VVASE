/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  main_frame_class.h
// Created:  3/7/2008
// Author:  K. Loux
// Description:  Contains the class declaration for the MAIN_FRAME class.  Uses wxWidgets
//				 for the GUI components.  Also contains the application class definition
//				 and functionality at the bottom of this file (minimal).
// History:
//	3/9/2008	- Changed the structure of the DEBUGGER class so one object can debug the
//				  entire application and we can print the outputs to OutputPane, K. Loux.
//  5/13/2008	- Added variables and functions to the class for managing GUI_CAR objects.
//				  Functionality includes maintaining a list of open objects to prevent
//				  memory leaks, K. Loux.
//	1/24/2009	- Major application structure change - MAIN_FRAME uses GUI_OBJECT instead of
//				  GUI_CAR.  GUI_OBJECT changed to only contain either GUI_CAR or ITERATION
//				  objects, K. Loux.
//	1/28/2009	- Changed structure of GUI components so context menu creation for all
//				  objects is handled by this class, K. Loux.
//	2/10/2009	- Added EDIT_PANEL to application, K. Loux.
//	3/24/2009	- Created one, application level KINEMATICS object, K. Loux.
//	6/7/2009	- Changed GetFilenameFromUser() to make use of wx functions for checking for file
//				  existance and selecting multiple files to open, K. Loux.
//	10/18/2010	- Added undo/redo stack object, K. Loux.

#ifndef _MAIN_FRAME_CLASS_H_
#define _MAIN_FRAME_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/aui/aui.h>

// VVASE headers
#include "vUtilities/convert_class.h"
#include "vUtilities/debug_class.h"
#include "vUtilities/managed_list_class.h"
#include "vSolver/physics/kinematics_class.h"
#include "gui/undo_redo_stack_class.h"
#include "gui/gui_object_class.h"	// Can't use a forward declaration here because
									//MANAGED_LIST can't compile without a definition

// wxWidgets forward declarations
class wxFileHistory;

// VVASE forward declarations
class CAR;
class KINEMATIC_OUTPUTS;
class MAIN_NOTEBOOK;
class MAIN_TREE;
class EDIT_PANEL;
class OUTPUT_PANEL;
class JOB_QUEUE;
class THREAD_JOB;

// The main frame class
class MAIN_FRAME : public wxFrame
{
public:
	// Constructor
	MAIN_FRAME();

	// Destructor
	~MAIN_FRAME();

	// Public class level constant - the name of the config file
	static const wxString PathToConfigFile;

	// Method for opening files
	bool LoadFile(wxString PathAndFileName);

	// For managing gui objects - these functions are the accessors
	// for the private LINKED_LIST
	int AddObjectToList(GUI_OBJECT *ObjectToAdd);
	void RemoveObjectFromList(int Index);
	inline int GetObjectCount(void) const { return OpenObjectList.GetCount(); };
	inline GUI_OBJECT *GetObjectByIndex(int Index) const { return OpenObjectList.GetObject(Index); };

	// Controls the object that is currently "active"
	void SetActiveIndex(int Index, bool SelectNotebookTab = true);
	inline int GetActiveIndex(void) const { return ActiveIndex; };

	// Returns a pointer to the converter object
	inline const CONVERT& GetConverter(void) const { return Converter; };

	// Accessor to deletion flag
	inline bool ObjectIsBeingDeleted(void) const { return BeingDeleted; };

	// Accessor to reference to other gui components
	inline MAIN_TREE *GetSystemsTree(void) const { return SystemsTree; };
	inline MAIN_NOTEBOOK *GetNotebook(void) const { return Notebook; };
	inline EDIT_PANEL *GetEditPanel(void) const { return EditPanel; };

	// Adds/removes files to/from the recent history list
	void AddFileToHistory(wxString PathAndFileName);
	void RemoveFileFromHistory(wxString PathAndFileName);

	// For displaying a menu that was crated by this form
	// NOTE:  When calculating the Position to display this context menu,
	// consider that the coordinates for the calling object might be different
	// from the coordinates for this object!
	void CreateContextMenu(int ObjectIndex, wxPoint Position, bool AllowClosing = true);

	// For getting a open/save file name from the user
	wxArrayString GetFileNameFromUser(wxString DialogTitle, wxString DefaultDirectory,
		wxString DefaultFileName, wxString Wildcard, long Style);

	// Updated the kinematics outputs for each car
	void UpdateAnalysis(void);

	// Updates the output panel with current car output information 
	// (should be called following an analysis update)
	void UpdateOutputPanel(void);

	// Add a job to the queue to be processed by a worker thread
	void AddJob(THREAD_JOB &NewJob);

	// Returns the current inputs for the kinematics analysis
	inline KINEMATICS::INPUTS GetInputs(void) const { return KinematicInputs; };
	inline bool GetUseRackTravel(void) const { return UseRackTravel; };
	inline void SetUseRackTravel(bool _UseRackTravel) { UseRackTravel = _UseRackTravel; };

	// Returns true if there are any outstanding jobs
	bool JobsPending(void) const;

	// Returns the number of worker threads available for processing
	inline unsigned int GetNumberOfThreads(void) const { return NumberOfThreads; };
	void SetNumberOfThreads(unsigned int _NumberOfThreads);

	// Returns a reference to the undo/redo stack object
	inline UNDO_REDO_STACK& GetUndoRedoStack(void) { return UndoRedo; };

	// Accessors to enable/disable the Undo/Redo menu items
	void EnableUndo(void) { MenuBar->FindItem(IdMenuEditUndo)->Enable(true); };
	void EnableRedo(void) { MenuBar->FindItem(IdMenuEditRedo)->Enable(true); };
	void DisableUndo(void) { MenuBar->FindItem(IdMenuEditUndo)->Enable(false); };
	void DisableRedo(void) { MenuBar->FindItem(IdMenuEditRedo)->Enable(false); };

private:
	// The form object manager - this controls docking, sub-frame locations, etc.
	wxAuiManager Manager;

	// Utility objects
	CONVERT Converter;
	DEBUGGER Debugger;

	// Object to manage recently viewed file list
	wxFileHistory *RecentFileManager;

	// This object manages the worker threads
	JOB_QUEUE *JobQueue;

	// The actual number of active threads
	unsigned short ActiveThreads;

	// The number of worker threads in our thread pool
	unsigned short NumberOfThreads;

	// The number of un-closed jobs
	unsigned int OpenJobCount;

	// For creating controls
	void CreateMenuBar(void);
	void CreateKinematicAnalysisToolbar(void);

	// Functions that do some of the frame initialization and control positioning
	void DoLayout(void);
	void SetProperties(void);
	void InitializeSolver(void);

	// Sets the notebook page with index Index to be the top page
	void SetNotebookPage(int Index);

	// For object management we need an index specifying the acitve car
	int ActiveIndex;

	// Flag set to true during the deletion process
	bool BeingDeleted;

	// Asks for user confirmation and checks to make sure the cars are saved
	bool CloseThisForm(void);

	// Read and writes the configuration information to the registry (called on load and exit)
	void ReadConfiguration(void);
	void WriteConfiguration(void);

	// Update the object specific menu (called any time the ActiveIndex changes)
	void UpdateActiveObjectMenu(void);

	// For the context menus we need to store the index of the object
	// for which the menu was created - it might not be the active car!
	int ObjectOfInterestIndex;

	// For dynamically changing the menu bar
	wxMenu *CreateCarMenu(void);
	wxMenu *CreateIterationMenu(void);

	// The input parameters for the kinematic analyses
	KINEMATICS::INPUTS KinematicInputs;
	bool UseRackTravel;// if false, we use steering wheel angle

	// Maximum number of recent files to store
	static const int MaxRecentFiles = 9;

	// The event IDs
	enum MENUEvent_ID
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
		IdMenuFileRecentLast = IdMenuFileRecentStart + MaxRecentFiles,
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
	MAIN_TREE *SystemsTree;
	MAIN_NOTEBOOK *Notebook;
	EDIT_PANEL *EditPanel;
	OUTPUT_PANEL *OutputPanel;
	wxTextCtrl *DebugPane;

	// The menu and status bars
	wxMenuBar *MenuBar;
	wxStatusBar *StatusBar;

	// Toolbars
	wxToolBar *KinematicToolbar;

	// The list of open objects
	MANAGED_LIST<GUI_OBJECT> OpenObjectList;

	// The undo/redo object
	UNDO_REDO_STACK UndoRedo;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _MAIN_FRAME_CLASS_H_