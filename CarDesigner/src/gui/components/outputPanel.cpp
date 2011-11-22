/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  outputPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the OUTPUT_PANEL class.  This
//				 class is used to display the kinematic output variables.
// History:
//	3/30/2009	- Added FinishUpdate and changed UpdateInformation to allow for arraying
//				  outputs for all open cars, K. Loux.
//	5/7/2009	- Made grid columns re-sizable and started using the column labels, K. Loux.

// CarDesigner headers
#include "gui/components/outputPanel.h"
#include "gui/components/mainFrame.h"
#include "vCar/car.h"
#include "vCar/drivetrain.h"
#include "vCar/suspension.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vUtilities/debugger.h"
#include "vUtilities/convert.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			OUTPUT_PANEL
// Function:		OUTPUT_PANEL
//
// Description:		Constructor for OUTPUT_PANEL class.  Initializes the form
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
OUTPUT_PANEL::OUTPUT_PANEL(MAIN_FRAME &_MainFrame, wxWindowID id, const wxPoint &pos,
						   const wxSize &size, const Debugger &_debugger)
						   : wxPanel(&_MainFrame, id, pos, size),
						   debugger(_debugger), Converter(_MainFrame.GetConverter()),
						   MainFrame(_MainFrame)
{
	// Create the controls
	CreateControls();

	// Initialize the number of data columns to zero
	NumberOfDataColumns = 0;
}

//==========================================================================
// Class:			OUTPUT_PANEL
// Function:		~OUTPUT_PANEL
//
// Description:		Destructor for OUTPUT_PANEL class.
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
OUTPUT_PANEL::~OUTPUT_PANEL()
{
}

//==========================================================================
// Class:			OUTPUT_PANEL
// Function:		Event Tables
//
// Description:		Event table for the OUTPUT_PANEL class.
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
BEGIN_EVENT_TABLE(OUTPUT_PANEL, wxPanel)
	EVT_SIZE(OUTPUT_PANEL::OnSize)
END_EVENT_TABLE()

//==========================================================================
// Class:			OUTPUT_PANEL
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		Outputs		= KinematicOutputs containing the outputs we want to use
//					  for the update
//		car			= Car& pointing to the car with which these outputs are associated
//		Index		= int specifying the data column to edit
//		Name		= wxString specifying the name of the car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void OUTPUT_PANEL::UpdateInformation(KinematicOutputs Outputs, Car &car,
									 int Index, wxString Name)
{
	// Begin batch edit of the grid
	OutputsList->BeginBatch();

	// See if we need to add a column
	if (Index > NumberOfDataColumns)
	{
		// Add a column
		if (!OutputsList->InsertCols(Index))
		{
			// Add column failed - print warning and return
			debugger.Print(_T("Warning (OUTPUT_PANEL::UpdateInformation):  InsertCols failed!"),
				Debugger::PriorityMedium);

			return;
		}

		// Set the column width
		OutputsList->SetColumnWidth(Index, 50);

		// Increment the NumberOfDataColums
		NumberOfDataColumns++;
	}

	// Update the name for this column (and make it read only, bold and centered)
	OutputsList->SetColLabelValue(Index, Name);

	// Update all of the values and unit labels
	int i;
	for (i = 0; i < KinematicOutputs::NumberOfOutputScalars; i++)
	{
		// Convert and set the value
		OutputsList->SetCellValue(i, Index, Converter.FormatNumber(Converter.ConvertTo(
			Outputs.GetOutputValue((KinematicOutputs::OutputsComplete)i),
			KinematicOutputs::GetOutputUnitType((KinematicOutputs::OutputsComplete)i))));

		// If the current value is undefined, make sure the user knows it
		if (VVASEMath::IsNaN(Outputs.GetOutputValue((KinematicOutputs::OutputsComplete)i)))
		{
			// Change the cell value "Undef." instead of #1.Q0
			OutputsList->SetCellValue(i, Index, _T("Undef."));

			// Make the cell background yellow
			OutputsList->SetCellBackgroundColour(i, Index, wxColor(255, 255, 0));
		}
		else
			// Make sure the background color isn't still set from a previously
			// undefined value
			OutputsList->SetCellBackgroundColour(i, Index, *wxWHITE);

		// Set the alignment
		OutputsList->SetCellAlignment(i, Index, wxALIGN_CENTER, wxALIGN_TOP);

		// Make the cell read-only
		OutputsList->SetReadOnly(i, Index, true);
	}

	// For values that do not pertain to this car (front axle plunge for a car without
	// front axles), change the text to "N/A".  Also make sure the cell background is
	// white - otherwise it might appear to be "Undef." instead of "N/A".

	// Get a lock on the car's mutex
	wxMutexLocker lock(car.GetMutex());

	// Front half shafts
	if (!car.HasFrontHalfShafts())
	{
		// Change the cell's text
		OutputsList->SetCellValue(KinematicOutputs::StartRightFrontDoubles
			+ KinematicOutputs::AxlePlunge, Index, _T("N/A"));
		OutputsList->SetCellValue(KinematicOutputs::StartLeftFrontDoubles
			+ KinematicOutputs::AxlePlunge, Index, _T("N/A"));
	}

	// Rear half shafts
	if (!car.HasRearHalfShafts())
	{
		// Change the cell's text
		OutputsList->SetCellValue(KinematicOutputs::StartRightRearDoubles
			+ KinematicOutputs::AxlePlunge, Index, _T("N/A"));
		OutputsList->SetCellValue(KinematicOutputs::StartLeftRearDoubles
			+ KinematicOutputs::AxlePlunge, Index, _T("N/A"));
	}

	// Front anti-drive
	if (car.drivetrain->driveType == Drivetrain::DriveRearWheel)
	{
		// Change the cell's text
		OutputsList->SetCellValue(KinematicOutputs::StartRightFrontDoubles
			+ KinematicOutputs::AntiDrivePitch, Index, _T("N/A"));
		OutputsList->SetCellValue(KinematicOutputs::StartLeftFrontDoubles
			+ KinematicOutputs::AntiDrivePitch, Index, _T("N/A"));
	}

	// Rear anti-drive
	if (car.drivetrain->driveType == Drivetrain::DriveFrontWheel)
	{
		// Change the cell's text
		OutputsList->SetCellValue(KinematicOutputs::StartRightRearDoubles
			+ KinematicOutputs::AntiDrivePitch, Index, _T("N/A"));
		OutputsList->SetCellValue(KinematicOutputs::StartLeftRearDoubles
			+ KinematicOutputs::AntiDrivePitch, Index, _T("N/A"));
	}

	// Front third spring
	if (!car.suspension->frontHasThirdSpring)
	{
		// Change the cell's text
		OutputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::FrontThirdSpring, Index, _T("N/A"));
		OutputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::FrontThirdShock, Index, _T("N/A"));
	}

	// Rear third spring
	if (!car.suspension->rearHasThirdSpring)
	{
		// Change the cell's text
		OutputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::RearThirdSpring, Index, _T("N/A"));
		OutputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::RearThirdShock, Index, _T("N/A"));
	}

	// Front sway bar
	if (car.suspension->frontBarStyle == Suspension::SwayBarNone)
	{
		// Change the cell's text
		OutputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::FrontARBMotionRatio, Index, _T("N/A"));
		OutputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::FrontARBTwist, Index, _T("N/A"));
	}

	// Rear sway bar
	if (car.suspension->rearBarStyle == Suspension::SwayBarNone)
	{
		// Change the cell's text
		OutputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::RearARBMotionRatio, Index, _T("N/A"));
		OutputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::RearARBTwist, Index, _T("N/A"));
	}

	// Make sure the correct column is highlighted
	HighlightColumn(MainFrame.GetObjectByIndex(MainFrame.GetActiveIndex())->GetCleanName());
}

//==========================================================================
// Class:			OUTPUT_PANEL
// Function:		FinishUpdate
//
// Description:		Second part of the panel update - this updates the units,
//					removes extra columns, and ends the batch edit for the grid.
//
// Input Arguments:
//		_NumberOfDataColumns	= int specifying how many data columns we should have
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void OUTPUT_PANEL::FinishUpdate(int _NumberOfDataColumns)
{
	// See if we need to remove any columns
	while (NumberOfDataColumns > _NumberOfDataColumns)
	{
		// Delete the unnecessary columns
		if (!OutputsList->DeleteCols(_NumberOfDataColumns + 1, NumberOfDataColumns - _NumberOfDataColumns))
		{
			// Delete columns failed - display warning and return
			debugger.Print(_T("Warning (OUTPUT_PANEL::FinishUpdate):  DeleteCols failed!"), Debugger::PriorityMedium);

			return;
		}

		// Decrement the number of data columns
		NumberOfDataColumns--;
	}

	// Update the column heading for the units column
	OutputsList->SetColLabelValue(NumberOfDataColumns + 1, _T("Units"));

	// Update all of the unit lables
	wxString UnitString;
	int i;
	for (i = 0; i < KinematicOutputs::NumberOfOutputScalars; i++)
	{
		// Set the unit label
		UnitString.Printf("(%s)", Converter.GetUnitType(KinematicOutputs::GetOutputUnitType(
			(KinematicOutputs::OutputsComplete)i)).c_str());
		OutputsList->SetCellValue(i, NumberOfDataColumns + 1, UnitString);
	}

	// End batch edit of the grid
	OutputsList->EndBatch();

	// Force a re-paint of this pane - for some reason this doesn't happen automatically
	Refresh();
	Update();
}

//==========================================================================
// Class:			OUTPUT_PANEL
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
void OUTPUT_PANEL::CreateControls()
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALL, 1);

	// Create the grid for the hard point entry
	OutputsList = new wxGrid(this, wxID_ANY);
	OutputsList->CreateGrid(KinematicOutputs::NumberOfOutputScalars, 2, wxGrid::wxGridSelectRows);

	// Begin a batch edit of the grid
	OutputsList->BeginBatch();

	// Hide the label column and set the size for the label row
	OutputsList->SetRowLabelSize(0);
	OutputsList->SetColLabelSize(OutputsList->GetRowSize(0));

	// To enable hiding of the non-label rows, we need to set the minimum height to zero
	OutputsList->SetRowMinimalAcceptableHeight(0);

	// Add the grid to the sizer
	MainSizer->Add(OutputsList, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALL | wxALIGN_TOP, 1);

	// Set the column headings
	OutputsList->SetColLabelValue(0, _T("Output"));
	OutputsList->SetColLabelValue(1, _T("Units"));

	// Do the processing that needs to be done for each row
	int i;
	for (i = 0; i < KinematicOutputs::NumberOfOutputScalars; i++)
	{
		// Make all cells read-only
		OutputsList->SetReadOnly(i, 0, true);
		OutputsList->SetReadOnly(i, 1, true);

		// Center the 'value' and 'units' columns
		OutputsList->SetCellAlignment(i, 1, wxALIGN_CENTER, wxALIGN_TOP);

		// Add the names of all of the points to the grid
		OutputsList->SetCellValue(i, 0, KinematicOutputs::GetOutputName((KinematicOutputs::OutputsComplete)i));
	}

	// Don't let the user move or re-size the rows and columns
	OutputsList->EnableDragColMove(false);
	OutputsList->EnableDragColSize(true);
	OutputsList->EnableDragGridSize(false);
	OutputsList->EnableDragRowSize(false);
	
	// Set the column widths automatically
	OutputsList->AutoSizeColumns();

	// End the batch mode edit and re-paint the control
	OutputsList->EndBatch();

	// Assign the top level sizer to the panel
	SetSizer(TopSizer);
}

//==========================================================================
// Class:			OUTPUT_PANEL
// Function:		ColumnResizeEvent
//
// Description:		Event handler for the column re-size event.
//
// Input Arguments:
//		event	= wxGridSizeEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void OUTPUT_PANEL::ColumnResizeEvent(wxGridSizeEvent& WXUNUSED(event))
{
	// Force a re-paint of the grid
	OutputsList->Refresh();
	OutputsList->Update();
}

//==========================================================================
// Class:			OUTPUT_PANEL
// Function:		HighlightColumn
//
// Description:		Highlights the column with the specified column heading
//
// Input Arguments:
//		_Name	= wxString specifying the column heading
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void OUTPUT_PANEL::HighlightColumn(wxString _Name)
{
	// Begin a batch edit of the grid
	OutputsList->BeginBatch();

	// Go through all of the data columns (it's safe to assume that the first column
	// will never be highlighted) and determine the correct cell color
	int Row, Col;
	wxColor Color;
	for (Col = 1; Col < OutputsList->GetNumberCols(); Col++)
	{
		// If the current column belongs to the specified car, use light grey,
		// otherwise, use white
		if (OutputsList->GetColLabelValue(Col).compare(_Name) != 0)
			Color = *wxWHITE;
		else
			Color.Set(220, 220, 220);

		for (Row = 0; Row < KinematicOutputs::NumberOfOutputScalars; Row++)
		{
			// If the cell is "Undef.", leave it alone (it will be yellow).
			// Otherwise, change it's color to the appropriate color
			if (OutputsList->GetCellValue(Row, Col).compare(_T("Undef.")) != 0)
				OutputsList->SetCellBackgroundColour(Row, Col, Color);
		}
	}

	// End the batch mode edit and re-paint the control
	OutputsList->EndBatch();

	// For some reason, this doesn't happen automatically
	Refresh();
	Update();
}