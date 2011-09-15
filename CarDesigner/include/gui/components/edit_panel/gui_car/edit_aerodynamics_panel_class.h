/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_aerodynamics_panel_class.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_AERODYNAMICS_PANEL class.
// History:

#ifndef _EDIT_AERODYNAMICS_PANEL_CLASS_H_
#define _EDIT_AERODYNAMICS_PANEL_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EDIT_PANEL;

class EDIT_AERODYNAMICS_PANEL : public wxPanel
{
public:
	// Constructor
	EDIT_AERODYNAMICS_PANEL(EDIT_PANEL* _Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const DEBUGGER &_Debugger);

	// Destructor
	~EDIT_AERODYNAMICS_PANEL();

private:
	// Debugger message printing utility
	const DEBUGGER &Debugger;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);
};

#endif// _EDIT_AERODYNAMICS_PANEL_CLASS_H_