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

#ifndef _APPEARNACE_OPTIONS_DIALOG_CLASS_H_
#define _APPEARNACE_OPTIONS_DIALOG_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "gui/appearanceOptions.h"

// wxWidgets forward declarations
class wxGridEvent;
class wxNotebook;

// VVASE forward declarations
class MAIN_FRAME;
class SuperGrid;
class Convert;
class Color;

class APPEARANCE_OPTIONS_DIALOG : public wxDialog
{
public:
	// Constructor
	APPEARANCE_OPTIONS_DIALOG(MAIN_FRAME &MainFrame, APPEARANCE_OPTIONS *_Options,
		wxWindowID Id, const wxPoint &Position, long Style = wxDEFAULT_DIALOG_STYLE);

	// Destructor
	~APPEARANCE_OPTIONS_DIALOG();

private:
	// Sets up the size and position of this dialog and its contents
	void CreateControls(void);

	// The object that handles the unit conversions between the input and output
	const Convert &converter;

	// The object to edit
	APPEARANCE_OPTIONS *Options;

	// A copy of the color array
	Color ColorOptions[APPEARANCE_OPTIONS::ColorCount];

	// The event IDs
	enum EVENT_ID
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
	wxNotebook *Notebook;

	wxPanel *ColorPanel;
	wxPanel *VisibilityPanel;
	wxPanel *SizePanel;
	wxPanel *ResolutionPanel;

	SuperGrid *ColorGrid;
	wxCheckListBox *VisibilityList;
	SuperGrid *SizeGrid;
	SuperGrid *ResolutionGrid;

	// The event table
	DECLARE_EVENT_TABLE()
};

#endif// _APPEARNACE_OPTIONS_DIALOG_CLASS_H_