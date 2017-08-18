/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSweepPlotsPanel.h
// Date:  11/14/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the EditSweepPlotsPanel class.

#ifndef EDIT_SWEEP_PLOTS_PANEL_H_
#define EDIT_SWEEP_PLOTS_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

// Local forward declarations
class Sweep;
class EditSweepNotebook;

class EditSweepPlotsPanel : public wxPanel
{
public:
	EditSweepPlotsPanel(EditSweepNotebook &parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size);
	~EditSweepPlotsPanel();

	// Updates the information on the panel
	void UpdateInformation(Sweep *currentSweep);
	void UpdateInformation();

private:
	EditSweepNotebook &parent;

	Sweep *currentSweep;

	void CreateControls();

	// The controls on this panel
	wxCheckListBox *plotListCheckBox;

	enum EditSweepPlotsPanelEventIds
	{
		PlotSelectionCheckList = 800 + wxID_HIGHEST
	};

	// Event handlers-----------------------------------------------------
	void PlotSelectionChangeEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	DECLARE_EVENT_TABLE();
};

}// namespace VVASE

#endif// EDIT_SWEEP_PLOTS_PANEL_H_
