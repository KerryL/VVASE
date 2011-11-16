/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011.

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  outputPanel.h
// Created:  3/15/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the OUTPUT_PANEL class.
// History:

#ifndef _OUTPUT_PANEL_H_
#define _OUTPUT_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/grid.h>

// VVASE forward declarations
class Debugger;
class Convert;
class CAR;
class KINEMATIC_OUTPUTS;
class MAIN_FRAME;

class OUTPUT_PANEL : public wxPanel
{
public:
	// Constructor
	OUTPUT_PANEL(MAIN_FRAME &_MainFrame, wxWindowID id, const wxPoint &pos,
		const wxSize &size, const Debugger &_debugger);

	// Destructor
	~OUTPUT_PANEL();

	// Updates the information on the panel
	void UpdateInformation(KINEMATIC_OUTPUTS Outputs, CAR &Car, int Index,
		wxString Name);
	void FinishUpdate(int _NumberOfDataColumns);

	// For highlighting the column that corresponds to the active car
	void HighlightColumn(wxString _Name);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The application's converter object
	const Convert &Converter;

	// Creates the controls and positions everything within the panel
	void CreateControls(void);

	// Pointer to the main application window
	MAIN_FRAME &MainFrame;

	// The number of data columns we currently have
	int NumberOfDataColumns;

	// The event IDs
	enum EVENT_ID
	{
		IdOutputGrid = wxID_HIGHEST + 1100
	};

	// Event handlers-----------------------------------------------------
	void ColumnResizeEvent(wxGridSizeEvent &event);
	// End event handlers-------------------------------------------------

	// The text-entry control
	wxGrid *OutputsList;

protected:
	DECLARE_EVENT_TABLE()
};

#endif// _OUTPUT_PANEL_H_