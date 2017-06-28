/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSuspensionNotebook.h
// Date:  2/19/2009
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the EditSuspensionNotebook class.

#ifndef EDIT_SUSPENSION_NOTEBOOK_H_
#define EDIT_SUSPENSION_NOTEBOOK_H_

// wxWidgets headers
#include <wx/notebook.h>

// wxWidgets forward declarations
class wxPanel;
class wxCombo;
class wxCheckBox;

namespace VVASE
{

// Local forward declarations
class Car;
class EditPanel;
class EditCornerPanel;
class EditSuspensionPanel;
class EditSuspensionRatesPanel;

class EditSuspensionNotebook : public wxNotebook
{
public:
	EditSuspensionNotebook(EditPanel &parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, long style);
	~EditSuspensionNotebook();

	inline EditPanel &GetParent() { return parent; }

	void UpdateInformation(Car *currentCar);
	void UpdateInformation();

private:
	EditPanel &parent;

	Car *currentCar;

	void CreateControls();

	// The different notebook tabs
	EditSuspensionPanel *suspension;
	EditCornerPanel *rightFront;
	EditCornerPanel *leftFront;
	EditCornerPanel *rightRear;
	EditCornerPanel *leftRear;
	EditSuspensionRatesPanel* rates;
};

}// namespace VVASE

#endif// EDIT_SUSPENSION_NOTEBOOK_H_
