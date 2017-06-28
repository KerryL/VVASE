/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editDrivetrainPanel.h
// Date:  2/19/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
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
