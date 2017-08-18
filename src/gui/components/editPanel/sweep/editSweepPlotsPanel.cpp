/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSweepPlotsPanel.cpp
// Date:  11/14/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditSweepPlotsPanel
//        class.

// Local headers
#include "editSweepPlotsPanel.h"
#include "editSweepNotebook.h"
#include "../../../sweep.h"
#include "VVASE/gui/components/mainFrame.h"
#include "../editPanel.h"
#include "VVASE/gui/utilities/unitConverter.h"

namespace VVASE
{

//==========================================================================
// Class:			EditSweepPlotsPanel
// Function:		EditSweepPlotsPanel
//
// Description:		Constructor for EditSweepPlotsPanel class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		parent		= EditSweepNotebook&, reference to this object's owner
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
EditSweepPlotsPanel::EditSweepPlotsPanel(EditSweepNotebook &parent,
	wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxPanel(&parent, id, pos, size), parent(parent)
{
	currentSweep = NULL;
	CreateControls();
}

//==========================================================================
// Class:			EditSweepPlotsPanel
// Function:		~EditSweepPlotsPanel
//
// Description:		Destructor for EditSweepPlotsPanel class.
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
EditSweepPlotsPanel::~EditSweepPlotsPanel()
{
}

//==========================================================================
// Class:			EditSweepPlotsPanel
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
BEGIN_EVENT_TABLE(EditSweepPlotsPanel, wxPanel)
	EVT_CHECKLISTBOX(PlotSelectionCheckList,	EditSweepPlotsPanel::PlotSelectionChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditSweepPlotsPanel
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
void EditSweepPlotsPanel::UpdateInformation()
{
	// Make sure the suspension object exists
	if (currentSweep)
		// Call the method that performs the update
		UpdateInformation(currentSweep);
}

//==========================================================================
// Class:			EditSweepPlotsPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		currentSweep	= Sweep* pointing to the associated sweep
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditSweepPlotsPanel::UpdateInformation(Sweep *currentSweep)
{
	this->currentSweep = currentSweep;

	if (!currentSweep)
		return;

	// Check the plots for the plots that are already active with the associated sweep
	unsigned int i;
	for (i = 0; i < Sweep::NumberOfPlots; i++)
		plotListCheckBox->Check(i, currentSweep->GetActivePlot(static_cast<Sweep::PlotID>(i)));
}

//==========================================================================
// Class:			EditSweepPlotsPanel
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
void EditSweepPlotsPanel::CreateControls()
{
	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 1, wxGROW | wxALL, 5);

	// Add a description of the panel to the sizer
	wxStaticText *descriptionText = new wxStaticText(this, wxID_STATIC, _T("Select plots to display:"));
	mainSizer->Add(descriptionText, 0, wxALL, 5);

	// Set up the list of plots to choose from - add the names of the available
	// plots to the list
	unsigned int i;
	wxArrayString choices;
	for (i = 0; i < Sweep::NumberOfPlots; i++)
		choices.Add(currentSweep->GetPlotName(static_cast<Sweep::PlotID>(i)));

	// Create the checklist box and add it to the sizer
	plotListCheckBox = new wxCheckListBox(this, PlotSelectionCheckList, wxDefaultPosition, wxDefaultSize, choices);
	mainSizer->Add(plotListCheckBox, 1, wxGROW | wxALL, 5);

	// Assign the top level sizer to the panel
	SetSizer(topSizer);
}

//==========================================================================
// Class:			EditSweepPlotsPanel
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
void EditSweepPlotsPanel::PlotSelectionChangeEvent(wxCommandEvent &event)
{
	// Update the sweep to match the checklist
	currentSweep->SetActivePlot(static_cast<Sweep::PlotID>(event.GetInt()),
		plotListCheckBox->IsChecked(event.GetInt()));

	// Update the display
	currentSweep->UpdateDisplay();
}

}// namespace VVASE
