/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editBrakesPanel.h
// Date:  2/19/2009
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the EditBrakesPanel class.

#ifndef EDIT_BRAKES_PANEL_CLASS_
#define EDIT_BRAKES_PANEL_CLASS_

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

// Local forward declarations
class EditPanel;
class Brakes;

class EditBrakesPanel : public wxScrolledWindow
{
public:
	EditBrakesPanel(EditPanel &parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size);
	~EditBrakesPanel();

	void UpdateInformation(Brakes *currentBrakes);

private:
	EditPanel &parent;

	// The data with which we are currently associated
	Brakes *currentBrakes;

	// Creates the controls and positions everything within the panel
	void CreateControls();

	// Event IDs
	enum EditBrakesEventIds
	{
		TextBoxPercentFrontBraking = 600 + wxID_HIGHEST,

		CheckBoxFrontBrakesInboard,
		CheckBoxRearBrakesInboard
	};

	// Event handlers-----------------------------------------------------
	void TextBoxEditEvent(wxCommandEvent &event);
	void CheckBoxChange(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// The text box controls
	wxTextCtrl *percentFrontBraking;

	// The check boxes
	wxCheckBox *frontBrakesInboard;
	wxCheckBox *rearBrakesInboard;

	DECLARE_EVENT_TABLE();
};

}// namespace VVASE

#endif// EDIT_BRAKES_PANEL_CLASS_
