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

// wxWidgets forward declarations
class wxGridEvent;

// VVASE forward declarations
class EditPanel;
class MassProperties;
class SuperGrid;

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

		TextBoxCornerMassLeftFront,
		TextBoxCornerMassRightFront,
		TextBoxCornerMassLeftRear,
		TextBoxCornerMassRightRear,
		
		TextBoxUnsprungMassLeftFront,
		TextBoxUnsprungMassRightFront,
		TextBoxUnsprungMassLeftRear,
		TextBoxUnsprungMassRightRear
	};

	enum GridRows
	{
		RowUnits,
		RowTotalCG,
		RowUnsprungLeftFrontCG,
		RowUnsprungRightFrontCG,
		RowUnsprungLeftRearCG,
		RowUnsprungRightRearCG,

		GridRowCount
	};

	// Event handlers-----------------------------------------------------
	void TextBoxEditEvent(wxCommandEvent &event);
	void GridCellChangeEvent(wxGridEvent &event);
	// End event handlers-------------------------------------------------

	// The text box controls
	wxTextCtrl *ixx;
	wxTextCtrl *iyy;
	wxTextCtrl *izz;
	wxTextCtrl *ixy;
	wxTextCtrl *ixz;
	wxTextCtrl *iyz;

	wxTextCtrl *cornerMassLeftFront;
	wxTextCtrl *cornerMassRightFront;
	wxTextCtrl *cornerMassLeftRear;
	wxTextCtrl *cornerMassRightRear;
	
	wxTextCtrl *unsprungMassLeftFront;
	wxTextCtrl *unsprungMassRightFront;
	wxTextCtrl *unsprungMassLeftRear;
	wxTextCtrl *unsprungMassRightRear;

	SuperGrid *cgHeights;

	// The static text controls
	wxStaticText *inertiaUnitsLabel;
	wxStaticText *cornerMassLeftFrontUnitsLabel;
	wxStaticText *cornerMassRightFrontUnitsLabel;
	wxStaticText *cornerMassLeftRearUnitsLabel;
	wxStaticText *cornerMassRightRearUnitsLabel;
	wxStaticText *unsprungMassLeftFrontUnitsLabel;
	wxStaticText *unsprungMassRightFrontUnitsLabel;
	wxStaticText *unsprungMassLeftRearUnitsLabel;
	wxStaticText *unsprungMassRightRearUnitsLabel;

	wxStaticText *iyx;
	wxStaticText *izx;
	wxStaticText *izy;

	DECLARE_EVENT_TABLE();
};

#endif// EDIT_MASS_PANEL_H_