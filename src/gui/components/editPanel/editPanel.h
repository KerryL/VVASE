/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editPanel.h
// Date:  2/10/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the EditPanel class.

#ifndef EDIT_PANEL_H_
#define EDIT_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

#undef None

// Local headers
#include "VVASE/gui/guiObject.h"

// Standard C++ headers
#include <mutex>

// wxWidgets forward declarations
class wxCombo;
class wxCheckBox;

namespace VVASE
{

// Local forward declarations
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
class EditSweepNotebook;

class EditPanel : public wxPanel
{
public:
	EditPanel(MainFrame &mainFrame, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);

	// Updates the information on the panel
	void UpdateInformation(GuiObject *currentObject);
	void UpdateInformation();

	inline MainFrame &GetMainFrame() { return mainFrame; }
	inline GuiObject *GetCurrentObject() { return currentObject; }
	std::mutex *GetCurrentMutex() { return carMutex; }

private:
	MainFrame &mainFrame;
	MainTree *systemsTree;

	GuiObject *currentObject = nullptr;

	void CreateControls(bool ignoreSystemsTree);

	void DeleteAllControls();

	GuiObject::ItemType currentType = GuiObject::ItemType::None;

	wxTreeItemId currentTreeID;

	// The various edit panels we need to display
	// GuiCar
	EditAerodynamicsPanel	*editAerodynamics = nullptr;
	EditBrakesPanel			*editBrakes = nullptr;
	EditDifferentialPanel	*editDifferential = nullptr;
	EditDrivetrainPanel		*editDrivetrain = nullptr;
	EditEnginePanel			*editEngine = nullptr;
	EditMassPanel			*editMass = nullptr;
	EditSuspensionNotebook	*editSuspension = nullptr;
	EditTiresPanel			*editTires = nullptr;

	// Sweep
	EditSweepNotebook	*editSweep = nullptr;

	wxBoxSizer *sizer;

	std::mutex *carMutex = nullptr;
};

}// namespace VVASE

#endif// EDIT_PANEL_H_
