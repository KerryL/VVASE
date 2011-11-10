/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationOptionsPanel.h
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_ITERATION_RANGE_PANEL class.
// History:

#ifndef _EDIT_ITERATION_RANGE_PANEL_CLASS_H_
#define _EDIT_ITERATION_RANGE_PANEL_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class Debugger;
class ITERATION;
class Convert;
class EDIT_ITERATION_NOTEBOOK;

class EDIT_ITERATION_RANGE_PANEL : public wxPanel
{
public:
	// Constructor
	EDIT_ITERATION_RANGE_PANEL(EDIT_ITERATION_NOTEBOOK &_Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EDIT_ITERATION_RANGE_PANEL();

	// Updates the information on the panel
	void UpdateInformation(ITERATION *_CurrentIteration);
	void UpdateInformation(void);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The application's converter
	const Convert &Converter;

	// The parent panel
	EDIT_ITERATION_NOTEBOOK &Parent;

	// The iteration with which we are currently associated
	ITERATION *CurrentIteration;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// The text boxes
	wxTextCtrl *StartPitchInput;
	wxTextCtrl *StartRollInput;
	wxTextCtrl *StartHeaveInput;
	wxTextCtrl *StartSteerInput;
	wxTextCtrl *EndPitchInput;
	wxTextCtrl *EndRollInput;
	wxTextCtrl *EndHeaveInput;
	wxTextCtrl *EndSteerInput;
	wxTextCtrl *NumberOfPointsInput;

	// Enumeration for event IDs
	enum EDIT_ITERATION_OPTIONS_EVENT_ID
	{
		RangeTextBox = 700 + wxID_HIGHEST
	};

	// Event handlers-----------------------------------------------------
	void RangeTextBoxChangeEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_ITERATION_RANGE_PANEL_CLASS_H_