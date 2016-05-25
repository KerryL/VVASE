/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editMassPanel.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EditMassPanel class.
// History:

#ifndef EDIT_MASS_PANEL_H_
#define EDIT_MASS_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EditPanel;
class MassProperties;

class EditMassPanel : public wxScrolledWindow
{
public:
	EditMassPanel(EditPanel &parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size);
	~EditMassPanel();

	void UpdateInformation(MassProperties *currentMassProperties);

private:
	EditPanel &parent;

	MassProperties *currentMassProperties;
	void CreateControls();

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

	DECLARE_EVENT_TABLE();
};

#endif// EDIT_MASS_PANEL_H_