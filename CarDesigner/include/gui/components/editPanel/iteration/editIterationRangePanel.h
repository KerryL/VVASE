/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationRangePanel.h
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_ITERATION_RANGE_PANEL class.
// History:

#ifndef _EDIT_ITERATION_RANGE_PANEL_H_
#define _EDIT_ITERATION_RANGE_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class Debugger;
class Iteration;
class Convert;
class EditIterationNotebook;

class EditIterationRangePanel : public wxScrolledWindow
{
public:
	// Constructor
	EditIterationRangePanel(EditIterationNotebook &_parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EditIterationRangePanel();

	// Updates the information on the panel
	void UpdateInformation(Iteration *_currentIteration);
	void UpdateInformation(void);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The application's converter
	const Convert &converter;

	// The parent panel
	EditIterationNotebook &parent;

	// The iteration with which we are currently associated
	Iteration *currentIteration;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

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

	// Enumeration for event IDs
	enum EditIterationRangePanelEventIds
	{
		RangeTextBox = wxID_HIGHEST + 1300
	};

	// Event handlers-----------------------------------------------------
	void RangeTextBoxChangeEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_ITERATION_RANGE_PANEL_H_