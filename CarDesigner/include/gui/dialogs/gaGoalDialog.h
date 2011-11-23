/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  gaGoalDialog.h
// Created:  7/30/2009
// Author:  K. Loux
// Description:  Dialog for editing goal properties.
// History:

// wxWidgets headers
#include <wx/wx.h>

// CarDesigner headers
#include "vSolver/physics/kinematicOutputs.h"
#include "vSolver/physics/kinematics.h"
#include "vUtilities/convert.h"

// CarDesigner forward declarations
class MainFrame;

class GAGoalDialog : public wxDialog
{
public:
	// Constructor
	GAGoalDialog(wxWindow *parent, const Convert &_converter, const KinematicOutputs::OutputsComplete &_output,
		const double &_desiredValue, const double &_expectedDeviation, const double &_importance,
		const Kinematics::Inputs &_beforeInputs, const Kinematics::Inputs &_afterInputs,
		wxWindowID id, const wxPoint &position, long style = wxDEFAULT_DIALOG_STYLE);

	// Destructor
	~GAGoalDialog();

	// Private member accessors
	KinematicOutputs::OutputsComplete GetOutput(void) const { return output; };
	double GetDesiredValue(void) const { return desiredValue; };
	double GetExpectedDeviation(void) const { return expectedDeviation; };
	double GetImportance(void) const { return importance; };
	Kinematics::Inputs GetBeforeInputs(void) const { return beforeInputs; };
	Kinematics::Inputs GetAfterInputs(void) const { return afterInputs; };

private:
	// The object that handles the unit conversions between the input and output
	const Convert &converter;

	// Method for creating controls
	void CreateControls(void);

	// Updates some controls when user clicks the checkbox
	void FormatDialogDifference(void);

	// Controls
	wxComboBox *outputCombo;

	wxTextCtrl *beforePitchText;
	wxTextCtrl *beforeRollText;
	wxTextCtrl *beforeHeaveText;
	wxTextCtrl *beforeSteerText;

	wxTextCtrl *afterPitchText;
	wxTextCtrl *afterRollText;
	wxTextCtrl *afterHeaveText;
	wxTextCtrl *afterSteerText;

	wxTextCtrl *desiredValueText;
	wxTextCtrl *deviationText;
	wxTextCtrl *importanceText;

	wxCheckBox *difference;

	wxStaticText *desiredValueLabel;
	wxStaticText *desiredValueUnitsLabel;
	wxStaticText *deviationUnitsLabel;
	wxStaticText *beforeLabel;
	wxStaticText *afterLabel;

	// Values (populated when OK is clicked)
	KinematicOutputs::OutputsComplete output;

	double desiredValue;
	double expectedDeviation;
	double importance;

	Kinematics::Inputs beforeInputs;
	Kinematics::Inputs afterInputs;

	// Event handlers
	virtual void OKClickEvent(wxCommandEvent &event);
	virtual void CancelClickEvent(wxCommandEvent &event);
	virtual void OnCheckEvent(wxCommandEvent &event);
	virtual void OnOutputChangeEvent(wxCommandEvent &event);

	// For the event table
	DECLARE_EVENT_TABLE();
};