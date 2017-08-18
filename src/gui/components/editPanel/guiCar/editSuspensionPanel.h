/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSuspensionPanel.h
// Date:  2/10/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the EditSuspensionPanel class.

#ifndef EDIT_SUSPENSION_PANEL_H_
#define EDIT_SUSPENSION_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// wxWidgets forward declarations
class wxCombo;
class wxGridEvent;

namespace VVASE
{

// Local forward declarations
class Suspension;
class SuperGrid;
class EditSuspensionNotebook;

class EditSuspensionPanel : public wxScrolledWindow
{
public:
	EditSuspensionPanel(EditSuspensionNotebook &parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size);
	~EditSuspensionPanel();

	void UpdateInformation(Suspension *currentSuspension);
	void UpdateInformation();

private:
	EditSuspensionNotebook &parent;

	Suspension *currentSuspension;

	void CreateControls();

	// This helps avoid unnecessary function calls to update the screen
	int lastRowSelected;

	// Enumeration for event IDs
	enum EditSuspensionEventId
	{
		CheckBoxIsSymmetric = 300 + wxID_HIGHEST,
		CheckBoxFrontHasThirdSpring,
		CheckBoxRearHasThirdSpring,

		ComboBoxFrontBarStyle,
		ComboBoxRearBarStyle
	};

	// Event handlers-----------------------------------------------------
	void SelectCellEvent(wxGridEvent &event);
	void GridCellChangedEvent(wxGridEvent &event);
	void SymmetricCheckboxEvent(wxCommandEvent &event);
	void FrontThirdCheckboxEvent(wxCommandEvent &event);
	void RearThirdCheckboxEvent(wxCommandEvent &event);
	void FrontBarStyleChangeEvent(wxCommandEvent &event);
	void RearBarStyleChangeEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	SuperGrid *hardpoints;

	// Checkboxes
	wxCheckBox *isSymmetric;
	wxCheckBox *frontHasThirdSpring;
	wxCheckBox *rearHasThirdSpring;

	// Style drop-down menus
	wxComboBox *frontBarStyle;
	wxComboBox *rearBarStyle;

	void SetFront3rdSpringEnable();
	void SetRear3rdSpringEnable();

	DECLARE_EVENT_TABLE();
};

}// namespace VVASE

#endif// EDIT_SUSPENSION_PANEL_H_
