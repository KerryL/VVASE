/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  guiObject.h
// Created:  6/5/2008
// Author:  K. Loux
// Description:  Contains class declaration for GUI_OBJECT class.  This aids with GUI functionality
//				 by providing a common link between all the GUI controls and car objects.
// History:
//	7/27/2008	- Made ITEM_TYPE static and moved UNIT_TYPE to Convert, K. Loux.
//	8/18/2008	- Changed name from TREE_ITEM to GUI_OBJECT and broadened the functionality of the
//				  class to include a link between all of the controls, K. Loux.
//	1/24/2009	- Major application structure change - MAIN_FRAME uses GUI_OBJECT instead of
//				  GUI_CAR.  GUI_OBJECT changed to only contain either GUI_CAR or ITERATION
//				  objects.
//	5/19/2009	- Made abstract for base class for GUI_CAR and ITERATION, K. Loux.

#ifndef _GUI_OBJECT_CLASS_H_
#define _GUI_OBJECT_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/treebase.h>

// CarDesigner forward declarations
class GUI_CAR;
class ITERATION;
class MAIN_TREE;
class MAIN_NOTEBOOK;
class MAIN_FRAME;
class Debugger;
class RenderWindow;
class Primitive;

class GUI_OBJECT
{
public:
	// Constructor
	GUI_OBJECT(MAIN_FRAME &_MainFrame, const Debugger &_debugger,
		wxString _PathAndFileName = wxEmptyString);

	// Destructor
	virtual ~GUI_OBJECT();

	// Possible types of data contained in a GUI_OBJECT
	enum ITEM_TYPE
	{
		TYPE_CAR,
		TYPE_ITERATION,
		TYPE_OPTIMIZATION,

		TYPE_NONE				// Indicates an object doesn't exist
	};

	// Returns true of false depending on whether or not this item is selected in
	// the systems tree
	bool IsThisObjectSelected(wxTreeItemId Selected) const;
	bool IsThisObjectSelected(Primitive *PickedObject) const;

	// Initialization of these objects requires some calls to pure virtual functions, which
	// causes problems.  To avoid this, we make all of those calls here, and call this from
	// the derived object's constructors.
	void Initialize(void);

	// Checks to see if the object has been saved, asks for user confirmation, etc.
	bool Close(bool NotebookPageAlreadyClosed = false);

	// Gets/sets a flag to let us know if the object has changed since it was last saved
	inline bool GetModified(void) const { return ModifiedSinceLastSave; };
	void SetModified(void);

	// Returns the object that draws this on the screen
	inline wxWindow *GetNotebookTab(void) const { return notebookTab; };

	// Generates an image file of the render window contents
	bool WriteImageToFile(wxString PathAndFileName);

	// Private data accessors
	inline wxTreeItemId GetTreeItemId(void) const { return TreeID; };

	// Returns this object's type (mandatory overload)
	virtual ITEM_TYPE GetType(void) const = 0;

	// Return pointers to the main GUI components of this application
	inline MAIN_FRAME& GetMainFrame(void) const { return MainFrame; };

	// Gets/sets the index for this object
	inline int GetIndex(void) const { return Index; };
	inline void SetIndex(int _Index) { Index = _Index; };

	// Gets/sets the name for this object
	wxString GetName(void) const { return Name; };
	wxString GetCleanName(void) const;
	void SetName(wxString _Name);

	// Calls the write methods for the data associated with this object
	bool SaveToFile(bool SaveAsNewFileName = false);

	// Calls the update function for the data associated with this object
	// This method (usually) creates a job for a worker thread
	virtual void UpdateData(void) = 0;

	// Call the update function for the display associated with this object
	// This method does not create worker thread jobs
	virtual void UpdateDisplay(void) = 0;

	// Forces this object's tree item to be selected in the systems tree
	void SelectThisObjectInTree(void);

	// Returns the status of this object
	bool IsInitialized(void) { return ObjectIsInitialized; };

protected:
	// Debugger message printing utility
	const Debugger &debugger;

	// The objects name (for display purposes)
	wxString Name;

	// Performs the saving and loading to/from file
	virtual bool PerformLoadFromFile(void) = 0;
	virtual bool PerformSaveToFile(void) = 0;

	// Calls the read methods for the data associated with this object
	bool LoadFromFile(void);

	// Flag indicating whether or not the initialization routine is complete
	bool ObjectIsInitialized;

	// Strips down file names and paths to get the object name
	wxString GetNameFromFileName(void);

	// Gets the proper index for this object's icon in the systems tree
	virtual int GetIconHandle(void) const = 0;

	// The object's index
	// (associated with the list in the MAIN_FRAME object and the MAIN_NOTEBOOK's pages)
	int Index;

	// For referencing this object in the SystemsTree
	wxTreeItemId TreeID;

	// Pointers to the main GUI components for this application
	MAIN_TREE *SystemsTree;
	MAIN_FRAME &MainFrame;
	MAIN_NOTEBOOK *Notebook;

	// The object to be added to the notebook
	wxWindow *notebookTab;

	// Flag indicating whether or not this object has been modified since it was last saved
	bool ModifiedSinceLastSave;

	// The path and filename pointing to the location of this object
	// on the hard disk (or wxEmptyString if this hasn't been saved)
	wxString PathAndFileName;

	// Verifies that no other open object has the same filename
	bool VerifyUniqueness(void);
};

#endif// _GUI_OBJECT_CLASS_H_