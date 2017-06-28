/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  superGrid.h
// Date:  11/11/2011
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from wxGrid, implements some additional helper methods and options.

#ifndef SUPER_GRID_H_
#define SUPER_GRID_H_

// wxWidgets headers
#include <wx/grid.h>

namespace VVASE
{

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

}// namespace VVASE

#endif// SUPER_GRID_H_
