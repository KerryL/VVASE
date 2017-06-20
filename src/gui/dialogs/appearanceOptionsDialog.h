/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  appearanceOptionsDialog.h
// Created:  4/23/2009
// Author:  K. Loux
// Description:  A dialog object for editing the contents of the AppearanceOptions object.
// History:

#ifndef APPEARNACE_OPTIONS_DIALOG_H_
#define APPEARNACE_OPTIONS_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "gui/appearanceOptions.h"

// wxWidgets forward declarations
class wxGridEvent;
class wxNotebook;

// VVASE forward declarations
class MainFrame;
class SuperGrid;
class Color;

class AppearanceOptionsDialog : public wxDialog
{
public:
	// Constructor
	AppearanceOptionsDialog(MainFrame &mainFrame, AppearanceOptions *options,
		wxWindowID id, const wxPoint &position, long style = wxDEFAULT_DIALOG_STYLE);

private:
	void CreateControls();

	AppearanceOptions *options;
	Color colorOptions[AppearanceOptions::ColorCount];

	enum EventId
	{
		IdColorGrid = wxID_HIGHEST + 700,
	};

	// Event handlers-----------------------------------------------------
	void OKClickEvent(wxCommandEvent &event);
	void CancelClickEvent(wxCommandEvent &event);
	void ColorGridDoubleClickEvent(wxGridEvent &event);
	void AlphaChangeEvent(wxGridEvent &event);
	// End event handlers-------------------------------------------------

	wxNotebook *notebook;

	wxPanel *colorPanel;
	wxPanel *visibilityPanel;
	wxPanel *sizePanel;
	wxPanel *resolutionPanel;

	SuperGrid *colorGrid;
	wxCheckListBox *visibilityList;
	SuperGrid *sizeGrid;
	SuperGrid *resolutionGrid;

	DECLARE_EVENT_TABLE()
};

#endif// APPEARNACE_OPTIONS_DIALOG_H_