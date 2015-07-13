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
	systemsTree = mainFrame.GetSystemsTree();

	currentType = GuiObject::TypeNone;
	currentObject = NULL;

	editAerodynamics = NULL;
	editBrakes = NULL;
	editDifferential = NULL;
	editDrivetrain = NULL;
	editEngine = NULL;
	editMass = NULL;
	editSuspension = NULL;
	editTires = NULL;

	editIteration = NULL;

	carMutex = NULL;

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
	if (currentObject)
		UpdateInformation(currentObject);
}

//==========================================================================
// Class:			EditPanel
// Function:		UpdateInformation
//
// Description:		Updates the information in this notebook.
//
// Input Arguments:
//		currentObject	= GuiObject* pointing to the currenly active object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditPanel::UpdateInformation(GuiObject *currentObject)
{
	this->currentObject = currentObject;

	if (!currentObject)
	{
		currentType = GuiObject::TypeNone;
		DeleteAllControls();
		return;
	}

	carMutex = NULL;

	bool ignoreSystemsTree = true;
	wxTreeItemId tempTreeID;
	if (systemsTree->GetSelectedItem(&tempTreeID) == currentObject)
	{
		// The object was a match, but let's make sure the tree item ID is valid
		if (tempTreeID.IsOk())
			ignoreSystemsTree = false;
	}

	// Compare the current type with the type of the new object to decide if
	// we need to delete the existing pages
	if (currentType != currentObject->GetType() || (!ignoreSystemsTree && tempTreeID != currentTreeID))
	{
		currentType = currentObject->GetType();
		currentTreeID = tempTreeID;
		CreateControls(ignoreSystemsTree);
	}

	switch (currentType)
	{
	case GuiObject::TypeCar:
		{
			carMutex = &(static_cast<GuiCar*>(currentObject)->GetOriginalCar().GetMutex());
			wxMutexLocker lock(*carMutex);

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
				editSuspension->UpdateInformation(&static_cast<GuiCar*>(currentObject)->GetOriginalCar());
			else if (editTires)
				editTires->UpdateInformation(static_cast<GuiCar*>(currentObject)->GetOriginalCar().tires);
		}

		break;

	case GuiObject::TypeIteration:
		editIteration->UpdateInformation(static_cast<Iteration*>(currentObject));
		break;

	case GuiObject::TypeOptimization:
	case GuiObject::TypeNone:
		break;

	// Fail on unknown cases to prevent forgetting any
	default:
		assert(0);
		break;
	}
	
	Layout();
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
	DeleteAllControls();

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
				editBrakes = new EditBrakesPanel(*this, wxID_ANY,
					wxDefaultPosition, wxDefaultSize);
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
				editMass = new EditMassPanel(*this, wxID_ANY,
					wxDefaultPosition, wxDefaultSize);
				sizer->Add(editMass, 1, wxEXPAND);
			}
			else if (systemsTree->GetItemType(currentTreeID) == GuiCar::SubsystemSuspension)
			{
				editSuspension = new EditSuspensionNotebook(*this, wxID_ANY,
					wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
				sizer->Add(editSuspension, 1, wxEXPAND);
			}
			else if (systemsTree->GetItemType(currentTreeID) == GuiCar::SubsystemTires)
			{
				editTires = new EditTiresPanel(*this, wxID_ANY,
					wxDefaultPosition, wxDefaultSize);
				sizer->Add(editTires, 1, wxEXPAND);
			}
		}

		break;

	case GuiObject::TypeIteration:
		editIteration = new EditIterationNotebook(*this, wxID_ANY,
			wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);

		sizer->Add(editIteration, 1, wxEXPAND);
		break;

	// Unused cases
	case GuiObject::TypeOptimization:
	case GuiObject::TypeNone:
		break;

	default:
		assert(0);
		break;
	}

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
		if (currentObject->GetType() == GuiObject::TypeCar)
		{
			static_cast<CarRenderer*>(currentObject->GetNotebookTab())->DeactivateHelperOrb();
			currentObject->UpdateDisplay();
		}
	}
}