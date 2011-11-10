/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  optionsDialog.cpp
// Created:  2/9/2009
// Author:  K. Loux
// Description:  Contains the definition for the OPTION_DIALOG class.
// History:
//	3/24/2009	- Added Kinematics tab, K. Loux.

// Standard C++ headers
#include <cfloat>

// wxWidgets headers
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/radiobox.h>

// CarDesigner headers
#include "gui/dialogs/optionsDialog.h"
#include "gui/components/mainFrame.h"
#include "vUtilities/convert.h"
#include "vUtilities/wxRelatedUtilities.h"

//==========================================================================
// Class:			OPTIONS_DIALOG
// Function:		OPTIONS_DIALOG
//
// Description:		Constructor for OPTIONS_DIALOG class.
//
// Input Arguments:
//		_MainFrame			= MainFrame& pointing to the application's main window
//		_Converter			= Convert& pointing to the application's converter object
//		_KinematicInputs	= KINEMATICS::INPUTS& reference to the application's analysis options
//		Id					= wxWindowId for this object
//		Position			= const wxPoint& where this object will be drawn
//		_debugger			= Debugger& pointing to this application's debug
//							  printing utility
//		Style				= long defining the style for this dialog
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
OPTIONS_DIALOG::OPTIONS_DIALOG(MAIN_FRAME &_MainFrame, Convert &_Converter,
							   KINEMATICS::INPUTS &_KinematicInputs, wxWindowID Id,
							   const wxPoint &Position, Debugger &_debugger, long Style)
							   : wxDialog(&_MainFrame, Id, _T("Options"), Position, wxDefaultSize, Style),
							   debugger(_debugger), Converter(_Converter), KinematicInputs(_KinematicInputs),
							   MainFrame(_MainFrame)
{
	// Set up the form's layout
	CreateControls();

	// Center the dialog on the screen
	Center();
}

//==========================================================================
// Class:			OPTIONS_DIALOG
// Function:		~OPTIONS_DIALOG
//
// Description:		Destructor for OPTIONS_DIALOG class.
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
OPTIONS_DIALOG::~OPTIONS_DIALOG()
{
}

//==========================================================================
// Class:			OPTIONS_DIALOG
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
BEGIN_EVENT_TABLE(OPTIONS_DIALOG, wxDialog)
	EVT_BUTTON(wxID_OK,		OPTIONS_DIALOG::OKClickEvent)
	EVT_BUTTON(wxID_CANCEL,	OPTIONS_DIALOG::CancelClickEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			OPTIONS_DIALOG
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
void OPTIONS_DIALOG::CreateControls(void)
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the notebook
	Notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
	MainSizer->Add(Notebook);

	// Create the units page
	UnitsPage = new wxPanel(Notebook);
	Notebook->AddPage(UnitsPage, _T("Units"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *UnitsTopSizer = new wxBoxSizer(wxVERTICAL);

	// Create the units page main sizer
	wxBoxSizer *UnitsSizer = new wxBoxSizer(wxVERTICAL);
	UnitsTopSizer->Add(UnitsSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Add a horizontal spacer for the text across the top
	wxBoxSizer *UnitsCaptionSizer = new wxBoxSizer(wxHORIZONTAL);
	UnitsSizer->Add(UnitsCaptionSizer, 0, wxALIGN_TOP);

	// Add the text to this spacer
	wxStaticText *Prompt = new wxStaticText(UnitsPage, wxID_STATIC,
		_T("Choose the units to use for each quantity"));
	UnitsCaptionSizer->Add(-1, -1, 0, wxALL, 5);
	UnitsCaptionSizer->Add(Prompt, 0, wxALL, 5);

	// Create a sizer to contain the inputs
	wxFlexGridSizer *unitSelectionSizer = new wxFlexGridSizer(5, 5, 5);
	unitSelectionSizer->SetFlexibleDirection(wxHORIZONTAL);
	UnitsSizer->Add(unitSelectionSizer, 0, wxALL, 5);
	
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
	wxArrayString Choices;
	int i;

	// Angle
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_ANGLE_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfAngle)i));

	wxStaticText *AngleLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of angle"));
	UnitOfAngle = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypeAngle),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfAngle, additionalWidth);
	unitSelectionSizer->Add(AngleLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfAngle, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Distance
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_DISTANCE_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfDistance)i));

	wxStaticText *DistanceLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of distance"));
	UnitOfDistance = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypeDistance),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfDistance, additionalWidth);
	unitSelectionSizer->Add(DistanceLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfDistance, 0, unitFlags);

	// Area
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_AREA_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfArea)i));

	wxStaticText *AreaLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of area"));
	UnitOfArea = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypeArea),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfArea, additionalWidth);
	unitSelectionSizer->Add(AreaLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfArea, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Force
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_FORCE_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfForce)i));

	wxStaticText *ForceLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of force"));
	UnitOfForce = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypeForce),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfForce, additionalWidth);
	unitSelectionSizer->Add(ForceLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfForce, 0, unitFlags);

	// Pressure
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_PRESSURE_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfPressure)i));

	wxStaticText *PressureLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of pressure"));
	UnitOfPressure = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypePressure),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfPressure, additionalWidth);
	unitSelectionSizer->Add(PressureLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfPressure, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Moment
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_MOMENT_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfMoment)i));

	wxStaticText *MomentLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of moment"));
	UnitOfMoment = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypeMoment),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfMoment, additionalWidth);
	unitSelectionSizer->Add(MomentLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfMoment, 0, unitFlags);

	// Mass
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_MASS_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfMass)i));

	wxStaticText *MassLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of mass"));
	UnitOfMass = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypeMass),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfMass, additionalWidth);
	unitSelectionSizer->Add(MassLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfMass, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Velocity
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_VELOCITY_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfVelocity)i));

	wxStaticText *VelocityLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of velocity"));
	UnitOfVelocity = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypeVelocity),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfVelocity, additionalWidth);
	unitSelectionSizer->Add(VelocityLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfVelocity, 0, unitFlags);

	// Acceleration
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_ACCELERATION_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfAcceleration)i));

	wxStaticText *AccelerationLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of acceleration"));
	UnitOfAcceleration = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypeAcceleration),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfAcceleration, additionalWidth);
	unitSelectionSizer->Add(AccelerationLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfAcceleration, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Inertia
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_INERTIA_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfInertia)i));

	wxStaticText *InertiaLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of inertia"));
	UnitOfInertia = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypeInertia),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfInertia, additionalWidth);
	unitSelectionSizer->Add(InertiaLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfInertia, 0, unitFlags);

	// Density
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_DENSITY_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfDensity)i));

	wxStaticText *DensityLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of density"));
	UnitOfDensity = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypeDensity),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfDensity, additionalWidth);
	unitSelectionSizer->Add(DensityLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfDensity, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Power
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_POWER_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfPower)i));

	wxStaticText *PowerLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of power"));
	UnitOfPower = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypePower),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfPower, additionalWidth);
	unitSelectionSizer->Add(PowerLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfPower, 0, unitFlags);

	// Energy
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_ENERGY_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfEnergy)i));

	wxStaticText *EnergyLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of energy"));
	UnitOfEnergy = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypeEnergy),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfEnergy, additionalWidth);
	unitSelectionSizer->Add(EnergyLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfEnergy, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Temperature
	Choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_TEMPERATURE_UNITS; i++)
		Choices.Add(Converter.GetUnits((Convert::UnitsOfTemperature)i));

	wxStaticText *TemperatureLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of temperature"));
	UnitOfTemperature = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(Convert::UnitTypeTemperature),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	SetMinimumWidthFromContents(UnitOfTemperature, additionalWidth);
	unitSelectionSizer->Add(TemperatureLabelText, 0, labelFlags);
	unitSelectionSizer->Add(UnitOfTemperature, 0, unitFlags);

	// Set the unit page's sizer
	UnitsPage->SetSizer(UnitsTopSizer);

	// Add the Digits page
	DigitsPage = new wxPanel(Notebook);
	Notebook->AddPage(DigitsPage, _T("Digits"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *DigitsTopSizer = new wxBoxSizer(wxVERTICAL);

	// Create the digits page main sizer
	wxBoxSizer *DigitsSizer = new wxBoxSizer(wxVERTICAL);
	DigitsTopSizer->Add(DigitsSizer, 0,
		wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the label for the number of digits combo box
	wxStaticText *NumberOfDigitsLabel = new wxStaticText(DigitsPage, wxID_STATIC,
		_T("Number Of Digits"), wxDefaultPosition);

	// Create the number of digits combo box
	Choices.Clear();
	wxString NumberString;
	for (i = 0; i <= DBL_DIG; i++)
	{
		NumberString.Printf("%i", i);
		Choices.Add(NumberString);
	}
	NumberString.Printf("%i", Converter.GetNumberOfDigits());
	NumberOfDigits = new wxComboBox(DigitsPage, wxID_ANY, NumberString,
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);

	// Create a sizer for the number of digits label and input
	wxBoxSizer *NumberOfDigitsSizer = new wxBoxSizer(wxHORIZONTAL);

	// Add the controls to the sizer and add the sizer to the page's sizer
	NumberOfDigitsSizer->Add(NumberOfDigitsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	NumberOfDigitsSizer->Add(NumberOfDigits, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	DigitsSizer->Add(NumberOfDigitsSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	SetMinimumWidthFromContents(NumberOfDigits, additionalWidth);

	// Create the checkboxes
	UseSignificantDigits = new wxCheckBox(DigitsPage, wxID_ANY,
		_T("Use a consistent number of significant digits"));
	UseScientificNotation = new wxCheckBox(DigitsPage, wxID_ANY,
		_T("Use scientific notation"));

	// Set the initial value of the checkboxes to match the properties of the converter object
	UseSignificantDigits->SetValue(Converter.GetUseSignificantDigits());
	UseScientificNotation->SetValue(Converter.GetUseScientificNotation());

	// Add the checkboxes to the control
	DigitsSizer->Add(UseSignificantDigits, 0, wxALIGN_LEFT | wxALL, 5);
	DigitsSizer->Add(UseScientificNotation, 0, wxALIGN_LEFT | wxALL, 5);

	// Set the digits page's sizer
	DigitsPage->SetSizerAndFit(DigitsTopSizer);

	// Add the Kinematic Analysis page
	KinematicsPage = new wxPanel(Notebook);
	Notebook->AddPage(KinematicsPage, _T("Kinematics"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *KinematicsTopSizer = new wxBoxSizer(wxVERTICAL);

	// Specify the column widths
	int inputColumnWidth = 50;

	// Create the kinematics page main sizer
	wxBoxSizer *KinematicsSizer = new wxBoxSizer(wxVERTICAL);
	KinematicsTopSizer->Add(KinematicsSizer, 0,
		wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the center of rotation labels
	wxStaticText *XLabel = new wxStaticText(KinematicsPage, wxID_ANY, _T("X"));
	wxStaticText *YLabel = new wxStaticText(KinematicsPage, wxID_ANY, _T("Y"));
	wxStaticText *ZLabel = new wxStaticText(KinematicsPage, wxID_ANY, _T("Z"));

	// Create the center of rotation inputs
	wxBoxSizer *CoRSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *CoRLabel = new wxStaticText(KinematicsPage, wxID_ANY, _T("Center Of Rotation"));
	wxString ValueString;
	ValueString.Printf("%0.3f", KinematicInputs.CenterOfRotation.x);
	CenterOfRotationX = new wxTextCtrl(KinematicsPage, wxID_ANY, ValueString, wxDefaultPosition,
		wxSize(inputColumnWidth, -1));
	ValueString.Printf("%0.3f", KinematicInputs.CenterOfRotation.y);
	CenterOfRotationY = new wxTextCtrl(KinematicsPage, wxID_ANY, ValueString, wxDefaultPosition,
		wxSize(inputColumnWidth, -1));
	ValueString.Printf("%0.3f", KinematicInputs.CenterOfRotation.z);
	CenterOfRotationZ = new wxTextCtrl(KinematicsPage, wxID_ANY, ValueString, wxDefaultPosition,
		wxSize(inputColumnWidth, -1));

	// Create the center of rotation units label
	wxStaticText *CoRUnits = new wxStaticText(KinematicsPage, wxID_ANY, _T("(") + 
		Converter.GetUnitType(Convert::UnitTypeDistance) + _T(")"));

	// Add the controls to the spacer
	CoRSizer->Add(CoRLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	CoRSizer->Add(XLabel, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
	CoRSizer->Add(CenterOfRotationX, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	CoRSizer->Add(YLabel, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
	CoRSizer->Add(CenterOfRotationY, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	CoRSizer->Add(ZLabel, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
	CoRSizer->Add(CenterOfRotationZ, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	CoRSizer->Add(CoRUnits, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Create a sizer for the radio box objects
	wxBoxSizer *RadioOptionsSizer = new wxBoxSizer(wxHORIZONTAL);

	// Add the radio box for the order of rotation
	wxArrayString OptionsArray;
	OptionsArray.Add(_T("Roll first, then pitch about global axis"));
	OptionsArray.Add(_T("Pitch first, then roll about global axis"));
	RotationOrder = new wxRadioBox(KinematicsPage, wxID_ANY, _T("Euler Rotation Order"), wxDefaultPosition,
		wxDefaultSize, OptionsArray, OptionsArray.GetCount(), wxRA_SPECIFY_ROWS);

	// Make sure the correct radio button is selected
	if (KinematicInputs.FirstRotation == Vector::AxisX)
		RotationOrder->SetSelection(0);
	else
		RotationOrder->SetSelection(1);
		
	// Add the radio box to the sizer
	RadioOptionsSizer->Add(RotationOrder, 0, wxALL, 5);

	// Add the radio box for the steering input type
	OptionsArray.Clear();
	OptionsArray.Add(_T("Use rack travel distance"));
	OptionsArray.Add(_T("Use steering wheel angle"));
	SteeringInputType = new wxRadioBox(KinematicsPage, wxID_ANY, _T("Steering Input"), wxDefaultPosition,
		wxDefaultSize, OptionsArray, OptionsArray.GetCount(), wxRA_SPECIFY_ROWS);

	// Make sure the correct radio button is selected
	if (MainFrame.GetUseRackTravel())
		SteeringInputType->SetSelection(0);
	else
		SteeringInputType->SetSelection(1);
		
	// Add the radio box to the sizer
	RadioOptionsSizer->Add(SteeringInputType, 0, wxALL, 5);

	// Create the "Number of Threads" text box
	wxBoxSizer *NumberOfThreadsSizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *NumberOfThreadsLabel = new wxStaticText(KinematicsPage,
		wxID_ANY, _T("Number of Simultaneous Threads to Execute"));
	ValueString.Printf("%i", MainFrame.GetNumberOfThreads());
	SimultaneousThreads = new wxTextCtrl(KinematicsPage, wxID_ANY, ValueString,
		wxDefaultPosition, wxSize(inputColumnWidth, -1));
	NumberOfThreadsSizer->Add(NumberOfThreadsLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	NumberOfThreadsSizer->Add(SimultaneousThreads, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Add the sizers to the MainSizer
	KinematicsSizer->Add(CoRSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	KinematicsSizer->Add(RadioOptionsSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	KinematicsSizer->Add(NumberOfThreadsSizer, 0, wxALIGN_BOTTOM | wxALIGN_CENTER_HORIZONTAL);

	// Set the kinematics page's sizer
	KinematicsPage->SetSizerAndFit(KinematicsTopSizer);

	// Add the debugger page
	DebuggerPage = new wxPanel(Notebook);
	Notebook->AddPage(DebuggerPage, _T("Debugging"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *DebuggerTopSizer = new wxBoxSizer(wxVERTICAL);

	// Create the kinematics page main sizer
	wxBoxSizer *DebuggerSizer = new wxBoxSizer(wxVERTICAL);
	DebuggerTopSizer->Add(DebuggerSizer, 0,
		wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create a sizer for the radio box object
	wxBoxSizer *DebugLevelsSizer = new wxBoxSizer(wxHORIZONTAL);

	// Add the radio box for the order of rotation
	OptionsArray.Clear();
	OptionsArray.Add(_T("Very Verbose:  Most detailed; displays all available information"));
	OptionsArray.Add(_T("Verbose:  Displays some additional error details"));
	OptionsArray.Add(_T("Normal:  Displays all warnings and errors, without details"));
	OptionsArray.Add(_T("Minimal:  Only critical errors and warnings are displayed"));
	DebugLevel = new wxRadioBox(DebuggerPage, wxID_ANY, _T("Debugger Message Verbosity"), wxDefaultPosition,
		wxDefaultSize, OptionsArray, OptionsArray.GetCount(), wxRA_SPECIFY_ROWS);

	// Make sure the correct radio button is selected
	if(debugger.GetDebugLevel() == Debugger::PriorityLow)
		DebugLevel->SetSelection(0);
	else if(debugger.GetDebugLevel() == Debugger::PriorityMedium)
		DebugLevel->SetSelection(1);
	else if(debugger.GetDebugLevel() == Debugger::PriorityHigh)
		DebugLevel->SetSelection(2);
	else// Debugger::PriorityVeryHigh
		DebugLevel->SetSelection(3);
		
	// Add the radio box to the sizer
	DebugLevelsSizer->Add(DebugLevel, 0, wxALL, 5);

	// Add the sizers to the MainSizer
	DebuggerSizer->Add(DebugLevelsSizer, 0, wxALIGN_CENTER_HORIZONTAL);

	// Set the debug page's sizer
	DebuggerPage->SetSizerAndFit(DebuggerTopSizer);

	// Add a spacer between the notebook and the buttons
	MainSizer->AddSpacer(10);

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
// Class:			OPTIONS_DIALOG
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
void OPTIONS_DIALOG::OKClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Update the center of rotation for the kinematic analysis object
	// It is important that this is done before the converter is updated!
	// FIXME:  This should use a textvalidator and it will be much cleaner
	Vector CenterOfRotation;
	if (CenterOfRotationX->GetValue().ToDouble(&CenterOfRotation.x) &&
		CenterOfRotationY->GetValue().ToDouble(&CenterOfRotation.y) &&
		CenterOfRotationZ->GetValue().ToDouble(&CenterOfRotation.z))
		// Convert the center of rotation and update the analysis object
		KinematicInputs.CenterOfRotation = Converter.ConvertDistance(CenterOfRotation);
	else
		// Print a warning so the user knows the CoR was rejected
		debugger.Print(_T("Warning!  Center of rotation is not a valid vector - using previous value"),
			Debugger::PriorityHigh);

	// Set the order of rotations
	if (RotationOrder->GetSelection() == 0)
		KinematicInputs.FirstRotation = Vector::AxisX;
	else
		KinematicInputs.FirstRotation = Vector::AxisY;

	// Set the steering input type
	if (SteeringInputType->GetSelection() == 0)
		MainFrame.SetUseRackTravel(true);
	else
		MainFrame.SetUseRackTravel(false);

	// Set the number of threads to use in the solver
	long NumberOfThreads(0);
	if (SimultaneousThreads->GetValue().ToLong(&NumberOfThreads) && NumberOfThreads > 0)
		MainFrame.SetNumberOfThreads(NumberOfThreads);
	else
		debugger.Print(Debugger::PriorityHigh,
			"Warning!  Specified number of threads is not valid (must be a number greater than zero) - using previous value of %i",
			MainFrame.GetNumberOfThreads());

	// Update the default units for the converter object
	// NOTE:  This section MUST come after the center of rotation is updated in order for
	// the units label on the center of rotation input to have been correct
	Converter.SetAngleUnits((Convert::UnitsOfAngle)SafelyGetComboBoxSelection(UnitOfAngle));
	Converter.SetDistanceUnits((Convert::UnitsOfDistance)SafelyGetComboBoxSelection(UnitOfDistance));
	Converter.SetForceUnits((Convert::UnitsOfForce)SafelyGetComboBoxSelection(UnitOfForce));
	Converter.SetAreaUnits((Convert::UnitsOfArea)SafelyGetComboBoxSelection(UnitOfArea));
	Converter.SetPressureUnits((Convert::UnitsOfPressure)SafelyGetComboBoxSelection(UnitOfPressure));
	Converter.SetMomentUnits((Convert::UnitsOfMoment)SafelyGetComboBoxSelection(UnitOfMoment));
	Converter.SetMassUnits((Convert::UnitsOfMass)SafelyGetComboBoxSelection(UnitOfMass));
	Converter.SetVelocityUnits((Convert::UnitsOfVelocity)SafelyGetComboBoxSelection(UnitOfVelocity));
	Converter.SetAccelerationUnits((Convert::UnitsOfAcceleration)SafelyGetComboBoxSelection(UnitOfAcceleration));
	Converter.SetInertiaUnits((Convert::UnitsOfInertia)SafelyGetComboBoxSelection(UnitOfInertia));
	Converter.SetDensityUnits((Convert::UnitsOfDensity)SafelyGetComboBoxSelection(UnitOfDensity));
	Converter.SetPowerUnits((Convert::UnitsOfPower)SafelyGetComboBoxSelection(UnitOfPower));
	Converter.SetEnergyUnits((Convert::UnitsOfEnergy)SafelyGetComboBoxSelection(UnitOfEnergy));
	Converter.SetTemperatureUnits((Convert::UnitsOfTemperature)SafelyGetComboBoxSelection(UnitOfTemperature));

	// Update the number of digits and the rules for formatting numbers
	Converter.SetNumberOfDigits(SafelyGetComboBoxSelection(NumberOfDigits));
	Converter.SetUseSignificantDigits(UseSignificantDigits->GetValue());
	Converter.SetUseScientificNotation(UseScientificNotation->GetValue());

	// Update the debug level
	if (DebugLevel->GetSelection() == 0)
		debugger.SetDebugLevel(Debugger::PriorityLow);
	else if (DebugLevel->GetSelection() == 1)
		debugger.SetDebugLevel(Debugger::PriorityMedium);
	else if (DebugLevel->GetSelection() == 2)
		debugger.SetDebugLevel(Debugger::PriorityHigh);
	else
		debugger.SetDebugLevel(Debugger::PriorityVeryHigh);

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
// Class:			OPTIONS_DIALOG
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
void OPTIONS_DIALOG::CancelClickEvent(wxCommandEvent& WXUNUSED(event))
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
