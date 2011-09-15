/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_iteration_options_panel_class.h
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_ITERATION_OPTIONS_PANEL class.
// History:

#ifndef _EDIT_ITERATION_OPTIONS_PANEL_CLASS_H_
#define _EDIT_ITERATION_OPTIONS_PANEL_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class DEBUGGER;
class ITERATION;
class CONVERT;
class EDIT_ITERATION_NOTEBOOK;

class EDIT_ITERATION_OPTIONS_PANEL : public wxPanel
{
public:
	// Constructor
	EDIT_ITERATION_OPTIONS_PANEL(EDIT_ITERATION_NOTEBOOK &_Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const DEBUGGER &_Debugger);

	// Destructor
	~EDIT_ITERATION_OPTIONS_PANEL();

	// Updates the information on the panel
	void UpdateInformation(ITERATION *_CurrentIteration);
	void UpdateInformation(void);

private:
	// Debugger message printing utility
	const DEBUGGER &Debugger;

	// The parent panel
	EDIT_ITERATION_NOTEBOOK &Parent;

	// The iteration with which we are currently associated
	ITERATION *CurrentIteration;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// Controls on this panel
	wxCheckBox *ShowGridLines;
	wxCheckBox *AutoLabelXAxis;
	wxCheckBox *AutoLabelZAxis;
	wxCheckBox *AutoTitle;

	wxTextCtrl *TitleText;
	wxTextCtrl *XLabelText;
	wxTextCtrl *ZLabelText;

	wxButton *SetAsDefault;

	// Enumeration for event IDs
	enum EDIT_ITERATION_OPTIONS_EVENT_ID
	{
		CheckBoxIterationOptions = 900 + wxID_HIGHEST,
		TextBoxIterationOptions,
		ButtonSetAsDefault
	};

	// Event handlers-----------------------------------------------------
	void OptionsCheckBoxEvent(wxCommandEvent &event);
	void OptionsTextBoxEvent(wxCommandEvent &event);
	void SetAsDefaultClickedEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_ITERATION_OPTIONS_PANEL_CLASS_H_