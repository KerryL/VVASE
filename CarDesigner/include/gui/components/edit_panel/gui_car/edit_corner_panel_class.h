/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_corner_panel_class.h
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_CORNER_PANEL class.
// History:

#ifndef _EDIT_CORNER_PANEL_CLASS_H_
#define _EDIT_CORNER_PANEL_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/grid.h>

// VVASE headers
#include "vCar/suspension_class.h"
#include "vCar/drivetrain_class.h"

// wxWidgets forward declarations
class wxCombo;

// VVASE forward declarations
class DEBUGGER;
class CONVERT;
class CORNER;
class EDIT_SUSPENSION_NOTEBOOK;

class EDIT_CORNER_PANEL : public wxPanel
{
public:
	// Constructor
	EDIT_CORNER_PANEL(EDIT_SUSPENSION_NOTEBOOK &_Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const DEBUGGER &_Debugger);

	// Destructor
	~EDIT_CORNER_PANEL();

	// Updates the information on the panel
	void UpdateInformation(CORNER *_CurrentCorner, SUSPENSION::BAR_STYLE BarStyle, bool HasHalfShaft);

private:
	// Debugger message printing utility
	const DEBUGGER &Debugger;

	// The application's converter
	const CONVERT &Converter;

	// The parent panel
	EDIT_SUSPENSION_NOTEBOOK &Parent;

	// The corner with which we are currently associated
	CORNER *CurrentCorner;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// This helps avoid unnecessary function calls to update the screen
	int LastRowSelected;

	// Event IDs
	enum EDIT_CORNER_EVENT_ID
	{
		ComboBoxActuationAttachment = 200 + wxID_HIGHEST,
		ComboBoxActuationType,

		TextBoxStaticCamber,
		TextBoxStaticToe
	};

	// Event handlers-----------------------------------------------------
	void SelectCellEvent(wxGridEvent &event);
	void GridCellChangedEvent(wxGridEvent &event);
	void ActuationAttachmentChangeEvent(wxCommandEvent &event);
	void ActuationTypeChangeEvent(wxCommandEvent &event);
	void StaticCamberChangeEvent(wxCommandEvent &event);
	void StaticToeChangeEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// The pushrod attachment options (and 'do we want a pushrod?')
	wxComboBox *ActuationAttachment;
	wxComboBox *ActuationType;

	// The text-entry control
	wxGrid *Hardpoints;

	// The controls for the static-setup options
	wxTextCtrl *StaticCamber;
	wxTextCtrl *StaticToe;
	wxStaticText *CamberUnitsLabel;
	wxStaticText *ToeUnitsLabel;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_CORNER_PANEL_CLASS_H_