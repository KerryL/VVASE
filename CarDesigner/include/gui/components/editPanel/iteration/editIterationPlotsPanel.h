/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationPlotsPanel.h
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class declaration for the EditIterationPlotsPanel class.
// History:

#ifndef EDIT_ITERATION_PLOTS_PANEL_H_
#define EDIT_ITERATION_PLOTS_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class Iteration;
class EditIterationNotebook;

class EditIterationPlotsPanel : public wxPanel
{
public:
	EditIterationPlotsPanel(EditIterationNotebook &parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size);
	~EditIterationPlotsPanel();

	// Updates the information on the panel
	void UpdateInformation(Iteration *currentIteration);
	void UpdateInformation();

private:
	EditIterationNotebook &parent;

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

#endif// EDIT_ITERATION_PLOTS_PANEL_H_