/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editBrakesPanel.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EditBrakesPanel class.
// History:

#ifndef _EDIT_BRAKES_PANEL_CLASS_
#define _EDIT_BRAKES_PANEL_CLASS_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EditPanel;
class Brakes;

class EditBrakesPanel : public wxScrolledWindow
{
public:
	// Constructor
	EditBrakesPanel(EditPanel &_parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EditBrakesPanel();

	// Updates the information on the panel
	void UpdateInformation(Brakes *_currentBrakes);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The application's converter
	const Convert &converter;

	// The parent panel
	EditPanel &parent;

	// The data with which we are currently associated
	Brakes *currentBrakes;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// Event IDs
	enum EditBrakesEventIds
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
	wxTextCtrl *percentFrontBraking;

	// The check boxes
	wxCheckBox *frontBrakesInboard;
	wxCheckBox *rearBrakesInboard;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_BRAKES_PANEL_CLASS_