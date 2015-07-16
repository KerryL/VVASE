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
//				 MainNotebook class.  Derived from wxAuiNotebook.
// History:
//	1/28/2009	- Changed structure of GUI components so context menu creation for all
//				  objects is handled by the MainFrame class.

// CarDesigner headers
#include "gui/components/mainNotebook.h"
#include "gui/components/mainFrame.h"
#include "vUtilities/debugger.h"
#include "gui/guiObject.h"

// wxWidgets headers
#include <wx/utils.h>

//==========================================================================
// Class:			MainNotebook
// Function:		MainNotebook
//
// Description:		Constructor for the MainNotebook class.
//
// Input Arguments:
//		mainFrame	= MainFrame&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		style		= long for passing to parent class's constructor
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MainNotebook::MainNotebook(MainFrame &mainFrame, wxWindowID id, const wxPoint& pos,
	const wxSize& size, long style)
	: wxAuiNotebook(&mainFrame, id, pos, size, style), mainFrame(mainFrame)
{
	SetArtProvider(new wxAuiSimpleTabArt);
}

//==========================================================================
// Class:			MainNotebook
// Function:		~MainNotebook
//
// Description:		Destructor for the MainNotebook class.
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
MainNotebook::~MainNotebook()
{
}

//==========================================================================
// Class:			MainNotebook
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
BEGIN_EVENT_TABLE(MainNotebook, wxAuiNotebook)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY,		MainNotebook::NotebookPageClose_Event)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY,		MainNotebook::NotebookPageChanged_Event)
	EVT_AUINOTEBOOK_TAB_RIGHT_UP(wxID_ANY,		MainNotebook::NotebookTabRightClick_Event)
END_EVENT_TABLE();

//==========================================================================
// Class:			MainNotebook
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
void MainNotebook::NotebookPageClose_Event(wxAuiNotebookEvent &event)
{
	// Determine the object that was just clicked on
	GuiObject *clickedObject = mainFrame.GetObjectByIndex(event.GetSelection());

	// Make sure the selection is valid
	if (clickedObject == NULL)
		return;

	// Call the close function to make sure everything is saved and the user actually
	// wants to close the object.
	if (!clickedObject->Close(true))
		event.Veto();
}

//==========================================================================
// Class:			MainNotebook
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
void MainNotebook::NotebookPageChanged_Event(wxAuiNotebookEvent& WXUNUSED(event))
{
	// Tell the main application what car we've just selected
	mainFrame.SetActiveIndex(GetSelection());
}

//==========================================================================
// Class:			MainNotebook
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
void MainNotebook::NotebookTabRightClick_Event(wxAuiNotebookEvent &event)
{
	// Determine the position of the mouse (this is in screen coords)
	wxPoint mousePosition = wxGetMousePosition();

	// Subtract the location of the top left corner of the window from the
	// mouse position to get the mouse position w.r.t. this notebook.
	mousePosition -= GetScreenPosition();

	// To generate a context menu in the correct position, we need to convert
	// the position to client coordinates w.r.t. mainFrame.
	mousePosition += GetPosition();

	// Create and display the context menu
	mainFrame.CreateContextMenu(event.GetSelection(), mousePosition);
}