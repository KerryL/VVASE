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

#ifndef _EDIT_SUSPENSION_NOTEBOOK_H_
#define _EDIT_SUSPENSION_NOTEBOOK_H_

// wxWidgets headers
#include <wx/notebook.h>

// wxWidgets forward declarations
class wxPanel;
class wxCombo;
class wxCheckBox;

// VVASE forward declarations
class Debugger;
class Car;
class EditPanel;
class EditCornerPanel;
class EditSuspensionPanel;

class EditSuspensionNotebook : public wxNotebook
{
public:
	// Constructor
	EditSuspensionNotebook(EditPanel &_parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, long style, const Debugger &_debugger);

	// Destructor
	~EditSuspensionNotebook();

	// Returns access to this object's parent
	inline EditPanel &GetParent() { return parent; };

	// Updates the information on the panel
	void UpdateInformation(Car *_currentCar);
	void UpdateInformation(void);

	// Keeps the sides of the car identical for symmetric suspensions
	void UpdateSymmetry(void);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The parent window
	EditPanel &parent;

	// The object with which we are currently associated
	Car *currentCar;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// The different notebook tabs
	EditSuspensionPanel *suspension;
	EditCornerPanel *rightFront;
	EditCornerPanel *leftFront;
	EditCornerPanel *rightRear;
	EditCornerPanel *leftRear;
};

#endif// _EDIT_SUSPENSION_NOTEBOOK_H_