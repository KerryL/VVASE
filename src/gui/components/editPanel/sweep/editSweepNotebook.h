/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSweepNotebook.h
// Date:  11/14/2010
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the EditSweepNotebook class.

#ifndef EDIT_ITERATION_NOTEBOOK_H_
#define EDIT_ITERATION_NOTEBOOK_H_

// wxWidgets headers
#include <wx/notebook.h>

namespace VVASE
{

// Local forward declarations
class Sweep;
class EditPanel;
class EditSweepRangePanel;
class EditSweepPlotsPanel;
class EditSweepOptionsPanel;

class EditSweepNotebook : public wxNotebook
{
public:
	EditSweepNotebook(EditPanel &parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, long style);
	~EditSweepNotebook();

	inline EditPanel &GetParent() { return parent; };

	void UpdateInformation(Sweep *currentSweep);
	void UpdateInformation();

private:
	EditPanel &parent;

	Sweep *currentSweep;

	void CreateControls();

	EditSweepRangePanel *editRange;
	EditSweepPlotsPanel *editPlots;
	EditSweepOptionsPanel *editOptions;
};

}// namespace VVASE

#endif// EDIT_ITERATION_NOTEBOOK_H_
