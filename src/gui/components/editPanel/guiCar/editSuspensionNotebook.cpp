/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSuspensionNotebook.cpp
// Date:  2/19/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditSuspensionNotebook class.
//        This class is used to edit the car parameters.  Different panels
//        are displayed for editing different sub-systems.

// Local headers
#include "vCar/car.h"
#include "vCar/suspension.h"
#include "vCar/tireSet.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/components/mainTree.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editSuspensionNotebook.h"
#include "gui/components/editPanel/guiCar/editCornerPanel.h"
#include "gui/components/editPanel/guiCar/editSuspensionPanel.h"
#include "gui/components/editPanel/guiCar/editSuspensionRatesPanel.h"
#include "vUtilities/debugger.h"

namespace VVASE
{

//==========================================================================
// Class:			EditSuspensionNotebook
// Function:		EditSuspensionNotebook
//
// Description:		Constructor for EditSuspensionNotebook class.  Initializes
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
EditSuspensionNotebook::EditSuspensionNotebook(EditPanel &parent, wxWindowID id,
	const wxPoint& pos, const wxSize& size, long style)
	: wxNotebook(&parent, id, pos, size, style), parent(parent)
{
	currentCar = NULL;
}

//==========================================================================
// Class:			EditSuspensionNotebook
// Function:		~EditSuspensionNotebook
//
// Description:		Destructor for EditSuspensionNotebook class.
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
EditSuspensionNotebook::~EditSuspensionNotebook()
{
}

//==========================================================================
// Class:			EditSuspensionNotebook
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
void EditSuspensionNotebook::UpdateInformation()
{
	// Make sure the object has already been assigned
	if (currentCar)
		// Call the method that performs the update
		UpdateInformation(currentCar);
}

//==========================================================================
// Class:			EditSuspensionNotebook
// Function:		UpdateInformation
//
// Description:		Updates the information in this notebook.
//
// Input Arguments:
//		currentCar	= Car* pointing to the currenly active object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSuspensionNotebook::UpdateInformation(Car *currentCar)
{
	this->currentCar = currentCar;

	if (!currentCar)
	{
		DeleteAllPages();
		return;
	}

	if (GetPageCount() == 0)
		CreateControls();

	// Check to see if the car is symmetric, and if the current number of pages is
	// correct.
	if (currentCar->suspension->isSymmetric)
	{
		// A symmetric car only gets four pages
		if (GetPageCount() > 4)
		{
			// Remove the extra pages (the left side pages)
			DeletePage(5);
			DeletePage(3);

			// Set the left side pointers to NULL
			leftFront = NULL;
			leftRear = NULL;

			// Rename the tabs
			SetPageText(2, _T("Front"));
			SetPageText(3, _T("Rear"));
		}
	}
	else// Not symmetric
	{
		// Make sure we have enough pages
		if (GetPageCount() != 6)
			// Delete and re-create all of the pages
			CreateControls();
	}

	// Call the update functions for the pages, too
	suspension->UpdateInformation(currentCar->suspension);
	rates->UpdateInformation(currentCar->suspension);
	rightFront->UpdateInformation(&currentCar->suspension->rightFront, currentCar->suspension,
		currentCar->suspension->frontBarStyle, currentCar->HasFrontHalfShafts());
	if (leftFront)
		leftFront->UpdateInformation(&currentCar->suspension->leftFront, currentCar->suspension,
			currentCar->suspension->frontBarStyle, currentCar->HasFrontHalfShafts());
	rightRear->UpdateInformation(&currentCar->suspension->rightRear, currentCar->suspension,
		currentCar->suspension->rearBarStyle, currentCar->HasRearHalfShafts());
	if (leftRear)
		leftRear->UpdateInformation(&currentCar->suspension->leftRear, currentCar->suspension,
			currentCar->suspension->rearBarStyle, currentCar->HasRearHalfShafts());

	// Also make sure we're symmetric if we're supposed to be
	currentCar->suspension->UpdateSymmetry();
}

//==========================================================================
// Class:			EditSuspensionNotebook
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
void EditSuspensionNotebook::CreateControls()
{
	// Delete the existing pages before we create the new ones
	DeleteAllPages();

	// Create the notebook pages
	suspension = new EditSuspensionPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	rates = new EditSuspensionRatesPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	rightFront = new EditCornerPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	leftFront = new EditCornerPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	rightRear = new EditCornerPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	leftRear = new EditCornerPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	// Add them to the notebook
	AddPage(suspension, _T("General"));
	AddPage(rates, _T("Rates"));
	AddPage(rightFront, _T("RF"));
	AddPage(leftFront, _T("LF"));
	AddPage(rightRear, _T("RR"));
	AddPage(leftRear, _T("LR"));
}

}// namespace VVASE
