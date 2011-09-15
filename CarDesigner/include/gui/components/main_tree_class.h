/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  main_tree_class.h
// Created:  6/14/2008
// Author:  K. Loux
// Description:  Contains the class declaration for the MAIN_TREE class.
// History:
//	1/28/2009	- Changed structure of GUI components so context menu creation for all
//				  objects is handled by the MAIN_FRAME class.

#ifndef _MAIN_TREE_CLASS_H_
#define _MAIN_TREE_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/treectrl.h>

// VVASE forward declarations
class DEBUGGER;
class MAIN_FRAME;
class GUI_OBJECT;

class MAIN_TREE : public wxTreeCtrl
{
public:
	// Constructor
	MAIN_TREE(MAIN_FRAME &_Parent, wxWindowID id, const wxPoint &pos,
		const wxSize &size, long style, const DEBUGGER &_Debugger);

	// Destructor
	~MAIN_TREE();

	// Returns a poionter to the selected GUI_OBJECT, or NULL if none is selected
	GUI_OBJECT *GetSelectedItem(wxTreeItemId *SelectedItem);

	// Returns the number of items below the root for the specified item
	// Can be used for getting the type of a child item
	int GetItemType(wxTreeItemId ItemId);

	// Returns a pointer to this object's owner
	inline MAIN_FRAME& GetMainFrame(void) { return MainFrame; };

	enum ICON_HANDLE
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

	// For assigning icons, we need to give other objects access to the indecies
	int GetIconHandle(ICON_HANDLE Id) { return IconHandle[Id]; };

private:
	// Debugger message utility object
	const DEBUGGER &Debugger;

	// Pointer to this object's owner
	MAIN_FRAME &MainFrame;

	// Event handlers-----------------------------------------------------
	// Tree events
	void SelectionChangeEvent(wxTreeEvent &event);
	void ItemRightClickEvent(wxTreeEvent &event);
	void ItemActivatedEvent(wxTreeEvent &event);
	// End event handlers-------------------------------------------------

	// Handles selection logic when the selection changes or an item is activated
	void PerformSelection(bool IsActivated);

	// Determines the application's index number for the currently selected item
	int GetSelectedObjectIndex(void);

	// The image list for storing tree item icons
	wxImageList *IconList;

	// These are for storing the icon indecies as we add them to the image list
	int IconHandle[NumberOfIcons];

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _MAIN_TREE_CLASS_H_