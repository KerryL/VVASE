/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  editDrivetrainPanel.h
// Date:  2/19/2009
// Author:  K. Loux
// Desc:  Contains the class declaration for the EditDrivetrainPanel class.

#ifndef EDIT_DRIVETRAIN_PANEL_H_
#define EDIT_DRIVETRAIN_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

// Local forward declarations
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

}// namespace VVASE

#endif// EDIT_DRIVETRAIN_PANEL_H_
