/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  appearanceOptionsDialog.cpp
// Created:  4/23/2009
// Author:  K. Loux
// Description:  A dialog object for editing the contents of the APPEARANCE_OPTIONS object.
// History:
//	5/7/2009	- Made grid columns re-sizable and started using the column labels, K. Loux.
//	10/17/2010	- Added transparency options for color definition, K. Loux.
//	11/9/2011	- Corrected use of sizers for better cross-platform usage, K. Loux.

// wxWidgets headers
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/checklst.h>
#include <wx/colordlg.h>

// VVASE headers
#include "gui/dialogs/appearanceOptionsDialog.h"
#include "gui/components/mainFrame.h"
#include "gui/superGrid.h"
#include "vRenderer/color.h"
#include "vUtilities/convert.h"

//==========================================================================
// Class:			APPEARANCE_OPTIONS_DIALOG
// Function:		APPEARANCE_OPTIONS_DIALOG
//
// Description:		Constructor for APPEARANCE_OPTIONS_DIALOG class.
//
// Input Arguments:
//		MainFrame	= MAIN_FRAME& pointing to the application's main window
//		_Options	= APPEARANCE_OPTIONS* pointing to the object to edit
//		Id			= wxWindowId for this object
//		Position	= const wxPoint& where this object will be drawn
//		Style		= long defining the style for this dialog
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
APPEARANCE_OPTIONS_DIALOG::APPEARANCE_OPTIONS_DIALOG(MAIN_FRAME &MainFrame,
													 APPEARANCE_OPTIONS *_Options,
													 wxWindowID Id, const wxPoint &Position,
													 long Style) : wxDialog(&MainFrame, Id,
													 _T("Appearance Options"), Position,
													 wxDefaultSize, Style),
													 converter(MainFrame.GetConverter())
{
	// Assign the pointer to the options object
	Options = _Options;

	// Copy the colors into our local color array
	int i;
	for (i = 0; i < APPEARANCE_OPTIONS::ColorCount; i++)
		ColorOptions[i] = Options->GetColor((APPEARANCE_OPTIONS::OBJECT_COLOR)i);

	// Create the controls
	CreateControls();

	// Center the dialog on the screen
	Center();
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS_DIALOG
// Function:		~APPEARANCE_OPTIONS_DIALOG
//
// Description:		Destructor for APPEARANCE_OPTIONS_DIALOG class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
APPEARANCE_OPTIONS_DIALOG::~APPEARANCE_OPTIONS_DIALOG()
{
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS_DIALOG
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
BEGIN_EVENT_TABLE(APPEARANCE_OPTIONS_DIALOG, wxDialog)
	EVT_BUTTON(wxID_OK,							APPEARANCE_OPTIONS_DIALOG::OKClickEvent)
	EVT_BUTTON(wxID_CANCEL,						APPEARANCE_OPTIONS_DIALOG::CancelClickEvent)
	EVT_GRID_CMD_CELL_LEFT_DCLICK(IdColorGrid,	APPEARANCE_OPTIONS_DIALOG::ColorGridDoubleClickEvent)
	EVT_GRID_CELL_CHANGE(						APPEARANCE_OPTIONS_DIALOG::AlphaChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			APPEARANCE_OPTIONS_DIALOG
// Function:		CreateControls
//
// Description:		Sets up this form's contents and size.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void APPEARANCE_OPTIONS_DIALOG::CreateControls(void)
{
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the notebook
	Notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
	MainSizer->Add(Notebook);

	// Create the color panel
	ColorPanel = new wxPanel(Notebook);
	Notebook->AddPage(ColorPanel, _T("Colors"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *ColorTopSizer = new wxBoxSizer(wxHORIZONTAL);

	// Create the color page main sizer
	wxBoxSizer *colorSizer = new wxBoxSizer(wxVERTICAL);
	ColorTopSizer->Add(colorSizer, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Add the text across the top of the page
	wxStaticText *ColorPrompt = new wxStaticText(ColorPanel, wxID_STATIC,
		_T("Edit the object colors:"));
	colorSizer->Add(ColorPrompt, 0, wxALL, 5);

	// Create the grid for the list of colors
	ColorGrid = new SuperGrid(ColorPanel, IdColorGrid);
	ColorGrid->CreateGrid(APPEARANCE_OPTIONS::ColorCount, 3, wxGrid::wxGridSelectCells);
	ColorGrid->AutoStretchColumn(0);

	// Begin a batch edit of the grid
	ColorGrid->BeginBatch();

	// Hide the label column and set the size for the label row
	ColorGrid->SetRowLabelSize(0);
	ColorGrid->SetColLabelSize(ColorGrid->GetRowSize(0));

	// Add the grid to the sizer
	colorSizer->Add(ColorGrid, 0, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL | wxALIGN_TOP, 5);

	// Set the column headings
	ColorGrid->SetColLabelValue(0, _T("Object"));
	ColorGrid->SetColLabelValue(1, _T("Color"));
	ColorGrid->SetColLabelValue(2, _T("Alpha"));

	// Do the processing that needs to be done for each row
	int i;
	for (i = 0; i < APPEARANCE_OPTIONS::ColorCount; i++)
	{
		// Make the first column read-only
		ColorGrid->SetReadOnly(i, 0, true);

		// Add the names of all of the points to the grid
		ColorGrid->SetCellValue(i, 0, APPEARANCE_OPTIONS::GetColorString(
			(APPEARANCE_OPTIONS::OBJECT_COLOR)i));

		// Set the color column's cell background to match the color of the object
		ColorGrid->SetCellBackgroundColour(i, 1, ColorOptions[i].ToWxColor());

		// Set the transparency values
		ColorGrid->SetCellValue(i, 2, converter.FormatNumber(ColorOptions[i].GetAlpha()));
	}

	// Automatically size the columns
	ColorGrid->AutoSizeColumns();

	// Don't let the user move or re-size the rows and columns
	ColorGrid->EnableDragColMove(false);
	ColorGrid->EnableDragColSize(true);
	ColorGrid->EnableDragGridSize(false);
	ColorGrid->EnableDragRowSize(false);

	// End the batch mode edit and re-paint the control
	ColorGrid->EndBatch();

	// Set the color panels's sizer
	ColorPanel->SetSizer(ColorTopSizer);

	// Create the Visibility panel
	VisibilityPanel = new wxPanel(Notebook);
	Notebook->AddPage(VisibilityPanel, _T("Visibilities"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *VisibilityTopSizer = new wxBoxSizer(wxVERTICAL);

	// Create the visibility page main sizer
	wxBoxSizer *visibilitySizer = new wxBoxSizer(wxVERTICAL);
	VisibilityTopSizer->Add(visibilitySizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL | wxEXPAND, 5);

	// Add the text to this spacer
	wxStaticText *VisibilityPrompt = new wxStaticText(VisibilityPanel, wxID_STATIC,
		_T("Choose the visible objects:"));
	visibilitySizer->Add(VisibilityPrompt, 0, wxALL | wxEXPAND, 5);

	// The selections array contains the indicies of the items that are selected
	// which is used to initialize the list in the list box
	wxArrayString Choices;
	for (i = 0; i < APPEARANCE_OPTIONS::VisibilityCount; i++)
		// Add the item to the list
		Choices.Add(APPEARANCE_OPTIONS::GetVisibilityString((APPEARANCE_OPTIONS::OBJECT_VISIBILITY)i));

	// Create the check list box
	VisibilityList = new wxCheckListBox(VisibilityPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, Choices);
	visibilitySizer->Add(VisibilityList, 0, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL | wxALIGN_TOP, 5);

	// Go through the list and check the boxes of the items that are currenly visible
	for (i = 0; i < APPEARANCE_OPTIONS::VisibilityCount; i++)
		VisibilityList->Check(i, Options->GetVisibility((APPEARANCE_OPTIONS::OBJECT_VISIBILITY)i));

	// Set the visibility panel's sizer
	VisibilityPanel->SetSizer(VisibilityTopSizer);

	// Create the Size panel
	SizePanel = new wxPanel(Notebook);
	Notebook->AddPage(SizePanel, _T("Sizes"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *SizeTopSizer = new wxBoxSizer(wxHORIZONTAL);

	// Create the size page main sizer
	wxBoxSizer *sizeSizer = new wxBoxSizer(wxVERTICAL);
	SizeTopSizer->Add(sizeSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL | wxEXPAND, 5);

	// Add the text across the top of the page
	wxStaticText *SizePrompt = new wxStaticText(SizePanel, wxID_STATIC,
		_T("Edit the object sizes (units are ") + converter.GetUnitType(Convert::UnitTypeDistance)
		+ _T("):"));
	sizeSizer->Add(SizePrompt, 0, wxALL, 5);

	// Create the grid for the list of sizes
	SizeGrid = new SuperGrid(SizePanel, wxID_ANY);
	SizeGrid->CreateGrid(APPEARANCE_OPTIONS::SizeCount, 2, wxGrid::wxGridSelectRows);
	SizeGrid->AutoStretchColumn(0);

	// Begin a batch edit of the grid
	SizeGrid->BeginBatch();

	// Hide the label column and set the size for the label row
	SizeGrid->SetRowLabelSize(0);
	SizeGrid->SetColLabelSize(SizeGrid->GetRowSize(0));

	// Add the grid to the sizer
	sizeSizer->Add(SizeGrid, 1, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL | wxALIGN_TOP, 5);

	// Set the column headings
	SizeGrid->SetColLabelValue(0, _T("Object"));
	SizeGrid->SetColLabelValue(1, _T("Size"));

	// Do the processing that needs to be done for each row
	for (i = 0; i < APPEARANCE_OPTIONS::SizeCount; i++)
	{
		// Make the first column read-only
		SizeGrid->SetReadOnly(i, 0, true);

		// Set the alignment for all of the data cells to the right
		SizeGrid->SetCellAlignment(i, 1, wxALIGN_RIGHT, wxALIGN_TOP);

		// Add the names of all of the points to the grid
		SizeGrid->SetCellValue(i, 0, APPEARANCE_OPTIONS::GetSizeString(
			(APPEARANCE_OPTIONS::OBJECT_SIZE)i));

		// Set the values of all of the data cells
		SizeGrid->SetCellValue(i, 1, converter.FormatNumber(converter.ConvertDistance(
			Options->GetSize((APPEARANCE_OPTIONS::OBJECT_SIZE)i))));
	}

	// Automatically set column widths
	SizeGrid->AutoSizeColumns();

	// Don't let the user move or re-size the rows and columns
	SizeGrid->EnableDragColMove(false);
	SizeGrid->EnableDragColSize(true);
	SizeGrid->EnableDragGridSize(false);
	SizeGrid->EnableDragRowSize(false);

	// End the batch mode edit and re-paint the control
	SizeGrid->EndBatch();

	// Set the size panels's sizer
	SizePanel->SetSizer(SizeTopSizer);

	// Create the resolution panel
	ResolutionPanel = new wxPanel(Notebook);
	Notebook->AddPage(ResolutionPanel, _T("Resolutions"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *ResolutionTopSizer = new wxBoxSizer(wxHORIZONTAL);

	// Create the resolution page main sizer
	wxBoxSizer *resolutionSizer = new wxBoxSizer(wxVERTICAL);
	ResolutionTopSizer->Add(resolutionSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL | wxEXPAND, 5);

	// Add the text across the top of the page
	wxStaticText *ResolutionPrompt = new wxStaticText(ResolutionPanel, wxID_STATIC,
		_T("Edit the object resolutions (number of sides\nto use to approximate a round object):"),
		wxDefaultPosition, wxSize(-1, -1), 0);
	resolutionSizer->Add(ResolutionPrompt, 0, wxALL, 5);

	// Create the grid for the list of resolutions
	ResolutionGrid = new SuperGrid(ResolutionPanel, wxID_ANY);
	ResolutionGrid->CreateGrid(APPEARANCE_OPTIONS::ResolutionCount, 2, wxGrid::wxGridSelectRows);
	ResolutionGrid->AutoStretchColumn(0);

	// Begin a batch edit of the grid
	ResolutionGrid->BeginBatch();

	// Hide the label rows/columns
	ResolutionGrid->SetRowLabelSize(0);
	ResolutionGrid->SetColLabelSize(ResolutionGrid->GetRowSize(0));

	// Add the grid to the sizer
	resolutionSizer->Add(ResolutionGrid, 0, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL | wxALIGN_TOP, 5);

	// Set the column headings
	ResolutionGrid->SetColLabelValue(0, _T("Object"));
	ResolutionGrid->SetColLabelValue(1, _T("Resolution"));

	// Do the processing that needs to be done for each row
	wxString ValueString;
	for (i = 0; i < APPEARANCE_OPTIONS::ResolutionCount; i++)
	{
		// Make the first column read-only
		ResolutionGrid->SetReadOnly(i, 0, true);

		// Set the alignment for all of the data cells to the right
		ResolutionGrid->SetCellAlignment(i, 1, wxALIGN_RIGHT, wxALIGN_TOP);

		// Add the names of all of the points to the grid
		ResolutionGrid->SetCellValue(i, 0, APPEARANCE_OPTIONS::GetResolutionString(
			(APPEARANCE_OPTIONS::OBJECT_RESOLUTION)i));

		// Set the values of all of the data cells
		ValueString.Printf("%i", Options->GetResolution((APPEARANCE_OPTIONS::OBJECT_RESOLUTION)i));
		ResolutionGrid->SetCellValue(i, 1, ValueString);
	}

	// Automatically set the columns widths
	ResolutionGrid->AutoSizeColumns();

	// Don't let the user move or re-size the rows and columns
	ResolutionGrid->EnableDragColMove(false);
	ResolutionGrid->EnableDragColSize(true);
	ResolutionGrid->EnableDragGridSize(false);
	ResolutionGrid->EnableDragRowSize(false);

	// End the batch mode edit and re-paint the control
	ResolutionGrid->EndBatch();

	// Set the resolution panels's sizer
	ResolutionPanel->SetSizer(ResolutionTopSizer);

	// Add a spacer between the notebook and the buttons
	MainSizer->AddSpacer(10);

	// Create another sizer for the buttons at the bottom and add the buttons
	wxBoxSizer *ButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *OKButton = new wxButton(this, wxID_OK, _T("OK"),
		wxDefaultPosition, wxDefaultSize, 0);
	wxButton *CancelButton = new wxButton(this, wxID_CANCEL, _T("Cancel"),
		wxDefaultPosition, wxDefaultSize, 0);
	ButtonsSizer->Add(OKButton, 0, wxALL, 5);
	ButtonsSizer->Add(CancelButton, 0, wxALL, 5);
	MainSizer->Add(ButtonsSizer, 0, wxALIGN_CENTER_HORIZONTAL);

	// Make the OK button default
	OKButton->SetDefault();

	// Tell the dialog to auto-adjust it's size
	TopSizer->SetSizeHints(this);

	// Assign the top level sizer to the dialog
	SetSizer(TopSizer);

	return;
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS_DIALOG
// Function:		OKClickEvent
//
// Description:		Handles the OK button clicked events
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void APPEARANCE_OPTIONS_DIALOG::OKClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Update the appearance options object with the contents of this dialog

	// Update the colors
	int i;
	double TempAlpha;
	for (i = 0; i < APPEARANCE_OPTIONS::ColorCount; i++)
	{
		// Get the alpha before updating the color (wx-native dialog does not handle alpha)
		if (ColorGrid->GetCellValue(i, 2).ToDouble(&TempAlpha))
			ColorOptions[i].SetAlpha(TempAlpha);
		Options->SetColor((APPEARANCE_OPTIONS::OBJECT_COLOR)i, ColorOptions[i]);
	}

	// Update the visibilities
	for (i = 0; i < APPEARANCE_OPTIONS::VisibilityCount; i++)
		Options->SetVisibility((APPEARANCE_OPTIONS::OBJECT_VISIBILITY)i, VisibilityList->IsChecked(i));

	// Update the sizes
	double SizeValue;
	for (i = 0; i < APPEARANCE_OPTIONS::SizeCount; i++)
	{
		// Size must be a valid number
		if (SizeGrid->GetCellValue(i, 1).ToDouble(&SizeValue))
			Options->SetSize((APPEARANCE_OPTIONS::OBJECT_SIZE)i, converter.ReadDistance(SizeValue));
	}

	// Update the resolutions
	long ResolutionValue;
	for (i = 0; i < APPEARANCE_OPTIONS::ResolutionCount; i++)
	{
		// Resolution must be a valid number)
		if (ResolutionGrid->GetCellValue(i, 1).ToLong(&ResolutionValue))	
			Options->SetResolution((APPEARANCE_OPTIONS::OBJECT_RESOLUTION)i, (int)ResolutionValue);
	}

	// The way we handle this changes depending on how this form was displayed
	if (IsModal())
		EndModal(wxOK);
	else
	{
		SetReturnCode(wxOK);
		Show(false);
	}

	return;
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS_DIALOG
// Function:		CancelClickEvent
//
// Description:		Handles the Cancel button clicked event.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void APPEARANCE_OPTIONS_DIALOG::CancelClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// The way we handle this changes depending on how this form was displayed
	if (IsModal())
		EndModal(wxID_CANCEL);
	else
	{
		SetReturnCode(wxID_CANCEL);
		Show(false);
	}

	return;
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS_DIALOG
// Function:		ColorGridDoubleClickEvent
//
// Description:		Handles the color grid double click events.
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void APPEARANCE_OPTIONS_DIALOG::ColorGridDoubleClickEvent(wxGridEvent &event)
{
	// Get the color information that corresponds to this row, and put it in wxColor object,
	// which in turn goes into a wxColourData object
	wxColourData ColorData;
	ColorData.SetColour(ColorOptions[event.GetRow()].ToWxColor());

	// Display a color dialog
	wxColourDialog ColorDialog(this, &ColorData);

	// Display the dialog and check to see if the user clicked OK or Cancel
	if (ColorDialog.ShowModal() == wxID_OK)
	{
		// Assign this color to the CurrentColor
		ColorOptions[event.GetRow()].Set(ColorDialog.GetColourData().GetColour());

		// Set the cell background to the new color
		ColorGrid->SetCellBackgroundColour(event.GetRow(), 1, ColorDialog.GetColourData().GetColour());

		// Re-paint the grid
		ColorGrid->Refresh();
		ColorGrid->Update();
	}

	return;
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS_DIALOG
// Function:		AlphaChangeEvent
//
// Description:		Handles the color grid text change events.
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void APPEARANCE_OPTIONS_DIALOG::AlphaChangeEvent(wxGridEvent &event)
{
	// Make sure this is a valid row and colum to have changed
	if (event.GetCol() != 2)
		return;

	// Make sure the value is between 0 and 1
	double TempAlpha;
	if (ColorGrid->GetCellValue(event.GetRow(), event.GetCol()).ToDouble(&TempAlpha))
	{
		if (TempAlpha > 1.0)
			ColorGrid->SetCellValue(event.GetRow(), event.GetCol(), converter.FormatNumber(1.0));
		else if (TempAlpha < 0.0)
			ColorGrid->SetCellValue(event.GetRow(), event.GetCol(), converter.FormatNumber(0.0));
	}
	else
		ColorGrid->SetCellValue(event.GetRow(), event.GetCol(),
		converter.FormatNumber(ColorOptions[event.GetRow()].GetAlpha()));

	return;
}