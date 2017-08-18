/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSweepNotebook.cpp
// Date:  11/14/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditSweepNotebook class.

// Local headers
#include "editSweepNotebook.h"
#include "editSweepRangePanel.h"
#include "editSweepPlotsPanel.h"
#include "editSweepOptionsPanel.h"
#include "../../../sweep.h"
#include "VVASE/gui/components/mainFrame.h"
#include "../editPanel.h"
#include "VVASE/core/utilities/debugger.h"

namespace VVASE
{

//==========================================================================
// Class:			EditSweepNotebook
// Function:		EditSweepNotebook
//
// Description:		Constructor for EditSweepNotebook class.  Initializes
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
EditSweepNotebook::EditSweepNotebook(EditPanel &parent, wxWindowID id,
	const wxPoint& pos, const wxSize& size, long style)
	: wxNotebook(&parent, id, pos, size, style), parent(parent)
{
	currentSweep = NULL;
}

//==========================================================================
// Class:			EditSweepNotebook
// Function:		~EditSweepNotebook
//
// Description:		Destructor for EditSweepNotebook class.
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
EditSweepNotebook::~EditSweepNotebook()
{
}

//==========================================================================
// Class:			EditSweepNotebook
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
void EditSweepNotebook::UpdateInformation()
{
	// Make sure the object has already been assigned
	if (currentSweep)
		// Call the method that performs the update
		UpdateInformation(currentSweep);
}

//==========================================================================
// Class:			EditSweepNotebook
// Function:		UpdateInformation
//
// Description:		Updates the information in this notebook.
//
// Input Arguments:
//		currentSweep	= Sweep* pointing to the currenly active object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSweepNotebook::UpdateInformation(Sweep *currentSweep)
{
	this->currentSweep = currentSweep;

	if (!currentSweep)
	{
		DeleteAllPages();
		return;
	}

	if (GetPageCount() == 0)
		CreateControls();

	// Call the update functions for the pages, too
	editRange->UpdateInformation(currentSweep);
	editPlots->UpdateInformation(currentSweep);
	editOptions->UpdateInformation(currentSweep);
}

//==========================================================================
// Class:			EditSweepNotebook
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
void EditSweepNotebook::CreateControls()
{
	DeleteAllPages();

	// Create the notebook pages
	editRange = new EditSweepRangePanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	editPlots = new EditSweepPlotsPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	editOptions = new EditSweepOptionsPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	// Add them to the notebook
	AddPage(editRange, _T("Range"));
	AddPage(editPlots, _T("Active Plots"));
	AddPage(editOptions, _T("Options"));
}

}// namespace VVASE
