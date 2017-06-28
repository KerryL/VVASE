/*===================================================================================
                                    CarDesigner

===================================================================================*/

// File:  editIterationNotebook.h
// Date:  11/14/2010
// Author:  K. Loux
// Desc:  Contains the class declaration for the EditIterationNotebook class.

#ifndef EDIT_ITERATION_NOTEBOOK_H_
#define EDIT_ITERATION_NOTEBOOK_H_

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

// Local forward declarations
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

}// namespace VVASE

#endif// EDIT_ITERATION_NOTEBOOK_H_
