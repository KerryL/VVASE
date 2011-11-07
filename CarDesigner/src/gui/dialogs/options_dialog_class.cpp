/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  options_dialog_class.cpp
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
#include "gui/dialogs/options_dialog_class.h"
#include "gui/components/main_frame_class.h"
#include "vUtilities/convert_class.h"
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
//		_Debugger			= Debugger& pointing to this application's debug
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
OPTIONS_DIALOG::OPTIONS_DIALOG(MAIN_FRAME &_MainFrame, CONVERT &_Converter,
							   KINEMATICS::INPUTS &_KinematicInputs, wxWindowID Id,
							   const wxPoint &Position, DEBUGGER &_Debugger, long Style)
							   : wxDialog(&_MainFrame, Id, _T("Options"), Position, wxDefaultSize, Style),
							   Debugger(_Debugger), Converter(_Converter), KinematicInputs(_KinematicInputs),
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

	// Create a sizer for each column
	wxBoxSizer *sizerLeftLabelColumn = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizerLeftUnitsColumn = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizerRightLabelColumn = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizerRightUnitsColumn = new wxBoxSizer(wxVERTICAL);
	
	// When setting the control width, we need to account for the width of the
	// "expand" button, etc., so we specify that here
#ifdef __WXGTK__
	unsigned int additionalWidth = 40;// [pixels]
#else
	unsigned int additionalWidth = 30;// [pixels]
#endif
	
	// The column flags
	int labelFlags = wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT;
	int unitFlags = wxALL | wxEXPAND;

	// Add the static text and text controls to these sizers
	// For each type of units, we fill the string array with all of the options
	wxArrayString Choices;
	int i;

	// Angle
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_ANGLE_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_ANGLE)i));

	wxStaticText *AngleLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of angle"));
	UnitOfAngle = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_ANGLE),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerLeftLabelColumn->Add(AngleLabelText, 1, labelFlags, 5);
	sizerLeftUnitsColumn->Add(UnitOfAngle, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfAngle, additionalWidth);

	// Distance
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_DISTANCE_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_DISTANCE)i));

	wxStaticText *DistanceLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of distance"));
	UnitOfDistance = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_DISTANCE),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerRightLabelColumn->Add(DistanceLabelText, 1, labelFlags, 5);
	sizerRightUnitsColumn->Add(UnitOfDistance, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfDistance, additionalWidth);

	// Area
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_AREA_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_AREA)i));

	wxStaticText *AreaLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of area"));
	UnitOfArea = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_AREA),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerLeftLabelColumn->Add(AreaLabelText, 1, labelFlags, 5);
	sizerLeftUnitsColumn->Add(UnitOfArea, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfArea, additionalWidth);

	// Force
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_FORCE_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_FORCE)i));

	wxStaticText *ForceLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of force"));
	UnitOfForce = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_FORCE),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerRightLabelColumn->Add(ForceLabelText, 1, labelFlags, 5);
	sizerRightUnitsColumn->Add(UnitOfForce, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfForce, additionalWidth);

	// Pressure
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_PRESSURE_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_PRESSURE)i));

	wxStaticText *PressureLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of pressure"));
	UnitOfPressure = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_PRESSURE),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerLeftLabelColumn->Add(PressureLabelText, 1, labelFlags, 5);
	sizerLeftUnitsColumn->Add(UnitOfPressure, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfPressure, additionalWidth);

	// Moment
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_MOMENT_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_MOMENT)i));

	wxStaticText *MomentLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of moment"));
	UnitOfMoment = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_MOMENT),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerRightLabelColumn->Add(MomentLabelText, 1, labelFlags, 5);
	sizerRightUnitsColumn->Add(UnitOfMoment, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfMoment, additionalWidth);

	// Mass
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_MASS_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_MASS)i));

	wxStaticText *MassLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of mass"));
	UnitOfMass = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_MASS),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerLeftLabelColumn->Add(MassLabelText, 1, labelFlags, 5);
	sizerLeftUnitsColumn->Add(UnitOfMass, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfMass, additionalWidth);

	// Velocity
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_VELOCITY_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_VELOCITY)i));

	wxStaticText *VelocityLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of velocity"));
	UnitOfVelocity = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_VELOCITY),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerRightLabelColumn->Add(VelocityLabelText, 1, labelFlags, 5);
	sizerRightUnitsColumn->Add(UnitOfVelocity, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfVelocity, additionalWidth);

	// Acceleration
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_ACCELERATION_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_ACCELERATION)i));

	wxStaticText *AccelerationLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of acceleration"));
	UnitOfAcceleration = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_ACCELERATION),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerLeftLabelColumn->Add(AccelerationLabelText, 1, labelFlags, 5);
	sizerLeftUnitsColumn->Add(UnitOfAcceleration, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfAcceleration, additionalWidth);

	// Inertia
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_INERTIA_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_INERTIA)i));

	wxStaticText *InertiaLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of inertia"));
	UnitOfInertia = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_INERTIA),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerRightLabelColumn->Add(InertiaLabelText, 1, labelFlags, 5);
	sizerRightUnitsColumn->Add(UnitOfInertia, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfInertia, additionalWidth);

	// Density
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_DENSITY_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_DENSITY)i));

	wxStaticText *DensityLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of density"));
	UnitOfDensity = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_DENSITY),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerLeftLabelColumn->Add(DensityLabelText, 1, labelFlags, 5);
	sizerLeftUnitsColumn->Add(UnitOfDensity, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfDensity, additionalWidth);

	// Power
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_POWER_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_POWER)i));

	wxStaticText *PowerLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of power"));
	UnitOfPower = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_POWER),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerRightLabelColumn->Add(PowerLabelText, 1, labelFlags, 5);
	sizerRightUnitsColumn->Add(UnitOfPower, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfPower, additionalWidth);

	// Energy
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_ENERGY_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_ENERGY)i));

	wxStaticText *EnergyLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of energy"));
	UnitOfEnergy = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_ENERGY),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerLeftLabelColumn->Add(EnergyLabelText, 1, labelFlags, 5);
	sizerLeftUnitsColumn->Add(UnitOfEnergy, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfEnergy, additionalWidth);

	// Temperature
	Choices.Clear();
	for (i = 0; i < CONVERT::NUMBER_OF_TEMPERATURE_UNITS; i++)
		Choices.Add(Converter.GetUnits((CONVERT::UNITS_OF_TEMPERATURE)i));

	wxStaticText *TemperatureLabelText = new wxStaticText(UnitsPage, wxID_STATIC, _T("Units of temperature"));
	UnitOfTemperature = new wxComboBox(UnitsPage, wxID_ANY, Converter.GetUnitType(CONVERT::UNIT_TYPE_TEMPERATURE),
		wxDefaultPosition, wxDefaultSize, Choices, wxCB_READONLY);
	sizerRightLabelColumn->Add(TemperatureLabelText, 1, labelFlags, 5);
	sizerRightUnitsColumn->Add(UnitOfTemperature, 0, unitFlags, 5);
	SetMinimumWidthFromContents(UnitOfTemperature, additionalWidth);

	// Add the sizers to the MainSizer
	wxBoxSizer *columnSizer = new wxBoxSizer(wxHORIZONTAL);
	columnSizer->Add(sizerLeftLabelColumn, 1, wxEXPAND);
	columnSizer->Add(sizerLeftUnitsColumn, 0.5, 0);
	columnSizer->AddSpacer(20);
	columnSizer->Add(sizerRightLabelColumn, 1, wxEXPAND);
	columnSizer->Add(sizerRightUnitsColumn, 0.5, 0);
	UnitsSizer->Add(columnSizer);

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
	ValueString.Printf("%0.3f", KinematicInputs.CenterOfRotation.X);
	CenterOfRotationX = new wxTextCtrl(KinematicsPage, wxID_ANY, ValueString, wxDefaultPosition,
		wxSize(inputColumnWidth, -1));
	ValueString.Printf("%0.3f", KinematicInputs.CenterOfRotation.Y);
	CenterOfRotationY = new wxTextCtrl(KinematicsPage, wxID_ANY, ValueString, wxDefaultPosition,
		wxSize(inputColumnWidth, -1));
	ValueString.Printf("%0.3f", KinematicInputs.CenterOfRotation.Z);
	CenterOfRotationZ = new wxTextCtrl(KinematicsPage, wxID_ANY, ValueString, wxDefaultPosition,
		wxSize(inputColumnWidth, -1));

	// Create the center of rotation units label
	wxStaticText *CoRUnits = new wxStaticText(KinematicsPage, wxID_ANY, _T("(") + 
		Converter.GetUnitType(CONVERT::UNIT_TYPE_DISTANCE) + _T(")"));

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
	if (KinematicInputs.FirstRotation == VECTOR::AxisX)
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
	if(Debugger.GetDebugLevel() == DEBUGGER::PriorityLow)
		DebugLevel->SetSelection(0);
	else if(Debugger.GetDebugLevel() == DEBUGGER::PriorityMedium)
		DebugLevel->SetSelection(1);
	else if(Debugger.GetDebugLevel() == DEBUGGER::PriorityHigh)
		DebugLevel->SetSelection(2);
	else// debugger.:PriorityVeryHigh
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
	VECTOR CenterOfRotation;
	if (CenterOfRotationX->GetValue().ToDouble(&CenterOfRotation.X) &&
		CenterOfRotationY->GetValue().ToDouble(&CenterOfRotation.Y) &&
		CenterOfRotationZ->GetValue().ToDouble(&CenterOfRotation.Z))
		// Convert the center of rotation and update the analysis object
		KinematicInputs.CenterOfRotation = Converter.ConvertDistance(CenterOfRotation);
	else
		// Print a warning so the user knows the CoR was rejected
		Debugger.Print(_T("Warning!  Center of rotation is not a valid vector - using previous value"),
			DEBUGGER::PriorityHigh);

	// Set the order of rotations
	if (RotationOrder->GetSelection() == 0)
		KinematicInputs.FirstRotation = VECTOR::AxisX;
	else
		KinematicInputs.FirstRotation = VECTOR::AxisY;

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
		Debugger.Print(DEBUGGER::PriorityHigh,
			"Warning!  Specified number of threads is not valid (must be a number greater than zero) - using previous value of %i",
			MainFrame.GetNumberOfThreads());

	// Update the default units for the converter object
	// NOTE:  This section MUST come after the center of rotation is updated in order for
	// the units label on the center of rotation input to have been correct
	Converter.SetAngleUnits((CONVERT::UNITS_OF_ANGLE)SafelyGetComboBoxSelection(UnitOfAngle));
	Converter.SetDistanceUnits((CONVERT::UNITS_OF_DISTANCE)SafelyGetComboBoxSelection(UnitOfDistance));
	Converter.SetForceUnits((CONVERT::UNITS_OF_FORCE)SafelyGetComboBoxSelection(UnitOfForce));
	Converter.SetAreaUnits((CONVERT::UNITS_OF_AREA)SafelyGetComboBoxSelection(UnitOfArea));
	Converter.SetPressureUnits((CONVERT::UNITS_OF_PRESSURE)SafelyGetComboBoxSelection(UnitOfPressure));
	Converter.SetMomentUnits((CONVERT::UNITS_OF_MOMENT)SafelyGetComboBoxSelection(UnitOfMoment));
	Converter.SetMassUnits((CONVERT::UNITS_OF_MASS)SafelyGetComboBoxSelection(UnitOfMass));
	Converter.SetVelocityUnits((CONVERT::UNITS_OF_VELOCITY)SafelyGetComboBoxSelection(UnitOfVelocity));
	Converter.SetAccelerationUnits((CONVERT::UNITS_OF_ACCELERATION)SafelyGetComboBoxSelection(UnitOfAcceleration));
	Converter.SetInertiaUnits((CONVERT::UNITS_OF_INERTIA)SafelyGetComboBoxSelection(UnitOfInertia));
	Converter.SetDensityUnits((CONVERT::UNITS_OF_DENSITY)SafelyGetComboBoxSelection(UnitOfDensity));
	Converter.SetPowerUnits((CONVERT::UNITS_OF_POWER)SafelyGetComboBoxSelection(UnitOfPower));
	Converter.SetEnergyUnits((CONVERT::UNITS_OF_ENERGY)SafelyGetComboBoxSelection(UnitOfEnergy));
	Converter.SetTemperatureUnits((CONVERT::UNITS_OF_TEMPERATURE)SafelyGetComboBoxSelection(UnitOfTemperature));

	// Update the number of digits and the rules for formatting numbers
	Converter.SetNumberOfDigits(SafelyGetComboBoxSelection(NumberOfDigits));
	Converter.SetUseSignificantDigits(UseSignificantDigits->GetValue());
	Converter.SetUseScientificNotation(UseScientificNotation->GetValue());

	// Update the debug level
	if (DebugLevel->GetSelection() == 0)
		Debugger.SetDebugLevel(DEBUGGER::PriorityLow);
	else if (DebugLevel->GetSelection() == 1)
		Debugger.SetDebugLevel(DEBUGGER::PriorityMedium);
	else if (DebugLevel->GetSelection() == 2)
		Debugger.SetDebugLevel(DEBUGGER::PriorityHigh);
	else
		Debugger.SetDebugLevel(DEBUGGER::PriorityVeryHigh);

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
