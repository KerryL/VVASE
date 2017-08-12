/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  gaGeneDialog.cpp
// Date:  7/30/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Dialog for editing gene properties.

// Local headers
#include "gui/dialogs/gaGeneDialog.h"
#include "VVASE/gui/utilities/wxRelatedUtilities.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/gui/utilities/dataValidator.h"
#include "VVASE/core/car/subsystems/suspension.h"

namespace VVASE
{

//==========================================================================
// Class:			GAGeneDialog
// Function:		GAGeneDialog
//
// Description:		Constructor for the GAGeneDialog class.
//
// Input Arguments:
//		parent				= wxWindow&, reference to the main application window
//		currentSuspension	= const Suspension*
//		hardpoint			= const Corner::Hardpoints&
//		tiedTo				= const Corner::Hardpoints&
//		axisDirection		= const Eigen::Vector3d::Axis&
//		cornerLocation		= const Corner::Location&
//		minimum				= const double&
//		maximum				= const double&
//		numberOfValues		= const unsigned int&
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
GAGeneDialog::GAGeneDialog(wxWindow *parent, const Suspension* currentSuspension,
	const Corner::Hardpoints &hardpoint, const Corner::Hardpoints &tiedTo,
	const Eigen::Vector3d::Axis &axisDirection, const Corner::Location &cornerLocation,
	const double &minimum, const double &maximum, const unsigned int &numberOfValues,
	wxWindowID id, const wxPoint &position, long style)
	: wxDialog(parent, id, _T("Genetic Algorithm Gene"), position, wxDefaultSize, style),
	currentSuspension(currentSuspension)
{
	this->hardpoint = hardpoint;
	this->tiedTo = tiedTo;
	this->axisDirection = axisDirection;
	this->cornerLocation = cornerLocation;
	this->minimum = minimum;
	this->maximum = maximum;
	this->numberOfValues = numberOfValues;

	resolution = NULL;

	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
	CreateControls();
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
	EVT_COMBOBOX(idHardpoint,	GAGeneDialog::HardpointComboBoxChangeEvent)
	EVT_COMBOBOX(idLocation,	GAGeneDialog::HardpointComboBoxChangeEvent)
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
void GAGeneDialog::CreateControls()
{
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
	hardpointCombo = new wxComboBox(this, idHardpoint, Corner::GetHardpointName(hardpoint), wxDefaultPosition,
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
		list.Add(Math::GetAxisName((Eigen::Vector3d::Axis)i));
	wxStaticText *axisDirectionLabel = new wxStaticText(this, wxID_STATIC, _T("Axis Direction"));
	axisDirectionCombo = new wxComboBox(this, wxID_ANY, Math::GetAxisName(axisDirection), wxDefaultPosition,
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
	cornerLocationCombo = new wxComboBox(this, idLocation, Corner::GetLocationName(cornerLocation), wxDefaultPosition,
		wxDefaultSize, list, wxCB_READONLY);
	SetMinimumWidthFromContents(cornerLocationCombo, additionalWidth);
	inputAreaSizer->Add(cornerLocationLabel, 0, textSizerFlags);
	inputAreaSizer->Add(cornerLocationCombo, 0, comboSizerFlags);
	inputAreaSizer->AddSpacer(-1);

	inputAreaSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Current Value")), 0, textSizerFlags);
	if (currentSuspension)
	{
		currentX = new wxStaticText(this, wxID_ANY, wxEmptyString);
		currentY = new wxStaticText(this, wxID_ANY, wxEmptyString);
		currentZ = new wxStaticText(this, wxID_ANY, wxEmptyString);

		wxSizer* valueSizer = new wxBoxSizer(wxHORIZONTAL);
		valueSizer->Add(currentX, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
		valueSizer->Add(currentY, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
		valueSizer->Add(currentZ, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);

		inputAreaSizer->Add(valueSizer, 0, comboSizerFlags);
		inputAreaSizer->Add(new wxStaticText(this, wxID_STATIC,
			UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance)), 0, textSizerFlags);
	}
	else
	{
		inputAreaSizer->Add(new wxStaticText(this, wxID_STATIC, _T("No car selected")), 0, comboSizerFlags);
		inputAreaSizer->AddSpacer(-1);
	}

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
	minimumText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, UnitValidator(minimum, UnitConverter::UnitTypeDistance));
	inputAreaSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Minimum")), 0, textSizerFlags);
	inputAreaSizer->Add(minimumText, 0, comboSizerFlags);
	inputAreaSizer->Add(new wxStaticText(this, wxID_STATIC,
		UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance),
		wxDefaultPosition, wxDefaultSize, 0), 0, textSizerFlags);

	// Maximum
	maximumText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, UnitValidator(maximum, UnitConverter::UnitTypeDistance));
	inputAreaSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Maximum")), 0, textSizerFlags);
	inputAreaSizer->Add(maximumText, 0, comboSizerFlags);
	inputAreaSizer->Add(new wxStaticText(this, wxID_STATIC,
		UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance),
		wxDefaultPosition, wxDefaultSize, 0), 0, textSizerFlags);

	// Number of values
	numberOfValuesText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, UnsignedValidator(numberOfValues, 2, 0, UnsignedValidator::ClassMinimumInclusive));
	inputAreaSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Number of Values")), 0, textSizerFlags);
	inputAreaSizer->Add(numberOfValuesText, 0, comboSizerFlags);
	inputAreaSizer->AddSpacer(-1);

	// Resolution
	resolution = new wxStaticText(this, wxID_ANY, wxEmptyString);
	inputAreaSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Resolution")), 0, textSizerFlags);
	inputAreaSizer->Add(resolution, 0, textSizerFlags);
	inputAreaSizer->Add(new wxStaticText(this, wxID_STATIC,
		UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance),
		wxDefaultPosition, wxDefaultSize, 0), 0, textSizerFlags);

	// Add a spacer between the text controls and the buttons
	mainSizer->AddSpacer(15);

	// Create another sizer for the buttons at the bottom
	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxGROW);

	// Tell the dialog to auto-adjust it's size
	topSizer->SetSizeHints(this);

	// Assign the top level sizer to the dialog
	SetSizer(topSizer);

	UpdateCurrentVector();
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
	if (!Validate() || !TransferDataFromWindow())
	{
		wxMessageBox(_T("ERROR:  All values must be numeric!\nNumber of values must be at least two."),
			_T("Error Reading Data"), wxOK | wxICON_ERROR, this);
		return;
	}

	// Read the remaining items
	hardpoint = (Corner::Hardpoints)hardpointCombo->GetCurrentSelection();
	if (tiedToCombo->GetCurrentSelection() == 0)
		tiedTo = (Corner::Hardpoints)hardpointCombo->GetCurrentSelection();
	else
		tiedTo = (Corner::Hardpoints)(tiedToCombo->GetCurrentSelection() - 1);
	axisDirection = (Eigen::Vector3d::Axis)axisDirectionCombo->GetCurrentSelection();
	cornerLocation = (Corner::Location)cornerLocationCombo->GetCurrentSelection();

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
	double min(0.0), max(0.0);
	unsigned long valueCount(0);
	if (!minimumText->GetValue().ToDouble(&min) ||
		!maximumText->GetValue().ToDouble(&max) ||
		!numberOfValuesText->GetValue().ToULong(&valueCount))
		return;

	// Make sure the data is valid
	if (valueCount <= 1)
		return;

	// Set the text
	resolution->SetLabel(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(fabs(max - min) / double(valueCount - 1))));
}

//==========================================================================
// Class:			GAGeneDialog
// Function:		HardpointComboBoxChangeEvent
//
// Description:		Updates the text displaying the current vector value
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
void GAGeneDialog::HardpointComboBoxChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	UpdateCurrentVector();
}

//==========================================================================
// Class:			GAGeneDialog
// Function:		UpdateCurrentVector
//
// Description:		Updates the text displaying the current vector value
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
void GAGeneDialog::UpdateCurrentVector()
{
	if (!currentX || !currentY || !currentZ)
		return;

	if (!currentSuspension)
		return;

	Corner::Location location(static_cast<Corner::Location>(
		cornerLocationCombo->GetCurrentSelection()));
	Corner::Hardpoints hardpoint(static_cast<Corner::Hardpoints>(
		hardpointCombo->GetCurrentSelection()));

	Eigen::Vector3d v;
	if (location == Corner::LocationLeftFront)
		v = currentSuspension->leftFront.hardpoints[hardpoint];
	else if (location == Corner::LocationRightFront)
		v = currentSuspension->rightFront.hardpoints[hardpoint];
	else if (location == Corner::LocationLeftRear)
		v = currentSuspension->leftRear.hardpoints[hardpoint];
	else
		v = currentSuspension->rightRear.hardpoints[hardpoint];

	currentX->SetLabel(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(v.x)));
	currentY->SetLabel(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(v.y)));
	currentZ->SetLabel(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertDistanceOutput(v.z)));
}

}// namespace VVASE
