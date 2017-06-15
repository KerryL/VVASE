/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  optionsDialog.h
// Created:  2/9/2009
// Author:  K. Loux
// Description:  Contains the declaration for the OptionsDialog class.
// History:

#ifndef OPTIONS_DIALOG_H_
#define OPTIONS_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vSolver/physics/kinematics.h"

// wxWidgets forward declarations
class wxNotebook;
class wxRadioBox;

// VVASE forward declarations
class MainFrame;

class OptionsDialog : public wxDialog
{
public:
	OptionsDialog(MainFrame &mainFrame, Kinematics::Inputs &kinematicInputs,
		wxWindowID id, const wxPoint &position, long style = wxDEFAULT_DIALOG_STYLE);
	~OptionsDialog();

private:
	Kinematics::Inputs &kinematicInputs;
	MainFrame &mainFrame;

	// Sets up the size and position of this dialog and its contents
	void CreateControls();
	
	enum OptionsDialogEventIds
	{
		ChangeOutputFontButton = 1500 + wxID_HIGHEST,
		ChangePlotFontButton
	};

	// Event handlers-----------------------------------------------------
	void OKClickEvent(wxCommandEvent &event);
	void CancelClickEvent(wxCommandEvent &event);
	void ChangeOutputFontClickEvent(wxCommandEvent &event);
	void ChangePlotFontClickEvent(wxCommandEvent &event);
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
	
	// FONTS page
	wxPanel *fontPage;
	wxStaticText *outputFontLabel;
	wxStaticText *plotFontLabel;
	wxFont outputFont;
	wxFont plotFont;

	DECLARE_EVENT_TABLE()
};

#endif// OPTIONS_DIALOG_H_