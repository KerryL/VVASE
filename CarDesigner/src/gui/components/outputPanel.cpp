/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  outputPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the OutputPanel class.  This
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
#include "vUtilities/unitConverter.h"
#include "gui/superGrid.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			OutputPanel
// Function:		OutputPanel
//
// Description:		Constructor for OutputPanel class.  Initializes the form
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
OutputPanel::OutputPanel(MainFrame &mainFrame, wxWindowID id, const wxPoint &pos,
	const wxSize &size): wxPanel(&mainFrame, id, pos, size), mainFrame(mainFrame)
{
	CreateControls();

	numberOfDataColumns = 0;
}

//==========================================================================
// Class:			OutputPanel
// Function:		~OutputPanel
//
// Description:		Destructor for OutputPanel class.
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
OutputPanel::~OutputPanel()
{
}

//==========================================================================
// Class:			OutputPanel
// Function:		Event Tables
//
// Description:		Event table for the OutputPanel class.
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
/*BEGIN_EVENT_TABLE(OutputPanel, wxPanel)
	EVT_SIZE(OutputPanel::OnSize)
END_EVENT_TABLE()*/

//==========================================================================
// Class:			OutputPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		outputs		= GuiCar::CarOutputs containing the outputs we want to use
//					  for the update
//		car			= Car& pointing to the car with which these outputs are associated
//		index		= int specifying the data column to edit
//		name		= wxString specifying the name of the car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void OutputPanel::UpdateInformation(GuiCar::CarOutputs outputs, Car &car,
									 int index, wxString name)
{
	// Begin batch edit of the grid
	outputsList->BeginBatch();

	// See if we need to add a column
	if (index > numberOfDataColumns)
	{
		// Add a column
		if (!outputsList->InsertCols(index))
		{
			// Add column failed - print warning and return
			Debugger::GetInstance() << "Warning (OutputPanel::UpdateInformation):  InsertCols failed!"
				<< Debugger::PriorityMedium;

			return;
		}

		// Set the column width
		outputsList->SetColSize(index, 50);// FIXME:  This should be set based on text width to make it cross-platform friendly

		// Increment the NumberOfDataColums
		numberOfDataColumns++;
	}

	// Update the name for this column (and make it read only, bold and centered)
	outputsList->SetColLabelValue(index, name);

	// Update all of the values and unit labels
	int i;
	for (i = 0; i < KinematicOutputs::NumberOfOutputScalars; i++)
	{
		// Convert and set the value
		outputsList->SetCellValue(i, index, UnitConverter::GetInstance().FormatNumber(
			UnitConverter::GetInstance().ConvertOutput(
			outputs.kinematicOutputs.GetOutputValue((KinematicOutputs::OutputsComplete)i),
			KinematicOutputs::GetOutputUnitType((KinematicOutputs::OutputsComplete)i))));

		// If the current value is undefined, make sure the user knows it
		if (VVASEMath::IsNaN(outputs.kinematicOutputs.GetOutputValue((KinematicOutputs::OutputsComplete)i)))
		{
			// Change the cell value "Undef." instead of #1.Q0
			outputsList->SetCellValue(i, index, _T("Undef."));

			// Make the cell background yellow
			outputsList->SetCellBackgroundColour(i, index, wxColor(255, 255, 0));
		}
		else
			// Make sure the background color isn't still set from a previously
			// undefined value
			outputsList->SetCellBackgroundColour(i, index, *wxWHITE);

		// Set the alignment
		outputsList->SetCellAlignment(i, index, wxALIGN_CENTER, wxALIGN_TOP);

		// Make the cell read-only
		outputsList->SetReadOnly(i, index, true);
	}
	
	for (i = KinematicOutputs::NumberOfOutputScalars; i < KinematicOutputs::NumberOfOutputScalars + 7; i++)
	{
		outputsList->SetCellBackgroundColour(i, index, *wxWHITE);
		outputsList->SetCellAlignment(i, index, wxALIGN_CENTER, wxALIGN_TOP);
		outputsList->SetReadOnly(i, index, true);
	}
	
	if (outputs.hasQuasiStaticOutputs)
	{
		outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars, index,
			UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertOutput(
			outputs.quasiStaticOutputs.pitch, UnitConverter::UnitTypeAngle)));
			
		outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 1, index,
			UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertOutput(
			outputs.quasiStaticOutputs.roll, UnitConverter::UnitTypeAngle)));
			
		outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 2, index,
			UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertOutput(
			outputs.quasiStaticOutputs.heave, UnitConverter::UnitTypeDistance)));
			
		outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 3, index,
			UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertOutput(
			outputs.quasiStaticOutputs.wheelLoads.leftFront, UnitConverter::UnitTypeForce)));
			
		outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 4, index,
			UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertOutput(
			outputs.quasiStaticOutputs.wheelLoads.rightFront, UnitConverter::UnitTypeForce)));
			
		outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 5, index,
			UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertOutput(
			outputs.quasiStaticOutputs.wheelLoads.leftRear, UnitConverter::UnitTypeForce)));
			
		outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 6, index,
			UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertOutput(
			outputs.quasiStaticOutputs.wheelLoads.rightRear, UnitConverter::UnitTypeForce)));
			
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars, outputsList->GetRowHeight(0));
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars + 1, outputsList->GetRowHeight(0));
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars + 2, outputsList->GetRowHeight(0));
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars + 3, outputsList->GetRowHeight(0));
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars + 4, outputsList->GetRowHeight(0));
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars + 5, outputsList->GetRowHeight(0));
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars + 6, outputsList->GetRowHeight(0));
	}
	else
	{
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars, 0);
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars + 1, 0);
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars + 2, 0);
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars + 3, 0);
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars + 4, 0);
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars + 5, 0);
		outputsList->SetRowHeight(KinematicOutputs::NumberOfOutputScalars + 6, 0);
	}

	// For values that do not pertain to this car (front axle plunge for a car without
	// front axles), change the text to "N/A".  Also make sure the cell background is
	// white - otherwise it might appear to be "Undef." instead of "N/A".

	// Get a lock on the car's mutex
	wxMutexLocker lock(car.GetMutex());
	DebugLog::GetInstance()->Log(_T("OutputPanel::UpdateInformation (locker)"));

	// Front half shafts
	if (!car.HasFrontHalfShafts())
	{
		// Change the cell's text
		outputsList->SetCellValue(KinematicOutputs::StartRightFrontDoubles
			+ KinematicOutputs::AxlePlunge, index, _T("N/A"));
		outputsList->SetCellValue(KinematicOutputs::StartLeftFrontDoubles
			+ KinematicOutputs::AxlePlunge, index, _T("N/A"));
	}

	// Rear half shafts
	if (!car.HasRearHalfShafts())
	{
		// Change the cell's text
		outputsList->SetCellValue(KinematicOutputs::StartRightRearDoubles
			+ KinematicOutputs::AxlePlunge, index, _T("N/A"));
		outputsList->SetCellValue(KinematicOutputs::StartLeftRearDoubles
			+ KinematicOutputs::AxlePlunge, index, _T("N/A"));
	}

	// Front anti-drive
	if (car.drivetrain->driveType == Drivetrain::DriveRearWheel)
	{
		// Change the cell's text
		outputsList->SetCellValue(KinematicOutputs::StartRightFrontDoubles
			+ KinematicOutputs::AntiDrivePitch, index, _T("N/A"));
		outputsList->SetCellValue(KinematicOutputs::StartLeftFrontDoubles
			+ KinematicOutputs::AntiDrivePitch, index, _T("N/A"));
	}

	// Rear anti-drive
	if (car.drivetrain->driveType == Drivetrain::DriveFrontWheel)
	{
		// Change the cell's text
		outputsList->SetCellValue(KinematicOutputs::StartRightRearDoubles
			+ KinematicOutputs::AntiDrivePitch, index, _T("N/A"));
		outputsList->SetCellValue(KinematicOutputs::StartLeftRearDoubles
			+ KinematicOutputs::AntiDrivePitch, index, _T("N/A"));
	}

	// Front third spring
	if (!car.suspension->frontHasThirdSpring)
	{
		// Change the cell's text
		outputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::FrontThirdSpring, index, _T("N/A"));
		outputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::FrontThirdDamper, index, _T("N/A"));
	}

	// Rear third spring
	if (!car.suspension->rearHasThirdSpring)
	{
		// Change the cell's text
		outputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::RearThirdSpring, index, _T("N/A"));
		outputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::RearThirdDamper, index, _T("N/A"));
	}

	// Front sway bar
	if (car.suspension->frontBarStyle == Suspension::SwayBarNone)
	{
		// Change the cell's text
		outputsList->SetCellValue(KinematicOutputs::StartLeftFrontDoubles
			+ KinematicOutputs::ARBInstallationRatio, index, _T("N/A"));
		outputsList->SetCellValue(KinematicOutputs::StartRightFrontDoubles
			+ KinematicOutputs::ARBInstallationRatio, index, _T("N/A"));
		outputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::FrontARBTwist, index, _T("N/A"));
	}

	// Rear sway bar
	if (car.suspension->rearBarStyle == Suspension::SwayBarNone)
	{
		// Change the cell's text
		outputsList->SetCellValue(KinematicOutputs::StartLeftRearDoubles
			+ KinematicOutputs::ARBInstallationRatio, index, _T("N/A"));
		outputsList->SetCellValue(KinematicOutputs::StartRightRearDoubles
			+ KinematicOutputs::ARBInstallationRatio, index, _T("N/A"));
		outputsList->SetCellValue(KinematicOutputs::StartDoubles
			+ KinematicOutputs::RearARBTwist, index, _T("N/A"));
	}

	// Make sure the correct column is highlighted
	HighlightColumn(mainFrame.GetObjectByIndex(mainFrame.GetActiveIndex())->GetCleanName());
}

//==========================================================================
// Class:			OutputPanel
// Function:		FinishUpdate
//
// Description:		Second part of the panel update - this updates the units,
//					removes extra columns, and ends the batch edit for the grid.
//
// Input Arguments:
//		numberOfDataColumns	= int specifying how many data columns we should have
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void OutputPanel::FinishUpdate(int numberOfDataColumns)
{
	while (this->numberOfDataColumns > numberOfDataColumns)
	{
		if (!outputsList->DeleteCols(numberOfDataColumns + 1, this->numberOfDataColumns - numberOfDataColumns))
		{
			Debugger::GetInstance() << "Warning (OutputPanel::FinishUpdate):  DeleteCols failed!" << Debugger::PriorityMedium;
			return;
		}

		this->numberOfDataColumns--;
	}

	outputsList->SetColLabelValue(this->numberOfDataColumns + 1, _T("Units"));

	// Update all of the unit labels
	wxString unitString;
	int i;
	for (i = 0; i < KinematicOutputs::NumberOfOutputScalars; i++)
	{
		unitString.Printf("(%s)", UnitConverter::GetInstance().GetUnitType(KinematicOutputs::GetOutputUnitType(
			(KinematicOutputs::OutputsComplete)i)).c_str());
		outputsList->SetCellValue(i, this->numberOfDataColumns + 1, unitString);
	}
	
	unitString.Printf("(%s)", UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeAngle).c_str());
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars, this->numberOfDataColumns + 1, unitString);
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 1, this->numberOfDataColumns + 1, unitString);
	
	unitString.Printf("(%s)", UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance).c_str());
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 2, this->numberOfDataColumns + 1, unitString);
	
	unitString.Printf("(%s)", UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeForce).c_str());
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 3, this->numberOfDataColumns + 1, unitString);
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 4, this->numberOfDataColumns + 1, unitString);
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 5, this->numberOfDataColumns + 1, unitString);
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 6, this->numberOfDataColumns + 1, unitString);

	// End batch edit of the grid
	outputsList->EndBatch();

	// Force a re-paint of this pane - for some reason this doesn't happen automatically
	Refresh();
	Update();
}

//==========================================================================
// Class:			OutputPanel
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
void OutputPanel::CreateControls()
{
	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 1, wxGROW | wxALL, 1);

	// Create the grid for the hard point entry
	// Extra 7 rows for quasi-static outputs
	const unsigned int numberOfQuasiStaticOutputs(7);
	outputsList = new SuperGrid(this, wxID_ANY);
	outputsList->CreateGrid(KinematicOutputs::NumberOfOutputScalars + numberOfQuasiStaticOutputs, 2, wxGrid::wxGridSelectRows);

	// Begin a batch edit of the grid
	outputsList->BeginBatch();

	// Hide the label column and set the size for the label row
	outputsList->SetRowLabelSize(0);
	outputsList->SetColLabelSize(outputsList->GetRowSize(0));

	// To enable hiding of the non-label rows, we need to set the minimum height to zero
	outputsList->SetRowMinimalAcceptableHeight(0);

	// Add the grid to the sizer
	mainSizer->Add(outputsList, 1, wxGROW | wxALL | wxALIGN_TOP, 1);

	// Set the column headings
	outputsList->SetColLabelValue(0, _T("Output"));
	outputsList->SetColLabelValue(1, _T("Units"));

	// Do the processing that needs to be done for each row
	int i;
	for (i = 0; i < KinematicOutputs::NumberOfOutputScalars + numberOfQuasiStaticOutputs; i++)
	{
		// Make all cells read-only
		outputsList->SetReadOnly(i, 0, true);
		outputsList->SetReadOnly(i, 1, true);

		// Center the 'value' and 'units' columns
		outputsList->SetCellAlignment(i, 1, wxALIGN_CENTER, wxALIGN_TOP);

		// Add the names of all of the points to the grid
		if (i < KinematicOutputs::NumberOfOutputScalars)
			outputsList->SetCellValue(i, 0, KinematicOutputs::GetOutputName((KinematicOutputs::OutputsComplete)i));
	}
	
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars, 0, _T("Pitch"));
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 1, 0, _T("Roll"));
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 2, 0, _T("Heave"));
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 3, 0, _T("LF Tire Load"));
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 4, 0, _T("RF Tire Load"));
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 5, 0, _T("LR Tire Load"));
	outputsList->SetCellValue(KinematicOutputs::NumberOfOutputScalars + 6, 0, _T("RR Tire Load"));

	// Don't let the user move or re-size the rows and columns
	outputsList->EnableDragColMove(false);
	outputsList->EnableDragColSize(true);
	outputsList->EnableDragGridSize(false);
	outputsList->EnableDragRowSize(false);
	
	// Set the column widths automatically
	outputsList->AutoSizeColumns();

	// End the batch mode edit and re-paint the control
	outputsList->EndBatch();

	// Assign the top level sizer to the panel
	SetSizer(topSizer);
}

//==========================================================================
// Class:			OutputPanel
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
void OutputPanel::ColumnResizeEvent(wxGridSizeEvent& WXUNUSED(event))
{
	// Force a re-paint of the grid
	outputsList->Refresh();
	outputsList->Update();
}

//==========================================================================
// Class:			OutputPanel
// Function:		HighlightColumn
//
// Description:		Highlights the column with the specified column heading
//
// Input Arguments:
//		_name	= wxString specifying the column heading
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void OutputPanel::HighlightColumn(wxString name)
{
	// Begin a batch edit of the grid
	outputsList->BeginBatch();

	// Go through all of the data columns (it's safe to assume that the first column
	// will never be highlighted) and determine the correct cell color
	int row, col;
	wxColor color;
	for (col = 1; col < outputsList->GetNumberCols(); col++)
	{
		// If the current column belongs to the specified car, use light grey,
		// otherwise, use white
		if (outputsList->GetColLabelValue(col).compare(name) != 0)
			color = *wxWHITE;
		else
			color.Set(220, 220, 220);

		for (row = 0; row < KinematicOutputs::NumberOfOutputScalars; row++)
		{
			// If the cell is "Undef.", leave it alone (it will be yellow).
			// Otherwise, change it's color to the appropriate color
			if (outputsList->GetCellValue(row, col).compare(_T("Undef.")) != 0)
				outputsList->SetCellBackgroundColour(row, col, color);
		}
	}

	// End the batch mode edit and re-paint the control
	outputsList->EndBatch();

	// For some reason, this doesn't happen automatically
	Refresh();
	Update();
}