/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  mainTree.cpp
// Date:  6/14/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the MainTree class.

// wxWidgets headers
#include <wx/menu.h>
#include <wx/imaglist.h>

// Local headers
#include "VVASE/gui/components/mainFrame.h"
#include "VVASE/gui/components/mainTree.h"
#include "../guiCar.h"
#include "VVASE/gui/guiObject.h"
#include "VVASE/core/utilities/debugger.h"
#include "../sweep.h"// Something included from within this file defines None under GTK (TODO)

#undef None

// Icons (only for GTK)
#ifdef __WXGTK__
#include "../res/icons/car16.xpm"
#include "../res/icons/aerodynamics16.xpm"
#include "../res/icons/brakes16.xpm"
#include "../res/icons/drivetrain16.xpm"
#include "../res/icons/engine16.xpm"
#include "../res/icons/massProperties16.xpm"
#include "../res/icons/suspension16.xpm"
#include "../res/icons/tires16.xpm"
#include "../res/icons/sweep16.xpm"
#include "../res/icons/optimization16.xpm"
#endif

namespace VVASE
{

//==========================================================================
// Class:			MainTree
// Function:		MainTree
//
// Description:		Constructor for MainTree class.
//
// Input Arguments:
//		mainFrame	= MainFrame&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		style		= long for passing to parent class's constructor
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MainTree::MainTree(MainFrame &mainFrame, wxWindowID id, const wxPoint& pos,
	const wxSize& size, long style) : wxTreeCtrl(&mainFrame, id, pos, size, style),
	mainFrame(mainFrame)
{
	// Set up the image list for tree item icons
	int iconSize = 16;
	iconList = new wxImageList(iconSize, iconSize, true, 1);

	// Add icons to the list
#ifdef __WXMSW__
	iconHandle[CarIcon] = iconList->Add(wxIcon(_T("ICON_ID_CAR"),
		wxBITMAP_TYPE_ICO_RESOURCE, iconSize, iconSize));
	iconHandle[AerodynamicsIcon] = iconList->Add(wxIcon(_T("ICON_ID_AERODYNAMICS"),
		wxBITMAP_TYPE_ICO_RESOURCE, iconSize, iconSize));
	iconHandle[BrakesIcon] = iconList->Add(wxIcon(_T("ICON_ID_BRAKES"),
		wxBITMAP_TYPE_ICO_RESOURCE, iconSize, iconSize));
	iconHandle[DrivetrainIcon] = iconList->Add(wxIcon(_T("ICON_ID_DRIVETRAIN"),
		wxBITMAP_TYPE_ICO_RESOURCE, iconSize, iconSize));
	iconHandle[EngineIcon] = iconList->Add(wxIcon(_T("ICON_ID_ENGINE"),
		wxBITMAP_TYPE_ICO_RESOURCE, iconSize, iconSize));
	iconHandle[MassPropertiesIcon] = iconList->Add(wxIcon(_T("ICON_ID_MASS_PROPS"),
		wxBITMAP_TYPE_ICO_RESOURCE, iconSize, iconSize));
	iconHandle[SuspensionIcon] = iconList->Add(wxIcon(_T("ICON_ID_SUSPENSION"),
		wxBITMAP_TYPE_ICO_RESOURCE, iconSize, iconSize));
	iconHandle[TiresIcon] = iconList->Add(wxIcon(_T("ICON_ID_TIRES"),
		wxBITMAP_TYPE_ICO_RESOURCE, iconSize, iconSize));
	iconHandle[SweepIcon] = iconList->Add(wxIcon(_T("ICON_ID_SWEEP"),
		wxBITMAP_TYPE_ICO_RESOURCE, iconSize, iconSize));
	iconHandle[OptimizationIcon] = iconList->Add(wxIcon(_T("ICON_ID_OPTIMIZATION"),
		wxBITMAP_TYPE_ICO_RESOURCE, iconSize, iconSize));
#elif __WXGTK__
	iconHandle[CarIcon] = iconList->Add(wxIcon(car16_xpm));
	iconHandle[AerodynamicsIcon] = iconList->Add(wxIcon(aero16_xpm));
	iconHandle[BrakesIcon] = iconList->Add(wxIcon(brakes16_xpm));
	iconHandle[DrivetrainIcon] = iconList->Add(wxIcon(drivetrain16_xpm));
	iconHandle[EngineIcon] = iconList->Add(wxIcon(engine16_xpm));
	iconHandle[MassPropertiesIcon] = iconList->Add(wxIcon(massProperties16_xpm));
	iconHandle[SuspensionIcon] = iconList->Add(wxIcon(suspension16_xpm));
	iconHandle[TiresIcon] = iconList->Add(wxIcon(tires16_xpm));
	iconHandle[SweepIcon] = iconList->Add(wxIcon(sweep16_xpm));
	iconHandle[OptimizationIcon] = iconList->Add(wxIcon(optimization16_xpm));
#endif

	AssignImageList(iconList);
}

//==========================================================================
// Class:			MainTree
// Function:		~MainTree
//
// Description:		Destructor for the MainTree class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MainTree::~MainTree()
{
	iconList->RemoveAll();

	// DO NOT delete IconList here - when we make the AssignImageList call
	// in the constructor, we give the tree control ownership of the list,
	// and it handles the deletion for us.
}

//==========================================================================
// Class:			MainTree
// Function:		Event Table
//
// Description:		Links the GUI  events with the event handling functions.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
BEGIN_EVENT_TABLE(MainTree, wxTreeCtrl)
	EVT_TREE_SEL_CHANGED(wxID_ANY,				MainTree::SelectionChangeEvent)
	EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY,			MainTree::ItemRightClickEvent)
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY,			MainTree::ItemActivatedEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			MainTree
// Function:		SelectionChangeEvent
//
// Description:		Makes the selected car active.
//
// Input Arguments:
//		isActivated	= bool indicated whether this is firing on selection change
//					  or activated event
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainTree::PerformSelection(bool isActivated)
{
	// Make the notebook tab for the selected car come to the front, which
	// will in turn make that car active.
	mainFrame.SetActiveIndex(GetSelectedObjectIndex(), isActivated);
}

//==========================================================================
// Class:			MainTree
// Function:		GetSelectedObjectIndex
//
// Description:		Gets the application's object index for the selected object.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		int representing object's index
//
//==========================================================================
int MainTree::GetSelectedObjectIndex()
{
	// Determine the object that corresponds to the selected item
	int Index;
	for (Index = 0; Index < mainFrame.GetObjectCount(); Index++)
	{
		// Check to see if the object with index Index is the owner of the selected item
		if (mainFrame.GetObjectByIndex(Index)->IsThisObjectSelected(GetSelection()))
			break;
	}

	return Index;
}

//==========================================================================
// Class:			MainTree
// Function:		SelectionChangeEvent
//
// Description:		Makes the selected car active.
//
// Input Arguments:
//		event	= &wxTreeEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainTree::SelectionChangeEvent(wxTreeEvent &event)
{
	// Prevent this event from firing just because we deleted a car or lost focus
	if (mainFrame.ObjectIsBeingDeleted())
		return;

	// Determine if we should change the tab, or just update the edit panel
	bool changeTab = true;

	// Make sure the selected object index is still valid - we had some problems
	// under GTK where this event fires following an object being deleted and
	// attempts to GetObjectByIndex with an invalid index

	if (GetSelectedObjectIndex() < mainFrame.GetObjectCount())
	{
		// If the current type is not the same as the previous type and the current
		// type is a car, do not update the tab
		// Also, if the current selection is the same as the previous selection,
		// do not update the tab
		if ((mainFrame.GetObjectByIndex(GetSelectedObjectIndex())->GetType() !=
				mainFrame.GetObjectByIndex(mainFrame.GetActiveIndex())->GetType() &&
				mainFrame.GetObjectByIndex(GetSelectedObjectIndex())->GetType() == GuiObject::ItemType::Car) ||
				GetSelectedObjectIndex() == mainFrame.GetActiveIndex())
			changeTab = false;
	}

	PerformSelection(changeTab);

	event.Skip();
}

//==========================================================================
// Class:			MainTree
// Function:		ItemRightClickEvent
//
// Description:		Creates a pop-up menu for the selected item.  Menu
//					contents change depending on item type.
//
// Input Arguments:
//		event	= &wxTreeEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainTree::ItemRightClickEvent(wxTreeEvent &event)
{
	// Iterate through the objects to get the GUI_OBJECT associated with the item
	// that was right clicked.
	int index;
	for (index = 0; index < mainFrame.GetObjectCount(); index++)
	{
		// Check to see if the selected tree item belongs to the car with this index
		if (mainFrame.GetObjectByIndex(index)->IsThisObjectSelected(event.GetItem()))
			break;
	}

	mainFrame.CreateContextMenu(index);
	event.Skip();
}

//==========================================================================
// Class:			MainTree
// Function:		ItemActivatedEvent
//
// Description:		Sets the active index to the current itemm
//
// Input Arguments:
//		event	= &wxTreeEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainTree::ItemActivatedEvent(wxTreeEvent &event)
{
	PerformSelection(true);

	event.Skip();
}

//==========================================================================
// Class:			MainTree
// Function:		GetSelectedItem
//
// Description:		Retrieves the selected item.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		selectedItem	= wxTreeItemId* of the selected item
//
// Return Value:
//		*GuiObject = pointer to selected GuiObject, or NULL if no item is
//					  selected
//
//==========================================================================
GuiObject *MainTree::GetSelectedItem(wxTreeItemId *selectedItem)
{
	if (selectedItem)
		*selectedItem = GetSelection();

	int index;
	for (index = 0; index < mainFrame.GetObjectCount(); index++)
	{
		// Check to see if the selected tree item belongs to the object with this index
		if (mainFrame.GetObjectByIndex(index)->IsThisObjectSelected(GetSelection()))
			return mainFrame.GetObjectByIndex(index);
	}

	return NULL;
}

//==========================================================================
// Class:			MainTree
// Function:		GetItemType
//
// Description:		Returns the number of items below the root the specified
//					child is.  Used for determining the type of a child item.
//
// Input Arguments:
//		itemId	= wxTreeItemId
//
// Output Arguments:
//		None
//
// Return Value:
//		integer specifying the type of the selected item
//
//==========================================================================
int MainTree::GetItemType(wxTreeItemId itemId)
{
	int index;
	for (index = 0; index < mainFrame.GetObjectCount(); index++)
	{
		// Check to see if the selected tree item belongs to the object with this index
		if (mainFrame.GetObjectByIndex(index)->IsThisObjectSelected(GetSelection()))
		{
			GuiObject *selectedObject = mainFrame.GetObjectByIndex(index);

			// Now check each tree item ID for that object with the specified tree item ID
			// Each type has different children, so we need to first identify the type
			int i;
			switch (selectedObject->GetType())
			{
			case GuiObject::ItemType::Car:
				for (i = 0; i < GuiCar::NumberOfSubsystems; i++)
				{
					// If the ID matches, return this index
					if (itemId == static_cast<GuiCar*>(selectedObject)->subsystems[i])
						return i;
				}
				break;

			// Unused types
			case GuiObject::ItemType::Sweep:
			case GuiObject::ItemType::Optimization:
			case GuiObject::ItemType::None:
				break;

			// Fail on unknown types to avoid forgetting any types
			default:
				assert(0);
				break;
			}
		}
	}

	return -1;
}

}// namespace VVASE
