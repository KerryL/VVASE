/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editSuspensionNotebook.cpp
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_SUSPENSION_NOTEBOOK class.
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
// Class:			EDIT_SUSPENSION_NOTEBOOK
// Function:		EDIT_SUSPENSION_NOTEBOOK
//
// Description:		Constructor for EDIT_SUSPENSION_NOTEBOOK class.  Initializes
//					the form and creates the controls, etc.
//
// Input Arguments:
//		_Parent		= EDIT_PANEL&, reference to this object's owner
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
EDIT_SUSPENSION_NOTEBOOK::EDIT_SUSPENSION_NOTEBOOK(EDIT_PANEL &_Parent, wxWindowID id,
												 const wxPoint& pos, const wxSize& size,
												 long style, const Debugger &_debugger)
												 : wxNotebook(&_Parent, id, pos, size, style),
												 debugger(_debugger), Parent(_Parent)
{
	// Initialize the 'Current' class members
	CurrentCar = NULL;
}

//==========================================================================
// Class:			EDIT_SUSPENSION_NOTEBOOK
// Function:		~EDIT_SUSPENSION_NOTEBOOK
//
// Description:		Destructor for EDIT_SUSPENSION_NOTEBOOK class.
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
EDIT_SUSPENSION_NOTEBOOK::~EDIT_SUSPENSION_NOTEBOOK()
{
}

//==========================================================================
// Class:			EDIT_SUSPENSION_NOTEBOOK
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
void EDIT_SUSPENSION_NOTEBOOK::UpdateInformation(void)
{
	// Make sure the object has already been assigned
	if (CurrentCar)
		// Call the method that performs the update
		UpdateInformation(CurrentCar);
}

//==========================================================================
// Class:			EDIT_SUSPENSION_NOTEBOOK
// Function:		UpdateInformation
//
// Description:		Updates the information in this notebook.
//
// Input Arguments:
//		_CurrentCar	= Car* pointing to the currenly active object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_SUSPENSION_NOTEBOOK::UpdateInformation(Car *_CurrentCar)
{
	// Update the class member
	CurrentCar = _CurrentCar;

	// If the current object is NULL, we no longer have an object to represent
	if (!CurrentCar)
	{
		// Delete all the notebook pages
		DeleteAllPages();

		return;
	}

	// If we don't have any pages, we need to create the controls
	if (GetPageCount() == 0)
		CreateControls();

	// Check to see if the car is symmetric, and if the current number of pages is
	// correct.
	if (CurrentCar->suspension->isSymmetric)
	{
		// A symmetric car only gets three pages
		if (GetPageCount() > 3)
		{
			// Remove the extra pages (the left side pages)
			DeletePage(4);
			DeletePage(2);

			// Set the left side pointers to NULL
			LeftFront = NULL;
			LeftRear = NULL;

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
	Suspension->UpdateInformation(CurrentCar->suspension);
	RightFront->UpdateInformation(&CurrentCar->suspension->rightFront,
		CurrentCar->suspension->frontBarStyle, CurrentCar->HasFrontHalfShafts());
	if (LeftFront)
		LeftFront->UpdateInformation(&CurrentCar->suspension->leftFront,
			CurrentCar->suspension->frontBarStyle, CurrentCar->HasFrontHalfShafts());
	RightRear->UpdateInformation(&CurrentCar->suspension->rightRear,
		CurrentCar->suspension->rearBarStyle, CurrentCar->HasRearHalfShafts());
	if (LeftRear)
		LeftRear->UpdateInformation(&CurrentCar->suspension->leftRear,
			CurrentCar->suspension->rearBarStyle, CurrentCar->HasRearHalfShafts());

	// Also make sure we're symmetric if we're supposed to be
	UpdateSymmetry();
}

//==========================================================================
// Class:			EDIT_SUSPENSION_NOTEBOOK
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
void EDIT_SUSPENSION_NOTEBOOK::CreateControls(void)
{
	// Delete the existing pages before we create the new ones
	DeleteAllPages();

	// Create the notebook pages
	Suspension = new EDIT_SUSPENSION_PANEL(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize, debugger);
	RightFront = new EDIT_CORNER_PANEL(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize, debugger);
	LeftFront = new EDIT_CORNER_PANEL(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize, debugger);
	RightRear = new EDIT_CORNER_PANEL(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize, debugger);
	LeftRear = new EDIT_CORNER_PANEL(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize, debugger);

	// Add them to the notebook
	AddPage(Suspension, _T("General"));
	AddPage(RightFront, _T("RF"));
	AddPage(LeftFront, _T("LF"));
	AddPage(RightRear, _T("RR"));
	AddPage(LeftRear, _T("LR"));
}

//==========================================================================
// Class:			EDIT_SUSPENSION_NOTEBOOK
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
void EDIT_SUSPENSION_NOTEBOOK::UpdateSymmetry(void)
{
	// Check to make sure this car is symmetric
	if (CurrentCar->suspension->isSymmetric)
	{
		int i;

		// Copy the front points
		for (i = 0; i < Corner::NumberOfHardpoints; i++)
		{
			// Copy the point
			CurrentCar->suspension->leftFront.hardpoints[i] = CurrentCar->suspension->rightFront.hardpoints[i];

			// Flip the sign on the Y componenet
			CurrentCar->suspension->leftFront.hardpoints[i].y *= -1.0;
		}

		// Copy the rear points
		for (i = 0; i < Corner::NumberOfHardpoints; i++)
		{
			// Copy the point
			CurrentCar->suspension->leftRear.hardpoints[i] = CurrentCar->suspension->rightRear.hardpoints[i];

			// Flip the sign on the Y componenet
			CurrentCar->suspension->leftRear.hardpoints[i].y *= -1.0;
		}

		// Copy the other information in the front
		CurrentCar->suspension->leftFront.actuationAttachment = CurrentCar->suspension->rightFront.actuationAttachment;
		CurrentCar->suspension->leftFront.actuationType = CurrentCar->suspension->rightFront.actuationType;
		CurrentCar->suspension->leftFront.damper = CurrentCar->suspension->rightFront.damper;
		CurrentCar->suspension->leftFront.spring = CurrentCar->suspension->rightFront.spring;
		CurrentCar->suspension->leftFront.staticCamber = CurrentCar->suspension->rightFront.staticCamber;
		CurrentCar->suspension->leftFront.staticToe = CurrentCar->suspension->rightFront.staticToe;

		// Copy the other information in the rear
		CurrentCar->suspension->leftRear.actuationAttachment = CurrentCar->suspension->rightRear.actuationAttachment;
		CurrentCar->suspension->leftRear.actuationType = CurrentCar->suspension->rightRear.actuationType;
		CurrentCar->suspension->leftRear.damper = CurrentCar->suspension->rightRear.damper;
		CurrentCar->suspension->leftRear.spring = CurrentCar->suspension->rightRear.spring;
		CurrentCar->suspension->leftRear.staticCamber = CurrentCar->suspension->rightRear.staticCamber;
		CurrentCar->suspension->leftRear.staticToe = CurrentCar->suspension->rightRear.staticToe;
	}
}
