/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  appearanceOptionsDialog.cpp
// Date:  4/23/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  A dialog object for editing the contents of the AppearanceOptions object.

// wxWidgets headers
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/checklst.h>
#include <wx/colordlg.h>

// Local headers
#include "gui/dialogs/appearanceOptionsDialog.h"
#include "gui/components/mainFrame.h"
#include "gui/superGrid.h"
#include "vRenderer/color.h"
#include "vUtilities/unitConverter.h"

namespace VVASE
{

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
	AppearanceOptions *options, wxWindowID id, const wxPoint &position,
	long style) : wxDialog(&mainFrame, id, _T("Appearance Options"), position,
	wxDefaultSize, style)
{
	this->options = options;

	// Copy the colors into our local color array
	int i;
	for (i = 0; i < AppearanceOptions::ColorCount; i++)
		colorOptions[i] = options->GetColor((AppearanceOptions::ObjectColor)i);

	CreateControls();
	Center();
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
	EVT_GRID_CELL_CHANGED(						AppearanceOptionsDialog::AlphaChangeEvent)
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
void AppearanceOptionsDialog::CreateControls()
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
	mainSizer->Add(notebook);

	colorPanel = new wxPanel(notebook);
	notebook->AddPage(colorPanel, _T("Colors"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *colorTopSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer *colorSizer = new wxBoxSizer(wxVERTICAL);
	colorTopSizer->Add(colorSizer, 0, wxEXPAND | wxALL, 5);

	wxStaticText *colorPrompt = new wxStaticText(colorPanel, wxID_STATIC,
		_T("Edit the object colors:"));
	colorSizer->Add(colorPrompt, 0, wxALL, 5);

	colorGrid = new SuperGrid(colorPanel, IdColorGrid);
	colorGrid->CreateGrid(AppearanceOptions::ColorCount, 3, wxGrid::wxGridSelectCells);
	colorGrid->AutoStretchColumn(0);

	colorGrid->BeginBatch();

	// Hide the label column and set the size for the label row
	colorGrid->SetRowLabelSize(0);
	colorGrid->SetColLabelSize(colorGrid->GetRowSize(0));

	colorSizer->Add(colorGrid, 0, wxEXPAND | wxALL | wxALIGN_TOP, 5);

	// Set the column headings
	colorGrid->SetColLabelValue(0, _T("Object"));
	colorGrid->SetColLabelValue(1, _T("Color"));
	colorGrid->SetColLabelValue(2, _T("Alpha"));

	int i;
	for (i = 0; i < AppearanceOptions::ColorCount; i++)
	{
		colorGrid->SetReadOnly(i, 0, true);
		colorGrid->SetCellValue(i, 0, AppearanceOptions::GetColorString(
			(AppearanceOptions::ObjectColor)i));

		colorGrid->SetCellBackgroundColour(i, 1, colorOptions[i].ToWxColor());
		colorGrid->SetCellValue(i, 2, UnitConverter::GetInstance().FormatNumber(colorOptions[i].GetAlpha()));
	}

	colorGrid->AutoSizeColumns();

	colorGrid->EnableDragColMove(false);
	colorGrid->EnableDragColSize(true);
	colorGrid->EnableDragGridSize(false);
	colorGrid->EnableDragRowSize(false);


	colorGrid->EndBatch();
	colorPanel->SetSizer(colorTopSizer);

	visibilityPanel = new wxPanel(notebook);
	notebook->AddPage(visibilityPanel, _T("Visibilities"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *visibilityTopSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *visibilitySizer = new wxBoxSizer(wxVERTICAL);
	visibilityTopSizer->Add(visibilitySizer, 1, wxALL | wxEXPAND, 5);

	wxStaticText *visibilityPrompt = new wxStaticText(visibilityPanel, wxID_STATIC,
		_T("Choose the visible objects:"));
	visibilitySizer->Add(visibilityPrompt, 0, wxALL | wxEXPAND, 5);

	// The selections array contains the indices of the items that are selected
	// which is used to initialize the list in the list box
	wxArrayString choices;
	for (i = 0; i < AppearanceOptions::VisibilityCount; i++)
		choices.Add(AppearanceOptions::GetVisibilityString((AppearanceOptions::ObjectVisibility)i));

	visibilityList = new wxCheckListBox(visibilityPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
	visibilitySizer->Add(visibilityList, 1, wxEXPAND | wxALL | wxALIGN_TOP, 5);

	for (i = 0; i < AppearanceOptions::VisibilityCount; i++)
		// TODO:  Can DataValidator work with booleans?
		visibilityList->Check(i, options->GetVisibility((AppearanceOptions::ObjectVisibility)i));

	visibilityPanel->SetSizer(visibilityTopSizer);

	sizePanel = new wxPanel(notebook);
	notebook->AddPage(sizePanel, _T("Sizes"));

	wxBoxSizer *sizeTopSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer *sizeSizer = new wxBoxSizer(wxVERTICAL);
	sizeTopSizer->Add(sizeSizer, 0, wxALL | wxEXPAND, 5);

	wxStaticText *sizePrompt = new wxStaticText(sizePanel, wxID_STATIC,
		_T("Edit the object sizes (units are ") + UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance)
		+ _T("):"));
	sizeSizer->Add(sizePrompt, 0, wxALL, 5);

	// Create the grid for the list of sizes
	sizeGrid = new SuperGrid(sizePanel, wxID_ANY);
	sizeGrid->CreateGrid(AppearanceOptions::SizeCount, 2, wxGrid::wxGridSelectRows);
	sizeGrid->AutoStretchColumn(0);

	sizeGrid->BeginBatch();

	// Hide the label column and set the size for the label row
	sizeGrid->SetRowLabelSize(0);
	sizeGrid->SetColLabelSize(sizeGrid->GetRowSize(0));

	sizeSizer->Add(sizeGrid, 1, wxEXPAND | wxALL | wxALIGN_TOP, 5);

	sizeGrid->SetColLabelValue(0, _T("Object"));
	sizeGrid->SetColLabelValue(1, _T("Size"));

	for (i = 0; i < AppearanceOptions::SizeCount; i++)
	{
		sizeGrid->SetReadOnly(i, 0, true);
		sizeGrid->SetCellAlignment(i, 1, wxALIGN_RIGHT, wxALIGN_TOP);
		sizeGrid->SetCellValue(i, 0, AppearanceOptions::GetSizeString(
			(AppearanceOptions::ObjectSize)i));

		//sizeGrid->SetCellValidator(i, 1, UnitValidator(*(options->GetSizePointer() + i), UnitConverter::UnitsOfDistance));// TODO:  Implement something like this
		sizeGrid->SetCellValue(i, 1, UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
			options->GetSize((AppearanceOptions::ObjectSize)i))));
	}

	sizeGrid->AutoSizeColumns();

	sizeGrid->EnableDragColMove(false);
	sizeGrid->EnableDragColSize(true);
	sizeGrid->EnableDragGridSize(false);
	sizeGrid->EnableDragRowSize(false);

	sizeGrid->EndBatch();

	sizePanel->SetSizer(sizeTopSizer);

	resolutionPanel = new wxPanel(notebook);
	notebook->AddPage(resolutionPanel, _T("Resolutions"));

	// Use another outer sizer to create more room for the controls
	wxBoxSizer *resolutionTopSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer *resolutionSizer = new wxBoxSizer(wxVERTICAL);
	resolutionTopSizer->Add(resolutionSizer, 0, wxALL | wxEXPAND, 5);

	wxStaticText *resolutionPrompt = new wxStaticText(resolutionPanel, wxID_STATIC,
		_T("Edit the object resolutions (number of sides\nto use to approximate a round object):"),
		wxDefaultPosition, wxSize(-1, -1), 0);
	resolutionSizer->Add(resolutionPrompt, 0, wxALL, 5);

	// Create the grid for the list of resolutions
	resolutionGrid = new SuperGrid(resolutionPanel, wxID_ANY);
	resolutionGrid->CreateGrid(AppearanceOptions::ResolutionCount, 2, wxGrid::wxGridSelectRows);
	resolutionGrid->AutoStretchColumn(0);

	resolutionGrid->BeginBatch();

	// Hide the label rows/columns
	resolutionGrid->SetRowLabelSize(0);
	resolutionGrid->SetColLabelSize(resolutionGrid->GetRowSize(0));

	// Add the grid to the sizer
	resolutionSizer->Add(resolutionGrid, 0, wxEXPAND | wxALL | wxALIGN_TOP, 5);

	// Set the column headings
	resolutionGrid->SetColLabelValue(0, _T("Object"));
	resolutionGrid->SetColLabelValue(1, _T("Resolution"));

	wxString valueString;
	for (i = 0; i < AppearanceOptions::ResolutionCount; i++)
	{
		resolutionGrid->SetReadOnly(i, 0, true);
		resolutionGrid->SetCellAlignment(i, 1, wxALIGN_RIGHT, wxALIGN_TOP);
		//resolutionGrid->SetCellValidator(i, 0, UnsignedValidator(*(options->GetResolutionPointer() + i), 3, 0, UnsignedValidator::ClassMinimumInclusive));// TODO:  Implement something like this
		resolutionGrid->SetCellValue(i, 0, AppearanceOptions::GetResolutionString(
			(AppearanceOptions::ObjectResolution)i));
		valueString.Printf("%i", options->GetResolution((AppearanceOptions::ObjectResolution)i));
		resolutionGrid->SetCellValue(i, 1, valueString);
	}

	resolutionGrid->AutoSizeColumns();

	resolutionGrid->EnableDragColMove(false);
	resolutionGrid->EnableDragColSize(true);
	resolutionGrid->EnableDragGridSize(false);
	resolutionGrid->EnableDragRowSize(false);

	resolutionGrid->EndBatch();

	resolutionPanel->SetSizer(resolutionTopSizer);

	// Add a spacer between the notebook and the buttons
	mainSizer->AddSpacer(10);

	// Create another sizer for the buttons at the bottom
	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxGROW);

	topSizer->SetSizeHints(this);
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
			options->SetSize((AppearanceOptions::ObjectSize)i, UnitConverter::GetInstance().ConvertDistanceInput(sizeValue));
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
			colorGrid->SetCellValue(event.GetRow(), event.GetCol(), UnitConverter::GetInstance().FormatNumber(1.0));
		else if (tempAlpha < 0.0)
			colorGrid->SetCellValue(event.GetRow(), event.GetCol(), UnitConverter::GetInstance().FormatNumber(0.0));
	}
	else
		colorGrid->SetCellValue(event.GetRow(), event.GetCol(),
		UnitConverter::GetInstance().FormatNumber(colorOptions[event.GetRow()].GetAlpha()));
}

}// namespace VVASE
