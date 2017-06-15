/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editEnginePanel.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_ENGINE_PANEL class.
// History:

#ifndef EDIT_ENGINE_PANEL_H_
#define EDIT_ENGINE_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
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

#endif// EDIT_ENGINE_PANEL_H_