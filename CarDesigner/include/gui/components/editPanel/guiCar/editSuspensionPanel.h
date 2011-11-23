/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editSuspensionPanel.h
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_SUSPENSION_PANEL class.
// History:

#ifndef _EDIT_SUSPENSION_PANEL_H_
#define _EDIT_SUSPENSION_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// wxWidgets forward declarations
class wxCombo;
class wxGridEvent;

// VVASE forward declarations
class Debugger;
class Suspension;
class Convert;
class Vector;
class SuperGrid;
class EditSuspensionNotebook;

class EditSuspensionPanel : public wxScrolledWindow
{
public:
	// Constructor
	EditSuspensionPanel(EditSuspensionNotebook &_parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EditSuspensionPanel();

	// Updates the information on the panel
	void UpdateInformation(Suspension *_currentSuspension);
	void UpdateInformation(void);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The application's converter
	const Convert &converter;

	// The parent panel
	EditSuspensionNotebook &parent;

	// The suspension with which we are currently associated
	Suspension *currentSuspension;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// This helps avoid unnecessary function calls to update the screen
	int lastRowSelected;

	// Enumeration for event IDs
	enum EditSuspensionEventId
	{
		CheckBoxIsSymmetric = 300 + wxID_HIGHEST,
		CheckBoxFrontHasThirdSpring,
		CheckBoxRearHasThirdSpring,

		ComboBoxFrontBarStyle,
		ComboBoxRearBarStyle
	};

	// Event handlers-----------------------------------------------------
	void SelectCellEvent(wxGridEvent &event);
	void GridCellChangedEvent(wxGridEvent &event);
	void SymmetricCheckboxEvent(wxCommandEvent &event);
	void FrontThirdCheckboxEvent(wxCommandEvent &event);
	void RearThirdCheckboxEvent(wxCommandEvent &event);
	void FrontBarStyleChangeEvent(wxCommandEvent &event);
	void RearBarStyleChangeEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// The text-entry control
	SuperGrid *hardpoints;

	// Checkboxes
	wxCheckBox *isSymmetric;
	wxCheckBox *frontHasThirdSpring;
	wxCheckBox *rearHasThirdSpring;

	// Style drop-down menus
	wxComboBox *frontBarStyle;
	wxComboBox *rearBarStyle;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_SUSPENSION_PANEL_H_