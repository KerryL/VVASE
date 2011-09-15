/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  gui_object_class.cpp
// Created:  6/5/2008
// Author:  K. Loux
// Description:  Contains class definition for GUI_OBJECT class.  This aids with GUI functionality for the
//				 by serving as a link between all of the GUI componenets and the CAR object.
// History:
//	1/24/2009	- Major application structure change - MAIN_FRAME uses GUI_OBJECT instead of
//				  GUI_CAR.  GUI_OBJECT changed to only contain either GUI_CAR or ITERATION
//				  objects (or others, in future), K. Loux.
//	5/19/2009	- Made abstract for base class for GUI_CAR and ITERATION, K. Loux.

// Windows headers (this is still portable to Linux systems...)
#include <sys/stat.h>

// wxWidgets headers
//#include <wx/treectrl.h>
#include <wx/docview.h>

// VVASE headers
#include "gui/gui_object_class.h"
#include "gui/components/main_notebook_class.h"
#include "gui/components/main_tree_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/gui_car_class.h"
#include "gui/iteration_class.h"
#include "vRenderer/render_window_class.h"

//==========================================================================
// Class:			GUI_OBJECT
// Function:		GUI_OBJECT
//
// Description:		Constructor for the GUI_OBJECT class.  If specified,
//					it adds the object to the SystemsTree and it creates a
//					link between this object and the data that it represents,
//					depending on the object's type.
//
// Input Argurments:
//		_MainFrame			= MAIN_FRAME& pointing to the main frame for
//							  this application
//		_Debugger			= const DEBUGGER& reference to the debug message printing
//							  utility
//		_PathAndFileName	= wxString specifying the location to load this
//							  object from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GUI_OBJECT::GUI_OBJECT(MAIN_FRAME &_MainFrame, const DEBUGGER &_Debugger,
					   wxString _PathAndFileName) : Debugger(_Debugger), MainFrame(_MainFrame)
{
	// Make sure we know that loading is not complete
	ObjectIsInitialized = false;

	// Assign the arguments to the class members
	PathAndFileName	= _PathAndFileName;

	// Get the pointers to the other gui components from MainFrame
	Notebook		= MainFrame.GetNotebook();
	SystemsTree		= MainFrame.GetSystemsTree();

	// We initialize this to true - if we load an object from file we set it to false later
	ModifiedSinceLastSave = true;

	// Check to see if we are loading an object from file
	if (!_PathAndFileName.IsEmpty())
	{
		// Set the modified flag to false (we're loading a car that's already saved)
		ModifiedSinceLastSave = false;

		// Set the path to our class level variable
		PathAndFileName = _PathAndFileName;
	}
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		~GUI_OBJECT
//
// Description:		Destructor for the GUI_OBJECT class.  Removes the object
//					from the SystemsTree.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GUI_OBJECT::~GUI_OBJECT()
{
	// Remove the entry from the SystemsTree - IMPORTANT - the order that the GUI_OBJECTs
	// are deleted becomes important here.  You can't delete the parent of a group until
	// all of its children are deleted.
	if (TreeID.IsOk())
		SystemsTree->Delete(TreeID);
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		Initialize
//
// Description:		Contains calls to pure virtual functions and other initialization
//					routines.  To be called from DERIVED class constructors.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GUI_OBJECT::Initialize(void)
{
	// Declare the icon variables
	int NormalIcon = -1;
	int SelectedIcon = -1;

	// No icons unless we're in windows
#ifdef __WXMSW__
	// Get the icon from the systems tree
	NormalIcon = GetIconHandle();
#endif

	// Add a page to the notebook for this car
	Notebook->AddPage(Renderer, Name);

	// Add the entry to the SystemsTree
	TreeID = SystemsTree->AppendItem(SystemsTree->GetRootItem(), Name, NormalIcon, SelectedIcon);

	// If our path variable isn't empty, try to open the file
	if (!PathAndFileName.IsEmpty())
	{
		// Make sure that when we load the file, we don't have any errors
		if (!LoadFromFile())
		{
			// Prevent this object from being loaded
			Notebook->DeletePage(Index);
			return;
		}
	}

	// Set the flag indicating the initialization process is over
	ObjectIsInitialized = true;

	// Update the anaylsis for the new object
	MainFrame.UpdateAnalysis();

	return;
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		SetName
//
// Description:		Sets the name of the object.  Depending on the object's
//					type, other name-setting functions may be called as a
//					result of the change.
//
// Input Argurments:
//		_Name	= wxString specifying the object's new name
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GUI_OBJECT::SetName(wxString _Name)
{
	// Remove the asterisks from both names, if they have one
	/*wxString Asterisk('*');
	wxString NewNameSansAsterisk, OldNameSansAsterisk;
	if (Name.substr(Name.length() - 1, 1).compare(Asterisk) == 0)
		OldNameSansAsterisk = Name.substr(0, Name.length() - 1);
	else
		OldNameSansAsterisk = Name;
	if (_Name.substr(_Name.length() - 1, 1).compare(Asterisk) == 0)
		NewNameSansAsterisk = _Name.substr(0, _Name.length() - 1);
	else
		NewNameSansAsterisk = _Name;

	// Update the name in the output panel
	MainFrame.GetOutputPanel()->UpdateName(OldNameSansAsterisk, NewNameSansAsterisk);*/

	// Set the name for this object
	Name = _Name;

	// Update the names in the SystemsTree and the Notebook
	SystemsTree->SetItemText(TreeID, Name);
	Notebook->SetPageText(Index, Name);

	// Update the output panel to make sure names are up-to-date
	MainFrame.UpdateOutputPanel();

	return;
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		GetCleanName
//
// Description:		Retreives the name of the car.  Cleans up the name
//					(removes asterisks indicating the file needs to be
//					saved) if necessary.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString representing the cleaned-up name of the object
//
//==========================================================================
wxString GUI_OBJECT::GetCleanName(void) const
{
	// If the file has been modified, and it contains an asterisk, remove the
	// asterisk and return the name
	if (ModifiedSinceLastSave)
	{
		if (Name.substr(Name.length() - 1, 1) == '*')
			return Name.substr(0, Name.length() - 1);
	}

	// If we haven't returned yet, return the whole name
	return Name;
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		SetModified
//
// Description:		Sets a flag indicating that the object has been modified.
//					If the object has been saved to file, it also changes the
//					displayed name to include a "*".
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GUI_OBJECT::SetModified(void)
{
	// Set the flag
	ModifiedSinceLastSave = true;

	// If we haven't been saved, add the asterisk
	if (!PathAndFileName.IsEmpty())
	{
		// Make sure we don't already have an asterisk
		wxString Asterisk('*');
		if (Name.substr(Name.length() - 1,1).compare(Asterisk) != 0)
			SetName(Name + Asterisk);
	}
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		IsThisObjectSelected
//
// Description:		Compares the tree item ID for this object with the ID
//					passed to this function.
//
// Input Argurments:
//		Selected	= wxTreeItemId to compare with this object's TreeID
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the passed item matches this object
//
//==========================================================================
bool GUI_OBJECT::IsThisObjectSelected(wxTreeItemId Selected) const
{
	// Make sure we are being passes a valid tree item id
	if (!Selected.IsOk())
		return false;

	// Compare the argument with this object's tree item ID
	if (Selected == TreeID)
		return true;

	// Also compare this object's children
	int i;
	switch (GetType())
	{
	case TYPE_CAR:
		for (i = 0; i < GUI_CAR::NumberOfSubsystems; i++)
		{
			if (Selected == static_cast<const GUI_CAR*>(this)->Subsystems[i])
				return true;
		}

		break;

	// Unused types
	case GUI_OBJECT::TYPE_ITERATION:
	case GUI_OBJECT::TYPE_OPTIMIZATION:
	case GUI_OBJECT::TYPE_NONE:
		break;

	// Fail on unknown types to avoid forgetting any types
	default:
		assert(0);
		break;
	}

	return false;
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		Close
//
// Description:		If the object has been modified with being saved, it asks
//					the user if they want to save the object (with the option
//					of cancelling), and if it has been saved it asks for
//					confirmation to close.  If it was desired to close the
//					object, the appropriate closing actions are executed.
//
// Input Argurments:
//		NotebookPageAlreadyClosed	= bool to prevent memory corruption,
//									  if this function is called as a
//									  result of a notebook page closing,
//									  we shouldn't try to close the page
//									  again.
//
// Output Arguments:
//		None
//
// Return Value:
//		CLOSE_RESPONSE
//
//==========================================================================
bool GUI_OBJECT::Close(bool NotebookPageAlreadyClosed)
{
	// Ensure that there are no pending jobs - if there are, return false
	if (MainFrame.JobsPending())
		return false;

	// Check to see if this object has been modified without being saved
	if (ModifiedSinceLastSave)
	{
		// Display the message box
		int Response = wxMessageBox(Name + _T(" has not been saved.  Would you like to save")
			+ _T(" before closing?"), MainFrame.GetName(), wxYES_NO | wxCANCEL, &MainFrame);

		if (Response == wxYES)
		{
			// Call the object's save method
			if (!SaveToFile())
				// If SaveToFile returns false, it is because the user has clicked cancel.
				// Treat this case the same as if the user clicked cancel when asked the first
				// time.
				return false;
		}
		else if (Response == wxCANCEL)
			// Return false to signify that the car was not closed
			return false;
	}

	// If we have not yet returned, then continue with the close...

	// Close the corrsponding notebook page (only if this function is NOT being called
	// due to a notebook page closing...) (MUST be done BEFORE object is removed from list)
	if (!NotebookPageAlreadyClosed)
		Notebook->DeletePage(Index);

	// Remove any undo/redo actions associated with this object
	MainFrame.GetUndoRedoStack().RemoveGuiObjectFromStack(Index);

	// Delete this GUI_OBJECT
	MainFrame.RemoveObjectFromList(Index);

	return true;
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		LoadFromFile
//
// Description:		Loads this object from a location on the hard disk.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success
//
//==========================================================================
bool GUI_OBJECT::LoadFromFile(void)
{
	// Perform the load and check for errors
	if (!PerformLoadFromFile())
	{
		Debugger.Print(_T("ERROR:  Could not read from file '") + PathAndFileName + _T("'!"));

		// Remove this file from the recent history list
		MainFrame.RemoveFileFromHistory(PathAndFileName);

		return false;
	}

	// Make sure the desired file isn't already open - if it is, return false
	if (!VerifyUniqueness())
	{
		Debugger.Print(_T("Object at '") + PathAndFileName +
			_T("' already open!"), DEBUGGER::PriorityMedium);

		return false;
	}

	// Call SetName to ensure all tabs, tree items, etc. are re-named
	SetName(GetNameFromFileName());

	// Print a message to let the user know we successfully loaded the file
	Debugger.Print(_T("File loaded from '") + PathAndFileName + _T("'!"), DEBUGGER::PriorityMedium);

	// Add file to the recent history list
	MainFrame.AddFileToHistory(PathAndFileName);

	return true;
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		SaveToFile
//
// Description:		Displays all of the necessary dialogs and calls the
//					appropriate save function, depending on this object's
//					type.
//
// Input Argurments:
//		SaveAsNewFileName	= bool specifying whether or not we want to
//							  save with a new file name
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success
//
//==========================================================================
bool GUI_OBJECT::SaveToFile(bool SaveAsNewFileName)
{
	// Get the file extension
	wxString FileTypeExtension;
	if (GetType() == TYPE_CAR)
		FileTypeExtension.assign(_T("Car files (*.car)|*.car"));
	else if (GetType() == TYPE_ITERATION)
		FileTypeExtension.assign(_T("Iteration files (*.iteration)|*.iteration"));
	else if (GetType() == TYPE_OPTIMIZATION)
		FileTypeExtension.assign(_T("Optimization files (*.ga)|*.ga"));
	else
	{
		// Fail on unkown type to avoid forgetting any types
		assert(0);
		return false;
	}

	// Check to see if we're saving with our existing file name (if we have one) or
	// if we're saving with a new name/path
	if (PathAndFileName.IsEmpty() || SaveAsNewFileName)
	{
		wxString DefaultFileName = GetCleanName();
/*#ifdef __WXGTK__
		if (GetType() == TYPE_CAR)
			DefaultFileName.append(".car");
		else if (GetType() == TYPE_ITERATION)
			DefaultFileName.append(".iteration");
#endif*/// FIXME:  Do we still need this???
		wxArrayString TempPathAndFileName = MainFrame.GetFileNameFromUser(_T("Save As"),
			wxEmptyString, DefaultFileName, FileTypeExtension, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

		// Make sure the user didn't cancel
		if (TempPathAndFileName.IsEmpty())
			return false;

		// Assign the temporary variables to the class member
		PathAndFileName = TempPathAndFileName.Item(0);

		// Make sure the file name contains the extension - this is neccesary due to the following scenario:
		// When doing "Save As...", if the default file name is modified (i.e. Rev 1 is changed to Rev 2),
		// the extension is dropped.  This does not occur if the entire file name is changed.
		wxString EndOfFileName;
		if (GetType() == TYPE_CAR)
			EndOfFileName.assign(_T(".car"));
		else if (GetType() == TYPE_ITERATION)
			EndOfFileName.assign(_T(".iteration"));
		else if (GetType() == TYPE_OPTIMIZATION)
			EndOfFileName.assign(_T(".ga"));
		else
			// Make sure we're not forgetting any types
			assert(0);

		if (EndOfFileName.CmpNoCase(PathAndFileName.substr(PathAndFileName.length() - EndOfFileName.length(),
			PathAndFileName.length())) != 0)
			PathAndFileName.append(EndOfFileName);

		// Set the name for the object
		SetName(GetNameFromFileName());

		// Add this file to the list of recent files in the MainFrame
		MainFrame.AddFileToHistory(PathAndFileName);
	}
	else if (ModifiedSinceLastSave)
		// If this is true (and we haven't been saved yet) the display
		// name has an asterisk which we need to remove
		SetName(Name.substr(0, Name.length() - 1));

	// Perform the save and check for errors
	if (!PerformSaveToFile())
	{
		Debugger.Print(_T("ERROR:  Could not save file to '") + PathAndFileName + _T("'!"));

		return false;
	}

	// Set the flag to let us know we've been saved recently
	ModifiedSinceLastSave = false;

	// Print a message to let the user know we successfully saved the file
	Debugger.Print(_T("File saved to '") + PathAndFileName + _T("'!"), DEBUGGER::PriorityMedium);

	// Remove this object from the undo/redo stacks
	MainFrame.GetUndoRedoStack().RemoveGuiObjectFromStack(Index);

	// Return true if we get to the end (true = car was saved)
	return true;
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		Render
//
// Description:		Calls the appropriate Update() method, depending on the
//					type of this object.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GUI_OBJECT::Render(void)
{
	// Make sure our renderer pointer is valid
	if (Renderer)
		Renderer->Render();

	return;
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		IsThisObjectSelected
//
// Description:		Assertains whether or not the selected item in the
//					systems tree is an object associated with this object or
//					not.
//
// Input Argurments:
//		PickedObject	= PRIMITIVE* pointing to the selected actor on the active
//						  notebook page
//
// Output Arguments:
//		None
//
// Return Value:
//		bool = true if the selected item is associated with this car, or
//			   false otherwise
//
//==========================================================================
bool GUI_OBJECT::IsThisObjectSelected(PRIMITIVE *PickedObject) const
{
	return Renderer->IsThisRendererSelected(PickedObject);
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		WriteImageToFile
//
// Description:		Displays a dialog asking the user to specify a file name
//					and file type, and generates an image file with the
//					contents of the render window.
//
// Input Argurments:
//		PathAndFileName	= wxString specifying where the file should be saved
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool GUI_OBJECT::WriteImageToFile(wxString PathAndFileName)
{
	// Ask the renderer to write the image to file
	if (Renderer)
		return Renderer->WriteImageToFile(PathAndFileName);
	else
		return false;
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		SelectThisObjectInTree
//
// Description:		Selects the root item for this object in the systems tree.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GUI_OBJECT::SelectThisObjectInTree(void)
{
	// Make sure the tree ID is valid
	if (TreeID.IsOk())
		// Select this object's tree item
		SystemsTree->SelectItem(TreeID);

	return;
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		GetNameFromFileName
//
// Description:		Strips down the path and file name (must be set before
//					calling this function) to get just the name.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the stripped-down name
//
//==========================================================================
wxString GUI_OBJECT::GetNameFromFileName(void)
{
	wxChar PathDelimiter;
#ifdef __WXGTK__
	PathDelimiter = '/';
#else
	PathDelimiter = '\\';
#endif

	// Disect the PathAndFileName to get just the name (also truncate the extension)
	int StartOfFileName = PathAndFileName.Last(PathDelimiter) + 1;
	int EndOfFileName = PathAndFileName.Last('.');
	Name = PathAndFileName.Mid(StartOfFileName, EndOfFileName - StartOfFileName);

	return Name;
}

//==========================================================================
// Class:			GUI_OBJECT
// Function:		VerifyUniqueness
//
// Description:		Checks to make sure no other open object has the same
//					file name as that passed to this object.  Sets focus to
//					the pre-opened object if there is one.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the stripped-down name
//
//==========================================================================
bool GUI_OBJECT::VerifyUniqueness(void)
{
	// Check the local path and filename against all those beinging managed by
	// the main frame
	int i;
	for (i = 0; i < MainFrame.GetObjectCount(); i++)
	{
		if (MainFrame.GetObjectByIndex(i)->PathAndFileName.CompareTo(
			PathAndFileName) == 0 && i != Index)
		{
			// Set focus to the existing object and return false
			MainFrame.SetActiveIndex(i);// FIXME:  This doesn't work
			return false;
		}
	}

	// No duplicates found
	return true;
}