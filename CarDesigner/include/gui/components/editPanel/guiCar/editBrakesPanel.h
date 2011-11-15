/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editBrakesPanel.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_BRAKES_PANEL class.
// History:

#ifndef _EDIT_BRAKES_PANEL_CLASS_
#define _EDIT_BRAKES_PANEL_CLASS_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EDIT_PANEL;
class BRAKES;

class EDIT_BRAKES_PANEL : public wxScrolledWindow
{
public:
	// Constructor
	EDIT_BRAKES_PANEL(EDIT_PANEL &_Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EDIT_BRAKES_PANEL();

	// Updates the information on the panel
	void UpdateInformation(BRAKES *_CurrentBrakes);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The application's converter
	const Convert &Converter;

	// The parent panel
	EDIT_PANEL &Parent;

	// The data with which we are currently associated
	BRAKES *CurrentBrakes;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// Event IDs
	enum EDIT_BRAKES_EVENT_ID
	{
		TextBoxPercentFrontBraking = 600 + wxID_HIGHEST,
		
		CheckBoxFrontBrakesInboard,
		CheckBoxRearBrakesInboard
	};

	// Event handlers-----------------------------------------------------
	void TextBoxEditEvent(wxCommandEvent &event);
	void CheckBoxChange(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// The text box controls
	wxTextCtrl *PercentFrontBraking;

	// The check boxes
	wxCheckBox *FrontBrakesInboard;
	wxCheckBox *RearBrakesInboard;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_BRAKES_PANEL_CLASS_