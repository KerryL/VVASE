/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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

// Standard C++ headers
#include <algorithm>

// Local headers
#include "vCar/massProperties.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/superGrid.h"
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
	EVT_GRID_CELL_CHANGED(EditMassPanel::GridCellChangeEvent)
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
	unsprungMassLeftFront->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->unsprungMass.leftFront)));
	unsprungMassRightFront->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->unsprungMass.rightFront)));
	unsprungMassLeftRear->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->unsprungMass.leftRear)));
	unsprungMassRightRear->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->unsprungMass.rightRear)));

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

	// Update the centers of gravity
	centerOfGravity->SetCellValue(RowSprungMassCG, 1, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->centerOfGravity.x)));
	centerOfGravity->SetCellValue(RowSprungMassCG, 2, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->centerOfGravity.y)));
	centerOfGravity->SetCellValue(RowSprungMassCG, 3, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->centerOfGravity.z)));

	centerOfGravity->SetCellValue(RowUnsprungLeftFrontCG, 1, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCentersOfGravity.leftFront.x)));
	centerOfGravity->SetCellValue(RowUnsprungLeftFrontCG, 2, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCentersOfGravity.leftFront.y)));
	centerOfGravity->SetCellValue(RowUnsprungLeftFrontCG, 3, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCentersOfGravity.leftFront.z)));

	centerOfGravity->SetCellValue(RowUnsprungRightFrontCG, 1, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCentersOfGravity.rightFront.x)));
	centerOfGravity->SetCellValue(RowUnsprungRightFrontCG, 2, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCentersOfGravity.rightFront.y)));
	centerOfGravity->SetCellValue(RowUnsprungRightFrontCG, 3, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCentersOfGravity.rightFront.z)));

	centerOfGravity->SetCellValue(RowUnsprungLeftRearCG, 1, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCentersOfGravity.leftRear.x)));
	centerOfGravity->SetCellValue(RowUnsprungLeftRearCG, 2, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCentersOfGravity.leftRear.y)));
	centerOfGravity->SetCellValue(RowUnsprungLeftRearCG, 3, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCentersOfGravity.leftRear.z)));

	centerOfGravity->SetCellValue(RowUnsprungRightRearCG, 1, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCentersOfGravity.rightRear.x)));
	centerOfGravity->SetCellValue(RowUnsprungRightRearCG, 2, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCentersOfGravity.rightRear.y)));
	centerOfGravity->SetCellValue(RowUnsprungRightRearCG, 3, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCentersOfGravity.rightRear.z)));

	// Update the units
	inertiaUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeInertia) + ')');
	massUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeMass) + ')');
	unsprungMassLeftFrontUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeMass) + ')');
	unsprungMassRightFrontUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeMass) + ')');
	unsprungMassLeftRearUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeMass) + ')');
	unsprungMassRightRearUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeMass) + ')');
	centerOfGravity->SetCellValue(RowUnits, 1, '(' + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance) + ')');
	centerOfGravity->SetCellValue(RowUnits, 2, '(' + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance) + ')');
	centerOfGravity->SetCellValue(RowUnits, 3, '(' + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance) + ')');

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
	SetScrollRate(10, 10);
	
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
	wxFlexGridSizer *inertiaInputSizer = new wxFlexGridSizer(3, 3, 3);

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
	inertiaInputSizer->Add(ixx, 0, wxEXPAND);
	inertiaInputSizer->Add(ixy, 0, wxEXPAND);
	inertiaInputSizer->Add(ixz, 0, wxEXPAND);

	// Second row
	inertiaInputSizer->Add(iyx, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(iyy, 0, wxEXPAND);
	inertiaInputSizer->Add(iyz, 0, wxEXPAND);

	// Third row
	inertiaInputSizer->Add(izx, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(izy, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(izz, 0, wxEXPAND);
	
	// Blank row for space
	int spaceSize(30);
	inertiaInputSizer->AddSpacer(spaceSize);

	// Make the columns growable
	inertiaInputSizer->AddGrowableCol(0, 1);
	inertiaInputSizer->AddGrowableCol(1, 1);
	inertiaInputSizer->AddGrowableCol(2, 1);

	// Create the mass inputs
	wxFlexGridSizer *massInputSizer = new wxFlexGridSizer(3, 3, 3);
	wxStaticText *massLabel = new wxStaticText(this, wxID_ANY, _T("Total Mass"));
	wxStaticText *unsprungMassLabelLeftFront = new wxStaticText(this, wxID_ANY, _T("LF Unsprung Mass"));
	wxStaticText *unsprungMassLabelRightFront = new wxStaticText(this, wxID_ANY, _T("RF Unsprung Mass"));
	wxStaticText *unsprungMassLabelLeftRear = new wxStaticText(this, wxID_ANY, _T("LR Unsprung Mass"));
	wxStaticText *unsprungMassLabelRightRear = new wxStaticText(this, wxID_ANY, _T("RR Unsprung Mass"));

	mass = new wxTextCtrl(this, TextBoxMass);
	unsprungMassLeftFront = new wxTextCtrl(this, TextBoxUnsprungMassLeftFront);
	unsprungMassRightFront = new wxTextCtrl(this, TextBoxUnsprungMassRightFront);
	unsprungMassLeftRear = new wxTextCtrl(this, TextBoxUnsprungMassLeftRear);
	unsprungMassRightRear = new wxTextCtrl(this, TextBoxUnsprungMassRightRear);

	massUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	unsprungMassLeftFrontUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	unsprungMassRightFrontUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	unsprungMassLeftRearUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	unsprungMassRightRearUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);

	massInputSizer->Add(massLabel, 0, wxALIGN_CENTER_VERTICAL);
	massInputSizer->Add(mass, 0, wxEXPAND);
	massInputSizer->Add(massUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	massInputSizer->Add(unsprungMassLabelLeftFront, 0, wxALIGN_CENTER_VERTICAL);
	massInputSizer->Add(unsprungMassLeftFront, 0, wxEXPAND);
	massInputSizer->Add(unsprungMassLeftFrontUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	massInputSizer->Add(unsprungMassLabelRightFront, 0, wxALIGN_CENTER_VERTICAL);
	massInputSizer->Add(unsprungMassRightFront, 0, wxEXPAND);
	massInputSizer->Add(unsprungMassRightFrontUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	massInputSizer->Add(unsprungMassLabelLeftRear, 0, wxALIGN_CENTER_VERTICAL);
	massInputSizer->Add(unsprungMassLeftRear, 0, wxEXPAND);
	massInputSizer->Add(unsprungMassLeftRearUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	massInputSizer->Add(unsprungMassLabelRightRear, 0, wxALIGN_CENTER_VERTICAL);
	massInputSizer->Add(unsprungMassRightRear, 0, wxEXPAND);
	massInputSizer->Add(unsprungMassRightRearUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Blank row for space
	massInputSizer->AddSpacer(spaceSize);

	// Create the center of gravity inputs
	wxBoxSizer *cgInputSizer = new wxBoxSizer(wxHORIZONTAL);
	centerOfGravity = new SuperGrid(this);
	centerOfGravity->CreateGrid(GridRowCount, 4, wxGrid::wxGridSelectRows);

	centerOfGravity->BeginBatch();

	int i;
	for (i = 0; i < centerOfGravity->GetNumberCols(); i++)
		centerOfGravity->SetReadOnly(0, i, true);

	// Do the processing that needs to be done for each row
	for (i = 0; i < GridRowCount; i++)
	{
		centerOfGravity->SetReadOnly(i, 0, true);

		// Set the alignment for all of the cells that contain numbers to the right
		centerOfGravity->SetCellAlignment(i, 1, wxALIGN_RIGHT, wxALIGN_TOP);
		centerOfGravity->SetCellAlignment(i, 2, wxALIGN_RIGHT, wxALIGN_TOP);
		centerOfGravity->SetCellAlignment(i, 3, wxALIGN_RIGHT, wxALIGN_TOP);
	}

	centerOfGravity->SetCellValue(RowSprungMassCG, 0, _T("Sprung Mass"));
	centerOfGravity->SetCellValue(RowUnsprungLeftFrontCG, 0, _T("LF Unsprung Mass"));
	centerOfGravity->SetCellValue(RowUnsprungRightFrontCG, 0, _T("RF Unsprung Mass"));
	centerOfGravity->SetCellValue(RowUnsprungLeftRearCG, 0, _T("LR Unsprung Mass"));
	centerOfGravity->SetCellValue(RowUnsprungRightRearCG, 0, _T("RR Unsprung Mass"));

	// Hide the label column and set the size for the label row
	centerOfGravity->SetRowLabelSize(0);
	centerOfGravity->SetColLabelSize(centerOfGravity->GetRowSize(0));

	// Add the grid to the sizer
	cgInputSizer->Add(centerOfGravity, 0, wxALIGN_TOP | wxEXPAND);

	centerOfGravity->SetColLabelValue(0, _T("Body"));
	centerOfGravity->SetColLabelValue(1, _T("CG X"));
	centerOfGravity->SetColLabelValue(2, _T("CG Y"));
	centerOfGravity->SetColLabelValue(3, _T("CG Z"));

	// Center the cells for the column headings
	for (i = 0; i < centerOfGravity->GetNumberCols(); i++)
		centerOfGravity->SetCellAlignment(0, i, wxALIGN_CENTER, wxALIGN_TOP);

	// Size the columns
	// The X, Y, and Z columns should be big enough to fit 80.0 as formatted
	// by the converter.  First column is stretchable
	centerOfGravity->SetCellValue(3, 3, UnitConverter::GetInstance().FormatNumber(80.0));
	centerOfGravity->AutoSizeColumn(3);
	centerOfGravity->SetColSize(1, centerOfGravity->GetColSize(3));
	centerOfGravity->SetColSize(2, centerOfGravity->GetColSize(3));
	// The value we just put in cell (3,3) will get overwritten with a call to UpdateInformation()
	centerOfGravity->AutoStretchColumn(0);

	centerOfGravity->EnableDragColMove(false);
	centerOfGravity->EnableDragColSize(true);
	centerOfGravity->EnableDragGridSize(false);
	centerOfGravity->EnableDragRowSize(false);

	centerOfGravity->EndBatch();

	// Add the sizers to the main sizer
	mainSizer->Add(inertiaSizer);
	mainSizer->Add(inertiaInputSizer);
	mainSizer->Add(massInputSizer);
	mainSizer->Add(cgInputSizer);

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
	unsprungMassLeftFront->SetMinSize(wxSize(minWidth, -1));
	unsprungMassRightFront->SetMinSize(wxSize(minWidth, -1));
	unsprungMassLeftRear->SetMinSize(wxSize(minWidth, -1));
	unsprungMassRightRear->SetMinSize(wxSize(minWidth, -1));

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

	case TextBoxUnsprungMassLeftFront:
		textBox = unsprungMassLeftFront;
		dataLocation = &currentMassProperties->unsprungMass.leftFront;
		units = UnitConverter::UnitTypeMass;
		break;

	case TextBoxUnsprungMassRightFront:
		textBox = unsprungMassRightFront;
		dataLocation = &currentMassProperties->unsprungMass.rightFront;
		units = UnitConverter::UnitTypeMass;
		break;

	case TextBoxUnsprungMassLeftRear:
		textBox = unsprungMassLeftRear;
		dataLocation = &currentMassProperties->unsprungMass.leftRear;
		units = UnitConverter::UnitTypeMass;
		break;

	case TextBoxUnsprungMassRightRear:
		textBox = unsprungMassRightRear;
		dataLocation = &currentMassProperties->unsprungMass.rightRear;
		units = UnitConverter::UnitTypeMass;
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

//==========================================================================
// Class:			EditMassPanel
// Function:		GridCellChangeEvent
//
// Description:		Event that fires when any grid cell changes.
//
// Input Arguments:
//		event = wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditMassPanel::GridCellChangeEvent(wxGridEvent& event)
{
	if (event.GetRow() > 0)
	{
		wxString valueString = centerOfGravity->GetCellValue(event.GetRow(), event.GetCol());
		double value;

		// Make sure the value is numeric
		if (!valueString.ToDouble(&value))
			// The value is non-numeric - don't do anything
			return;

		Vector* vector;
		double* valueLocation;

		wxMutex *mutex = parent.GetCurrentMutex();
		mutex->Lock();

		if (event.GetRow() == RowSprungMassCG)
			vector = &currentMassProperties->centerOfGravity;
		else if (event.GetRow() == RowUnsprungLeftFrontCG)
			vector = &currentMassProperties->unsprungCentersOfGravity.leftFront;
		else if (event.GetRow() == RowUnsprungRightFrontCG)
			vector = &currentMassProperties->unsprungCentersOfGravity.rightFront;
		else if (event.GetRow() == RowUnsprungLeftRearCG)
			vector = &currentMassProperties->unsprungCentersOfGravity.leftRear;
		else// if (event.GetRow() == RowUnsprungRightRearCG)
			vector = &currentMassProperties->unsprungCentersOfGravity.rightRear;

		if (event.GetCol() == 1)// X
			valueLocation = &vector->x;
		else if (event.GetCol() == 2)// Y
			valueLocation = &vector->y;
		else// if (event.GetCol() == 3)// Z
			valueLocation = &vector->z;

		parent.GetMainFrame().GetUndoRedoStack().AddOperation(
			parent.GetMainFrame().GetActiveIndex(),
			UndoRedoStack::Operation::DataTypeDouble,
			valueLocation);

		*valueLocation = UnitConverter::GetInstance().ConvertDistanceInput(value);

		mutex->Unlock();

		parent.GetCurrentObject()->SetModified();
		parent.GetMainFrame().UpdateAnalysis();
		parent.GetMainFrame().UpdateOutputPanel();
	}

	event.Skip();
}
