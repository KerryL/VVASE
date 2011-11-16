/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationNotebook.h
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_ITERATION_NOTEBOOK class.
// History:

#ifndef _EDIT_ITERATION_NOTEBOOK_H_
#define _EDIT_ITERATION_NOTEBOOK_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class Debugger;
class ITERATION;
class EDIT_PANEL;
class EDIT_ITERATION_RANGE_PANEL;
class EDIT_ITERATION_PLOTS_PANEL;
class EDIT_ITERATION_OPTIONS_PANEL;

class EDIT_ITERATION_NOTEBOOK : public wxNotebook
{
public:
	// Constructor
	EDIT_ITERATION_NOTEBOOK(EDIT_PANEL &_Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, long style, const Debugger &_debugger);

	// Destructor
	~EDIT_ITERATION_NOTEBOOK();

	// Returns access to this object's parent
	inline EDIT_PANEL &GetParent() { return Parent; };

	// Updates the information on the panel
	void UpdateInformation(ITERATION *_CurrentIteration);
	void UpdateInformation(void);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The parent window
	EDIT_PANEL &Parent;

	// The object with which we are currently associated
	ITERATION *CurrentIteration;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// The different notebook tabs
	EDIT_ITERATION_RANGE_PANEL *EditRange;
	EDIT_ITERATION_PLOTS_PANEL *EditPlots;
	EDIT_ITERATION_OPTIONS_PANEL *EditOptions;
};

#endif// _EDIT_ITERATION_NOTEBOOK_H_