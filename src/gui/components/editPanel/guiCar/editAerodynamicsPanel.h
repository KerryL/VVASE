/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  editAerodynamicsPanel.h
// Date:  2/19/2009
// Author:  K. Loux
// Desc:  Contains the class declaration for the EDIT_AERODYNAMICS_PANEL class.

#ifndef EDIT_AERODYNAMICS_PANEL_H_
#define EDIT_AERODYNAMICS_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

// Local forward declarations
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

}// namespace VVASE

#endif// EDIT_AERODYNAMICS_PANEL_H_
