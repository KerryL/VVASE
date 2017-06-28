/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSweepOptionsPanel.cpp
// Date:  11/14/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditIterationOptionsPanel
//        class.

// Local headers
#include "gui/iteration.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/iteration/editIterationOptionsPanel.h"
#include "gui/components/editPanel/iteration/editIterationNotebook.h"
#include "vUtilities/unitConverter.h"
#include "vUtilities/debugger.h"

//==========================================================================
// Class:			EditIterationOptionsPanel
// Function:		EditIterationOptionsPanel
//
// Description:		Constructor for EditIterationOptionsPanel class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		parent		= EditIterationNotebook&, reference to this object's owner
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
EditIterationOptionsPanel::EditIterationOptionsPanel(EditIterationNotebook &parent,
	wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxScrolledWindow(&parent, id, pos, size), parent(parent)
{
	currentIteration = NULL;
	CreateControls();
}

//==========================================================================
// Class:			EditIterationOptionsPanel
// Function:		~EditIterationOptionsPanel
//
// Description:		Destructor for EditIterationOptionsPanel class.
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
EditIterationOptionsPanel::~EditIterationOptionsPanel()
{
}

//==========================================================================
// Class:			EditIterationOptionsPanel
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
BEGIN_EVENT_TABLE(EditIterationOptionsPanel, wxPanel)
	EVT_CHECKBOX(CheckBoxIterationOptions,	EditIterationOptionsPanel::OptionsCheckBoxEvent)
	EVT_TEXT(TextBoxIterationOptions,		EditIterationOptionsPanel::OptionsTextBoxEvent)
	EVT_BUTTON(ButtonSetAsDefault,			EditIterationOptionsPanel::SetAsDefaultClickedEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditIterationOptionsPanel
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
void EditIterationOptionsPanel::UpdateInformation()
{
	if (currentIteration)
		UpdateInformation(currentIteration);
}

//==========================================================================
// Class:			EditIterationOptionsPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		currentIteration	= Iteration* pointing to the associated iteration
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditIterationOptionsPanel::UpdateInformation(Iteration *currentIteration)
{
	this->currentIteration = currentIteration;
	if (!currentIteration)
		return;

	// Update the controls
	// We also enable/disable the label text boxes depending on the state of the checkbox
	autoTitle->SetValue(currentIteration->GetAutoGenerateTitle());
	autoLabelXAxis->SetValue(currentIteration->GetAutoGenerateXLabel());
	autoLabelZAxis->SetValue(currentIteration->GetAutoGenerateZLabel());
	showGridLines->SetValue(currentIteration->GetShowGridLines());

	titleText->ChangeValue(currentIteration->GetTitle());
	xLabelText->ChangeValue(currentIteration->GetXLabel());
	zLabelText->ChangeValue(currentIteration->GetZLabel());

	titleText->Enable(!autoTitle->GetValue());
	xLabelText->Enable(!autoLabelXAxis->GetValue());
	zLabelText->Enable(!autoLabelZAxis->GetValue());
}

//==========================================================================
// Class:			EditIterationOptionsPanel
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
void EditIterationOptionsPanel::CreateControls()
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
	showGridLines = new wxCheckBox(this, CheckBoxIterationOptions, _T("Show Grid Lines"));
	autoLabelXAxis = new wxCheckBox(this, CheckBoxIterationOptions, _T("Automatic"));
	autoLabelZAxis = new wxCheckBox(this, CheckBoxIterationOptions, _T("Automatic"));
	autoTitle = new wxCheckBox(this, CheckBoxIterationOptions, _T("From File Name"));

	titleText = new wxTextCtrl(this, TextBoxIterationOptions);
	xLabelText = new wxTextCtrl(this, TextBoxIterationOptions);
	zLabelText = new wxTextCtrl(this, TextBoxIterationOptions);

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
// Class:			EditIterationOptionsPanel
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
void EditIterationOptionsPanel::OptionsCheckBoxEvent(wxCommandEvent& WXUNUSED(event))
{
	// Set all of the checkbox-type parameters
	currentIteration->SetAutoGenerateTitle(autoTitle->GetValue());
	currentIteration->SetAutoGenerateXLabel(autoLabelXAxis->GetValue());
	currentIteration->SetAutoGenerateZLabel(autoLabelZAxis->GetValue());
	currentIteration->SetShowGridLines(showGridLines->GetValue());

	// Enable/disable the text boxes depending on the state of the checkboxes
	titleText->Enable(!autoTitle->GetValue());
	xLabelText->Enable(!autoLabelXAxis->GetValue());
	zLabelText->Enable(!autoLabelZAxis->GetValue());

	// Set the modified flag
	currentIteration->SetModified();

	// Update the display
	currentIteration->UpdateDisplay();
}

//==========================================================================
// Class:			EditIterationOptionsPanel
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
void EditIterationOptionsPanel::OptionsTextBoxEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure all values are valid ASCII (or we might crash FTGL)
	if (!titleText->GetValue().IsAscii() ||
		!xLabelText->GetValue().IsAscii() ||
		!zLabelText->GetValue().IsAscii())
		return;

	currentIteration->SetTitle(titleText->GetValue());
	currentIteration->SetXLabel(xLabelText->GetValue());
	currentIteration->SetZLabel(zLabelText->GetValue());

	currentIteration->SetModified();
	currentIteration->UpdateDisplay();
}

//==========================================================================
// Class:			EditIterationOptionsPanel
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
void EditIterationOptionsPanel::SetAsDefaultClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	currentIteration->WriteDefaultsToConfig();

	// FIXME:  Would be nice if this could save other plot options, like axis associativity, line colors, etc.

	// Display a message to the user so they know the changes took effect
	Debugger::GetInstance() << "Current plot settings saved as default" << Debugger::PriorityHigh;
}
