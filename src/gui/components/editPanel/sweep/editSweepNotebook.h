/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSweepNotebook.h
// Date:  11/14/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the EditIterationNotebook class.

#ifndef EDIT_ITERATION_NOTEBOOK_H_
#define EDIT_ITERATION_NOTEBOOK_H_

// wxWidgets headers
#include <wx/notebook.h>

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
