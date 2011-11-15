/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_PANEL class.  This
//				 class is used to edit the car parameters.  Different panels
//				 are displayed for editing different sub-systems.
// History:

// CarDesigner headers
#include "vCar/car.h"
#include "vCar/tireSet.h"
#include "gui/guiObject.h"
#include "gui/iteration.h"
#include "gui/guiCar.h"
#include "gui/renderer/carRenderer.h"
#include "gui/components/mainFrame.h"
#include "gui/components/mainTree.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editAerodynamicsPanel.h"
#include "gui/components/editPanel/guiCar/editBrakesPanel.h"
#include "gui/components/editPanel/guiCar/editDifferentialPanel.h"
#include "gui/components/editPanel/guiCar/editDrivetrainPanel.h"
#include "gui/components/editPanel/guiCar/editEnginePanel.h"
#include "gui/components/editPanel/guiCar/editSuspensionNotebook.h"
#include "gui/components/editPanel/guiCar/editMassPanel.h"
#include "gui/components/editPanel/guiCar/editTiresPanel.h"
#include "gui/components/editPanel/iteration/editIterationNotebook.h"
#include "vUtilities/debugger.h"

//==========================================================================
// Class:			EDIT_PANEL
// Function:		EDIT_PANEL
//
// Description:		Constructor for EDIT_PANEL class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		_MainFrame	= MAIN_FRAME&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		_debugger	= const Debugger& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EDIT_PANEL::EDIT_PANEL(MAIN_FRAME &_MainFrame, wxWindowID id, const wxPoint& pos,
					   const wxSize& size, const Debugger &_debugger) :
					   wxPanel(&_MainFrame, id, pos, size), debugger(_debugger),
					   MainFrame(_MainFrame)
{
	// Get the systems tree
	SystemsTree = MainFrame.GetSystemsTree();

	// Initialize the 'Current' class members
	CurrentType = GUI_OBJECT::TYPE_NONE;
	CurrentObject = NULL;

	// Initialize all of the control pointers to NULL
	EditAerodynamics = NULL;
	EditBrakes = NULL;
	EditDifferential = NULL;
	EditDrivetrain = NULL;
	EditEngine = NULL;
	EditMass = NULL;
	EditSuspension = NULL;
	EditTires = NULL;

	EditIteration = NULL;

	// Initialize the mutex pointer to NULL
	CarMutex = NULL;

	// Create and set the sizer for this panel
	Sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(Sizer);
}

//==========================================================================
// Class:			EDIT_PANEL
// Function:		~EDIT_PANEL
//
// Description:		Destructor for EDIT_PANEL class.
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
EDIT_PANEL::~EDIT_PANEL()
{
}

//==========================================================================
// Class:			EDIT_PANEL
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
void EDIT_PANEL::UpdateInformation(void)
{
	// Make sure the object has already been assigned
	if (CurrentObject)
		// Call the method that performs the update
		UpdateInformation(CurrentObject);

	return;
}

//==========================================================================
// Class:			EDIT_PANEL
// Function:		UpdateInformation
//
// Description:		Updates the information in this notebook.
//
// Input Arguments:
//		_CurrentObject	= GUI_OBJECT* pointing to the currenly active object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_PANEL::UpdateInformation(GUI_OBJECT *_CurrentObject)
{
	// Update the class member
	CurrentObject = _CurrentObject;

	// If the current object is NULL, we no longer have an object to represent
	if (!CurrentObject)
	{
		// Set CurrentType to None
		CurrentType = GUI_OBJECT::TYPE_NONE;

		// Delete all of the controls and set the pointers to NULL
		DeleteAllControls();

		return;
	}

	// Set the mutex pointer to NULL
	CarMutex = NULL;

	// Check to see if the item selected in the tree control is associated with this
	bool IgnoreSystemsTree = true;
	wxTreeItemId TempTreeID;
	if (SystemsTree->GetSelectedItem(&TempTreeID) == CurrentObject)
	{
		// The object was a match, but let's make sure the tree item ID is valid
		if (TempTreeID.IsOk())
			// The SystemsTree has a valid item selected - don't ignore it
			IgnoreSystemsTree = false;
	}

	// Compare the current type with the type of the new object to decide if
	// we need to delete the existing pages
	if (CurrentType != CurrentObject->GetType() || (!IgnoreSystemsTree && TempTreeID != CurrentTreeID))
	{
		// Assign the current type to this object
		CurrentType = CurrentObject->GetType();

		// Assign the current tree item ID to this object
		CurrentTreeID = TempTreeID;

		// Create the controls
		CreateControls(IgnoreSystemsTree);
	}

	// Take different actions depending on the object's type
	switch (CurrentType)
	{
	case GUI_OBJECT::TYPE_CAR:
		{
			// Get a pointer to the car's mutex
			CarMutex = &(static_cast<GUI_CAR*>(CurrentObject)->GetOriginalCar().GetMutex());

			// Ensure exclusive access to the object during the update
			wxMutexLocker Lock(*CarMutex);

			// Call the appropriate update function
			// We check to see which one to call based on which object exists
			if (EditAerodynamics)
			{
			}
			else if (EditBrakes)
				EditBrakes->UpdateInformation(static_cast<GUI_CAR*>(CurrentObject)->GetOriginalCar().Brakes);
			else if (EditDifferential)
			{
			}
			else if (EditDrivetrain)
			{
			}
			else if (EditEngine)
			{
			}
			else if (EditMass)
				EditMass->UpdateInformation(static_cast<GUI_CAR*>(CurrentObject)->GetOriginalCar().MassProperties);
			else if (EditSuspension)
				// Update the suspension notebook
				EditSuspension->UpdateInformation(&static_cast<GUI_CAR*>(CurrentObject)->GetOriginalCar());
			else if (EditTires)
				// Update the tires panel
				EditTires->UpdateInformation(static_cast<GUI_CAR*>(CurrentObject)->GetOriginalCar().Tires);
		}

		break;

	case GUI_OBJECT::TYPE_ITERATION:
		EditIteration->UpdateInformation(static_cast<ITERATION*>(CurrentObject));
		break;

	// Unused cases
	case GUI_OBJECT::TYPE_OPTIMIZATION:
	case GUI_OBJECT::TYPE_NONE:
		break;

	// Fail on unknown cases to prevent forgetting any
	default:
		assert(0);
		break;
	}

	return;
}

//==========================================================================
// Class:			EDIT_PANEL
// Function:		CreateControls
//
// Description:		Creates the controls for this notebook.
//
// Input Arguments:
//		IgnoreSystemsTree	= bool indicating whether or not to trust the
//							  type of the selection in the systems tree
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_PANEL::CreateControls(bool IgnoreSystemsTree)
{
	// Delete any existing panels/notebooks
	DeleteAllControls();

	// Add different pages depending on the object's type
	switch (CurrentType)
	{
	case GUI_OBJECT::TYPE_CAR:
		// Create the appropriate controls, depending on the type of the selected item
		// If the selected item is the root item, we don't create any controls here
		if (!IgnoreSystemsTree)
		{
			if (SystemsTree->GetItemType(CurrentTreeID) == GUI_CAR::SubsystemAerodynamics)
			{
			}
			else if (SystemsTree->GetItemType(CurrentTreeID) == GUI_CAR::SubsystemBrakes)
			{
				// Create the brakes panel
				EditBrakes = new EDIT_BRAKES_PANEL(*this, wxID_ANY,
					wxDefaultPosition, wxDefaultSize, debugger);

				// Add the brakes panel to the sizer
				Sizer->Add(EditBrakes, 1, wxEXPAND);
			}
			else if (SystemsTree->GetItemType(CurrentTreeID) == GUI_CAR::SubsystemDrivetrain)
			{
			}
			else if (SystemsTree->GetItemType(CurrentTreeID) == GUI_CAR::SubsystemEngine)
			{
			}
			else if (SystemsTree->GetItemType(CurrentTreeID) == GUI_CAR::SubsystemMassProperties)
			{
				// Create the mass properties panel
				EditMass = new EDIT_MASS_PANEL(*this, wxID_ANY,
					wxDefaultPosition, wxDefaultSize, debugger);

				// Add the mass panel to the sizer
				Sizer->Add(EditMass, 1, wxEXPAND);
			}
			else if (SystemsTree->GetItemType(CurrentTreeID) == GUI_CAR::SubsystemSuspension)
			{
				// Create the suspension notebook
				EditSuspension = new EDIT_SUSPENSION_NOTEBOOK(*this, wxID_ANY,
					wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM, debugger);

				// Add the suspension to the sizer
				Sizer->Add(EditSuspension, 1, wxEXPAND);
			}
			else if (SystemsTree->GetItemType(CurrentTreeID) == GUI_CAR::SubsystemTires)
			{
				// Create the tires panel
				EditTires = new EDIT_TIRES_PANEL(*this, wxID_ANY,
					wxDefaultPosition, wxDefaultSize, debugger);

				// Add the tire set to the sizer
				Sizer->Add(EditTires, 1, wxEXPAND);
			}
		}

		break;

	case GUI_OBJECT::TYPE_ITERATION:
		// Create the iteration panel
		EditIteration = new EDIT_ITERATION_NOTEBOOK(*this, wxID_ANY,
			wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM, debugger);

		// Add the iteration panel to the sizer
		Sizer->Add(EditIteration, 1, wxEXPAND);
		break;

	// Unused cases
	case GUI_OBJECT::TYPE_OPTIMIZATION:
	case GUI_OBJECT::TYPE_NONE:
		break;

	// Fail on unknown cases to prevent forgetting any
	default:
		assert(0);
		break;
	}

	// Update the sizer
	Sizer->Layout();

	return;
}

//==========================================================================
// Class:			EDIT_PANEL
// Function:		DeleteAllControls
//
// Description:		Deletes all of the controls in the notebook.
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
void EDIT_PANEL::DeleteAllControls(void)
{
	// Remove the panel from the sizer (if the panel exists)
	if (EditAerodynamics || EditBrakes || EditDifferential || EditDrivetrain
		|| EditEngine || EditMass || EditSuspension || EditTires ||
		EditIteration)
		Sizer->Remove(0);
	Sizer->Layout();

	// Delete all of the controls
	delete EditAerodynamics;
	delete EditBrakes;
	delete EditDifferential;
	delete EditDrivetrain;
	delete EditEngine;
	delete EditMass;
	delete EditSuspension;
	delete EditTires;

	delete EditIteration;

	// Set all of the pointers to NULL
	EditAerodynamics = NULL;
	EditBrakes = NULL;
	EditDifferential = NULL;
	EditDrivetrain = NULL;
	EditEngine = NULL;
	EditMass = NULL;
	EditSuspension = NULL;
	EditTires = NULL;

	EditIteration = NULL;

	// When we remove these controls, we no longer want the helpr orb to be active
	// in this car's renderer.  This makes the orb go inactive if we click off of
	// the suspension page.
	if (CurrentObject)
	{
		// This is only necessary if the current object is TYPE_CAR
		if (CurrentObject->GetType() == GUI_OBJECT::TYPE_CAR)
		{
			// Deactive the helper orb
			static_cast<CAR_RENDERER*>(CurrentObject->GetNotebookTab())->DeactivateHelperOrb();

			// Update the visual display
			CurrentObject->UpdateDisplay();
		}
	}

	return;
}