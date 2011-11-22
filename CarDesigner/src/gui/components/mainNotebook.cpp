/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mainNotebook.cpp
// Created:  4/4/2008
// Author:  K. Loux
// Description:  Contains the class functionality (event handlers, etc.) for the
//				 MAIN_NOTEBOOK class.  Derived from wxAuiNotebook.
// History:
//	1/28/2009	- Changed structure of GUI components so context menu creation for all
//				  objects is handled by the MAIN_FRAME class.

// CarDesigner headers
#include "gui/components/mainNotebook.h"
#include "gui/components/mainFrame.h"
#include "vUtilities/debugger.h"
#include "gui/guiObject.h"

// wxWidgets headers
#include <wx/utils.h>

//==========================================================================
// Class:			MAIN_NOTEBOOK
// Function:		MAIN_NOTEBOOK
//
// Description:		Constructor for the MAIN_NOTEBOOK class.
//
// Input Arguments:
//		_MainFrame	= MAIN_FRAME&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		style		= long for passing to parent class's constructor
//		_debugger	= const Debugger& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MAIN_NOTEBOOK::MAIN_NOTEBOOK(MAIN_FRAME &_MainFrame, wxWindowID id, const wxPoint& pos,
							 const wxSize& size, long style, const Debugger &_debugger)
							 : wxAuiNotebook(&_MainFrame, id, pos, size, style),
							 debugger(_debugger), MainFrame(_MainFrame)
{
	SetArtProvider(new wxAuiSimpleTabArt);
}

//==========================================================================
// Class:			MAIN_NOTEBOOK
// Function:		~MAIN_NOTEBOOK
//
// Description:		Destructor for the MAIN_NOTEBOOK class.
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
MAIN_NOTEBOOK::~MAIN_NOTEBOOK()
{
}

//==========================================================================
// Class:			MAIN_NOTEBOOK
// Function:		Event Table
//
// Description:		Links the GUI events with event handling functions.
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
BEGIN_EVENT_TABLE(MAIN_NOTEBOOK, wxAuiNotebook)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY,		MAIN_NOTEBOOK::NotebookPageClose_Event)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY,		MAIN_NOTEBOOK::NotebookPageChanged_Event)
	EVT_AUINOTEBOOK_TAB_RIGHT_UP(wxID_ANY,		MAIN_NOTEBOOK::NotebookTabRightClick_Event)
END_EVENT_TABLE();

//==========================================================================
// Class:			MAIN_NOTEBOOK
// Function:		NotebookPageClose_Event
//
// Description:		Calls the close method for the object associated with the
//					closed tab.  Vetoed if the user does not confirm the
//					close.
//
// Input Arguments:
//		event	= &wxAuiNotebookEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_NOTEBOOK::NotebookPageClose_Event(wxAuiNotebookEvent &event)
{
	// Determine the object that was just clicked on
	GUI_OBJECT *ClickedObject = MainFrame.GetObjectByIndex(event.GetSelection());

	// Make sure the selection is valid
	if (ClickedObject == NULL)
		return;

	// Call the close function to make sure everything is saved and the user actually
	// wants to close the object.
	if (!ClickedObject->Close(true))
		event.Veto();
}

//==========================================================================
// Class:			MAIN_NOTEBOOK
// Function:		NotebookPageChanged_Event
//
// Description:		Sets the ActiveCarIndex to the car associated with the
//					notebook page that was just selected.
//
// Input Arguments:
//		event	= &wxAuiNotebookEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_NOTEBOOK::NotebookPageChanged_Event(wxAuiNotebookEvent& WXUNUSED(event))
{
	// Tell the main application what car we've just selected
	MainFrame.SetActiveIndex(GetSelection());
}

//==========================================================================
// Class:			MAIN_NOTEBOOK
// Function:		NotebookTabRightClick_Event
//
// Description:		Pops-up a menu containing actions that can be performed
//					on the selected object.
//
// Input Arguments:
//		event	= &wxAuiNotebookEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_NOTEBOOK::NotebookTabRightClick_Event(wxAuiNotebookEvent &event)
{
	// Determine the position of the mouse (this is in screen coords)
	wxPoint MousePosition = wxGetMousePosition();

	// Subtract the location of the top left corner of the window from the
	// mouse position to get the mouse position w.r.t. this notebook.
	MousePosition -= GetScreenPosition();

	// To generate a context menu in the correct position, we need to convert
	// the position to client coordinates w.r.t. MainFrame.
	MousePosition += GetPosition();

	// Create and display the context menu
	MainFrame.CreateContextMenu(event.GetSelection(), MousePosition);
}