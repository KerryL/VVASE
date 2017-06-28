/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSweepNotebook.cpp
// Date:  11/14/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditIterationNotebook class.

// Local headers
#include "gui/iteration.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/iteration/editIterationNotebook.h"
#include "gui/components/editPanel/iteration/editIterationRangePanel.h"
#include "gui/components/editPanel/iteration/editIterationPlotsPanel.h"
#include "gui/components/editPanel/iteration/editIterationOptionsPanel.h"
#include "vUtilities/debugger.h"

//==========================================================================
// Class:			EditIterationNotebook
// Function:		EditIterationNotebook
//
// Description:		Constructor for EditIterationNotebook class.  Initializes
//					the form and creates the controls, etc.
//
// Input Arguments:
//		parent		= EditPanel&, reference to this object's owner
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
EditIterationNotebook::EditIterationNotebook(EditPanel &parent, wxWindowID id,
	const wxPoint& pos, const wxSize& size, long style)
	: wxNotebook(&parent, id, pos, size, style), parent(parent)
{
	currentIteration = NULL;
}

//==========================================================================
// Class:			EditIterationNotebook
// Function:		~EditIterationNotebook
//
// Description:		Destructor for EditIterationNotebook class.
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
EditIterationNotebook::~EditIterationNotebook()
{
}

//==========================================================================
// Class:			EditIterationNotebook
// Function:		UpdateInformation
//
// Description:		Updates the information in this notebook for the current
//					object (if one exists).
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
void EditIterationNotebook::UpdateInformation()
{
	// Make sure the object has already been assigned
	if (currentIteration)
		// Call the method that performs the update
		UpdateInformation(currentIteration);
}

//==========================================================================
// Class:			EditIterationNotebook
// Function:		UpdateInformation
//
// Description:		Updates the information in this notebook.
//
// Input Arguments:
//		_currentIteration	= Iteration* pointing to the currenly active object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditIterationNotebook::UpdateInformation(Iteration *currentIteration)
{
	this->currentIteration = currentIteration;

	if (!currentIteration)
	{
		DeleteAllPages();
		return;
	}

	if (GetPageCount() == 0)
		CreateControls();

	// Call the update functions for the pages, too
	editRange->UpdateInformation(currentIteration);
	editPlots->UpdateInformation(currentIteration);
	editOptions->UpdateInformation(currentIteration);
}

//==========================================================================
// Class:			EditIterationNotebook
// Function:		CreateControls
//
// Description:		Creates the controls for this notebook.
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
void EditIterationNotebook::CreateControls()
{
	DeleteAllPages();

	// Create the notebook pages
	editRange = new EditIterationRangePanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	editPlots = new EditIterationPlotsPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	editOptions = new EditIterationOptionsPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	// Add them to the notebook
	AddPage(editRange, _T("Range"));
	AddPage(editPlots, _T("Active Plots"));
	AddPage(editOptions, _T("Options"));
}
