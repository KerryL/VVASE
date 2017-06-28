/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  optionsDialog.cpp
// Date:  2/9/2009
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the definition for the OptionsDialog class.

// Standard C++ headers
#include <cfloat>

// wxWidgets headers
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/radiobox.h>
#include <wx/fontdlg.h>

// VVASE headers
#include "gui/dialogs/optionsDialog.h"
#include "gui/components/mainFrame.h"
#include "vUtilities/unitConverter.h"
#include "vUtilities/debugger.h"
#include "vUtilities/wxRelatedUtilities.h"
#include "vUtilities/dataValidator.h"

//==========================================================================
// Class:			OptionsDialog
// Function:		OptionsDialog
//
// Description:		Constructor for OptionsDialog class.
//
// Input Arguments:
//		mainFrame			= MainFrame& pointing to the application's main window
//		kinematicInputs	= Kinematics::Inputs& reference to the application's analysis options
//		id					= wxWindowId for this object
//		position			= const wxPoint& where this object will be drawn
//		style				= long defining the style for this dialog
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
OptionsDialog::OptionsDialog(MainFrame &mainFrame, Kinematics::Inputs &kinematicInputs,
	wxWindowID id, const wxPoint &position, long style) : wxDialog(&mainFrame, id,
	_T("Options"), position, wxDefaultSize, style),
	kinematicInputs(kinematicInputs), mainFrame(mainFrame)
{
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
	CreateControls();
	Center();
}

//==========================================================================
// Class:			OptionsDialog
// Function:		~OptionsDialog
//
// Description:		Destructor for OptionsDialog class.
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
OptionsDialog::~OptionsDialog()
{
}

//==========================================================================
// Class:			OptionsDialog
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
BEGIN_EVENT_TABLE(OptionsDialog, wxDialog)
	EVT_BUTTON(wxID_OK,					OptionsDialog::OKClickEvent)
	EVT_BUTTON(wxID_CANCEL,				OptionsDialog::CancelClickEvent)
	EVT_BUTTON(ChangeOutputFontButton,	OptionsDialog::ChangeOutputFontClickEvent)
	EVT_BUTTON(ChangePlotFontButton,	OptionsDialog::ChangePlotFontClickEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			OptionsDialog
// Function:		CreateControls
//
// Description:		Sets up this form's contents and size.
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
void OptionsDialog::CreateControls()
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the notebook
	notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
	mainSizer->Add(notebook);

	// Create the units page
	unitsPage = new wxPanel(notebook);
	notebook->AddPage(unitsPage, _T("Units"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *unitsTopSizer = new wxBoxSizer(wxVERTICAL);

	// Create the units page main sizer
	wxBoxSizer *unitsSizer = new wxBoxSizer(wxVERTICAL);
	unitsTopSizer->Add(unitsSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Add a horizontal spacer for the text across the top
	wxBoxSizer *unitsCaptionSizer = new wxBoxSizer(wxHORIZONTAL);
	unitsSizer->Add(unitsCaptionSizer, 0, wxALIGN_TOP);

	// Add the text to this spacer
	wxStaticText *prompt = new wxStaticText(unitsPage, wxID_STATIC,
		_T("Choose the units to use for each quantity"));
	unitsCaptionSizer->Add(-1, -1, 0, wxALL, 5);
	unitsCaptionSizer->Add(prompt, 0, wxALL, 5);

	// Create a sizer to contain the inputs
	wxFlexGridSizer *unitSelectionSizer = new wxFlexGridSizer(5, 5, 5);
	unitSelectionSizer->SetFlexibleDirection(wxHORIZONTAL);
	unitsSizer->Add(unitSelectionSizer, 0, wxALL, 5);

	// When setting the control width, we need to account for the width of the
	// "expand" button, etc., so we specify that here
#ifdef __WXGTK__
	unsigned int additionalWidth = 40;
#else
	unsigned int additionalWidth = 30;
#endif

	// Set some additional space between left and right sets of unit selections
	int middleSpaceWidth = 15;

	// The column flags
	int labelFlags = wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT;
	int unitFlags = wxEXPAND;

	// Add the static text and text controls to these sizers
	// For each type of units, we fill the string array with all of the options
	wxArrayString choices;
	int i;

	// Angle
	choices.Clear();
	for (i = 0; i < UnitConverter::AngleUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfAngle)i));

	wxStaticText *angleLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of angle"));
	unitOfAngle = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeAngle),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfAngle, additionalWidth);
	unitSelectionSizer->Add(angleLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfAngle, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfAngle->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeAngle));
#endif

	// Distance
	choices.Clear();
	for (i = 0; i < UnitConverter::DistanceUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfDistance)i));

	wxStaticText *DistanceLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of distance"));
	unitOfDistance = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfDistance, additionalWidth);
	unitSelectionSizer->Add(DistanceLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfDistance, 0, unitFlags);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfDistance->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));
#endif

	// Area
	choices.Clear();
	for (i = 0; i < UnitConverter::AreaUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfArea)i));

	wxStaticText *AreaLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of area"));
	unitOfArea = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeArea),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfArea, additionalWidth);
	unitSelectionSizer->Add(AreaLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfArea, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfArea->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeArea));
#endif

	// Force
	choices.Clear();
	for (i = 0; i < UnitConverter::ForceUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfForce)i));

	wxStaticText *ForceLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of force"));
	unitOfForce = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeForce),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfForce, additionalWidth);
	unitSelectionSizer->Add(ForceLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfForce, 0, unitFlags);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfForce->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeForce));
#endif

	// Pressure
	choices.Clear();
	for (i = 0; i < UnitConverter::PressureUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfPressure)i));

	wxStaticText *PressureLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of pressure"));
	unitOfPressure = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypePressure),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfPressure, additionalWidth);
	unitSelectionSizer->Add(PressureLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfPressure, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfPressure->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypePressure));
#endif

	// Moment
	choices.Clear();
	for (i = 0; i < UnitConverter::MomentUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfMoment)i));

	wxStaticText *MomentLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of moment"));
	unitOfMoment = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeMoment),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfMoment, additionalWidth);
	unitSelectionSizer->Add(MomentLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfMoment, 0, unitFlags);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfMoment->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeMoment));
#endif

	// Mass
	choices.Clear();
	for (i = 0; i < UnitConverter::MassUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfMass)i));

	wxStaticText *MassLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of mass"));
	unitOfMass = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeMass),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfMass, additionalWidth);
	unitSelectionSizer->Add(MassLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfMass, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfMass->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeMass));
#endif

	// Velocity
	choices.Clear();
	for (i = 0; i < UnitConverter::VelocityUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfVelocity)i));

	wxStaticText *VelocityLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of velocity"));
	unitOfVelocity = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeVelocity),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfVelocity, additionalWidth);
	unitSelectionSizer->Add(VelocityLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfVelocity, 0, unitFlags);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfVelocity->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeVelocity));
#endif

	// Acceleration
	choices.Clear();
	for (i = 0; i < UnitConverter::AccelerationUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfAcceleration)i));

	wxStaticText *AccelerationLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of acceleration"));
	unitOfAcceleration = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeAcceleration),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfAcceleration, additionalWidth);
	unitSelectionSizer->Add(AccelerationLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfAcceleration, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfAcceleration->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeAcceleration));
#endif

	// Inertia
	choices.Clear();
	for (i = 0; i < UnitConverter::InertiaUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfInertia)i));

	wxStaticText *InertiaLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of inertia"));
	unitOfInertia = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeInertia),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfInertia, additionalWidth);
	unitSelectionSizer->Add(InertiaLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfInertia, 0, unitFlags);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfInertia->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeInertia));
#endif

	// Density
	choices.Clear();
	for (i = 0; i < UnitConverter::DensityUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfDensity)i));

	wxStaticText *DensityLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of density"));
	unitOfDensity = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDensity),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfDensity, additionalWidth);
	unitSelectionSizer->Add(DensityLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfDensity, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfDensity->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDensity));
#endif

	// Power
	choices.Clear();
	for (i = 0; i < UnitConverter::PowerUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfPower)i));

	wxStaticText *PowerLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of power"));
	unitOfPower = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypePower),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfPower, additionalWidth);
	unitSelectionSizer->Add(PowerLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfPower, 0, unitFlags);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfPower->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypePower));
#endif

	// Energy
	choices.Clear();
	for (i = 0; i < UnitConverter::EnergyUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfEnergy)i));

	wxStaticText *EnergyLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of energy"));
	unitOfEnergy = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeEnergy),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfEnergy, additionalWidth);
	unitSelectionSizer->Add(EnergyLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfEnergy, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfEnergy->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeEnergy));
#endif

	// Temperature
	choices.Clear();
	for (i = 0; i < UnitConverter::TemperatureUnitCount; i++)
		choices.Add(UnitConverter::GetInstance().GetUnits((UnitConverter::UnitsOfTemperature)i));

	wxStaticText *TemperatureLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of temperature"));
	unitOfTemperature = new wxComboBox(unitsPage, wxID_ANY, UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeTemperature),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfTemperature, additionalWidth);
	unitSelectionSizer->Add(TemperatureLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfTemperature, 0, unitFlags);
	// Work-around for value not being set properly if initial string contains exponent (GTK ANSI build)
#if defined(__WXGTK__) && !defined(_UNICODE)
	unitOfTemperature->ChangeValue(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeTemperature));
#endif

	unitsPage->SetSizer(unitsTopSizer);

	// Add the Digits page
	digitsPage = new wxPanel(notebook);
	notebook->AddPage(digitsPage, _T("Digits"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *digitsTopSizer = new wxBoxSizer(wxVERTICAL);

	// Create the digits page main sizer
	wxBoxSizer *digitsSizer = new wxBoxSizer(wxVERTICAL);
	digitsTopSizer->Add(digitsSizer, 0,
		wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the label for the number of digits combo box
	wxStaticText *numberOfDigitsLabel = new wxStaticText(digitsPage, wxID_STATIC,
		_T("Number Of Digits"), wxDefaultPosition);

	// Create the number of digits combo box
	choices.Clear();
	wxString numberString;
	for (i = 0; i <= DBL_DIG; i++)
	{
		numberString.Printf("%i", i);
		choices.Add(numberString);
	}
	numberString.Printf("%i", UnitConverter::GetInstance().GetNumberOfDigits());
	numberOfDigits = new wxComboBox(digitsPage, wxID_ANY, numberString,
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);

	// Create a sizer for the number of digits label and input
	wxBoxSizer *numberOfDigitsSizer = new wxBoxSizer(wxHORIZONTAL);

	// Add the controls to the sizer and add the sizer to the page's sizer
	numberOfDigitsSizer->Add(numberOfDigitsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	numberOfDigitsSizer->Add(numberOfDigits, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	digitsSizer->Add(numberOfDigitsSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	SetMinimumWidthFromContents(numberOfDigits, additionalWidth);

	// Create the checkboxes
	useSignificantDigits = new wxCheckBox(digitsPage, wxID_ANY,
		_T("Use a consistent number of significant digits"));
	useScientificNotation = new wxCheckBox(digitsPage, wxID_ANY,
		_T("Use scientific notation"));

	// Set the initial value of the checkboxes to match the properties of the converter object
	useSignificantDigits->SetValue(UnitConverter::GetInstance().GetUseSignificantDigits());
	useScientificNotation->SetValue(UnitConverter::GetInstance().GetUseScientificNotation());

	digitsSizer->Add(useSignificantDigits, 0, wxALIGN_LEFT | wxALL, 5);
	digitsSizer->Add(useScientificNotation, 0, wxALIGN_LEFT | wxALL, 5);

	digitsPage->SetSizerAndFit(digitsTopSizer);

	kinematicsPage = new wxPanel(notebook);
	notebook->AddPage(kinematicsPage, _T("Kinematics"));
	wxBoxSizer *kinematicsTopSizer = new wxBoxSizer(wxVERTICAL);

	// Create the kinematics page main sizer
	wxBoxSizer *kinematicsSizer = new wxBoxSizer(wxVERTICAL);
	kinematicsTopSizer->Add(kinematicsSizer, 0,
		wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the center of rotation inputs
	wxBoxSizer *corSizer = new wxBoxSizer(wxHORIZONTAL);
	centerOfRotationX = new wxTextCtrl(kinematicsPage, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0, UnitValidator(
		kinematicInputs.centerOfRotation.x, UnitConverter::UnitTypeDistance));
	centerOfRotationY = new wxTextCtrl(kinematicsPage, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0, UnitValidator(
		kinematicInputs.centerOfRotation.y, UnitConverter::UnitTypeDistance));
	centerOfRotationZ = new wxTextCtrl(kinematicsPage, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0, UnitValidator(
		kinematicInputs.centerOfRotation.z, UnitConverter::UnitTypeDistance));

	// Create the center of rotation units label
	wxStaticText *corUnits = new wxStaticText(kinematicsPage, wxID_ANY, _T("(") +
		UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance) + _T(")"));

	// Add the controls to the spacer
	corSizer->Add(new wxStaticText(kinematicsPage, wxID_ANY,
		_T("Center Of Rotation")), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	corSizer->Add(new wxStaticText(kinematicsPage, wxID_ANY, _T("X")),
		0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
	corSizer->Add(centerOfRotationX, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	corSizer->Add(new wxStaticText(kinematicsPage, wxID_ANY, _T("Y")),
		0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
	corSizer->Add(centerOfRotationY, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	corSizer->Add(new wxStaticText(kinematicsPage, wxID_ANY, _T("Z")),
		0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
	corSizer->Add(centerOfRotationZ, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	corSizer->Add(corUnits, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Create a sizer for the radio box objects
	wxBoxSizer *radioOptionsSizer = new wxBoxSizer(wxHORIZONTAL);

	// Add the radio box for the order of rotation
	wxArrayString optionsArray;
	optionsArray.Add(_T("Roll first, then pitch about global axis"));
	optionsArray.Add(_T("Pitch first, then roll about global axis"));
	rotationOrder = new wxRadioBox(kinematicsPage, wxID_ANY, _T("Euler Rotation Order"), wxDefaultPosition,
		wxDefaultSize, optionsArray, optionsArray.GetCount(), wxRA_SPECIFY_ROWS);

	// Make sure the correct radio button is selected
	if (kinematicInputs.firstRotation == Vector::AxisX)
		rotationOrder->SetSelection(0);
	else
		rotationOrder->SetSelection(1);

	radioOptionsSizer->Add(rotationOrder, 0, wxALL, 5);

	// Add the radio box for the steering input type
	optionsArray.Clear();
	optionsArray.Add(_T("Use rack travel distance"));
	optionsArray.Add(_T("Use steering wheel angle"));
	steeringInputType = new wxRadioBox(kinematicsPage, wxID_ANY, _T("Steering Input"), wxDefaultPosition,
		wxDefaultSize, optionsArray, optionsArray.GetCount(), wxRA_SPECIFY_ROWS);

	// Make sure the correct radio button is selected
	if (mainFrame.GetUseRackTravel())
		steeringInputType->SetSelection(0);
	else
		steeringInputType->SetSelection(1);

	radioOptionsSizer->Add(steeringInputType, 0, wxALL, 5);

	// Create the "Number of Threads" text box
	wxBoxSizer *numberOfThreadsSizer = new wxBoxSizer(wxHORIZONTAL);
	wxString valueString;// TODO:  Use UnsignedValidator
	valueString.Printf("%i", mainFrame.GetNumberOfThreads());
	simultaneousThreads = new wxTextCtrl(kinematicsPage, wxID_ANY, valueString);
	numberOfThreadsSizer->Add(new wxStaticText(kinematicsPage, wxID_ANY,
		_T("Number of Simultaneous Threads to Execute")), 0,
		wxALIGN_CENTER_VERTICAL | wxALL, 5);
	numberOfThreadsSizer->Add(simultaneousThreads, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Add the sizers to the MainSizer
	kinematicsSizer->Add(corSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	kinematicsSizer->Add(radioOptionsSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	kinematicsSizer->Add(numberOfThreadsSizer, 0, wxALIGN_CENTER_HORIZONTAL);

	// Set the kinematics page's sizer
	kinematicsPage->SetSizerAndFit(kinematicsTopSizer);

	// Add the debugger page
	debuggerPage = new wxPanel(notebook);
	notebook->AddPage(debuggerPage, _T("Debugging"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *debuggerTopSizer = new wxBoxSizer(wxVERTICAL);

	// Create the kinematics page main sizer
	wxBoxSizer *debuggerSizer = new wxBoxSizer(wxVERTICAL);
	debuggerTopSizer->Add(debuggerSizer, 0,
		wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create a sizer for the radio box object
	wxBoxSizer *debugLevelsSizer = new wxBoxSizer(wxHORIZONTAL);

	// Add the radio box for the order of rotation
	optionsArray.Clear();
	optionsArray.Add(_T("Very Verbose:  Most detailed; displays all available information"));
	optionsArray.Add(_T("Verbose:  Displays some additional error details"));
	optionsArray.Add(_T("Normal:  Displays all warnings and errors, without details"));
	optionsArray.Add(_T("Minimal:  Only critical errors and warnings are displayed"));
	debugLevel = new wxRadioBox(debuggerPage, wxID_ANY, _T("Debugger Message Verbosity"), wxDefaultPosition,
		wxDefaultSize, optionsArray, optionsArray.GetCount(), wxRA_SPECIFY_ROWS);

	// Make sure the correct radio button is selected
	if(Debugger::GetInstance().GetDebugLevel() == Debugger::PriorityLow)
		debugLevel->SetSelection(0);
	else if(Debugger::GetInstance().GetDebugLevel() == Debugger::PriorityMedium)
		debugLevel->SetSelection(1);
	else if(Debugger::GetInstance().GetDebugLevel() == Debugger::PriorityHigh)
		debugLevel->SetSelection(2);
	else// Debugger::PriorityVeryHigh
		debugLevel->SetSelection(3);

	debugLevelsSizer->Add(debugLevel, 0, wxALL, 5);

	debuggerSizer->Add(debugLevelsSizer, 0, wxALIGN_CENTER_HORIZONTAL);

	debuggerPage->SetSizerAndFit(debuggerTopSizer);

	// Add the fonts page
	fontPage = new wxPanel(notebook);
	notebook->AddPage(fontPage, _T("Fonts"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *fontsTopSizer = new wxBoxSizer(wxVERTICAL);

	// Create the kinematics page main sizer
	wxFlexGridSizer *fontsSizer = new wxFlexGridSizer(3, 10, 10);
	fontsTopSizer->Add(fontsSizer, 0,
		wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Get the plot objects from the main frame
	outputFont = mainFrame.GetOutputFont();
	plotFont = mainFrame.GetPlotFont();

	if (outputFont.IsOk())
	{
		outputFontLabel = new wxStaticText(fontPage, wxID_ANY, outputFont.GetFaceName());
		outputFontLabel->SetFont(outputFont);
	}
	else
		outputFontLabel = new wxStaticText(fontPage, wxID_ANY, _T("Not Set"));

	if (plotFont.IsOk())
	{
		plotFontLabel = new wxStaticText(fontPage, wxID_ANY, plotFont.GetFaceName());
		plotFontLabel->SetFont(plotFont);
	}
	else
		plotFontLabel = new wxStaticText(fontPage, wxID_ANY, _T("Not Set"));

	// Add the controls to the MainSizer
	fontsSizer->Add(new wxStaticText(fontPage, wxID_ANY, _T("Output Font")),
		0, wxALIGN_CENTER_VERTICAL);
	fontsSizer->Add(outputFontLabel, 0, wxALIGN_CENTER_VERTICAL);
	fontsSizer->Add(new wxButton(fontPage, ChangeOutputFontButton,
		_T("Change...")), 0, wxALIGN_CENTER_VERTICAL);
	fontsSizer->Add(new wxStaticText(fontPage, wxID_ANY, _T("Plot Font")),
		0, wxALIGN_CENTER_VERTICAL);
	fontsSizer->Add(plotFontLabel, 0, wxALIGN_CENTER_VERTICAL);
	fontsSizer->Add(new wxButton(fontPage, ChangePlotFontButton,
		_T("Change...")), 0, wxALIGN_CENTER_VERTICAL);

	// Set the debug page's sizer
	fontPage->SetSizerAndFit(fontsTopSizer);

	// Add a spacer between the notebook and the buttons
	mainSizer->AddSpacer(10);

	// Create another sizer for the buttons at the bottom and add the buttons
	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxGROW);

	topSizer->SetSizeHints(this);

	SetSizer(topSizer);
}

//==========================================================================
// Class:			OptionsDialog
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
void OptionsDialog::OKClickEvent(wxCommandEvent& WXUNUSED(event))
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	if (rotationOrder->GetSelection() == 0)
		kinematicInputs.firstRotation = Vector::AxisX;
	else
		kinematicInputs.firstRotation = Vector::AxisY;

	if (steeringInputType->GetSelection() == 0)
		mainFrame.SetUseRackTravel(true);
	else
		mainFrame.SetUseRackTravel(false);

	long numberOfThreads(0);
	if (simultaneousThreads->GetValue().ToLong(&numberOfThreads) && numberOfThreads > 0)
		mainFrame.SetNumberOfThreads(numberOfThreads);
	else
		Debugger::GetInstance() <<
			"Warning!  Specified number of threads is not valid (must be a number greater than zero) - using previous value of "
			 << mainFrame.GetNumberOfThreads() << Debugger::PriorityHigh;

	// Update the default units for the converter object
	// NOTE:  This section MUST come after the center of rotation is updated in order for
	// the units label on the center of rotation input to have been correct
	UnitConverter::GetInstance().SetAngleUnits((UnitConverter::UnitsOfAngle)SafelyGetComboBoxSelection(unitOfAngle));
	UnitConverter::GetInstance().SetDistanceUnits((UnitConverter::UnitsOfDistance)SafelyGetComboBoxSelection(unitOfDistance));
	UnitConverter::GetInstance().SetForceUnits((UnitConverter::UnitsOfForce)SafelyGetComboBoxSelection(unitOfForce));
	UnitConverter::GetInstance().SetAreaUnits((UnitConverter::UnitsOfArea)SafelyGetComboBoxSelection(unitOfArea));
	UnitConverter::GetInstance().SetPressureUnits((UnitConverter::UnitsOfPressure)SafelyGetComboBoxSelection(unitOfPressure));
	UnitConverter::GetInstance().SetMomentUnits((UnitConverter::UnitsOfMoment)SafelyGetComboBoxSelection(unitOfMoment));
	UnitConverter::GetInstance().SetMassUnits((UnitConverter::UnitsOfMass)SafelyGetComboBoxSelection(unitOfMass));
	UnitConverter::GetInstance().SetVelocityUnits((UnitConverter::UnitsOfVelocity)SafelyGetComboBoxSelection(unitOfVelocity));
	UnitConverter::GetInstance().SetAccelerationUnits((UnitConverter::UnitsOfAcceleration)SafelyGetComboBoxSelection(unitOfAcceleration));
	UnitConverter::GetInstance().SetInertiaUnits((UnitConverter::UnitsOfInertia)SafelyGetComboBoxSelection(unitOfInertia));
	UnitConverter::GetInstance().SetDensityUnits((UnitConverter::UnitsOfDensity)SafelyGetComboBoxSelection(unitOfDensity));
	UnitConverter::GetInstance().SetPowerUnits((UnitConverter::UnitsOfPower)SafelyGetComboBoxSelection(unitOfPower));
	UnitConverter::GetInstance().SetEnergyUnits((UnitConverter::UnitsOfEnergy)SafelyGetComboBoxSelection(unitOfEnergy));
	UnitConverter::GetInstance().SetTemperatureUnits((UnitConverter::UnitsOfTemperature)SafelyGetComboBoxSelection(unitOfTemperature));

	UnitConverter::GetInstance().SetNumberOfDigits(SafelyGetComboBoxSelection(numberOfDigits));
	UnitConverter::GetInstance().SetUseSignificantDigits(useSignificantDigits->GetValue());
	UnitConverter::GetInstance().SetUseScientificNotation(useScientificNotation->GetValue());

	if (debugLevel->GetSelection() == 0)
		Debugger::GetInstance().SetDebugLevel(Debugger::PriorityLow);
	else if (debugLevel->GetSelection() == 1)
		Debugger::GetInstance().SetDebugLevel(Debugger::PriorityMedium);
	else if (debugLevel->GetSelection() == 2)
		Debugger::GetInstance().SetDebugLevel(Debugger::PriorityHigh);
	else
		Debugger::GetInstance().SetDebugLevel(Debugger::PriorityVeryHigh);

	mainFrame.SetOutputFont(outputFont);
	mainFrame.SetPlotFont(plotFont);

	if (IsModal())
		EndModal(wxOK);
	else
	{
		SetReturnCode(wxOK);
		Show(false);
	}
}

//==========================================================================
// Class:			OptionsDialog
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
void OptionsDialog::CancelClickEvent(wxCommandEvent& WXUNUSED(event))
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
// Class:			OptionsDialog
// Function:		ChangeOutputFontClickEvent
//
// Description:		Handles the "change output font" button clicked event.
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
void OptionsDialog::ChangeOutputFontClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Display the font selection dialog
	// If user selected a new font, update the font labels and the class members
	wxFont tempFont = wxGetFontFromUser(this, outputFont, _T("Select Output Font"));
	if (tempFont.IsOk())
	{
		outputFont = tempFont;
		outputFontLabel->SetFont(outputFont);
		outputFontLabel->SetLabel(outputFont.GetFaceName());
	}

	// FIXME:  Re-size sizers in case font name changed length
}

//==========================================================================
// Class:			OptionsDialog
// Function:		ChangePlotFontClickEvent
//
// Description:		Handles the "change plot font" button clicked event.
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
void OptionsDialog::ChangePlotFontClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Display the font selection dialog
	// If user selected a new font, update the font labels and the class members
	wxFont tempFont = wxGetFontFromUser(this, plotFont, _T("Select Plot Font"));
	if (tempFont.IsOk())
	{
		plotFont = tempFont;
		plotFontLabel->SetFont(plotFont);
		plotFontLabel->SetLabel(plotFont.GetFaceName());
	}

	// FIXME:  Re-size sizers in case font name changed length
}
