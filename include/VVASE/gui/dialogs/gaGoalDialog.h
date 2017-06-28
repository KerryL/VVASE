/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  gaGoalDialog.h
// Date:  7/30/2009
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Dialog for editing goal properties.

#ifndef GA_GOAL_DIALOG_H_
#define GA_GOAL_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "vSolver/physics/kinematicOutputs.h"
#include "vSolver/physics/kinematics.h"

namespace VVASE
{

// Local forward declarations
class MainFrame;

class GAGoalDialog : public wxDialog
{
public:
	// Constructor
	GAGoalDialog(wxWindow *parent, const KinematicOutputs::OutputsComplete &output,
		const double &desiredValue, const double &expectedDeviation, const double &importance,
		const Kinematics::Inputs &beforeInputs, const Kinematics::Inputs &afterInputs,
		wxWindowID id, const wxPoint &position, long style = wxDEFAULT_DIALOG_STYLE);

	// Destructor
	~GAGoalDialog();

	// Private member accessors
	KinematicOutputs::OutputsComplete GetOutput() const { return output; };
	double GetDesiredValue() const { return desiredValue; };
	double GetExpectedDeviation() const { return expectedDeviation; };
	double GetImportance() const { return importance; };
	Kinematics::Inputs GetBeforeInputs() const { return beforeInputs; };
	Kinematics::Inputs GetAfterInputs() const { return afterInputs; };

private:
	// Method for creating controls
	void CreateControls();

	// Updates some controls when user clicks the checkbox
	void FormatDialogDifference();

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

}// namespace VVASE

#endif// GA_GOAL_DIALOG_H_
