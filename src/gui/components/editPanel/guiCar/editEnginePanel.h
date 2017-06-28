/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  editEnginePanel.h
// Date:  2/19/2009
// Author:  K. Loux
// Desc:  Contains the class declaration for the EditEnginePanel class.

#ifndef EDIT_ENGINE_PANEL_H_
#define EDIT_ENGINE_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

// Local forward declarations
class EditPanel;
class Debugger;

class EditEnginePanel : public wxPanel
{
public:
	EditEnginePanel(EditPanel* parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &debugger);
	~EditEnginePanel();

private:
	const Debugger &debugger;

	void CreateControls();
};

}// namespace VVASE

#endif// EDIT_ENGINE_PANEL_H_
