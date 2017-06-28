/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  editSuspensionNotebook.h
// Date:  2/19/2009
// Author:  K. Loux
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
