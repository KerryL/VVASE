/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  superGrid.h
// Created:  11/11/2011
// Author:  K. Loux
// Description:  Derived from wxGrid, implements some additional helper methods and options.
// History:

#ifndef SUPER_GRID_H_
#define SUPER_GRID_H_

// wxWidgets headers
#include <wx/grid.h>

class SuperGrid : public wxGrid
{
public:
	SuperGrid(wxWindow *parent, wxWindowID id = wxID_ANY);

	// Sets the specified column to automatically fill up the remaining space in the grid
	void AutoStretchColumn(const int &col);

	// Adjust the height of the control to match the height of the sum of the rows
	void FitHeight();

private:
	wxArrayInt stretchColumns;

	DECLARE_EVENT_TABLE();

protected:
	virtual void OnSize(wxSizeEvent &event);
};

#endif// SUPER_GRID_H_