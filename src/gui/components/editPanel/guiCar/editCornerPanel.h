/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editCornerPanel.h
// Date:  2/10/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the EditCornerPanel class.

#ifndef EDIT_CORNER_PANEL_H_
#define EDIT_CORNER_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "VVASE/core/car/subsystems/suspension.h"
#include "VVASE/core/car/subsystems/drivetrain.h"

// wxWidgets forward declarations
class wxCombo;
class wxGridEvent;

namespace VVASE
{

// Local forward declarations
class Corner;
class EditSuspensionNotebook;
class SuperGrid;

class EditCornerPanel : public wxScrolledWindow
{
public:
	// Constructor
	EditCornerPanel(EditSuspensionNotebook &parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size);

	// Destructor
	~EditCornerPanel();

	// Updates the information on the panel
	void UpdateInformation(Corner *currentCorner, Suspension* currentSuspension,
		Suspension::BarStyle barStyle, bool hasHalfShaft);

private:
	// The parent panel
	EditSuspensionNotebook &parent;

	// The corner with which we are currently associated
	Corner *currentCorner;
	Suspension *currentSuspension;

	// Creates the controls and positions everything within the panel
	void CreateControls();

	// This helps avoid unnecessary function calls to update the screen
	int lastRowSelected;

	// Event IDs
	enum EditCornerEventIds
	{
		ComboBoxActuationAttachment = 200 + wxID_HIGHEST,
		ComboBoxActuationType,

		TextBoxStaticCamber,
		TextBoxStaticToe
	};

	// Event handlers-----------------------------------------------------
	void SelectCellEvent(wxGridEvent &event);
	void GridCellChangedEvent(wxGridEvent &event);
	void ActuationAttachmentChangeEvent(wxCommandEvent &event);
	void ActuationTypeChangeEvent(wxCommandEvent &event);
	void StaticCamberChangeEvent(wxCommandEvent &event);
	void StaticToeChangeEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// The pushrod attachment options (and 'do we want a pushrod?')
	wxComboBox *actuationAttachment;
	wxComboBox *actuationType;

	// The text-entry control
	SuperGrid *hardpoints;

	// The controls for the static-setup options
	wxTextCtrl *staticCamber;
	wxTextCtrl *staticToe;
	wxStaticText *camberUnitsLabel;
	wxStaticText *toeUnitsLabel;

	// For the event table
	DECLARE_EVENT_TABLE();
};

}// namespace VVASE

#endif// EDIT_CORNER_PANEL_H_
