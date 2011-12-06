/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editPanel.h
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the EDIT_PANEL class.
// History:

#ifndef _EDIT_PANEL_H_
#define _EDIT_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "gui/guiObject.h"

// wxWidgets forward declarations
class wxPanel;
class wxCombo;
class wxCheckBox;

// VVASE forward declarations
class MainFrame;
class MainTree;
class EditAerodynamicsPanel;
class EditBrakesPanel;
class EditDifferentialPanel;
class EditDrivetrainPanel;
class EditEnginePanel;
class EditMassPanel;
class EditSuspensionNotebook;
class EditTiresPanel;
class EditIterationNotebook;

class EditPanel : public wxPanel
{
public:
	// Constructor
	EditPanel(MainFrame &_mainFrame, wxWindowID id, const wxPoint& pos,
		const wxSize& size);

	// Destructor
	~EditPanel();

	// Updates the information on the panel
	void UpdateInformation(GuiObject *_currentObject);
	void UpdateInformation(void);

	// Retrieves this object's parent
	inline MainFrame &GetMainFrame(void) { return mainFrame; };

	// Retrieves the current object
	inline GuiObject *GetCurrentObject(void) { return currentObject; };

	// Accessor for the current car's mutex object
	wxMutex *GetCurrentMutex(void) { return carMutex; };

private:
	// The parent window
	MainFrame &mainFrame;

	// The systems tree
	MainTree *systemsTree;

	// The object with which we are currently associated
	GuiObject *currentObject;

	// Creates the controls and positions everything within the panel
	void CreateControls(bool ignoreSystemsTree);

	// Deletes all of the controls
	void DeleteAllControls(void);

	// The type of object this panel is currently representing
	GuiObject::ItemType currentType;

	// Tree item ID for the current selection
	wxTreeItemId currentTreeID;

	// The various edit panels we need to display
	// GUI_CAR
	EditAerodynamicsPanel	*editAerodynamics;
	EditBrakesPanel			*editBrakes;
	EditDifferentialPanel	*editDifferential;
	EditDrivetrainPanel		*editDrivetrain;
	EditEnginePanel			*editEngine;
	EditMassPanel			*editMass;
	EditSuspensionNotebook	*editSuspension;
	EditTiresPanel			*editTires;

	// ITERATION
	EditIterationNotebook		*editIteration;

	// The sizer containing the panels
	wxBoxSizer *sizer;

	// Syncronization object for modifying cars
	wxMutex *carMutex;
};

#endif// _EDIT_PANEL_H_
