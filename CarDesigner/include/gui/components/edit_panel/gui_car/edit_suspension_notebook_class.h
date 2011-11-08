/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_suspension_notebook_class.h
// Created:  2/19/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_SUSPENSION_NOTEBOOK class.
// History:

#ifndef _EDIT_SUSPENSION_NOTEBOOK_CLASS_H_
#define _EDIT_SUSPENSION_NOTEBOOK_CLASS_H_

// wxWidgets headers
#include <wx/notebook.h>

// wxWidgets forward declarations
class wxPanel;
class wxCombo;
class wxCheckBox;

// VVASE forward declarations
class Debugger;
class CAR;
class EDIT_PANEL;
class EDIT_CORNER_PANEL;
class EDIT_SUSPENSION_PANEL;

class EDIT_SUSPENSION_NOTEBOOK : public wxNotebook
{
public:
	// Constructor
	EDIT_SUSPENSION_NOTEBOOK(EDIT_PANEL &_Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, long style, const Debugger &_debugger);

	// Destructor
	~EDIT_SUSPENSION_NOTEBOOK();

	// Returns access to this object's parent
	inline EDIT_PANEL &GetParent() { return Parent; };

	// Updates the information on the panel
	void UpdateInformation(CAR *_CurrentCar);
	void UpdateInformation(void);

	// Keeps the sides of the car identical for symmetric suspensions
	void UpdateSymmetry(void);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The parent window
	EDIT_PANEL &Parent;

	// The object with which we are currently associated
	CAR *CurrentCar;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// The different notebook tabs
	EDIT_SUSPENSION_PANEL *Suspension;
	EDIT_CORNER_PANEL *RightFront;
	EDIT_CORNER_PANEL *LeftFront;
	EDIT_CORNER_PANEL *RightRear;
	EDIT_CORNER_PANEL *LeftRear;
};

#endif// _EDIT_SUSPENSION_NOTEBOOK_CLASS_H_