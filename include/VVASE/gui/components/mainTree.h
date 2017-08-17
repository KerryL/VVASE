/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  mainTree.h
// Date:  6/14/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the MAIN_TREE class.

#ifndef MAIN_TREE_H_
#define MAIN_TREE_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/treectrl.h>

namespace VVASE
{

// Local forward declarations
class MainFrame;
class GuiObject;

class MainTree : public wxTreeCtrl
{
public:
	MainTree(MainFrame &parent, wxWindowID id, const wxPoint &pos,
		const wxSize &size, long style);
	~MainTree();

	GuiObject *GetSelectedItem(wxTreeItemId *selectedItem);

	// Returns the number of items below the root for the specified item
	// Can be used for getting the type of a child item
	int GetItemType(wxTreeItemId itemId);

	// Returns a pointer to this object's owner
	inline MainFrame& GetMainFrame() { return mainFrame; }

	enum IconHandle
	{
		CarIcon,
		AerodynamicsIcon,
		BrakesIcon,
		DrivetrainIcon,
		EngineIcon,
		MassPropertiesIcon,
		SuspensionIcon,
		TiresIcon,

		SweepIcon,

		OptimizationIcon,

		NumberOfIcons
	};

	// For assigning icons, we need to give other objects access to the indeces
	int GetIconHandle(IconHandle id) { return iconHandle[id]; }

private:
	MainFrame &mainFrame;

	// Event handlers-----------------------------------------------------
	// Tree events
	void SelectionChangeEvent(wxTreeEvent &event);
	void ItemRightClickEvent(wxTreeEvent &event);
	void ItemActivatedEvent(wxTreeEvent &event);
	// End event handlers-------------------------------------------------

	void PerformSelection(bool isActivated);

	int GetSelectedObjectIndex();

	wxImageList *iconList;

	// These are for storing the icon indeces as we add them to the image list
	int iconHandle[NumberOfIcons];

	DECLARE_EVENT_TABLE();
};

}// namespace VVASE

#endif// MAIN_TREE_H_
