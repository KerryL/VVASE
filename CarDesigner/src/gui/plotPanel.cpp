/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotPanel.cpp
// Created:  11/9/2011
// Author:  K. Loux
// Description:  Panel containing plot renderer and grid listing active plot channels.
// History:

// Local headers
#include "gui/plotPanel.h"
#include "gui/renderer/plotRenderer.h"

// Standard C++ headers
#include <fstream>
#include <vector>
#include <algorithm>

// wxWidgets headers
#include <wx/grid.h>
#include <wx/colordlg.h>

// Local headers
#include "gui/renderer/plotRenderer.h"
#include "gui/dialogs/rangeLimitsDialog.h"
#include "vRenderer/color.h"
#include "vMath/carMath.h"
#include "vMath/signals/integral.h"
#include "vMath/signals/derivative.h"
#include "vMath/signals/rms.h"
#include "vMath/signals/fft.h"
#include "vMath/expressionTree.h"
#include "vMath/signals/filters/lowPassOrder1.h"
#include "vMath/signals/filters/highPassOrder1.h"
#include "vMath/signals/curveFit.h"
#include "vUtilities/debugger.h"

//==========================================================================
// Class:			PlotPanel
// Function:		PlotPanel
//
// Description:		Constructor for PlotPanel class.  Initializes the panel
//					and creates the controls, etc.
//
// Input Arguments:
//		parent		= wxWindow*
//		_debugger	= const Debugger&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PlotPanel::PlotPanel(wxWindow *parent, const Debugger &_debugger) : wxPanel(parent), debugger(_debugger)
{
	CreateControls();
}

//==========================================================================
// Class:			PlotPanel
// Function:		~PlotPanel
//
// Description:		Destructor for PlotPanel class.
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
PlotPanel::~PlotPanel()
{
}

//==========================================================================
// Class:			PlotPanel
// Function:		CreateControls
//
// Description:		Creates the objects on the panel.
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
void PlotPanel::CreateControls(void)
{
	// Create the top sizer, and on inside of it just to pad the borders a bit
	topSizer = new wxBoxSizer(wxHORIZONTAL);
	wxFlexGridSizer *mainSizer = new wxFlexGridSizer(1, 5, 5);
	topSizer->Add(mainSizer, 0, wxEXPAND | wxALL, 5);
	mainSizer->AddGrowableRow(0);

	// Create the main control
	optionsGrid = NULL;// To avoid crashing in UpdateCursors
	int args[] = {WX_GL_DOUBLEBUFFER, 0};
	renderer = new PlotRenderer(*this, wxID_ANY, args, debugger);
	renderer->SetGridOn();
	mainSizer->Add(renderer, 0, wxEXPAND);

	// Create the options control
	optionsGrid = new wxGrid(this, wxID_ANY);
	mainSizer->Add(optionsGrid, 0, wxEXPAND | wxALIGN_BOTTOM | wxALL, 5);

	// Configure the grid
	optionsGrid->BeginBatch();

	optionsGrid->CreateGrid(0, colCount, wxGrid::wxGridSelectRows);
	optionsGrid->SetRowLabelSize(0);
	optionsGrid->SetColFormatNumber(colSize);
	optionsGrid->SetColFormatFloat(colLeftCursor);
	optionsGrid->SetColFormatFloat(colRightCursor);
	optionsGrid->SetColFormatFloat(colDifference);
	optionsGrid->SetColFormatBool(colVisible);
	optionsGrid->SetColFormatBool(colRightAxis);

	optionsGrid->SetColLabelValue(colName, _T("Curve"));
	optionsGrid->SetColLabelValue(colColor, _T("Color"));
	optionsGrid->SetColLabelValue(colSize, _T("Size"));
	optionsGrid->SetColLabelValue(colLeftCursor, _T("Left Cursor"));
	optionsGrid->SetColLabelValue(colRightCursor, _T("Right Cursor"));
	optionsGrid->SetColLabelValue(colDifference, _T("Difference"));
	optionsGrid->SetColLabelValue(colVisible, _T("Visible"));
	optionsGrid->SetColLabelValue(colRightAxis, _T("Right Axis"));

	optionsGrid->SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	optionsGrid->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

	optionsGrid->EndBatch();

	// Assign sizers and resize the frame
	SetSizerAndFit(topSizer);
	SetAutoLayout(true);
	topSizer->SetSizeHints(this);
}

//==========================================================================
// Class:			PlotPanel
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
BEGIN_EVENT_TABLE(PlotPanel, wxPanel)
	// Grid control
	EVT_GRID_CELL_RIGHT_CLICK(PlotPanel::GridRightClickEvent)
	EVT_GRID_CELL_LEFT_DCLICK(PlotPanel::GridDoubleClickEvent)
	EVT_GRID_CELL_LEFT_CLICK(PlotPanel::GridLeftClickEvent)
	EVT_GRID_CELL_CHANGE(PlotPanel::GridCellChangeEvent)

	// Context menu
	EVT_MENU(idContextAddMathChannel,				PlotPanel::ContextAddMathChannelEvent)
	EVT_MENU(idContextPlotDerivative,				PlotPanel::ContextPlotDerivativeEvent)
	EVT_MENU(idContextPlotIntegral,					PlotPanel::ContextPlotIntegralEvent)
	EVT_MENU(idContextPlotRMS,						PlotPanel::ContextPlotRMSEvent)
	EVT_MENU(idContextPlotFFT,						PlotPanel::ContextPlotFFTEvent)
	EVT_MENU(idContextRemoveCurve,					PlotPanel::ContextRemoveCurveEvent)

	EVT_MENU(idContextFilterLowPass,				PlotPanel::ContextFilterLowPassEvent)
	EVT_MENU(idContextFilterHighPass,				PlotPanel::ContextFilterHighPassEvent)

	EVT_MENU(idContextFitCurve,						PlotPanel::ContextFitCurve)

	EVT_MENU(idPlotContextToggleGridlines,			PlotPanel::ContextToggleGridlines)
	EVT_MENU(idPlotContextAutoScale,				PlotPanel::ContextAutoScale)
	EVT_MENU(idPlotContextWriteImageFile,			PlotPanel::ContextWriteImageFile)

	EVT_MENU(idPlotContextBGColor,					PlotPanel::ContextPlotBGColor)
	EVT_MENU(idPlotContextGridColor,				PlotPanel::ContextGridColor)

	EVT_MENU(idPlotContextToggleBottomGridlines,	PlotPanel::ContextToggleGridlinesBottom)
	EVT_MENU(idPlotContextSetBottomRange,			PlotPanel::ContextSetRangeBottom)
	EVT_MENU(idPlotContextAutoScaleBottom,			PlotPanel::ContextAutoScaleBottom)

	//EVT_MENU(idPlotContextToggleTopGridlines,		PlotPanel::)
	//EVT_MENU(idPlotContextSetTopRange,				PlotPanel::)
	//EVT_MENU(idPlotContextAutoScaleTop,				PlotPanel::)

	EVT_MENU(idPlotContextToggleLeftGridlines,		PlotPanel::ContextToggleGridlinesLeft)
	EVT_MENU(idPlotContextSetLeftRange,				PlotPanel::ContextSetRangeLeft)
	EVT_MENU(idPlotContextAutoScaleLeft,			PlotPanel::ContextAutoScaleLeft)

	EVT_MENU(idPlotContextToggleRightGridlines,		PlotPanel::ContextToggleGridlinesRight)
	EVT_MENU(idPlotContextSetRightRange,			PlotPanel::ContextSetRangeRight)
	EVT_MENU(idPlotContextAutoScaleRight,			PlotPanel::ContextAutoScaleRight)
END_EVENT_TABLE();

//==========================================================================
// Class:			PlotPanel
// Function:		UpdateDisplay
//
// Description:		Updates the plot and options grid.
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
void PlotPanel::UpdateDisplay(void)
{
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextRemoveCurveEvent
//
// Description:		Event fires when user clicks "RemoveCurve" in context menu.
//
// Input Arguments:
//		event	= &wxCommandEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::ContextRemoveCurveEvent(wxCommandEvent& WXUNUSED(event))
{
	// Known bug with wxGrid::GetSelectedRows() - returns empty set
	// This is the cleanest way to do it, after the bug is fixed
	/*wxArrayInt rows = optionsGrid->GetSelectedRows();

	// Must have row selected
	unsigned int i;
	for (i = 0; i < rows.Count(); i++)
	{
		// Cannot remove time row
		if (rows[i] == 0)
			continue;

		RemoveCurve(rows[i] - 1);
	}*/

	// Workaround for now
	int i;
	for (i = 1; i < optionsGrid->GetRows(); i++)
	{
		if (optionsGrid->IsInSelection(i, 0))
			RemoveCurve(i - 1);
	}

	renderer->UpdateDisplay();

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		CreateGridContextMenu
//
// Description:		Displays a context menu for the grid control.
//
// Input Arguments:
//		position	= const wxPoint& specifying the position to display the menu
//		row			= const unsigned int& specifying the row that was clicked
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::CreateGridContextMenu(const wxPoint &position, const unsigned int &row)
{
	// Declare the menu variable and get the position of the cursor
	wxMenu *contextMenu = new wxMenu();

	// Start building the context menu
	contextMenu->Append(idContextAddMathChannel, _T("Add Math Channel"));

	if (row > 0)
	{
		contextMenu->Append(idContextPlotDerivative, _T("Plot Derivative"));
		contextMenu->Append(idContextPlotIntegral, _T("Plot Integral"));
		contextMenu->Append(idContextPlotRMS, _T("Plot RMS"));
		contextMenu->Append(idContextPlotFFT, _T("Plot FFT"));

		contextMenu->AppendSeparator();

		wxMenu *filterMenu = new wxMenu();
		filterMenu->Append(idContextFilterLowPass, _T("Low-Pass"));
		filterMenu->Append(idContextFilterHighPass, _T("High-Pass"));
		contextMenu->AppendSubMenu(filterMenu, _T("Filter"));

		contextMenu->Append(idContextFitCurve, _T("Fit Curve"));

		contextMenu->AppendSeparator();

		contextMenu->Append(idContextRemoveCurve, _T("Remove Curve"));
	}

	// Show the menu
	PopupMenu(contextMenu, position);

	// Delete the context menu object
	delete contextMenu;
	contextMenu = NULL;

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		CreatePlotContextMenu
//
// Description:		Displays a context menu for the plot.
//
// Input Arguments:
//		position	= const wxPoint& specifying the position to display the menu
//		context		= const PlotContext& describing the area of the plot
//					  on which the click occured
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::CreatePlotContextMenu(const wxPoint &position, const PlotContext &context)
{
	// Declare the menu variable and get the position of the cursor
	wxMenu *contextMenu = new wxMenu();

	// Build the menu
	switch (context)
	{
	case plotContextXAxis:
		contextMenu->Append(idPlotContextToggleBottomGridlines, _T("Toggle Axis Gridlines"));
		contextMenu->Append(idPlotContextAutoScaleBottom, _T("Auto Scale Axis"));
		contextMenu->Append(idPlotContextSetBottomRange, _T("Set Range"));
		break;

	case plotContextLeftYAxis:
		contextMenu->Append(idPlotContextToggleLeftGridlines, _T("Toggle Axis Gridlines"));
		contextMenu->Append(idPlotContextAutoScaleLeft, _T("Auto Scale Axis"));
		contextMenu->Append(idPlotContextSetLeftRange, _T("Set Range"));
		break;

	case plotContextRightYAxis:
		contextMenu->Append(idPlotContextToggleRightGridlines, _T("Toggle Axis Gridlines"));
		contextMenu->Append(idPlotContextAutoScaleRight, _T("Auto Scale Axis"));
		contextMenu->Append(idPlotContextSetRightRange, _T("Set Range"));
		break;

	default:
	case plotContextPlotArea:
		contextMenu->Append(idPlotContextToggleGridlines, _T("Toggle Gridlines"));
		contextMenu->Append(idPlotContextAutoScale, _T("Auto Scale"));
		contextMenu->Append(idPlotContextWriteImageFile, _T("Write Image File"));
		contextMenu->AppendSeparator();
		contextMenu->Append(idPlotContextBGColor, _T("Set Background Color"));
		contextMenu->Append(idPlotContextGridColor, _T("Set Gridline Color"));
		break;
	}

	// Show the menu
	PopupMenu(contextMenu, position);

	// Delete the context menu object
	delete contextMenu;
	contextMenu = NULL;

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextWriteImageFile
//
// Description:		Writes plot to image file.
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
void PlotPanel::ContextWriteImageFile(wxCommandEvent &event)
{
	// FIXME:  Implement this (see DataPlotter code)
}

//==========================================================================
// Class:			PlotPanel
// Function:		ClearAllCurves
//
// Description:		Removes all curves from the plot.
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
void PlotPanel::ClearAllCurves(void)
{
	// Remove the curves locally
	while (plotList.GetCount() > 0)
		RemoveCurve(0);

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		AddCurve
//
// Description:		Adds a new dataset to the plot, created by operating on
//					existing datasets.
//
// Input Arguments:
//		mathString	= wxString describing the desired math operations
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::AddCurve(wxString mathString)
{
	// String will be empty if the user cancelled
	if (mathString.IsEmpty())
		return;

	// Parse string and determine what the new dataset should look like
	ExpressionTree expression(plotList);
	Dataset2D *mathChannel = new Dataset2D;

	double xAxisFactor(1.0);
	// FIXME:  Carry-over from DataPlotter
	/*if (!GetXAxisScalingFactor(xAxisFactor))
	{
		// FIXME:  Do we warn the user or not?  This is really only a problem if the user
		// specified FFT or a filter
	}*/

	wxString errors = expression.Solve(mathString, *mathChannel, xAxisFactor);

	// Check to see if there were any problems solving the tree
	if (!errors.IsEmpty())
	{
		// Tell the user about the errors
		::wxMessageBox(_T("Could not solve expression:\n\n") + errors, _T("Error Solving Expression"));

		DisplayMathChannelDialog(mathString);
		return;
	}

	// Then, add the new dataset to the plot
	AddCurve(mathChannel, mathString.Upper());// FIXME:  Get better name from user

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		AddCurve
//
// Description:		Adds an existing dataset to the plot.
//
// Input Arguments:
//		data	= Dataset2D* to add
//		name	= wxString specifying the label for the curve
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::AddCurve(Dataset2D *data, wxString name)
{
	plotList.Add(data);

	// Handle adding to the grid control
	optionsGrid->BeginBatch();

	// If this is the first curve to be added, add a row for the time, too
	unsigned int i;
	if (optionsGrid->GetNumberRows() == 0)
	{
		optionsGrid->AppendRows();

		for (i = 0; i < colCount; i++)
			optionsGrid->SetReadOnly(0, i, true);
	}
	unsigned int index = optionsGrid->GetNumberRows();
	optionsGrid->AppendRows();

	unsigned int maxLineSize(5);

	optionsGrid->SetCellEditor(index, colVisible, new wxGridCellBoolEditor);
	optionsGrid->SetCellEditor(index, colRightAxis, new wxGridCellBoolEditor);
	optionsGrid->SetCellEditor(index, colSize, new wxGridCellNumberEditor(1, maxLineSize));

	// Don't allow the user to change the contents of anything except the boolean cells
	for (i = 0; i < colDifference; i++)
			optionsGrid->SetReadOnly(index, i, true);
	optionsGrid->SetReadOnly(index, colSize, false);

	// Populate cell values
	optionsGrid->SetCellValue(index, colName, name);

	// Choose next color and set background color of appropriate cell
	unsigned int colorIndex = (index - 1) % 8;
	Color color;
	switch (colorIndex)
	{
	case 0:
		color = Color::ColorBlue;
		break;

	case 1:
		color = Color::ColorRed;
		break;

	case 2:
		color = Color::ColorGreen;
		break;

	case 3:
		color = Color::ColorMagenta;
		break;

	case 4:
		color = Color::ColorCyan;
		break;

	case 5:
		color = Color::ColorYellow;
		break;

	case 6:
		color = Color::ColorGray;
		break;

	default:
	case 7:
		color = Color::ColorBlack;
		break;
	}

	optionsGrid->SetCellBackgroundColour(index, colColor, color.ToWxColor());

	optionsGrid->SetCellValue(index, colSize, _T("1"));

	// Set default boolean values
	optionsGrid->SetCellValue(index, colVisible, _T("1"));

	optionsGrid->AutoSizeColumns();

	optionsGrid->EndBatch();

	// Add the curve to the plot
	renderer->AddCurve(*data);
	unsigned long size;
	optionsGrid->GetCellValue(index, colSize).ToULong(&size);
	renderer->SetCurveProperties(index - 1, color, true, false, size);

	renderer->UpdateDisplay();

	// Resize to prevent scrollbars and hidden values in the grid control
	topSizer->Layout();

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		RemoveCurve
//
// Description:		Removes a curve from the plot.
//
// Input Arguments:
//		i	= const unsigned int& specifying curve to remove
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::RemoveCurve(const unsigned int &i)
{
	// Remove from grid control
	optionsGrid->DeleteRows(i + 1);

	// If this was the last data row, also remove the time row
	if (optionsGrid->GetNumberRows() == 1)
		optionsGrid->DeleteRows();

	optionsGrid->AutoSizeColumns();

	// Also remove the curve from the plot
	renderer->RemoveCurve(i);

	// And remove from our local list (calls destructor for the dataset)
	plotList.Remove(i);

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		GridRightClickEvent
//
// Description:		Handles right-click events on the grid control.  Displays
//					context menu.
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
void PlotPanel::GridRightClickEvent(wxGridEvent &event)
{
	optionsGrid->SelectRow(event.GetRow());
	CreateGridContextMenu(event.GetPosition() + optionsGrid->GetPosition(), event.GetRow());

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		GridDoubleClickEvent
//
// Description:		Handles double click event for the grid control.  If the
//					click occurs on the color box, display the color dialog.
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
void PlotPanel::GridDoubleClickEvent(wxGridEvent &event)
{
	// No color for Time
	int row = event.GetRow();
	if (row == 0)
		return;

	if (event.GetCol() != colColor)
	{
		event.Skip();
		return;
	}

	wxColourData colorData;
	colorData.SetColour(optionsGrid->GetCellBackgroundColour(row, colColor));

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
    dialog.SetTitle(_T("Choose Line Color"));
    if (dialog.ShowModal() == wxID_OK)
    {
        colorData = dialog.GetColourData();
		optionsGrid->SetCellBackgroundColour(row, colColor, colorData.GetColour());
		Color color;
		color.Set(colorData.GetColour());
		unsigned long size;
		optionsGrid->GetCellValue(row, colSize).ToULong(&size);
		renderer->SetCurveProperties(row - 1, color,
			!optionsGrid->GetCellValue(row, colVisible).IsEmpty(),
			!optionsGrid->GetCellValue(row, colRightAxis).IsEmpty(), size);
    }

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		GridLeftClickEvent
//
// Description:		Handles grid cell change events (for boolean controls).
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
void PlotPanel::GridLeftClickEvent(wxGridEvent &event)
{
	unsigned int row = event.GetRow();

	// This stuff may be necessary after bug is fixed with wxGrid::GetSelectedRows()?
	// See ButtonRemoveCurveClickedEvent() for details
	//optionsGrid->SetSelectionMode(wxGrid::wxGridSelectRows);
	//optionsGrid->SelectRow(row, event.ControlDown());

	// Skip to handle row selection (with SHIFT and CTRL) and also boolean column click handlers
	event.Skip();

	// Was this click in one of the boolean columns and not in the time row?
	if (row == 0 || (event.GetCol() != colVisible && event.GetCol() != colRightAxis))
		return;

	if (optionsGrid->GetCellValue(row, event.GetCol()).Cmp(_T("1")) == 0)
		optionsGrid->SetCellValue(row, event.GetCol(), wxEmptyString);
	else
		optionsGrid->SetCellValue(row, event.GetCol(), _T("1"));

	// If the only visible curves are FFTs, change the x-label
/*	int i;
	bool showFFTLabel(false);
	for (i = 1; i < optionsGrid->GetRows(); i++)
	{
		if (optionsGrid->GetCellValue(i, colVisible).Cmp(_T("1")) == 0)
		{
			if (optionsGrid->GetCellValue(i, colName).Mid(0, 3).CmpNoCase(_T("FFT")) == 0)
				showFFTLabel = true;
			else
			{
				showFFTLabel = false;
				break;
			}
		}
	}
	// FIXME:  This is carry-over from DataPlotter
	if (showFFTLabel)
		SetXDataLabel(FormatFFT);
	else
		SetXDataLabel(currentFileFormat);*/

	Color color;
	color.Set(optionsGrid->GetCellBackgroundColour(row, colColor));
	unsigned long size;
	optionsGrid->GetCellValue(row, colSize).ToULong(&size);
	renderer->SetCurveProperties(row - 1, color,
		!optionsGrid->GetCellValue(row, colVisible).IsEmpty(),
		!optionsGrid->GetCellValue(row, colRightAxis).IsEmpty(), size);

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		GridLeftClickEvent
//
// Description:		Handles grid cell change events (for text controls).
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
void PlotPanel::GridCellChangeEvent(wxGridEvent &event)
{
	// This event is only valid for one column, and then not in the first row
	unsigned int row(event.GetRow());
	if (row == 0 || event.GetCol() != colSize)
		event.Skip();

	// Update all of the line parameters
	Color color;
	color.Set(optionsGrid->GetCellBackgroundColour(row, colColor));
	unsigned long size;
	optionsGrid->GetCellValue(row, colSize).ToULong(&size);
	renderer->SetCurveProperties(row - 1, color,
		!optionsGrid->GetCellValue(row, colVisible).IsEmpty(),
		!optionsGrid->GetCellValue(row, colRightAxis).IsEmpty(), size);
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextAddMathChannelEvent
//
// Description:		Adds a user-defined math channel to the plot.
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
void PlotPanel::ContextAddMathChannelEvent(wxCommandEvent& WXUNUSED(event))
{
	DisplayMathChannelDialog();

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextPlotDerivativeEvent
//
// Description:		Adds a curve showing the derivative of the selected grid
//					row to the plot.
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
void PlotPanel::ContextPlotDerivativeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the derivative of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(DiscreteDerivative::ComputeTimeHistory(*plotList[row - 1]));

	wxString name = _T("d/dt(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextPlotDerivativeEvent
//
// Description:		Adds a curve showing the integral of the selected grid
//					row to the plot.
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
void PlotPanel::ContextPlotIntegralEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the integral of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(DiscreteIntegral::ComputeTimeHistory(*plotList[row - 1]));

	wxString name = _T("integral(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextPlotDerivativeEvent
//
// Description:		Adds a curve showing the RMS of the selected grid
//					row to the plot.
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
void PlotPanel::ContextPlotRMSEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the RMS of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(RootMeanSquare::ComputeTimeHistory(*plotList[row - 1]));

	wxString name = _T("RMS(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextPlotDerivativeEvent
//
// Description:		Adds a curve showing the FFT of the selected grid
//					row to the plot.
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
void PlotPanel::ContextPlotFFTEvent(wxCommandEvent& WXUNUSED(event))
{
	double factor(1.0);
	// FIXME:  Carry-over from DataPlotter
	/*if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"), _T("Accuracy Warning"));*/

	// Create new dataset containing the FFT of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(FastFourierTransform::Compute(*plotList[row - 1]));

	// Scale as required
	newData->MultiplyXData(factor);

	wxString name = _T("FFT(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextFilterLowPassEvent
//
// Description:		Adds a curve showing the filtered signal to the plot.
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
void PlotPanel::ContextFilterLowPassEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the FFT of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	const Dataset2D *currentData = plotList[row - 1];
	Dataset2D *newData = new Dataset2D(*currentData);

	// Display a dialog asking for the cutoff frequency
	wxString cutoffString = ::wxGetTextFromUser(_T("Specify the cutoff frequency in Hertz:"),
		_T("Filter Cutoff Frequency"), _T("1.0"), this);
	double cutoff;
	if (!cutoffString.ToDouble(&cutoff))
	{
		::wxMessageBox(_T("ERROR:  Cutoff frequency must be numeric!"), _T("Filter Error"));
		return;
	}

	// Create the filter
	double sampleRate = 1.0 / (currentData->GetXData(1) - currentData->GetXData(0));// [Hz]

	double factor(1.0);
	// FIXME:  Carry-over from DataPlotter
	/*if (!GetXAxisScalingFactor(factor))
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Cutoff frequency may be incorrect!"), _T("Accuracy Warning"));*/

	sampleRate *= factor;
	LowPassFirstOrderFilter filter(cutoff, sampleRate, currentData->GetYData(0));

	// Apply the filter
	unsigned int i;
	for (i = 0; i < newData->GetNumberOfPoints(); i++)
		newData->GetYPointer()[i] = filter.Apply(currentData->GetYData(i));

	wxString name = cutoffString.Trim() + _T(" Hz low-pass(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextFilterHighPassEvent
//
// Description:		Adds a curve showing the filtered signal to the plot.
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
void PlotPanel::ContextFilterHighPassEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the filtered dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	const Dataset2D *currentData = plotList[row - 1];
	Dataset2D *newData = new Dataset2D(*currentData);

	// Display a dialog asking for the cutoff frequency
	wxString cutoffString = ::wxGetTextFromUser(_T("Specify the cutoff frequency in Hertz:"),
		_T("Filter Cutoff Frequency"), _T("1.0"), this);
	double cutoff;
	if (!cutoffString.ToDouble(&cutoff))
	{
		::wxMessageBox(_T("ERROR:  Cutoff frequency must be numeric!"), _T("Filter Error"));
		return;
	}

	// Create the filter
	double sampleRate = 1.0 / (currentData->GetXData(1) - currentData->GetXData(0));// [Hz]

	double factor(1.0);
	// FIXME:  Carry-over from DataPlotter
	/*if (!GetXAxisScalingFactor(factor))
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Cutoff frequency may be incorrect!"), _T("Accuracy Warning"));*/

	sampleRate *= factor;
	HighPassFirstOrderFilter filter(cutoff, sampleRate, currentData->GetYData(0));

	// Apply the filter
	unsigned int i;
	for (i = 0; i < newData->GetNumberOfPoints(); i++)
		newData->GetYPointer()[i] = filter.Apply(currentData->GetYData(i));

	wxString name = cutoffString.Trim() + _T(" Hz high-pass(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextFitCurve
//
// Description:		Fits a curve to the dataset selected in the grid control.
//					User is asked to specify the order of the fit.
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
void PlotPanel::ContextFitCurve(wxCommandEvent& WXUNUSED(event))
{
	// Ask the user what order to use for the polynomial
	unsigned long order;
	wxString orderString = ::wxGetTextFromUser(_T("Specify the order of the polynomial fit:"),
		_T("Polynomial Curve Fit"), _T("2"), this);

	if (!orderString.ToULong(&order))
	{
		::wxMessageBox(_T("ERROR:  Order must be a positive integer!"), _T("Error Fitting Curve"));
		return;
	}

	// Fit the data
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	CurveFit::PolynomialFit fitData = CurveFit::DoPolynomialFit(*plotList[row - 1], order);

	// Create a data set to draw the fit and add it to the plot
	Dataset2D *newData = new Dataset2D(*plotList[row - 1]);
	unsigned int i;
	for (i = 0; i < newData->GetNumberOfPoints(); i++)
		newData->GetYPointer()[i] = CurveFit::EvaluateFit(newData->GetXData(i), fitData);

	// Create discriptive string to use as the name
	wxString name, termString;
	//name.Printf("Order %lu Fit([%i]), R^2 = %0.2f", order, row, fitData.rSquared);
	name.Printf("Fit [%i] (R^2 = %0.2f): ", row, fitData.rSquared);
	for (i = 0; i <= order; i++)
	{
		if (i == 0)
			termString.Printf("%1.2e", fitData.coefficients[i]);
		else if (i == 1)
			termString.Printf("%0.2ex", fabs(fitData.coefficients[i]));
		else
			termString.Printf("%0.2ex^%i", fabs(fitData.coefficients[i]), i);

		if (i < order)
		{
			if (fitData.coefficients[i] > 0.0)
				termString.Append(_T(" + "));
			else
				termString.Append(_T(" - "));
		}
		name.Append(termString);
	}
	AddCurve(newData, name);

	// Free the coefficient data
	delete [] fitData.coefficients;

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleGridlines
//
// Description:		Toggles gridlines for the entire plot on and off.
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
void PlotPanel::ContextToggleGridlines(wxCommandEvent& WXUNUSED(event))
{
	if (renderer->GetGridOn())
		renderer->SetGridOff();
	else
		renderer->SetGridOn();

	renderer->UpdateDisplay();

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextAutoScale
//
// Description:		Autoscales the plot.
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
void PlotPanel::ContextAutoScale(wxCommandEvent& WXUNUSED(event))
{
	renderer->AutoScale();
	renderer->UpdateDisplay();

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		UpdateCursorValues
//
// Description:		Updates the values for the cursors and their differences
//					in the options grid.
//
// Input Arguments:
//		leftVisible		= const bool& indicating whether or not the left
//						  cursor is visible
//		rightVisible	= const bool& indicating whether or not the right
//						  cursor is visible
//		leftValue		= const double& giving the value of the left cursor
//		rightValue		= const double& giving the value of the right cursor
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue)
{
	if (optionsGrid == NULL)
		return;

	// FIXME:  This would be nicer with smart precision so we show enough digits but not too many

	// For each curve, update the cursor values
	int i;
	double value;
	wxString valueString;
	for (i = 1; i < optionsGrid->GetRows(); i++)
	{
		if (leftVisible)
		{
			valueString.Printf("%f", leftValue);
			optionsGrid->SetCellValue(0, colLeftCursor, valueString);

			value = leftValue;
			if (plotList[i - 1]->GetYAt(value))
			{
				valueString.Printf("%f", value);
				optionsGrid->SetCellValue(i, colLeftCursor, _T("*") + valueString);
			}
			else
			{
				valueString.Printf("%f", value);
				optionsGrid->SetCellValue(i, colLeftCursor, valueString);
			}
		}
		else
		{
			optionsGrid->SetCellValue(0, colLeftCursor, wxEmptyString);
			optionsGrid->SetCellValue(i, colLeftCursor, wxEmptyString);

			// The difference column only exists if both cursors are visible
			optionsGrid->SetCellValue(0, colDifference, wxEmptyString);
			optionsGrid->SetCellValue(i, colDifference, wxEmptyString);
		}

		if (rightVisible)
		{
			valueString.Printf("%f", rightValue);
			optionsGrid->SetCellValue(0, colRightCursor, valueString);

			value = rightValue;
			if (plotList[i - 1]->GetYAt(value))
			{
				valueString.Printf("%f", value);
				optionsGrid->SetCellValue(i, colRightCursor, _T("*") + valueString);
			}
			else
			{
				valueString.Printf("%f", value);
				optionsGrid->SetCellValue(i, colRightCursor, valueString);
			}

			// Update the difference cells if the left cursor is visible, too
			if (leftVisible)
			{
				double left = leftValue;
				plotList[i - 1]->GetYAt(left);
				valueString.Printf("%f", value - left);
				optionsGrid->SetCellValue(i, colDifference, valueString);

				valueString.Printf("%f", rightValue - leftValue);
				optionsGrid->SetCellValue(0, colDifference, valueString);
			}
		}
		else
		{
			optionsGrid->SetCellValue(0, colRightCursor, wxEmptyString);
			optionsGrid->SetCellValue(i, colRightCursor, wxEmptyString);

			// The difference column only exists if both cursors are visible
			optionsGrid->SetCellValue(0, colDifference, wxEmptyString);
			optionsGrid->SetCellValue(i, colDifference, wxEmptyString);
		}
	}

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		DisplayMathChannelDialog
//
// Description:		Displays an input dialog that allows the user to enter a
//					math expression.  If an expression is entered, it attempts
//					to add the channel.
//
// Input Arguments:
//		defaultInput	= wxString (optional)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::DisplayMathChannelDialog(wxString defaultInput)
{
	// Display input dialog in which user can specify the math desired
	wxString message(_T("Enter the math you would like to perform:\n\n"));
	message.Append(_T("    Use [x] notation to specify channels, where x = 0 is Time, x = 1 is the first data channel, etc.\n"));
	message.Append(_T("    Valid operations are: +, -, *, /, ddt, and int\n"));
	message.Append(_T("    Use () to specify order of operations"));

	AddCurve(::wxGetTextFromUser(message, _T("Specify Math Channel"), defaultInput, this));

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		DisplayAxisRangeDialog
//
// Description:		Displays an input dialog that allows the user to set the
//					range for an axis.
//
// Input Arguments:
//		axis	= const PlotContext& specifying the axis which is to be resized
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::DisplayAxisRangeDialog(const PlotContext &axis)
{
	// Assign min and max to current axis limits
	double min, max;
	switch (axis)
	{
	case plotContextXAxis:
		min = renderer->GetXMin();
		max = renderer->GetXMax();
		break;

	case plotContextLeftYAxis:
		min = renderer->GetLeftYMin();
		max = renderer->GetLeftYMax();
		break;

	case plotContextRightYAxis:
		min = renderer->GetRightYMin();
		max = renderer->GetRightYMax();
		break;

	default:
	case plotContextPlotArea:
		// Plot area is not a valid context in which we can set axis limits
		return;
	}

	// Display the dialog and make sure the user doesn't cancel
	RangeLimitsDialog dialog(this, min, max);
	if (dialog.ShowModal() != wxID_OK)
		return;

	// Get the new limits (and correct if they entered the larger value in the min box)
	if (dialog.GetMinimum() < dialog.GetMaximum())
	{
		min = dialog.GetMinimum();
		max = dialog.GetMaximum();
	}
	else
	{
		max = dialog.GetMinimum();
		min = dialog.GetMaximum();
	}

	// Make sure the limits aren't equal
	if (min == max)
	{
		::wxMessageBox(_T("ERROR:  Limits must unique!"), _T("Error Setting Limits"));
		return;
	}

	switch (axis)
	{
	case plotContextLeftYAxis:
		renderer->SetLeftYLimits(min, max);
		break;

	case plotContextRightYAxis:
		renderer->SetRightYLimits(min, max);
		break;

	default:
	case plotContextXAxis:
		renderer->SetXLimits(min, max);
	}

	renderer->UpdateDisplay();

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleGridlinesBottom
//
// Description:		Toggles gridlines for the bottom axis.
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
void PlotPanel::ContextToggleGridlinesBottom(wxCommandEvent& WXUNUSED(event))
{
	renderer->SetBottomGrid(!renderer->GetBottomGrid());

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextAutoScaleBottom
//
// Description:		Auto-scales the bottom axis.
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
void PlotPanel::ContextAutoScaleBottom(wxCommandEvent& WXUNUSED(event))
{
	renderer->AutoScaleBottom();

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextSetRangeBottom
//
// Description:		Dispalys a dialog box for setting the axis range.
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
void PlotPanel::ContextSetRangeBottom(wxCommandEvent& WXUNUSED(event))
{
	DisplayAxisRangeDialog(plotContextXAxis);

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleGridlinesLeft
//
// Description:		Toggles gridlines for the bottom axis.
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
void PlotPanel::ContextToggleGridlinesLeft(wxCommandEvent& WXUNUSED(event))
{
	renderer->SetLeftGrid(!renderer->GetLeftGrid());

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextAutoScaleLeft
//
// Description:		Toggles gridlines for the bottom axis.
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
void PlotPanel::ContextAutoScaleLeft(wxCommandEvent& WXUNUSED(event))
{
	renderer->AutoScaleLeft();

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextSetRangeLeft
//
// Description:		Dispalys a dialog box for setting the axis range.
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
void PlotPanel::ContextSetRangeLeft(wxCommandEvent& WXUNUSED(event))
{
	DisplayAxisRangeDialog(plotContextLeftYAxis);

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleGridlinesRight
//
// Description:		Toggles gridlines for the bottom axis.
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
void PlotPanel::ContextToggleGridlinesRight(wxCommandEvent& WXUNUSED(event))
{
	renderer->SetRightGrid(!renderer->GetRightGrid());

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextAutoScaleRight
//
// Description:		Toggles gridlines for the bottom axis.
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
void PlotPanel::ContextAutoScaleRight(wxCommandEvent& WXUNUSED(event))
{
	renderer->AutoScaleRight();

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextSetRangeRight
//
// Description:		Dispalys a dialog box for setting the axis range.
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
void PlotPanel::ContextSetRangeRight(wxCommandEvent& WXUNUSED(event))
{
	DisplayAxisRangeDialog(plotContextRightYAxis);

	return;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextPlotBGColor
//
// Description:		Displays a dialog allowing the user to specify the plot's
//					background color.
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
void PlotPanel::ContextPlotBGColor(wxCommandEvent& WXUNUSED(event))
{
	wxColourData colorData;
	colorData.SetColour(renderer->GetBackgroundColor().ToWxColor());

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
    dialog.SetTitle(_T("Choose Background Color"));
    if (dialog.ShowModal() == wxID_OK)
    {
		Color color;
		color.Set(dialog.GetColourData().GetColour());
		renderer->SetBackgroundColor(color);
		renderer->UpdateDisplay();
    }
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextGridColor
//
// Description:		Dispalys a dialog box allowing the user to specify the
//					gridline color.
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
void PlotPanel::ContextGridColor(wxCommandEvent& WXUNUSED(event))
{
	wxColourData colorData;
	colorData.SetColour(renderer->GetGridColor().ToWxColor());

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
    dialog.SetTitle(_T("Choose Background Color"));
    if (dialog.ShowModal() == wxID_OK)
    {
		Color color;
		color.Set(dialog.GetColourData().GetColour());
		renderer->SetGridColor(color);
		renderer->UpdateDisplay();
    }
}