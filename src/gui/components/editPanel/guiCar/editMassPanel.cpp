/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editMassPanel.cpp
// Date:  2/10/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditMassPanel class.  This
//        class is used to edit the hard points on a particular corner of the
//        car.

// Standard C++ headers
#include <algorithm>

// Local headers
#include "VVASE/core/car/subsystems/massProperties.h"
#include "VVASE/core/threads/threadDefs.h"
#include "../../../guiCar.h"
#include "VVASE/gui/components/mainFrame.h"
#include "VVASE/gui/superGrid.h"
#include "../editPanel.h"
#include "editMassPanel.h"
#include "VVASE/gui/utilities/unitConverter.h"

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

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
	unsprungMassLeftFront->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->unsprungMass.leftFront)));
	unsprungMassRightFront->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->unsprungMass.rightFront)));
	unsprungMassLeftRear->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->unsprungMass.leftRear)));
	unsprungMassRightRear->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->unsprungMass.rightRear)));

	cornerMassLeftFront->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->cornerWeights.leftFront)));
	cornerMassRightFront->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->cornerWeights.rightFront)));
	cornerMassLeftRear->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->cornerWeights.leftRear)));
	cornerMassRightRear->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertMassOutput(currentMassProperties->cornerWeights.rightRear)));

	// Update the inertia
	ixx->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertInertiaOutput(currentMassProperties->ixx)));
	iyy->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertInertiaOutput(currentMassProperties->iyy)));
	izz->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertInertiaOutput(currentMassProperties->izz)));
	ixy->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertInertiaOutput(currentMassProperties->ixy)));
	iyx->SetLabel(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertInertiaOutput(currentMassProperties->ixy)));
	ixz->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertInertiaOutput(currentMassProperties->ixz)));
	izx->SetLabel(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertInertiaOutput(currentMassProperties->ixz)));
	iyz->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertInertiaOutput(currentMassProperties->iyz)));
	izy->SetLabel(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertInertiaOutput(currentMassProperties->iyz)));

	// Update the centers of gravity
	cgHeights->SetCellValue(RowTotalCG, 1, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->totalCGHeight)));
	cgHeights->SetCellValue(RowUnsprungLeftFrontCG, 1, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCGHeights.leftFront)));
	cgHeights->SetCellValue(RowUnsprungRightFrontCG, 1, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCGHeights.rightFront)));
	cgHeights->SetCellValue(RowUnsprungLeftRearCG, 1, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCGHeights.leftRear)));
	cgHeights->SetCellValue(RowUnsprungRightRearCG, 1, UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(currentMassProperties->unsprungCGHeights.rightRear)));

	// Update the units
	inertiaUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitType::Inertia) + ')');
	unsprungMassLeftFrontUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitType::Mass) + ')');
	unsprungMassRightFrontUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitType::Mass) + ')');
	unsprungMassLeftRearUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitType::Mass) + ')');
	unsprungMassRightRearUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitType::Mass) + ')');
	cornerMassLeftFrontUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitType::Mass) + ')');
	cornerMassRightFrontUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitType::Mass) + ')');
	cornerMassLeftRearUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitType::Mass) + ')');
	cornerMassRightRearUnitsLabel->SetLabel('(' + UnitConverter::GetInstance().GetUnitType(UnitType::Mass) + ')');
	cgHeights->SetCellValue(RowUnits, 1, '(' + UnitConverter::GetInstance().GetUnitType(UnitType::Distance) + ')');

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
	wxStaticText *cornerMassLabelLeftFront = new wxStaticText(this, wxID_ANY, _T("LF Corner Mass"));
	wxStaticText *cornerMassLabelRightFront = new wxStaticText(this, wxID_ANY, _T("RF Corner Mass"));
	wxStaticText *cornerMassLabelLeftRear = new wxStaticText(this, wxID_ANY, _T("LR Corner Mass"));
	wxStaticText *cornerMassLabelRightRear = new wxStaticText(this, wxID_ANY, _T("RR Corner Mass"));

	wxStaticText *unsprungMassLabelLeftFront = new wxStaticText(this, wxID_ANY, _T("LF Unsprung Mass"));
	wxStaticText *unsprungMassLabelRightFront = new wxStaticText(this, wxID_ANY, _T("RF Unsprung Mass"));
	wxStaticText *unsprungMassLabelLeftRear = new wxStaticText(this, wxID_ANY, _T("LR Unsprung Mass"));
	wxStaticText *unsprungMassLabelRightRear = new wxStaticText(this, wxID_ANY, _T("RR Unsprung Mass"));

	cornerMassLeftFront = new wxTextCtrl(this, TextBoxCornerMassLeftFront);
	cornerMassRightFront = new wxTextCtrl(this, TextBoxCornerMassRightFront);
	cornerMassLeftRear = new wxTextCtrl(this, TextBoxCornerMassLeftRear);
	cornerMassRightRear = new wxTextCtrl(this, TextBoxCornerMassRightRear);

	unsprungMassLeftFront = new wxTextCtrl(this, TextBoxUnsprungMassLeftFront);
	unsprungMassRightFront = new wxTextCtrl(this, TextBoxUnsprungMassRightFront);
	unsprungMassLeftRear = new wxTextCtrl(this, TextBoxUnsprungMassLeftRear);
	unsprungMassRightRear = new wxTextCtrl(this, TextBoxUnsprungMassRightRear);

	cornerMassLeftFrontUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	cornerMassRightFrontUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	cornerMassLeftRearUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	cornerMassRightRearUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);

	unsprungMassLeftFrontUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	unsprungMassRightFrontUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	unsprungMassLeftRearUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	unsprungMassRightRearUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);

	massInputSizer->Add(cornerMassLabelLeftFront, 0, wxALIGN_CENTER_VERTICAL);
	massInputSizer->Add(cornerMassLeftFront, 0, wxEXPAND);
	massInputSizer->Add(cornerMassLeftFrontUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	massInputSizer->Add(cornerMassLabelRightFront, 0, wxALIGN_CENTER_VERTICAL);
	massInputSizer->Add(cornerMassRightFront, 0, wxEXPAND);
	massInputSizer->Add(cornerMassRightFrontUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	massInputSizer->Add(cornerMassLabelLeftRear, 0, wxALIGN_CENTER_VERTICAL);
	massInputSizer->Add(cornerMassLeftRear, 0, wxEXPAND);
	massInputSizer->Add(cornerMassLeftRearUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	massInputSizer->Add(cornerMassLabelRightRear, 0, wxALIGN_CENTER_VERTICAL);
	massInputSizer->Add(cornerMassRightRear, 0, wxEXPAND);
	massInputSizer->Add(cornerMassRightRearUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

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
	cgHeights = new SuperGrid(this);
	cgHeights->CreateGrid(GridRowCount, 2, wxGrid::wxGridSelectRows);

	cgHeights->BeginBatch();

	int i;
	for (i = 0; i < cgHeights->GetNumberCols(); i++)
		cgHeights->SetReadOnly(0, i, true);

	// Do the processing that needs to be done for each row
	for (i = 0; i < GridRowCount; i++)
	{
		cgHeights->SetReadOnly(i, 0, true);

		// Set the alignment for all of the cells that contain numbers to the right
		cgHeights->SetCellAlignment(i, 1, wxALIGN_RIGHT, wxALIGN_TOP);
	}

	cgHeights->SetCellValue(RowTotalCG, 0, _T("Total"));
	cgHeights->SetCellValue(RowUnsprungLeftFrontCG, 0, _T("LF Unsprung Mass"));
	cgHeights->SetCellValue(RowUnsprungRightFrontCG, 0, _T("RF Unsprung Mass"));
	cgHeights->SetCellValue(RowUnsprungLeftRearCG, 0, _T("LR Unsprung Mass"));
	cgHeights->SetCellValue(RowUnsprungRightRearCG, 0, _T("RR Unsprung Mass"));

	// Hide the label column and set the size for the label row
	cgHeights->SetRowLabelSize(0);
	cgHeights->SetColLabelSize(cgHeights->GetRowSize(0));

	// Add the grid to the sizer
	cgInputSizer->Add(cgHeights, 0, wxALIGN_TOP | wxEXPAND);

	cgHeights->SetColLabelValue(0, _T("Element"));
	cgHeights->SetColLabelValue(1, _T("CG Z"));

	// Center the cells for the column headings
	for (i = 0; i < cgHeights->GetNumberCols(); i++)
		cgHeights->SetCellAlignment(0, i, wxALIGN_CENTER, wxALIGN_TOP);

	// Size the columns
	// The X, Y, and Z columns should be big enough to fit 80.0 as formatted
	// by the converter.  First column is stretchable
	cgHeights->SetCellValue(1, 1, UnitConverter::GetInstance().FormatNumber(80.0));
	cgHeights->AutoSizeColumn(1);
	// The value we just put in cell (1,1) will get overwritten with a call to UpdateInformation()
	cgHeights->AutoStretchColumn(0);

	cgHeights->EnableDragColMove(false);
	cgHeights->EnableDragColSize(true);
	cgHeights->EnableDragGridSize(false);
	cgHeights->EnableDragRowSize(false);

	cgHeights->EndBatch();

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
	cornerMassLeftFront->SetMinSize(wxSize(minWidth, -1));
	cornerMassRightFront->SetMinSize(wxSize(minWidth, -1));
	cornerMassLeftRear->SetMinSize(wxSize(minWidth, -1));
	cornerMassRightRear->SetMinSize(wxSize(minWidth, -1));
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
	UnitType units;
	wxTextCtrl *textBox = NULL;

	switch (event.GetId())
	{
	case TextBoxIxx:
		textBox = ixx;
		dataLocation = &currentMassProperties->ixx;
		units = UnitType::Inertia;
		break;

	case TextBoxIyy:
		textBox = iyy;
		dataLocation = &currentMassProperties->iyy;
		units = UnitType::Inertia;
		break;

	case TextBoxIzz:
		textBox = izz;
		dataLocation = &currentMassProperties->izz;
		units = UnitType::Inertia;
		break;

	case TextBoxIxy:
		textBox = ixy;
		dataLocation = &currentMassProperties->ixy;
		units = UnitType::Inertia;
		break;

	case TextBoxIxz:
		textBox = ixz;
		dataLocation = &currentMassProperties->ixz;
		units = UnitType::Inertia;
		break;

	case TextBoxIyz:
		textBox = iyz;
		dataLocation = &currentMassProperties->iyz;
		units = UnitType::Inertia;
		break;

	case TextBoxCornerMassLeftFront:
		textBox = cornerMassLeftFront;
		dataLocation = &currentMassProperties->cornerWeights.leftFront;
		units = UnitType::Force;
		break;

	case TextBoxCornerMassRightFront:
		textBox = cornerMassRightFront;
		dataLocation = &currentMassProperties->cornerWeights.rightFront;
		units = UnitType::Force;
		break;

	case TextBoxCornerMassLeftRear:
		textBox = cornerMassLeftRear;
		dataLocation = &currentMassProperties->cornerWeights.leftRear;
		units = UnitType::Force;
		break;

	case TextBoxCornerMassRightRear:
		textBox = cornerMassRightRear;
		dataLocation = &currentMassProperties->cornerWeights.rightRear;
		units = UnitType::Force;
		break;

	case TextBoxUnsprungMassLeftFront:
		textBox = unsprungMassLeftFront;
		dataLocation = &currentMassProperties->unsprungMass.leftFront;
		units = UnitType::Mass;
		break;

	case TextBoxUnsprungMassRightFront:
		textBox = unsprungMassRightFront;
		dataLocation = &currentMassProperties->unsprungMass.rightFront;
		units = UnitType::Mass;
		break;

	case TextBoxUnsprungMassLeftRear:
		textBox = unsprungMassLeftRear;
		dataLocation = &currentMassProperties->unsprungMass.leftRear;
		units = UnitType::Mass;
		break;

	case TextBoxUnsprungMassRightRear:
		textBox = unsprungMassRightRear;
		dataLocation = &currentMassProperties->unsprungMass.rightRear;
		units = UnitType::Mass;
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

	{
		MutexLocker lock(*parent.GetCurrentMutex());
		*dataLocation = UnitConverter::GetInstance().ConvertInput(value, units);
	}

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
		wxString valueString = cgHeights->GetCellValue(event.GetRow(), event.GetCol());
		double value;

		// Make sure the value is numeric
		if (!valueString.ToDouble(&value))
			// The value is non-numeric - don't do anything
			return;

		double* valueLocation;

		{
			MutexLocker lock(*parent.GetCurrentMutex());

			if (event.GetRow() == RowTotalCG)
				valueLocation = &currentMassProperties->totalCGHeight;
			else if (event.GetRow() == RowUnsprungLeftFrontCG)
				valueLocation = &currentMassProperties->unsprungCGHeights.leftFront;
			else if (event.GetRow() == RowUnsprungRightFrontCG)
				valueLocation = &currentMassProperties->unsprungCGHeights.rightFront;
			else if (event.GetRow() == RowUnsprungLeftRearCG)
				valueLocation = &currentMassProperties->unsprungCGHeights.leftRear;
			else// if (event.GetRow() == RowUnsprungRightRearCG)
				valueLocation = &currentMassProperties->unsprungCGHeights.rightRear;

			parent.GetMainFrame().GetUndoRedoStack().AddOperation(
				parent.GetMainFrame().GetActiveIndex(),
				UndoRedoStack::Operation::DataTypeDouble,
				valueLocation);

			*valueLocation = UnitConverter::GetInstance().ConvertDistanceInput(value);
		}

		parent.GetCurrentObject()->SetModified();
		parent.GetMainFrame().UpdateAnalysis();
		parent.GetMainFrame().UpdateOutputPanel();
	}

	event.Skip();
}

}// namespace VVASE
