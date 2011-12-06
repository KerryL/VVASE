/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EditPanel class.  This
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
// Class:			EditPanel
// Function:		EditPanel
//
// Description:		Constructor for EditPanel class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		_mainFrame	= MainFrame&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EditPanel::EditPanel(MainFrame &_mainFrame, wxWindowID id, const wxPoint& pos,
					   const wxSize& size) :
					   wxPanel(&_mainFrame, id, pos, size), mainFrame(_mainFrame)
{
	// Get the systems tree
	systemsTree = mainFrame.GetSystemsTree();

	// Initialize the 'Current' class members
	currentType = GuiObject::TypeNone;
	currentObject = NULL;

	// Initialize all of the control pointers to NULL
	editAerodynamics = NULL;
	editBrakes = NULL;
	editDifferential = NULL;
	editDrivetrain = NULL;
	editEngine = NULL;
	editMass = NULL;
	editSuspension = NULL;
	editTires = NULL;

	editIteration = NULL;

	// Initialize the mutex pointer to NULL
	carMutex = NULL;

	// Create and set the sizer for this panel
	sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);
}

//==========================================================================
// Class:			EditPanel
// Function:		~EditPanel
//
// Description:		Destructor for EditPanel class.
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
EditPanel::~EditPanel()
{
}

//==========================================================================
// Class:			EditPanel
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
void EditPanel::UpdateInformation(void)
{
	// Make sure the object has already been assigned
	if (currentObject)
		// Call the method that performs the update
		UpdateInformation(currentObject);
}

//==========================================================================
// Class:			EditPanel
// Function:		UpdateInformation
//
// Description:		Updates the information in this notebook.
//
// Input Arguments:
//		_currentObject	= GuiObject* pointing to the currenly active object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditPanel::UpdateInformation(GuiObject *_currentObject)
{
	// Update the class member
	currentObject = _currentObject;

	// If the current object is NULL, we no longer have an object to represent
	if (!currentObject)
	{
		// Set CurrentType to None
		currentType = GuiObject::TypeNone;

		// Delete all of the controls and set the pointers to NULL
		DeleteAllControls();

		return;
	}

	// Set the mutex pointer to NULL
	carMutex = NULL;

	// Check to see if the item selected in the tree control is associated with this
	bool ignoreSystemsTree = true;
	wxTreeItemId tempTreeID;
	if (systemsTree->GetSelectedItem(&tempTreeID) == currentObject)
	{
		// The object was a match, but let's make sure the tree item ID is valid
		if (tempTreeID.IsOk())
			// The SystemsTree has a valid item selected - don't ignore it
			ignoreSystemsTree = false;
	}

	// Compare the current type with the type of the new object to decide if
	// we need to delete the existing pages
	if (currentType != currentObject->GetType() || (!ignoreSystemsTree && tempTreeID != currentTreeID))
	{
		// Assign the current type to this object
		currentType = currentObject->GetType();

		// Assign the current tree item ID to this object
		currentTreeID = tempTreeID;

		// Create the controls
		CreateControls(ignoreSystemsTree);
	}

	// Take different actions depending on the object's type
	switch (currentType)
	{
	case GuiObject::TypeCar:
		{
			// Get a pointer to the car's mutex
			carMutex = &(static_cast<GuiCar*>(currentObject)->GetOriginalCar().GetMutex());

			// Ensure exclusive access to the object during the update
			wxMutexLocker lock(*carMutex);

			// Call the appropriate update function
			// We check to see which one to call based on which object exists
			if (editAerodynamics)
			{
			}
			else if (editBrakes)
				editBrakes->UpdateInformation(static_cast<GuiCar*>(currentObject)->GetOriginalCar().brakes);
			else if (editDifferential)
			{
			}
			else if (editDrivetrain)
			{
			}
			else if (editEngine)
			{
			}
			else if (editMass)
				editMass->UpdateInformation(static_cast<GuiCar*>(currentObject)->GetOriginalCar().massProperties);
			else if (editSuspension)
				// Update the suspension notebook
				editSuspension->UpdateInformation(&static_cast<GuiCar*>(currentObject)->GetOriginalCar());
			else if (editTires)
				// Update the tires panel
				editTires->UpdateInformation(static_cast<GuiCar*>(currentObject)->GetOriginalCar().tires);
		}

		break;

	case GuiObject::TypeIteration:
		editIteration->UpdateInformation(static_cast<Iteration*>(currentObject));
		break;

	// Unused cases
	case GuiObject::TypeOptimization:
	case GuiObject::TypeNone:
		break;

	// Fail on unknown cases to prevent forgetting any
	default:
		assert(0);
		break;
	}
}

//==========================================================================
// Class:			EditPanel
// Function:		CreateControls
//
// Description:		Creates the controls for this notebook.
//
// Input Arguments:
//		ignoreSystemsTree	= bool indicating whether or not to trust the
//							  type of the selection in the systems tree
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditPanel::CreateControls(bool ignoreSystemsTree)
{
	// Delete any existing panels/notebooks
	DeleteAllControls();

	// Add different pages depending on the object's type
	switch (currentType)
	{
	case GuiObject::TypeCar:
		// Create the appropriate controls, depending on the type of the selected item
		// If the selected item is the root item, we don't create any controls here
		if (!ignoreSystemsTree)
		{
			if (systemsTree->GetItemType(currentTreeID) == GuiCar::SubsystemAerodynamics)
			{
			}
			else if (systemsTree->GetItemType(currentTreeID) == GuiCar::SubsystemBrakes)
			{
				// Create the brakes panel
				editBrakes = new EditBrakesPanel(*this, wxID_ANY,
					wxDefaultPosition, wxDefaultSize);

				// Add the brakes panel to the sizer
				sizer->Add(editBrakes, 1, wxEXPAND);
			}
			else if (systemsTree->GetItemType(currentTreeID) == GuiCar::SubsystemDrivetrain)
			{
			}
			else if (systemsTree->GetItemType(currentTreeID) == GuiCar::SubsystemEngine)
			{
			}
			else if (systemsTree->GetItemType(currentTreeID) == GuiCar::SubsystemMassProperties)
			{
				// Create the mass properties panel
				editMass = new EditMassPanel(*this, wxID_ANY,
					wxDefaultPosition, wxDefaultSize);

				// Add the mass panel to the sizer
				sizer->Add(editMass, 1, wxEXPAND);
			}
			else if (systemsTree->GetItemType(currentTreeID) == GuiCar::SubsystemSuspension)
			{
				// Create the suspension notebook
				editSuspension = new EditSuspensionNotebook(*this, wxID_ANY,
					wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);

				// Add the suspension to the sizer
				sizer->Add(editSuspension, 1, wxEXPAND);
			}
			else if (systemsTree->GetItemType(currentTreeID) == GuiCar::SubsystemTires)
			{
				// Create the tires panel
				editTires = new EditTiresPanel(*this, wxID_ANY,
					wxDefaultPosition, wxDefaultSize);

				// Add the tire set to the sizer
				sizer->Add(editTires, 1, wxEXPAND);
			}
		}

		break;

	case GuiObject::TypeIteration:
		// Create the iteration panel
		editIteration = new EditIterationNotebook(*this, wxID_ANY,
			wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);

		// Add the iteration panel to the sizer
		sizer->Add(editIteration, 1, wxEXPAND);
		break;

	// Unused cases
	case GuiObject::TypeOptimization:
	case GuiObject::TypeNone:
		break;

	// Fail on unknown cases to prevent forgetting any
	default:
		assert(0);
		break;
	}

	// Update the sizer
	sizer->Layout();
}

//==========================================================================
// Class:			EditPanel
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
void EditPanel::DeleteAllControls(void)
{
	// Remove the panel from the sizer (if the panel exists)
	if (editAerodynamics || editBrakes || editDifferential || editDrivetrain
		|| editEngine || editMass || editSuspension || editTires ||
		editIteration)
		sizer->Remove(0);
	sizer->Layout();

	// Delete all of the controls
	delete editAerodynamics;
	delete editBrakes;
	delete editDifferential;
	delete editDrivetrain;
	delete editEngine;
	delete editMass;
	delete editSuspension;
	delete editTires;

	delete editIteration;

	// Set all of the pointers to NULL
	editAerodynamics = NULL;
	editBrakes = NULL;
	editDifferential = NULL;
	editDrivetrain = NULL;
	editEngine = NULL;
	editMass = NULL;
	editSuspension = NULL;
	editTires = NULL;

	editIteration = NULL;

	// When we remove these controls, we no longer want the helpr orb to be active
	// in this car's renderer.  This makes the orb go inactive if we click off of
	// the suspension page.
	if (currentObject)
	{
		// This is only necessary if the current object is TYPE_CAR
		if (currentObject->GetType() == GuiObject::TypeCar)
		{
			// Deactive the helper orb
			static_cast<CarRenderer*>(currentObject->GetNotebookTab())->DeactivateHelperOrb();

			// Update the visual display
			currentObject->UpdateDisplay();
		}
	}
}