/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationOptionsPanel.h
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_ITERATION_OPTIONS_PANEL class.
// History:

#ifndef EDIT_ITERATION_OPTIONS_PANEL_H_
#define EDIT_ITERATION_OPTIONS_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class Iteration;
class Convert;
class EditIterationNotebook;

class EditIterationOptionsPanel : public wxScrolledWindow
{
public:
	EditIterationOptionsPanel(EditIterationNotebook &parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size);
	~EditIterationOptionsPanel();

	// Updates the information on the panel
	void UpdateInformation(Iteration *currentIteration);
	void UpdateInformation();

private:
	EditIterationNotebook &parent;

	Iteration *currentIteration;

	void CreateControls();

	// Controls on this panel
	wxCheckBox *showGridLines;
	wxCheckBox *autoLabelXAxis;
	wxCheckBox *autoLabelZAxis;
	wxCheckBox *autoTitle;

	wxTextCtrl *titleText;
	wxTextCtrl *xLabelText;
	wxTextCtrl *zLabelText;

	wxButton *setAsDefault;

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

	DECLARE_EVENT_TABLE();
};

#endif// EDIT_ITERATION_OPTIONS_PANEL_H_