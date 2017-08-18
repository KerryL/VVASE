/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSweepRangePanel.h
// Date:  11/14/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the EditSweepRangePanel class.

#ifndef EDIT_SWEEP_RANGE_PANEL_H_
#define EDIT_SWEEP_RANGE_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

// Local forward declarations
class Sweep;
class EditSweepNotebook;

class EditSweepRangePanel : public wxScrolledWindow
{
public:
	EditSweepRangePanel(EditSweepNotebook &parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size);
	~EditSweepRangePanel();

	void UpdateInformation(Sweep *currentSweep);
	void UpdateInformation();

private:
	EditSweepNotebook &parent;

	Sweep *currentSweep;
	void CreateControls();

	// The text boxes
	wxTextCtrl *startPitchInput;
	wxTextCtrl *startRollInput;
	wxTextCtrl *startHeaveInput;
	wxTextCtrl *startSteerInput;
	wxTextCtrl *endPitchInput;
	wxTextCtrl *endRollInput;
	wxTextCtrl *endHeaveInput;
	wxTextCtrl *endSteerInput;
	wxTextCtrl *numberOfPointsInput;

	// The unit labels
	wxStaticText *pitchUnitsLabel;
	wxStaticText *rollUnitsLabel;
	wxStaticText *heaveUnitsLabel;
	wxStaticText *steerUnitsLabel;

	wxStaticText *steerInputLabel;

	enum EditSweepRangePanelEventIds
	{
		RangeTextBox = wxID_HIGHEST + 1300
	};

	// Event handlers-----------------------------------------------------
	void RangeTextBoxChangeEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	DECLARE_EVENT_TABLE();
};

}// namespace VVASE

#endif// EDIT_SWEEP_RANGE_PANEL_H_
