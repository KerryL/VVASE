/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  outputPanel.h
// Date:  3/15/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the OutputPanel class.

#ifndef OUTPUT_PANEL_H_
#define OUTPUT_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/grid.h>

// Local headers
#include "gui/guiCar.h"

namespace VVASE
{

// VVASE forward declarations
class Car;
class MainFrame;
class SuperGrid;

class OutputPanel : public wxPanel
{
public:
	OutputPanel(MainFrame &mainFrame, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize);
	~OutputPanel();

	void UpdateInformation(GuiCar::CarOutputs outputs, Car &car, int index,
		wxString name);
	void FinishUpdate(int numberOfDataColumns);

	// For highlighting the column that corresponds to the active car
	void HighlightColumn(wxString name);

private:
	void CreateControls();

	MainFrame &mainFrame;

	int numberOfDataColumns;

	enum EventId
	{
		IdOutputGrid = wxID_HIGHEST + 1100
	};

	SuperGrid *outputsList;
};

}// namespace VVASE

#endif// OUTPUT_PANEL_H_
