/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editSuspensionRatesPanel.h
// Date:  5/27/2016
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditSuspensionRatesPanel class.
//        This class is used to edit the spring and ARB rates.

#ifndef EDIT_SUSPENSION_RATES_PANEL_H_
#define EDIT_SUSPENSION_RATES_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// wxWidgets forward declarations
class wxGridEvent;

namespace VVASE
{

// Local forward declarations
class Suspension;
class SuperGrid;
class EditSuspensionNotebook;

class EditSuspensionRatesPanel : public wxScrolledWindow
{
public:
	EditSuspensionRatesPanel(EditSuspensionNotebook &parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size);
	~EditSuspensionRatesPanel();

	void UpdateInformation(Suspension *currentSuspension);
	void UpdateInformation();

private:
	EditSuspensionNotebook &parent;

	Suspension *currentSuspension;

	enum GridRows
	{
		RowLeftFrontSpring,
		RowRightFrontSpring,
		RowLeftRearSpring,
		RowRightRearSpring,

		RowFrontThirdSpring,
		RowRearThirdSpring,

		RowFrontARB,
		RowRearARB,

		GridRowCount
	};

	enum GridCols
	{
		ColLabel,
		ColValue,
		ColUnits,

		GridColCount,
	};

	void CreateControls();

	// This helps avoid unnecessary function calls to update the screen
	int lastRowSelected;

	// Event handlers-----------------------------------------------------
	void GridCellChangedEvent(wxGridEvent &event);
	// End event handlers-------------------------------------------------

	SuperGrid *rates;

	double ConvertLinearSpringInput(const double& value) const;
	double ConvertLinearSpringOutput(const double& value) const;
	double ConvertRotarySpringInput(const double& value) const;
	double ConvertRotarySpringOutput(const double& value) const;

	DECLARE_EVENT_TABLE();
};

}// namespace VVASE

#endif// EDIT_SUSPENSION_RATES_PANEL_H_
