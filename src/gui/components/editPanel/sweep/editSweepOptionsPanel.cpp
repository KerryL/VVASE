/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSweepOptionsPanel.cpp
// Date:  11/14/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditSweepOptionsPanel
//        class.

// Local headers
#include "editSweepOptionsPanel.h"
#include "editSweepNotebook.h"
#include "../../../sweep.h"
#include "VVASE/gui/components/mainFrame.h"
#include "../editPanel.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/debugger.h"

namespace VVASE
{

//==========================================================================
// Class:			EditSweepOptionsPanel
// Function:		EditSweepOptionsPanel
//
// Description:		Constructor for EditSweepOptionsPanel class.  Initializes the form
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
EditSweepOptionsPanel::EditSweepOptionsPanel(EditSweepNotebook &parent,
	wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxScrolledWindow(&parent, id, pos, size), parent(parent)
{
	currentSweep = NULL;
	CreateControls();
}

//==========================================================================
// Class:			EditSweepOptionsPanel
// Function:		~EditSweepOptionsPanel
//
// Description:		Destructor for EditSweepOptionsPanel class.
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
EditSweepOptionsPanel::~EditSweepOptionsPanel()
{
}

//==========================================================================
// Class:			EditSweepOptionsPanel
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
BEGIN_EVENT_TABLE(EditSweepOptionsPanel, wxPanel)
	EVT_CHECKBOX(CheckBoxSweepOptions,	EditSweepOptionsPanel::OptionsCheckBoxEvent)
	EVT_TEXT(TextBoxSweepOptions,		EditSweepOptionsPanel::OptionsTextBoxEvent)
	EVT_BUTTON(ButtonSetAsDefault,		EditSweepOptionsPanel::SetAsDefaultClickedEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditSweepOptionsPanel
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
void EditSweepOptionsPanel::UpdateInformation()
{
	if (currentSweep)
		UpdateInformation(currentSweep);
}

//==========================================================================
// Class:			EditSweepOptionsPanel
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
void EditSweepOptionsPanel::UpdateInformation(Sweep *currentSweep)
{
	this->currentSweep = currentSweep;
	if (!currentSweep)
		return;

	// Update the controls
	// We also enable/disable the label text boxes depending on the state of the checkbox
	autoTitle->SetValue(currentSweep->GetAutoGenerateTitle());
	autoLabelXAxis->SetValue(currentSweep->GetAutoGenerateXLabel());
	autoLabelZAxis->SetValue(currentSweep->GetAutoGenerateZLabel());
	showGridLines->SetValue(currentSweep->GetShowGridLines());

	titleText->ChangeValue(currentSweep->GetTitle());
	xLabelText->ChangeValue(currentSweep->GetXLabel());
	zLabelText->ChangeValue(currentSweep->GetZLabel());

	titleText->Enable(!autoTitle->GetValue());
	xLabelText->Enable(!autoLabelXAxis->GetValue());
	zLabelText->Enable(!autoLabelZAxis->GetValue());
}

//==========================================================================
// Class:			EditSweepOptionsPanel
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
void EditSweepOptionsPanel::CreateControls()
{
	// Enable scrolling
	SetScrollRate(10, 10);

	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 1, wxGROW | wxALL, 5);

	int cellPadding(3);

	// Creat the checkboxes
	showGridLines = new wxCheckBox(this, CheckBoxSweepOptions, _T("Show Grid Lines"));
	autoLabelXAxis = new wxCheckBox(this, CheckBoxSweepOptions, _T("Automatic"));
	autoLabelZAxis = new wxCheckBox(this, CheckBoxSweepOptions, _T("Automatic"));
	autoTitle = new wxCheckBox(this, CheckBoxSweepOptions, _T("From File Name"));

	titleText = new wxTextCtrl(this, TextBoxSweepOptions);
	xLabelText = new wxTextCtrl(this, TextBoxSweepOptions);
	zLabelText = new wxTextCtrl(this, TextBoxSweepOptions);

	setAsDefault = new wxButton(this, ButtonSetAsDefault, _T("Set As Default Properties"));

	// Add the controls to the sizer (and create the necessary labels)
	mainSizer->Add(new wxStaticText(this, wxID_ANY, _T("Plot Title")));
	mainSizer->Add(autoTitle, 0, wxALL, cellPadding);
	mainSizer->Add(titleText, 0, wxALL | wxGROW, cellPadding);
	mainSizer->AddSpacer(2 * cellPadding);

	mainSizer->Add(new wxStaticText(this, wxID_ANY, _T("X-Axis Label")));
	mainSizer->Add(autoLabelXAxis, 0, wxALL, cellPadding);
	mainSizer->Add(xLabelText, 0, wxALL | wxGROW, cellPadding);
	mainSizer->AddSpacer(2 * cellPadding);

	mainSizer->Add(new wxStaticText(this, wxID_ANY, _T("Y-Axis Label")));
	mainSizer->Add(autoLabelZAxis, 0, wxALL, cellPadding);
	mainSizer->Add(zLabelText, 0, wxALL | wxGROW, cellPadding);
	mainSizer->AddSpacer(4 * cellPadding);

	mainSizer->Add(showGridLines, 0, wxALL, cellPadding);

	mainSizer->AddStretchSpacer();

	mainSizer->Add(setAsDefault, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, cellPadding);

	// Assign the top level sizer to the panel
	SetSizer(topSizer);
}

//==========================================================================
// Class:			EditSweepOptionsPanel
// Function:		OptionsCheckBoxEvent
//
// Description:		Event handler for when checkboxes are toggled.
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
void EditSweepOptionsPanel::OptionsCheckBoxEvent(wxCommandEvent& WXUNUSED(event))
{
	// Set all of the checkbox-type parameters
	currentSweep->SetAutoGenerateTitle(autoTitle->GetValue());
	currentSweep->SetAutoGenerateXLabel(autoLabelXAxis->GetValue());
	currentSweep->SetAutoGenerateZLabel(autoLabelZAxis->GetValue());
	currentSweep->SetShowGridLines(showGridLines->GetValue());

	// Enable/disable the text boxes depending on the state of the checkboxes
	titleText->Enable(!autoTitle->GetValue());
	xLabelText->Enable(!autoLabelXAxis->GetValue());
	zLabelText->Enable(!autoLabelZAxis->GetValue());

	// Set the modified flag
	currentSweep->SetModified();

	// Update the display
	currentSweep->UpdateDisplay();
}

//==========================================================================
// Class:			EditSweepOptionsPanel
// Function:		OptionsTextBoxEvent
//
// Description:		Event handler for when text boxes are edited.
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
void EditSweepOptionsPanel::OptionsTextBoxEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure all values are valid ASCII (or we might crash FTGL)
	if (!titleText->GetValue().IsAscii() ||
		!xLabelText->GetValue().IsAscii() ||
		!zLabelText->GetValue().IsAscii())
		return;

	currentSweep->SetTitle(titleText->GetValue());
	currentSweep->SetXLabel(xLabelText->GetValue());
	currentSweep->SetZLabel(zLabelText->GetValue());

	currentSweep->SetModified();
	currentSweep->UpdateDisplay();
}

//==========================================================================
// Class:			EditSweepOptionsPanel
// Function:		SetAsDefaultClickedEvent
//
// Description:		Event handler for when the Set As Default button is clicked.
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
void EditSweepOptionsPanel::SetAsDefaultClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	currentSweep->WriteDefaultsToConfig();

	// FIXME:  Would be nice if this could save other plot options, like axis associativity, line colors, etc.

	// Display a message to the user so they know the changes took effect
	Debugger::GetInstance() << "Current plot settings saved as default" << Debugger::Priority::High;
}

}// namespace VVASE
