/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  ga_goal_dialog_class.h
// Created:  7/30/2009
// Author:  K. Loux
// Description:  Dialog for editing goal properties.
// History:

// wxWidgets headers
#include <wx/wx.h>

// CarDesigner headers
#include "vSolver/physics/kinematic_outputs_class.h"
#include "vSolver/physics/kinematics_class.h"
#include "vUtilities/convert_class.h"

// CarDesigner forward declarations
class MAIN_FRAME;

class GA_GOAL_DIALOG : public wxDialog
{
public:
	// Constructor
	GA_GOAL_DIALOG(wxWindow *Parent, const CONVERT &_Converter, const KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE &_Output,
		const double &_DesiredValue, const double &_ExpectedDeviation, const double &_Importance,
		const KINEMATICS::INPUTS &_BeforeInputs, const KINEMATICS::INPUTS &_AfterInputs,
		wxWindowID Id, const wxPoint &Position, long Style = wxDEFAULT_DIALOG_STYLE);

	// Destructor
	~GA_GOAL_DIALOG();

	// Private member accessors
	KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE GetOutput(void) const { return Output; };
	double GetDesiredValue(void) const { return DesiredValue; };
	double GetExpectedDeviation(void) const { return ExpectedDeviation; };
	double GetImportance(void) const { return Importance; };
	KINEMATICS::INPUTS GetBeforeInputs(void) const { return BeforeInputs; };
	KINEMATICS::INPUTS GetAfterInputs(void) const { return AfterInputs; };

private:
	// The object that handles the unit conversions between the input and output
	const CONVERT &Converter;

	// Method for creating controls
	void CreateControls(void);

	// Updates some controls when user clicks the checkbox
	void FormatDialogDifference(void);

	// Controls
	wxComboBox *OutputCombo;

	wxTextCtrl *BeforePitchText;
	wxTextCtrl *BeforeRollText;
	wxTextCtrl *BeforeHeaveText;
	wxTextCtrl *BeforeSteerText;

	wxTextCtrl *AfterPitchText;
	wxTextCtrl *AfterRollText;
	wxTextCtrl *AfterHeaveText;
	wxTextCtrl *AfterSteerText;

	wxTextCtrl *DesiredValueText;
	wxTextCtrl *DeviationText;
	wxTextCtrl *ImportanceText;

	wxCheckBox *Difference;

	wxStaticText *DesiredValueLabel;
	wxStaticText *DesiredValueUnitsLabel;
	wxStaticText *DeviationUnitsLabel;
	wxStaticText *BeforeLabel;
	wxStaticText *AfterLabel;

	// Spacers for the components that are hidden/shown
	wxStaticText *BeforeLabelSpacer;
	wxStaticText *AfterLabelSpacer;
	wxStaticText *AfterPitchSpacer;
	wxStaticText *AfterRollSpacer;
	wxStaticText *AfterHeaveSpacer;
	wxStaticText *AfterSteerSpacer;

	// Values (populated when OK is clicked)
	KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE Output;

	double DesiredValue;
	double ExpectedDeviation;
	double Importance;

	KINEMATICS::INPUTS BeforeInputs;
	KINEMATICS::INPUTS AfterInputs;

	// Event handlers
	virtual void OKClickEvent(wxCommandEvent &event);
	virtual void CancelClickEvent(wxCommandEvent &event);
	virtual void OnCheckEvent(wxCommandEvent &event);
	virtual void OnOutputChangeEvent(wxCommandEvent &event);

	// For the event table
	DECLARE_EVENT_TABLE();
};