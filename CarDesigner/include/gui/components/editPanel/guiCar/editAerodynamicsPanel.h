/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editAerodynamicsPanel.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_AERODYNAMICS_PANEL class.
// History:

#ifndef EDIT_AERODYNAMICS_PANEL_H_
#define EDIT_AERODYNAMICS_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EditPanel;
class Debugger;

class EditAerodynamicsPanel : public wxPanel
{
public:
	EditAerodynamicsPanel(EditPanel* parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &debugger);
	~EditAerodynamicsPanel();

private:
	const Debugger &debugger;

	void CreateControls();
};

#endif// EDIT_AERODYNAMICS_PANEL_H_