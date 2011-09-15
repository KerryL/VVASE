/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_iteration_notebook_class.cpp
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_ITERATION_NOTEBOOK class.
// History:

// VVASE headers
#include "gui/iteration_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/components/edit_panel/edit_panel_class.h"
#include "gui/components/edit_panel/iteration/edit_iteration_notebook_class.h"
#include "gui/components/edit_panel/iteration/edit_iteration_range_panel_class.h"
#include "gui/components/edit_panel/iteration/edit_iteration_plots_panel_class.h"
#include "gui/components/edit_panel/iteration/edit_iteration_options_panel_class.h"
#include "vUtilities/debug_class.h"

//==========================================================================
// Class:			EDIT_ITERATION_NOTEBOOK
// Function:		EDIT_ITERATION_NOTEBOOK
//
// Description:		Constructor for EDIT_ITERATION_NOTEBOOK class.  Initializes
//					the form and creates the controls, etc.
//
// Input Argurments:
//		_Parent		= EDIT_PANEL&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		style		= long for passing to parent class's constructor
//		_Debugger	= const DEBUGGER& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EDIT_ITERATION_NOTEBOOK::EDIT_ITERATION_NOTEBOOK(EDIT_PANEL &_Parent, wxWindowID id,
												 const wxPoint& pos, const wxSize& size,
												 long style, const DEBUGGER &_Debugger)
												 : wxNotebook(&_Parent, id, pos, size, style),
												 Debugger(_Debugger), Parent(_Parent)
{
	// Initialize the 'Current' class members
	CurrentIteration = NULL;
}

//==========================================================================
// Class:			EDIT_ITERATION_NOTEBOOK
// Function:		~EDIT_ITERATION_NOTEBOOK
//
// Description:		Destructor for EDIT_ITERATION_NOTEBOOK class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EDIT_ITERATION_NOTEBOOK::~EDIT_ITERATION_NOTEBOOK()
{
}

//==========================================================================
// Class:			EDIT_ITERATION_NOTEBOOK
// Function:		UpdateInformation
//
// Description:		Updates the information in this notebook for the current
//					object (if one exists).
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_ITERATION_NOTEBOOK::UpdateInformation(void)
{
	// Make sure the object has already been assigned
	if (CurrentIteration)
		// Call the method that performs the update
		UpdateInformation(CurrentIteration);

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_NOTEBOOK
// Function:		UpdateInformation
//
// Description:		Updates the information in this notebook.
//
// Input Argurments:
//		_CurrentIteration	= ITERATION* pointing to the currenly active object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_ITERATION_NOTEBOOK::UpdateInformation(ITERATION *_CurrentIteration)
{
	// Update the class member
	CurrentIteration = _CurrentIteration;

	// If the current object is NULL, we no longer have an object to represent
	if (!CurrentIteration)
	{
		// Delete all the notebook pages
		DeleteAllPages();

		return;
	}

	// If we don't have any pages, we need to create the controls
	if (GetPageCount() == 0)
		CreateControls();

	// Call the update functions for the pages, too
	EditRange->UpdateInformation(CurrentIteration);
	EditPlots->UpdateInformation(CurrentIteration);
	EditOptions->UpdateInformation(CurrentIteration);

	return;
}

//==========================================================================
// Class:			EDIT_ITERATION_NOTEBOOK
// Function:		CreateControls
//
// Description:		Creates the controls for this notebook.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EDIT_ITERATION_NOTEBOOK::CreateControls(void)
{
	// Delete the existing pages before we create the new ones
	DeleteAllPages();

	// Create the notebook pages
	EditRange = new EDIT_ITERATION_RANGE_PANEL(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize, Debugger);
	EditPlots = new EDIT_ITERATION_PLOTS_PANEL(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize, Debugger);
	EditOptions = new EDIT_ITERATION_OPTIONS_PANEL(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize, Debugger);

	// Add them to the notebook
	AddPage(EditRange, _T("Range"));
	AddPage(EditPlots, _T("Active Plots"));
	AddPage(EditOptions, _T("Options"));

	return;
}