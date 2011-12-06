/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationPlotsPanel.cpp
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class definition for the EditIterationPlotsPanel
//				 class.
// History:

// CarDesigner headers
#include "gui/iteration.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/iteration/editIterationPlotsPanel.h"
#include "gui/components/editPanel/iteration/editIterationNotebook.h"
#include "vUtilities/convert.h"

//==========================================================================
// Class:			EditIterationPlotsPanel
// Function:		EditIterationPlotsPanel
//
// Description:		Constructor for EditIterationPlotsPanel class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		_parent		= EditIterationNotebook&, reference to this object's owner
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
EditIterationPlotsPanel::EditIterationPlotsPanel(EditIterationNotebook &_parent,
													   wxWindowID id, const wxPoint& pos,
													   const wxSize& size) :
													   wxPanel(&_parent, id, pos, size),
													   parent(_parent)
{
	// Initialize the current object variable
	currentIteration = NULL;

	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EditIterationPlotsPanel
// Function:		~EditIterationPlotsPanel
//
// Description:		Destructor for EditIterationPlotsPanel class.
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
EditIterationPlotsPanel::~EditIterationPlotsPanel()
{
}

//==========================================================================
// Class:			EditIterationPlotsPanel
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
BEGIN_EVENT_TABLE(EditIterationPlotsPanel, wxPanel)
	EVT_CHECKLISTBOX(PlotSelectionCheckList,	EditIterationPlotsPanel::PlotSelectionChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditIterationPlotsPanel
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
void EditIterationPlotsPanel::UpdateInformation(void)
{
	// Make sure the suspension object exists
	if (currentIteration)
		// Call the method that performs the update
		UpdateInformation(currentIteration);
}

//==========================================================================
// Class:			EditIterationPlotsPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_currentIteration	= Iteration* pointing to the associated iteration
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditIterationPlotsPanel::UpdateInformation(Iteration *_currentIteration)
{
	// Update the class members
	currentIteration = _currentIteration;

	// Make sure the iteration exists
	if (!currentIteration)
		return;

	// Check the plots for the plots that are already active with the associated iteration
	unsigned int i;
	for (i = 0; i < Iteration::NumberOfPlots; i++)
		plotListCheckBox->Check(i, currentIteration->GetActivePlot((Iteration::PlotID)i));
}

//==========================================================================
// Class:			EditIterationPlotsPanel
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
void EditIterationPlotsPanel::CreateControls()
{
	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALL, 5);

	// Add a description of the panel to the sizer
	wxStaticText *descriptionText = new wxStaticText(this, wxID_STATIC, _T("Select plots to display:"));
	mainSizer->Add(descriptionText, 0, wxALL, 5);

	// Set up the list of plots to choose from - add the names of the available
	// plots to the list
	unsigned int i;
	wxArrayString choices;
	for (i = 0; i < Iteration::NumberOfPlots; i++)
		choices.Add(currentIteration->GetPlotName((Iteration::PlotID)i));

	// Create the checklist box and add it to the sizer
	plotListCheckBox = new wxCheckListBox(this, PlotSelectionCheckList, wxDefaultPosition, wxDefaultSize, choices);
	mainSizer->Add(plotListCheckBox, 1, wxGROW | wxALL, 5);

	// Assign the top level sizer to the panel
	SetSizer(topSizer);
}

//==========================================================================
// Class:			EditIterationPlotsPanel
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
void EditIterationPlotsPanel::PlotSelectionChangeEvent(wxCommandEvent &event)
{
	// Update the iteration to match the checklist
	currentIteration->SetActivePlot((Iteration::PlotID)event.GetInt(),
		plotListCheckBox->IsChecked(event.GetInt()));

	// Update the dispaly
	currentIteration->UpdateDisplay();
}