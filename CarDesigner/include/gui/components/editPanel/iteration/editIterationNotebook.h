/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationNotebook.h
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class declaration for the EditIterationNotebook class.
// History:

#ifndef _EDIT_ITERATION_NOTEBOOK_H_
#define _EDIT_ITERATION_NOTEBOOK_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class Iteration;
class EditPanel;
class EditIterationRangePanel;
class EditIterationPlotsPanel;
class EditIterationOptionsPanel;

class EditIterationNotebook : public wxNotebook
{
public:
	// Constructor
	EditIterationNotebook(EditPanel &_parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, long style);

	// Destructor
	~EditIterationNotebook();

	// Returns access to this object's parent
	inline EditPanel &GetParent() { return parent; };

	// Updates the information on the panel
	void UpdateInformation(Iteration *_currentIteration);
	void UpdateInformation(void);

private:
	// The parent window
	EditPanel &parent;

	// The object with which we are currently associated
	Iteration *currentIteration;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// The different notebook tabs
	EditIterationRangePanel *editRange;
	EditIterationPlotsPanel *editPlots;
	EditIterationOptionsPanel *editOptions;
};

#endif// _EDIT_ITERATION_NOTEBOOK_H_