/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editPanel.cpp
// Date:  2/10/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditPanel class.  This
//        class is used to edit the car parameters.  Different panels
//        are displayed for editing different sub-systems.

// Local headers
#include "VVASE/core/car/car.h"
#include "VVASE/core/car/subsystems/tireSet.h"
#include "VVASE/core/car/subsystems/brakes.h"
#include "VVASE/core/car/subsystems/massProperties.h"
#include "VVASE/gui/guiObject.h"
#include "../../Sweep.h"
#include "../../guiCar.h"
#include "VVASE/gui/renderer/carRenderer.h"
#include "VVASE/gui/components/mainFrame.h"
#include "VVASE/gui/components/mainTree.h"
#include "editPanel.h"
#include "guiCar/editAerodynamicsPanel.h"
#include "guiCar/editBrakesPanel.h"
#include "guiCar/editDifferentialPanel.h"
#include "guiCar/editDrivetrainPanel.h"
#include "guiCar/editEnginePanel.h"
#include "guiCar/editSuspensionNotebook.h"
#include "guiCar/editMassPanel.h"
#include "guiCar/editTiresPanel.h"
#include "sweep/editSweepNotebook.h"
#include "VVASE/core/utilities/debugger.h"

namespace VVASE
{

//==========================================================================
// Class:			EditPanel
// Function:		EditPanel
//
// Description:		Constructor for EditPanel class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		mainFrame	= MainFrame&, reference to this object's owner
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
EditPanel::EditPanel(MainFrame &mainFrame, wxWindowID id, const wxPoint& pos,
	const wxSize& size) : wxPanel(&mainFrame, id, pos, size), mainFrame(mainFrame)
{
	systemsTree = mainFrame.GetSystemsTree();

	currentType = GuiObject::TypeNone;
	currentObject = nullptr;

	editAerodynamics = nullptr;
	editBrakes = nullptr;
	editDifferential = nullptr;
	editDrivetrain = nullptr;
	editEngine = nullptr;
	editMass = nullptr;
	editSuspension = nullptr;
	editTires = nullptr;

	editSweep = nullptr;

	carMutex = nullptr;

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
void EditPanel::UpdateInformation()
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

	carMutex = nullptr;

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
			MutexLocker lock(static_cast<GuiCar*>(currentObject)->GetOriginalCar().GetMutex());

			if (editAerodynamics)
			{
			}
			else if (editBrakes)
				editBrakes->UpdateInformation(static_cast<GuiCar*>(currentObject)->GetOriginalCar().GetSubsystem<Brakes>());
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
				editMass->UpdateInformation(static_cast<GuiCar*>(currentObject)->GetOriginalCar().GetSubsystem<MassProperties>());
			else if (editSuspension)
				editSuspension->UpdateInformation(&static_cast<GuiCar*>(currentObject)->GetOriginalCar());
			else if (editTires)
				editTires->UpdateInformation(static_cast<GuiCar*>(currentObject)->GetOriginalCar().GetSubsystem<TireSet>());
		}

		break;

	case GuiObject::TypeSweep:
		editSweep->UpdateInformation(static_cast<Sweep*>(currentObject));
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

	case GuiObject::TypeSweep:
		editSweep = new EditSweepNotebook(*this, wxID_ANY,
			wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);

		sizer->Add(editSweep, 1, wxEXPAND);
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
void EditPanel::DeleteAllControls()
{
	// Remove the panel from the sizer (if the panel exists)
	if (editAerodynamics || editBrakes || editDifferential || editDrivetrain
		|| editEngine || editMass || editSuspension || editTires ||
		editSweep)
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

	delete editSweep;

	// Set all of the pointers to nullptr
	editAerodynamics = nullptr;
	editBrakes = nullptr;
	editDifferential = nullptr;
	editDrivetrain = nullptr;
	editEngine = nullptr;
	editMass = nullptr;
	editSuspension = nullptr;
	editTires = nullptr;

	editSweep = nullptr;

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

}// namespace VVASE
