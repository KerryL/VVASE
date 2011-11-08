/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  options_dialog_class.h
// Created:  2/9/2009
// Author:  K. Loux
// Description:  Contains the declaration for the OPTION_DIALOG class.
// History:

#ifndef _OPTIONS_DIALOG_CLASS_H_
#define _OPTIONS_DIALOG_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vSolver/physics/kinematics_class.h"

// wxWidgets forward declarations
class wxNotebook;
class wxRadioBox;

// VVASE forward declarations
class Convert;
class MAIN_FRAME;
class Debugger;

class OPTIONS_DIALOG : public wxDialog
{
public:
	// Constructor
	OPTIONS_DIALOG(MAIN_FRAME &_MainFrame, Convert &_Converter, KINEMATICS::INPUTS &_KinematicInputs,
		wxWindowID Id, const wxPoint &Position, Debugger &_debugger, long Style = wxDEFAULT_DIALOG_STYLE);

	// Destructor
	~OPTIONS_DIALOG();

private:
	// Debugger printing utility
	Debugger &debugger;

	// The object that handles the unit conversions between the input and output
	Convert &Converter;

	// The object that contains the options for the kinematic analysis
	KINEMATICS::INPUTS &KinematicInputs;

	// The parent window
	MAIN_FRAME &MainFrame;

	// Sets up the size and position of this dialog and its contents
	void CreateControls(void);

	// Event handlers-----------------------------------------------------
	void OKClickEvent(wxCommandEvent &event);
	void CancelClickEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// The controls
	// Main notebook
	wxNotebook *Notebook;

	// UNITS page
	wxPanel *UnitsPage;
	wxComboBox *UnitOfAngle;
	wxComboBox *UnitOfDistance;
	wxComboBox *UnitOfArea;
	wxComboBox *UnitOfForce;
	wxComboBox *UnitOfPressure;
	wxComboBox *UnitOfMoment;
	wxComboBox *UnitOfMass;
	wxComboBox *UnitOfVelocity;
	wxComboBox *UnitOfAcceleration;
	wxComboBox *UnitOfInertia;
	wxComboBox *UnitOfDensity;
	wxComboBox *UnitOfPower;
	wxComboBox *UnitOfEnergy;
	wxComboBox *UnitOfTemperature;

	// DIGITS page
	wxPanel *DigitsPage;
	wxComboBox *NumberOfDigits;
	wxCheckBox *UseSignificantDigits;
	wxCheckBox *UseScientificNotation;

	// KINEMATIC ANALYSIS page
	wxPanel *KinematicsPage;
	wxRadioBox *RotationOrder;
	wxRadioBox *SteeringInputType;
	wxTextCtrl *CenterOfRotationX;
	wxTextCtrl *CenterOfRotationY;
	wxTextCtrl *CenterOfRotationZ;
	wxTextCtrl *SimultaneousThreads;

	// DEBUG page
	wxPanel *DebuggerPage;
	wxRadioBox *DebugLevel;

	// The event table
	DECLARE_EVENT_TABLE()
};

#endif// _OPTIONS_DIALOG_CLASS_H_