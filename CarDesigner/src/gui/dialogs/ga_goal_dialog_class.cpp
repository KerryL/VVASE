/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  ga_goal_dialog_class.cpp
// Created:  7/30/2009
// Author:  K. Loux
// Description:  Dialog for editing goal properties.
// History:

// wxWidgets headers
#include <wx/gbsizer.h>

// CarDesigner headers
#include "gui/dialogs/ga_goal_dialog_class.h"
#include "vUtilities/wxRelatedUtilities.h"

//==========================================================================
// Class:			GA_GOAL_DIALOG
// Function:		GA_GOAL_DIALOG
//
// Description:		Constructor for the GA_GOAL_DIALOG class.
//
// Input Arguments:
//		Parent				= wxWindow&, reference to the main application window
//		_Converter			= const Convert&, reference to conversion utility object
//		_Output				= const KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE&
//		_DesiredValue		= const double&
//		_ExpectedDeviation	= const double&
//		_Importance			= const double&
//		_BeforeInputs		= const KINEMATICS::INPUTS&
//		_AfterInputs		= const KINEMATICS::INPUTS&
//		Id					= wxWindowID
//		Position			= const wxPoint&
//		Style				= long
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GA_GOAL_DIALOG::GA_GOAL_DIALOG(wxWindow *Parent, const Convert &_Converter, const KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE &_Output,
							   const double &_DesiredValue, const double &_ExpectedDeviation, const double &_Importance,
							   const KINEMATICS::INPUTS &_BeforeInputs, const KINEMATICS::INPUTS &_AfterInputs,
							   wxWindowID Id, const wxPoint &Position, long Style) :
							   wxDialog(Parent, Id, _T("Genetic Algorithm Goal"), Position, wxDefaultSize, Style),
							   Converter(_Converter)
{
	// Assign local members
	Output = _Output;
	DesiredValue = _DesiredValue;
	ExpectedDeviation = _ExpectedDeviation;
	Importance = _Importance;
	BeforeInputs = _BeforeInputs;
	AfterInputs = _AfterInputs;

	// Create the controls
	CreateControls();

	// Center the dialog on screen
	Center();
}

//==========================================================================
// Class:			GA_GOAL_DIALOG
// Function:		~GA_GOAL_DIALOG
//
// Description:		Destructor for the GA_GOAL_DIALOG class.
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
GA_GOAL_DIALOG::~GA_GOAL_DIALOG()
{
}

//==========================================================================
// Class:			GA_GOAL_DIALOG
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
BEGIN_EVENT_TABLE(GA_GOAL_DIALOG, wxDialog)
	EVT_BUTTON(wxID_OK,		GA_GOAL_DIALOG::OKClickEvent)
	EVT_BUTTON(wxID_CANCEL,	GA_GOAL_DIALOG::CancelClickEvent)
	EVT_CHECKBOX(wxID_ANY,	GA_GOAL_DIALOG::OnCheckEvent)
	EVT_COMBOBOX(wxID_ANY,	GA_GOAL_DIALOG::OnOutputChangeEvent)
END_EVENT_TABLE()

//==========================================================================
// Class:			GA_GOAL_DIALOG
// Function:		CreateControls
//
// Description:		Creates the controls that populate this dialog.  Also fills
//					controls with approprite data (passed via constructor).
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
void GA_GOAL_DIALOG::CreateControls(void)
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create a sizer for the inputs
	int cellPadding(5);
	wxGridBagSizer *inputAreaSizer = new wxGridBagSizer(cellPadding, cellPadding);
	MainSizer->Add(inputAreaSizer, wxALL, 5);

	// Set sizer flags
	int sizerFlags = wxEXPAND | wxALIGN_CENTER_VERTICAL;

	// When setting the control width, we need to account for the width of the
	// "expand" button, etc., so we specify that here
#ifdef __WXGTK__
	unsigned int additionalWidth = 40;
#else
	unsigned int additionalWidth = 30;
#endif

	// Add the controls to these sizers
	// Output
	int row(1);
	int i;
	wxArrayString List;
	for (i = 0; i < KINEMATIC_OUTPUTS::NumberOfOutputScalars; i++)
		List.Add(KINEMATIC_OUTPUTS::GetOutputName((KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)i));
	wxStaticText *OutputLabel = new wxStaticText(this, wxID_STATIC, _T("Output Parameter"));
	OutputCombo = new wxComboBox(this, wxID_ANY, KINEMATIC_OUTPUTS::GetOutputName(Output), wxDefaultPosition,
		wxDefaultSize, List, wxCB_READONLY);
	SetMinimumWidthFromContents(OutputCombo, additionalWidth);
	inputAreaSizer->Add(OutputLabel, wxGBPosition(row, 1), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(OutputCombo, wxGBPosition(row, 2), wxGBSpan(1, 3), sizerFlags);
	row++;

#ifdef __WXGTK__
	// Under GTK, the combo box selections are set to -1 until the user changes the selection, even if
	// we set a default value when we construct the controls.  This causes errors when getting the
	// selection and attempting to assign it to a gene.  To combat this effect, we set the selection
	// manually here.
	OutputCombo->SetSelection((int)Output);
#endif

	// Difference check box
	Difference = new wxCheckBox(this, wxID_ANY, _T("Optimize difference between two states"));
	inputAreaSizer->Add(Difference, wxGBPosition(row, 1), wxGBSpan(1, 4), wxALIGN_CENTER_HORIZONTAL | sizerFlags);
	Difference->SetValue(BeforeInputs != AfterInputs);
	row++;

	// Before and after labels
	BeforeLabel = new wxStaticText(this, wxID_STATIC, _T("State 1"));
	AfterLabel = new wxStaticText(this, wxID_STATIC, _T("State 2"));
	inputAreaSizer->Add(BeforeLabel, wxGBPosition(row, 2), wxGBSpan(), wxALIGN_CENTER_HORIZONTAL | sizerFlags);
	inputAreaSizer->Add(AfterLabel, wxGBPosition(row, 3), wxGBSpan(), wxALIGN_CENTER_HORIZONTAL | sizerFlags);
	row++;

	// Pitch inputs
	wxStaticText *PitchLabel = new wxStaticText(this, wxID_STATIC, _T("Pitch"));
	BeforePitchText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertAngle(BeforeInputs.Pitch)));
	AfterPitchText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertAngle(AfterInputs.Pitch)));
	wxStaticText *PitchUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeAngle));
	inputAreaSizer->Add(PitchLabel, wxGBPosition(row, 1), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(BeforePitchText, wxGBPosition(row, 2), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(AfterPitchText, wxGBPosition(row, 3), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(PitchUnitsLabel, wxGBPosition(row, 4), wxGBSpan(), sizerFlags);
	row++;

	// Roll inputs
	wxStaticText *RollLabel = new wxStaticText(this, wxID_STATIC, _T("Roll"));
	BeforeRollText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertAngle(BeforeInputs.Roll)));
	AfterRollText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertAngle(AfterInputs.Roll)));
	wxStaticText *RollUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeAngle));
	inputAreaSizer->Add(RollLabel, wxGBPosition(row, 1), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(BeforeRollText, wxGBPosition(row, 2), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(AfterRollText, wxGBPosition(row, 3), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(RollUnitsLabel, wxGBPosition(row, 4), wxGBSpan(), sizerFlags);
	row++;

	// Heave inputs
	wxStaticText *HeaveLabel = new wxStaticText(this, wxID_STATIC, _T("Heave"));
	BeforeHeaveText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertDistance(BeforeInputs.Heave)));
	AfterHeaveText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertDistance(AfterInputs.Heave)));
	wxStaticText *HeaveUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeDistance));
	inputAreaSizer->Add(HeaveLabel, wxGBPosition(row, 1), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(BeforeHeaveText, wxGBPosition(row, 2), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(AfterHeaveText, wxGBPosition(row, 3), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(HeaveUnitsLabel, wxGBPosition(row, 4), wxGBSpan(), sizerFlags);
	row++;

	// Steer inputs
	wxStaticText *SteerLabel = new wxStaticText(this, wxID_STATIC, _T("Rack Travel"));
	BeforeSteerText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertDistance(BeforeInputs.RackTravel)));
	AfterSteerText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertDistance(AfterInputs.RackTravel)));
	wxStaticText *SteerUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeDistance));
	inputAreaSizer->Add(SteerLabel, wxGBPosition(row, 1), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(BeforeSteerText, wxGBPosition(row, 2), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(AfterSteerText, wxGBPosition(row, 3), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(SteerUnitsLabel, wxGBPosition(row, 4), wxGBSpan(), sizerFlags);
	row++;

	// Desired value
	DesiredValueLabel = new wxStaticText(this, wxID_STATIC, _T("Desired Value"));
	DesiredValueText = new wxTextCtrl(this, wxID_ANY, Converter.FormatNumber(Converter.ConvertTo(DesiredValue,
		KINEMATIC_OUTPUTS::GetOutputUnitType(Output))));
	DesiredValueUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(KINEMATIC_OUTPUTS::GetOutputUnitType(Output)));
	inputAreaSizer->Add(DesiredValueLabel, wxGBPosition(row, 1), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(DesiredValueText, wxGBPosition(row, 2), wxGBSpan(1, 2), sizerFlags);
	inputAreaSizer->Add(DesiredValueUnitsLabel, wxGBPosition(row, 4), wxGBSpan(), sizerFlags);
	row++;

	// Expected deviation
	wxStaticText *DeviationLabel = new wxStaticText(this, wxID_STATIC, _T("Expected Deviation"));
	DeviationText = new wxTextCtrl(this, wxID_ANY, Converter.FormatNumber(Converter.ConvertTo(ExpectedDeviation,
		KINEMATIC_OUTPUTS::GetOutputUnitType(Output))));
	DeviationUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(KINEMATIC_OUTPUTS::GetOutputUnitType(Output)));
	inputAreaSizer->Add(DeviationLabel, wxGBPosition(row, 1), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(DeviationText, wxGBPosition(row, 2), wxGBSpan(1, 2), sizerFlags);
	inputAreaSizer->Add(DeviationUnitsLabel, wxGBPosition(row, 4), wxGBSpan(), sizerFlags);
	row++;

	// Importance
	wxStaticText *ImportanceLabel = new wxStaticText(this, wxID_STATIC, _T("Relative Importance"));
	ImportanceText = new wxTextCtrl(this, wxID_ANY, Converter.FormatNumber(Importance));
	inputAreaSizer->Add(ImportanceLabel, wxGBPosition(row, 1), wxGBSpan(), sizerFlags);
	inputAreaSizer->Add(ImportanceText, wxGBPosition(row, 2), wxGBSpan(1, 2), sizerFlags);
	row++;

	// The width of the second and third columns should be equal
	// We do this by creating a blank row with the widths we specify
	inputAreaSizer->Add(-1, PitchLabel->GetSize().GetHeight(), wxGBPosition(3, 1), wxGBSpan());
	int inputTextWidth = (OutputCombo->GetMinWidth() - PitchUnitsLabel->GetSize().GetWidth()) / 2 - 2 * cellPadding;
	inputAreaSizer->Add(inputTextWidth, 15, wxGBPosition(row, 2), wxGBSpan());
	inputAreaSizer->Add(inputTextWidth, 15, wxGBPosition(row, 3), wxGBSpan());

	// Set the minimum widths of the text controls
	BeforePitchText->SetMinSize(wxSize(inputTextWidth, -1));
	AfterPitchText->SetMinSize(wxSize(inputTextWidth, -1));
	BeforeRollText->SetMinSize(wxSize(inputTextWidth, -1));
	AfterRollText->SetMinSize(wxSize(inputTextWidth, -1));
	BeforeHeaveText->SetMinSize(wxSize(inputTextWidth, -1));
	AfterHeaveText->SetMinSize(wxSize(inputTextWidth, -1));
	BeforeSteerText->SetMinSize(wxSize(inputTextWidth, -1));
	AfterSteerText->SetMinSize(wxSize(inputTextWidth, -1));

	// Create another sizer for the buttons at the bottom and add the buttons
	wxBoxSizer *ButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *OKButton = new wxButton(this, wxID_OK, _T("OK"),
		wxDefaultPosition, wxDefaultSize, 0);
	wxButton *CancelButton = new wxButton(this, wxID_CANCEL, _T("Cancel"),
		wxDefaultPosition, wxDefaultSize, 0);
	ButtonsSizer->Add(OKButton, 0, wxALL, 5);
	ButtonsSizer->Add(CancelButton, 0, wxALL, 5);
	MainSizer->Add(ButtonsSizer, 0, wxALIGN_CENTER_HORIZONTAL);

	// Make the OK button default
	OKButton->SetDefault();

	// Tell the dialog to auto-adjust it's size
	TopSizer->SetSizeHints(this);

	// Assign the top level sizer to the dialog
	SetSizer(TopSizer);

	// Set the checkbox as appropriate for this goal.  This will handle showing/hiding the
	// controls and labeling the desired value as appropriate.
	FormatDialogDifference();

	return;
}

//==========================================================================
// Class:			GA_GOAL_DIALOG
// Function:		OKClickEvent
//
// Description:		Handles the OK button clicked events.
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
void GA_GOAL_DIALOG::OKClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Update the class members with the data currently displayed in the dialog controls
	if (!DesiredValueText->GetValue().ToDouble(&DesiredValue) ||
		!DeviationText->GetValue().ToDouble(&ExpectedDeviation) ||
		!ImportanceText->GetValue().ToDouble(&Importance) ||
		!BeforeHeaveText->GetValue().ToDouble(&BeforeInputs.Heave) ||
		!BeforePitchText->GetValue().ToDouble(&BeforeInputs.Pitch) ||
		!BeforeRollText->GetValue().ToDouble(&BeforeInputs.Roll) ||
		!BeforeSteerText->GetValue().ToDouble(&BeforeInputs.RackTravel))
	{
		wxMessageBox(_T("ERROR:  All values must be numeric!"), _T("Error Validating Data"),
			wxOK | wxICON_ERROR, this);
		return;
	}

	// Check the after inputs separately, because we may not actually read them
	if (Difference->IsChecked())
	{
		if (!AfterHeaveText->GetValue().ToDouble(&AfterInputs.Heave) ||
			!AfterPitchText->GetValue().ToDouble(&AfterInputs.Pitch) ||
			!AfterRollText->GetValue().ToDouble(&AfterInputs.Roll) ||
			!AfterSteerText->GetValue().ToDouble(&AfterInputs.RackTravel))
		{
			wxMessageBox(_T("ERROR:  All values must be numeric!"), _T("Error Validating Data"),
				wxOK | wxICON_ERROR, this);
			return;
		}
	}

	// Expected deviation cannot be zero
	if (ExpectedDeviation == 0.0)
	{
		wxMessageBox(_T("ERROR:  Expected deviaiton cannot be zero!"), _T("Error Validating Data"),
			wxOK | wxICON_ERROR, this);
		return;
	}

	Output = (KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)OutputCombo->GetCurrentSelection();

	// Convert the input values
	DesiredValue = Converter.Read(DesiredValue, KINEMATIC_OUTPUTS::GetOutputUnitType(Output));
	ExpectedDeviation = Converter.Read(ExpectedDeviation, KINEMATIC_OUTPUTS::GetOutputUnitType(Output));
	BeforeInputs.Pitch = Converter.ReadAngle(BeforeInputs.Pitch);
	BeforeInputs.Roll = Converter.ReadAngle(BeforeInputs.Roll);
	BeforeInputs.Heave = Converter.ReadDistance(BeforeInputs.Heave);
	BeforeInputs.RackTravel = Converter.ReadDistance(BeforeInputs.RackTravel);

	if (Difference->IsChecked())
	{
		AfterInputs.Pitch = Converter.ReadAngle(AfterInputs.Pitch);
		AfterInputs.Roll = Converter.ReadAngle(AfterInputs.Roll);
		AfterInputs.Heave = Converter.ReadDistance(AfterInputs.Heave);
		AfterInputs.RackTravel = Converter.ReadDistance(AfterInputs.RackTravel);
	}
	else
		AfterInputs = BeforeInputs;

	// The way we handle this changes depending on how this form was displayed
	if (IsModal())
		EndModal(wxOK);
	else
	{
		SetReturnCode(wxOK);
		Show(false);
	}

	return;
}

//==========================================================================
// Class:			GA_GOAL_DIALOG
// Function:		CancelClickEvent
//
// Description:		Handles the Cancel button clicked event.
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
void GA_GOAL_DIALOG::CancelClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// The way we handle this changes depending on how this form was displayed
	if (IsModal())
		EndModal(wxID_CANCEL);
	else
	{
		SetReturnCode(wxID_CANCEL);
		Show(false);
	}

	return;
}

//==========================================================================
// Class:			GA_GOAL_DIALOG
// Function:		OnCheckEvent
//
// Description:		Event handler for the "Use Difference" checkbox.  This is
//					responsible for hiding/showing the second set of inputs
//					controls and changing the name on the desired value label
//					as appropriate.
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
void GA_GOAL_DIALOG::OnCheckEvent(wxCommandEvent& WXUNUSED(event))
{
	// Update the dialog depending on the value of the checkbox
	FormatDialogDifference();

	return;
}

//==========================================================================
// Class:			GA_GOAL_DIALOG
// Function:		OnOutputChangeEvent
//
// Description:		Event handler when selected output changes.  Updates the
//					units for the desired value and expected deviation.
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
void GA_GOAL_DIALOG::OnOutputChangeEvent(wxCommandEvent &event)
{
	// Update the units labels for the desired value and expected deviation
	DesiredValueUnitsLabel->SetLabel(Converter.GetUnitType(
		KINEMATIC_OUTPUTS::GetOutputUnitType((KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)event.GetSelection())));
	DeviationUnitsLabel->SetLabel(Converter.GetUnitType(
		KINEMATIC_OUTPUTS::GetOutputUnitType((KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)event.GetSelection())));

	return;
}

//==========================================================================
// Class:			GA_GOAL_DIALOG
// Function:		FormatDialogDifference
//
// Description:		Formats the dialog according to whether we want two input
//					states or one.
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
void GA_GOAL_DIALOG::FormatDialogDifference(void)
{
	// Hide or display the second set of KINEMATICS::INPUTS controls
	if (Difference->IsChecked())
	{
		// Show the controls
		AfterPitchText->Show();
		AfterRollText->Show();
		AfterHeaveText->Show();
		AfterSteerText->Show();
		BeforeLabel->Show();
		AfterLabel->Show();

		// The desired value is actually a desired change
		DesiredValueLabel->SetLabel(_T("Desired Change"));
	}
	else
	{
		// Hide the controls
		AfterPitchText->Hide();
		AfterRollText->Hide();
		AfterHeaveText->Hide();
		AfterSteerText->Hide();
		BeforeLabel->Hide();
		AfterLabel->Hide();

		// The desired value really is a single value
		DesiredValueLabel->SetLabel(_T("Desired Value"));
	}

	// Update the appearance of this dialog
	this->GetSizer()->SetSizeHints(this);
	this->GetSizer()->Layout();

	return;
}