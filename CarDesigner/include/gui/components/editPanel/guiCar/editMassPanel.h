/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editMassPanel.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_MASS_PANEL class.
// History:

#ifndef _EDIT_MASS_PANEL_H_
#define _EDIT_MASS_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EditPanel;
class MassProperties;

class EditMassPanel : public wxScrolledWindow
{
public:
	// Constructor
	EditMassPanel(EditPanel &_Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EditMassPanel();

	// Updates the information on the panel
	void UpdateInformation(MassProperties *_currentMassProperties);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The application's converter
	const Convert &converter;

	// The parent panel
	EditPanel &parent;

	// The data with which we are currently associated
	MassProperties *currentMassProperties;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// Event IDs
	enum EditMassPropertiesEventIds
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
	wxTextCtrl *ixx;
	wxTextCtrl *iyy;
	wxTextCtrl *izz;
	wxTextCtrl *ixy;
	wxTextCtrl *ixz;
	wxTextCtrl *iyz;

	wxTextCtrl *mass;
	wxTextCtrl *centerOfGravityX;
	wxTextCtrl *centerOfGravityY;
	wxTextCtrl *centerOfGravityZ;

	// The static text controls
	wxStaticText *inertiaUnitsLabel;
	wxStaticText *massUnitsLabel;
	wxStaticText *cogUnitsLabel;

	wxStaticText *iyx;
	wxStaticText *izx;
	wxStaticText *izy;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_MASS_PANEL_H_