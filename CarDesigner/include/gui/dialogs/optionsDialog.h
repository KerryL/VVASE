/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  optionsDialog.h
// Created:  2/9/2009
// Author:  K. Loux
// Description:  Contains the declaration for the OPTION_DIALOG class.
// History:

#ifndef _OPTIONS_DIALOG_H_
#define _OPTIONS_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vSolver/physics/kinematics.h"

// wxWidgets forward declarations
class wxNotebook;
class wxRadioBox;

// VVASE forward declarations
class Convert;
class MainFrame;
class Debugger;

class OptionsDialog : public wxDialog
{
public:
	// Constructor
	OptionsDialog(MainFrame &_mainFrame, Convert &_converter, Kinematics::Inputs &_kinematicInputs,
		wxWindowID id, const wxPoint &position, Debugger &_debugger, long style = wxDEFAULT_DIALOG_STYLE);

	// Destructor
	~OptionsDialog();

private:
	// Debugger printing utility
	Debugger &debugger;

	// The object that handles the unit conversions between the input and output
	Convert &converter;

	// The object that contains the options for the kinematic analysis
	Kinematics::Inputs &kinematicInputs;

	// The parent window
	MainFrame &mainFrame;

	// Sets up the size and position of this dialog and its contents
	void CreateControls(void);

	// Event handlers-----------------------------------------------------
	void OKClickEvent(wxCommandEvent &event);
	void CancelClickEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// The controls
	// Main notebook
	wxNotebook *notebook;

	// UNITS page
	wxPanel *unitsPage;
	wxComboBox *unitOfAngle;
	wxComboBox *unitOfDistance;
	wxComboBox *unitOfArea;
	wxComboBox *unitOfForce;
	wxComboBox *unitOfPressure;
	wxComboBox *unitOfMoment;
	wxComboBox *unitOfMass;
	wxComboBox *unitOfVelocity;
	wxComboBox *unitOfAcceleration;
	wxComboBox *unitOfInertia;
	wxComboBox *unitOfDensity;
	wxComboBox *unitOfPower;
	wxComboBox *unitOfEnergy;
	wxComboBox *unitOfTemperature;

	// DIGITS page
	wxPanel *digitsPage;
	wxComboBox *numberOfDigits;
	wxCheckBox *useSignificantDigits;
	wxCheckBox *useScientificNotation;

	// KINEMATIC ANALYSIS page
	wxPanel *kinematicsPage;
	wxRadioBox *rotationOrder;
	wxRadioBox *steeringInputType;
	wxTextCtrl *centerOfRotationX;
	wxTextCtrl *centerOfRotationY;
	wxTextCtrl *centerOfRotationZ;
	wxTextCtrl *simultaneousThreads;

	// DEBUG page
	wxPanel *debuggerPage;
	wxRadioBox *debugLevel;

	// The event table
	DECLARE_EVENT_TABLE()
};

#endif// _OPTIONS_DIALOG_H_