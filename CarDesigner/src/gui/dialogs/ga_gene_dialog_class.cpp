/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  ga_gene_dialog_class.cpp
// Created:  7/30/2009
// Author:  K. Loux
// Description:  Dialog for editing gene properties.
// History:

// CarDesigner headers
#include "gui/dialogs/ga_gene_dialog_class.h"

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
//		_AxisDirection	= const Vector::AXIS&
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
							   const CORNER::HARDPOINTS &_TiedTo, const Vector::AXIS &_AxisDirection,
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

	// Create a sizer for each of the inputs
	wxBoxSizer *HardpointSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *TiedToSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *AxisDirectionSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *CornerLocationSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *MinimumSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *MaximumSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *NumberOfValuesSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *ResolutionSizer = new wxBoxSizer(wxHORIZONTAL);

	// Set up the column widths
	int LabelColumnWidth(90);
	int InputColumnWidth(140);
	int i;
	wxArrayString List;

	// Add the static text and text controls to these sizers
	// Hardpoints
	for (i = 0; i < CORNER::NumberOfHardpoints; i++)
		List.Add(CORNER::GetHardpointName((CORNER::HARDPOINTS)i));
	wxStaticText *HardpointLabel = new wxStaticText(this, wxID_STATIC, _T("Hardpoint"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	HardpointCombo = new wxComboBox(this, wxID_ANY, CORNER::GetHardpointName(Hardpoint), wxDefaultPosition,
		wxSize(InputColumnWidth, -1), List, wxCB_READONLY);
	HardpointSizer->Add(HardpointLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	HardpointSizer->Add(HardpointCombo, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Tied To
	// Re-use the same list for this combo box
	List.Insert(_T("None"), 0);
	wxStaticText *TiedToLabel = new wxStaticText(this, wxID_STATIC, _T("Alternate With"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	TiedToCombo = new wxComboBox(this, wxID_ANY, CORNER::GetHardpointName(TiedTo), wxDefaultPosition,
		wxSize(InputColumnWidth, -1), List, wxCB_READONLY);
	TiedToSizer->Add(TiedToLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	TiedToSizer->Add(TiedToCombo, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Axis Direction
	List.Clear();
	for (i = 0; i < 3; i++)
		List.Add(Vector::GetAxisName((Vector::Axis)i));
	wxStaticText *AxisDirectionLabel = new wxStaticText(this, wxID_STATIC, _T("Axis Direction"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	AxisDirectionCombo = new wxComboBox(this, wxID_ANY, Vector::GetAxisName(AxisDirection), wxDefaultPosition,
		wxSize(InputColumnWidth, -1), List, wxCB_READONLY);
	AxisDirectionSizer->Add(AxisDirectionLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	AxisDirectionSizer->Add(AxisDirectionCombo, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Corner Location
	List.Clear();
	for (i = 0; i < CORNER::NumberOfLocations; i++)
		List.Add(CORNER::GetLocationName((CORNER::LOCATION)i));
	wxStaticText *CornerLocationLabel = new wxStaticText(this, wxID_STATIC, _T("Corner Location"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	CornerLocationCombo = new wxComboBox(this, wxID_ANY, CORNER::GetLocationName(CornerLocation), wxDefaultPosition,
		wxSize(InputColumnWidth, -1), List, wxCB_READONLY);
	CornerLocationSizer->Add(CornerLocationLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	CornerLocationSizer->Add(CornerLocationCombo, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

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
	wxStaticText *MinimumLabel = new wxStaticText(this, wxID_STATIC, _T("Minimum"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	MinimumText = new wxTextCtrl(this, wxID_ANY, Converter.FormatNumber(Converter.ConvertDistance(Minimum)),
		wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	wxStaticText *MinimumUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE), wxDefaultPosition, wxDefaultSize, 0);
	MinimumSizer->Add(MinimumLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	MinimumSizer->Add(MinimumText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	MinimumSizer->Add(MinimumUnitsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Maximum
	wxStaticText *MaximumLabel = new wxStaticText(this, wxID_STATIC, _T("Maximum"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	MaximumText = new wxTextCtrl(this, wxID_ANY, Converter.FormatNumber(Converter.ConvertDistance(Maximum)),
		wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	wxStaticText *MaximumUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE), wxDefaultPosition, wxDefaultSize, 0);
	MaximumSizer->Add(MaximumLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	MaximumSizer->Add(MaximumText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	MaximumSizer->Add(MaximumUnitsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Number of values
	wxStaticText *NumberOfValuesLabel = new wxStaticText(this, wxID_STATIC, _T("Number of Values"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	NumberOfValuesText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	NumberOfValuesSizer->Add(NumberOfValuesLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	NumberOfValuesSizer->Add(NumberOfValuesText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Resolution
	wxStaticText *ResolutionLabel = new wxStaticText(this, wxID_STATIC, _T("Resolution"),
		wxDefaultPosition, wxSize(LabelColumnWidth, -1), 0);
	Resolution = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(InputColumnWidth, -1), 0);
	wxStaticText *ResolutionUnitsLabel = new wxStaticText(this, wxID_STATIC,
		Converter.GetUnitType(Convert::UNIT_TYPE_DISTANCE), wxDefaultPosition, wxDefaultSize, 0);
	ResolutionSizer->Add(ResolutionLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	ResolutionSizer->Add(Resolution, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	ResolutionSizer->Add(ResolutionUnitsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// This is set as a separate event to cause the resolution to update
	wxString Temp;
	Temp.Printf("%lu", NumberOfValues);
	NumberOfValuesText->SetValue(Temp);

	// Add the sizers to the MainSizer
	MainSizer->Add(HardpointSizer, 0, wxALIGN_TOP);
	MainSizer->Add(TiedToSizer, 0, wxALIGN_TOP);
	MainSizer->Add(AxisDirectionSizer, 0, wxALIGN_TOP);
	MainSizer->Add(CornerLocationSizer, 0, wxALIGN_TOP);
	MainSizer->Add(MinimumSizer, 0, wxALIGN_TOP);
	MainSizer->Add(MaximumSizer, 0, wxALIGN_TOP);
	MainSizer->Add(NumberOfValuesSizer, 0, wxALIGN_TOP);
	MainSizer->Add(ResolutionSizer, 0, wxALIGN_TOP);

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
	AxisDirection = (Vector::AXIS)AxisDirectionCombo->GetCurrentSelection();
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