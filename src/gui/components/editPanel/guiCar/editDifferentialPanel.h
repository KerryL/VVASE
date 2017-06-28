/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  editDifferentialPanel.h
// Date:  2/19/2009
// Author:  K. Loux
// Desc:  Contains the class declaration for the EditDifferentialPanel class.

#ifndef EDIT_DIFFERENTIAL_PANEL_H_
#define EDIT_DIFFERENTIAL_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

// VVASE forward declarations
class EditPanel;
class Debugger;

class EditDifferentialPanel : public wxPanel
{
public:
	EditDifferentialPanel(EditPanel* parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &debugger);
	~EditDifferentialPanel();

private:
	const Debugger &debugger;

	void CreateControls();
};

}// namespace VVASE

#endif// EDIT_DIFFERENTIAL_PANEL_H_
