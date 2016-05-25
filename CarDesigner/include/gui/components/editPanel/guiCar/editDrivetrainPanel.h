/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editDrivetrainPanel.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_DRIVETRAIN_PANEL class.
// History:

#ifndef EDIT_DRIVETRAIN_PANEL_H_
#define EDIT_DRIVETRAIN_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class EditPanel;
class Debugger;

class EditDrivetrainPanel : public wxPanel
{
public:
	EditDrivetrainPanel(EditPanel* parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &debugger);
	~EditDrivetrainPanel();

private:
	const Debugger &debugger;

	void CreateControls();
};

#endif// EDIT_DRIVETRAIN_PANEL_H_