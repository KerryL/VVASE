/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editSuspensionNotebook.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_SUSPENSION_NOTEBOOK class.
// History:

#ifndef EDIT_SUSPENSION_NOTEBOOK_H_
#define EDIT_SUSPENSION_NOTEBOOK_H_

// wxWidgets headers
#include <wx/notebook.h>

// wxWidgets forward declarations
class wxPanel;
class wxCombo;
class wxCheckBox;

// VVASE forward declarations
class Car;
class EditPanel;
class EditCornerPanel;
class EditSuspensionPanel;

class EditSuspensionNotebook : public wxNotebook
{
public:
	EditSuspensionNotebook(EditPanel &parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, long style);
	~EditSuspensionNotebook();

	inline EditPanel &GetParent() { return parent; }

	void UpdateInformation(Car *currentCar);
	void UpdateInformation();

	// Keeps the sides of the car identical for symmetric suspensions
	void UpdateSymmetry();

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
};

#endif// EDIT_SUSPENSION_NOTEBOOK_H_