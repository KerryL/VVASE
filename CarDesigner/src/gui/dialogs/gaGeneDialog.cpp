/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  gaGeneDialog.cpp
// Created:  7/30/2009
// Author:  K. Loux
// Description:  Dialog for editing gene properties.
// History:

// CarDesigner headers
#include "gui/dialogs/gaGeneDialog.h"
#include "vUtilities/wxRelatedUtilities.h"

//==========================================================================
// Class:			GAGeneDialog
// Function:		GAGeneDialog
//
// Description:		Constructor for the GAGeneDialog class.
//
// Input Arguments:
//		parent			= wxWindow&, reference to the main application window
//		_hardpoint		= const Corner::Hardpoints&
//		_tiedTo			= const Corner::Hardpoints&
//		_axisDirection	= const Vector::Axis&
//		_cornerLocation	= const Corner::Location&
//		_minimum		= const double&
//		_maximum		= const double&
//		_numberOfValues	= const unsigned int&
//		id				= wxWindowID
//		position		= const wxPoint&
//		style			= long
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GAGeneDialog::GAGeneDialog(wxWindow *parent, const Corner::Hardpoints &_hardpoint,
							   const Corner::Hardpoints &_tiedTo, const Vector::Axis &_axisDirection,
							   const Corner::Location &_cornerLocation, const double &_minimum, const double &_maximum,
							   const unsigned int &_numberOfValues, wxWindowID id, const wxPoint &position, long style) :
							   wxDialog(parent, id, _T("Genetic Algorithm Gene"), position, wxDefaultSize, style)
{
	// Assign local members
	hardpoint = _hardpoint;
	tiedTo = _tiedTo;
	axisDirection = _axisDirection;
	cornerLocation = _cornerLocation;
	minimum = _minimum;
	maximum = _maximum;
	numberOfValues = _numberOfValues;

	// Initialize the resolution pointer
	resolution = NULL;

	// Create the controls
	CreateControls();

	// Center the dialog on screen
	Center();
}

//==========================================================================
// Class:			GAGeneDialog
// Function:		~GAGeneDialog
//
// Description:		Destructor for the GAGeneDialog class.
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
GAGeneDialog::~GAGeneDialog()
{
}

//==========================================================================
// Class:			GAGeneDialog
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
BEGIN_EVENT_TABLE(GAGeneDialog, wxDialog)
	EVT_BUTTON(wxID_OK,			GAGeneDialog::OKClickEvent)
	EVT_BUTTON(wxID_CANCEL,		GAGeneDialog::CancelClickEvent)
	EVT_TEXT(wxID_ANY,			GAGeneDialog::TextChangeEvent)
END_EVENT_TABLE()

//==========================================================================
// Class:			GAGeneDialog
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
void GAGeneDialog::CreateControls(void)
{
	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create a sizer to contain the inputs
	wxFlexGridSizer *inputAreaSizer = new wxFlexGridSizer(3, 5, 5);
	inputAreaSizer->SetFlexibleDirection(wxHORIZONTAL);
	mainSizer->Add(inputAreaSizer, 0, wxALL, 5);

	// Specify the sizer flags for all controls in the grid here
	int textSizerFlags = wxALIGN_CENTER_VERTICAL;
	int comboSizerFlags = wxALIGN_CENTER_VERTICAL | wxEXPAND;

	// When setting the control width, we need to account for the width of the
	// "expand" button, etc., so we specify that here
#ifdef __WXGTK__
	unsigned int additionalWidth = 40;
#else
	unsigned int additionalWidth = 30;
#endif

	// Set up the column widths
	int i;
	wxArrayString list;

	// Add the static text and text controls to these sizers
	// Hardpoints
	for (i = 0; i < Corner::NumberOfHardpoints; i++)
		list.Add(Corner::GetHardpointName((Corner::Hardpoints)i));
	wxStaticText *hardpointLabel = new wxStaticText(this, wxID_STATIC, _T("Hardpoint"));
	hardpointCombo = new wxComboBox(this, wxID_ANY, Corner::GetHardpointName(hardpoint), wxDefaultPosition,
		wxDefaultSize, list, wxCB_READONLY);
	SetMinimumWidthFromContents(hardpointCombo, additionalWidth);
	inputAreaSizer->Add(hardpointLabel, 0, textSizerFlags);
	inputAreaSizer->Add(hardpointCombo, 0, comboSizerFlags);
	inputAreaSizer->AddSpacer(-1);

	// Tied To
	// Re-use the same list for this combo box
	list.Insert(_T("None"), 0);
	wxStaticText *tiedToLabel = new wxStaticText(this, wxID_STATIC, _T("Alternate With"));
	tiedToCombo = new wxComboBox(this, wxID_ANY, Corner::GetHardpointName(tiedTo), wxDefaultPosition,
		wxDefaultSize, list, wxCB_READONLY);
	SetMinimumWidthFromContents(tiedToCombo, additionalWidth);
	inputAreaSizer->Add(tiedToLabel, 0, textSizerFlags);
	inputAreaSizer->Add(tiedToCombo, 0, comboSizerFlags);
	inputAreaSizer->AddSpacer(-1);

	// Axis Direction
	list.Clear();
	for (i = 0; i < 3; i++)
		list.Add(Vector::GetAxisName((Vector::Axis)i));
	wxStaticText *axisDirectionLabel = new wxStaticText(this, wxID_STATIC, _T("Axis Direction"));
	axisDirectionCombo = new wxComboBox(this, wxID_ANY, Vector::GetAxisName(axisDirection), wxDefaultPosition,
		wxDefaultSize, list, wxCB_READONLY);
	SetMinimumWidthFromContents(axisDirectionCombo, additionalWidth);
	inputAreaSizer->Add(axisDirectionLabel, 0, textSizerFlags);
	inputAreaSizer->Add(axisDirectionCombo, 0, comboSizerFlags);
	inputAreaSizer->AddSpacer(-1);

	// Corner Location
	list.Clear();
	for (i = 0; i < Corner::NumberOfLocations; i++)
		list.Add(Corner::GetLocationName((Corner::Location)i));
	wxStaticText *cornerLocationLabel = new wxStaticText(this, wxID_STATIC, _T("Corner Location"));
	cornerLocationCombo = new wxComboBox(this, wxID_ANY, Corner::GetLocationName(cornerLocation), wxDefaultPosition,
		wxDefaultSize, list, wxCB_READONLY);
	SetMinimumWidthFromContents(cornerLocationCombo, additionalWidth);
	inputAreaSizer->Add(cornerLocationLabel, 0, textSizerFlags);
	inputAreaSizer->Add(cornerLocationCombo, 0, comboSizerFlags);
	inputAreaSizer->AddSpacer(-1);

#ifdef __WXGTK__
	// Under GTK, the combo box selections are set to -1 until the user changes the selection, even if
	// we set a default value when we construct the controls.  This causes errors when getting the
	// selection and attempting to assign it to a gene.  To combat this effect, we set the selection
	// manually here.
	hardpointCombo->SetSelection((int)hardpoint);
	tiedToCombo->SetSelection((int)tiedTo + 1);
	axisDirectionCombo->SetSelection((int)axisDirection);
	cornerLocationCombo->SetSelection((int)cornerLocation);
#endif

	// If the main hardpoint and the alternate are the same, select index zero on the alternate box
	if (hardpointCombo->GetSelection() == tiedToCombo->GetSelection() - 1)
		tiedToCombo->SetSelection(0);

	// Minimum
	wxStaticText *minimumLabel = new wxStaticText(this, wxID_STATIC, _T("Minimum"));
	minimumText = new wxTextCtrl(this, wxID_ANY, converter.FormatNumber(converter.ConvertDistance(minimum)));
	wxStaticText *minimumUnitsLabel = new wxStaticText(this, wxID_STATIC,
		converter.GetUnitType(Convert::UnitTypeDistance), wxDefaultPosition, wxDefaultSize, 0);
	inputAreaSizer->Add(minimumLabel, 0, textSizerFlags);
	inputAreaSizer->Add(minimumText, 0, comboSizerFlags);
	inputAreaSizer->Add(minimumUnitsLabel, 0, textSizerFlags);

	// Maximum
	wxStaticText *maximumLabel = new wxStaticText(this, wxID_STATIC, _T("Maximum"));
	maximumText = new wxTextCtrl(this, wxID_ANY, converter.FormatNumber(converter.ConvertDistance(maximum)));
	wxStaticText *maximumUnitsLabel = new wxStaticText(this, wxID_STATIC,
		converter.GetUnitType(Convert::UnitTypeDistance), wxDefaultPosition, wxDefaultSize, 0);
	inputAreaSizer->Add(maximumLabel, 0, textSizerFlags);
	inputAreaSizer->Add(maximumText, 0, comboSizerFlags);
	inputAreaSizer->Add(maximumUnitsLabel, 0, textSizerFlags);

	// Number of values
	wxStaticText *numberOfValuesLabel = new wxStaticText(this, wxID_STATIC, _T("Number of Values"));
	numberOfValuesText = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
	inputAreaSizer->Add(numberOfValuesLabel, 0, textSizerFlags);
	inputAreaSizer->Add(numberOfValuesText, 0, comboSizerFlags);
	inputAreaSizer->AddSpacer(-1);

	// Resolution
	wxStaticText *resolutionLabel = new wxStaticText(this, wxID_STATIC, _T("Resolution"));
	resolution = new wxStaticText(this, wxID_ANY, wxEmptyString);
	wxStaticText *resolutionUnitsLabel = new wxStaticText(this, wxID_STATIC,
		converter.GetUnitType(Convert::UnitTypeDistance), wxDefaultPosition, wxDefaultSize, 0);
	inputAreaSizer->Add(resolutionLabel, 0, textSizerFlags);
	inputAreaSizer->Add(resolution, 0, textSizerFlags);
	inputAreaSizer->Add(resolutionUnitsLabel, 0, textSizerFlags);

	// This is set as a separate event to cause the resolution to update
	wxString temp;
	temp.Printf("%lu", numberOfValues);
	numberOfValuesText->SetValue(temp);

	// Add a spacer between the text controls and the buttons
	mainSizer->AddSpacer(15);

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
}

//==========================================================================
// Class:			GAGeneDialog
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
void GAGeneDialog::OKClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Update the class members with the data currently displayed in the dialog controls
	if (!minimumText->GetValue().ToDouble(&minimum) ||
		!maximumText->GetValue().ToDouble(&maximum) ||
		!numberOfValuesText->GetValue().ToULong(&numberOfValues))
	{
		wxMessageBox(_T("ERROR:  All values must be numeric!"), _T("Error Reading Data"),
			wxOK | wxICON_ERROR, this);
		return;
	}

	// Make sure the number of values is greater than 1
	if (numberOfValues <= 1)
	{
		wxMessageBox(_T("ERROR:  Number of values must be greater than one!"), _T("Error Reading Data"),
			wxOK | wxICON_ERROR, this);
		return;
	}

	// Read the remaining items
	hardpoint = (Corner::Hardpoints)hardpointCombo->GetCurrentSelection();
	if (tiedToCombo->GetCurrentSelection() == 0)
		tiedTo = (Corner::Hardpoints)hardpointCombo->GetCurrentSelection();
	else
		tiedTo = (Corner::Hardpoints)(tiedToCombo->GetCurrentSelection() - 1);
	axisDirection = (Vector::Axis)axisDirectionCombo->GetCurrentSelection();
	cornerLocation = (Corner::Location)cornerLocationCombo->GetCurrentSelection();

	// Convert the input values
	minimum = converter.ReadDistance(minimum);
	maximum = converter.ReadDistance(maximum);

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
// Class:			GAGeneDialog
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
void GAGeneDialog::CancelClickEvent(wxCommandEvent& WXUNUSED(event))
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
// Class:			GAGeneDialog
// Function:		NumberOfValuesChangeEvent
//
// Description:		Updates the resolution display as the number of values changes.
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
void GAGeneDialog::TextChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the resolution label exists
	if (!resolution)
		return;

	// Get the input values
	double Min(0.0), Max(0.0);
	unsigned long ValueCount(0);
	if (!minimumText->GetValue().ToDouble(&Min) ||
		!maximumText->GetValue().ToDouble(&Max) ||
		!numberOfValuesText->GetValue().ToULong(&ValueCount))
		return;

	// Make sure the data is valid
	if (ValueCount <= 1)
		return;

	// Set the text
	resolution->SetLabel(converter.FormatNumber(
		converter.ConvertDistance(fabs(Max - Min) / double(ValueCount - 1))));
}