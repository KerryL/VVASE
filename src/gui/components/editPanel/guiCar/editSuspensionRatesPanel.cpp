/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSuspensionRatesPanel.cpp
// Date:  5/27/2016
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditSuspensionRatesPanel class.
//        This class is used to edit the spring and ARB rates.

// Local headers
#include "vCar/suspension.h"
#include "gui/guiCar.h"
#include "gui/superGrid.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editSuspensionRatesPanel.h"
#include "gui/components/editPanel/guiCar/editSuspensionNotebook.h"
#include "vUtilities/unitConverter.h"

//==========================================================================
// Class:			EditSuspensionRatesPanel
// Function:		EditSuspensionRatesPanel
//
// Description:		Constructor for EditSuspensionRatesPanel class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		_parent		= EditSuspensionNotebook&, reference to this object's owner
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
EditSuspensionRatesPanel::EditSuspensionRatesPanel(EditSuspensionNotebook &parent,
	wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxScrolledWindow(&parent, id, pos, size), parent(parent)
{
	CreateControls();
}

//==========================================================================
// Class:			EditSuspensionRatesPanel
// Function:		~EditSuspensionRatesPanel
//
// Description:		Destructor for EditSuspensionRatesPanel class.
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
EditSuspensionRatesPanel::~EditSuspensionRatesPanel()
{
}

//==========================================================================
// Class:			EditSuspensionRatesPanel
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
BEGIN_EVENT_TABLE(EditSuspensionRatesPanel, wxPanel)
	EVT_GRID_CELL_CHANGED(EditSuspensionRatesPanel::GridCellChangedEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditSuspensionRatesPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel, if the associated
//					object already exists.
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
void EditSuspensionRatesPanel::UpdateInformation()
{
	if (currentSuspension)
		UpdateInformation(currentSuspension);
}

//==========================================================================
// Class:			EditSuspensionRatesPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_currentSuspension	= Suspension* pointing to the associated suspension
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSuspensionRatesPanel::UpdateInformation(Suspension *currentSuspension)
{
	this->currentSuspension = currentSuspension;

	rates->BeginBatch();

	// Hide or show rows according to this object's configuration
	if (currentSuspension->frontHasThirdSpring)
		rates->SetRowSize(RowFrontThirdSpring, rates->GetRowHeight(0));
	else
		rates->SetRowSize(RowFrontThirdSpring, 0);

	if (currentSuspension->rearHasThirdSpring)
		rates->SetRowSize(RowRearThirdSpring, rates->GetRowHeight(0));
	else
		rates->SetRowSize(RowRearThirdSpring, 0);

	if (currentSuspension->frontBarStyle != Suspension::SwayBarNone)
		rates->SetRowSize(RowFrontARB, rates->GetRowHeight(0));
	else
		rates->SetRowSize(RowFrontARB, 0);

	if (currentSuspension->rearBarStyle != Suspension::SwayBarNone)
		rates->SetRowSize(RowRearARB, rates->GetRowHeight(0));
	else
		rates->SetRowSize(RowRearARB, 0);

	wxString linearUnitString;
	linearUnitString.Printf("(%s/%s)",
		UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeForce).c_str(),
		UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance).c_str());

	wxString rotaryUnitString;
	rotaryUnitString.Printf("(%s/%s)",
		UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeMoment).c_str(),
		UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeAngle).c_str());

	rates->SetCellValue(RowLeftFrontSpring, ColValue, UnitConverter::GetInstance().FormatNumber(
		ConvertLinearSpringOutput(currentSuspension->leftFront.spring.rate)));
	rates->SetCellValue(RowRightFrontSpring, ColValue, UnitConverter::GetInstance().FormatNumber(
		ConvertLinearSpringOutput(currentSuspension->rightFront.spring.rate)));
	rates->SetCellValue(RowLeftRearSpring, ColValue, UnitConverter::GetInstance().FormatNumber(
		ConvertLinearSpringOutput(currentSuspension->leftRear.spring.rate)));
	rates->SetCellValue(RowRightRearSpring, ColValue, UnitConverter::GetInstance().FormatNumber(
		ConvertLinearSpringOutput(currentSuspension->rightRear.spring.rate)));
	rates->SetCellValue(RowFrontThirdSpring, ColValue, UnitConverter::GetInstance().FormatNumber(
		ConvertLinearSpringOutput(currentSuspension->frontThirdSpring.rate)));
	rates->SetCellValue(RowRearThirdSpring, ColValue, UnitConverter::GetInstance().FormatNumber(
		ConvertLinearSpringOutput(currentSuspension->rearThirdSpring.rate)));
	rates->SetCellValue(RowFrontARB, ColValue, UnitConverter::GetInstance().FormatNumber(
		ConvertRotarySpringOutput(currentSuspension->barRate.front)));
	rates->SetCellValue(RowRearARB, ColValue, UnitConverter::GetInstance().FormatNumber(
		ConvertRotarySpringOutput(currentSuspension->barRate.rear)));

	rates->SetCellValue(RowLeftFrontSpring, ColUnits, linearUnitString);
	rates->SetCellValue(RowRightFrontSpring, ColUnits, linearUnitString);
	rates->SetCellValue(RowLeftRearSpring, ColUnits, linearUnitString);
	rates->SetCellValue(RowRightRearSpring, ColUnits, linearUnitString);
	rates->SetCellValue(RowFrontThirdSpring, ColUnits, linearUnitString);
	rates->SetCellValue(RowRearThirdSpring, ColUnits, linearUnitString);
	rates->SetCellValue(RowFrontARB, ColUnits, rotaryUnitString);
	rates->SetCellValue(RowRearARB, ColUnits, rotaryUnitString);

	rates->FitHeight();
	rates->EndBatch();

	Layout();

	// FIXME:  Need way to turn grid scrollbars off
}

//==========================================================================
// Class:			EditSuspensionRatesPanel
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
void EditSuspensionRatesPanel::CreateControls()
{
	// Enable scrolling
	SetScrollRate(10, 10);

	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxFlexGridSizer *mainSizer = new wxFlexGridSizer(1, 2, 2);
	topSizer->Add(mainSizer, 1, wxGROW | wxALL, 5);

	// Create the grid for the hard point entry
	rates = new SuperGrid(this, wxID_ANY);
	rates->CreateGrid(GridRowCount, GridColCount, wxGrid::wxGridSelectRows);

	rates->BeginBatch();

	// Do the processing that needs to be done for each row
	int i;
	for (i = 0; i < GridRowCount; i++)
	{
		rates->SetReadOnly(i, ColLabel, true);
		rates->SetReadOnly(i, ColUnits, true);

		// Set the alignment for all of the cells that contain numbers to the right
		rates->SetCellAlignment(i, ColValue, wxALIGN_RIGHT, wxALIGN_TOP);
		rates->SetCellAlignment(i, ColUnits, wxALIGN_CENTER, wxALIGN_TOP);
	}

	rates->SetCellValue(RowLeftFrontSpring, ColLabel, _T("Left Front"));
	rates->SetCellValue(RowRightFrontSpring, ColLabel, _T("Right Front"));
	rates->SetCellValue(RowLeftRearSpring, ColLabel, _T("Left Rear"));
	rates->SetCellValue(RowRightRearSpring, ColLabel, _T("Right Rear"));
	rates->SetCellValue(RowFrontThirdSpring, ColLabel, _T("Front 3rd"));
	rates->SetCellValue(RowRearThirdSpring, ColLabel, _T("Rear 3rd"));
	rates->SetCellValue(RowFrontARB, ColLabel, _T("Front ARB"));
	rates->SetCellValue(RowRearARB, ColLabel, _T("Rear ARB"));

	// Hide the label column and set the size for the label row
	rates->SetRowLabelSize(0);
	rates->SetColLabelSize(rates->GetRowSize(0));

	// To enable hiding of the non-label rows, we need to set the minimum height to zero
	rates->SetRowMinimalAcceptableHeight(0);

	// Add the grid to the sizer
	mainSizer->Add(rates, 0, wxALIGN_TOP | wxEXPAND);
	mainSizer->AddGrowableCol(0);

	rates->SetColLabelValue(ColLabel, _T("Component"));
	rates->SetColLabelValue(ColValue, _T("Rate"));
	rates->SetColLabelValue(ColUnits, _T("Units"));

	// Size the columns
	// The X, Y, and Z columns should be big enough to fit 800.0 as formatted
	// by the converter.  First column is stretchable
	rates->SetCellValue(RowLeftFrontSpring, ColValue, UnitConverter::GetInstance().FormatNumber(800.0));
	rates->AutoSizeColumn(ColValue);
	// The value we just put in cell (3,3) will get overwritten with a call to UpdateInformation()
	rates->AutoStretchColumn(ColLabel);
	rates->AutoStretchColumn(ColUnits);

	rates->EnableDragColMove(false);
	rates->EnableDragColSize(true);
	rates->EnableDragGridSize(false);
	rates->EnableDragRowSize(false);

	rates->EndBatch();

	SetSizer(topSizer);

	// Initialize the last row selected variable
	lastRowSelected = -1;
}

//==========================================================================
// Class:			EditSuspensionRatesPanel
// Function:		GridCellChangedEvent
//
// Description:		Event that fires when a cell is changed.
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
void EditSuspensionRatesPanel::GridCellChangedEvent(wxGridEvent &event)
{
	if (event.GetRow() > 0)
	{
		wxString valueString = rates->GetCellValue(event.GetRow(), event.GetCol());
		double value;

		// Make sure the value is numeric
		if (!valueString.ToDouble(&value))
			// The value is non-numeric - don't do anything
			return;

		wxMutex *mutex = parent.GetParent().GetCurrentMutex();
		mutex->Lock();

		double *valueLocation;
		if (event.GetRow() == RowLeftFrontSpring)
			valueLocation = &currentSuspension->leftFront.spring.rate;
		else if (event.GetRow() == RowRightFrontSpring)
			valueLocation = &currentSuspension->rightFront.spring.rate;
		else if (event.GetRow() == RowLeftRearSpring)
			valueLocation = &currentSuspension->leftRear.spring.rate;
		else if (event.GetRow() == RowRightRearSpring)
			valueLocation = &currentSuspension->rightRear.spring.rate;
		else if (event.GetRow() == RowFrontThirdSpring)
			valueLocation = &currentSuspension->frontThirdSpring.rate;
		else if (event.GetRow() == RowRearThirdSpring)
			valueLocation = &currentSuspension->rearThirdSpring.rate;
		else if (event.GetRow() == RowFrontARB)
			valueLocation = &currentSuspension->barRate.front;
		else// if (event.GetRow() == RowRearARB)
			valueLocation = &currentSuspension->barRate.rear;

		// Add the operation to the undo/redo stack
		parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
			parent.GetParent().GetMainFrame().GetActiveIndex(),
			UndoRedoStack::Operation::DataTypeDouble, valueLocation);

		if (event.GetRow() == RowFrontARB || event.GetRow() == RowRearARB)
			*valueLocation = ConvertRotarySpringInput(value);
		else
			*valueLocation = ConvertLinearSpringInput(value);

		mutex->Unlock();

		parent.GetParent().GetCurrentObject()->SetModified();
		parent.GetParent().GetMainFrame().UpdateAnalysis();
		parent.GetParent().GetMainFrame().UpdateOutputPanel();
	}

	event.Skip();
}

//==========================================================================
// Class:			EditSuspensionRatesPanel
// Function:		ConvertLinearSpringInput
//
// Description:		Converts units for linear springs from user-specified to internal.
//
// Input Arguments:
//		value	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
double EditSuspensionRatesPanel::ConvertLinearSpringInput(const double& value) const
{
	double converted = 1.0 / UnitConverter::GetInstance().ConvertForceInput(value);
	return 1.0 / UnitConverter::GetInstance().ConvertDistanceInput(converted);
}

//==========================================================================
// Class:			EditSuspensionRatesPanel
// Function:		ConvertLinearSpringOutput
//
// Description:		Converts units for linear springs from internal to user-specified.
//
// Input Arguments:
//		value	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
double EditSuspensionRatesPanel::ConvertLinearSpringOutput(const double& value) const
{
	double converted = 1.0 / UnitConverter::GetInstance().ConvertForceOutput(value);
	return 1.0 / UnitConverter::GetInstance().ConvertDistanceOutput(converted);
}

//==========================================================================
// Class:			EditSuspensionRatesPanel
// Function:		ConvertRotarySpringInput
//
// Description:		Converts units for rotary springs from user-specified to internal.
//
// Input Arguments:
//		value	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
double EditSuspensionRatesPanel::ConvertRotarySpringInput(const double& value) const
{
	double converted = 1.0 / UnitConverter::GetInstance().ConvertMomentInput(value);
	return 1.0 / UnitConverter::GetInstance().ConvertAngleInput(converted);
}

//==========================================================================
// Class:			EditSuspensionRatesPanel
// Function:		ConvertRotarySpringOutput
//
// Description:		Converts units for rotary springs from internal to user-specified.
//
// Input Arguments:
//		value	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
double EditSuspensionRatesPanel::ConvertRotarySpringOutput(const double& value) const
{
	double converted = 1.0 / UnitConverter::GetInstance().ConvertMomentOutput(value);
	return 1.0 / UnitConverter::GetInstance().ConvertAngleOutput(converted);
}
