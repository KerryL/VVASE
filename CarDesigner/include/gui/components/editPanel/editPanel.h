/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_panel_class.h
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_PANEL class.
// History:

#ifndef _EDIT_PANEL_CLASS_H_
#define _EDIT_PANEL_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "gui/gui_object_class.h"

// wxWidgets forward declarations
class wxPanel;
class wxCombo;
class wxCheckBox;

// VVASE forward declarations
class MAIN_FRAME;
class MAIN_TREE;
class Debugger;
class EDIT_AERODYNAMICS_PANEL;
class EDIT_BRAKES_PANEL;
class EDIT_DIFFERENTIAL_PANEL;
class EDIT_DRIVETRAIN_PANEL;
class EDIT_ENGINE_PANEL;
class EDIT_MASS_PANEL;
class EDIT_SUSPENSION_NOTEBOOK;
class EDIT_TIRES_PANEL;
class EDIT_ITERATION_NOTEBOOK;

class EDIT_PANEL : public wxPanel
{
public:
	// Constructor
	EDIT_PANEL(MAIN_FRAME &_MainFrame, wxWindowID id, const wxPoint& pos,
		const wxSize& size, const Debugger &_debugger);

	// Destructor
	~EDIT_PANEL();

	// Updates the information on the panel
	void UpdateInformation(GUI_OBJECT *_CurrentObject);
	void UpdateInformation(void);

	// Retrieves this object's parent
	inline MAIN_FRAME &GetMainFrame(void) { return MainFrame; };

	// Retrieves the current object
	inline GUI_OBJECT *GetCurrentObject(void) { return CurrentObject; };

	// Accessor for the current car's mutex object
	wxMutex *GetCurrentMutex(void) { return CarMutex; };

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The parent window
	MAIN_FRAME &MainFrame;

	// The systems tree
	MAIN_TREE *SystemsTree;

	// The object with which we are currently associated
	GUI_OBJECT *CurrentObject;

	// Creates the controls and positions everything within the panel
	void CreateControls(bool IgnoreSystemsTree);

	// Deletes all of the controls
	void DeleteAllControls(void);

	// The type of object this panel is currently representing
	GUI_OBJECT::ITEM_TYPE CurrentType;

	// Tree item ID for the current selection
	wxTreeItemId CurrentTreeID;

	// The various edit panels we need to display
	// GUI_CAR
	EDIT_AERODYNAMICS_PANEL		*EditAerodynamics;
	EDIT_BRAKES_PANEL			*EditBrakes;
	EDIT_DIFFERENTIAL_PANEL		*EditDifferential;
	EDIT_DRIVETRAIN_PANEL		*EditDrivetrain;
	EDIT_ENGINE_PANEL			*EditEngine;
	EDIT_MASS_PANEL				*EditMass;
	EDIT_SUSPENSION_NOTEBOOK	*EditSuspension;
	EDIT_TIRES_PANEL			*EditTires;

	// ITERATION
	EDIT_ITERATION_NOTEBOOK		*EditIteration;

	// The sizer containing the panels
	wxBoxSizer *Sizer;

	// Syncronization object for modifying cars
	wxMutex *CarMutex;
};

#endif// _EDIT_PANEL_CLASS_H_