/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editCornerPanel.h
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_CORNER_PANEL class.
// History:

#ifndef _EDIT_CORNER_PANEL_H_
#define _EDIT_CORNER_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vCar/suspension.h"
#include "vCar/drivetrain.h"

// wxWidgets forward declarations
class wxCombo;
class wxGridEvent;

// VVASE forward declarations
class Debugger;
class Convert;
class Corner;
class EDIT_SUSPENSION_NOTEBOOK;
class SuperGrid;

class EDIT_CORNER_PANEL : public wxScrolledWindow
{
public:
	// Constructor
	EDIT_CORNER_PANEL(EDIT_SUSPENSION_NOTEBOOK &_Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EDIT_CORNER_PANEL();

	// Updates the information on the panel
	void UpdateInformation(Corner *_CurrentCorner, Suspension::BarStyle BarStyle, bool HasHalfShaft);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The application's converter
	const Convert &Converter;

	// The parent panel
	EDIT_SUSPENSION_NOTEBOOK &Parent;

	// The corner with which we are currently associated
	Corner *CurrentCorner;

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
	SuperGrid *Hardpoints;

	// The controls for the static-setup options
	wxTextCtrl *StaticCamber;
	wxTextCtrl *StaticToe;
	wxStaticText *CamberUnitsLabel;
	wxStaticText *ToeUnitsLabel;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_CORNER_PANEL_H_