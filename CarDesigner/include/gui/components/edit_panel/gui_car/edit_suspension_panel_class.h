/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_suspension_panel_class.h
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_SUSPENSION_PANEL class.
// History:

#ifndef _EDIT_SUSPENSION_PANEL_CLASS_H_
#define _EDIT_SUSPENSION_PANEL_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/grid.h>

// wxWidgets forward declarations
class wxCombo;

// VVASE forward declarations
class Debugger;
class SUSPENSION;
class Convert;
class Vector;
class EDIT_SUSPENSION_NOTEBOOK;

class EDIT_SUSPENSION_PANEL : public wxPanel
{
public:
	// Constructor
	EDIT_SUSPENSION_PANEL(EDIT_SUSPENSION_NOTEBOOK &_Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EDIT_SUSPENSION_PANEL();

	// Updates the information on the panel
	void UpdateInformation(SUSPENSION *_CurrentSuspension);
	void UpdateInformation(void);

private:
	// Debugger message printing utility
	const Debugger &Debugger;

	// The application's converter
	const Convert &Converter;

	// The parent panel
	EDIT_SUSPENSION_NOTEBOOK &Parent;

	// The suspension with which we are currently associated
	SUSPENSION *CurrentSuspension;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// This helps avoid unnecessary function calls to update the screen
	int LastRowSelected;

	// Enumeration for event IDs
	enum EDIT_SUSPENSION_GENERAL_EVENT_ID
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
	wxGrid *Hardpoints;

	// Checkboxes
	wxCheckBox *IsSymmetric;
	wxCheckBox *FrontHasThirdSpring;
	wxCheckBox *RearHasThirdSpring;

	// Style drop-down menus
	wxComboBox *FrontBarStyle;
	wxComboBox *RearBarStyle;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_SUSPENSION_PANEL_CLASS_H_