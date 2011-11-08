/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  main_tree_class.cpp
// Created:  6/14/2008
// Author:  K. Loux
// Description:  Contains the class declaration for the MAIN_TREE class.
// History:
//	1/28/2009	- Changed structure of GUI components so context menu creation for all
//				  objects is handled by the MAIN_FRAME class.

// wxWidgets headers
#include <wx/menu.h>
#include <wx/imaglist.h>

// CarDesigner headers
#include "gui/components/main_frame_class.h"
#include "gui/components/main_tree_class.h"
#include "gui/gui_car_class.h"
#include "gui/gui_object_class.h"
#include "vUtilities/debug_class.h"
#include "gui/iteration_class.h"

//==========================================================================
// Class:			MAIN_TREE
// Function:		MAIN_TREE
//
// Description:		Constructor for MAIN_TREE class.
//
// Input Arguments:
//		_MainFrame	= MAIN_FRAME&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		style		= long for passing to parent class's constructor
//		_debugger	= const Debugger& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MAIN_TREE::MAIN_TREE(MAIN_FRAME &_MainFrame, wxWindowID id, const wxPoint& pos,
					 const wxSize& size, long style, const Debugger &_debugger)
					 : wxTreeCtrl(&_MainFrame, id, pos, size, style),
					 Debugger(_debugger), MainFrame(_MainFrame)
{
	// Set up the image list for tree item icons
	int Size = 16;
	IconList = new wxImageList(Size, Size, true, 1);

	// No icons unless we're in windows
#ifdef __WXMSW__
	IconHandle[CarIcon] = IconList->Add(wxIcon(_T("ICON_ID_CAR"),
		wxBITMAP_TYPE_ICO_RESOURCE, Size, Size));
	IconHandle[AerodynamicsIcon] = IconList->Add(wxIcon(_T("ICON_ID_AERODYNAMICS"),
		wxBITMAP_TYPE_ICO_RESOURCE, Size, Size));
	IconHandle[BrakesIcon] = IconList->Add(wxIcon(_T("ICON_ID_BRAKES"),
		wxBITMAP_TYPE_ICO_RESOURCE, Size, Size));
	IconHandle[DrivetrainIcon] = IconList->Add(wxIcon(_T("ICON_ID_DRIVETRAIN"),
		wxBITMAP_TYPE_ICO_RESOURCE, Size, Size));
	IconHandle[EngineIcon] = IconList->Add(wxIcon(_T("ICON_ID_ENGINE"),
		wxBITMAP_TYPE_ICO_RESOURCE, Size, Size));
	IconHandle[MassPropertiesIcon] = IconList->Add(wxIcon(_T("ICON_ID_MASS_PROPS"),
		wxBITMAP_TYPE_ICO_RESOURCE, Size, Size));
	IconHandle[SuspensionIcon] = IconList->Add(wxIcon(_T("ICON_ID_SUSPENSION"),
		wxBITMAP_TYPE_ICO_RESOURCE, Size, Size));
	IconHandle[TiresIcon] = IconList->Add(wxIcon(_T("ICON_ID_TIRES"),
		wxBITMAP_TYPE_ICO_RESOURCE, Size, Size));
	IconHandle[IterationIcon] = IconList->Add(wxIcon(_T("ICON_ID_ITERATION"),
		wxBITMAP_TYPE_ICO_RESOURCE, Size, Size));
	IconHandle[OptimizationIcon] = IconList->Add(wxIcon(_T("ICON_ID_ITERATION"),// FIXME!!!
		wxBITMAP_TYPE_ICO_RESOURCE, Size, Size));
#endif
	// FIXME!!! Linux icons

	// Assign the image list to the tree
	AssignImageList(IconList);
}

//==========================================================================
// Class:			MAIN_TREE
// Function:		~MAIN_TREE
//
// Description:		Destructor for the MAIN_TREE class.
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
MAIN_TREE::~MAIN_TREE()
{
	// Clear out the icon list
	IconList->RemoveAll();

	// DO NOT delete IconList here - when we make the AssignImageList call
	// in the constructor, we give the tree control ownership of the list,
	// and it handles the deletion for us.
}

//==========================================================================
// Class:			MAIN_TREE
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
BEGIN_EVENT_TABLE(MAIN_TREE, wxTreeCtrl)
	EVT_TREE_SEL_CHANGED(wxID_ANY,				MAIN_TREE::SelectionChangeEvent)
	EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY,			MAIN_TREE::ItemRightClickEvent)
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY,			MAIN_TREE::ItemActivatedEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			MAIN_TREE
// Function:		SelectionChangeEvent
//
// Description:		Makes the selected car active.
//
// Input Arguments:
//		IsActivated	= bool indicated whether this is firing on selection change
//					  or activated event
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MAIN_TREE::PerformSelection(bool IsActivated)
{
	// Make the notebook tab for the selected car come to the front, which
	// will in turn make that car active.
	MainFrame.SetActiveIndex(GetSelectedObjectIndex(), IsActivated);

	return;
}

//==========================================================================
// Class:			MAIN_TREE
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
int MAIN_TREE::GetSelectedObjectIndex(void)
{
	// Determine the object that corresponds to the selected item
	int Index;
	for (Index = 0; Index < MainFrame.GetObjectCount(); Index++)
	{
		// Check to see if the object with index Index is the owner of the selected item
		if (MainFrame.GetObjectByIndex(Index)->IsThisObjectSelected(GetSelection()))
			break;
	}

	return Index;
}

//==========================================================================
// Class:			MAIN_TREE
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
void MAIN_TREE::SelectionChangeEvent(wxTreeEvent &event)
{
	// Prevent this event from firing just because we deleted a car
	if (MainFrame.ObjectIsBeingDeleted())
		return;

	// Determine if we should change the tab, or just update the edit panel
	bool ChangeTab = true;
	
	// Make sure the selected object index is still valid - we had some problems
	// under GTK where this event fires following an object being deleted and
	// attempts to GetObjectByIndex with an invalid index

	if (GetSelectedObjectIndex() < MainFrame.GetObjectCount())
	{
		// If the current type is not the same as the previous type and the current
		// type is a car, do not update the tab
		// Also, if the current selection is the same as the previous selection,
		// do not update the tab
		if ((MainFrame.GetObjectByIndex(GetSelectedObjectIndex())->GetType() !=
				MainFrame.GetObjectByIndex(MainFrame.GetActiveIndex())->GetType() &&
				MainFrame.GetObjectByIndex(GetSelectedObjectIndex())->GetType() == GUI_OBJECT::TYPE_CAR) ||
				GetSelectedObjectIndex() == MainFrame.GetActiveIndex())
			ChangeTab = false;
	}

	// Perform the appropriate actions
	PerformSelection(ChangeTab);

	event.Skip();

	return;
}

//==========================================================================
// Class:			MAIN_TREE
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
void MAIN_TREE::ItemRightClickEvent(wxTreeEvent &event)
{
	// Iterate through the objects to get the GUI_OBJECT associated with the item
	// that was right clicked.
	int Index;
	for (Index = 0; Index < MainFrame.GetObjectCount(); Index++)
	{
		// Check to see if the selected tree item belongs to the car with this index
		if (MainFrame.GetObjectByIndex(Index)->IsThisObjectSelected(event.GetItem()))
			break;
	}

	// The point we get from the event is in client coordinates w.r.t. SystemsTree.
	// To generate a context menu in the correct position, we need to convert this to client
	// coordinates w.r.t. MainFrame.
	wxPoint MenuPosition = event.GetPoint() + GetPosition();

	// Create and display the context menu
	MainFrame.CreateContextMenu(Index, MenuPosition);

	event.Skip();

	return;
}

//==========================================================================
// Class:			MAIN_TREE
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
void MAIN_TREE::ItemActivatedEvent(wxTreeEvent &event)
{
	// Perform the appropriate actions
	PerformSelection(true);

	event.Skip();

	return;
}

//==========================================================================
// Class:			MAIN_TREE
// Function:		GetSelectedItem
//
// Description:		Retrieves the selected item.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		SelectedItem	= wxTreeItemId* of the selected item
//
// Return Value:
//		*GUI_OBJECT = pointer to selected GUI_OBJECT, or NULL if no item is
//					  selected
//
//==========================================================================
GUI_OBJECT *MAIN_TREE::GetSelectedItem(wxTreeItemId *SelectedItem)
{
	// Assign the output arguement (if we were passed a valid pointer)
	if (SelectedItem)
		*SelectedItem = GetSelection();

	// Iterate through the open objects
	int Index;
	for (Index = 0; Index < MainFrame.GetObjectCount(); Index++)
	{
		// Check to see if the selected tree item belongs to the object with this index
		if (MainFrame.GetObjectByIndex(Index)->IsThisObjectSelected(GetSelection()))
			return MainFrame.GetObjectByIndex(Index);
	}

	// No matches - return NULL
	return NULL;
}

//==========================================================================
// Class:			MAIN_TREE
// Function:		GetItemType
//
// Description:		Returns the number of items below the root the specified
//					child is.  Used for determining the type of a child item.
//
// Input Arguments:
//		ItemId	= wxTreeItemId
//
// Output Arguments:
//		None
//
// Return Value:
//		integer specifying the type of the selected item
//
//==========================================================================
int MAIN_TREE::GetItemType(wxTreeItemId ItemId)
{
	// Iterate through the open objects
	int Index;
	for (Index = 0; Index < MainFrame.GetObjectCount(); Index++)
	{
		// Check to see if the selected tree item belongs to the object with this index
		if (MainFrame.GetObjectByIndex(Index)->IsThisObjectSelected(GetSelection()))
		{
			GUI_OBJECT *SelectedObject = MainFrame.GetObjectByIndex(Index);

			// Now check each tree item ID for that object with the specified tree item ID
			// Each type has different children, so we need to first identify the type
			int i;
			switch (SelectedObject->GetType())
			{
			case GUI_OBJECT::TYPE_CAR:
				for (i = 0; i < GUI_CAR::NumberOfSubsystems; i++)
				{
					// If the ID matches, return this index
					if (ItemId == static_cast<GUI_CAR*>(SelectedObject)->Subsystems[i])
						return i;
				}
				break;

			// Unused types
			case GUI_OBJECT::TYPE_ITERATION:
			case GUI_OBJECT::TYPE_OPTIMIZATION:
			case GUI_OBJECT::TYPE_NONE:
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