/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  dropTarget.cpp
// Date:  3/7/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Handles events when users drag-and-drop files onto this application.

// Local headers
#include "gui/dropTarget.h"
#include "gui/components/mainFrame.h"

namespace VVASE
{

//==========================================================================
// Class:			DropTarget
// Function:		DropTarget
//
// Description:		Constructor for DropTarget class.
//
// Input Arguments:
//		mainFrame	= &MainFrame, reference to main application window
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DropTarget::DropTarget(MainFrame &mainFrame) : mainFrame(mainFrame)
{
}

//==========================================================================
// Class:			DropTarget
// Function:		~DropTarget
//
// Description:		Destructor for DropTarget class.
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
DropTarget::~DropTarget()
{
}

//==========================================================================
// Class:			DropTarget
// Function:		OnDropFiles
//
// Description:		Overloaded virtual method from wxFileDropTarget.
//
// Input Arguments:
//		x			= wxCoord (unused)
//		y			= wxCoord (unused)
//		filenames	= const &wxArrayString containing the list of filenames
//					  being dropped
//
// Output Arguments:
//		None
//
// Return Value:
//		true to accept the data, false to veto
//
//==========================================================================
bool DropTarget::OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
							  const wxArrayString &filenames)
{
	// Load each file
	unsigned int i;
	for (i = 0; i < filenames.Count(); i++)
		mainFrame.LoadFile(filenames[i]);

	return true;
}

}// namespace VVASE
