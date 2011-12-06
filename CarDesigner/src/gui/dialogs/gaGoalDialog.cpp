/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  gaGoalDialog.cpp
// Created:  7/30/2009
// Author:  K. Loux
// Description:  Dialog for editing goal properties.
// History:

// wxWidgets headers
#include <wx/gbsizer.h>

// CarDesigner headers
#include "gui/dialogs/gaGoalDialog.h"
#include "vUtilities/wxRelatedUtilities.h"
#include "vUtilities/convert.h"

//==========================================================================
// Class:			GAGoalDialog
// Function:		GAGoalDialog
//
// Description:		Constructor for the GAGoalDialog class.
//
// Input Arguments:
//		parent				= wxWindow&, reference to the main application window
//		_output				= const KinematicOutputs::OutputsComplete&
//		_desiredValue		= const double&
//		_expectedDeviation	= const double&
//		_importance			= const double&
//		_beforeInputs		= const Kinematics::Inputs&
//		_afterInputs		= const Kinematics::Inputs&
//		id					= wxWindowID
//		position			= const wxPoint&
//		style				= long
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GAGoalDialog::GAGoalDialog(wxWindow *parent, const KinematicOutputs::OutputsComplete &_output,
							   const double &_desiredValue, const double &_expectedDeviation, const double &_importance,
							   const Kinematics::Inputs &_beforeInputs, const Kinematics::Inputs &_afterInputs,
							   wxWindowID id, const wxPoint &position, long style) :
							   wxDialog(parent, id, _T("Genetic Algorithm Goal"), position, wxDefaultSize, style)
{
	// Assign local members
	output = _output;
	desiredValue = _desiredValue;
	expectedDeviation = _expectedDeviation;
	importance = _importance;
	beforeInputs = _beforeInputs;
	afterInputs = _afterInputs;

	// Create the controls
	CreateControls();

	// Center the dialog on screen
	Center();
}

//==========================================================================
// Class:			GAGoalDialog
// Function:		~GAGoalDialog
//
// Description:		Destructor for the GAGoalDialog class.
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
GAGoalDialog::~GAGoalDialog()
{
}

//==========================================================================
// Class:			GAGoalDialog
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
BEGIN_EVENT_TABLE(GAGoalDialog, wxDialog)
	EVT_BUTTON(wxID_OK,		GAGoalDialog::OKClickEvent)
	EVT_BUTTON(wxID_CANCEL,	GAGoalDialog::CancelClickEvent)
	EVT_CHECKBOX(wxID_ANY,	GAGoalDialog::OnCheckEvent)
	EVT_COMBOBOX(wxID_ANY,	GAGoalDialog::OnOutputChangeEvent)
END_EVENT_TABLE()

//==========================================================================
// Class:			GAGoalDialog
// Function:		CreateControls
//
// Description:		Creates the controls that populate this dialog.  Also fills
//					controls with appropriate data (passed via constructor).
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
void GAGoalDialog::CreateControls(void)
{
	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create a sizer for the inputs
	int cellPadding(5);
	wxGridBagSizer *inputAreaSizer = new wxGridBagSizer(cellPadding, cellPadding);
	mainSizer->Add(inputAreaSizer, wxALL, 5);

	// Set sizer flags
	int textSizerFlags = wxALIGN_CENTER_VERTICAL;
	int inputSizerFlags = wxEXPAND | wxALIGN_CENTER_VERTICAL;

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
	wxArrayString list;
	for (i = 0; i < KinematicOutputs::NumberOfOutputScalars; i++)
		list.Add(KinematicOutputs::GetOutputName((KinematicOutputs::OutputsComplete)i));
	wxStaticText *outputLabel = new wxStaticText(this, wxID_STATIC, _T("Output Parameter"));
	outputCombo = new wxComboBox(this, wxID_ANY, KinematicOutputs::GetOutputName(output), wxDefaultPosition,
		wxDefaultSize, list, wxCB_READONLY);
	SetMinimumWidthFromContents(outputCombo, additionalWidth);
	inputAreaSizer->Add(outputLabel, wxGBPosition(row, 1), wxGBSpan(), textSizerFlags);
	inputAreaSizer->Add(outputCombo, wxGBPosition(row, 2), wxGBSpan(1, 3), inputSizerFlags);
	row++;

#ifdef __WXGTK__
	// Under GTK, the combo box selections are set to -1 until the user changes the selection, even if
	// we set a default value when we construct the controls.  This causes errors when getting the
	// selection and attempting to assign it to a gene.  To combat this effect, we set the selection
	// manually here.
	outputCombo->SetSelection((int)output);
#endif

	// Difference check box
	difference = new wxCheckBox(this, wxID_ANY, _T("Optimize difference between two states"));
	inputAreaSizer->Add(difference, wxGBPosition(row, 1), wxGBSpan(1, 4),
					 wxALIGN_CENTER_HORIZONTAL);
	difference->SetValue(beforeInputs != afterInputs);
	row++;

	// Before and after labels
	beforeLabel = new wxStaticText(this, wxID_STATIC, _T("State 1"));
	afterLabel = new wxStaticText(this, wxID_STATIC, _T("State 2"));
	inputAreaSizer->Add(beforeLabel, wxGBPosition(row, 2), wxGBSpan(),
					 wxALIGN_CENTER_HORIZONTAL | textSizerFlags);
	inputAreaSizer->Add(afterLabel, wxGBPosition(row, 3), wxGBSpan(),
					 wxALIGN_CENTER_HORIZONTAL | textSizerFlags);
	row++;

	// Pitch inputs
	wxStaticText *pitchLabel = new wxStaticText(this, wxID_STATIC, _T("Pitch"));
	beforePitchText = new wxTextCtrl(this, wxID_ANY,
		Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertAngle(beforeInputs.pitch)));
	afterPitchText = new wxTextCtrl(this, wxID_ANY,
		Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertAngle(afterInputs.pitch)));
	wxStaticText *pitchUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Convert::GetInstance().GetUnitType(Convert::UnitTypeAngle));
	inputAreaSizer->Add(pitchLabel, wxGBPosition(row, 1), wxGBSpan(), textSizerFlags);
	inputAreaSizer->Add(beforePitchText, wxGBPosition(row, 2), wxGBSpan(), inputSizerFlags);
	inputAreaSizer->Add(afterPitchText, wxGBPosition(row, 3), wxGBSpan(), inputSizerFlags);
	inputAreaSizer->Add(pitchUnitsLabel, wxGBPosition(row, 4), wxGBSpan(), textSizerFlags);
	row++;

	// Roll inputs
	wxStaticText *rollLabel = new wxStaticText(this, wxID_STATIC, _T("Roll"));
	beforeRollText = new wxTextCtrl(this, wxID_ANY,
		Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertAngle(beforeInputs.roll)));
	afterRollText = new wxTextCtrl(this, wxID_ANY,
		Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertAngle(afterInputs.roll)));
	wxStaticText *rollUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Convert::GetInstance().GetUnitType(Convert::UnitTypeAngle));
	inputAreaSizer->Add(rollLabel, wxGBPosition(row, 1), wxGBSpan(), textSizerFlags);
	inputAreaSizer->Add(beforeRollText, wxGBPosition(row, 2), wxGBSpan(), inputSizerFlags);
	inputAreaSizer->Add(afterRollText, wxGBPosition(row, 3), wxGBSpan(), inputSizerFlags);
	inputAreaSizer->Add(rollUnitsLabel, wxGBPosition(row, 4), wxGBSpan(), textSizerFlags);
	row++;

	// Heave inputs
	wxStaticText *heaveLabel = new wxStaticText(this, wxID_STATIC, _T("Heave"));
	beforeHeaveText = new wxTextCtrl(this, wxID_ANY,
		Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertDistance(beforeInputs.heave)));
	afterHeaveText = new wxTextCtrl(this, wxID_ANY,
		Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertDistance(afterInputs.heave)));
	wxStaticText *heaveUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Convert::GetInstance().GetUnitType(Convert::UnitTypeDistance));
	inputAreaSizer->Add(heaveLabel, wxGBPosition(row, 1), wxGBSpan(), textSizerFlags);
	inputAreaSizer->Add(beforeHeaveText, wxGBPosition(row, 2), wxGBSpan(), inputSizerFlags);
	inputAreaSizer->Add(afterHeaveText, wxGBPosition(row, 3), wxGBSpan(), inputSizerFlags);
	inputAreaSizer->Add(heaveUnitsLabel, wxGBPosition(row, 4), wxGBSpan(), textSizerFlags);
	row++;

	// Steer inputs
	wxStaticText *steerLabel = new wxStaticText(this, wxID_STATIC, _T("Rack Travel"));
	beforeSteerText = new wxTextCtrl(this, wxID_ANY,
		Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertDistance(beforeInputs.rackTravel)));
	afterSteerText = new wxTextCtrl(this, wxID_ANY,
		Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertDistance(afterInputs.rackTravel)));
	wxStaticText *steerUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Convert::GetInstance().GetUnitType(Convert::UnitTypeDistance));
	inputAreaSizer->Add(steerLabel, wxGBPosition(row, 1), wxGBSpan(), textSizerFlags);
	inputAreaSizer->Add(beforeSteerText, wxGBPosition(row, 2), wxGBSpan(), inputSizerFlags);
	inputAreaSizer->Add(afterSteerText, wxGBPosition(row, 3), wxGBSpan(), inputSizerFlags);
	inputAreaSizer->Add(steerUnitsLabel, wxGBPosition(row, 4), wxGBSpan(), textSizerFlags);
	row++;

	// Desired value
	desiredValueLabel = new wxStaticText(this, wxID_STATIC, _T("Desired Value"));
	desiredValueText = new wxTextCtrl(this, wxID_ANY, Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertTo(desiredValue,
		KinematicOutputs::GetOutputUnitType(output))));
	desiredValueUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Convert::GetInstance().GetUnitType(KinematicOutputs::GetOutputUnitType(output)));
	inputAreaSizer->Add(desiredValueLabel, wxGBPosition(row, 1), wxGBSpan(), textSizerFlags);
	inputAreaSizer->Add(desiredValueText, wxGBPosition(row, 2), wxGBSpan(1, 2), inputSizerFlags);
	inputAreaSizer->Add(desiredValueUnitsLabel, wxGBPosition(row, 4), wxGBSpan(), textSizerFlags);
	row++;

	// Expected deviation
	wxStaticText *deviationLabel = new wxStaticText(this, wxID_STATIC, _T("Expected Deviation"));
	deviationText = new wxTextCtrl(this, wxID_ANY, Convert::GetInstance().FormatNumber(
		Convert::GetInstance().ConvertTo(expectedDeviation,
		KinematicOutputs::GetOutputUnitType(output))));
	deviationUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Convert::GetInstance().GetUnitType(KinematicOutputs::GetOutputUnitType(output)));
	inputAreaSizer->Add(deviationLabel, wxGBPosition(row, 1), wxGBSpan(), textSizerFlags);
	inputAreaSizer->Add(deviationText, wxGBPosition(row, 2), wxGBSpan(1, 2), inputSizerFlags);
	inputAreaSizer->Add(deviationUnitsLabel, wxGBPosition(row, 4), wxGBSpan(), textSizerFlags);
	row++;

	// Importance
	wxStaticText *importanceLabel = new wxStaticText(this, wxID_STATIC, _T("Relative Importance"));
	importanceText = new wxTextCtrl(this, wxID_ANY, Convert::GetInstance().FormatNumber(importance));
	inputAreaSizer->Add(importanceLabel, wxGBPosition(row, 1), wxGBSpan(), textSizerFlags);
	inputAreaSizer->Add(importanceText, wxGBPosition(row, 2), wxGBSpan(1, 2), inputSizerFlags);
	row++;

	// The width of the second and third columns should be equal
	// We do this by creating a blank row with the widths we specify
	inputAreaSizer->Add(-1, pitchLabel->GetSize().GetHeight(), wxGBPosition(3, 1), wxGBSpan());
	int inputTextWidth = (outputCombo->GetMinWidth() - pitchUnitsLabel->GetSize().GetWidth()) / 2 - 2 * cellPadding;
	inputAreaSizer->Add(inputTextWidth, 15, wxGBPosition(row, 2), wxGBSpan());
	inputAreaSizer->Add(inputTextWidth, 15, wxGBPosition(row, 3), wxGBSpan());

	// Set the minimum widths of the text controls
	beforePitchText->SetMinSize(wxSize(inputTextWidth, -1));
	afterPitchText->SetMinSize(wxSize(inputTextWidth, -1));
	beforeRollText->SetMinSize(wxSize(inputTextWidth, -1));
	afterRollText->SetMinSize(wxSize(inputTextWidth, -1));
	beforeHeaveText->SetMinSize(wxSize(inputTextWidth, -1));
	afterHeaveText->SetMinSize(wxSize(inputTextWidth, -1));
	beforeSteerText->SetMinSize(wxSize(inputTextWidth, -1));
	afterSteerText->SetMinSize(wxSize(inputTextWidth, -1));

	// Create another sizer for the buttons at the bottom and add the buttons
	wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *okButton = new wxButton(this, wxID_OK, _T("OK"),
		wxDefaultPosition, wxDefaultSize, 0);
	wxButton *cancelButton = new wxButton(this, wxID_CANCEL, _T("Cancel"),
		wxDefaultPosition, wxDefaultSize, 0);
	buttonsSizer->Add(okButton, 0, wxALL, 5);
	buttonsSizer->Add(cancelButton, 0, wxALL, 5);
	mainSizer->Add(buttonsSizer, 0, wxALIGN_CENTER_HORIZONTAL);

	// Make the OK button default
	okButton->SetDefault();

	// Tell the dialog to auto-adjust it's size
	topSizer->SetSizeHints(this);

	// Assign the top level sizer to the dialog
	SetSizer(topSizer);

	// Set the checkbox as appropriate for this goal.  This will handle showing/hiding the
	// controls and labeling the desired value as appropriate.
	FormatDialogDifference();
}

//==========================================================================
// Class:			GAGoalDialog
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
void GAGoalDialog::OKClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Update the class members with the data currently displayed in the dialog controls
	if (!desiredValueText->GetValue().ToDouble(&desiredValue) ||
		!deviationText->GetValue().ToDouble(&expectedDeviation) ||
		!importanceText->GetValue().ToDouble(&importance) ||
		!beforeHeaveText->GetValue().ToDouble(&beforeInputs.heave) ||
		!beforePitchText->GetValue().ToDouble(&beforeInputs.pitch) ||
		!beforeRollText->GetValue().ToDouble(&beforeInputs.roll) ||
		!beforeSteerText->GetValue().ToDouble(&beforeInputs.rackTravel))
	{
		wxMessageBox(_T("ERROR:  All values must be numeric!"), _T("Error Validating Data"),
			wxOK | wxICON_ERROR, this);
		return;
	}

	// Check the after inputs separately, because we may not actually read them
	if (difference->IsChecked())
	{
		if (!afterHeaveText->GetValue().ToDouble(&afterInputs.heave) ||
			!afterPitchText->GetValue().ToDouble(&afterInputs.pitch) ||
			!afterRollText->GetValue().ToDouble(&afterInputs.roll) ||
			!afterSteerText->GetValue().ToDouble(&afterInputs.rackTravel))
		{
			wxMessageBox(_T("ERROR:  All values must be numeric!"), _T("Error Validating Data"),
				wxOK | wxICON_ERROR, this);
			return;
		}
	}

	// Expected deviation cannot be zero
	if (expectedDeviation == 0.0)
	{
		wxMessageBox(_T("ERROR:  Expected deviation cannot be zero!"), _T("Error Validating Data"),
			wxOK | wxICON_ERROR, this);
		return;
	}

	output = (KinematicOutputs::OutputsComplete)outputCombo->GetCurrentSelection();

	// Convert the input values
	desiredValue = Convert::GetInstance().Read(desiredValue, KinematicOutputs::GetOutputUnitType(output));
	expectedDeviation = Convert::GetInstance().Read(expectedDeviation, KinematicOutputs::GetOutputUnitType(output));
	beforeInputs.pitch = Convert::GetInstance().ReadAngle(beforeInputs.pitch);
	beforeInputs.roll = Convert::GetInstance().ReadAngle(beforeInputs.roll);
	beforeInputs.heave = Convert::GetInstance().ReadDistance(beforeInputs.heave);
	beforeInputs.rackTravel = Convert::GetInstance().ReadDistance(beforeInputs.rackTravel);

	if (difference->IsChecked())
	{
		afterInputs.pitch = Convert::GetInstance().ReadAngle(afterInputs.pitch);
		afterInputs.roll = Convert::GetInstance().ReadAngle(afterInputs.roll);
		afterInputs.heave = Convert::GetInstance().ReadDistance(afterInputs.heave);
		afterInputs.rackTravel = Convert::GetInstance().ReadDistance(afterInputs.rackTravel);
	}
	else
		afterInputs = beforeInputs;

	// The way we handle this changes depending on how this form was displayed
	if (IsModal())
		EndModal(wxOK);
	else
	{
		SetReturnCode(wxOK);
		Show(false);
	}
}

//==========================================================================
// Class:			GAGoalDialog
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
void GAGoalDialog::CancelClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// The way we handle this changes depending on how this form was displayed
	if (IsModal())
		EndModal(wxID_CANCEL);
	else
	{
		SetReturnCode(wxID_CANCEL);
		Show(false);
	}
}

//==========================================================================
// Class:			GAGoalDialog
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
void GAGoalDialog::OnCheckEvent(wxCommandEvent& WXUNUSED(event))
{
	// Update the dialog depending on the value of the checkbox
	FormatDialogDifference();
}

//==========================================================================
// Class:			GAGoalDialog
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
void GAGoalDialog::OnOutputChangeEvent(wxCommandEvent &event)
{
	// Update the units labels for the desired value and expected deviation
	desiredValueUnitsLabel->SetLabel(Convert::GetInstance().GetUnitType(
		KinematicOutputs::GetOutputUnitType((KinematicOutputs::OutputsComplete)event.GetSelection())));
	deviationUnitsLabel->SetLabel(Convert::GetInstance().GetUnitType(
		KinematicOutputs::GetOutputUnitType((KinematicOutputs::OutputsComplete)event.GetSelection())));
}

//==========================================================================
// Class:			GAGoalDialog
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
void GAGoalDialog::FormatDialogDifference(void)
{
	// Hide or display the second set of KINEMATICS::INPUTS controls
	if (difference->IsChecked())
	{
		// Show the controls
		afterPitchText->Show();
		afterRollText->Show();
		afterHeaveText->Show();
		afterSteerText->Show();
		beforeLabel->Show();
		afterLabel->Show();

		// The desired value is actually a desired change
		desiredValueLabel->SetLabel(_T("Desired Change"));
	}
	else
	{
		// Hide the controls
		afterPitchText->Hide();
		afterRollText->Hide();
		afterHeaveText->Hide();
		afterSteerText->Hide();
		beforeLabel->Hide();
		afterLabel->Hide();

		// The desired value really is a single value
		desiredValueLabel->SetLabel(_T("Desired Value"));
	}

	// Update the appearance of this dialog
	this->GetSizer()->SetSizeHints(this);
	this->GetSizer()->Layout();
}