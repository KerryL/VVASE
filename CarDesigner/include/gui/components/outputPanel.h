/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016.

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  outputPanel.h
// Created:  3/15/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the OutputPanel class.
// History:

#ifndef OUTPUT_PANEL_H_
#define OUTPUT_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/grid.h>

// VVASE forward declarations
class Car;
class KinematicOutputs;
class MainFrame;
class SuperGrid;

class OutputPanel : public wxPanel
{
public:
	OutputPanel(MainFrame &mainFrame, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize);
	~OutputPanel();

	void UpdateInformation(KinematicOutputs outputs, Car &car, int index,
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

	// Event handlers-----------------------------------------------------
	void ColumnResizeEvent(wxGridSizeEvent &event);
	// End event handlers-------------------------------------------------

	SuperGrid *outputsList;

protected:
	//DECLARE_EVENT_TABLE()
};

#endif// OUTPUT_PANEL_H_