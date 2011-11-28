/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  optionsDialog.cpp
// Created:  2/9/2009
// Author:  K. Loux
// Description:  Contains the definition for the OptionsDialog class.
// History:
//	3/24/2009	- Added Kinematics tab, K. Loux.

// Standard C++ headers
#include <cfloat>

// wxWidgets headers
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/radiobox.h>
#include <wx/fontdlg.h>

// CarDesigner headers
#include "gui/dialogs/optionsDialog.h"
#include "gui/components/mainFrame.h"
#include "vUtilities/convert.h"
#include "vUtilities/wxRelatedUtilities.h"

//==========================================================================
// Class:			OptionsDialog
// Function:		OptionsDialog
//
// Description:		Constructor for OptionsDialog class.
//
// Input Arguments:
//		_mainFrame			= MainFrame& pointing to the application's main window
//		_converter			= Convert& pointing to the application's converter object
//		_kinematicInputs	= Kinematics::Inputs& reference to the application's analysis options
//		id					= wxWindowId for this object
//		position			= const wxPoint& where this object will be drawn
//		_debugger			= Debugger& pointing to this application's debug
//							  printing utility
//		style				= long defining the style for this dialog
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
OptionsDialog::OptionsDialog(MainFrame &_mainFrame, Convert &_converter,
							   Kinematics::Inputs &_kinematicInputs, wxWindowID id,
							   const wxPoint &position, Debugger &_debugger, long style)
							   : wxDialog(&_mainFrame, id, _T("Options"), position, wxDefaultSize, style),
							   debugger(_debugger), converter(_converter), kinematicInputs(_kinematicInputs),
							   mainFrame(_mainFrame)
{
	// Set up the form's layout
	CreateControls();

	// Center the dialog on the screen
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
void OptionsDialog::CreateControls(void)
{
	// Top-level sizer
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
	for (i = 0; i < Convert::NUMBER_OF_ANGLE_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfAngle)i));

	wxStaticText *angleLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of angle"));
	unitOfAngle = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypeAngle),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfAngle, additionalWidth);
	unitSelectionSizer->Add(angleLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfAngle, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Distance
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_DISTANCE_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfDistance)i));

	wxStaticText *DistanceLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of distance"));
	unitOfDistance = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypeDistance),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfDistance, additionalWidth);
	unitSelectionSizer->Add(DistanceLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfDistance, 0, unitFlags);

	// Area
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_AREA_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfArea)i));

	wxStaticText *AreaLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of area"));
	unitOfArea = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypeArea),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfArea, additionalWidth);
	unitSelectionSizer->Add(AreaLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfArea, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Force
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_FORCE_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfForce)i));

	wxStaticText *ForceLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of force"));
	unitOfForce = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypeForce),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfForce, additionalWidth);
	unitSelectionSizer->Add(ForceLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfForce, 0, unitFlags);

	// Pressure
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_PRESSURE_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfPressure)i));

	wxStaticText *PressureLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of pressure"));
	unitOfPressure = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypePressure),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfPressure, additionalWidth);
	unitSelectionSizer->Add(PressureLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfPressure, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Moment
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_MOMENT_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfMoment)i));

	wxStaticText *MomentLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of moment"));
	unitOfMoment = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypeMoment),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfMoment, additionalWidth);
	unitSelectionSizer->Add(MomentLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfMoment, 0, unitFlags);

	// Mass
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_MASS_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfMass)i));

	wxStaticText *MassLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of mass"));
	unitOfMass = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypeMass),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfMass, additionalWidth);
	unitSelectionSizer->Add(MassLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfMass, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Velocity
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_VELOCITY_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfVelocity)i));

	wxStaticText *VelocityLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of velocity"));
	unitOfVelocity = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypeVelocity),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfVelocity, additionalWidth);
	unitSelectionSizer->Add(VelocityLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfVelocity, 0, unitFlags);

	// Acceleration
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_ACCELERATION_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfAcceleration)i));

	wxStaticText *AccelerationLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of acceleration"));
	unitOfAcceleration = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypeAcceleration),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfAcceleration, additionalWidth);
	unitSelectionSizer->Add(AccelerationLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfAcceleration, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Inertia
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_INERTIA_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfInertia)i));

	wxStaticText *InertiaLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of inertia"));
	unitOfInertia = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypeInertia),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfInertia, additionalWidth);
	unitSelectionSizer->Add(InertiaLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfInertia, 0, unitFlags);

	// Density
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_DENSITY_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfDensity)i));

	wxStaticText *DensityLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of density"));
	unitOfDensity = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypeDensity),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfDensity, additionalWidth);
	unitSelectionSizer->Add(DensityLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfDensity, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Power
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_POWER_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfPower)i));

	wxStaticText *PowerLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of power"));
	unitOfPower = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypePower),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfPower, additionalWidth);
	unitSelectionSizer->Add(PowerLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfPower, 0, unitFlags);

	// Energy
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_ENERGY_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfEnergy)i));

	wxStaticText *EnergyLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of energy"));
	unitOfEnergy = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypeEnergy),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfEnergy, additionalWidth);
	unitSelectionSizer->Add(EnergyLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfEnergy, 0, unitFlags);
	unitSelectionSizer->AddSpacer(middleSpaceWidth);

	// Temperature
	choices.Clear();
	for (i = 0; i < Convert::NUMBER_OF_TEMPERATURE_UNITS; i++)
		choices.Add(converter.GetUnits((Convert::UnitsOfTemperature)i));

	wxStaticText *TemperatureLabelText = new wxStaticText(unitsPage, wxID_STATIC, _T("Units of temperature"));
	unitOfTemperature = new wxComboBox(unitsPage, wxID_ANY, converter.GetUnitType(Convert::UnitTypeTemperature),
		wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	SetMinimumWidthFromContents(unitOfTemperature, additionalWidth);
	unitSelectionSizer->Add(TemperatureLabelText, 0, labelFlags);
	unitSelectionSizer->Add(unitOfTemperature, 0, unitFlags);

	// Set the unit page's sizer
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
	numberString.Printf("%i", converter.GetNumberOfDigits());
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
	useSignificantDigits->SetValue(converter.GetUseSignificantDigits());
	useScientificNotation->SetValue(converter.GetUseScientificNotation());

	// Add the checkboxes to the control
	digitsSizer->Add(useSignificantDigits, 0, wxALIGN_LEFT | wxALL, 5);
	digitsSizer->Add(useScientificNotation, 0, wxALIGN_LEFT | wxALL, 5);

	// Set the digits page's sizer
	digitsPage->SetSizerAndFit(digitsTopSizer);

	// Add the Kinematic Analysis page
	kinematicsPage = new wxPanel(notebook);
	notebook->AddPage(kinematicsPage, _T("Kinematics"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *kinematicsTopSizer = new wxBoxSizer(wxVERTICAL);

	// Create the kinematics page main sizer
	wxBoxSizer *kinematicsSizer = new wxBoxSizer(wxVERTICAL);
	kinematicsTopSizer->Add(kinematicsSizer, 0,
		wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the center of rotation inputs
	wxBoxSizer *corSizer = new wxBoxSizer(wxHORIZONTAL);
	wxString valueString;
	valueString.Printf("%0.3f", kinematicInputs.centerOfRotation.x);
	centerOfRotationX = new wxTextCtrl(kinematicsPage, wxID_ANY, valueString);
	valueString.Printf("%0.3f", kinematicInputs.centerOfRotation.y);
	centerOfRotationY = new wxTextCtrl(kinematicsPage, wxID_ANY, valueString);
	valueString.Printf("%0.3f", kinematicInputs.centerOfRotation.z);
	centerOfRotationZ = new wxTextCtrl(kinematicsPage, wxID_ANY, valueString);

	// Create the center of rotation units label
	wxStaticText *corUnits = new wxStaticText(kinematicsPage, wxID_ANY, _T("(") + 
		converter.GetUnitType(Convert::UnitTypeDistance) + _T(")"));

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
		
	// Add the radio box to the sizer
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
		
	// Add the radio box to the sizer
	radioOptionsSizer->Add(steeringInputType, 0, wxALL, 5);

	// Create the "Number of Threads" text box
	wxBoxSizer *numberOfThreadsSizer = new wxBoxSizer(wxHORIZONTAL);
	valueString.Printf("%i", mainFrame.GetNumberOfThreads());
	simultaneousThreads = new wxTextCtrl(kinematicsPage, wxID_ANY, valueString);
	numberOfThreadsSizer->Add(new wxStaticText(kinematicsPage, wxID_ANY,
		_T("Number of Simultaneous Threads to Execute")), 0,
		wxALIGN_CENTER_VERTICAL | wxALL, 5);
	numberOfThreadsSizer->Add(simultaneousThreads, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	// Add the sizers to the MainSizer
	kinematicsSizer->Add(corSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	kinematicsSizer->Add(radioOptionsSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	kinematicsSizer->Add(numberOfThreadsSizer, 0, wxALIGN_BOTTOM | wxALIGN_CENTER_HORIZONTAL);

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
	if(debugger.GetDebugLevel() == Debugger::PriorityLow)
		debugLevel->SetSelection(0);
	else if(debugger.GetDebugLevel() == Debugger::PriorityMedium)
		debugLevel->SetSelection(1);
	else if(debugger.GetDebugLevel() == Debugger::PriorityHigh)
		debugLevel->SetSelection(2);
	else// Debugger::PriorityVeryHigh
		debugLevel->SetSelection(3);
		
	// Add the radio box to the sizer
	debugLevelsSizer->Add(debugLevel, 0, wxALL, 5);

	// Add the sizers to the MainSizer
	debuggerSizer->Add(debugLevelsSizer, 0, wxALIGN_CENTER_HORIZONTAL);

	// Set the debug page's sizer
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
	wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *okButton = new wxButton(this, wxID_OK, _T("OK"));
	buttonsSizer->Add(okButton, 0, wxALL, 5);
	buttonsSizer->Add(new wxButton(this, wxID_CANCEL, _T("Cancel")), 0, wxALL, 5);
	mainSizer->Add(buttonsSizer, 0, wxALIGN_CENTER_HORIZONTAL);

	// Make the OK button default
	okButton->SetDefault();

	// Tell the dialog to auto-adjust it's size
	topSizer->SetSizeHints(this);

	// Assign the top level sizer to the dialog
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
	// Update the center of rotation for the kinematic analysis object
	// It is important that this is done before the converter is updated!
	// FIXME:  This should use a textvalidator and it will be much cleaner
	Vector centerOfRotation;
	if (centerOfRotationX->GetValue().ToDouble(&centerOfRotation.x) &&
		centerOfRotationY->GetValue().ToDouble(&centerOfRotation.y) &&
		centerOfRotationZ->GetValue().ToDouble(&centerOfRotation.z))
		// Convert the center of rotation and update the analysis object
		kinematicInputs.centerOfRotation = converter.ConvertDistance(centerOfRotation);
	else
		// Print a warning so the user knows the CoR was rejected
		debugger.Print(_T("Warning!  Center of rotation is not a valid vector - using previous value"),
			Debugger::PriorityHigh);

	// Set the order of rotations
	if (rotationOrder->GetSelection() == 0)
		kinematicInputs.firstRotation = Vector::AxisX;
	else
		kinematicInputs.firstRotation = Vector::AxisY;

	// Set the steering input type
	if (steeringInputType->GetSelection() == 0)
		mainFrame.SetUseRackTravel(true);
	else
		mainFrame.SetUseRackTravel(false);

	// Set the number of threads to use in the solver
	long numberOfThreads(0);
	if (simultaneousThreads->GetValue().ToLong(&numberOfThreads) && numberOfThreads > 0)
		mainFrame.SetNumberOfThreads(numberOfThreads);
	else
		debugger.Print(Debugger::PriorityHigh,
			"Warning!  Specified number of threads is not valid (must be a number greater than zero) - using previous value of %i",
			mainFrame.GetNumberOfThreads());

	// Update the default units for the converter object
	// NOTE:  This section MUST come after the center of rotation is updated in order for
	// the units label on the center of rotation input to have been correct
	converter.SetAngleUnits((Convert::UnitsOfAngle)SafelyGetComboBoxSelection(unitOfAngle));
	converter.SetDistanceUnits((Convert::UnitsOfDistance)SafelyGetComboBoxSelection(unitOfDistance));
	converter.SetForceUnits((Convert::UnitsOfForce)SafelyGetComboBoxSelection(unitOfForce));
	converter.SetAreaUnits((Convert::UnitsOfArea)SafelyGetComboBoxSelection(unitOfArea));
	converter.SetPressureUnits((Convert::UnitsOfPressure)SafelyGetComboBoxSelection(unitOfPressure));
	converter.SetMomentUnits((Convert::UnitsOfMoment)SafelyGetComboBoxSelection(unitOfMoment));
	converter.SetMassUnits((Convert::UnitsOfMass)SafelyGetComboBoxSelection(unitOfMass));
	converter.SetVelocityUnits((Convert::UnitsOfVelocity)SafelyGetComboBoxSelection(unitOfVelocity));
	converter.SetAccelerationUnits((Convert::UnitsOfAcceleration)SafelyGetComboBoxSelection(unitOfAcceleration));
	converter.SetInertiaUnits((Convert::UnitsOfInertia)SafelyGetComboBoxSelection(unitOfInertia));
	converter.SetDensityUnits((Convert::UnitsOfDensity)SafelyGetComboBoxSelection(unitOfDensity));
	converter.SetPowerUnits((Convert::UnitsOfPower)SafelyGetComboBoxSelection(unitOfPower));
	converter.SetEnergyUnits((Convert::UnitsOfEnergy)SafelyGetComboBoxSelection(unitOfEnergy));
	converter.SetTemperatureUnits((Convert::UnitsOfTemperature)SafelyGetComboBoxSelection(unitOfTemperature));

	// Update the number of digits and the rules for formatting numbers
	converter.SetNumberOfDigits(SafelyGetComboBoxSelection(numberOfDigits));
	converter.SetUseSignificantDigits(useSignificantDigits->GetValue());
	converter.SetUseScientificNotation(useScientificNotation->GetValue());

	// Update the debug level
	if (debugLevel->GetSelection() == 0)
		debugger.SetDebugLevel(Debugger::PriorityLow);
	else if (debugLevel->GetSelection() == 1)
		debugger.SetDebugLevel(Debugger::PriorityMedium);
	else if (debugLevel->GetSelection() == 2)
		debugger.SetDebugLevel(Debugger::PriorityHigh);
	else
		debugger.SetDebugLevel(Debugger::PriorityVeryHigh);
	
	// Update the fonts
	mainFrame.SetOutputFont(outputFont);
	mainFrame.SetPlotFont(plotFont);

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