/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editTiresPanel.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EditTiresPanel class.
// History:

#ifndef EDIT_TIRES_PANEL_H_
#define EDIT_TIRES_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EditPanel;
class TireSet;

class EditTiresPanel : public wxScrolledWindow
{
public:
	EditTiresPanel(EditPanel &parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size);
	~EditTiresPanel();

	void UpdateInformation(TireSet *tireSet);

private:
	EditPanel &parent;

	TireSet *currentTireSet;

	void CreateControls();

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

	DECLARE_EVENT_TABLE();
};

#endif// EDIT_TIRES_PANEL_H_