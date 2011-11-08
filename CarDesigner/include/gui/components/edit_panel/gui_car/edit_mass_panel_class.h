/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_mass_panel_class.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_MASS_PANEL class.
// History:

#ifndef _EDIT_MASS_PANEL_CLASS_H_
#define _EDIT_MASS_PANEL_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EDIT_PANEL;
class MASS_PROPERTIES;

class EDIT_MASS_PANEL : public wxPanel
{
public:
	// Constructor
	EDIT_MASS_PANEL(EDIT_PANEL &_Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EDIT_MASS_PANEL();

	// Updates the information on the panel
	void UpdateInformation(MASS_PROPERTIES *_CurrentMassProperties);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The application's converter
	const Convert &Converter;

	// The parent panel
	EDIT_PANEL &Parent;

	// The data with which we are currently associated
	MASS_PROPERTIES *CurrentMassProperties;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// Event IDs
	enum EDIT_MASS_PROPERTIES_EVENT_ID
	{
		TextBoxIxx = 400 + wxID_HIGHEST,
		TextBoxIyy,
		TextBoxIzz,
		TextBoxIxy,
		TextBoxIxz,
		TextBoxIyz,

		TextBoxMass,
		TextBoxCenterOfGravityX,
		TextBoxCenterOfGravityY,
		TextBoxCenterOfGravityZ
	};

	// Event handlers-----------------------------------------------------
	void TextBoxEditEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// The text box controls
	wxTextCtrl *Ixx;
	wxTextCtrl *Iyy;
	wxTextCtrl *Izz;
	wxTextCtrl *Ixy;
	wxTextCtrl *Ixz;
	wxTextCtrl *Iyz;

	wxTextCtrl *Mass;
	wxTextCtrl *CenterOfGravityX;
	wxTextCtrl *CenterOfGravityY;
	wxTextCtrl *CenterOfGravityZ;

	// The static text controls
	wxStaticText *InertiaUnitsLabel;
	wxStaticText *MassUnitsLabel;
	wxStaticText *CoGUnitsLabel;

	wxStaticText *Iyx;
	wxStaticText *Izx;
	wxStaticText *Izy;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_MASS_PANEL_CLASS_H_