/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationRangePanel.h
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class declaration for the EditIterationPanel class.
// History:

#ifndef EDIT_ITERATION_RANGE_PANEL_H_
#define EDIT_ITERATION_RANGE_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class Iteration;
class EditIterationNotebook;

class EditIterationRangePanel : public wxScrolledWindow
{
public:
	EditIterationRangePanel(EditIterationNotebook &parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size);
	~EditIterationRangePanel();

	void UpdateInformation(Iteration *currentIteration);
	void UpdateInformation();

private:
	EditIterationNotebook &parent;

	Iteration *currentIteration;
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

	enum EditIterationRangePanelEventIds
	{
		RangeTextBox = wxID_HIGHEST + 1300
	};

	// Event handlers-----------------------------------------------------
	void RangeTextBoxChangeEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	DECLARE_EVENT_TABLE();
};

#endif// EDIT_ITERATION_RANGE_PANEL_H_