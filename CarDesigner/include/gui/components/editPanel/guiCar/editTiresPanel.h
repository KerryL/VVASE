/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editTiresPanel.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EditTiresPanel class.
// History:

#ifndef _EDIT_TIRES_PANEL_H_
#define _EDIT_TIRES_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EditPanel;
class TireSet;

class EditTiresPanel : public wxScrolledWindow
{
public:
	// Constructor
	EditTiresPanel(EditPanel &_parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size);

	// Destructor
	~EditTiresPanel();

	// Updates the information in the display
	void UpdateInformation(TireSet *_tireSet);

private:
	// The parent panel
	EditPanel &parent;

	// The corner with which we are currently associated
	TireSet *currentTireSet;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// Event IDs
	enum EditTiresEventIds
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
	wxTextCtrl *rightFrontTireDiameter;
	wxTextCtrl *rightFrontTireWidth;
	wxStaticText *rightFrontDiameterUnitsLabel;
	wxStaticText *rightFrontWidthUnitsLabel;

	wxTextCtrl *leftFrontTireDiameter;
	wxTextCtrl *leftFrontTireWidth;
	wxStaticText *leftFrontDiameterUnitsLabel;
	wxStaticText *leftFrontWidthUnitsLabel;

	wxTextCtrl *rightRearTireDiameter;
	wxTextCtrl *rightRearTireWidth;
	wxStaticText *rightRearDiameterUnitsLabel;
	wxStaticText *rightRearWidthUnitsLabel;

	wxTextCtrl *leftRearTireDiameter;
	wxTextCtrl *leftRearTireWidth;
	wxStaticText *leftRearDiameterUnitsLabel;
	wxStaticText *leftRearWidthUnitsLabel;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_TIRES_PANEL_H_