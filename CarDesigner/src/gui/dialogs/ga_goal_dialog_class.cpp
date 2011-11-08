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

// CarDesigner headers
#include "gui/dialogs/ga_goal_dialog_class.h"

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

	// Create a sizer for each of the inputs
	wxBoxSizer *OutputSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *InputsSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *DesiredValueSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *ExpectedDeviationSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *ImportanceSizer = new wxBoxSizer(wxHORIZONTAL);

	// Set up the column widths
	int LabelColumnWidth(100);
	int InputColumnWidth(160);
	int UnitsColumnWidth(40);
	int CellPadding(5);

	// Add the controls to these sizers
	// Output
	int i;
	wxArrayString List;
	for (i = 0; i < KINEMATIC_OUTPUTS::NumberOfOutputScalars; i++)
		List.Add(KINEMATIC_OUTPUTS::GetOutputName((KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)i));
	wxStaticText *OutputLabel = new wxStaticText(this, wxID_STATIC, _T("Output Parameter"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	OutputCombo = new wxComboBox(this, wxID_ANY, KINEMATIC_OUTPUTS::GetOutputName(Output), wxDefaultPosition,
		wxSize(InputColumnWidth + CellPadding + UnitsColumnWidth, -1), List, wxCB_READONLY);
	OutputSizer->Add(OutputLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	OutputSizer->Add(OutputCombo, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

#ifdef __WXGTK__
	// Under GTK, the combo box selections are set to -1 until the user changes the selection, even if
	// we set a default value when we construct the controls.  This causes errors when getting the
	// selection and attempting to assign it to a gene.  To combat this effect, we set the selection
	// manually here.
	OutputCombo->SetSelection((int)Output);
#endif

	// Input set
	wxBoxSizer *DifferenceSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *BeforeAfterLabelSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *PitchInputSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *RollInputSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *HeaveInputSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *SteerInputSizer = new wxBoxSizer(wxHORIZONTAL);

	// Difference check box
	Difference = new wxCheckBox(this, wxID_ANY, _T("Optimize difference between two states"));
	DifferenceSizer->Add(Difference, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, CellPadding);
	Difference->SetValue(BeforeInputs != AfterInputs);

	// Before and after labels
	BeforeLabel = new wxStaticText(this, wxID_STATIC, _T("State 1"), wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	AfterLabel = new wxStaticText(this, wxID_STATIC, _T("State 2"), wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	BeforeLabelSpacer = new wxStaticText(this, wxID_STATIC, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	AfterLabelSpacer = new wxStaticText(this, wxID_STATIC, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	BeforeAfterLabelSizer->Add(LabelColumnWidth + 2 * CellPadding, 0, 0);
	BeforeAfterLabelSizer->Add(BeforeLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, CellPadding);
	BeforeAfterLabelSizer->Add(BeforeLabelSpacer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, CellPadding);
	BeforeAfterLabelSizer->Add(AfterLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, CellPadding);
	BeforeAfterLabelSizer->Add(AfterLabelSpacer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, CellPadding);

	// Pitch inputs
	wxStaticText *PitchLabel = new wxStaticText(this, wxID_STATIC, _T("Pitch"), wxDefaultPosition,
		wxSize(LabelColumnWidth, -1), 0);
	BeforePitchText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertAngle(BeforeInputs.Pitch)), wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	AfterPitchText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertAngle(AfterInputs.Pitch)), wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	AfterPitchSpacer = new wxStaticText(this, wxID_STATIC, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	wxStaticText *PitchUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UNIT_TYPE_ANGLE),
		wxDefaultPosition, wxSize(UnitsColumnWidth, -1));
	PitchInputSizer->Add(PitchLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	PitchInputSizer->Add(BeforePitchText, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	PitchInputSizer->Add(AfterPitchText, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	PitchInputSizer->Add(AfterPitchSpacer, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	PitchInputSizer->Add(PitchUnitsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);

	// Roll inputs
	wxStaticText *RollLabel = new wxStaticText(this, wxID_STATIC, _T("Roll"), wxDefaultPosition,
		wxSize(LabelColumnWidth, -1), 0);
	BeforeRollText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertAngle(BeforeInputs.Roll)), wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	AfterRollText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertAngle(AfterInputs.Roll)), wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	AfterRollSpacer = new wxStaticText(this, wxID_STATIC, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	wxStaticText *RollUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UNIT_TYPE_ANGLE),
		wxDefaultPosition, wxSize(UnitsColumnWidth, -1));
	RollInputSizer->Add(RollLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	RollInputSizer->Add(BeforeRollText, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	RollInputSizer->Add(AfterRollText, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	RollInputSizer->Add(AfterRollSpacer, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	RollInputSizer->Add(RollUnitsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);

	// Heave inputs
	wxStaticText *HeaveLabel = new wxStaticText(this, wxID_STATIC, _T("Heave"), wxDefaultPosition,
		wxSize(LabelColumnWidth, -1), 0);
	BeforeHeaveText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertDistance(BeforeInputs.Heave)), wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	AfterHeaveText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertDistance(AfterInputs.Heave)), wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	AfterHeaveSpacer = new wxStaticText(this, wxID_STATIC, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	wxStaticText *HeaveUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE),
		wxDefaultPosition, wxSize(UnitsColumnWidth, -1));
	HeaveInputSizer->Add(HeaveLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	HeaveInputSizer->Add(BeforeHeaveText, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	HeaveInputSizer->Add(AfterHeaveText, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	HeaveInputSizer->Add(AfterHeaveSpacer, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	HeaveInputSizer->Add(HeaveUnitsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);

	// Steer inputs
	wxStaticText *SteerLabel = new wxStaticText(this, wxID_STATIC, _T("Rack Travel"), wxDefaultPosition,
		wxSize(LabelColumnWidth, -1), 0);
	BeforeSteerText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertDistance(BeforeInputs.RackTravel)), wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	AfterSteerText = new wxTextCtrl(this, wxID_ANY,
		Converter.FormatNumber(Converter.ConvertDistance(AfterInputs.RackTravel)), wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	AfterSteerSpacer = new wxStaticText(this, wxID_STATIC, wxEmptyString, wxDefaultPosition,
		wxSize(InputColumnWidth / 2 - CellPadding, -1), 0);
	wxStaticText *SteerUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE),
		wxDefaultPosition, wxSize(UnitsColumnWidth, -1));
	SteerInputSizer->Add(SteerLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	SteerInputSizer->Add(BeforeSteerText, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	SteerInputSizer->Add(AfterSteerText, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	SteerInputSizer->Add(AfterSteerSpacer, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	SteerInputSizer->Add(SteerUnitsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);

	// Add the input sizers to the input sizer
	InputsSizer->Add(DifferenceSizer, 0, wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL);
	InputsSizer->Add(BeforeAfterLabelSizer, 0, wxALIGN_TOP);
	InputsSizer->Add(PitchInputSizer, 0, wxALIGN_TOP);
	InputsSizer->Add(RollInputSizer, 0, wxALIGN_TOP);
	InputsSizer->Add(HeaveInputSizer, 0, wxALIGN_TOP);
	InputsSizer->Add(SteerInputSizer, 0, wxALIGN_TOP);

	// Desired value
	DesiredValueLabel = new wxStaticText(this, wxID_STATIC, _T("Desired Value"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	DesiredValueText = new wxTextCtrl(this, wxID_ANY, Converter.FormatNumber(Converter.Convert(DesiredValue,
		KINEMATIC_OUTPUTS::GetOutputUnitType(Output))),
		wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	DesiredValueUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(KINEMATIC_OUTPUTS::GetOutputUnitType(Output)),
		wxDefaultPosition, wxSize(UnitsColumnWidth, -1), 0);
	DesiredValueSizer->Add(DesiredValueLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	DesiredValueSizer->Add(DesiredValueText, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	DesiredValueSizer->Add(DesiredValueUnitsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);

	// Expected deviation
	wxStaticText *DeviationLabel = new wxStaticText(this, wxID_STATIC, _T("Expected Deviation"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	DeviationText = new wxTextCtrl(this, wxID_ANY, Converter.FormatNumber(Converter.Convert(ExpectedDeviation,
		KINEMATIC_OUTPUTS::GetOutputUnitType(Output))),
		wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	DeviationUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(KINEMATIC_OUTPUTS::GetOutputUnitType(Output)),
		wxDefaultPosition, wxSize(UnitsColumnWidth, -1), 0);
	ExpectedDeviationSizer->Add(DeviationLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	ExpectedDeviationSizer->Add(DeviationText, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	ExpectedDeviationSizer->Add(DeviationUnitsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);

	// Importance
	wxStaticText *ImportanceLabel = new wxStaticText(this, wxID_STATIC, _T("Relative Importance"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	ImportanceText = new wxTextCtrl(this, wxID_ANY, Converter.FormatNumber(Importance),
		wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	ImportanceSizer->Add(ImportanceLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);
	ImportanceSizer->Add(ImportanceText, 0, wxALIGN_CENTER_VERTICAL | wxALL, CellPadding);

	// Add the sizers to the MainSizer
	MainSizer->Add(OutputSizer, 0, wxALIGN_TOP);
	MainSizer->Add(InputsSizer, 0, wxALIGN_TOP);
	MainSizer->Add(DesiredValueSizer, 0, wxALIGN_TOP);
	MainSizer->Add(ExpectedDeviationSizer, 0, wxALIGN_TOP);
	MainSizer->Add(ImportanceSizer, 0, wxALIGN_TOP);

	// Add a spacer between the text controls and the buttons
	MainSizer->AddSpacer(15);

	// Create another sizer for the buttons at the bottom and add the buttons
	wxBoxSizer *ButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *OKButton = new wxButton(this, wxID_OK, _T("OK"),
		wxDefaultPosition, wxDefaultSize, 0);
	wxButton *CancelButton = new wxButton(this, wxID_CANCEL, _T("Cancel"),
		wxDefaultPosition, wxDefaultSize, 0);
	ButtonsSizer->Add(OKButton, 0, wxALL, CellPadding);
	ButtonsSizer->Add(CancelButton, 0, wxALL, CellPadding);
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

		// Hide the spacers
		AfterPitchSpacer->Hide();
		AfterRollSpacer->Hide();
		AfterHeaveSpacer->Hide();
		AfterSteerSpacer->Hide();
		BeforeLabelSpacer->Hide();
		AfterLabelSpacer->Hide();

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

		// Show the spacers
		AfterPitchSpacer->Show();
		AfterRollSpacer->Show();
		AfterHeaveSpacer->Show();
		AfterSteerSpacer->Show();
		BeforeLabelSpacer->Show();
		AfterLabelSpacer->Show();

		// The desired value really is a single value
		DesiredValueLabel->SetLabel(_T("Desired Value"));
	}

	// Update the appearance of this dialog
	this->GetSizer()->SetSizeHints(this);
	this->GetSizer()->Layout();

	return;
}