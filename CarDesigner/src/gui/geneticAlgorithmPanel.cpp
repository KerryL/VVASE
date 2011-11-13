/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  geneticAlgorithmPanel.cpp
// Created:  7/30/2009
// Author:  K. Loux
// Description:  Contains the class definitions for a panel that allows users to specify
//				 the parameters, genes and goals of a genetic algorithm.
// History:

// wxWidgets headers
#include <wx/grid.h>
#include <wx/gauge.h>

// CarDesigner headers
#include "gui/geneticAlgorithmPanel.h"
#include "gui/components/mainFrame.h"
#include "gui/dialogs/gaGoalDialog.h"
#include "gui/dialogs/gaGeneDialog.h"
#include "gui/guiCar.h"
#include "gui/geneticOptimization.h"
#include "gui/gaObject.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vCar/corner.h"

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		GENETIC_ALGORITHM_PANEL
//
// Description:		Constructor for the GENETIC_ALGORITHM_PANEL class.
//
// Input Arguments:
//		_MainFrame		= MAIN_FRAME&, reference to the main application window
//		_Optimization	= GENETIC_OPTIMIZATION&, reference to the object that we represent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GENETIC_ALGORITHM_PANEL::GENETIC_ALGORITHM_PANEL(MAIN_FRAME &_MainFrame,
												 GENETIC_OPTIMIZATION &_Optimization)
												 : wxPanel(&_MainFrame),
												 Converter(_MainFrame.GetConverter()),
												 Optimization(_Optimization), MainFrame(_MainFrame)
{
	// Initialize the overall progress control pointer
	// (This is used as in indication that the panel is completely built)
	OverallProgress = NULL;

	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		GENETIC_ALGORITHM_PANEL
//
// Description:		Destructor for the GENETIC_ALGORITHM_PANEL class.
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
GENETIC_ALGORITHM_PANEL::~GENETIC_ALGORITHM_PANEL()
{
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
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
BEGIN_EVENT_TABLE(GENETIC_ALGORITHM_PANEL, wxPanel)
	// Buttons
	EVT_BUTTON(IdAddGene,					GENETIC_ALGORITHM_PANEL::AddGeneButtonClickedEvent)
	EVT_BUTTON(IdEditGene,					GENETIC_ALGORITHM_PANEL::EditGeneButtonClickedEvent)
	EVT_BUTTON(IdRemoveGene,				GENETIC_ALGORITHM_PANEL::RemoveGeneButtonClickedEvent)
	EVT_BUTTON(IdAddGoal,					GENETIC_ALGORITHM_PANEL::AddGoalButtonClickedEvent)
	EVT_BUTTON(IdEditGoal,					GENETIC_ALGORITHM_PANEL::EditGoalButtonClickedEvent)
	EVT_BUTTON(IdRemoveGoal,				GENETIC_ALGORITHM_PANEL::RemoveGoalButtonClickedEvent)
	EVT_BUTTON(IdStartStopOptimization,		GENETIC_ALGORITHM_PANEL::StartStopOptimizationClickedEvent)
	EVT_GRID_CMD_SELECT_CELL(IdGeneList,	GENETIC_ALGORITHM_PANEL::GeneListSelectCellEvent)
	EVT_GRID_CMD_SELECT_CELL(IdGoalList,	GENETIC_ALGORITHM_PANEL::GoalListSelectCellEvent)
	EVT_TEXT(wxID_ANY,						GENETIC_ALGORITHM_PANEL::TextBoxChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		CreateControls
//
// Description:		Creates the controls for this dialog.
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
void GENETIC_ALGORITHM_PANEL::CreateControls(void)
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 1, wxALIGN_LEFT | wxGROW | wxALL, 5);
	
	// Set the sizer flags
	int labelSizerFlags = wxALIGN_CENTER_VERTICAL;
	int inputSizerFlags = wxEXPAND | wxALIGN_CENTER_VERTICAL;
	int padding(2);

	// Create a sizer at the top to contain the GA parameters and the start/stop button
	wxFlexGridSizer *UpperSizer = new wxFlexGridSizer(3, padding, padding);
	UpperSizer->SetFlexibleDirection(wxHORIZONTAL);
	MainSizer->Add(UpperSizer, 0, wxALL | wxGROW, 5);

	// Population size
	wxStaticText *PopulationLabel = new wxStaticText(this, wxID_ANY, _T("Population Size"));
	PopulationSize = new wxTextCtrl(this, wxID_ANY, _T("500"));
	UpperSizer->Add(PopulationLabel, 0, labelSizerFlags);
	UpperSizer->Add(PopulationSize, 0, inputSizerFlags);
	
	// Start/stop button
	StartStopOptimization = new wxButton(this, IdStartStopOptimization, _T("Start GA"));
	UpperSizer->Add(StartStopOptimization, 0, wxALIGN_LEFT);

	// Generation limit
	wxStaticText *GenerationLimitLabel = new wxStaticText(this, wxID_ANY, _T("Generation Limit"));
	GenerationLimit = new wxTextCtrl(this, wxID_ANY, _T("20"));
	UpperSizer->Add(GenerationLimitLabel, 0, labelSizerFlags);
	UpperSizer->Add(GenerationLimit, 0, inputSizerFlags);
	UpperSizer->AddSpacer(-1);

	// Elitism percentage
	wxStaticText *ElitismLabel = new wxStaticText(this, wxID_ANY, _T("Elitism Fraction"));
	ElitismFraction = new wxTextCtrl(this, wxID_ANY, _T("0.05"));
	UpperSizer->Add(ElitismLabel, 0, labelSizerFlags);
	UpperSizer->Add(ElitismFraction, 0, inputSizerFlags);
	UpperSizer->AddSpacer(-1);

	// Mutation probability
	wxStaticText *MutationLabel = new wxStaticText(this, wxID_ANY, _T("Mutation Probability"));
	MutationProbability = new wxTextCtrl(this, wxID_ANY, _T("0.01"));
	UpperSizer->Add(MutationLabel, 0, labelSizerFlags);
	UpperSizer->Add(MutationProbability, 0, inputSizerFlags);
	UpperSizer->AddSpacer(-1);

	// Crossover point
	wxStaticText *CrossoverLabel = new wxStaticText(this, wxID_ANY, _T("Crossover Point"));
	CrossoverPoint = new wxTextCtrl(this, wxID_ANY, _T("0"));
	UpperSizer->Add(CrossoverLabel, 0, labelSizerFlags);
	UpperSizer->Add(CrossoverPoint, 0, inputSizerFlags);
	UpperSizer->AddSpacer(-1);

	// Selection drop down box
	wxStaticText *SelectionLabel = new wxStaticText(this, wxID_ANY, _T("Car to Optimize"));
	SelectedCar = new wxComboBox(this, IdSelectCar, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, NULL, wxCB_READONLY);
	UpperSizer->Add(SelectionLabel, 0, labelSizerFlags);
	UpperSizer->Add(SelectedCar, 0, inputSizerFlags);

	// Gene list
	wxBoxSizer *GeneSizer = new wxBoxSizer(wxHORIZONTAL);
	MainSizer->Add(GeneSizer, 1, wxGROW | wxALL, 5);
	GeneList = new wxGrid(this, IdGeneList);
	GeneList->CreateGrid(1, 6, wxGrid::wxGridSelectRows);

	// Begin a batch edit of the grid
	GeneList->BeginBatch();

	// Make all cells read-only and center the text
	int i, j;
	for (i = 0; i < GeneList->GetNumberRows(); i++)
	{
		for (j = 0; j < GeneList->GetNumberCols(); j++)
		{
			GeneList->SetReadOnly(i, j, true);
			GeneList->SetCellAlignment(wxALIGN_CENTER, i, j);
		}
	}

	// Hide the label column and set the size for the label row
	GeneList->SetRowLabelSize(0);
	GeneList->SetColLabelSize(GeneList->GetRowSize(0));

	// Add the grid to the sizer
	GeneSizer->Add(GeneList, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW
		| wxALL | wxALIGN_TOP, 5);

	// Set the column headings
	GeneList->SetColLabelValue(0, _T("Hardpoint"));
	GeneList->SetColLabelValue(1, _T("Alternate With"));
	GeneList->SetColLabelValue(2, _T("Axis Direction"));
	GeneList->SetColLabelValue(3, _T("Minimum"));
	GeneList->SetColLabelValue(4, _T("Maximum"));
	GeneList->SetColLabelValue(5, _T("Number Of Values"));

	// Assign rights for the user move or re-size the rows or move the columns
	GeneList->EnableDragColMove(false);
	GeneList->EnableDragColSize(true);
	GeneList->EnableDragGridSize(false);
	GeneList->EnableDragRowSize(false);

	// Auto-size the grid columns
	// FIXME:  This doesn't work the way we would like - it just sizes to column headings, not column contents
	// Workaround is below
	GeneList->AutoSizeColumns(false);

	// Assign column widths
	// FIXME:  We would rather use auto-size (this is workaround for above)
	GeneList->SetColumnWidth(0, 140);
	GeneList->SetColumnWidth(1, 140);
	// Default size (after auto-size) is OK for the remaining columns

	// End the batch mode edit and re-paint the control
	GeneList->EndBatch();

	// Add/Edit/Remove Gene buttons
	wxBoxSizer *GeneButtonSizer = new wxBoxSizer(wxVERTICAL);
	GeneSizer->Add(GeneButtonSizer);
	AddGene = new wxButton(this, IdAddGene, _T("Add Gene"));
	EditGene = new wxButton(this, IdEditGene, _T("Edit Gene"));
	RemoveGene = new wxButton(this, IdRemoveGene, _T("Remove Gene"));
	GeneButtonSizer->Add(AddGene, 0, wxEXPAND);
	GeneButtonSizer->Add(EditGene, 0, wxEXPAND);
	GeneButtonSizer->Add(RemoveGene, 0, wxEXPAND);

	// Goal list
	wxBoxSizer *GoalSizer = new wxBoxSizer(wxHORIZONTAL);
	MainSizer->Add(GoalSizer, 1, wxGROW | wxALL, 5);
	GoalList = new wxGrid(this, IdGoalList);
	GoalList->CreateGrid(1, 6, wxGrid::wxGridSelectRows);

	// Begin a batch edit of the grid
	GoalList->BeginBatch();

	// Make all cells read-only
	for (i = 0; i < GoalList->GetNumberRows(); i++)
	{
		for (j = 0; j < GoalList->GetNumberCols(); j++)
		{
			GoalList->SetReadOnly(i, j, true);
			GoalList->SetCellAlignment(wxALIGN_CENTER, i, j);
		}
	}

	// Hide the label column and set the size for the label row
	GoalList->SetRowLabelSize(0);
	GoalList->SetColLabelSize(GoalList->GetRowSize(0));

	// Add the grid to the sizer
	GoalSizer->Add(GoalList, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW
		| wxALL | wxALIGN_TOP, 5);

	// Set the column headings
	GoalList->SetColLabelValue(0, _T("Output"));
	GoalList->SetColLabelValue(1, _T("State 1 Inputs"));
	GoalList->SetColLabelValue(2, _T("State 2 Inputs"));
	GoalList->SetColLabelValue(3, _T("Desired Value"));
	GoalList->SetColLabelValue(4, _T("Expected Deviation"));
	GoalList->SetColLabelValue(5, _T("Importance"));

	// Assign rights for the user move or re-size the rows or move the columns
	GoalList->EnableDragColMove(false);
	GoalList->EnableDragColSize(true);
	GoalList->EnableDragGridSize(false);
	GoalList->EnableDragRowSize(false);

	// Auto-size the grid columns
	// FIXME:  This doesn't work the way we would like - it just sizes to column headings, not column contents
	// Workaround is below
	GoalList->AutoSizeColumns(false);

	// Assign column widths
	// FIXME:  We would rather do this automatically (this is workaround for above)
	GoalList->SetColumnWidth(0, 190);
	GoalList->SetColumnWidth(1, 220);
	GoalList->SetColumnWidth(2, 220);
	// Default size (after auto-size) is OK for the remaining columns

	// End the batch mode edit and re-paint the control
	GoalList->EndBatch();

	// Add/Edit/Remove Goal buttons
	wxBoxSizer *GoalButtonSizer = new wxBoxSizer(wxVERTICAL);
	GoalSizer->Add(GoalButtonSizer);
	AddGoal = new wxButton(this, IdAddGoal, _T("Add Goal"));
	EditGoal = new wxButton(this, IdEditGoal, _T("Edit Goal"));
	RemoveGoal = new wxButton(this, IdRemoveGoal, _T("Remove Goal"));
	GoalButtonSizer->Add(AddGoal, 0, wxEXPAND);
	GoalButtonSizer->Add(EditGoal, 0, wxEXPAND);
	GoalButtonSizer->Add(RemoveGoal, 0, wxEXPAND);

	// Progress bars
	wxFlexGridSizer *ProgressSizer = new wxFlexGridSizer(2, 2, 5);
	ProgressSizer->SetFlexibleDirection(wxHORIZONTAL);
	MainSizer->Add(ProgressSizer, 0, wxALL | wxEXPAND | wxALIGN_BOTTOM, 5);
	ProgressSizer->AddGrowableCol(1);

	wxStaticText *GenerationProgressLabel = new wxStaticText(this, wxID_ANY, _T("Current Generation"));
	GenerationProgress = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition,
		wxSize(-1, GenerationProgressLabel->GetSize().GetHeight()), wxGA_HORIZONTAL | wxGA_SMOOTH);
	ProgressSizer->Add(GenerationProgressLabel, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	ProgressSizer->Add(GenerationProgress, 0, wxEXPAND);

	wxStaticText *OverallProgressLabel = new wxStaticText(this, wxID_ANY, _T("Overall"));
	OverallProgress = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition,
		wxSize(-1, OverallProgressLabel->GetSize().GetHeight()), wxGA_HORIZONTAL | wxGA_SMOOTH);
	ProgressSizer->Add(OverallProgressLabel, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	ProgressSizer->Add(OverallProgress, 0, wxEXPAND);

	// Assign the top level sizer to the dialog
	SetSizer(TopSizer);

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		AddGeneButtonClickedEvent
//
// Description:		Event handler for the Add Gene button.  Displays the
//					GA_GENE_DIALOG to allow the user to create the gene
//					they wish to add.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM_PANEL::AddGeneButtonClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create the dialog box with default gene properties
	GA_GENE_DIALOG GeneDialog(static_cast<wxWindow*>(&MainFrame), Converter, (CORNER::HARDPOINTS)0,
		(CORNER::HARDPOINTS)0, (Vector::Axis)0, (CORNER::LOCATION)0, 0.0, 1.0, 5, wxID_ANY, wxDefaultPosition);

	// Display the dialog
	if (GeneDialog.ShowModal() == wxOK)
	{
		// Create the new gene and add it to the list
		Optimization.GetAlgorithm().AddGene(GeneDialog.GetHardpoint(), GeneDialog.GetTiedTo(), GeneDialog.GetCornerLocation(),
			GeneDialog.GetAxisDirection(), GeneDialog.GetMinimum(), GeneDialog.GetMaximum(), GeneDialog.GetNumberOfValues());

		// Update the display for the gene
		UpdateGeneList();

		// Mark the optimization as modified
		Optimization.SetModified();
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		EditGeneButtonClickedEvent
//
// Description:		Event handler for the Edit Gene button.  Displays the
//					GA_GENE_DIALOG to allow the user to edit the gene
//					they selected.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM_PANEL::EditGeneButtonClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure exactly ONE row is selected
	if (!GeneList->IsSelection() || GeneList->GetSelectedRows().GetCount() != 1)
		return;

	// Get the selected gene
	GA_OBJECT::GENE GeneToEdit = Optimization.GetAlgorithm().GetGene(GeneList->GetSelectedRows()[0]);

	// Create the dialog box with properties corresponding to the selected gene
	GA_GENE_DIALOG GeneDialog(static_cast<wxWindow*>(&MainFrame), Converter, GeneToEdit.Hardpoint,
		GeneToEdit.TiedTo, GeneToEdit.Direction, GeneToEdit.Location, GeneToEdit.Minimum, GeneToEdit.Maximum,
		GeneToEdit.NumberOfValues, wxID_ANY, wxDefaultPosition);

	// Display the dialog
	if (GeneDialog.ShowModal() == wxOK)
	{
		// Update the gene
		Optimization.GetAlgorithm().UpdateGene(GeneList->GetSelectedRows()[0], GeneDialog.GetHardpoint(),
			GeneDialog.GetTiedTo(), GeneDialog.GetCornerLocation(), GeneDialog.GetAxisDirection(),
			GeneDialog.GetMinimum(), GeneDialog.GetMaximum(), GeneDialog.GetNumberOfValues());

		// Update the display for the gene
		UpdateGeneList();

		// Mark the optimization as modified
		Optimization.SetModified();
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		RemoveGeneButtonClickedEvent
//
// Description:		Event handler for the Remove Gene button.  Removes the
//					selected gene from the list.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM_PANEL::RemoveGeneButtonClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure a row is selected
	if (!GeneList->IsSelection())
		return;

	// Make sure a gene exists (there is an empty row when the grid is first created -
	// clicking on it and trying to remove it would crash us)
	if (Optimization.GetAlgorithm().GetGeneCount() == 0)
		return;

	// Remove the selected gene(s)
	unsigned int i;
	for (i = 0; i < GeneList->GetSelectedRows().GetCount(); i++)
		Optimization.GetAlgorithm().RemoveGene(GeneList->GetSelectedRows()[i]);

	// Update the list
	UpdateGeneList();

	// Mark the optimization as modified
	Optimization.SetModified();

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		AddGoalButtonClickedEvent
//
// Description:		Event handler for the Add Goal button.  Displays the
//					GA_GOAL_DIALOG to allow the user to create the goal
//					they wish to add.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM_PANEL::AddGoalButtonClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Set up the default kinematic inputs
	KINEMATICS::INPUTS DefaultInputs = MainFrame.GetInputs();
	DefaultInputs.Heave = 0.0;
	DefaultInputs.Pitch = 0.0;
	DefaultInputs.Roll = 0.0;
	DefaultInputs.RackTravel = 0.0;

	// Create the dialog box with default goal properties
	GA_GOAL_DIALOG GoalDialog(static_cast<wxWindow*>(&MainFrame), Converter, (KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)0, 0.0,
		0.0, 1.0, DefaultInputs, DefaultInputs, wxID_ANY, wxDefaultPosition);

	// Display the dialog
	if (GoalDialog.ShowModal() == wxOK)
	{
		// Create the new goal and add it to the list
		Optimization.GetAlgorithm().AddGoal(GoalDialog.GetOutput(), GoalDialog.GetDesiredValue(),
			GoalDialog.GetExpectedDeviation(), GoalDialog.GetImportance(), GoalDialog.GetBeforeInputs(),
			GoalDialog.GetAfterInputs());

		// Update the display for the goal
		UpdateGoalList();

		// Mark the optimization as modified
		Optimization.SetModified();
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		EditGoalButtonClickedEvent
//
// Description:		Event handler for the Edit Goal button.  Displays the
//					GA_GOAL_DIALOG to allow the user to edit the goal
//					they selected.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM_PANEL::EditGoalButtonClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure exactly ONE row is selected
	if (!GoalList->IsSelection() || GoalList->GetSelectedRows().GetCount() != 1)
		return;

	// Get the selected goal
	GA_OBJECT::GOAL GoalToEdit = Optimization.GetAlgorithm().GetGoal(GoalList->GetSelectedRows()[0]);

	// Create the dialog box with properties corresponding to the selected goal
	GA_GOAL_DIALOG GoalDialog(static_cast<wxWindow*>(&MainFrame), Converter, GoalToEdit.Output, GoalToEdit.DesiredValue,
		GoalToEdit.ExpectedDeviation, GoalToEdit.Importance, GoalToEdit.BeforeInputs, GoalToEdit.AfterInputs,
		wxID_ANY, wxDefaultPosition);

	// Display the dialog
	if (GoalDialog.ShowModal() == wxOK)
	{
		// Create the new goal and add it to the list
		Optimization.GetAlgorithm().UpdateGoal(GoalList->GetSelectedRows()[0], GoalDialog.GetOutput(),
			GoalDialog.GetDesiredValue(), GoalDialog.GetExpectedDeviation(), GoalDialog.GetImportance(),
			GoalDialog.GetBeforeInputs(), GoalDialog.GetAfterInputs());

		// Update the display for the goal
		UpdateGoalList();

		// Mark the optimization as modified
		Optimization.SetModified();
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		RemoveGoalButtonClickedEvent
//
// Description:		Event handler for the Remove Goal button.  Removes the
//					selected goal from the list.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM_PANEL::RemoveGoalButtonClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure a row is selected
	if (!GoalList->IsSelection())
		return;

	// Make sure a goal exists (there is an empty row when the grid is first created -
	// clicking on it and trying to remove it would crash us)
	if (Optimization.GetAlgorithm().GetGoalCount() == 0)
		return;

	// Remove the selected goal(s)
	unsigned int i;
	for (i = 0; i < GoalList->GetSelectedRows().GetCount(); i++)
		Optimization.GetAlgorithm().RemoveGoal(GoalList->GetSelectedRows()[i]);

	// Update the list
	UpdateGoalList();

	// Mark the optimization as modified
	Optimization.SetModified();

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		StartStopOptimizationClickedEvent
//
// Description:		Event handler for the Start/Stop optimization button.
//					Controls operation of optimization algorithm.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM_PANEL::StartStopOptimizationClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Determine if we should be starting or stopping the optimization
	if (Optimization.GetAlgorithm().OptimizationIsRunning())
	{
		// Stop the optimization
		Optimization.HaltOptimization();

		// Change the caption on the button
		// FIXME:  Capture event when GA ends to do this!!!
		//StartStopOptimization->SetLabel(_T("Start GA"));
	}
	else
	{
		// Make sure all of our parameters are valid
		if (!UpdateGAParameters())
			return;

		// Make sure a car is selected
		if (SelectedCar->GetValue().IsEmpty())
		{
			// Display a message box to warn the user
			wxMessageBox(_T("ERROR:  No car selected to optimize!"), _T("No Selected Car"),
				wxOK | wxICON_ERROR, this);

			return;
		}

		// Make sure we have some genes and goals defined
		if (Optimization.GetAlgorithm().GetGeneCount() == 0 ||
			Optimization.GetAlgorithm().GetGoalCount() == 0)
		{
			// Display a message box to warn the user
			wxMessageBox(_T("ERROR:  Optimization requires at least one gene and one goal!"), _T("Goal and Gene Required"),
				wxOK | wxICON_ERROR, this);

			return;
		}

		// Assign our car to optimize
		// This is necessary for handling GUI interaction and displays
		int i(0), CarIndex(-1);
		for (i = 0; i < MainFrame.GetObjectCount(); i++)
		{
			// If the object with index i is a car, check to see if it is the selected car
			if (MainFrame.GetObjectByIndex(i)->GetType() == GUI_OBJECT::TYPE_CAR)
			{
				CarIndex++;

				// FIXME:  Under GTK, GetSelection only returns the correct index if the combobox has focus?
				// This is the reason for the #ifdef __WXGTK__ below
				if (CarIndex == SelectedCar->GetSelection())
					break;
			}
		}

#ifdef __WXGTK__
		// FIXME:  Under GTK, GetSelection only works the first time (while it has focus?)
		// See note above
		if (i > MainFrame.GetObjectCount())
		{
			wxMessageBox(_T("Error identifying associated car!  Please re-select."));
			return;
		}
#endif

		Optimization.SetCarToOptimize(static_cast<GUI_CAR*>(MainFrame.GetObjectByIndex(i)));

		// Set the parameters for the analysis
		Optimization.GetAlgorithm().SetUp(&(Optimization.GetCarToOptimize()->GetOriginalCar()));

		// Reset the status bars (must occur after algorithm is SetUp() so we know
		// how many analyses to expect
		ResetStatusBars();

		// Start the analysis
		Optimization.BeginOptimization();

		// Change the caption on the button
		// FIXME:  Capture event when GA starts to do this!!!
		//StartStopOptimization->SetLabel(_T("Stop Optimization"));
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		UpdateSelectableCars
//
// Description:		Updates the list of cars available for optimization.
//					Every open car will be added to the list.
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
void GENETIC_ALGORITHM_PANEL::UpdateSelectableCars(void)
{
	// NOTE:  If cars are opened or closed, this should follow the associated car, even though
	// we may need to change the index to do so
	// Check iterations for this behavior, too!

	// Store the current name
	wxString CurrentName = SelectedCar->GetValue();

	bool FoundOriginalCar(false);
	if (CurrentName.IsEmpty())
		FoundOriginalCar = true;

	// Clear out the list and re-populate it with the currenly open cars
	SelectedCar->Clear();
	int i;
	for (i = 0; i < MainFrame.GetObjectCount(); i++)
	{
		// If the current object is a car, add it to the list
		if (MainFrame.GetObjectByIndex(i)->GetType() == GUI_OBJECT::TYPE_CAR)
		{
			SelectedCar->Append(MainFrame.GetObjectByIndex(i)->GetCleanName());
			if (CurrentName.Cmp(MainFrame.GetObjectByIndex(i)->GetCleanName()) == 0)
				FoundOriginalCar = true;
		}
	}

	// Re-selecte the originally selected car
	if (FoundOriginalCar)
		SelectedCar->SetValue(CurrentName);
	else
	{
		// Handle the error if the original car is now gone
		// FIXME!!!
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		UpdateInformation
//
// Description:		Updates the parameters associated with optimization.  This
//					includes the gene and goal lists.
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
void GENETIC_ALGORITHM_PANEL::UpdateInformation(void)
{
	// Get the GA object from the gui's optimization object
	wxString Temp;

	// Update parameters
	Temp.Printf("%i", Optimization.GetAlgorithm().GetPopulationSize());
	PopulationSize->ChangeValue(Temp);
	Temp.Printf("%i", Optimization.GetAlgorithm().GetGenerationLimit());
	GenerationLimit->ChangeValue(Temp);
	ElitismFraction->ChangeValue(Converter.FormatNumber(Optimization.GetAlgorithm().GetElitismPercentage()));
	MutationProbability->ChangeValue(Converter.FormatNumber(Optimization.GetAlgorithm().GetMutationProbability()));
	
	Temp.Printf("%i", Optimization.GetAlgorithm().GetCrossoverPoint());
	CrossoverPoint->ChangeValue(Temp);

	// Update the rest of the panel
	UpdateGeneList();
	UpdateGoalList();
	UpdateSelectableCars();

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		UpdateGeneList
//
// Description:		Updates the gene list to match the genes present in the
//					associated GA_OBJECT.
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
void GENETIC_ALGORITHM_PANEL::UpdateGeneList(void)
{
	// Make sure we have the correct number of rows
	if (GeneList->GetRows() > Optimization.GetAlgorithm().GetGeneCount()
		&& Optimization.GetAlgorithm().GetGeneCount() > 1)
		GeneList->DeleteRows(0, GeneList->GetRows() - Optimization.GetAlgorithm().GetGeneCount(), false);
	else if (GeneList->GetRows() < Optimization.GetAlgorithm().GetGeneCount())
		GeneList->InsertRows(0, Optimization.GetAlgorithm().GetGeneCount() - GeneList->GetRows(), false);

	// Go through row-by-row and populate the cells with the correct information
	wxString Temp;
	int i, j;
	for (i = 0; i < Optimization.GetAlgorithm().GetGeneCount(); i++)
	{
		GeneList->SetCellValue(i, 0, CORNER::GetHardpointName(Optimization.GetAlgorithm().GetGene(i).Hardpoint));
		if (Optimization.GetAlgorithm().GetGene(i).Hardpoint == Optimization.GetAlgorithm().GetGene(i).TiedTo)
			GeneList->SetCellValue(i, 1, _T("None"));
		else
			GeneList->SetCellValue(i, 1, CORNER::GetHardpointName(Optimization.GetAlgorithm().GetGene(i).TiedTo));
		GeneList->SetCellValue(i, 2, Vector::GetAxisName(Optimization.GetAlgorithm().GetGene(i).Direction));
		GeneList->SetCellValue(i, 3, Converter.FormatNumber(
			Converter.ConvertDistance(Optimization.GetAlgorithm().GetGene(i).Minimum)));
		GeneList->SetCellValue(i, 4, Converter.FormatNumber(
			Converter.ConvertDistance(Optimization.GetAlgorithm().GetGene(i).Maximum)));
		Temp.Printf("%i", Optimization.GetAlgorithm().GetGene(i).NumberOfValues);
		GeneList->SetCellValue(i, 5, Temp);

		// Make the cells read-only and center the text
		for (j = 0; j < GeneList->GetCols(); j++)
		{
			GeneList->SetReadOnly(i, j, true);
			GeneList->SetCellAlignment(wxALIGN_CENTER, i, j);
		}
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		UpdateGoalList
//
// Description:		Updates the goal list to match the goals present in the
//					associated GA_OBJECT.
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
void GENETIC_ALGORITHM_PANEL::UpdateGoalList(void)
{
	// Make sure we have the correct number of rows
	if (GoalList->GetRows() > Optimization.GetAlgorithm().GetGoalCount()
		&& Optimization.GetAlgorithm().GetGoalCount() > 1)
		GoalList->DeleteRows(0, GoalList->GetRows() - Optimization.GetAlgorithm().GetGoalCount(), false);
	else if (GoalList->GetRows() < Optimization.GetAlgorithm().GetGoalCount())
		GoalList->InsertRows(0, Optimization.GetAlgorithm().GetGoalCount() - GoalList->GetRows(), false);

	// Go through row-by-row and populate the cells with the correct information
	int i, j;
	for (i = 0; i < Optimization.GetAlgorithm().GetGoalCount(); i++)
	{
		GoalList->SetCellValue(i, 0, KINEMATIC_OUTPUTS::GetOutputName(Optimization.GetAlgorithm().GetGoal(i).Output));
		GoalList->SetCellValue(i, 1, GetInputString(Optimization.GetAlgorithm().GetGoal(i).BeforeInputs));
		GoalList->SetCellValue(i, 2, GetInputString(Optimization.GetAlgorithm().GetGoal(i).AfterInputs,
			&Optimization.GetAlgorithm().GetGoal(i).BeforeInputs));
		GoalList->SetCellValue(i, 5, Converter.FormatNumber(Optimization.GetAlgorithm().GetGoal(i).Importance));

		// Here, we make sure we perform the appropriate conversion, depending on the units
		// of this particular output
		GoalList->SetCellValue(i, 3, Converter.FormatNumber(
			Converter.ConvertTo(Optimization.GetAlgorithm().GetGoal(i).DesiredValue,
			KINEMATIC_OUTPUTS::GetOutputUnitType(Optimization.GetAlgorithm().GetGoal(i).Output))));
		GoalList->SetCellValue(i, 4, Converter.FormatNumber(
			Converter.ConvertTo(Optimization.GetAlgorithm().GetGoal(i).ExpectedDeviation,
			KINEMATIC_OUTPUTS::GetOutputUnitType(Optimization.GetAlgorithm().GetGoal(i).Output))));

		// Make the cells read-only and center the text
		for (j = 0; j < GoalList->GetCols(); j++)
		{
			GoalList->SetReadOnly(i, j, true);
			GoalList->SetCellAlignment(wxALIGN_CENTER, i, j);
		}
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		ResetStatusBars
//
// Description:		Resets status bars to zero progress and sets range so that
//					each completed analysis represents one increment.
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
void GENETIC_ALGORITHM_PANEL::ResetStatusBars(void)
{
	// Set the range for the progress bars
	// The progress bars get updated every time an analysis completes, so
	// the range should be initialized so that it reaches 100% only when
	// the last analysis is complete.
	GenerationProgress->SetRange(Optimization.GetAlgorithm().GetPopulationSize()
		* Optimization.GetAlgorithm().GetNumberOfInputs());
	OverallProgress->SetRange(GenerationProgress->GetRange()
		* Optimization.GetAlgorithm().GetGenerationLimit());

	// Set the progress values to zero
	GenerationProgress->SetValue(0);
	OverallProgress->SetValue(0);

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		IncrementStatusBars
//
// Description:		Increments the level of completion of both status bars.
//					Status bars are normalized so that every time this is
//					called, the progress should be increased by one.
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
void GENETIC_ALGORITHM_PANEL::IncrementStatusBars(void)
{
	// If the generation progress bar is already full, reset it to zero
	if (GenerationProgress->GetValue() == GenerationProgress->GetRange())
		GenerationProgress->SetValue(0);

	// Add one to the status of both status bars
	GenerationProgress->SetValue(GenerationProgress->GetValue() + 1);
	OverallProgress->SetValue(OverallProgress->GetValue() + 1);

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		GetInputString
//
// Description:		Returns a human-readable string representing the passed
//					input conditions.  Only GA modifiable conditions are
//					displayed (i.e. euler rotations are not relevant here).
//
// Input Arguments:
//		Inputs			= const KINEMATICS::INPUT& referring to the relevant inputs
//		SecondInputs	= const KINEMATICS::INPUT* pointing to a second (optional)
//						  set of inputs
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
wxString GENETIC_ALGORITHM_PANEL::GetInputString(const KINEMATICS::INPUTS &Inputs,
												 const KINEMATICS::INPUTS *SecondInputs) const
{
	wxString InputString;

	// If the second set of inputs exists, make sure they don't match the first
	if (SecondInputs)
	{
		// If the first set of inputs matches the second, return nothing
		if (Inputs == *SecondInputs)
			return _T("None");
	}

	// Create the string
	InputString.Printf("P:%s, R:%s, H:%s, S:%s",
		Converter.FormatNumber(Converter.ConvertAngle(Inputs.Pitch)).c_str(),
		Converter.FormatNumber(Converter.ConvertAngle(Inputs.Roll)).c_str(),
		Converter.FormatNumber(Converter.ConvertDistance(Inputs.Heave)).c_str(),
		Converter.FormatNumber(Converter.ConvertDistance(Inputs.RackTravel)).c_str());

	// FIXME!!!:  This doesn't work if they are using steering wheel angle instead of rack travel!

	return InputString;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		GeneListSelectCellEvent
//
// Description:		Event handler for cells being selected.  The purpose of
//					this method is to set flags to ensure wxWidgets sees that
//					the entire row was selected, rather than just one cell.
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM_PANEL::GeneListSelectCellEvent(wxGridEvent &event)
{
	// Select the proper row
	GeneList->SelectRow(event.GetRow(), event.ControlDown());

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		GoalListSelectCellEvent
//
// Description:		Event handler for cells being selected.  The purpose of
//					this method is to set flags to ensure wxWidgets sees that
//					the entire row was selected, rather than just one cell.
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM_PANEL::GoalListSelectCellEvent(wxGridEvent &event)
{
	// Select the proper row
	GoalList->SelectRow(event.GetRow(), event.ControlDown());

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		TextBoxChangeEvent
//
// Description:		Event handler for text boxes changing.  Sets the flag
//					to indicate that the optimization has changed, and also
//					updates the optimization information, so the object can
//					be saved to file.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM_PANEL::TextBoxChangeEvent(wxCommandEvent &event)
{
	// Don't do this until we're done loading
	if (!OverallProgress)
		return;

	// Don't do this for combobox events
	if (event.GetId() == IdSelectCar)
		return;

	// Update the GA object to match the current inputs
	UpdateGAParameters(false);

	// Mark the optimization as modified
	Optimization.SetModified();

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM_PANEL
// Function:		UpdateGAParameters
//
// Description:		Updates the 
//
// Input Arguments:
//		ShowWarnings	= const bool& indicating whether or not to show warning
//						  dialog boxes
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating whether or not all of the parameters were valid
//
//==========================================================================
bool GENETIC_ALGORITHM_PANEL::UpdateGAParameters(const bool &ShowWarnings)
{
	// Check to see if the inputs are OK
	double TempDouble;
	long TempLong;
	if (!PopulationSize->GetValue().ToLong(&TempLong))
	{
		// Display a message box to warn the user
		if (ShowWarnings)
			wxMessageBox(_T("ERROR:  Population size must be a valid integer number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else if (TempLong <= 0)
	{
		// Display a message box to warn the user
		if (ShowWarnings)
			wxMessageBox(_T("ERROR:  Population size must be a positive number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else
		Optimization.GetAlgorithm().SetPopulationSize(TempLong);

	if (!GenerationLimit->GetValue().ToLong(&TempLong))
	{
		// Display a message box to warn the user
		if (ShowWarnings)
			wxMessageBox(_T("ERROR:  Generation limit must be a valid integer number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else if (TempLong <= 0)
	{
		// Display a message box to warn the user
		if (ShowWarnings)
			wxMessageBox(_T("ERROR:  Generation limit must be a positive number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else
		Optimization.GetAlgorithm().SetGenerationLimit(TempLong);

	if (!CrossoverPoint->GetValue().ToLong(&TempLong))
	{
		// Display a message box to warn the user
		if (ShowWarnings)
			wxMessageBox(_T("ERROR:  Crossover point must be a valid integer number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else if (TempLong < 0)
	{
		// Display a message box to warn the user
		if (ShowWarnings)
			wxMessageBox(_T("ERROR:  Crossover point must be non-negative!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else
		Optimization.GetAlgorithm().SetCrossoverPoint(TempLong);

	if (!ElitismFraction->GetValue().ToDouble(&TempDouble))
	{
		// Display a message box to warn the user
		if (ShowWarnings)
			wxMessageBox(_T("ERROR:  Elitism fraction must be a valid number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else if (TempDouble < 0.0)
	{
		// Display a message box to warn the user
		if (ShowWarnings)
			wxMessageBox(_T("ERROR:  Elitism fraction must be non-negative!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else
		Optimization.GetAlgorithm().SetElitismPercentage(TempDouble);

	if (!MutationProbability->GetValue().ToDouble(&TempDouble))
	{
		// Display a message box to warn the user
		if (ShowWarnings)
			wxMessageBox(_T("ERROR:  Mutation probability must be a valid number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else if (TempDouble < 0.0)
	{
		// Display a message box to warn the user
		if (ShowWarnings)
			wxMessageBox(_T("ERROR:  Mutation probability must be non-negative!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else
		Optimization.GetAlgorithm().SetMutationProbability(TempDouble);

	return true;
}