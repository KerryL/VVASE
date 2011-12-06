/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  guiObject.cpp
// Created:  6/5/2008
// Author:  K. Loux
// Description:  Contains class definition for GuiObject class.  This aids with GUI functionality for the
//				 by serving as a link between all of the GUI components and the CAR object.
// History:
//	1/24/2009	- Major application structure change - MainFrame uses GuiObject instead of
//				  GuiCar.  GuiObject changed to only contain either GuiCar or Iteration
//				  objects (or others, in future), K. Loux.
//	5/19/2009	- Made abstract for base class for GuiCar and Iteration, K. Loux.

// Windows headers (this is still portable to Linux systems...)
#include <sys/stat.h>

// wxWidgets headers
//#include <wx/treectrl.h>
#include <wx/docview.h>

// VVASE headers
#include "gui/guiObject.h"
#include "gui/components/mainNotebook.h"
#include "gui/components/mainTree.h"
#include "gui/components/mainFrame.h"
#include "gui/guiCar.h"
#include "gui/iteration.h"
#include "vRenderer/renderWindow.h"
#include "gui/plotPanel.h"
#include "vUtilities/debugger.h"

//==========================================================================
// Class:			GuiObject
// Function:		GuiObject
//
// Description:		Constructor for the GuiObject class.  If specified,
//					it adds the object to the SystemsTree and it creates a
//					link between this object and the data that it represents,
//					depending on the object's type.
//
// Input Arguments:
//		_mainFrame			= MainFrame& pointing to the main frame for
//							  this application
//		_pathAndFileName	= wxString specifying the location to load this
//							  object from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GuiObject::GuiObject(MainFrame &_mainFrame,
					   wxString _pathAndFileName) : mainFrame(_mainFrame)
{
	// Make sure we know that loading is not complete
	objectIsInitialized = false;

	// Assign the arguments to the class members
	pathAndFileName	= _pathAndFileName;

	// Get the pointers to the other gui components from mainFrame
	notebook		= mainFrame.GetNotebook();
	systemsTree		= mainFrame.GetSystemsTree();

	// We initialize this to true - if we load an object from file we set it to false later
	modifiedSinceLastSave = true;

	// Check to see if we are loading an object from file
	if (!_pathAndFileName.IsEmpty())
	{
		// Set the modified flag to false (we're loading a car that's already saved)
		modifiedSinceLastSave = false;

		// Set the path to our class level variable
		pathAndFileName = _pathAndFileName;
	}
}

//==========================================================================
// Class:			GuiObject
// Function:		~GuiObject
//
// Description:		Destructor for the GuiObject class.  Removes the object
//					from the SystemsTree.
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
GuiObject::~GuiObject()
{
	// Remove the entry from the SystemsTree - IMPORTANT - the order that the GuiObjects
	// are deleted becomes important here.  You can't delete the parent of a group until
	// all of its children are deleted.
	if (treeID.IsOk())
		systemsTree->Delete(treeID);
}

//==========================================================================
// Class:			GuiObject
// Function:		Initialize
//
// Description:		Contains calls to pure virtual functions and other initialization
//					routines.  To be called from DERIVED class constructors.
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
void GuiObject::Initialize(void)
{
	// Declare the icon variables
	int normalIcon = -1;
	int selectedIcon = -1;

	// Get the icon from the systems tree
	normalIcon = GetIconHandle();

	// Add a page to the notebook for this car
	notebook->AddPage(notebookTab, name);

	// Add the entry to the SystemsTree
	treeID = systemsTree->AppendItem(systemsTree->GetRootItem(), name, normalIcon, selectedIcon);

	// If our path variable isn't empty, try to open the file
	if (!pathAndFileName.IsEmpty())
	{
		// Make sure that when we load the file, we don't have any errors
		if (!LoadFromFile())
		{
			// Prevent this object from being loaded
			notebook->DeletePage(index);
			return;
		}
	}

	// Set the flag indicating the initialization process is over
	objectIsInitialized = true;

	// Update the analysis for the new object
	mainFrame.UpdateAnalysis();
}

//==========================================================================
// Class:			GuiObject
// Function:		SetName
//
// Description:		Sets the name of the object.  Depending on the object's
//					type, other name-setting functions may be called as a
//					result of the change.
//
// Input Arguments:
//		_name	= wxString specifying the object's new name
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiObject::SetName(wxString _name)
{
	// Set the name for this object
	name = _name;

	// Update the names in the SystemsTree and the Notebook
	systemsTree->SetItemText(treeID, name);
	notebook->SetPageText(index, name);

	// Update the output panel to make sure names are up-to-date
	mainFrame.UpdateOutputPanel();
}

//==========================================================================
// Class:			GuiObject
// Function:		GetCleanName
//
// Description:		Retrieves the name of the car.  Cleans up the name
//					(removes asterisks indicating the file needs to be
//					saved) if necessary.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString representing the cleaned-up name of the object
//
//==========================================================================
wxString GuiObject::GetCleanName(void) const
{
	// If the file has been modified, and it contains an asterisk, remove the
	// asterisk and return the name
	if (modifiedSinceLastSave)
	{
		if (name.substr(name.length() - 1, 1) == '*')
			return name.substr(0, name.length() - 1);
	}

	// If we haven't returned yet, return the whole name
	return name;
}

//==========================================================================
// Class:			GuiObject
// Function:		SetModified
//
// Description:		Sets a flag indicating that the object has been modified.
//					If the object has been saved to file, it also changes the
//					displayed name to include a "*".
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
void GuiObject::SetModified(void)
{
	// Set the flag
	modifiedSinceLastSave = true;

	// If we haven't been saved, add the asterisk
	if (!pathAndFileName.IsEmpty())
	{
		// Make sure we don't already have an asterisk
		wxString asterisk('*');
		if (name.substr(name.length() - 1,1).compare(asterisk) != 0)
			SetName(name + asterisk);
	}
}

//==========================================================================
// Class:			GuiObject
// Function:		IsThisObjectSelected
//
// Description:		Compares the tree item ID for this object with the ID
//					passed to this function.
//
// Input Arguments:
//		selected	= wxTreeItemId to compare with this object's TreeID
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the passed item matches this object
//
//==========================================================================
bool GuiObject::IsThisObjectSelected(wxTreeItemId selected) const
{
	// Make sure we are being passes a valid tree item id
	if (!selected.IsOk())
		return false;

	// Compare the argument with this object's tree item ID
	if (selected == treeID)
		return true;

	// Also compare this object's children
	int i;
	switch (GetType())
	{
	case TypeCar:
		for (i = 0; i < GuiCar::NumberOfSubsystems; i++)
		{
			if (selected == static_cast<const GuiCar*>(this)->subsystems[i])
				return true;
		}

		break;

	// Unused types
	case GuiObject::TypeIteration:
	case GuiObject::TypeOptimization:
	case GuiObject::TypeNone:
		break;

	// Fail on unknown types to avoid forgetting any types
	default:
		assert(0);
		break;
	}

	return false;
}

//==========================================================================
// Class:			GuiObject
// Function:		Close
//
// Description:		If the object has been modified with being saved, it asks
//					the user if they want to save the object (with the option
//					of canceling), and if it has been saved it asks for
//					confirmation to close.  If it was desired to close the
//					object, the appropriate closing actions are executed.
//
// Input Arguments:
//		notebookPageAlreadyClosed	= bool to prevent memory corruption,
//									  if this function is called as a
//									  result of a notebook page closing,
//									  we shouldn't try to close the page
//									  again.
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool GuiObject::Close(bool notebookPageAlreadyClosed)
{
	// Ensure that there are no pending jobs - if there are, return false
	// FIXME:  Why is this? - Removed 11/16/2011, minimal testing performed
	/*if (mainFrame.JobsPending())
		return false;*/

	// Check to see if this object has been modified without being saved
	if (modifiedSinceLastSave)
	{
		// Display the message box
		int response = wxMessageBox(name + _T(" has not been saved.  Would you like to save")
			+ _T(" before closing?"), mainFrame.GetName(), wxYES_NO | wxCANCEL, &mainFrame);

		if (response == wxYES)
		{
			// Call the object's save method
			if (!SaveToFile())
				// If SaveToFile returns false, it is because the user has clicked cancel.
				// Treat this case the same as if the user clicked cancel when asked the first
				// time.
				return false;
		}
		else if (response == wxCANCEL)
			// Return false to signify that the car was not closed
			return false;
	}

	// If we have not yet returned, then continue with the close...

	// Close the corrsponding notebook page (only if this function is NOT being called
	// due to a notebook page closing...) (MUST be done BEFORE object is removed from list)
	if (!notebookPageAlreadyClosed)
		notebook->DeletePage(index);

	// Remove any undo/redo actions associated with this object
	mainFrame.GetUndoRedoStack().RemoveGuiObjectFromStack(index);

	// Delete this GuiObject
	mainFrame.RemoveObjectFromList(index);

	return true;
}

//==========================================================================
// Class:			GuiObject
// Function:		LoadFromFile
//
// Description:		Loads this object from a location on the hard disk.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success
//
//==========================================================================
bool GuiObject::LoadFromFile(void)
{
	// Perform the load and check for errors
	if (!PerformLoadFromFile())
	{
		Debugger::GetInstance().Print(_T("ERROR:  Could not read from file '") + pathAndFileName + _T("'!"));

		// Remove this file from the recent history list
		mainFrame.RemoveFileFromHistory(pathAndFileName);

		return false;
	}

	// Make sure the desired file isn't already open - if it is, return false
	if (!VerifyUniqueness())
	{
		Debugger::GetInstance().Print(_T("Object at '") + pathAndFileName +
			_T("' already open!"), Debugger::PriorityMedium);

		return false;
	}

	// Call SetName to ensure all tabs, tree items, etc. are re-named
	SetName(GetNameFromFileName());

	// Print a message to let the user know we successfully loaded the file
	Debugger::GetInstance().Print(_T("File loaded from '") + pathAndFileName + _T("'!"), Debugger::PriorityMedium);

	// Add file to the recent history list
	mainFrame.AddFileToHistory(pathAndFileName);

	return true;
}

//==========================================================================
// Class:			GuiObject
// Function:		SaveToFile
//
// Description:		Displays all of the necessary dialogs and calls the
//					appropriate save function, depending on this object's
//					type.
//
// Input Arguments:
//		saveAsNewFileName	= bool specifying whether or not we want to
//							  save with a new file name
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success
//
//==========================================================================
bool GuiObject::SaveToFile(bool saveAsNewFileName)
{
	// Get the file extension
	wxString fileTypeExtension;
	if (GetType() == TypeCar)
		fileTypeExtension.assign(_T("Car files (*.car)|*.car"));
	else if (GetType() == TypeIteration)
		fileTypeExtension.assign(_T("Iteration files (*.iteration)|*.iteration"));
	else if (GetType() == TypeOptimization)
		fileTypeExtension.assign(_T("Optimization files (*.ga)|*.ga"));
	else
	{
		// Fail on unknown type to avoid forgetting any types
		assert(0);
		return false;
	}

	// Check to see if we're saving with our existing file name (if we have one) or
	// if we're saving with a new name/path
	if (pathAndFileName.IsEmpty() || saveAsNewFileName)
	{
		wxString defaultFileName = GetCleanName();
		wxArrayString tempPathAndFileName = mainFrame.GetFileNameFromUser(_T("Save As"),
			wxEmptyString, defaultFileName, fileTypeExtension, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

		// Make sure the user didn't cancel
		if (tempPathAndFileName.IsEmpty())
			return false;

		// Assign the temporary variables to the class member
		pathAndFileName = tempPathAndFileName.Item(0);

		// Make sure the file name contains the extension - this is necessary due to the following scenario:
		// When doing "Save As...", if the default file name is modified (i.e. Rev 1 is changed to Rev 2),
		// the extension is dropped.  This does not occur if the entire file name is changed.
		wxString endOfFileName;
		if (GetType() == TypeCar)
			endOfFileName.assign(_T(".car"));
		else if (GetType() == TypeIteration)
			endOfFileName.assign(_T(".iteration"));
		else if (GetType() == TypeOptimization)
			endOfFileName.assign(_T(".ga"));
		else
			// Make sure we're not forgetting any types
			assert(0);

		if (endOfFileName.CmpNoCase(pathAndFileName.substr(pathAndFileName.length() - endOfFileName.length(),
			pathAndFileName.length())) != 0)
			pathAndFileName.append(endOfFileName);

		// Set the name for the object
		SetName(GetNameFromFileName());

		// Add this file to the list of recent files in the mainFrame
		mainFrame.AddFileToHistory(pathAndFileName);
	}
	else if (modifiedSinceLastSave)
		// If this is true (and we haven't been saved yet) the display
		// name has an asterisk which we need to remove
		SetName(name.substr(0, name.length() - 1));

	// Perform the save and check for errors
	if (!PerformSaveToFile())
	{
		Debugger::GetInstance().Print(_T("ERROR:  Could not save file to '") + pathAndFileName + _T("'!"));

		return false;
	}

	// Set the flag to let us know we've been saved recently
	modifiedSinceLastSave = false;

	// Print a message to let the user know we successfully saved the file
	Debugger::GetInstance().Print(_T("File saved to '") + pathAndFileName + _T("'!"), Debugger::PriorityMedium);

	// Remove this object from the undo/redo stacks
	mainFrame.GetUndoRedoStack().RemoveGuiObjectFromStack(index);

	// Return true if we get to the end (true = car was saved)
	return true;
}

//==========================================================================
// Class:			GuiObject
// Function:		WriteImageToFile
//
// Description:		Displays a dialog asking the user to specify a file name
//					and file type, and generates an image file with the
//					contents of the render window.
//
// Input Arguments:
//		pathAndFileName	= wxString specifying where the file should be saved
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool GuiObject::WriteImageToFile(wxString pathAndFileName)
{
	// Ask the renderer to write the image to file (if there is an image)
	switch (GetType())
	{
		// Types that have a renderer
	case TypeCar:
			return static_cast<RenderWindow*>(notebookTab)->WriteImageToFile(pathAndFileName);
		break;
		
	case TypeIteration:
			return static_cast<PlotPanel*>(notebookTab)->WriteImageToFile(pathAndFileName);
		break;
		
		// Everything else
	default:
		break;
	}
	
	return false;
}

//==========================================================================
// Class:			GuiObject
// Function:		SelectThisObjectInTree
//
// Description:		Selects the root item for this object in the systems tree.
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
void GuiObject::SelectThisObjectInTree(void)
{
	// Make sure the tree ID is valid
	if (treeID.IsOk())
		// Select this object's tree item
		systemsTree->SelectItem(treeID);
}

//==========================================================================
// Class:			GuiObject
// Function:		GetNameFromFileName
//
// Description:		Strips down the path and file name (must be set before
//					calling this function) to get just the name.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the stripped-down name
//
//==========================================================================
wxString GuiObject::GetNameFromFileName(void)
{
	wxChar pathDelimiter;
#ifdef __WXGTK__
	pathDelimiter = '/';
#else
	pathDelimiter = '\\';
#endif

	// Dissect the PathAndFileName to get just the name (also truncate the extension)
	int startOfFileName = pathAndFileName.Last(pathDelimiter) + 1;
	int endOfFileName = pathAndFileName.Last('.');
	name = pathAndFileName.Mid(startOfFileName, endOfFileName - startOfFileName);

	return name;
}

//==========================================================================
// Class:			GuiObject
// Function:		VerifyUniqueness
//
// Description:		Checks to make sure no other open object has the same
//					file name as that passed to this object.  Sets focus to
//					the pre-opened object if there is one.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the stripped-down name
//
//==========================================================================
bool GuiObject::VerifyUniqueness(void)
{
	// Check the local path and filename against all those being managed by
	// the main frame
	int i;
	for (i = 0; i < mainFrame.GetObjectCount(); i++)
	{
		if (mainFrame.GetObjectByIndex(i)->pathAndFileName.CompareTo(
			pathAndFileName) == 0 && i != index)
		{
			// Set focus to the existing object and return false
			mainFrame.SetActiveIndex(i);// FIXME:  This doesn't work
			return false;
		}
	}

	// No duplicates found
	return true;
}