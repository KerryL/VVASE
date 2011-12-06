/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editMassPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EditMassPanel class.  This
//				 class is used to edit the hard points on a particular corner of the
//				 car.
// History:

// CarDesigner headers
#include "vCar/massProperties.h"
#include "gui/renderer/carRenderer.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editMassPanel.h"
#include "vUtilities/convert.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			EditMassPanel
// Function:		EditMassPanel
//
// Description:		Constructor for EditMassPanel class.
//
// Input Arguments:
//		_parent		= EditPanel&, reference to this object's owner
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
EditMassPanel::EditMassPanel(EditPanel &_parent, wxWindowID id,
								 const wxPoint& pos, const wxSize& size) :
								 wxScrolledWindow(&_parent, id, pos, size),
								 parent(_parent)
{
	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EditMassPanel
// Function:		EditMassPanel
//
// Description:		Destructor for EditMassPanel class.
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
EditMassPanel::~EditMassPanel()
{
}

//==========================================================================
// Class:			EditMassPanel
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
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
BEGIN_EVENT_TABLE(EditMassPanel, wxPanel)
	EVT_TEXT(wxID_ANY,	EditMassPanel::TextBoxEditEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditMassPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_currentMassProperties	= MassProperties* pointing to the associated
//								  mass properties
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditMassPanel::UpdateInformation(MassProperties *_currentMassProperties)
{
	// Update the class member
	currentMassProperties = _currentMassProperties;

	// Update the mass
	mass->ChangeValue(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertMass(currentMassProperties->mass)));

	// Update the inertia
	ixx->ChangeValue(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertMass(currentMassProperties->ixx)));
	iyy->ChangeValue(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertMass(currentMassProperties->iyy)));
	izz->ChangeValue(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertMass(currentMassProperties->izz)));
	ixy->ChangeValue(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertMass(currentMassProperties->ixy)));
	iyx->SetLabel(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertMass(currentMassProperties->ixy)));
	ixz->ChangeValue(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertMass(currentMassProperties->ixz)));
	izx->SetLabel(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertMass(currentMassProperties->ixz)));
	iyz->ChangeValue(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertMass(currentMassProperties->iyz)));
	izy->SetLabel(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertMass(currentMassProperties->iyz)));

	// Update the center of gravity
	centerOfGravityX->ChangeValue(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertDistance(currentMassProperties->centerOfGravity.x)));
	centerOfGravityY->ChangeValue(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertDistance(currentMassProperties->centerOfGravity.y)));
	centerOfGravityZ->ChangeValue(Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertDistance(currentMassProperties->centerOfGravity.z)));

	// Update the units
	inertiaUnitsLabel->SetLabel('(' + Convert::GetInstance().GetUnitType(Convert::UnitTypeInertia) + ')');
	massUnitsLabel->SetLabel('(' + Convert::GetInstance().GetUnitType(Convert::UnitTypeMass) + ')');
	cogUnitsLabel->SetLabel('(' + Convert::GetInstance().GetUnitType(Convert::UnitTypeDistance) + ')');

	// Update sizers
	Layout();
}

//==========================================================================
// Class:			EditMassPanel
// Function:		CreateControls
//
// Description:		Creates the controls for this panel.
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
void EditMassPanel::CreateControls()
{
	// Enable scrolling
	SetScrollRate(1, 1);
	
	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 1, wxGROW | wxALL, 5);

	// Create the inertia inputs
	wxBoxSizer *inertiaSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *inertiaCaption = new wxStaticText(this, wxID_ANY,
		_T("Sprung Mass Inertia Tensor (w.r.t. CG)"));
	inertiaUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);

	inertiaSizer->Add(inertiaCaption, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	inertiaSizer->Add(inertiaUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

	// Create sizer of input section
	wxFlexGridSizer *inertiaInputSizer = new wxFlexGridSizer(5, 3, 3);

	// Create the text boxes
	ixx = new wxTextCtrl(this, TextBoxIxx);
	iyy = new wxTextCtrl(this, TextBoxIyy);
	izz = new wxTextCtrl(this, TextBoxIzz);
	ixy = new wxTextCtrl(this, TextBoxIxy);
	ixz = new wxTextCtrl(this, TextBoxIxz);
	iyz = new wxTextCtrl(this, TextBoxIyz);

	// Create the static text controls
	iyx = new wxStaticText(this, wxID_ANY, wxEmptyString);
	izx = new wxStaticText(this, wxID_ANY, wxEmptyString);
	izy = new wxStaticText(this, wxID_ANY, wxEmptyString);

	// First row
	inertiaInputSizer->AddSpacer(-1);
	inertiaInputSizer->Add(ixx, 0, wxEXPAND);
	inertiaInputSizer->Add(ixy, 0, wxEXPAND);
	inertiaInputSizer->Add(ixz, 0, wxEXPAND);
	inertiaInputSizer->AddSpacer(-1);

	// Second row
	inertiaInputSizer->AddSpacer(-1);
	inertiaInputSizer->Add(iyx, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(iyy, 0, wxEXPAND);
	inertiaInputSizer->Add(iyz, 0, wxEXPAND);
	inertiaInputSizer->AddSpacer(-1);

	// Third row
	inertiaInputSizer->AddSpacer(-1);
	inertiaInputSizer->Add(izx, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(izy, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(izz, 0, wxEXPAND);
	inertiaInputSizer->AddSpacer(-1);
	
	// Blank row for space
	int spaceSize(15);
	inertiaInputSizer->AddSpacer(spaceSize);
	inertiaInputSizer->AddSpacer(spaceSize);
	inertiaInputSizer->AddSpacer(spaceSize);
	inertiaInputSizer->AddSpacer(spaceSize);
	inertiaInputSizer->AddSpacer(spaceSize);

	// Create the mass input
	wxStaticText *massLabel = new wxStaticText(this, wxID_ANY, _T("Mass"));
	mass = new wxTextCtrl(this, TextBoxMass);
	massUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);

	inertiaInputSizer->Add(massLabel, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(mass, 0, wxEXPAND);
	inertiaInputSizer->Add(massUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->AddSpacer(-1);
	inertiaInputSizer->AddSpacer(-1);

	// Create the center of gravity inputs
	wxStaticText *cogLabel = new wxStaticText(this, wxID_ANY, _T("CG"));
	centerOfGravityX = new wxTextCtrl(this, TextBoxCenterOfGravityX);
	centerOfGravityY = new wxTextCtrl(this, TextBoxCenterOfGravityY);
	centerOfGravityZ = new wxTextCtrl(this, TextBoxCenterOfGravityZ);
	cogUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);

	inertiaInputSizer->Add(cogLabel, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(centerOfGravityX, 0, wxEXPAND);
	inertiaInputSizer->Add(centerOfGravityY, 0, wxEXPAND);
	inertiaInputSizer->Add(centerOfGravityZ, 0, wxEXPAND);
	inertiaInputSizer->Add(cogUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);
	
	// Make the middle columns growable
	inertiaInputSizer->AddGrowableCol(1, 1);
	inertiaInputSizer->AddGrowableCol(2, 1);
	inertiaInputSizer->AddGrowableCol(3, 1);

	// Add the sizers to the main sizer
	mainSizer->Add(inertiaSizer);
	mainSizer->Add(inertiaInputSizer, 1, wxEXPAND);

	// Set the text box minimum widths
	// Use reasonable values in default units for an average car, add a negative sign, and format
	// as requested to calculate the proper widths
	int minInertiaWidth, minMassWidth, minDistanceWidth, minWidth;
	GetTextExtent(Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertInertia(-850000.0)), &minInertiaWidth, NULL);
	GetTextExtent(Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertMass(-8000.0)), &minMassWidth, NULL);
	GetTextExtent(Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertDistance(-200.0)), &minDistanceWidth, NULL);
	minWidth = std::max(minInertiaWidth, std::max(minMassWidth, minDistanceWidth));

	ixx->SetMinSize(wxSize(minWidth, -1));
	iyy->SetMinSize(wxSize(minWidth, -1));
	izz->SetMinSize(wxSize(minWidth, -1));
	ixy->SetMinSize(wxSize(minWidth, -1));
	ixz->SetMinSize(wxSize(minWidth, -1));
	iyz->SetMinSize(wxSize(minWidth, -1));
	mass->SetMinSize(wxSize(minWidth, -1));
	centerOfGravityX->SetMinSize(wxSize(minWidth, -1));
	centerOfGravityY->SetMinSize(wxSize(minWidth, -1));
	centerOfGravityZ->SetMinSize(wxSize(minWidth, -1));

	// Assign the top level sizer to the dialog
	SetSizer(topSizer);
}

//==========================================================================
// Class:			EditMassPanel
// Function:		TextBoxEditEvent
//
// Description:		Event that fires when any text box changes.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditMassPanel::TextBoxEditEvent(wxCommandEvent &event)
{
	// The place where the data will be written
	double *dataLocation = NULL;

	// We also need to know what kind of data we're retrieving
	Convert::UnitType units;

	// A pointer to the text box
	wxTextCtrl *textBox = NULL;

	// Take different action depending on the event ID
	switch (event.GetId())
	{
	case TextBoxIxx:
		// Get the text box, the location to write it, and the units
		textBox = ixx;
		dataLocation = &currentMassProperties->ixx;
		units = Convert::UnitTypeInertia;
		break;

	case TextBoxIyy:
		// Get the new data, the location to write it, and the units
		textBox = iyy;
		dataLocation = &currentMassProperties->iyy;
		units = Convert::UnitTypeInertia;
		break;

	case TextBoxIzz:
		// Get the new data, the location to write it, and the units
		textBox = izz;
		dataLocation = &currentMassProperties->izz;
		units = Convert::UnitTypeInertia;
		break;

	case TextBoxIxy:
		// Get the new data, the location to write it, and the units
		textBox = ixy;
		dataLocation = &currentMassProperties->ixy;
		units = Convert::UnitTypeInertia;
		break;

	case TextBoxIxz:
		// Get the new data, the location to write it, and the units
		textBox = ixz;
		dataLocation = &currentMassProperties->ixz;
		units = Convert::UnitTypeInertia;
		break;

	case TextBoxIyz:
		// Get the new data, the location to write it, and the units
		textBox = iyz;
		dataLocation = &currentMassProperties->iyz;
		units = Convert::UnitTypeInertia;
		break;

	case TextBoxMass:
		// Get the new data, the location to write it, and the units
		textBox = mass;
		dataLocation = &currentMassProperties->mass;
		units = Convert::UnitTypeMass;
		break;

	case TextBoxCenterOfGravityX:
		// Get the new data, the location to write it, and the units
		textBox = centerOfGravityX;
		dataLocation = &currentMassProperties->centerOfGravity.x;
		units = Convert::UnitTypeDistance;
		break;

	case TextBoxCenterOfGravityY:
		// Get the new data, the location to write it, and the units
		textBox = centerOfGravityY;
		dataLocation = &currentMassProperties->centerOfGravity.y;
		units = Convert::UnitTypeDistance;
		break;

	case TextBoxCenterOfGravityZ:
		// Get the new data, the location to write it, and the units
		textBox = centerOfGravityZ;
		dataLocation = &currentMassProperties->centerOfGravity.z;
		units = Convert::UnitTypeDistance;
		break;

	default:
		// Don't do anything if we don't recognize the ID
		return;
		break;
	}

	// Make sure the value is numeric
	wxString valueString = textBox->GetValue();
	double value;
	if (!valueString.ToDouble(&value))
		return;

	// Add the operation to the undo/redo stack
	parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		dataLocation);

	// Get a lock on the car
	wxMutex *mutex = parent.GetCurrentMutex();
	mutex->Lock();

	// Update the mass properties object
	*dataLocation = Convert::GetInstance().Read(value, units);

	// Unlock the car
	mutex->Unlock();

	// Tell the car object that it was modified
	parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetMainFrame().UpdateAnalysis();

	// If one of the off-diagonal inertias was updated, we need to change the
	// corresponding value on the lower side of the diagonal
	iyx->SetLabel(Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertMass(currentMassProperties->ixy)));
	izx->SetLabel(Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertMass(currentMassProperties->ixz)));
	izy->SetLabel(Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertMass(currentMassProperties->iyz)));

	event.Skip();
}
