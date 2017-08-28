/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  geneticAlgorithmPanel.cpp
// Date:  7/30/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definitions for a panel that allows users to specify
//        the parameters, genes and goals of a genetic algorithm.

// wxWidgets headers
#include <wx/grid.h>
#include <wx/gauge.h>

// Local headers
#include "geneticAlgorithmPanel.h"
#include "VVASE/gui/components/mainFrame.h"
#include "VVASE/gui/dialogs/gaGoalDialog.h"
#include "VVASE/gui/dialogs/gaGeneDialog.h"
#include "guiCar.h"
#include "VVASE/gui/geneticOptimization.h"
#include "gaObject.h"
#include "VVASE/core/analysis/kinematicOutputs.h"
#include "VVASE/core/car/subsystems/corner.h"
#include "VVASE/core/car/car.h"
#include "VVASE/gui/utilities/wxRelatedUtilities.h"
#include "VVASE/gui/utilities/unitConverter.h"

namespace VVASE
{

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		GeneticAlgorithmPanel
//
// Description:		Constructor for the GeneticAlgorithmPanel class.
//
// Input Arguments:
//		mainFrame		= MainFrame&, reference to the main application window
//		optimization	= GeneticOptimization&, reference to the object that we represent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GeneticAlgorithmPanel::GeneticAlgorithmPanel(MainFrame &mainFrame,
	GeneticOptimization &optimization) : wxScrolledWindow(&mainFrame),
	optimization(optimization), mainFrame(mainFrame)
{
	// This is used as in indication that the panel is completely built
	overallProgress = NULL;

	CreateControls();
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		GeneticAlgorithmPanel
//
// Description:		Destructor for the GeneticAlgorithmPanel class.
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
GeneticAlgorithmPanel::~GeneticAlgorithmPanel()
{
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
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
BEGIN_EVENT_TABLE(GeneticAlgorithmPanel, wxPanel)
	// Buttons
	EVT_BUTTON(IdAddGene,						GeneticAlgorithmPanel::AddGeneButtonClickedEvent)
	EVT_BUTTON(IdEditGene,						GeneticAlgorithmPanel::EditGeneButtonClickedEvent)
	EVT_BUTTON(IdRemoveGene,					GeneticAlgorithmPanel::RemoveGeneButtonClickedEvent)
	EVT_BUTTON(IdAddGoal,						GeneticAlgorithmPanel::AddGoalButtonClickedEvent)
	EVT_BUTTON(IdEditGoal,						GeneticAlgorithmPanel::EditGoalButtonClickedEvent)
	EVT_BUTTON(IdRemoveGoal,					GeneticAlgorithmPanel::RemoveGoalButtonClickedEvent)
	EVT_BUTTON(IdStartStopOptimization,			GeneticAlgorithmPanel::StartStopOptimizationClickedEvent)
	EVT_GRID_CMD_SELECT_CELL(IdGeneList,		GeneticAlgorithmPanel::GeneListSelectCellEvent)
	EVT_GRID_CMD_SELECT_CELL(IdGoalList,		GeneticAlgorithmPanel::GoalListSelectCellEvent)
	EVT_GRID_CMD_CELL_LEFT_DCLICK(IdGeneList,	GeneticAlgorithmPanel::GeneGridDoubleClickedEvent)
	EVT_GRID_CMD_CELL_LEFT_DCLICK(IdGoalList,	GeneticAlgorithmPanel::GoalGridDoubleClickedEvent)
	EVT_TEXT(wxID_ANY,							GeneticAlgorithmPanel::TextBoxChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			GeneticAlgorithmPanel
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
void GeneticAlgorithmPanel::CreateControls()
{
	// Enable scrolling
	SetScrollRate(10, 10);

	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 1, wxALIGN_LEFT | wxGROW | wxALL, 5);

	// Set the sizer flags
	int labelSizerFlags = wxALIGN_CENTER_VERTICAL;
	int inputSizerFlags = wxEXPAND | wxALIGN_CENTER_VERTICAL;
	int padding(2);

	// Create a sizer at the top to contain the GA parameters and the start/stop button
	wxFlexGridSizer *upperSizer = new wxFlexGridSizer(3, padding, padding);
	upperSizer->SetFlexibleDirection(wxHORIZONTAL);
	mainSizer->Add(upperSizer, 0, wxALL | wxGROW, 5);

	// Population size
	wxStaticText *populationLabel = new wxStaticText(this, wxID_ANY, _T("Population Size"));
	populationSize = new wxTextCtrl(this, wxID_ANY, _T("500"));
	upperSizer->Add(populationLabel, 0, labelSizerFlags);
	upperSizer->Add(populationSize, 0, inputSizerFlags);

	// Start/stop button
	startStopOptimization = new wxButton(this, IdStartStopOptimization, _T("Start GA"));
	upperSizer->Add(startStopOptimization, 0, wxALIGN_LEFT);

	// Generation limit
	wxStaticText *generationLimitLabel = new wxStaticText(this, wxID_ANY, _T("Generation Limit"));
	generationLimit = new wxTextCtrl(this, wxID_ANY, _T("20"));
	upperSizer->Add(generationLimitLabel, 0, labelSizerFlags);
	upperSizer->Add(generationLimit, 0, inputSizerFlags);
	upperSizer->AddSpacer(-1);

	// Elitism percentage
	wxStaticText *elitismLabel = new wxStaticText(this, wxID_ANY, _T("Elitism Fraction"));
	elitismFraction = new wxTextCtrl(this, wxID_ANY, _T("0.05"));
	upperSizer->Add(elitismLabel, 0, labelSizerFlags);
	upperSizer->Add(elitismFraction, 0, inputSizerFlags);
	upperSizer->AddSpacer(-1);

	// Mutation probability
	wxStaticText *mutationLabel = new wxStaticText(this, wxID_ANY, _T("Mutation Probability"));
	mutationProbability = new wxTextCtrl(this, wxID_ANY, _T("0.01"));
	upperSizer->Add(mutationLabel, 0, labelSizerFlags);
	upperSizer->Add(mutationProbability, 0, inputSizerFlags);
	upperSizer->AddSpacer(-1);

	// Crossover point
	wxStaticText *crossoverLabel = new wxStaticText(this, wxID_ANY, _T("Crossover Point"));
	crossoverPoint = new wxTextCtrl(this, wxID_ANY, _T("0"));
	upperSizer->Add(crossoverLabel, 0, labelSizerFlags);
	upperSizer->Add(crossoverPoint, 0, inputSizerFlags);
	upperSizer->AddSpacer(-1);

	// Selection drop down box
	wxStaticText *selectionLabel = new wxStaticText(this, wxID_ANY, _T("Car to Optimize"));
	selectedCar = new wxComboBox(this, IdSelectCar, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, NULL, wxCB_READONLY);
	upperSizer->Add(selectionLabel, 0, labelSizerFlags);
	upperSizer->Add(selectedCar, 0, inputSizerFlags);

	// Gene list
	wxFlexGridSizer *geneGoalSizer = new wxFlexGridSizer(2, 3, 3);
	mainSizer->Add(geneGoalSizer, 1, wxGROW | wxALL, 5);
	geneList = new wxGrid(this, IdGeneList);
	geneList->CreateGrid(1, 6, wxGrid::wxGridSelectRows);

	// Begin a batch edit of the grid
	geneList->BeginBatch();

	// Make all cells read-only and center the text
	int i, j;
	for (i = 0; i < geneList->GetNumberRows(); i++)
	{
		for (j = 0; j < geneList->GetNumberCols(); j++)
		{
			geneList->SetReadOnly(i, j, true);
			geneList->SetCellAlignment(i, j, wxALIGN_CENTER, wxALIGN_TOP);
		}
	}

	// Hide the label column and set the size for the label row
	geneList->SetRowLabelSize(0);
	geneList->SetColLabelSize(geneList->GetRowSize(0));

	// Add the grid to the sizer
	geneGoalSizer->Add(geneList, 1, wxALIGN_LEFT | wxGROW | wxALIGN_TOP);

	// Set the column headings
	geneList->SetColLabelValue(0, _T("Hardpoint"));
	geneList->SetColLabelValue(1, _T("Alternate With"));
	geneList->SetColLabelValue(2, _T("Axis Direction"));
	geneList->SetColLabelValue(3, _T("Minimum"));
	geneList->SetColLabelValue(4, _T("Maximum"));
	geneList->SetColLabelValue(5, _T("Number Of Values"));

	// Assign rights for the user move or re-size the rows or move the columns
	geneList->EnableDragColMove(false);
	geneList->EnableDragColSize(true);
	geneList->EnableDragGridSize(false);
	geneList->EnableDragRowSize(false);

	// Auto-size the grid columns
	// FIXME:  This doesn't work the way we would like - it just sizes to column headings, not column contents
	// Workaround is below
	geneList->AutoSizeColumns(false);

	// Assign column widths
	// FIXME:  We would rather use auto-size (this is workaround for above)
	geneList->SetColSize(0, 140);
	geneList->SetColSize(1, 140);
	// Default size (after auto-size) is OK for the remaining columns

	// End the batch mode edit and re-paint the control
	geneList->EndBatch();

	// Add/Edit/Remove Gene buttons
	wxBoxSizer *geneButtonSizer = new wxBoxSizer(wxVERTICAL);
	geneGoalSizer->Add(geneButtonSizer, 0, wxALIGN_LEFT);
	addGene = new wxButton(this, IdAddGene, _T("Add Gene"));
	editGene = new wxButton(this, IdEditGene, _T("Edit Gene"));
	removeGene = new wxButton(this, IdRemoveGene, _T("Remove Gene"));
	geneButtonSizer->Add(addGene, 0, wxEXPAND);
	geneButtonSizer->Add(editGene, 0, wxEXPAND);
	geneButtonSizer->Add(removeGene, 0, wxEXPAND);

	// Goal list
	goalList = new wxGrid(this, IdGoalList);
	goalList->CreateGrid(1, 6, wxGrid::wxGridSelectRows);

	// Begin a batch edit of the grid
	goalList->BeginBatch();

	// Make all cells read-only
	for (i = 0; i < goalList->GetNumberRows(); i++)
	{
		for (j = 0; j < goalList->GetNumberCols(); j++)
		{
			goalList->SetReadOnly(i, j, true);
			goalList->SetCellAlignment(i, j, wxALIGN_CENTER, wxALIGN_TOP);
		}
	}

	// Hide the label column and set the size for the label row
	goalList->SetRowLabelSize(0);
	goalList->SetColLabelSize(goalList->GetRowSize(0));

	// Add the grid to the sizer
	geneGoalSizer->Add(goalList, 0, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALIGN_TOP);

	// Set the column headings
	goalList->SetColLabelValue(0, _T("Output"));
	goalList->SetColLabelValue(1, _T("State 1 Inputs"));
	goalList->SetColLabelValue(2, _T("State 2 Inputs"));
	goalList->SetColLabelValue(3, _T("Desired Value"));
	goalList->SetColLabelValue(4, _T("Expected Deviation"));
	goalList->SetColLabelValue(5, _T("Importance"));

	// Assign rights for the user move or re-size the rows or move the columns
	goalList->EnableDragColMove(false);
	goalList->EnableDragColSize(true);
	goalList->EnableDragGridSize(false);
	goalList->EnableDragRowSize(false);

	// Auto-size the grid columns
	// FIXME:  This doesn't work the way we would like - it just sizes to column headings, not column contents
	// Workaround is below
	goalList->AutoSizeColumns(false);

	// Assign column widths
	// FIXME:  We would rather do this automatically (this is workaround for above)
	goalList->SetColSize(0, 190);
	goalList->SetColSize(1, 220);
	goalList->SetColSize(2, 220);
	// Default size (after auto-size) is OK for the remaining columns

	// End the batch mode edit and re-paint the control
	goalList->EndBatch();

	// Add/Edit/Remove Goal buttons
	wxBoxSizer *goalButtonSizer = new wxBoxSizer(wxVERTICAL);
	geneGoalSizer->Add(goalButtonSizer, 0, wxALIGN_LEFT);
	addGoal = new wxButton(this, IdAddGoal, _T("Add Goal"));
	editGoal = new wxButton(this, IdEditGoal, _T("Edit Goal"));
	removeGoal = new wxButton(this, IdRemoveGoal, _T("Remove Goal"));
	goalButtonSizer->Add(addGoal, 0, wxEXPAND);
	goalButtonSizer->Add(editGoal, 0, wxEXPAND);
	goalButtonSizer->Add(removeGoal, 0, wxEXPAND);

	geneGoalSizer->AddGrowableRow(0, 1);
	geneGoalSizer->AddGrowableRow(1, 1);

	// Progress bars
	wxFlexGridSizer *progressSizer = new wxFlexGridSizer(2, 2, 5);
	progressSizer->SetFlexibleDirection(wxHORIZONTAL);
	mainSizer->Add(progressSizer, 0, wxALL | wxEXPAND, 5);
	progressSizer->AddGrowableCol(1);

	wxStaticText *generationProgressLabel = new wxStaticText(this, wxID_ANY, _T("Current Generation"));
	generationProgress = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition,
		wxSize(-1, generationProgressLabel->GetSize().GetHeight()), wxGA_HORIZONTAL | wxGA_SMOOTH);
	progressSizer->Add(generationProgressLabel, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	progressSizer->Add(generationProgress, 0, wxEXPAND);

	wxStaticText *overallProgressLabel = new wxStaticText(this, wxID_ANY, _T("Overall"));
	overallProgress = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition,
		wxSize(-1, overallProgressLabel->GetSize().GetHeight()), wxGA_HORIZONTAL | wxGA_SMOOTH);
	progressSizer->Add(overallProgressLabel, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	progressSizer->Add(overallProgress, 0, wxEXPAND);

	// Assign the top level sizer to the dialog
	SetSizer(topSizer);
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		AddGeneButtonClickedEvent
//
// Description:		Event handler for the Add Gene button.  Displays the
//					GAGeneDialog to allow the user to create the gene
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
void GeneticAlgorithmPanel::AddGeneButtonClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	const Suspension* suspension(nullptr);
	const GuiCar* carSelection(GetSelectedCar());
	if (carSelection)
		suspension = carSelection->GetOriginalCar().GetSubsystem<Suspension>();

	// Create the dialog box with default gene properties
	GAGeneDialog geneDialog(static_cast<wxWindow*>(&mainFrame), suspension, (Corner::Hardpoints)0,
		(Corner::Hardpoints)0, (Math::Axis)0, (Corner::Location)0, 0.0, 1.0, 5, wxID_ANY, wxDefaultPosition);

	if (geneDialog.ShowModal() == wxOK)
	{
		optimization.GetAlgorithm().AddGene(geneDialog.GetHardpoint(), geneDialog.GetTiedTo(), geneDialog.GetCornerLocation(),
			geneDialog.GetAxisDirection(), geneDialog.GetMinimum(), geneDialog.GetMaximum(), geneDialog.GetNumberOfValues());

		UpdateGeneList();
		optimization.SetModified();
	}
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		EditGeneButtonClickedEvent
//
// Description:		Event handler for the Edit Gene button.  Displays the
//					GAGeneDialog to allow the user to edit the gene
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
void GeneticAlgorithmPanel::EditGeneButtonClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure only one row is selected before displaying the dialog
	if (geneList->IsSelection() && geneList->GetSelectedRows().GetCount() == 1)
		EditSelectedGene();
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
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
void GeneticAlgorithmPanel::RemoveGeneButtonClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure a row is selected
	if (!geneList->IsSelection())
		return;

	// Make sure a gene exists (there is an empty row when the grid is first created -
	// clicking on it and trying to remove it would crash us)
	if (optimization.GetAlgorithm().GetGeneCount() == 0)
		return;

	// Remove the selected gene(s)
	unsigned int i;
	for (i = 0; i < geneList->GetSelectedRows().GetCount(); i++)
		optimization.GetAlgorithm().RemoveGene(geneList->GetSelectedRows()[i]);

	// Update the list
	UpdateGeneList();

	// Mark the optimization as modified
	optimization.SetModified();
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		AddGoalButtonClickedEvent
//
// Description:		Event handler for the Add Goal button.  Displays the
//					GAGoalDialog to allow the user to create the goal
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
void GeneticAlgorithmPanel::AddGoalButtonClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Set up the default kinematic inputs
	Kinematics::Inputs defaultInputs = mainFrame.GetInputs();
	defaultInputs.heave = 0.0;
	defaultInputs.pitch = 0.0;
	defaultInputs.roll = 0.0;
	defaultInputs.rackTravel = 0.0;
	defaultInputs.tireDeflections.leftFront = 0.0;
	defaultInputs.tireDeflections.rightFront = 0.0;
	defaultInputs.tireDeflections.leftRear = 0.0;
	defaultInputs.tireDeflections.rightRear = 0.0;

	// Create the dialog box with default goal properties
	GAGoalDialog goalDialog(static_cast<wxWindow*>(&mainFrame), (KinematicOutputs::OutputsComplete)0, 0.0,
		1.0, 1.0, defaultInputs, defaultInputs, wxID_ANY, wxDefaultPosition);

	// Display the dialog
	if (goalDialog.ShowModal() == wxOK)
	{
		// Create the new goal and add it to the list
		optimization.GetAlgorithm().AddGoal(goalDialog.GetOutput(), goalDialog.GetDesiredValue(),
			goalDialog.GetExpectedDeviation(), goalDialog.GetImportance(), goalDialog.GetBeforeInputs(),
			goalDialog.GetAfterInputs());

		// Update the display for the goal
		UpdateGoalList();

		// Mark the optimization as modified
		optimization.SetModified();
	}
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		EditGoalButtonClickedEvent
//
// Description:		Event handler for the Edit Goal button.  Displays the
//					GAGoalDialog to allow the user to edit the goal
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
void GeneticAlgorithmPanel::EditGoalButtonClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure exactly ONE row is selected
	if (goalList->IsSelection() && goalList->GetSelectedRows().GetCount() == 1)
		EditSelectedGoal();
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
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
void GeneticAlgorithmPanel::RemoveGoalButtonClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure a row is selected
	if (!goalList->IsSelection())
		return;

	// Make sure a goal exists (there is an empty row when the grid is first created -
	// clicking on it and trying to remove it would crash us)
	if (optimization.GetAlgorithm().GetGoalCount() == 0)
		return;

	// Remove the selected goal(s)
	unsigned int i;
	for (i = 0; i < goalList->GetSelectedRows().GetCount(); i++)
		optimization.GetAlgorithm().RemoveGoal(goalList->GetSelectedRows()[i]);

	// Update the list
	UpdateGoalList();

	// Mark the optimization as modified
	optimization.SetModified();
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		EditSelectedGene
//
// Description:		Displays a dialog to allow editing the selected gene.
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
void GeneticAlgorithmPanel::EditSelectedGene()
{
	// When a new optimization is created, there is an empty row in the grid.
	// It does not correspond to any real object and so it cannot be edited.
	// If we don't have any real object to edit, just return
	if (optimization.GetAlgorithm().GetGeneCount()  == 0)
		return;

	GAObject::Gene geneToEdit = optimization.GetAlgorithm().GetGene(geneList->GetSelectedRows()[0]);

	const Suspension* suspension(nullptr);
	const GuiCar* carSelection(GetSelectedCar());
	if (carSelection)
		suspension = carSelection->GetOriginalCar().GetSubsystem<Suspension>();

	// Create the dialog box with properties corresponding to the selected gene
	GAGeneDialog geneDialog(static_cast<wxWindow*>(&mainFrame), suspension, geneToEdit.hardpoint,
		geneToEdit.tiedTo, geneToEdit.direction, geneToEdit.location, geneToEdit.minimum, geneToEdit.maximum,
		geneToEdit.numberOfValues, wxID_ANY, wxDefaultPosition);

	if (geneDialog.ShowModal() == wxOK)
	{
		optimization.GetAlgorithm().UpdateGene(geneList->GetSelectedRows()[0], geneDialog.GetHardpoint(),
			geneDialog.GetTiedTo(), geneDialog.GetCornerLocation(), geneDialog.GetAxisDirection(),
			geneDialog.GetMinimum(), geneDialog.GetMaximum(), geneDialog.GetNumberOfValues());

		UpdateGeneList();
		optimization.SetModified();
	}
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		EditSelectedGoal
//
// Description:		Displays a dialog to allow editing the selected goal.
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
void GeneticAlgorithmPanel::EditSelectedGoal()
{
	// When a new optimization is created, there is an empty row in the grid.
	// It does not correspond to any real object and so it cannot be edited.
	// If we don't have any real object to edit, just return
	if (optimization.GetAlgorithm().GetGoalCount()  == 0)
		return;

	GAObject::Goal goalToEdit = optimization.GetAlgorithm().GetGoal(goalList->GetSelectedRows()[0]);

	GAGoalDialog goalDialog(static_cast<wxWindow*>(&mainFrame), goalToEdit.output, goalToEdit.desiredValue,
		goalToEdit.expectedDeviation, goalToEdit.importance, goalToEdit.beforeInputs, goalToEdit.afterInputs,
		wxID_ANY, wxDefaultPosition);

	if (goalDialog.ShowModal() == wxOK)
	{
		// Create the new goal and add it to the list
		optimization.GetAlgorithm().UpdateGoal(goalList->GetSelectedRows()[0], goalDialog.GetOutput(),
			goalDialog.GetDesiredValue(), goalDialog.GetExpectedDeviation(), goalDialog.GetImportance(),
			goalDialog.GetBeforeInputs(), goalDialog.GetAfterInputs());

		UpdateGoalList();
		optimization.SetModified();
	}
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
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
void GeneticAlgorithmPanel::StartStopOptimizationClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Determine if we should be starting or stopping the optimization
	// FIXME:  This can't be called while the optimization is running or the main
	// thread hangs (and thus the whole app hangs because events cannot be processed)
	if (optimization.GetAlgorithm().OptimizationIsRunning())
	{
		optimization.HaltOptimization();

		// Change the caption on the button
		// FIXME:  Capture event when GA ends to do this!!!
		//StartStopOptimization->SetLabel(_T("Start GA"));
	}
	else
	{
		if (!UpdateGAParameters())
			return;

		if (selectedCar->GetValue().IsEmpty())
		{
			wxMessageBox(_T("ERROR:  No car selected to optimize!"), _T("No Selected Car"),
				wxOK | wxICON_ERROR, this);

			return;
		}

		// Make sure we have some genes and goals defined
		if (optimization.GetAlgorithm().GetGeneCount() == 0 ||
			optimization.GetAlgorithm().GetGoalCount() == 0)
		{
			wxMessageBox(_T("ERROR:  Optimization requires at least one gene and one goal!"),
				_T("Goal and Gene Required"), wxOK | wxICON_ERROR, this);

			return;
		}

		// Assign our car to optimize
		// This is necessary for handling GUI interaction and displays
		const GuiCar* carSelection(GetSelectedCar());
		if (!carSelection)
		{
			wxMessageBox(_T("Error identifying associated car!  Please re-select."));
			return;
		}

		optimization.SetCarToOptimize(*carSelection);

		// Set the parameters for the analysis
		optimization.GetAlgorithm().SetUp(optimization.GetCarToOptimize().GetOriginalCar());

		// Reset the status bars (must occur after algorithm is SetUp() so we know
		// how many analyses to expect
		ResetStatusBars();

		optimization.BeginOptimization();

		// Change the caption on the button
		// FIXME:  Capture event when GA starts to do this!!!
		//StartStopOptimization->SetLabel(_T("Stop Optimization"));
	}
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
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
void GeneticAlgorithmPanel::UpdateSelectableCars()
{
	// NOTE:  If cars are opened or closed, this should follow the associated car, even though
	// we may need to change the index to do so
	// Check sweep for this behavior, too!

	wxString currentName = selectedCar->GetValue();

	bool foundOriginalCar(false);
	if (currentName.IsEmpty())
		foundOriginalCar = true;

	// Clear out the list and re-populate it with the currently open cars
	selectedCar->Clear();
	int i;
	for (i = 0; i < mainFrame.GetObjectCount(); i++)
	{
		// If the current object is a car, add it to the list
		if (mainFrame.GetObjectByIndex(i)->GetType() == GuiObject::ItemType::Car)
		{
			selectedCar->Append(mainFrame.GetObjectByIndex(i)->GetCleanName());
			if (currentName.Cmp(mainFrame.GetObjectByIndex(i)->GetCleanName()) == 0)
				foundOriginalCar = true;
		}
	}

	// Re-select the originally selected car
	if (foundOriginalCar)
		selectedCar->SetValue(currentName);
	else
	{
		// Handle the error if the original car is now gone
		// FIXME!!!
	}
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
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
void GeneticAlgorithmPanel::UpdateInformation()
{
	wxString temp;

	// Update parameters
	temp.Printf("%i", optimization.GetAlgorithm().GetPopulationSize());
	populationSize->ChangeValue(temp);
	temp.Printf("%i", optimization.GetAlgorithm().GetGenerationLimit());
	generationLimit->ChangeValue(temp);
	elitismFraction->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		optimization.GetAlgorithm().GetElitismPercentage()));
	mutationProbability->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		optimization.GetAlgorithm().GetMutationProbability()));

	temp.Printf("%i", optimization.GetAlgorithm().GetCrossoverPoint());
	crossoverPoint->ChangeValue(temp);

	// Update the rest of the panel
	UpdateGeneList();
	UpdateGoalList();
	UpdateSelectableCars();
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		UpdateGeneList
//
// Description:		Updates the gene list to match the genes present in the
//					associated GAObject.
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
void GeneticAlgorithmPanel::UpdateGeneList()
{
	// Make sure we have the correct number of rows
	if (geneList->GetNumberRows() > optimization.GetAlgorithm().GetGeneCount()
		&& optimization.GetAlgorithm().GetGeneCount() > 1)
		geneList->DeleteRows(0, geneList->GetNumberRows() - optimization.GetAlgorithm().GetGeneCount(), false);
	else if (geneList->GetNumberRows() < optimization.GetAlgorithm().GetGeneCount())
		geneList->InsertRows(0, optimization.GetAlgorithm().GetGeneCount() - geneList->GetNumberRows(), false);

	// Go through row-by-row and populate the cells with the correct information
	wxString temp;
	int i, j;
	for (i = 0; i < optimization.GetAlgorithm().GetGeneCount(); i++)
	{
		geneList->SetCellValue(i, 0, Corner::GetHardpointName(optimization.GetAlgorithm().GetGene(i).hardpoint));
		if (optimization.GetAlgorithm().GetGene(i).hardpoint == optimization.GetAlgorithm().GetGene(i).tiedTo)
			geneList->SetCellValue(i, 1, _T("None"));
		else
			geneList->SetCellValue(i, 1, Corner::GetHardpointName(optimization.GetAlgorithm().GetGene(i).tiedTo));
		geneList->SetCellValue(i, 2, Math::GetAxisName(optimization.GetAlgorithm().GetGene(i).direction));
		geneList->SetCellValue(i, 3, UnitConverter::GetInstance().FormatNumber(
			UnitConverter::GetInstance().ConvertDistanceOutput(optimization.GetAlgorithm().GetGene(i).minimum)));
		geneList->SetCellValue(i, 4, UnitConverter::GetInstance().FormatNumber(
			UnitConverter::GetInstance().ConvertDistanceOutput(optimization.GetAlgorithm().GetGene(i).maximum)));
		temp.Printf("%i", optimization.GetAlgorithm().GetGene(i).numberOfValues);
		geneList->SetCellValue(i, 5, temp);

		// Make the cells read-only and center the text
		for (j = 0; j < geneList->GetNumberCols(); j++)
		{
			geneList->SetReadOnly(i, j, true);
			geneList->SetCellAlignment(i, j, wxALIGN_CENTER, wxALIGN_TOP);
		}
	}
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		UpdateGoalList
//
// Description:		Updates the goal list to match the goals present in the
//					associated GAObject.
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
void GeneticAlgorithmPanel::UpdateGoalList()
{
	// Make sure we have the correct number of rows
	if (goalList->GetNumberRows() > optimization.GetAlgorithm().GetGoalCount()
		&& optimization.GetAlgorithm().GetGoalCount() > 1)
		goalList->DeleteRows(0, goalList->GetNumberRows() - optimization.GetAlgorithm().GetGoalCount(), false);
	else if (goalList->GetNumberRows() < optimization.GetAlgorithm().GetGoalCount())
		goalList->InsertRows(0, optimization.GetAlgorithm().GetGoalCount() - goalList->GetNumberRows(), false);

	// Go through row-by-row and populate the cells with the correct information
	int i, j;
	for (i = 0; i < optimization.GetAlgorithm().GetGoalCount(); i++)
	{
		goalList->SetCellValue(i, 0, KinematicOutputs::GetOutputName(optimization.GetAlgorithm().GetGoal(i).output));
		goalList->SetCellValue(i, 1, GetInputString(optimization.GetAlgorithm().GetGoal(i).beforeInputs));
		goalList->SetCellValue(i, 2, GetInputString(optimization.GetAlgorithm().GetGoal(i).afterInputs,
			&optimization.GetAlgorithm().GetGoal(i).beforeInputs));
		goalList->SetCellValue(i, 5, UnitConverter::GetInstance().FormatNumber(
			optimization.GetAlgorithm().GetGoal(i).importance));

		// Here, we make sure we perform the appropriate conversion, depending on the units
		// of this particular output
		goalList->SetCellValue(i, 3, UnitConverter::GetInstance().FormatNumber(
			UnitConverter::GetInstance().ConvertOutput(optimization.GetAlgorithm().GetGoal(i).desiredValue,
			KinematicOutputs::GetOutputUnitType(optimization.GetAlgorithm().GetGoal(i).output))));
		goalList->SetCellValue(i, 4, UnitConverter::GetInstance().FormatNumber(
			UnitConverter::GetInstance().ConvertOutput(optimization.GetAlgorithm().GetGoal(i).expectedDeviation,
			KinematicOutputs::GetOutputUnitType(optimization.GetAlgorithm().GetGoal(i).output))));

		// Make the cells read-only and center the text
		for (j = 0; j < goalList->GetNumberCols(); j++)
		{
			goalList->SetReadOnly(i, j, true);
			goalList->SetCellAlignment(i, j, wxALIGN_CENTER, wxALIGN_TOP);
		}
	}
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
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
void GeneticAlgorithmPanel::ResetStatusBars()
{
	// Set the range for the progress bars
	// The progress bars get updated every time an analysis completes, so
	// the range should be initialized so that it reaches 100% only when
	// the last analysis is complete.
	generationProgress->SetRange(optimization.GetAlgorithm().GetPopulationSize()
		* optimization.GetAlgorithm().GetNumberOfInputs());
	overallProgress->SetRange(generationProgress->GetRange()
		* optimization.GetAlgorithm().GetGenerationLimit());

	// Set the progress values to zero
	generationProgress->SetValue(0);
	overallProgress->SetValue(0);
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
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
void GeneticAlgorithmPanel::IncrementStatusBars()
{
	// If the generation progress bar is already full, reset it to zero
	if (generationProgress->GetValue() == generationProgress->GetRange())
		generationProgress->SetValue(0);

	// Add one to the status of both status bars
	generationProgress->SetValue(generationProgress->GetValue() + 1);
	overallProgress->SetValue(overallProgress->GetValue() + 1);
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		GetInputString
//
// Description:		Returns a human-readable string representing the passed
//					input conditions.  Only GA modifiable conditions are
//					displayed (i.e. Euler rotations are not relevant here).
//
// Input Arguments:
//		inputs			= const Kinematics::Inputs& referring to the relevant inputs
//		secondInputs	= const Kinematics::Inputs* pointing to a second (optional)
//						  set of inputs
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
wxString GeneticAlgorithmPanel::GetInputString(const Kinematics::Inputs &inputs,
												 const Kinematics::Inputs *secondInputs) const
{
	wxString inputString;

	// If the second set of inputs exists, make sure they don't match the first
	if (secondInputs)
	{
		// If the first set of inputs matches the second, return nothing
		if (inputs == *secondInputs)
			return _T("None");
	}

	inputString.Printf("P:%s, R:%s, H:%s, S:%s",
		UnitConverter::GetInstance().FormatNumber(
			UnitConverter::GetInstance().ConvertAngleOutput(inputs.pitch)).c_str(),
		UnitConverter::GetInstance().FormatNumber(
			UnitConverter::GetInstance().ConvertAngleOutput(inputs.roll)).c_str(),
		UnitConverter::GetInstance().FormatNumber(
			UnitConverter::GetInstance().ConvertDistanceOutput(inputs.heave)).c_str(),
		UnitConverter::GetInstance().FormatNumber(
			UnitConverter::GetInstance().ConvertDistanceOutput(inputs.rackTravel)).c_str());

	// FIXME!!!:  This doesn't work if they are using steering wheel angle instead of rack travel!

	return inputString;
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
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
void GeneticAlgorithmPanel::GeneListSelectCellEvent(wxGridEvent &event)
{
	// Select the proper row
	geneList->SelectRow(event.GetRow(), event.ControlDown());
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
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
void GeneticAlgorithmPanel::GoalListSelectCellEvent(wxGridEvent &event)
{
	// Select the proper row
	goalList->SelectRow(event.GetRow(), event.ControlDown());
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		GeneGridDoubleClickedEvent
//
// Description:		Event handler for double clicking on a gene list row.
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
void GeneticAlgorithmPanel::GeneGridDoubleClickedEvent(wxGridEvent& WXUNUSED(event))
{
	if (geneList->IsSelection() && geneList->GetSelectedRows().GetCount() == 1)
		EditSelectedGene();
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		GoalGridDoubleClickedEvent
//
// Description:		Event handler for double clicking on a goal list row.
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
void GeneticAlgorithmPanel::GoalGridDoubleClickedEvent(wxGridEvent& WXUNUSED(event))
{
	if (goalList->IsSelection() && goalList->GetSelectedRows().GetCount() == 1)
		EditSelectedGoal();
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
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
void GeneticAlgorithmPanel::TextBoxChangeEvent(wxCommandEvent &event)
{
	if (!overallProgress)
		return;

	if (event.GetId() == IdSelectCar)
		return;

	UpdateGAParameters(false);
	optimization.SetModified();
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		UpdateGAParameters
//
// Description:		Updates the algorithm parameters.
//
// Input Arguments:
//		showWarnings	= const bool& indicating whether or not to show warning
//						  dialog boxes
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating whether or not all of the parameters were valid
//
//==========================================================================
bool GeneticAlgorithmPanel::UpdateGAParameters(const bool &showWarnings)
{
	// Check to see if the inputs are OK
	double tempDouble;
	long tempLong;
	if (!populationSize->GetValue().ToLong(&tempLong))
	{
		// Display a message box to warn the user
		if (showWarnings)
			wxMessageBox(_T("ERROR:  Population size must be a valid integer number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else if (tempLong <= 0)
	{
		// Display a message box to warn the user
		if (showWarnings)
			wxMessageBox(_T("ERROR:  Population size must be a positive number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else
		optimization.GetAlgorithm().SetPopulationSize(tempLong);

	if (!generationLimit->GetValue().ToLong(&tempLong))
	{
		// Display a message box to warn the user
		if (showWarnings)
			wxMessageBox(_T("ERROR:  Generation limit must be a valid integer number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else if (tempLong <= 0)
	{
		// Display a message box to warn the user
		if (showWarnings)
			wxMessageBox(_T("ERROR:  Generation limit must be a positive number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else
		optimization.GetAlgorithm().SetGenerationLimit(tempLong);

	if (!crossoverPoint->GetValue().ToLong(&tempLong))
	{
		// Display a message box to warn the user
		if (showWarnings)
			wxMessageBox(_T("ERROR:  Crossover point must be a valid integer number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else if (tempLong < 0)
	{
		// Display a message box to warn the user
		if (showWarnings)
			wxMessageBox(_T("ERROR:  Crossover point must be non-negative!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else
		optimization.GetAlgorithm().SetCrossoverPoint(tempLong);

	if (!elitismFraction->GetValue().ToDouble(&tempDouble))
	{
		// Display a message box to warn the user
		if (showWarnings)
			wxMessageBox(_T("ERROR:  Elitism fraction must be a valid number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else if (tempDouble < 0.0)
	{
		// Display a message box to warn the user
		if (showWarnings)
			wxMessageBox(_T("ERROR:  Elitism fraction must be non-negative!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else
		optimization.GetAlgorithm().SetElitismPercentage(tempDouble);

	if (!mutationProbability->GetValue().ToDouble(&tempDouble))
	{
		// Display a message box to warn the user
		if (showWarnings)
			wxMessageBox(_T("ERROR:  Mutation probability must be a valid number!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else if (tempDouble < 0.0)
	{
		// Display a message box to warn the user
		if (showWarnings)
			wxMessageBox(_T("ERROR:  Mutation probability must be non-negative!"), _T("Invalid Parameter"),
				wxOK | wxICON_ERROR, this);

		return false;
	}
	else
		optimization.GetAlgorithm().SetMutationProbability(tempDouble);

	return true;
}

//==========================================================================
// Class:			GeneticAlgorithmPanel
// Function:		GetSelectedCar
//
// Description:		Returns the car object specified in the selection combobox.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		cosnt GuiCar*
//
//==========================================================================
const GuiCar* GeneticAlgorithmPanel::GetSelectedCar()
{
	int i, carIndex(-1);
	for (i = 0; i < mainFrame.GetObjectCount(); i++)
	{
		// If the object with index i is a car, check to see if it is the selected car
		if (mainFrame.GetObjectByIndex(i)->GetType() == GuiObject::ItemType::Car)
		{
			carIndex++;

			if (carIndex == wxUtilities::SafelyGetComboBoxSelection(selectedCar))
				break;
		}
	}

	// Verify that the selection is valid
	if (i >= mainFrame.GetObjectCount() ||
		mainFrame.GetObjectByIndex(i)->GetType() != GuiObject::ItemType::Car)
		return NULL;

	return static_cast<GuiCar*>(mainFrame.GetObjectByIndex(i));
}

}// namespace VVASE
