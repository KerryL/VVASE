/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_iteration_options_panel_class.cpp
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_ITERATION_OPTIONS_PANEL
//				 class.
// History:

// CarDesigner headers
#include "gui/iteration_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/components/edit_panel/edit_panel_class.h"
#include "gui/components/edit_panel/iteration/edit_iteration_options_panel_class.h"
#include "gui/components/edit_panel/iteration/edit_iteration_notebook_class.h"
#include "vUtilities/convert_class.h"

//==========================================================================
// Class:			EDIT_ITERATION_OPTIONS_PANEL
// Function:		EDIT_ITERATION_OPTIONS_PANEL
//
// Description:		Constructor for EDIT_ITERATION_OPTIONS_PANEL class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		_Parent		= EDIT_ITERATION_NOTEBOOK&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		_Debugger	= const DEBUGGER& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EDIT_ITERATION_OPTIONS_PANEL::EDIT_ITERATION_OPTIONS_PANEL(EDIT_ITERATION_NOTEBOOK &_Parent,
														   wxWindowID id, const wxPoint& pos,
														   const wxSize& size, const DEBUGGER &_Debugger) :
														   wxPanel(&_Parent, id, pos, size), Debugger(_Debugger),
														   Parent(_Parent)
{
	// Initialize the current object variable
	CurrentIteration = NULL;

	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EDIT_ITERATION_OPTIONS_PANEL
// Function:		~EDIT_ITERATION_OPTIONS_PANEL
//
// Description:		Destructor for EDIT_ITERATION_OPTIONS_PANEL class.
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
EDIT_ITERATION_OPTIONS_PANEL::~EDIT_ITERATION_OPTIONS_PANEL()
{
}

//==========================================================================
// Class:			EDIT_ITERATION_OPTIONS_PANEL
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
BEGIN_EVENT_TABLE(EDIT_ITERATION_OPTIONS_PANEL, wxPanel)
	EVT_CHECKBOX(CheckBoxIterationOptions,	EDIT_ITERATION_OPTIONS_PANEL::OptionsCheckBoxEvent)
	EVT_TEXT(TextBoxIterationOptions,		EDIT_ITERATION_OPTIONS_PANEL::OptionsTextBoxEvent)
	EVT_BUTTON(ButtonSetAsDefault,			EDIT_ITERATION_OPTIONS_PANEL::SetAsDefaultClickedEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EDIT_ITERATION_OPTIONS_PANEL
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
void EDIT_ITERATION_OPTIONS_PANEL::UpdateInformation(void)
{
	// Make sure the suspension object exists
	if (CurrentIteration)
		// Call the method that performs the update
		UpdateInformation(CurrentIteration);

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_OPTIONS_PANEL
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
void EDIT_ITERATION_OPTIONS_PANEL::UpdateInformation(ITERATION *_CurrentIteration)
{
	// Update the class members
	CurrentIteration = _CurrentIteration;

	// Make sure the iteration is valid
	if (!CurrentIteration)
		return;

	// Update the controls
	// We also enable/disable the label text boxes depending on the state of the checkbox
	AutoTitle->SetValue(CurrentIteration->GetAutoGenerateTitle());
	AutoLabelXAxis->SetValue(CurrentIteration->GetAutoGenerateXLabel());
	AutoLabelZAxis->SetValue(CurrentIteration->GetAutoGenerateZLabel());
	ShowGridLines->SetValue(CurrentIteration->GetShowGridLines());

	TitleText->ChangeValue(CurrentIteration->GetTitle());
	XLabelText->ChangeValue(CurrentIteration->GetXLabel());
	ZLabelText->ChangeValue(CurrentIteration->GetZLabel());

	TitleText->Enable(!AutoTitle->GetValue());
	XLabelText->Enable(!AutoLabelXAxis->GetValue());
	ZLabelText->Enable(!AutoLabelZAxis->GetValue());

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_OPTIONS_PANEL
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
void EDIT_ITERATION_OPTIONS_PANEL::CreateControls()
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 1, wxALIGN_CENTER_HORIZONTAL | wxGROW | wxALL, 5);

	// Creat the checkboxes
	ShowGridLines = new wxCheckBox(this, CheckBoxIterationOptions, _T("Show Grid Lines"));
	AutoLabelXAxis = new wxCheckBox(this, CheckBoxIterationOptions, _T("Automatic"));
	AutoLabelZAxis = new wxCheckBox(this, CheckBoxIterationOptions, _T("Automatic"));
	AutoTitle = new wxCheckBox(this, CheckBoxIterationOptions, _T("From File Name"));

	TitleText = new wxTextCtrl(this, TextBoxIterationOptions);
	XLabelText = new wxTextCtrl(this, TextBoxIterationOptions);
	ZLabelText = new wxTextCtrl(this, TextBoxIterationOptions);

	SetAsDefault = new wxButton(this, ButtonSetAsDefault, _T("Set As Default Properties"), wxDefaultPosition, wxSize(200, -1));

	// Add the controls to the sizer (and create the necessary labels)
	MainSizer->Add(new wxStaticText(this, wxID_ANY, _T("Plot Title")));
	MainSizer->Add(AutoTitle, 0, wxALL, 5);
	MainSizer->Add(TitleText, 0, wxALL | wxGROW, 5);
	MainSizer->AddSpacer(10);

	MainSizer->Add(new wxStaticText(this, wxID_ANY, _T("X-Axis Label")));
	MainSizer->Add(AutoLabelXAxis, 0, wxALL, 5);
	MainSizer->Add(XLabelText, 0, wxALL | wxGROW, 5);
	MainSizer->AddSpacer(10);

	MainSizer->Add(new wxStaticText(this, wxID_ANY, _T("Y-Axis Label")));
	MainSizer->Add(AutoLabelZAxis, 0, wxALL, 5);
	MainSizer->Add(ZLabelText, 0, wxALL | wxGROW, 5);
	MainSizer->AddSpacer(20);

	MainSizer->Add(ShowGridLines, 0, wxALL, 5);

	MainSizer->AddStretchSpacer();

	MainSizer->Add(SetAsDefault, 0, wxALIGN_CENTER_HORIZONTAL | wxALL | wxALIGN_BOTTOM, 5);

	// Assign the top level sizer to the panel
	SetSizer(TopSizer);

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_OPTIONS_PANEL
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
void EDIT_ITERATION_OPTIONS_PANEL::OptionsCheckBoxEvent(wxCommandEvent& WXUNUSED(event))
{
	// Set all of the checkbox-type parameters
	CurrentIteration->SetAutoGenerateTitle(AutoTitle->GetValue());
	CurrentIteration->SetAutoGenerateXLabel(AutoLabelXAxis->GetValue());
	CurrentIteration->SetAutoGenerateZLabel(AutoLabelZAxis->GetValue());
	CurrentIteration->SetShowGridLines(ShowGridLines->GetValue());

	// Enable/disable the text boxes depending on the state of the checkboxes
	TitleText->Enable(!AutoTitle->GetValue());
	XLabelText->Enable(!AutoLabelXAxis->GetValue());
	ZLabelText->Enable(!AutoLabelZAxis->GetValue());

	// Set the modified flag
	CurrentIteration->SetModified();

	// Update the display
	CurrentIteration->UpdateDisplay();

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_OPTIONS_PANEL
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
void EDIT_ITERATION_OPTIONS_PANEL::OptionsTextBoxEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure all values are valid ASCII (or we might crash FTGL)
	if (!TitleText->GetValue().IsAscii() ||
		!XLabelText->GetValue().IsAscii() ||
		!ZLabelText->GetValue().IsAscii())
		return;

	// Set all of the text box-type parameters
	CurrentIteration->SetTitle(TitleText->GetValue());
	CurrentIteration->SetXLabel(XLabelText->GetValue());
	CurrentIteration->SetZLabel(ZLabelText->GetValue());

	// Set the modified flag
	CurrentIteration->SetModified();

	// Update the display
	CurrentIteration->UpdateDisplay();

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_OPTIONS_PANEL
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
void EDIT_ITERATION_OPTIONS_PANEL::SetAsDefaultClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Save the current settings to file
	CurrentIteration->WriteDefaultsToConfig();

	// Display a message to the user so they know the changes took effect
	Debugger.Print(_T("Current plot settings saved as default"), DEBUGGER::PriorityHigh);

	return;
}