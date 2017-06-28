/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  wxRelatedUtilities.cpp
// Date:  11/5/2011
// Author:  K. Loux
// Desc:  Contains helper functions and wrappers for working with wxWidgets.

// Local headers
#include "vUtilities/wxRelatedUtilities.h"

//==========================================================================
// Class:			None
// Function:		SetMinimumWidthFromContents
//
// Description:		Finds the largest minimum width for a control based on
//					its contents, and sets all controls in the array to that
//					width.
//
// Input Arguments:
//		control		= wxComboBox*
//		additional	= unsigned int, to be added to the width of the longest
//					  string
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void SetMinimumWidthFromContents(wxComboBox *control, unsigned int additional)
{
	unsigned int i;
	int maxWidth(0), width;
	for (i = 0; i < control->GetCount(); i++)
	{
		control->GetTextExtent(control->GetString(i), &width, NULL);
		if (width > maxWidth)
			maxWidth = width;
	}

	control->SetMinSize(wxSize(maxWidth + additional, -1));
}

//==========================================================================
// Class:			None
// Function:		SafelyGetComboBoxSelection
//
// Description:		Returns the index of the selected combo box item.
//
// Input Arguments:
//		control		= wxComboBox*
//
// Output Arguments:
//		None
//
// Return Value:
//		int, index of selected item, or wxNOT_FOUND if not found
//
//==========================================================================
int SafelyGetComboBoxSelection(wxComboBox *control)
{
	int selection = control->GetCurrentSelection();

#ifndef __WXMSW__
	// Under MSW, this is not needed, otherwise, an initial value might cause
	// wxNOT_FOUND to be returned (i.e., text is in the control, but the user
	// has not changed it from the default value).  We handle that case here.
	if (selection == wxNOT_FOUND)
	{
		// Compare the text displayed on the control to each item
		unsigned int i;
		for (i = 0; i < control->GetCount(); i++)
		{
			if (control->GetString(i).IsSameAs(control->GetValue()))
			{
				selection = i;
				break;
			}
		}
	}
#endif

	return selection;
}

//==========================================================================
// Class:			None
// Function:		SkipMouseEvent
//
// Description:		Mouse events cannot be Skip()ed, so we force propegation
//					directly to parent.
//
// Input Arguments:
//		event = wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void SkipMouseEvent(wxMouseEvent &event)
{
	static_cast<EventWindowData*>(event.GetEventUserData())->GetWindow()->
		GetEventHandler()->ProcessEvent(event);
}
