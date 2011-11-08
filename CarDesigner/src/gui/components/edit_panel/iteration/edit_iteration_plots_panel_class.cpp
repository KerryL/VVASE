/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_iteration_options_panel_class.cpp
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_ITERATION_PLOTS_PANEL
//				 class.
// History:

// CarDesigner headers
#include "gui/iteration_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/components/edit_panel/edit_panel_class.h"
#include "gui/components/edit_panel/iteration/edit_iteration_plots_panel_class.h"
#include "gui/components/edit_panel/iteration/edit_iteration_notebook_class.h"
#include "vUtilities/convert_class.h"

//==========================================================================
// Class:			EDIT_ITERATION_PLOTS_PANEL
// Function:		EDIT_ITERATION_PLOTS_PANEL
//
// Description:		Constructor for EDIT_ITERATION_PLOTS_PANEL class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		_Parent		= EDIT_ITERATION_NOTEBOOK&, reference to this object's owner
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
EDIT_ITERATION_PLOTS_PANEL::EDIT_ITERATION_PLOTS_PANEL(EDIT_ITERATION_NOTEBOOK &_Parent,
													   wxWindowID id, const wxPoint& pos,
													   const wxSize& size, const Debugger &_debugger) :
													   wxPanel(&_Parent, id, pos, size), Debugger(_debugger),
													   Parent(_Parent)
{
	// Initialize the current object variable
	CurrentIteration = NULL;

	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EDIT_ITERATION_PLOTS_PANEL
// Function:		~EDIT_ITERATION_PLOTS_PANEL
//
// Description:		Destructor for EDIT_ITERATION_PLOTS_PANEL class.
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
EDIT_ITERATION_PLOTS_PANEL::~EDIT_ITERATION_PLOTS_PANEL()
{
}

//==========================================================================
// Class:			EDIT_ITERATION_PLOTS_PANEL
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
BEGIN_EVENT_TABLE(EDIT_ITERATION_PLOTS_PANEL, wxPanel)
	EVT_CHECKLISTBOX(PlotSelectionCheckList,	EDIT_ITERATION_PLOTS_PANEL::PlotSelectionChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EDIT_ITERATION_PLOTS_PANEL
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
void EDIT_ITERATION_PLOTS_PANEL::UpdateInformation(void)
{
	// Make sure the suspension object exists
	if (CurrentIteration)
		// Call the method that performs the update
		UpdateInformation(CurrentIteration);

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_PLOTS_PANEL
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_CurrentIteration	= ITERATION* pointing to the associated iteration
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_ITERATION_PLOTS_PANEL::UpdateInformation(ITERATION *_CurrentIteration)
{
	// Update the class members
	CurrentIteration = _CurrentIteration;

	// Make sure the iteration exists
	if (!CurrentIteration)
		return;

	// Check the plots for the plots that are already active with the associated iteration
	unsigned int i;
	for (i = 0; i < ITERATION::NumberOfPlots; i++)
		PlotListCheckBox->Check(i, CurrentIteration->GetActivePlot((ITERATION::PLOT_ID)i));

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_PLOTS_PANEL
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
void EDIT_ITERATION_PLOTS_PANEL::CreateControls()
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALL, 5);

	// Add a description of the panel to the sizer
	wxStaticText *DescriptionText = new wxStaticText(this, wxID_STATIC, _T("Select plots to display:"));
	MainSizer->Add(DescriptionText, 0, wxALL, 5);

	// Set up the list of plots to choose from - add the names of the available
	// plots to the list
	unsigned int i;
	wxArrayString Choices;
	for (i = 0; i < ITERATION::NumberOfPlots; i++)
		Choices.Add(CurrentIteration->GetPlotName((ITERATION::PLOT_ID)i));

	// Create the checklist box and add it to the sizer
	PlotListCheckBox = new wxCheckListBox(this, PlotSelectionCheckList, wxDefaultPosition, wxDefaultSize, Choices);
	MainSizer->Add(PlotListCheckBox, 1, wxGROW | wxALL, 5);

	// Assign the top level sizer to the panel
	SetSizer(TopSizer);

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_PLOTS_PANEL
// Function:		PlotSelectionChangeEvent
//
// Description:		Creates the controls for this panel.
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
void EDIT_ITERATION_PLOTS_PANEL::PlotSelectionChangeEvent(wxCommandEvent &event)
{
	// Update the iteration to match the checklist
	CurrentIteration->SetActivePlot((ITERATION::PLOT_ID)event.GetInt(),
		PlotListCheckBox->IsChecked(event.GetInt()));

	// Update the dispaly
	CurrentIteration->UpdateDisplay();

	return;
}