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
// Class:			GA_GENE_DIALOG
// Function:		GA_GENE_DIALOG
//
// Description:		Constructor for the GA_GENE_DIALOG class.
//
// Input Arguments:
//		Parent			= wxWindow&, reference to the main application window
//		_Converter		= const Convert&, reference to conversion utility object
//		_Hardpoint		= const CORNER::HARDPOINTS&
//		_TiedTo			= const CORNER::HARDPOINTS&
//		_AxisDirection	= const Vector::Axis&
//		_CornerLocation	= const CORNER::LOCATION&
//		_Minimum		= const double&
//		_Maximum		= const double&
//		_NumberOfValues	= const unsigned int&
//		Id				= wxWindowID
//		Position		= const wxPoint&
//		Style			= long
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GA_GENE_DIALOG::GA_GENE_DIALOG(wxWindow *Parent, const Convert &_Converter, const CORNER::HARDPOINTS &_Hardpoint,
							   const CORNER::HARDPOINTS &_TiedTo, const Vector::Axis &_AxisDirection,
							   const CORNER::LOCATION &_CornerLocation, const double &_Minimum, const double &_Maximum,
							   const unsigned int &_NumberOfValues, wxWindowID Id, const wxPoint &Position, long Style) :
							   wxDialog(Parent, Id, _T("Genetic Algorithm Gene"), Position, wxDefaultSize, Style),
							   Converter(_Converter)
{
	// Assign local members
	Hardpoint = _Hardpoint;
	TiedTo = _TiedTo;
	AxisDirection = _AxisDirection;
	CornerLocation = _CornerLocation;
	Minimum = _Minimum;
	Maximum = _Maximum;
	NumberOfValues = _NumberOfValues;

	// Initialize the resolution pointer
	Resolution = NULL;

	// Create the controls
	CreateControls();

	// Center the dialog on screen
	Center();
}

//==========================================================================
// Class:			GA_GENE_DIALOG
// Function:		~GA_GENE_DIALOG
//
// Description:		Destructor for the GA_GENE_DIALOG class.
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
GA_GENE_DIALOG::~GA_GENE_DIALOG()
{
}

//==========================================================================
// Class:			GA_GENE_DIALOG
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
BEGIN_EVENT_TABLE(GA_GENE_DIALOG, wxDialog)
	EVT_BUTTON(wxID_OK,			GA_GENE_DIALOG::OKClickEvent)
	EVT_BUTTON(wxID_CANCEL,		GA_GENE_DIALOG::CancelClickEvent)
	EVT_TEXT(wxID_ANY,			GA_GENE_DIALOG::TextChangeEvent)
END_EVENT_TABLE()

//==========================================================================
// Class:			GA_GENE_DIALOG
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
void GA_GENE_DIALOG::CreateControls(void)
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create a sizer to contain the inputs
	wxFlexGridSizer *inputAreaSizer = new wxFlexGridSizer(3, 5, 5);
	inputAreaSizer->SetFlexibleDirection(wxHORIZONTAL);
	MainSizer->Add(inputAreaSizer, 0, wxALL, 5);

	// Specify the sizer flags for all controls in the grid here
	int sizerFlags = wxEXPAND | wxALIGN_CENTER_VERTICAL;

	// When setting the control width, we need to account for the width of the
	// "expand" button, etc., so we specify that here
#ifdef __WXGTK__
	unsigned int additionalWidth = 40;
#else
	unsigned int additionalWidth = 30;
#endif

	// Set up the column widths
	int i;
	wxArrayString List;

	// Add the static text and text controls to these sizers
	// Hardpoints
	for (i = 0; i < CORNER::NumberOfHardpoints; i++)
		List.Add(CORNER::GetHardpointName((CORNER::HARDPOINTS)i));
	wxStaticText *HardpointLabel = new wxStaticText(this, wxID_STATIC, _T("Hardpoint"));
	HardpointCombo = new wxComboBox(this, wxID_ANY, CORNER::GetHardpointName(Hardpoint), wxDefaultPosition,
		wxDefaultSize, List, wxCB_READONLY);
	SetMinimumWidthFromContents(HardpointCombo, additionalWidth);
	inputAreaSizer->Add(HardpointLabel, 0, sizerFlags);
	inputAreaSizer->Add(HardpointCombo, 0, sizerFlags);
	inputAreaSizer->AddSpacer(-1);

	// Tied To
	// Re-use the same list for this combo box
	List.Insert(_T("None"), 0);
	wxStaticText *TiedToLabel = new wxStaticText(this, wxID_STATIC, _T("Alternate With"));
	TiedToCombo = new wxComboBox(this, wxID_ANY, CORNER::GetHardpointName(TiedTo), wxDefaultPosition,
		wxDefaultSize, List, wxCB_READONLY);
	SetMinimumWidthFromContents(TiedToCombo, additionalWidth);
	inputAreaSizer->Add(TiedToLabel, 0, sizerFlags);
	inputAreaSizer->Add(TiedToCombo, 0, sizerFlags);
	inputAreaSizer->AddSpacer(-1);

	// Axis Direction
	List.Clear();
	for (i = 0; i < 3; i++)
		List.Add(Vector::GetAxisName((Vector::Axis)i));
	wxStaticText *AxisDirectionLabel = new wxStaticText(this, wxID_STATIC, _T("Axis Direction"));
	AxisDirectionCombo = new wxComboBox(this, wxID_ANY, Vector::GetAxisName(AxisDirection), wxDefaultPosition,
		wxDefaultSize, List, wxCB_READONLY);
	SetMinimumWidthFromContents(AxisDirectionCombo, additionalWidth);
	inputAreaSizer->Add(AxisDirectionLabel, 0, sizerFlags);
	inputAreaSizer->Add(AxisDirectionCombo, 0, sizerFlags);
	inputAreaSizer->AddSpacer(-1);

	// Corner Location
	List.Clear();
	for (i = 0; i < CORNER::NumberOfLocations; i++)
		List.Add(CORNER::GetLocationName((CORNER::LOCATION)i));
	wxStaticText *CornerLocationLabel = new wxStaticText(this, wxID_STATIC, _T("Corner Location"));
	CornerLocationCombo = new wxComboBox(this, wxID_ANY, CORNER::GetLocationName(CornerLocation), wxDefaultPosition,
		wxDefaultSize, List, wxCB_READONLY);
	SetMinimumWidthFromContents(CornerLocationCombo, additionalWidth);
	inputAreaSizer->Add(CornerLocationLabel, 0, sizerFlags);
	inputAreaSizer->Add(CornerLocationCombo, 0, sizerFlags);
	inputAreaSizer->AddSpacer(-1);

#ifdef __WXGTK__
	// Under GTK, the combo box selections are set to -1 until the user changes the selection, even if
	// we set a default value when we construct the controls.  This causes errors when getting the
	// selection and attempting to assign it to a gene.  To combat this effect, we set the selection
	// manually here.
	HardpointCombo->SetSelection((int)Hardpoint);
	TiedToCombo->SetSelection((int)TiedTo + 1);
	AxisDirectionCombo->SetSelection((int)AxisDirection);
	CornerLocationCombo->SetSelection((int)CornerLocation);
#endif

	// If the main hardpoint and the alternate are the same, select index zero on the alternate box
	if (HardpointCombo->GetSelection() == TiedToCombo->GetSelection() - 1)
		TiedToCombo->SetSelection(0);

	// Minimum
	wxStaticText *MinimumLabel = new wxStaticText(this, wxID_STATIC, _T("Minimum"));
	MinimumText = new wxTextCtrl(this, wxID_ANY, Converter.FormatNumber(Converter.ConvertDistance(Minimum)));
	wxStaticText *MinimumUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeDistance), wxDefaultPosition, wxDefaultSize, 0);
	inputAreaSizer->Add(MinimumLabel, 0, sizerFlags);
	inputAreaSizer->Add(MinimumText, 0, sizerFlags);
	inputAreaSizer->Add(MinimumUnitsLabel, 0, sizerFlags);

	// Maximum
	wxStaticText *MaximumLabel = new wxStaticText(this, wxID_STATIC, _T("Maximum"));
	MaximumText = new wxTextCtrl(this, wxID_ANY, Converter.FormatNumber(Converter.ConvertDistance(Maximum)));
	wxStaticText *MaximumUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeDistance), wxDefaultPosition, wxDefaultSize, 0);
	inputAreaSizer->Add(MaximumLabel, 0, sizerFlags);
	inputAreaSizer->Add(MaximumText, 0, sizerFlags);
	inputAreaSizer->Add(MaximumUnitsLabel, 0, sizerFlags);

	// Number of values
	wxStaticText *NumberOfValuesLabel = new wxStaticText(this, wxID_STATIC, _T("Number of Values"));
	NumberOfValuesText = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
	inputAreaSizer->Add(NumberOfValuesLabel, 0, sizerFlags);
	inputAreaSizer->Add(NumberOfValuesText, 0, sizerFlags);
	inputAreaSizer->AddSpacer(-1);

	// Resolution
	wxStaticText *ResolutionLabel = new wxStaticText(this, wxID_STATIC, _T("Resolution"));
	Resolution = new wxStaticText(this, wxID_ANY, wxEmptyString);
	wxStaticText *ResolutionUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UnitTypeDistance), wxDefaultPosition, wxDefaultSize, 0);
	inputAreaSizer->Add(ResolutionLabel, 0, sizerFlags);
	inputAreaSizer->Add(Resolution, 0, sizerFlags);
	inputAreaSizer->Add(ResolutionUnitsLabel, 0, sizerFlags);

	// This is set as a separate event to cause the resolution to update
	wxString Temp;
	Temp.Printf("%lu", NumberOfValues);
	NumberOfValuesText->SetValue(Temp);

	// Add a spacer between the text controls and the buttons
	MainSizer->AddSpacer(15);

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

	return;
}

//==========================================================================
// Class:			GA_GENE_DIALOG
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
void GA_GENE_DIALOG::OKClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Update the class members with the data currently displayed in the dialog controls
	if (!MinimumText->GetValue().ToDouble(&Minimum) ||
		!MaximumText->GetValue().ToDouble(&Maximum) ||
		!NumberOfValuesText->GetValue().ToULong(&NumberOfValues))
	{
		wxMessageBox(_T("ERROR:  All values must be numeric!"), _T("Error Reading Data"),
			wxOK | wxICON_ERROR, this);
		return;
	}

	// Make sure the number of values is greater than 1
	if (NumberOfValues <= 1)
	{
		wxMessageBox(_T("ERROR:  Number of values must be greater than one!"), _T("Error Reading Data"),
			wxOK | wxICON_ERROR, this);
		return;
	}

	// Read the remaining items
	Hardpoint = (CORNER::HARDPOINTS)HardpointCombo->GetCurrentSelection();
	if (TiedToCombo->GetCurrentSelection() == 0)
		TiedTo = (CORNER::HARDPOINTS)HardpointCombo->GetCurrentSelection();
	else
		TiedTo = (CORNER::HARDPOINTS)(TiedToCombo->GetCurrentSelection() - 1);
	AxisDirection = (Vector::Axis)AxisDirectionCombo->GetCurrentSelection();
	CornerLocation = (CORNER::LOCATION)CornerLocationCombo->GetCurrentSelection();

	// Convert the input values
	Minimum = Converter.ReadDistance(Minimum);
	Maximum = Converter.ReadDistance(Maximum);

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
// Class:			GA_GENE_DIALOG
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
void GA_GENE_DIALOG::CancelClickEvent(wxCommandEvent& WXUNUSED(event))
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
// Class:			GA_GENE_DIALOG
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
void GA_GENE_DIALOG::TextChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Make sure the resolution label exists
	if (!Resolution)
		return;

	// Get the input values
	double Min(0.0), Max(0.0);
	unsigned long ValueCount(0);
	if (!MinimumText->GetValue().ToDouble(&Min) ||
		!MaximumText->GetValue().ToDouble(&Max) ||
		!NumberOfValuesText->GetValue().ToULong(&ValueCount))
		return;

	// Make sure the data is valid
	if (ValueCount <= 1)
		return;

	// Set the text
	Resolution->SetLabel(Converter.FormatNumber(
		Converter.ConvertDistance(fabs(Max - Min) / double(ValueCount - 1))));

	return;
}