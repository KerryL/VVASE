/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY,		MainNotebook::OnNotebookPageClose)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY,		MainNotebook::OnNotebookPageChanged)
	EVT_AUINOTEBOOK_TAB_RIGHT_UP(wxID_ANY,		MainNotebook::OnNotebookTabRightClick)
END_EVENT_TABLE();

//==========================================================================
// Class:			MainNotebook
// Function:		OnNotebookPageClose
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
void MainNotebook::OnNotebookPageClose(wxAuiNotebookEvent &event)
{
	GuiObject *clickedObject = mainFrame.GetObjectByIndex(event.GetSelection());
	if (clickedObject == NULL)
		return;

	if (!clickedObject->Close(true))
		event.Veto();
}

//==========================================================================
// Class:			MainNotebook
// Function:		OnNotebookPageChanged
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
void MainNotebook::OnNotebookPageChanged(wxAuiNotebookEvent& WXUNUSED(event))
{
	mainFrame.SetActiveIndex(GetSelection());
}

//==========================================================================
// Class:			MainNotebook
// Function:		OnNotebookTabRightClick
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
void MainNotebook::OnNotebookTabRightClick(wxAuiNotebookEvent &event)
{
	wxPoint mousePosition = wxGetMousePosition();

	mousePosition -= GetScreenPosition();
	mousePosition += GetPosition();

	mainFrame.CreateContextMenu(event.GetSelection(), mousePosition);
}