/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationPlotsPanel.h
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class declaration for the EditIterationPlotsPanel class.
// History:

#ifndef _EDIT_ITERATION_PLOTS_PANEL_H_
#define _EDIT_ITERATION_PLOTS_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class Iteration;
class EditIterationNotebook;

class EditIterationPlotsPanel : public wxPanel
{
public:
	// Constructor
	EditIterationPlotsPanel(EditIterationNotebook &_parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size);

	// Destructor
	~EditIterationPlotsPanel();

	// Updates the information on the panel
	void UpdateInformation(Iteration *_currentIteration);
	void UpdateInformation(void);

private:
	// The parent panel
	EditIterationNotebook &parent;

	// The iteration with which we are currently associated
	Iteration *currentIteration;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// The controls on this panel
	wxCheckListBox *plotListCheckBox;

	// Enumeration for event IDs
	enum EditIterationPlotsPanelEventIds
	{
		PlotSelectionCheckList = 800 + wxID_HIGHEST
	};

	// Event handlers-----------------------------------------------------
	void PlotSelectionChangeEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _EDIT_ITERATION_PLOTS_PANEL_H_