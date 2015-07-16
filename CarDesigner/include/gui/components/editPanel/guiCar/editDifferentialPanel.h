/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editDifferentialPanel.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EditDifferentialPanel class.
// History:

#ifndef EDIT_DIFFERENTIAL_PANEL_H_
#define EDIT_DIFFERENTIAL_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

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

#endif// EDIT_DIFFERENTIAL_PANEL_H_