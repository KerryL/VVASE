/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editEnginePanel.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_ENGINE_PANEL class.
// History:

#ifndef _EDIT_ENGINE_PANEL_H_
#define _EDIT_ENGINE_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EditPanel;

class EditEnginePanel : public wxPanel
{
public:
	// Constructor
	EditEnginePanel(EditPanel* _parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EditEnginePanel();

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);
};

#endif// _EDIT_ENGINE_PANEL_H_