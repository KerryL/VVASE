/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  appearanceOptionsDialog.h
// Created:  4/23/2009
// Author:  K. Loux
// Description:  A dialog object for editing the contents of the APPEARANCE_OPTIONS object.
// History:

#ifndef _APPEARNACE_OPTIONS_DIALOG_H_
#define _APPEARNACE_OPTIONS_DIALOG_H_

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
class Convert;
class Color;

class AppearanceOptionsDialog : public wxDialog
{
public:
	// Constructor
	AppearanceOptionsDialog(MainFrame &mainFrame, AppearanceOptions *_options,
		wxWindowID id, const wxPoint &position, long style = wxDEFAULT_DIALOG_STYLE);

	// Destructor
	~AppearanceOptionsDialog();

private:
	// Sets up the size and position of this dialog and its contents
	void CreateControls(void);

	// The object that handles the unit conversions between the input and output
	const Convert &converter;

	// The object to edit
	AppearanceOptions *options;

	// A copy of the color array
	Color colorOptions[AppearanceOptions::ColorCount];

	// The event IDs
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

	// The controls
	wxNotebook *notebook;

	wxPanel *colorPanel;
	wxPanel *visibilityPanel;
	wxPanel *sizePanel;
	wxPanel *resolutionPanel;

	SuperGrid *colorGrid;
	wxCheckListBox *visibilityList;
	SuperGrid *sizeGrid;
	SuperGrid *resolutionGrid;

	// The event table
	DECLARE_EVENT_TABLE()
};

#endif// _APPEARNACE_OPTIONS_DIALOG_H_