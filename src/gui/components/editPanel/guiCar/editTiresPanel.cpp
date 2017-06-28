/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  editTiresPanel.cpp
// Date:  2/10/2009
// Author:  K. Loux
// Desc:  Contains the class definition for the EditTiresPanel class.  This
//        class is used to edit the hard points on a particular corner of the
//        car.

// Local headers
#include "vCar/tire.h"
#include "vCar/tireSet.h"
#include "gui/renderer/carRenderer.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editTiresPanel.h"
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			EditTiresPanel
// Function:		EditTiresPanel
//
// Description:		Constructor for EditTiresPanel class.
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
EditTiresPanel::EditTiresPanel(EditPanel &parent, wxWindowID id,
	const wxPoint& pos, const wxSize& size)
	: wxScrolledWindow(&parent, id, pos, size), parent(parent)
{
	CreateControls();
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		EditTiresPanel
//
// Description:		Destructor for EditTiresPanel class.
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
EditTiresPanel::~EditTiresPanel()
{
}

//==========================================================================
// Class:			EditTiresPanel
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
BEGIN_EVENT_TABLE(EditTiresPanel, wxPanel)
	EVT_TEXT(wxID_ANY, EditTiresPanel::TextBoxChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditTiresPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		currentTireSet	= TireSet* pointing to the associated tire set
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditTiresPanel::UpdateInformation(TireSet *currentTireSet)
{
	this->currentTireSet = currentTireSet;

	// FIXME:  Handle symmetry

	// Update the text boxes
	rightFrontTireDiameter->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->rightFront->diameter)));
	rightFrontTireWidth->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->rightFront->width)));
	rightFrontTireStiffness->ChangeValue(UnitConverter::GetInstance().FormatNumber(ConvertSpringOutput(
		currentTireSet->rightFront->stiffness)));

	leftFrontTireDiameter->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->leftFront->diameter)));
	leftFrontTireWidth->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->leftFront->width)));
	leftFrontTireStiffness->ChangeValue(UnitConverter::GetInstance().FormatNumber(ConvertSpringOutput(
		currentTireSet->leftFront->stiffness)));

	rightRearTireDiameter->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->rightRear->diameter)));
	rightRearTireWidth->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->rightRear->width)));
	rightRearTireStiffness->ChangeValue(UnitConverter::GetInstance().FormatNumber(ConvertSpringOutput(
		currentTireSet->rightRear->stiffness)));

	leftRearTireDiameter->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->leftRear->diameter)));
	leftRearTireWidth->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->leftRear->width)));
	leftRearTireStiffness->ChangeValue(UnitConverter::GetInstance().FormatNumber(ConvertSpringOutput(
		currentTireSet->leftRear->stiffness)));

	// And their units
	rightFrontDiameterUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));
	rightFrontWidthUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));
	rightFrontStiffnessUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeForce)
		+ _T("/") + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));

	leftFrontDiameterUnitsLabel->SetLabel(rightFrontDiameterUnitsLabel->GetLabel());
	leftFrontWidthUnitsLabel->SetLabel(rightFrontWidthUnitsLabel->GetLabel());
	leftFrontStiffnessUnitsLabel->SetLabel(rightFrontStiffnessUnitsLabel->GetLabel());

	rightRearDiameterUnitsLabel->SetLabel(rightFrontDiameterUnitsLabel->GetLabel());
	rightRearWidthUnitsLabel->SetLabel(rightFrontWidthUnitsLabel->GetLabel());
	rightRearStiffnessUnitsLabel->SetLabel(rightFrontStiffnessUnitsLabel->GetLabel());

	leftRearDiameterUnitsLabel->SetLabel(rightFrontDiameterUnitsLabel->GetLabel());
	leftRearWidthUnitsLabel->SetLabel(rightFrontWidthUnitsLabel->GetLabel());
	leftRearStiffnessUnitsLabel->SetLabel(rightFrontStiffnessUnitsLabel->GetLabel());

	// Update the sizers
	/*int minWidth;
	GetTextExtent(leftRearWidthUnitsLabel->GetLabel(), &minWidth, NULL);
	leftRearWidthUnitsLabel->SetMinSize(wxSize(minWidth, -1));
	Layout();*/
}

//==========================================================================
// Class:			EditTiresPanel
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
void EditTiresPanel::CreateControls()
{
	// Enable scrolling
	SetScrollRate(10, 10);

	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxFlexGridSizer *mainSizer = new wxFlexGridSizer(3, 5, 5);
	topSizer->Add(mainSizer, 1, wxALL | wxEXPAND, 5);

	// Spacing between each wheel
	int spacing(10);

	// Create the text controls
	// Right front
	wxStaticText *rightFrontLabel = new wxStaticText(this, wxID_ANY, _T("Right Front"));
	mainSizer->Add(rightFrontLabel, wxALIGN_CENTER_VERTICAL);
	mainSizer->AddSpacer(-1);
	mainSizer->AddSpacer(-1);

	// Diameter
	wxStaticText *rightFrontDiameterLabel = new wxStaticText(this, wxID_ANY, _T("Diameter"));
	rightFrontDiameterUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	rightFrontTireDiameter = new wxTextCtrl(this, TextBoxRightFrontTireDiameter);

	mainSizer->Add(rightFrontDiameterLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(rightFrontTireDiameter, 0, wxEXPAND);
	mainSizer->Add(rightFrontDiameterUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Width
	wxStaticText *rightFrontWidthLabel = new wxStaticText(this, wxID_ANY, _T("Width"));
	rightFrontWidthUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	rightFrontTireWidth = new wxTextCtrl(this, TextBoxRightFrontTireWidth);

	mainSizer->Add(rightFrontWidthLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(rightFrontTireWidth, 0, wxEXPAND);
	mainSizer->Add(rightFrontWidthUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Stiffness
	wxStaticText *rightFrontStiffnessLabel = new wxStaticText(this, wxID_ANY, _T("Stiffness"));
	rightFrontStiffnessUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	rightFrontTireStiffness = new wxTextCtrl(this, TextBoxRightFrontStiffness);

	mainSizer->Add(rightFrontStiffnessLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(rightFrontTireStiffness, 0, wxEXPAND);
	mainSizer->Add(rightFrontStiffnessUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Add space before the next corner
	mainSizer->AddSpacer(spacing);
	mainSizer->AddSpacer(spacing);
	mainSizer->AddSpacer(spacing);

	// Left front
	wxStaticText *leftFrontLabel = new wxStaticText(this, wxID_ANY, _T("Left Front"));
	mainSizer->Add(leftFrontLabel, wxALIGN_CENTER_VERTICAL);
	mainSizer->AddSpacer(-1);
	mainSizer->AddSpacer(-1);

	// Diameter
	wxStaticText *leftFrontDiameterLabel = new wxStaticText(this, wxID_ANY, _T("Diameter"));
	leftFrontDiameterUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	leftFrontTireDiameter = new wxTextCtrl(this, TextBoxLeftFrontTireDiameter);

	mainSizer->Add(leftFrontDiameterLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(leftFrontTireDiameter, 0, wxEXPAND);
	mainSizer->Add(leftFrontDiameterUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Width
	wxStaticText *leftFrontWidthLabel = new wxStaticText(this, wxID_ANY, _T("Width"));
	leftFrontWidthUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	leftFrontTireWidth = new wxTextCtrl(this, TextBoxLeftFrontTireWidth);

	mainSizer->Add(leftFrontWidthLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(leftFrontTireWidth, 0, wxEXPAND);
	mainSizer->Add(leftFrontWidthUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Stiffness
	wxStaticText *leftFrontStiffnessLabel = new wxStaticText(this, wxID_ANY, _T("Stiffness"));
	leftFrontStiffnessUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	leftFrontTireStiffness = new wxTextCtrl(this, TextBoxLeftFrontStiffness);

	mainSizer->Add(leftFrontStiffnessLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(leftFrontTireStiffness, 0, wxEXPAND);
	mainSizer->Add(leftFrontStiffnessUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Add space before the next corner
	mainSizer->AddSpacer(spacing);
	mainSizer->AddSpacer(spacing);
	mainSizer->AddSpacer(spacing);

	// Right rear
	wxStaticText *rightRearLabel = new wxStaticText(this, wxID_ANY, _T("Right Rear"));
	mainSizer->Add(rightRearLabel, wxALIGN_CENTER_VERTICAL);
	mainSizer->AddSpacer(-1);
	mainSizer->AddSpacer(-1);

	// Diameter
	wxStaticText *rightRearDiameterLabel = new wxStaticText(this, wxID_ANY, _T("Diameter"));
	rightRearDiameterUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	rightRearTireDiameter = new wxTextCtrl(this, TextBoxRightRearTireDiameter);

	mainSizer->Add(rightRearDiameterLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(rightRearTireDiameter, 0, wxEXPAND);
	mainSizer->Add(rightRearDiameterUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Width
	wxStaticText *rightRearWidthLabel = new wxStaticText(this, wxID_ANY, _T("Width"));
	rightRearWidthUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	rightRearTireWidth = new wxTextCtrl(this, TextBoxRightRearTireWidth);

	mainSizer->Add(rightRearWidthLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(rightRearTireWidth, 0, wxEXPAND);
	mainSizer->Add(rightRearWidthUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Stiffness
	wxStaticText *rightRearStiffnessLabel = new wxStaticText(this, wxID_ANY, _T("Stiffness"));
	rightRearStiffnessUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	rightRearTireStiffness = new wxTextCtrl(this, TextBoxRightRearStiffness);

	mainSizer->Add(rightRearStiffnessLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(rightRearTireStiffness, 0, wxEXPAND);
	mainSizer->Add(rightRearStiffnessUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Add space before the next corner
	mainSizer->AddSpacer(spacing);
	mainSizer->AddSpacer(spacing);
	mainSizer->AddSpacer(spacing);

	// Left rear
	wxStaticText *leftRearLabel = new wxStaticText(this, wxID_ANY, _T("Left Rear"));
	mainSizer->Add(leftRearLabel, wxALIGN_CENTER_VERTICAL);
	mainSizer->AddSpacer(-1);
	mainSizer->AddSpacer(-1);

	// Diameter
	wxStaticText *leftRearDiameterLabel = new wxStaticText(this, wxID_ANY, _T("Diameter"));
	leftRearDiameterUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	leftRearTireDiameter = new wxTextCtrl(this, TextBoxLeftRearTireDiameter);

	mainSizer->Add(leftRearDiameterLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(leftRearTireDiameter, 0, wxEXPAND);
	mainSizer->Add(leftRearDiameterUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Width
	wxStaticText *leftRearWidthLabel = new wxStaticText(this, wxID_ANY, _T("Width"));
	leftRearWidthUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	leftRearTireWidth = new wxTextCtrl(this, TextBoxLeftRearTireWidth);

	mainSizer->Add(leftRearWidthLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(leftRearTireWidth, 0, wxEXPAND);
	mainSizer->Add(leftRearWidthUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Stiffness
	wxStaticText *leftRearStiffnessLabel = new wxStaticText(this, wxID_ANY, _T("Stiffness"));
	leftRearStiffnessUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	leftRearTireStiffness = new wxTextCtrl(this, TextBoxLeftRearStiffness);

	mainSizer->Add(leftRearStiffnessLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(leftRearTireStiffness, 0, wxEXPAND);
	mainSizer->Add(leftRearStiffnessUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Adjust text box minimum size
	// Use number larger than actual anticipated value for these boxes to determine appropriate size
	int minWidth;
	GetTextExtent(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(-400000.0)), &minWidth, NULL);
	rightFrontTireDiameter->SetMinSize(wxSize(minWidth, -1));
	rightFrontTireWidth->SetMinSize(wxSize(minWidth, -1));
	rightFrontTireStiffness->SetMinSize(wxSize(minWidth, -1));
	leftFrontTireDiameter->SetMinSize(wxSize(minWidth, -1));
	leftFrontTireWidth->SetMinSize(wxSize(minWidth, -1));
	leftFrontTireStiffness->SetMinSize(wxSize(minWidth, -1));
	rightRearTireDiameter->SetMinSize(wxSize(minWidth, -1));
	rightRearTireWidth->SetMinSize(wxSize(minWidth, -1));
	rightRearTireStiffness->SetMinSize(wxSize(minWidth, -1));
	leftRearTireDiameter->SetMinSize(wxSize(minWidth, -1));
	leftRearTireWidth->SetMinSize(wxSize(minWidth, -1));
	leftRearTireStiffness->SetMinSize(wxSize(minWidth, -1));

	// Assign the top level sizer to the dialog
	SetSizer(topSizer);
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		TextBoxChangeEvent
//
// Description:		Event that fires when a text box changes.
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
void EditTiresPanel::TextBoxChangeEvent(wxCommandEvent &event)
{
	double *dataLocation = NULL;
	wxTextCtrl *textBox = NULL;

	switch (event.GetId())
	{
	case TextBoxRightFrontTireDiameter:
		textBox = rightFrontTireDiameter;
		dataLocation = &currentTireSet->rightFront->diameter;
		break;

	case TextBoxRightFrontTireWidth:
		textBox = rightFrontTireWidth;
		dataLocation = &currentTireSet->rightFront->width;
		break;

	case TextBoxRightFrontStiffness:
		textBox = rightFrontTireStiffness;
		dataLocation = &currentTireSet->rightFront->stiffness;
		break;

	case TextBoxLeftFrontTireDiameter:
		textBox = leftFrontTireDiameter;
		dataLocation = &currentTireSet->leftFront->diameter;
		break;

	case TextBoxLeftFrontTireWidth:
		textBox = leftFrontTireWidth;
		dataLocation = &currentTireSet->leftFront->width;
		break;

	case TextBoxLeftFrontStiffness:
		textBox = leftFrontTireStiffness;
		dataLocation = &currentTireSet->leftFront->stiffness;
		break;

	case TextBoxRightRearTireDiameter:
		textBox = rightRearTireDiameter;
		dataLocation = &currentTireSet->rightRear->diameter;
		break;

	case TextBoxRightRearTireWidth:
		textBox = rightRearTireWidth;
		dataLocation = &currentTireSet->rightRear->width;
		break;

	case TextBoxRightRearStiffness:
		textBox = rightRearTireStiffness;
		dataLocation = &currentTireSet->rightRear->stiffness;
		break;

	case TextBoxLeftRearTireDiameter:
		textBox = leftRearTireDiameter;
		dataLocation = &currentTireSet->leftRear->diameter;
		break;

	case TextBoxLeftRearTireWidth:
		textBox = leftRearTireWidth;
		dataLocation = &currentTireSet->leftRear->width;
		break;

	case TextBoxLeftRearStiffness:
		textBox = leftRearTireStiffness;
		dataLocation = &currentTireSet->leftRear->stiffness;
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
	if (event.GetId() == TextBoxRightFrontStiffness ||
		event.GetId() == TextBoxLeftFrontStiffness ||
		event.GetId() == TextBoxRightRearStiffness ||
		event.GetId() == TextBoxLeftRearStiffness)
		*dataLocation = ConvertSpringInput(value);
	else
		*dataLocation = UnitConverter::GetInstance().ConvertInput(value, UnitConverter::UnitTypeDistance);

	mutex->Unlock();

	parent.GetCurrentObject()->SetModified();
	parent.GetMainFrame().UpdateAnalysis();

	event.Skip();
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		ConvertSpringInput
//
// Description:		Converts stiffness inputs.
//
// Input Arguments:
//		value	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		dobule
//
//==========================================================================
double EditTiresPanel::ConvertSpringInput(const double& value)
{
	double converted = 1.0 / UnitConverter::GetInstance().ConvertForceInput(value);
	return 1.0 / UnitConverter::GetInstance().ConvertDistanceInput(converted);
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		ConvertSpringOutput
//
// Description:		Converts stiffness outputs.
//
// Input Arguments:
//		value	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		dobule
//
//==========================================================================
double EditTiresPanel::ConvertSpringOutput(const double& value)
{
	double converted = 1.0 / UnitConverter::GetInstance().ConvertForceOutput(value);
	return 1.0 / UnitConverter::GetInstance().ConvertDistanceOutput(converted);
}
