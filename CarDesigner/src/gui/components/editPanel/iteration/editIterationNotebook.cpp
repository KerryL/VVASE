/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationNotebook.cpp
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class definition for the EditIterationNotebook class.
// History:

// VVASE headers
#include "gui/iteration.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/iteration/editIterationNotebook.h"
#include "gui/components/editPanel/iteration/editIterationRangePanel.h"
#include "gui/components/editPanel/iteration/editIterationPlotsPanel.h"
#include "gui/components/editPanel/iteration/editIterationOptionsPanel.h"
#include "vUtilities/debugger.h"

//==========================================================================
// Class:			EditIterationNotebook
// Function:		EditIterationNotebook
//
// Description:		Constructor for EditIterationNotebook class.  Initializes
//					the form and creates the controls, etc.
//
// Input Arguments:
//		_parent		= EditPanel&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		style		= long for passing to parent class's constructor
//		_debugger	= const Debugger& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EditIterationNotebook::EditIterationNotebook(EditPanel &_parent, wxWindowID id,
												 const wxPoint& pos, const wxSize& size,
												 long style, const Debugger &_debugger)
												 : wxNotebook(&_parent, id, pos, size, style),
												 debugger(_debugger), parent(_parent)
{
	// Initialize the 'Current' class members
	currentIteration = NULL;
}

//==========================================================================
// Class:			EditIterationNotebook
// Function:		~EditIterationNotebook
//
// Description:		Destructor for EditIterationNotebook class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EditIterationNotebook::~EditIterationNotebook()
{
}

//==========================================================================
// Class:			EditIterationNotebook
// Function:		UpdateInformation
//
// Description:		Updates the information in this notebook for the current
//					object (if one exists).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditIterationNotebook::UpdateInformation(void)
{
	// Make sure the object has already been assigned
	if (currentIteration)
		// Call the method that performs the update
		UpdateInformation(currentIteration);
}

//==========================================================================
// Class:			EditIterationNotebook
// Function:		UpdateInformation
//
// Description:		Updates the information in this notebook.
//
// Input Arguments:
//		_currentIteration	= Iteration* pointing to the currenly active object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditIterationNotebook::UpdateInformation(Iteration *_currentIteration)
{
	// Update the class member
	currentIteration = _currentIteration;

	// If the current object is NULL, we no longer have an object to represent
	if (!currentIteration)
	{
		// Delete all the notebook pages
		DeleteAllPages();

		return;
	}

	// If we don't have any pages, we need to create the controls
	if (GetPageCount() == 0)
		CreateControls();

	// Call the update functions for the pages, too
	editRange->UpdateInformation(currentIteration);
	editPlots->UpdateInformation(currentIteration);
	editOptions->UpdateInformation(currentIteration);
}

//==========================================================================
// Class:			EditIterationNotebook
// Function:		CreateControls
//
// Description:		Creates the controls for this notebook.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditIterationNotebook::CreateControls(void)
{
	// Delete the existing pages before we create the new ones
	DeleteAllPages();

	// Create the notebook pages
	editRange = new EditIterationRangePanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize, debugger);
	editPlots = new EditIterationPlotsPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize, debugger);
	editOptions = new EditIterationOptionsPanel(*this, wxID_ANY, wxDefaultPosition, wxDefaultSize, debugger);

	// Add them to the notebook
	AddPage(editRange, _T("Range"));
	AddPage(editPlots, _T("Active Plots"));
	AddPage(editOptions, _T("Options"));
}