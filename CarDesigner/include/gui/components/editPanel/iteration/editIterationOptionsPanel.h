/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationOptionsPanel.h
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_ITERATION_OPTIONS_PANEL class.
// History:

#ifndef _EDIT_ITERATION_OPTIONS_PANEL_H_
#define _EDIT_ITERATION_OPTIONS_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class Iteration;
class Convert;
class EditIterationNotebook;

class EditIterationOptionsPanel : public wxScrolledWindow
{
public:
	// Constructor
	EditIterationOptionsPanel(EditIterationNotebook &_parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size);

	// Destructor
	~EditIterationOptionsPanel();

	// Updates the information on the panel
	void UpdateInformation(Iteration *_currentIteration);
	void UpdateInformation(void);

private:
	// The parent panel
	EditIterationNotebook &parent;

	// The iteration with which we are currently associated
	Iteration *currentIteration;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// Controls on this panel
	wxCheckBox *showGridLines;
	wxCheckBox *autoLabelXAxis;
	wxCheckBox *autoLabelZAxis;
	wxCheckBox *autoTitle;

	wxTextCtrl *titleText;
	wxTextCtrl *xLabelText;
	wxTextCtrl *zLabelText;

	wxButton *setAsDefault;

	// Enumeration for event IDs
	enum EditIterationOptionsEventIds
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

#endif// _EDIT_ITERATION_OPTIONS_PANEL_H_