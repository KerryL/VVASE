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

#ifndef EDIT_ITERATION_NOTEBOOK_H_
#define EDIT_ITERATION_NOTEBOOK_H_

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
	EditIterationNotebook(EditPanel &parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, long style);
	~EditIterationNotebook();

	inline EditPanel &GetParent() { return parent; };

	void UpdateInformation(Iteration *currentIteration);
	void UpdateInformation();

private:
	EditPanel &parent;

	Iteration *currentIteration;

	void CreateControls();

	EditIterationRangePanel *editRange;
	EditIterationPlotsPanel *editPlots;
	EditIterationOptionsPanel *editOptions;
};

#endif// EDIT_ITERATION_NOTEBOOK_H_