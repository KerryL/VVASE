/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mainTree.h
// Created:  6/14/2008
// Author:  K. Loux
// Description:  Contains the class declaration for the MAIN_TREE class.
// History:
//	1/28/2009	- Changed structure of GUI components so context menu creation for all
//				  objects is handled by the MAIN_FRAME class.

#ifndef _MAIN_TREE_H_
#define _MAIN_TREE_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/treectrl.h>

// VVASE forward declarations
class Debugger;
class MainFrame;
class GuiObject;

class MainTree : public wxTreeCtrl
{
public:
	// Constructor
	MainTree(MainFrame &_parent, wxWindowID id, const wxPoint &pos,
		const wxSize &_size, long style, const Debugger &_debugger);

	// Destructor
	~MainTree();

	// Returns a poionter to the selected GUI_OBJECT, or NULL if none is selected
	GuiObject *GetSelectedItem(wxTreeItemId *selectedItem);

	// Returns the number of items below the root for the specified item
	// Can be used for getting the type of a child item
	int GetItemType(wxTreeItemId itemId);

	// Returns a pointer to this object's owner
	inline MainFrame& GetMainFrame(void) { return mainFrame; };

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

		IterationIcon,

		OptimizationIcon,

		NumberOfIcons
	};

	// For assigning icons, we need to give other objects access to the indeces
	int GetIconHandle(IconHandle id) { return iconHandle[id]; };

private:
	// Debugger message utility object
	const Debugger &debugger;

	// Pointer to this object's owner
	MainFrame &mainFrame;

	// Event handlers-----------------------------------------------------
	// Tree events
	void SelectionChangeEvent(wxTreeEvent &event);
	void ItemRightClickEvent(wxTreeEvent &event);
	void ItemActivatedEvent(wxTreeEvent &event);
	// End event handlers-------------------------------------------------

	// Handles selection logic when the selection changes or an item is activated
	void PerformSelection(bool isActivated);

	// Determines the application's index number for the currently selected item
	int GetSelectedObjectIndex(void);

	// The image list for storing tree item icons
	wxImageList *iconList;

	// These are for storing the icon indeces as we add them to the image list
	int iconHandle[NumberOfIcons];

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _MAIN_TREE_H_