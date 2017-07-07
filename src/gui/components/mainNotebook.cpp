/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  mainNotebook.cpp
// Date:  4/4/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class functionality (event handlers, etc.) for the
//        MainNotebook class.  Derived from wxAuiNotebook.

// Local headers
#include "VVASE/gui/components/mainNotebook.h"
#include "VVASE/gui/components/mainFrame.h"
#include "VVASE/core/utilities/debugger.h"
#include "VVASE/gui/guiObject.h"

// wxWidgets headers
#include <wx/utils.h>

namespace VVASE
{

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

	mainFrame.CreateContextMenu(event.GetSelection());
}

}// namespace VVASE
