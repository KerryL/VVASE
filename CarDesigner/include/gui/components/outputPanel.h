/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011.

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  outputPanel.h
// Created:  3/15/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the OutputPanel class.
// History:

#ifndef _OUTPUT_PANEL_H_
#define _OUTPUT_PANEL_H_

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
	// Constructor
	OutputPanel(MainFrame &_mainFrame, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize);

	// Destructor
	~OutputPanel();

	// Updates the information on the panel
	void UpdateInformation(KinematicOutputs outputs, Car &car, int index,
		wxString name);
	void FinishUpdate(int _numberOfDataColumns);

	// For highlighting the column that corresponds to the active car
	void HighlightColumn(wxString _name);

private:
	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// Pointer to the main application window
	MainFrame &mainFrame;

	// The number of data columns we currently have
	int numberOfDataColumns;

	// The event IDs
	enum EventId
	{
		IdOutputGrid = wxID_HIGHEST + 1100
	};

	// Event handlers-----------------------------------------------------
	void ColumnResizeEvent(wxGridSizeEvent &event);
	// End event handlers-------------------------------------------------

	// The text-entry control
	SuperGrid *outputsList;

protected:
	//DECLARE_EVENT_TABLE()
};

#endif// _OUTPUT_PANEL_H_