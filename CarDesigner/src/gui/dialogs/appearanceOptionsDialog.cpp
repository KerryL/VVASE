/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  appearanceOptionsDialog.cpp
// Created:  4/23/2009
// Author:  K. Loux
// Description:  A dialog object for editing the contents of the AppearanceOptions object.
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
// Class:			AppearanceOptionsDialog
// Function:		AppearanceOptionsDialog
//
// Description:		Constructor for AppearanceOptionsDialog class.
//
// Input Arguments:
//		mainFrame	= MainFrame& pointing to the application's main window
//		_options	= AppearanceOptions* pointing to the object to edit
//		id			= wxWindowId for this object
//		position	= const wxPoint& where this object will be drawn
//		style		= long defining the style for this dialog
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
AppearanceOptionsDialog::AppearanceOptionsDialog(MainFrame &mainFrame,
													 AppearanceOptions *_options,
													 wxWindowID id, const wxPoint &position,
													 long style) : wxDialog(&mainFrame, id,
													 _T("Appearance Options"), position,
													 wxDefaultSize, style)
{
	// Assign the pointer to the options object
	options = _options;

	// Copy the colors into our local color array
	int i;
	for (i = 0; i < AppearanceOptions::ColorCount; i++)
		colorOptions[i] = options->GetColor((AppearanceOptions::ObjectColor)i);

	// Create the controls
	CreateControls();

	// Center the dialog on the screen
	Center();
}

//==========================================================================
// Class:			AppearanceOptionsDialog
// Function:		~AppearanceOptionsDialog
//
// Description:		Destructor for AppearanceOptionsDialog class.
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
AppearanceOptionsDialog::~AppearanceOptionsDialog()
{
}

//==========================================================================
// Class:			AppearanceOptionsDialog
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
BEGIN_EVENT_TABLE(AppearanceOptionsDialog, wxDialog)
	EVT_BUTTON(wxID_OK,							AppearanceOptionsDialog::OKClickEvent)
	EVT_BUTTON(wxID_CANCEL,						AppearanceOptionsDialog::CancelClickEvent)
	EVT_GRID_CMD_CELL_LEFT_DCLICK(IdColorGrid,	AppearanceOptionsDialog::ColorGridDoubleClickEvent)
	EVT_GRID_CELL_CHANGE(						AppearanceOptionsDialog::AlphaChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			AppearanceOptionsDialog
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
void AppearanceOptionsDialog::CreateControls(void)
{
	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Create the notebook
	notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
	mainSizer->Add(notebook);

	// Create the color panel
	colorPanel = new wxPanel(notebook);
	notebook->AddPage(colorPanel, _T("Colors"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *colorTopSizer = new wxBoxSizer(wxHORIZONTAL);

	// Create the color page main sizer
	wxBoxSizer *colorSizer = new wxBoxSizer(wxVERTICAL);
	colorTopSizer->Add(colorSizer, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Add the text across the top of the page
	wxStaticText *colorPrompt = new wxStaticText(colorPanel, wxID_STATIC,
		_T("Edit the object colors:"));
	colorSizer->Add(colorPrompt, 0, wxALL, 5);

	// Create the grid for the list of colors
	colorGrid = new SuperGrid(colorPanel, IdColorGrid);
	colorGrid->CreateGrid(AppearanceOptions::ColorCount, 3, wxGrid::wxGridSelectCells);
	colorGrid->AutoStretchColumn(0);

	// Begin a batch edit of the grid
	colorGrid->BeginBatch();

	// Hide the label column and set the size for the label row
	colorGrid->SetRowLabelSize(0);
	colorGrid->SetColLabelSize(colorGrid->GetRowSize(0));

	// Add the grid to the sizer
	colorSizer->Add(colorGrid, 0, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL | wxALIGN_TOP, 5);

	// Set the column headings
	colorGrid->SetColLabelValue(0, _T("Object"));
	colorGrid->SetColLabelValue(1, _T("Color"));
	colorGrid->SetColLabelValue(2, _T("Alpha"));

	// Do the processing that needs to be done for each row
	int i;
	for (i = 0; i < AppearanceOptions::ColorCount; i++)
	{
		// Make the first column read-only
		colorGrid->SetReadOnly(i, 0, true);

		// Add the names of all of the points to the grid
		colorGrid->SetCellValue(i, 0, AppearanceOptions::GetColorString(
			(AppearanceOptions::ObjectColor)i));

		// Set the color column's cell background to match the color of the object
		colorGrid->SetCellBackgroundColour(i, 1, colorOptions[i].ToWxColor());

		// Set the transparency values
		colorGrid->SetCellValue(i, 2, Convert::GetInstance().FormatNumber(colorOptions[i].GetAlpha()));
	}

	// Automatically size the columns
	colorGrid->AutoSizeColumns();

	// Don't let the user move or re-size the rows and columns
	colorGrid->EnableDragColMove(false);
	colorGrid->EnableDragColSize(true);
	colorGrid->EnableDragGridSize(false);
	colorGrid->EnableDragRowSize(false);

	// End the batch mode edit and re-paint the control
	colorGrid->EndBatch();

	// Set the color panels's sizer
	colorPanel->SetSizer(colorTopSizer);

	// Create the Visibility panel
	visibilityPanel = new wxPanel(notebook);
	notebook->AddPage(visibilityPanel, _T("Visibilities"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *visibilityTopSizer = new wxBoxSizer(wxVERTICAL);

	// Create the visibility page main sizer
	wxBoxSizer *visibilitySizer = new wxBoxSizer(wxVERTICAL);
	visibilityTopSizer->Add(visibilitySizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL | wxEXPAND, 5);

	// Add the text to this spacer
	wxStaticText *visibilityPrompt = new wxStaticText(visibilityPanel, wxID_STATIC,
		_T("Choose the visible objects:"));
	visibilitySizer->Add(visibilityPrompt, 0, wxALL | wxEXPAND, 5);

	// The selections array contains the indices of the items that are selected
	// which is used to initialize the list in the list box
	wxArrayString choices;
	for (i = 0; i < AppearanceOptions::VisibilityCount; i++)
		// Add the item to the list
		choices.Add(AppearanceOptions::GetVisibilityString((AppearanceOptions::ObjectVisibility)i));

	// Create the check list box
	visibilityList = new wxCheckListBox(visibilityPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
	visibilitySizer->Add(visibilityList, 0, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL | wxALIGN_TOP, 5);

	// Go through the list and check the boxes of the items that are currently visible
	for (i = 0; i < AppearanceOptions::VisibilityCount; i++)
		visibilityList->Check(i, options->GetVisibility((AppearanceOptions::ObjectVisibility)i));

	// Set the visibility panel's sizer
	visibilityPanel->SetSizer(visibilityTopSizer);

	// Create the Size panel
	sizePanel = new wxPanel(notebook);
	notebook->AddPage(sizePanel, _T("Sizes"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *sizeTopSizer = new wxBoxSizer(wxHORIZONTAL);

	// Create the size page main sizer
	wxBoxSizer *sizeSizer = new wxBoxSizer(wxVERTICAL);
	sizeTopSizer->Add(sizeSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL | wxEXPAND, 5);

	// Add the text across the top of the page
	wxStaticText *sizePrompt = new wxStaticText(sizePanel, wxID_STATIC,
		_T("Edit the object sizes (units are ") + Convert::GetInstance().GetUnitType(Convert::UnitTypeDistance)
		+ _T("):"));
	sizeSizer->Add(sizePrompt, 0, wxALL, 5);

	// Create the grid for the list of sizes
	sizeGrid = new SuperGrid(sizePanel, wxID_ANY);
	sizeGrid->CreateGrid(AppearanceOptions::SizeCount, 2, wxGrid::wxGridSelectRows);
	sizeGrid->AutoStretchColumn(0);

	// Begin a batch edit of the grid
	sizeGrid->BeginBatch();

	// Hide the label column and set the size for the label row
	sizeGrid->SetRowLabelSize(0);
	sizeGrid->SetColLabelSize(sizeGrid->GetRowSize(0));

	// Add the grid to the sizer
	sizeSizer->Add(sizeGrid, 1, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL | wxALIGN_TOP, 5);

	// Set the column headings
	sizeGrid->SetColLabelValue(0, _T("Object"));
	sizeGrid->SetColLabelValue(1, _T("Size"));

	// Do the processing that needs to be done for each row
	for (i = 0; i < AppearanceOptions::SizeCount; i++)
	{
		// Make the first column read-only
		sizeGrid->SetReadOnly(i, 0, true);

		// Set the alignment for all of the data cells to the right
		sizeGrid->SetCellAlignment(i, 1, wxALIGN_RIGHT, wxALIGN_TOP);

		// Add the names of all of the points to the grid
		sizeGrid->SetCellValue(i, 0, AppearanceOptions::GetSizeString(
			(AppearanceOptions::ObjectSize)i));

		// Set the values of all of the data cells
		sizeGrid->SetCellValue(i, 1, Convert::GetInstance().FormatNumber(Convert::GetInstance().ConvertDistance(
			options->GetSize((AppearanceOptions::ObjectSize)i))));
	}

	// Automatically set column widths
	sizeGrid->AutoSizeColumns();

	// Don't let the user move or re-size the rows and columns
	sizeGrid->EnableDragColMove(false);
	sizeGrid->EnableDragColSize(true);
	sizeGrid->EnableDragGridSize(false);
	sizeGrid->EnableDragRowSize(false);

	// End the batch mode edit and re-paint the control
	sizeGrid->EndBatch();

	// Set the size panels's sizer
	sizePanel->SetSizer(sizeTopSizer);

	// Create the resolution panel
	resolutionPanel = new wxPanel(notebook);
	notebook->AddPage(resolutionPanel, _T("Resolutions"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *resolutionTopSizer = new wxBoxSizer(wxHORIZONTAL);

	// Create the resolution page main sizer
	wxBoxSizer *resolutionSizer = new wxBoxSizer(wxVERTICAL);
	resolutionTopSizer->Add(resolutionSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL | wxEXPAND, 5);

	// Add the text across the top of the page
	wxStaticText *resolutionPrompt = new wxStaticText(resolutionPanel, wxID_STATIC,
		_T("Edit the object resolutions (number of sides\nto use to approximate a round object):"),
		wxDefaultPosition, wxSize(-1, -1), 0);
	resolutionSizer->Add(resolutionPrompt, 0, wxALL, 5);

	// Create the grid for the list of resolutions
	resolutionGrid = new SuperGrid(resolutionPanel, wxID_ANY);
	resolutionGrid->CreateGrid(AppearanceOptions::ResolutionCount, 2, wxGrid::wxGridSelectRows);
	resolutionGrid->AutoStretchColumn(0);

	// Begin a batch edit of the grid
	resolutionGrid->BeginBatch();

	// Hide the label rows/columns
	resolutionGrid->SetRowLabelSize(0);
	resolutionGrid->SetColLabelSize(resolutionGrid->GetRowSize(0));

	// Add the grid to the sizer
	resolutionSizer->Add(resolutionGrid, 0, wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxALL | wxALIGN_TOP, 5);

	// Set the column headings
	resolutionGrid->SetColLabelValue(0, _T("Object"));
	resolutionGrid->SetColLabelValue(1, _T("Resolution"));

	// Do the processing that needs to be done for each row
	wxString valueString;
	for (i = 0; i < AppearanceOptions::ResolutionCount; i++)
	{
		// Make the first column read-only
		resolutionGrid->SetReadOnly(i, 0, true);

		// Set the alignment for all of the data cells to the right
		resolutionGrid->SetCellAlignment(i, 1, wxALIGN_RIGHT, wxALIGN_TOP);

		// Add the names of all of the points to the grid
		resolutionGrid->SetCellValue(i, 0, AppearanceOptions::GetResolutionString(
			(AppearanceOptions::ObjectResolution)i));

		// Set the values of all of the data cells
		valueString.Printf("%i", options->GetResolution((AppearanceOptions::ObjectResolution)i));
		resolutionGrid->SetCellValue(i, 1, valueString);
	}

	// Automatically set the columns widths
	resolutionGrid->AutoSizeColumns();

	// Don't let the user move or re-size the rows and columns
	resolutionGrid->EnableDragColMove(false);
	resolutionGrid->EnableDragColSize(true);
	resolutionGrid->EnableDragGridSize(false);
	resolutionGrid->EnableDragRowSize(false);

	// End the batch mode edit and re-paint the control
	resolutionGrid->EndBatch();

	// Set the resolution panels's sizer
	resolutionPanel->SetSizer(resolutionTopSizer);

	// Add a spacer between the notebook and the buttons
	mainSizer->AddSpacer(10);

	// Create another sizer for the buttons at the bottom and add the buttons
	wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *okButton = new wxButton(this, wxID_OK, _T("OK"),
		wxDefaultPosition, wxDefaultSize, 0);
	wxButton *cancelButton = new wxButton(this, wxID_CANCEL, _T("Cancel"),
		wxDefaultPosition, wxDefaultSize, 0);
	buttonsSizer->Add(okButton, 0, wxALL, 5);
	buttonsSizer->Add(cancelButton, 0, wxALL, 5);
	mainSizer->Add(buttonsSizer, 0, wxALIGN_CENTER_HORIZONTAL);

	// Make the OK button default
	okButton->SetDefault();

	// Tell the dialog to auto-adjust it's size
	topSizer->SetSizeHints(this);

	// Assign the top level sizer to the dialog
	SetSizer(topSizer);
}

//==========================================================================
// Class:			AppearanceOptionsDialog
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
void AppearanceOptionsDialog::OKClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// Update the appearance options object with the contents of this dialog

	// Update the colors
	int i;
	double tempAlpha;
	for (i = 0; i < AppearanceOptions::ColorCount; i++)
	{
		// Get the alpha before updating the color (wx-native dialog does not handle alpha)
		if (colorGrid->GetCellValue(i, 2).ToDouble(&tempAlpha))
			colorOptions[i].SetAlpha(tempAlpha);
		options->SetColor((AppearanceOptions::ObjectColor)i, colorOptions[i]);
	}

	// Update the visibilities
	for (i = 0; i < AppearanceOptions::VisibilityCount; i++)
		options->SetVisibility((AppearanceOptions::ObjectVisibility)i, visibilityList->IsChecked(i));

	// Update the sizes
	double sizeValue;
	for (i = 0; i < AppearanceOptions::SizeCount; i++)
	{
		// Size must be a valid number
		if (sizeGrid->GetCellValue(i, 1).ToDouble(&sizeValue))
			options->SetSize((AppearanceOptions::ObjectSize)i, Convert::GetInstance().ReadDistance(sizeValue));
	}

	// Update the resolutions
	long resolutionValue;
	for (i = 0; i < AppearanceOptions::ResolutionCount; i++)
	{
		// Resolution must be a valid number)
		if (resolutionGrid->GetCellValue(i, 1).ToLong(&resolutionValue))	
			options->SetResolution((AppearanceOptions::ObjectResolution)i, (int)resolutionValue);
	}

	// The way we handle this changes depending on how this form was displayed
	if (IsModal())
		EndModal(wxOK);
	else
	{
		SetReturnCode(wxOK);
		Show(false);
	}
}

//==========================================================================
// Class:			AppearanceOptionsDialog
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
void AppearanceOptionsDialog::CancelClickEvent(wxCommandEvent& WXUNUSED(event))
{
	// The way we handle this changes depending on how this form was displayed
	if (IsModal())
		EndModal(wxID_CANCEL);
	else
	{
		SetReturnCode(wxID_CANCEL);
		Show(false);
	}
}

//==========================================================================
// Class:			AppearanceOptionsDialog
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
void AppearanceOptionsDialog::ColorGridDoubleClickEvent(wxGridEvent &event)
{
	// Get the color information that corresponds to this row, and put it in wxColor object,
	// which in turn goes into a wxColourData object
	wxColourData colorData;
	colorData.SetColour(colorOptions[event.GetRow()].ToWxColor());

	// Display a color dialog
	wxColourDialog colorDialog(this, &colorData);

	// Display the dialog and check to see if the user clicked OK or Cancel
	if (colorDialog.ShowModal() == wxID_OK)
	{
		// Assign this color to the CurrentColor
		colorOptions[event.GetRow()].Set(colorDialog.GetColourData().GetColour());

		// Set the cell background to the new color
		colorGrid->SetCellBackgroundColour(event.GetRow(), 1, colorDialog.GetColourData().GetColour());

		// Re-paint the grid
		colorGrid->Refresh();
		colorGrid->Update();
	}
}

//==========================================================================
// Class:			AppearanceOptionsDialog
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
void AppearanceOptionsDialog::AlphaChangeEvent(wxGridEvent &event)
{
	// Make sure this is a valid row and colum to have changed
	if (event.GetCol() != 2)
		return;

	// Make sure the value is between 0 and 1
	double tempAlpha;
	if (colorGrid->GetCellValue(event.GetRow(), event.GetCol()).ToDouble(&tempAlpha))
	{
		if (tempAlpha > 1.0)
			colorGrid->SetCellValue(event.GetRow(), event.GetCol(), Convert::GetInstance().FormatNumber(1.0));
		else if (tempAlpha < 0.0)
			colorGrid->SetCellValue(event.GetRow(), event.GetCol(), Convert::GetInstance().FormatNumber(0.0));
	}
	else
		colorGrid->SetCellValue(event.GetRow(), event.GetCol(),
		Convert::GetInstance().FormatNumber(colorOptions[event.GetRow()].GetAlpha()));
}