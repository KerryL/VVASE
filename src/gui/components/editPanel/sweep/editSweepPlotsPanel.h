/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSweepPlotsPanel.h
// Date:  11/14/2010
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the EditSweepPlotsPanel class.

#ifndef EDIT_SWEEP_PLOTS_PANEL_H_
#define EDIT_SWEEP_PLOTS_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

// Local forward declarations
class Iteration;
class EditSweepNotebook;

class EditIterationPlotsPanel : public wxPanel
{
public:
	EditIterationPlotsPanel(EditSweepNotebook &parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size);
	~EditIterationPlotsPanel();

	// Updates the information on the panel
	void UpdateInformation(Iteration *currentIteration);
	void UpdateInformation();

private:
	EditSweepNotebook &parent;

	Iteration *currentIteration;

	void CreateControls();

	// The controls on this panel
	wxCheckListBox *plotListCheckBox;

	enum EditIterationPlotsPanelEventIds
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
