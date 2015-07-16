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
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			EditMassPanel
// Function:		EditMassPanel
//
// Description:		Constructor for EditMassPanel class.
//
// Input Arguments:
//		parent		= EditPanel&, reference to this object's owner
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
EditMassPanel::EditMassPanel(EditPanel &parent, wxWindowID id,
	const wxPoint& pos, const wxSize& size)
	: wxScrolledWindow(&parent, id, pos, size), parent(parent)
{

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
void EditMassPanel::UpdateInformation(MassProperties *currentMassProperties)
{
	this->currentMassProperties = currentMassProperties;

	// Update the mass
	mass->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->mass)));

	// Update the inertia
	ixx->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->ixx)));
	iyy->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->iyy)));
	izz->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->izz)));
	ixy->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->ixy)));
	iyx->SetLabel(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->ixy)));
	ixz->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->ixz)));
	izx->SetLabel(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->ixz)));
	iyz->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->iyz)));
	izy->SetLabel(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->iyz)));

	// Update the center of gravity
	centerOfGravityX->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->centerOfGravity.x)));
	centerOfGravityY->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->centerOfGravity.y)));
	centerOfGravityZ->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->centerOfGravity.z)));

	// Update the units
	inertiaUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeInertia) + ')');
	massUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeMass) + ')');
	cogUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance) + ')');

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
	GetTextExtent(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertInertiaOutput(-850000.0)), &minInertiaWidth, NULL);
	GetTextExtent(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertMassOutput(-8000.0)), &minMassWidth, NULL);
	GetTextExtent(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(-200.0)), &minDistanceWidth, NULL);
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
	double *dataLocation = NULL;
	UnitConverter::UnitType units;
	wxTextCtrl *textBox = NULL;

	switch (event.GetId())
	{
	case TextBoxIxx:
		textBox = ixx;
		dataLocation = &currentMassProperties->ixx;
		units = UnitConverter::UnitTypeInertia;
		break;

	case TextBoxIyy:
		textBox = iyy;
		dataLocation = &currentMassProperties->iyy;
		units = UnitConverter::UnitTypeInertia;
		break;

	case TextBoxIzz:
		textBox = izz;
		dataLocation = &currentMassProperties->izz;
		units = UnitConverter::UnitTypeInertia;
		break;

	case TextBoxIxy:
		textBox = ixy;
		dataLocation = &currentMassProperties->ixy;
		units = UnitConverter::UnitTypeInertia;
		break;

	case TextBoxIxz:
		textBox = ixz;
		dataLocation = &currentMassProperties->ixz;
		units = UnitConverter::UnitTypeInertia;
		break;

	case TextBoxIyz:
		textBox = iyz;
		dataLocation = &currentMassProperties->iyz;
		units = UnitConverter::UnitTypeInertia;
		break;

	case TextBoxMass:
		textBox = mass;
		dataLocation = &currentMassProperties->mass;
		units = UnitConverter::UnitTypeMass;
		break;

	case TextBoxCenterOfGravityX:
		textBox = centerOfGravityX;
		dataLocation = &currentMassProperties->centerOfGravity.x;
		units = UnitConverter::UnitTypeDistance;
		break;

	case TextBoxCenterOfGravityY:
		textBox = centerOfGravityY;
		dataLocation = &currentMassProperties->centerOfGravity.y;
		units = UnitConverter::UnitTypeDistance;
		break;

	case TextBoxCenterOfGravityZ:
		textBox = centerOfGravityZ;
		dataLocation = &currentMassProperties->centerOfGravity.z;
		units = UnitConverter::UnitTypeDistance;
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

	parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		dataLocation);

	wxMutex *mutex = parent.GetCurrentMutex();
	mutex->Lock();
	*dataLocation = UnitConverter::GetInstance().ConvertInput(value, units);
	mutex->Unlock();

	parent.GetCurrentObject()->SetModified();
	parent.GetMainFrame().UpdateAnalysis();

	// If one of the off-diagonal inertias was updated, we need to change the
	// corresponding value on the lower side of the diagonal
	iyx->SetLabel(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->ixy)));
	izx->SetLabel(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->ixz)));
	izy->SetLabel(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->iyz)));

	event.Skip();
}
