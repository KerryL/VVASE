/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editSuspensionNotebook.cpp
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EditSuspensionNotebook class.
//				 This class is used to edit the car parameters.  Different panels
//				 are displayed for editing different sub-systems.
// History:

// VVASE headers
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
#include "vUtilities/debugger.h"

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
		// A symmetric car only gets three pages
		if (GetPageCount() > 3)
		{
			// Remove the extra pages (the left side pages)
			DeletePage(4);
			DeletePage(2);

			// Set the left side pointers to NULL
			leftFront = NULL;
			leftRear = NULL;

			// Rename the tabs
			SetPageText(1, _T("Front"));
			SetPageText(2, _T("Rear"));
		}
	}
	else// Not symmetric
	{
		// Make sure we have enough pages
		if (GetPageCount() != 5)
			// Delete and re-create all of the pages
			CreateControls();
	}

	// Call the update functions for the pages, too
	suspension->UpdateInformation(currentCar->suspension);
	rightFront->UpdateInformation(&currentCar->suspension->rightFront,
		currentCar->suspension->frontBarStyle, currentCar->HasFrontHalfShafts());
	if (leftFront)
		leftFront->UpdateInformation(&currentCar->suspension->leftFront,
			currentCar->suspension->frontBarStyle, currentCar->HasFrontHalfShafts());
	rightRear->UpdateInformation(&currentCar->suspension->rightRear,
		currentCar->suspension->rearBarStyle, currentCar->HasRearHalfShafts());
	if (leftRear)
		leftRear->UpdateInformation(&currentCar->suspension->leftRear,
			currentCar->suspension->rearBarStyle, currentCar->HasRearHalfShafts());

	// Also make sure we're symmetric if we're supposed to be
	UpdateSymmetry();
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
	rightFront = new EditCornerPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	leftFront = new EditCornerPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	rightRear = new EditCornerPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	leftRear = new EditCornerPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	// Add them to the notebook
	AddPage(suspension, _T("General"));
	AddPage(rightFront, _T("RF"));
	AddPage(leftFront, _T("LF"));
	AddPage(rightRear, _T("RR"));
	AddPage(leftRear, _T("LR"));
}

//==========================================================================
// Class:			EditSuspensionNotebook
// Function:		UpdateSymmetry
//
// Description:		Goes through all of the corner point and ensures symmetry
//					from right to left (updates left to match right).
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
void EditSuspensionNotebook::UpdateSymmetry()
{
	// Check to make sure this car is symmetric
	if (currentCar->suspension->isSymmetric)
	{
		int i;

		// Copy the front points
		for (i = 0; i < Corner::NumberOfHardpoints; i++)
		{
			// Copy the point
			currentCar->suspension->leftFront.hardpoints[i] = currentCar->suspension->rightFront.hardpoints[i];

			// Flip the sign on the Y componenet
			currentCar->suspension->leftFront.hardpoints[i].y *= -1.0;
		}

		// Copy the rear points
		for (i = 0; i < Corner::NumberOfHardpoints; i++)
		{
			// Copy the point
			currentCar->suspension->leftRear.hardpoints[i] = currentCar->suspension->rightRear.hardpoints[i];

			// Flip the sign on the Y componenet
			currentCar->suspension->leftRear.hardpoints[i].y *= -1.0;
		}

		// Copy the other information in the front
		currentCar->suspension->leftFront.actuationAttachment = currentCar->suspension->rightFront.actuationAttachment;
		currentCar->suspension->leftFront.actuationType = currentCar->suspension->rightFront.actuationType;
		currentCar->suspension->leftFront.damper = currentCar->suspension->rightFront.damper;
		currentCar->suspension->leftFront.spring = currentCar->suspension->rightFront.spring;
		currentCar->suspension->leftFront.staticCamber = currentCar->suspension->rightFront.staticCamber;
		currentCar->suspension->leftFront.staticToe = currentCar->suspension->rightFront.staticToe;

		// Copy the other information in the rear
		currentCar->suspension->leftRear.actuationAttachment = currentCar->suspension->rightRear.actuationAttachment;
		currentCar->suspension->leftRear.actuationType = currentCar->suspension->rightRear.actuationType;
		currentCar->suspension->leftRear.damper = currentCar->suspension->rightRear.damper;
		currentCar->suspension->leftRear.spring = currentCar->suspension->rightRear.spring;
		currentCar->suspension->leftRear.staticCamber = currentCar->suspension->rightRear.staticCamber;
		currentCar->suspension->leftRear.staticToe = currentCar->suspension->rightRear.staticToe;
	}
}
