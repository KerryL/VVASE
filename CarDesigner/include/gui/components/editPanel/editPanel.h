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

#ifndef EDIT_PANEL_H_
#define EDIT_PANEL_H_

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
	EditPanel(MainFrame &mainFrame, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);
	~EditPanel();

	// Updates the information on the panel
	void UpdateInformation(GuiObject *currentObject);
	void UpdateInformation();

	inline MainFrame &GetMainFrame() { return mainFrame; }
	inline GuiObject *GetCurrentObject() { return currentObject; }
	wxMutex *GetCurrentMutex() { return carMutex; }

private:
	MainFrame &mainFrame;
	MainTree *systemsTree;

	GuiObject *currentObject;

	void CreateControls(bool ignoreSystemsTree);

	void DeleteAllControls();

	GuiObject::ItemType currentType;

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

	wxBoxSizer *sizer;

	wxMutex *carMutex;
};

#endif// EDIT_PANEL_H_
