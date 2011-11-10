/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_drivetrain_panel_class.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_DRIVETRAIN_PANEL class.
// History:

#ifndef _EDIT_DRIVETRAIN_PANEL_CLASS_H_
#define _EDIT_DRIVETRAIN_PANEL_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EDIT_PANEL;

class EDIT_DRIVETRAIN_PANEL : public wxPanel
{
public:
	// Constructor
	EDIT_DRIVETRAIN_PANEL(EDIT_PANEL* _Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EDIT_DRIVETRAIN_PANEL();

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);
};

#endif// _EDIT_DRIVETRAIN_PANEL_CLASS_H_