/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  wxRelatedUtilites.cpp
// Created:  11/5/2011
// Author:  K. Loux
// Description:  Contains helper functions and wrappers for working with wxWidgets.
// History:

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