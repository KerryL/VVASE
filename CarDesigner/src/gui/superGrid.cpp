/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  superGrid.cpp
// Created:  11/11/2011
// Author:  K. Loux
// Description:  Derived from wxGrid, implements some additional helper methods and options.
// History:

// Local headers
#include "gui/superGrid.h"

//==========================================================================
// Class:			SuperGrid
// Function:		SuperGrid
//
// Description:		Constructor for SuperGrid class.
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
SuperGrid::SuperGrid(wxWindow *parent, wxWindowID id) : wxGrid(parent, id)
{
	SetTabBehaviour(Tab_Wrap);
}

//==========================================================================
// Class:			SuperGrid
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
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
BEGIN_EVENT_TABLE(SuperGrid, wxGrid)
	EVT_SIZE(SuperGrid::OnSize)
END_EVENT_TABLE();

//==========================================================================
// Class:			SuperGrid
// Function:		AutoStretchColumn
//
// Description:		Adds specified column to the list of columns to stretch.
//
// Input Arguments:
//		col		= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void SuperGrid::AutoStretchColumn(const int &col)
{
	wxASSERT(col < m_numCols);

	stretchColumns.Add(col);
}

//==========================================================================
// Class:			SuperGrid
// Function:		OnSize
// Description:		Override for OnSize event.
//
// Input Arguments:
//		event	= wxSizeEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void SuperGrid::OnSize(wxSizeEvent& event)
{
	if (stretchColumns.Count() > 0)
	{
		int i;
		unsigned int j;
		int fixedWidth(0);
		for (i = 0; i < m_numCols; i++)
		{
			// Check to see if that column is in the "stretch me" list
			for (j = 0; j < stretchColumns.Count(); j++)
			{
				if (i == stretchColumns[j])
					break;
			}

			if (j == stretchColumns.Count())
				fixedWidth += GetColSize(i);
		}

		// Calculate and set the correct width for the remaining columns
		int colWidth = (GetClientSize().GetWidth() - fixedWidth - GetRowLabelSize() - 1) / stretchColumns.Count();
		if (colWidth > 0)
		{
			for (j = 0; j < stretchColumns.Count(); j++)
				SetColSize(stretchColumns[j], colWidth);
		}
	}
	
	Refresh();
	event.Skip();
}

//==========================================================================
// Class:			SuperGrid
// Function:		FitHeight
// Description:		Sets the height of this control to exactly contain the
//					rows that are shown.
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
void SuperGrid::FitHeight()
{
	unsigned int height(GetColLabelSize() + GetNumberRows());
	int i;

	for (i = 0; i < GetNumberRows(); i++)
		height += GetRowHeight(i);

	SetSize(-1, height + 1);
	SetMinSize(GetSize());

	// Attempt to get scrollbars to dissappear (they are unneeded)
	ForceRefresh();
}
