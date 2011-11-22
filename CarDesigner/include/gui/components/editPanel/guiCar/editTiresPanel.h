/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editTiresPanel.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_TIRES_PANEL class.
// History:

#ifndef _EDIT_TIRES_PANEL_H_
#define _EDIT_TIRES_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EDIT_PANEL;
class TireSet;

class EDIT_TIRES_PANEL : public wxScrolledWindow
{
public:
	// Constructor
	EDIT_TIRES_PANEL(EDIT_PANEL &_Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EDIT_TIRES_PANEL();

	// Updates the information in the display
	void UpdateInformation(TireSet *_TireSet);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The application's converter
	const Convert &Converter;

	// The parent panel
	EDIT_PANEL &Parent;

	// The corner with which we are currently associated
	TireSet *CurrentTireSet;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// Event IDs
	enum EDIT_CORNER_EVENT_ID
	{
		TextBoxRightFrontTireDiameter = 500 + wxID_HIGHEST,
		TextBoxRightFrontTireWidth,

		TextBoxLeftFrontTireDiameter,
		TextBoxLeftFrontTireWidth,

		TextBoxRightRearTireDiameter,
		TextBoxRightRearTireWidth,

		TextBoxLeftRearTireDiameter,
		TextBoxLeftRearTireWidth
	};

	// Event handlers-----------------------------------------------------
	void RightFrontTireDiameterChangeEvent(wxCommandEvent &event);
	void RightFrontTireWidthChangeEvent(wxCommandEvent &event);

	void LeftFrontTireDiameterChangeEvent(wxCommandEvent &event);
	void LeftFrontTireWidthChangeEvent(wxCommandEvent &event);

	void RightRearTireDiameterChangeEvent(wxCommandEvent &event);
	void RightRearTireWidthChangeEvent(wxCommandEvent &event);

	void LeftRearTireDiameterChangeEvent(wxCommandEvent &event);
	void LeftRearTireWidthChangeEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// The controls for the static-setup options
	wxTextCtrl *RightFrontTireDiameter;
	wxTextCtrl *RightFrontTireWidth;
	wxStaticText *RightFrontDiameterUnitsLabel;
	wxStaticText *RightFrontWidthUnitsLabel;

	wxTextCtrl *LeftFrontTireDiameter;
	wxTextCtrl *LeftFrontTireWidth;
	wxStaticText *LeftFrontDiameterUnitsLabel;
	wxStaticText *LeftFrontWidthUnitsLabel;

	wxTextCtrl *RightRearTireDiameter;
	wxTextCtrl *RightRearTireWidth;
	wxStaticText *RightRearDiameterUnitsLabel;
	wxStaticText *RightRearWidthUnitsLabel;

	wxTextCtrl *LeftRearTireDiameter;
	wxTextCtrl *LeftRearTireWidth;
	wxStaticText *LeftRearDiameterUnitsLabel;
	wxStaticText *LeftRearWidthUnitsLabel;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_TIRES_PANEL_H_