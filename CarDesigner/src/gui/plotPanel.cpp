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
#include <wx/splitter.h>
#include <wx/clipbrd.h>
#include <wx/file.h>

// Local headers
#include "gui/renderer/plotRenderer.h"
#include "gui/dialogs/rangeLimitsDialog.h"
#include "gui/dialogs/textInputDialog.h"
#include "vRenderer/color.h"
#include "vMath/carMath.h"
#include "vMath/signals/integral.h"
#include "vMath/signals/derivative.h"
#include "vMath/signals/rms.h"
#include "vMath/signals/fft.h"
#include "vMath/expressionTree.h"
#include "vMath/signals/curveFit.h"
#include "vMath/signals/filter.h"
#include "vUtilities/debugger.h"
#include "gui/components/mainFrame.h"

//==========================================================================
// Class:			PlotPanel
// Function:		PlotPanel
//
// Description:		Constructor for PlotPanel class.  Initializes the panel
//					and creates the controls, etc.
//
// Input Arguments:
//		parent		= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PlotPanel::PlotPanel(wxWindow *parent) : wxPanel(parent)
{
	CreateControls();
	defaultLineSize = 1.0;
	defaultMarkerSize = -1;
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
	ClearAllCurves();
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
void PlotPanel::CreateControls()
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxSplitterWindow *splitter = new wxSplitterWindow(this);
	topSizer->Add(splitter, 1, wxGROW);

	wxPanel *lowerPanel = new wxPanel(splitter);
	wxBoxSizer *lowerSizer = new wxBoxSizer(wxHORIZONTAL);
	lowerSizer->Add(CreateOptionsGrid(lowerPanel), 1, wxGROW | wxALL, 5);
	lowerPanel->SetSizer(lowerSizer);

	CreatePlotArea(splitter);
	splitter->SplitHorizontally(plotArea, lowerPanel, plotArea->GetSize().GetHeight());
	splitter->SetSize(GetClientSize());
	splitter->SetSashGravity(1.0);
	splitter->SetMinimumPaneSize(120);

	SetSizerAndFit(topSizer);
	splitter->SetSashPosition(splitter->GetSashPosition(), false);

	Layout();// Manually calling layout make the plot area expand to the full available space
}

//==========================================================================
// Class:			PlotPanel
// Function:		CreateOptionsGrid
//
// Description:		Creates and formats the options grid.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxGrid* pointing to optionsGrid
//
//==========================================================================
wxGrid* PlotPanel::CreateOptionsGrid(wxWindow *parent)
{
	optionsGrid = new wxGrid(parent, wxID_ANY);
	optionsGrid->BeginBatch();

	optionsGrid->CreateGrid(0, colCount, wxGrid::wxGridSelectRows);
	optionsGrid->SetRowLabelSize(0);
	optionsGrid->SetColFormatNumber(colLineSize);
	optionsGrid->SetColFormatNumber(colMarkerSize);
	optionsGrid->SetColFormatFloat(colLeftCursor);
	optionsGrid->SetColFormatFloat(colRightCursor);
	optionsGrid->SetColFormatFloat(colDifference);
	optionsGrid->SetColFormatBool(colVisible);
	optionsGrid->SetColFormatBool(colRightAxis);

	optionsGrid->SetColLabelValue(colName, _T("Curve"));
	optionsGrid->SetColLabelValue(colColor, _T("Color"));
	optionsGrid->SetColLabelValue(colLineSize, _T("Line"));
	optionsGrid->SetColLabelValue(colMarkerSize, _T("Marker"));
	optionsGrid->SetColLabelValue(colLeftCursor, _T("Left Cursor"));
	optionsGrid->SetColLabelValue(colRightCursor, _T("Right Cursor"));
	optionsGrid->SetColLabelValue(colDifference, _T("Difference"));
	optionsGrid->SetColLabelValue(colVisible, _T("Visible"));
	optionsGrid->SetColLabelValue(colRightAxis, _T("Right Axis"));

	optionsGrid->SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	optionsGrid->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	optionsGrid->EnableDragRowSize(false);

	unsigned int i;
	for (i = 1; i < colCount; i++)// Skip the name column
		optionsGrid->AutoSizeColLabelSize(i);

	optionsGrid->EndBatch();

	return optionsGrid;
}

//==========================================================================
// Class:			PlotPanel
// Function:		CreatePlotArea
//
// Description:		Creates the main plot control.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		PlotRenderer* pointing to plotArea
//
//==========================================================================
PlotRenderer* PlotPanel::CreatePlotArea(wxWindow *parent)
{
	wxGLAttributes displayAttributes;
	displayAttributes.PlatformDefaults().RGBA().DoubleBuffer().SampleBuffers(1).Samplers(4).Stencil(1).EndList();

	assert(wxGLCanvas::IsDisplaySupported(displayAttributes));
	plotArea = new PlotRenderer(*parent, *this, wxID_ANY, displayAttributes);

	//plotArea->SetMinSize(wxSize(650, 320));
	plotArea->SetMajorGridOn();
	plotArea->SetLegendOn();

	return plotArea;
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
	EVT_GRID_CELL_CHANGED(PlotPanel::GridCellChangeEvent)

	// Context menu
	EVT_MENU(idContextAddMathChannel,				PlotPanel::ContextAddMathChannelEvent)
	EVT_MENU(idContextFRF,							PlotPanel::ContextFRFEvent)
	EVT_MENU(idContextCreateSignal,					PlotPanel::ContextCreateSignalEvent)
	EVT_MENU(idContextSetTimeUnits,					PlotPanel::ContextSetTimeUnitsEvent)
	EVT_MENU(idContextScaleXData,					PlotPanel::ContextScaleXDataEvent)
	EVT_MENU(idContextPlotDerivative,				PlotPanel::ContextPlotDerivativeEvent)
	EVT_MENU(idContextPlotIntegral,					PlotPanel::ContextPlotIntegralEvent)
	EVT_MENU(idContextPlotRMS,						PlotPanel::ContextPlotRMSEvent)
	//EVT_MENU(idContextPlotFFT,						PlotPanel::ContextPlotFFTEvent)
	//EVT_MENU(idButtonRemoveCurve,					PlotPanel::ButtonRemoveCurveClickedEvent)
	EVT_MENU(idContextBitMask,						PlotPanel::ContextBitMaskEvent)
	EVT_MENU(idContextTimeShift,					PlotPanel::ContextTimeShiftEvent)

	EVT_MENU(idContextFilter,						PlotPanel::ContextFilterEvent)
	EVT_MENU(idContextFitCurve,						PlotPanel::ContextFitCurve)

	EVT_MENU(idPlotContextCopy,						PlotPanel::ContextCopy)
	//EVT_MENU(idPlotContextPaste,					PlotPanel::ContextPaste)
	EVT_MENU(idPlotContextMajorGridlines,			PlotPanel::ContextToggleMajorGridlines)
	EVT_MENU(idPlotContextMinorGridlines,			PlotPanel::ContextToggleMinorGridlines)
	EVT_MENU(idPlotContextShowLegend,				PlotPanel::ContextToggleLegend)
	EVT_MENU(idPlotContextAutoScale,				PlotPanel::ContextAutoScale)
	EVT_MENU(idPlotContextWriteImageFile,			PlotPanel::ContextWriteImageFile)
	EVT_MENU(idPlotContextExportData,				PlotPanel::ContextExportData)

	EVT_MENU(idPlotContextBGColor,					PlotPanel::ContextPlotBGColor)
	EVT_MENU(idPlotContextGridColor,				PlotPanel::ContextGridColor)

	EVT_MENU(idPlotContextBottomMajorGridlines,		PlotPanel::ContextToggleMajorGridlinesBottom)
	EVT_MENU(idPlotContextBottomMinorGridlines,		PlotPanel::ContextToggleMinorGridlinesBottom)
	EVT_MENU(idPlotContextSetBottomRange,			PlotPanel::ContextSetRangeBottom)
	EVT_MENU(idPlotContextSetBottomMajorResolution,	PlotPanel::ContextSetMajorResolutionBottom)
	EVT_MENU(idPlotContextBottomLogarithmic,		PlotPanel::ContextSetLogarithmicBottom)
	EVT_MENU(idPlotContextAutoScaleBottom,			PlotPanel::ContextAutoScaleBottom)
	EVT_MENU(idPlotContextEditBottomLabel,			PlotPanel::ContextEditBottomLabel)

	//EVT_MENU(idPlotContextTopMajorGridlines,		PlotPanel::)
	//EVT_MENU(idPlotContextTopMinorGridlines,		PlotPanel::)
	//EVT_MENU(idPlotContextSetTopRange,			PlotPanel::)
	//EVT_MENU(idPlotContextSetTopMajorResolution,	PlotPanel::)
	//EVT_MENU(idPlotContextTopLogarithmic,			PlotPanel::)
	//EVT_MENU(idPlotContextAutoScaleTop,			PlotPanel::)
	//EVT_MENU(idPlotContextEditTopLabel,			PlotPanel::)

	EVT_MENU(idPlotContextLeftMajorGridlines,		PlotPanel::ContextToggleMajorGridlinesLeft)
	EVT_MENU(idPlotContextLeftMinorGridlines,		PlotPanel::ContextToggleMinorGridlinesLeft)
	EVT_MENU(idPlotContextSetLeftRange,				PlotPanel::ContextSetRangeLeft)
	EVT_MENU(idPlotContextSetLeftMajorResolution,	PlotPanel::ContextSetMajorResolutionLeft)
	EVT_MENU(idPlotContextLeftLogarithmic,			PlotPanel::ContextSetLogarithmicLeft)
	EVT_MENU(idPlotContextAutoScaleLeft,			PlotPanel::ContextAutoScaleLeft)
	EVT_MENU(idPlotContextEditLeftLabel,			PlotPanel::ContextEditLeftLabel)

	EVT_MENU(idPlotContextRightMajorGridlines,		PlotPanel::ContextToggleMajorGridlinesRight)
	EVT_MENU(idPlotContextRightMinorGridlines,		PlotPanel::ContextToggleMinorGridlinesRight)
	EVT_MENU(idPlotContextSetRightRange,			PlotPanel::ContextSetRangeRight)
	EVT_MENU(idPlotContextSetRightMajorResolution,	PlotPanel::ContextSetMajorResolutionRight)
	EVT_MENU(idPlotContextRightLogarithmic,			PlotPanel::ContextSetLogarithmicRight)
	EVT_MENU(idPlotContextAutoScaleRight,			PlotPanel::ContextAutoScaleRight)
	EVT_MENU(idPlotContextEditRightLabel,			PlotPanel::ContextEditRightLabel)
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
void PlotPanel::UpdateDisplay()
{
	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContexExportData
//
// Description:		Exports the data to file.
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
void PlotPanel::ContextExportData(wxCommandEvent& WXUNUSED(event))
{
	wxString wildcard(_T("Comma Separated (*.csv)|*.csv"));
	wildcard.append("|Tab Delimited (*.txt)|*.txt");

	wxArrayString pathAndFileName = static_cast<MainFrame*>(GetParent())->GetFileNameFromUser(_T("Save As"),
		wxEmptyString, wxEmptyString, wildcard, wxFD_SAVE);

	if (pathAndFileName.Count() == 0)
		return;

	if (wxFile::Exists(pathAndFileName[0]))
	{
		if (wxMessageBox(_T("File exists.  Overwrite?"), _T("Overwrite File?"), wxYES_NO, this) == wxNO)
			return;
	}

	wxString delimiter;
	if (pathAndFileName[0].Mid(pathAndFileName[0].Last('.')).CmpNoCase(_T(".txt")) == 0)
		delimiter = _T("\t");
	else
		delimiter = _T(",");// FIXME:  Need to handle descriptions containing commas so we don't have problems with import later on

	// Export both x and y data in case of asynchronous data or FFT, etc.
	std::ofstream outFile(pathAndFileName[0].mb_str(), std::ios::out);
	if (!outFile.is_open() || !outFile.good())
	{
		wxMessageBox(_T("Could not open '") + pathAndFileName[0] + _T("' for output."),
			_T("Error Writing File"), wxICON_ERROR, this);
		return;
	}

	unsigned int i, j(0);
	wxString temp;
	for (i = 1; i < plotList.GetCount() + 1; i++)
	{
		if (optionsGrid->GetCellValue(i, colName).Contains(_T("FFT")) ||
			optionsGrid->GetCellValue(i, colName).Contains(_T("FRF")))
			outFile << _T("Frequency [Hz]") << delimiter;
		else
		{
			if (delimiter.Cmp(",") == 0)
			{
				temp = plotArea->GetXLabel();
				temp.Replace(",", ";");
				outFile << temp << delimiter;
			}
			else
				outFile << plotArea->GetXLabel() << delimiter;
		}

		if (delimiter.Cmp(",") == 0)
		{
			temp = optionsGrid->GetCellValue(i, colName);
			temp.Replace(",", ";");
			outFile << temp;
		}
		else
			outFile << optionsGrid->GetCellValue(i, colName);

		if (i == plotList.GetCount())
			outFile << endl;
		else
			outFile << delimiter;
	}

	outFile.precision(14);

	bool done(false);
	while (!done)
	{
		done = true;
		for (i = 0; i < plotList.GetCount(); i++)
		{
			if (j < plotList[i]->GetNumberOfPoints())
				outFile << plotList[i]->GetXData(j) << delimiter << plotList[i]->GetYData(j);
			else
				outFile << delimiter;

			if (i == plotList.GetCount() - 1)
				outFile << endl;
			else
				outFile << delimiter;

			if (j + 1 < plotList[i]->GetNumberOfPoints())
				done = false;
		}

		j++;
	}

	outFile.close();
}

#if 0
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

	plotArea->UpdateDisplay();
}
#endif

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
	wxMenu *contextMenu = new wxMenu();

	contextMenu->Append(idContextAddMathChannel, _T("Add Math Channel"));
	/*contextMenu->Append(idContextFRF, _T("Frequency Response"));
	contextMenu->Append(idContextSetXData, _T("Use as X-Axis"));

	contextMenu->AppendSeparator();

	contextMenu->Append(idContextCreateSignal, _T("Create Signal"));

	contextMenu->AppendSeparator();*/

	/*if (row == 0)
	{
		contextMenu->Append(idContextSetTimeUnits, _T("Set Time Units"));
		contextMenu->Append(idContextScaleXData, _T("Scale X-Data"));
	}
	else*/ if (row > 0)
	{
		contextMenu->AppendSeparator();// TODO:  Remove this if the above menu options are re-introduced
		contextMenu->Append(idContextPlotDerivative, _T("Plot Derivative"));
		contextMenu->Append(idContextPlotIntegral, _T("Plot Integral"));
		contextMenu->Append(idContextPlotRMS, _T("Plot RMS"));
		/*contextMenu->Append(idContextPlotFFT, _T("Plot FFT"));
		contextMenu->Append(idContextTimeShift, _T("Plot Time-Shifted"));
		contextMenu->Append(idContextScaleXData, _T("Plot Time-Scaled"));
		contextMenu->Append(idContextBitMask, _T("Plot Bit"));*/

		contextMenu->AppendSeparator();

		//contextMenu->Append(idContextFilter, _T("Filter Curve"));
		contextMenu->Append(idContextFitCurve, _T("Fit Curve"));

		/*contextMenu->AppendSeparator();

		contextMenu->Append(idButtonRemoveCurve, _T("Remove Curve"));*/
	}

	PopupMenu(contextMenu);

	delete contextMenu;
	contextMenu = NULL;
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
	wxMenu *contextMenu;

	switch (context)
	{
	case PlotContextXAxis:
		contextMenu = CreateAxisContextMenu(idPlotContextBottomMajorGridlines);
		contextMenu->Check(idPlotContextBottomLogarithmic, plotArea->GetXLogarithmic());
		contextMenu->Check(idPlotContextBottomMajorGridlines, plotArea->GetBottomMajorGrid());
		contextMenu->Check(idPlotContextBottomMinorGridlines, plotArea->GetBottomMinorGrid());
		break;

	case PlotContextLeftYAxis:
		contextMenu = CreateAxisContextMenu(idPlotContextLeftMajorGridlines);
		contextMenu->Check(idPlotContextLeftLogarithmic, plotArea->GetLeftLogarithmic());
		contextMenu->Check(idPlotContextLeftMajorGridlines, plotArea->GetLeftMajorGrid());
		contextMenu->Check(idPlotContextLeftMinorGridlines, plotArea->GetLeftMinorGrid());
		break;

	case PlotContextRightYAxis:
		contextMenu = CreateAxisContextMenu(idPlotContextRightMajorGridlines);
		contextMenu->Check(idPlotContextRightLogarithmic, plotArea->GetRightLogarithmic());
		contextMenu->Check(idPlotContextRightMajorGridlines, plotArea->GetRightMajorGrid());
		contextMenu->Check(idPlotContextRightMinorGridlines, plotArea->GetRightMinorGrid());
		break;

	default:
	case PlotContextPlotArea:
		contextMenu = CreatePlotAreaContextMenu();
		break;
	}

	PopupMenu(contextMenu);

	delete contextMenu;
	contextMenu = NULL;
}

//==========================================================================
// Class:			PlotPanel
// Function:		CreatePlotAreaContextMenu
//
// Description:		Displays a context menu for the specified plot axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxMenu*
//
//==========================================================================
wxMenu* PlotPanel::CreatePlotAreaContextMenu() const
{
	wxMenu *contextMenu = new wxMenu();
	contextMenu->Append(idPlotContextCopy, _T("Copy"));
	//contextMenu->Append(idPlotContextPaste, _T("Paste"));
	contextMenu->Append(idPlotContextWriteImageFile, _T("Write Image File"));
	contextMenu->Append(idPlotContextExportData, _T("Export Data"));
	contextMenu->AppendSeparator();
	contextMenu->AppendCheckItem(idPlotContextMajorGridlines, _T("Major Gridlines"));
	contextMenu->AppendCheckItem(idPlotContextMinorGridlines, _T("Minor Gridlines"));
	contextMenu->AppendCheckItem(idPlotContextShowLegend, _T("Legend"));
	contextMenu->Append(idPlotContextAutoScale, _T("Auto Scale"));
	contextMenu->Append(idPlotContextBGColor, _T("Set Background Color"));
	contextMenu->Append(idPlotContextGridColor, _T("Set Gridline Color"));

	/*if (wxTheClipboard->Open())
	{
		if (!wxTheClipboard->IsSupported(wxDF_TEXT))
			contextMenu->Enable(idPlotContextPaste, false);
		wxTheClipboard->Close();
	}
	else
		contextMenu->Enable(idPlotContextPaste, false);*/

	contextMenu->Check(idPlotContextMajorGridlines, plotArea->GetMajorGridOn());
	contextMenu->Check(idPlotContextMinorGridlines, plotArea->GetMinorGridOn());
	contextMenu->Check(idPlotContextShowLegend, plotArea->LegendIsVisible());

	return contextMenu;
}

//==========================================================================
// Class:			PlotPanel
// Function:		CreateAxisContextMenu
//
// Description:		Displays a context menu for the specified plot axis.
//
// Input Arguments:
//		baseEventId	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxMenu*
//
//==========================================================================
wxMenu* PlotPanel::CreateAxisContextMenu(const unsigned int &baseEventId) const
{
	wxMenu* contextMenu = new wxMenu();

	unsigned int i = baseEventId;
	contextMenu->AppendCheckItem(i++, _T("Major Gridlines"));
	contextMenu->AppendCheckItem(i++, _T("Minor Gridlines"));
	contextMenu->Append(i++, _T("Auto Scale Axis"));
	contextMenu->Append(i++, _T("Set Range"));
	contextMenu->Append(i++, _T("Set Major Resolution"));
	contextMenu->AppendCheckItem(i++, _T("Logarithmic Scale"));
	contextMenu->Append(i++, _T("Edit Label"));

	return contextMenu;
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
void PlotPanel::ContextWriteImageFile(wxCommandEvent& WXUNUSED(event))
{
	// Get file name from user, then save to file with
	wxArrayString pathAndFileName = static_cast<MainFrame*>(GetParent())->GetFileNameFromUser(
			_T("Save Image File"), wxEmptyString, wxEmptyString,
		_T("Bitmap Image (*.bmp)|*.bmp|JPEG Image (*.jpg)|*.jpg|PNG Image (*.png)|*.png|TIFF Image (*.tif)|*.tif"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	
	// Make sure the user didn't cancel
	if (pathAndFileName.IsEmpty())
		return;
	
	if (WriteImageToFile(pathAndFileName[0]))
		Debugger::GetInstance() << "Image file written to" << pathAndFileName[0] << Debugger::PriorityHigh;
	else
		Debugger::GetInstance() << "Image file NOT written!" << Debugger::PriorityHigh;
}

//==========================================================================
// Class:			PlotPanel
// Function:		WriteImageToFile
//
// Description:		Writes plot to image file.
//
// Input Arguments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool PlotPanel::WriteImageToFile(wxString pathAndFileName)
{
	return plotArea->WriteImageToFile(pathAndFileName);
}

//==========================================================================
// Class:			PlotPanel
// Function:		SetXAxisGridText
//
// Description:		Sets the x-axis information in the grid.
//
// Input Arguments:
//		text	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::SetXAxisGridText(wxString text)
{
	if (!optionsGrid)
		return;

	optionsGrid->SetCellValue(0, 0, text);
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
void PlotPanel::ClearAllCurves()
{
	while (plotList.GetCount() > 0)
		RemoveCurve(0);
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
	ExpressionTree expression(&plotList);
	Dataset2D *mathChannel = new Dataset2D;

	double xAxisFactor(1.0);
	//GetXAxisScalingFactor(xAxisFactor);// No warning here:  it's only an issue for FFTs and filters; warning are generated then

	wxString errors = expression.Solve(mathString, *mathChannel, xAxisFactor);

	if (!errors.IsEmpty())
	{
		wxMessageBox(_T("Could not solve expression:\n\n") + errors, _T("Error Solving Expression"), wxICON_ERROR, this);
		delete mathChannel;

		DisplayMathChannelDialog(mathString);
		return;
	}

	AddCurve(mathChannel, mathString.Upper());
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

	optionsGrid->BeginBatch();
	if (optionsGrid->GetNumberRows() == 0)
		AddXRowToGrid();
	unsigned int index = AddDataRowToGrid(name);
	optionsGrid->EndBatch();

	optionsGrid->Scroll(-1, optionsGrid->GetNumberRows());

	plotArea->AddCurve(*data);
	UpdateCurveProperties(index - 1, GetNextColor(index), true, false);
	plotArea->UpdateDisplay();

	// Resize to prevent scrollbars and hidden values in the grid control
	//topSizer->Layout();
}

//==========================================================================
// Class:			PlotPanel
// Function:		AddXRowToGrid
//
// Description:		Adds the entry for the time data to the options grid.
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
void PlotPanel::AddXRowToGrid(void)
{
	optionsGrid->AppendRows();

	//SetXDataLabel(currentFileFormat);

	unsigned int i;
	for (i = 0; i < colCount; i++)
		optionsGrid->SetReadOnly(0, i, true);
}

//==========================================================================
// Class:			PlotPanel
// Function:		AddDataRowToGrid
//
// Description:		Adds the entry for the data to the options grid.
//
// Input Arguments:
//		name	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the index of the new data
//
//==========================================================================
unsigned int PlotPanel::AddDataRowToGrid(const wxString &name)
{
	unsigned int index = optionsGrid->GetNumberRows();
	optionsGrid->AppendRows();

	unsigned int maxMarkerSize(5);

	optionsGrid->SetCellRenderer(index, colVisible, new wxGridCellBoolRenderer);
	optionsGrid->SetCellRenderer(index, colRightAxis, new wxGridCellBoolRenderer);
	optionsGrid->SetCellEditor(index, colLineSize, new wxGridCellFloatEditor(1, 1));
	optionsGrid->SetCellEditor(index, colMarkerSize, new wxGridCellNumberEditor(-1, maxMarkerSize));

	unsigned int i;
	for (i = 1; i < colCount; i++)
			optionsGrid->SetReadOnly(index, i, true);
	optionsGrid->SetReadOnly(index, colLineSize, false);
	optionsGrid->SetReadOnly(index, colMarkerSize, false);
	optionsGrid->SetCellValue(index, colName, name);

	Color color = GetNextColor(index);

	optionsGrid->SetCellBackgroundColour(index, colColor, color.ToWxColor());
	optionsGrid->SetCellValue(index, colLineSize, wxString::Format(_T("%g"), defaultLineSize));
	optionsGrid->SetCellValue(index, colMarkerSize, wxString::Format(_T("%i"), defaultMarkerSize));
	optionsGrid->SetCellValue(index, colVisible, _T("1"));

	int width = optionsGrid->GetColSize(colName);
	optionsGrid->AutoSizeColumn(colName, false);
	if (optionsGrid->GetColSize(colName) < width)
		optionsGrid->SetColSize(colName, width);

	return index;
}

//==========================================================================
// Class:			PlotPanel
// Function:		GetNextColor
//
// Description:		Determines the next color to use (cycles through all the
//					pre-defined colors).
//
// Input Arguments:
//		index	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		Color to sue
//
//==========================================================================
Color PlotPanel::GetNextColor(const unsigned int &index) const
{
	unsigned int colorIndex = (index - 1) % 12;
	if (colorIndex == 0)
		return Color::GetColorHSL(0.0, 1.0, 0.5);// (red)
	else if (colorIndex == 1)
		return Color::GetColorHSL(2.0 / 3.0, 1.0, 0.5);// (blue)
	else if (colorIndex == 2)
		return Color::GetColorHSL(1.0 / 3.0, 1.0, 0.5);// (green)
	else if (colorIndex == 3)
		return Color::GetColorHSL(0.125, 1.0, 0.5);// (gold)
	else if (colorIndex == 4)
		return Color::GetColorHSL(0.5, 0.5, 0.5);// (teal)
	else if (colorIndex == 5)
		return Color::GetColorHSL(5.0 / 6.0, 1.0, 0.5);// (magenta)
	else if (colorIndex == 6)
		return Color::GetColorHSL(0.0, 0.5, 0.6);// (reddish brown)
	else if (colorIndex == 7)
		return Color::GetColorHSL(0.73, 0.5, 0.5);// (purple)
	else if (colorIndex == 8)
		return Color::GetColorHSL(1.0 / 3.0, 0.5, 0.5);// (dark green)
	else if (colorIndex == 9)
		return Color::GetColorHSL(1.0 / 6.0, 0.3, 0.5);// (gold brown)
	else if (colorIndex == 10)
		return Color::GetColorHSL(0.875, 0.5, 0.5);// (light purple)
	else if (colorIndex == 11)
		return Color::ColorBlack;
	else
		assert(false);

	return Color::ColorBlack;
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
	optionsGrid->DeleteRows(i + 1);

	if (optionsGrid->GetNumberRows() == 1)
		optionsGrid->DeleteRows();

	optionsGrid->AutoSizeColumns();

	plotArea->RemoveCurve(i);
	plotList.Remove(i);

	UpdateLegend();
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
	CreateGridContextMenu(event.GetPosition() + optionsGrid->GetPosition()
		+ optionsGrid->GetParent()->GetPosition(), event.GetRow());
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
		UpdateCurveProperties(row - 1);
	}
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

	ShowAppropriateXLabel();

	UpdateCurveProperties(row - 1);
}

//==========================================================================
// Class:			PlotPanel
// Function:		UpdateCurveProperties
//
// Description:		Updates the specified curve properties.
//
// Input Arguments:
//		index	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::UpdateCurveProperties(const unsigned int &index)
{
	Color color;
	color.Set(optionsGrid->GetCellBackgroundColour(index + 1, colColor));
	UpdateCurveProperties(index, color,
		!optionsGrid->GetCellValue(index + 1, colVisible).IsEmpty(),
		!optionsGrid->GetCellValue(index + 1, colRightAxis).IsEmpty());
}

//==========================================================================
// Class:			PlotPanel
// Function:		UpdateCurveProperties
//
// Description:		Updates the specified curve properties to match the arguments.
//
// Input Arguments:
//		index		= const unsigned int&
//		color		= const Color&
//		visible		= const bool&
//		rightAxis	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::UpdateCurveProperties(const unsigned int &index, const Color &color,
	const bool &visible, const bool &rightAxis)
{
	double lineSize;
	long markerSize;
	UpdateLegend();// Must come first in order to be updated simultaneously with line
	optionsGrid->GetCellValue(index + 1, colLineSize).ToDouble(&lineSize);
	optionsGrid->GetCellValue(index + 1, colMarkerSize).ToLong(&markerSize);
	plotArea->SetCurveProperties(index, color, visible, rightAxis, lineSize, markerSize);
	plotArea->SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotPanel
// Function:		UpdateLegend
//
// Description:		Updates the contents of the legend actor.
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
void PlotPanel::UpdateLegend()
{
	double lineSize;
	long markerSize;
	std::vector<Legend::LegendEntryInfo> entries;
	Legend::LegendEntryInfo info;
	int i;
	for (i = 1; i < optionsGrid->GetNumberRows(); i++)
	{
		if (optionsGrid->GetCellValue(i, colVisible).IsEmpty())
			continue;
			
		optionsGrid->GetCellValue(i, colLineSize).ToDouble(&lineSize);
		optionsGrid->GetCellValue(i, colMarkerSize).ToLong(&markerSize);
		info.color = Color(optionsGrid->GetCellBackgroundColour(i, colColor));
		info.lineSize = lineSize;
		info.markerSize = markerSize;
		info.text = optionsGrid->GetCellValue(i, colName);
		entries.push_back(info);
	}
	plotArea->UpdateLegend(entries);
}

//==========================================================================
// Class:			PlotPanel
// Function:		ShowAppropriateXLabel
//
// Description:		Updates the x-axis label as necessary.
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
void PlotPanel::ShowAppropriateXLabel()
{
	// If the only visible curves are frequency plots, change the x-label
	int i;
	bool showFrequencyLabel(false);
	for (i = 1; i < optionsGrid->GetNumberRows(); i++)
	{
		if (optionsGrid->GetCellValue(i, colVisible).Cmp(_T("1")) == 0)
		{
			if (optionsGrid->GetCellValue(i, colName).Mid(0, 3).CmpNoCase(_T("FFT")) == 0 ||
				optionsGrid->GetCellValue(i, colName).Mid(0, 3).CmpNoCase(_T("FRF")) == 0)
				showFrequencyLabel = true;
			else
			{
				showFrequencyLabel = false;
				break;
			}
		}
	}

	/*if (showFrequencyLabel)
		SetXDataLabel(FormatFrequency);
	else
		SetXDataLabel(currentFileFormat);*/
}

//==========================================================================
// Class:			PlotPanel
// Function:		GetDefaultLineSize
//
// Description:		Returns the value to be used for default line size.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PlotPanel::GetDefaultLineSize() const
{
	if (optionsGrid->GetNumberRows() < 2)
		return defaultLineSize;

	double size;
	if (!optionsGrid->GetCellValue(1, colLineSize).ToDouble(&size))
		return defaultLineSize;

	return size;
}

//==========================================================================
// Class:			PlotPanel
// Function:		GetDefaultMarkerSize
//
// Description:		Returns the value to be used for default marker size.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		int
//
//==========================================================================
int PlotPanel::GetDefaultMarkerSize() const
{
	if (optionsGrid->GetNumberRows() < 2)
		return defaultMarkerSize;

	long size;
	if (!optionsGrid->GetCellValue(1, colMarkerSize).ToLong(&size))
		return defaultMarkerSize;

	return size;
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
	unsigned int row(event.GetRow());
	if (row == 0 || (event.GetCol() != colLineSize && event.GetCol() != colMarkerSize))
	{
		event.Skip();
		UpdateLegend();// Included in case of text changes
		plotArea->Refresh();
		return;
	}

	UpdateCurveProperties(row - 1);
}

//==========================================================================
// Class:			PlotPanel
// Function:		GridLabelRightClickEvent
//
// Description:		Handles right-click events in blank areas of grid control.
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
void PlotPanel::GridLabelRightClickEvent(wxGridEvent &event)
{
	wxMenu *contextMenu = new wxMenu();

	contextMenu->Append(idContextCreateSignal, _T("Create Signal"));

	PopupMenu(contextMenu);

	delete contextMenu;
	contextMenu = NULL;
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
	DisplayMathChannelDialog(wxString::Format("[%i]", optionsGrid->GetSelectedRows()[0]));
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextFRFEvent
//
// Description:		Event handler for context menu transfer function events.
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
void PlotPanel::ContextFRFEvent(wxCommandEvent& WXUNUSED(event))
{
	/*double factor;
	if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	wxArrayString descriptions;
	int i;
	for (i = 1; i < optionsGrid->GetNumberRows(); i++)
		descriptions.Add(optionsGrid->GetCellValue(i, 0));

	FRFDialog dialog(this, descriptions);
	if (dialog.ShowModal() != wxID_OK)
		return;

	Dataset2D *amplitude = new Dataset2D, *phase = NULL, *coherence = NULL;

	if (dialog.GetComputePhase())
		phase = new Dataset2D;
	if (dialog.GetComputeCoherence())
		coherence = new Dataset2D;

	if (!VVASEMath::XDataConsistentlySpaced(*plotList[dialog.GetInputIndex()]) ||
		!VVASEMath::XDataConsistentlySpaced(*plotList[dialog.GetOutputIndex()]))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	FastFourierTransform::ComputeFRF(*plotList[dialog.GetInputIndex()],
		*plotList[dialog.GetOutputIndex()], dialog.GetNumberOfAverages(),
		FastFourierTransform::WindowHann, dialog.GetModuloPhase(), *amplitude, phase, coherence);

	AddFFTCurves(factor, amplitude, phase, coherence, wxString::Format("[%u] to [%u]",
		dialog.GetInputIndex(), dialog.GetOutputIndex()));*/
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextCreateSignalEvent
//
// Description:		Displays dialog for creating various signals.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::ContextCreateSignalEvent(wxCommandEvent& WXUNUSED(event))
{
	/*double startTime(0.0);// [sec]
	double duration(10.0);// [sec]
	double sampleRate(100.0);// [Hz]

	double factor(1.0);
	if (plotList.GetCount() > 0)
	{
		GetXAxisScalingFactor(factor);

		// Use first curve to pull time and frequency information
		sampleRate = 1.0 / VVASEMath::GetAverageXSpacing(*plotList[0]) * factor;
		startTime = plotList[0]->GetXData(0) / factor;
		duration = plotList[0]->GetXData(plotList[0]->GetNumberOfPoints() - 1) / factor - startTime;
	}

	CreateSignalDialog dialog(this, startTime, duration, sampleRate);

	if (dialog.ShowModal() != wxID_OK)
		return;

	AddCurve(&dialog.GetSignal()->MultiplyXData(factor), dialog.GetSignalName());

	// Set time units if it hasn't been done already
	double dummy;
	if (!GetXAxisScalingFactor(dummy))
	{
		genericXAxisLabel = _T("Time [sec]");
		SetXDataLabel(genericXAxisLabel);
		//plotArea->SaveCurrentZoom();// TODO:  Is this necessary?
	}*/
}

//==========================================================================
// Class:			PlotPanel
// Function:		AddFFTCurves
//
// Description:		Adds the FFT curves to the plot list.
//
// Input Arguments:
//		xFactor	= const double& scaling factor to convert X units to Hz
//		amplitude	= Dataset2D*
//		phase		= Dataset2D*
//		coherence	= Dataset2D*
//		namePortion	= const wxString& identifying the input/output signals
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
/*void PlotPanel::AddFFTCurves(const double& xFactor, Dataset2D *amplitude, Dataset2D *phase,
	Dataset2D *coherence, const wxString &namePortion)
{
	AddCurve(&(amplitude->MultiplyXData(xFactor)), _T("FRF Amplitude, ") + namePortion + _T(", [dB]"));
	SetMarkerSize(optionsGrid->GetRows() - 2, 0);

	if (phase)
	{
		AddCurve(&(phase->MultiplyXData(xFactor)), _T("FRF Phase, ") + namePortion + _T(", [deg]"));
		SetMarkerSize(optionsGrid->GetRows() - 2, 0);
	}

	if (coherence)
	{
		AddCurve(&(coherence->MultiplyXData(xFactor)), _T("FRF Coherence, ") + namePortion + _T(", [-]"));
		SetMarkerSize(optionsGrid->GetRows() - 2, 0);
	}
}*/

//==========================================================================
// Class:			PlotPanel
// Function:		ContextSetTimeUnitsEvent
//
// Description:		Available for the user to clarify the time units when we
//					are unable to determine them easily from the input file.
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
void PlotPanel::ContextSetTimeUnitsEvent(wxCommandEvent& WXUNUSED(event))
{
	/*double f;
	wxString units;

	// Check to see if we already have some confidence in our x-axis units
	if (GetXAxisScalingFactor(f, &units))
	{
		// Ask the user to confirm, since we don't think we need their help
		if (wxMessageBox(_T("Time units are being interpreted as ") + units +
			_T(", are you sure you want to change them?"), _T("Are You Sure?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this) == wxNO)
			return;
	}

	// Ask the user to specify the correct units
	wxString userUnits;
	userUnits = ::wxGetTextFromUser(_T("Specify time units (e.g. \"msec\" or \"minutes\")"),
		_T("Specify Units"), _T("seconds"), this);

	// If the user cancelled, we will have a blank string
	if (userUnits.IsEmpty())
		return;

	// Check to make sure we understand what the user specified
	wxString currentLabel(optionsGrid->GetCellValue(0, colName));
	genericXAxisLabel = _T("Time, [") + userUnits + _T("]");
	SetXDataLabel(genericXAxisLabel);
	if (!GetXAxisScalingFactor(f, &units))
	{
		// Set the label back to what it used to be and warn the user
		SetXDataLabel(currentLabel);
		wxMessageBox(_T("Could not understand units \"") + userUnits + _T("\"."), _T("Error Setting Units"), wxICON_ERROR, this);
	}*/
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextScaleXDataEvent
//
// Description:		Scales the X-data by the specified factor.
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
void PlotPanel::ContextScaleXDataEvent(wxCommandEvent& WXUNUSED(event))
{
	/*double factor(0.0);
	wxString factorText(_T("0.0"));

	while (!factorText.ToDouble(&factor) || factor == 0.0)
	{
		factorText = ::wxGetTextFromUser(_T("Specify scaling factor:"),
		_T("Specify Factor"), _T("1"), this);
		if (factorText.IsEmpty())
			return;
	}

	wxArrayInt selectedRows = optionsGrid->GetSelectedRows();
	unsigned int i;

	// If applied to the row 0, apply to all curves
	if (selectedRows.Count() == 1 && selectedRows[0] == 0)
	{
		unsigned int stopIndex(plotList.GetCount());
		for (i = 0; i < stopIndex; i++)
		{
			Dataset2D *scaledData = new Dataset2D(*plotList[i]);
			scaledData->MultiplyXData(factor);
			AddCurve(scaledData, optionsGrid->GetCellValue(i + 1, colName));

			optionsGrid->SetCellBackgroundColour(
				optionsGrid->GetCellBackgroundColour(i + 1, colColor),
				i + stopIndex + 1, colColor);
			optionsGrid->SetCellValue(optionsGrid->GetCellValue(i + 1, colLineSize),
				i + stopIndex + 1, colLineSize);
			optionsGrid->SetCellValue(optionsGrid->GetCellValue(i + 1, colMarkerSize),
				i + stopIndex + 1, colMarkerSize);
			optionsGrid->SetCellValue(optionsGrid->GetCellValue(i + 1, colVisible),
				i + stopIndex + 1, colVisible);
			optionsGrid->SetCellValue(optionsGrid->GetCellValue(i + 1, colRightAxis),
				i + stopIndex + 1, colRightAxis);

			UpdateCurveProperties(i + stopIndex);
		}

		for (i = stopIndex; i > 0; i--)
			RemoveCurve(i - 1);
	}
	// If applied to any other row, apply only to that row (by duplicating curve)
	else
	{
		for (i = 0; i < selectedRows.Count(); i++)
		{
			Dataset2D *scaledData = new Dataset2D(*plotList[selectedRows[i] - 1]);
			scaledData->MultiplyXData(factor);
			AddCurve(scaledData, optionsGrid->GetCellValue(selectedRows[i], colName)
				+ wxString::Format(", X-scaled by %f", factor));
		}
	}*/
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

	wxString name = _T("d/dx(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextPlotIntegralEvent
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
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextPlotRMSEvent
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
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextPlotFFTEvent
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
/*void PlotPanel::ContextPlotFFTEvent(wxCommandEvent& WXUNUSED(event))
{
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = GetFFTData(plotList[row - 1]);
	if (!newData)
		return;

	wxString name = _T("FFT(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);
	SetMarkerSize(optionsGrid->GetRows() - 2, 0);
}*/

//==========================================================================
// Class:			PlotPanel
// Function:		ContextBitMaskEvent
//
// Description:		Creates bit mask for the specified curve.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::ContextBitMaskEvent(wxCommandEvent& WXUNUSED(event))
{
	unsigned long bit;
	wxString bitString = wxGetTextFromUser(_T("Specify the bit to plot:"), _T("Bit Seleciton"), _T("0"), this);
	if (bitString.IsEmpty())
		return;
	else if (!bitString.ToULong(&bit))
	{
		wxMessageBox(_T("Bit value must be a positive integer."), _T("Bit Selection Error"), wxICON_ERROR, this);
		return;
	}

	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(VVASEMath::ApplyBitMask(*plotList[row - 1], bit));

	wxString name = optionsGrid->GetCellValue(row, colName) + _T(", Bit ") + wxString::Format("%lu", bit);
	AddCurve(newData, name);
}

//==========================================================================
// Class:			PlotPanel
// Function:		GetFFTData
//
// Description:		Returns a dataset containing an FFT of the specified data.
//
// Input Arguments:
//		data				= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D* pointing to a dataset contining the new FFT data
//
//==========================================================================
/*Dataset2D* PlotPanel::GetFFTData(const Dataset2D* data)
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	FFTDialog dialog(this, data->GetNumberOfPoints(),
		data->GetNumberOfZoomedPoints(plotArea->GetXMin(), plotArea->GetXMax()),
		data->GetAverageDeltaX() / factor);

	if (dialog.ShowModal() != wxID_OK)
		return NULL;

	if (!VVASEMath::XDataConsistentlySpaced(*data))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	Dataset2D *newData;

	if (dialog.GetUseZoomedData())
		newData = new Dataset2D(FastFourierTransform::ComputeFFT(GetXZoomedDataset(*data),
			dialog.GetFFTWindow(), dialog.GetWindowSize(), dialog.GetOverlap(),
			dialog.GetSubtractMean()));
	else
		newData = new Dataset2D(FastFourierTransform::ComputeFFT(*data,
			dialog.GetFFTWindow(), dialog.GetWindowSize(), dialog.GetOverlap(),
			dialog.GetSubtractMean()));

	newData->MultiplyXData(factor);

	return newData;
}*/

//==========================================================================
// Class:			PlotPanel
// Function:		GetXZoomedDataset
//
// Description:		Returns a dataset containing only the data within the
//					current zoomed x-limits.
//
// Input Arguments:
//		fullData	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D PlotPanel::GetXZoomedDataset(const Dataset2D &fullData) const
{
	unsigned int i, startIndex(0), endIndex(0);
	while (fullData.GetXData(startIndex) < plotArea->GetXMin() &&
		startIndex < fullData.GetNumberOfPoints())
		startIndex++;
	endIndex = startIndex;
	while (fullData.GetXData(endIndex) < plotArea->GetXMax() &&
		endIndex < fullData.GetNumberOfPoints())
		endIndex++;

	Dataset2D data(endIndex - startIndex);
	for (i = startIndex; i < endIndex; i++)
	{
		data.GetXPointer()[i - startIndex] = fullData.GetXData(i);
		data.GetYPointer()[i - startIndex] = fullData.GetYData(i);
	}

	return data;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextTimeShiftEvent
//
// Description:		Adds a new curve equivalent to the selected curve shifted
//					by the specified amount.
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
void PlotPanel::ContextTimeShiftEvent(wxCommandEvent& WXUNUSED(event))
{
	double shift(0.0);
	wxString shiftText = ::wxGetTextFromUser(
		_T("Specify the time to add to time data in original data:\n")
		_T("Use same units as time series.  Positive values shift curve to the right."),
		_T("Time Shift"), _T("0"), this);

	if (!shiftText.ToDouble(&shift) || shift == 0.0)
		return;

	// Create new dataset containing the RMS of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(*plotList[row - 1]);

	newData->XShift(shift);

	wxString name = optionsGrid->GetCellValue(row, colName) + _T(", t = t0 + ");
	name += shiftText;
	AddCurve(newData, name);
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextFilterEvent
//
// Description:		Displays a dialog allowing the user to specify the filter,
//					and adds the filtered curve to the plot.
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
void PlotPanel::ContextFilterEvent(wxCommandEvent& WXUNUSED(event))
{
	/*// Display dialog
	FilterParameters filterParameters = DisplayFilterDialog();
	if (filterParameters.order == 0)
		return;

	// Create new dataset containing the FFT of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	const Dataset2D *currentData = plotList[row - 1];
	Dataset2D *newData = new Dataset2D(*currentData);

	ApplyFilter(filterParameters, *newData);

	wxString name = FilterDialog::GetFilterNamePrefix(filterParameters) + _T(" (") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);*/
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextFitCurve
//
// Description:		Fits a curve to the dataset selected in the grid control.
//					User is asked to specify the order of the fit.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
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

	// If cancelled, the orderString will be empty.  It is possible that the user cleared the textbox
	// and clicked OK, but we'll ignore this case since we can't tell the difference
	if (orderString.IsEmpty())
		return;

	if (!orderString.ToULong(&order))
	{
		wxMessageBox(_T("ERROR:  Order must be a positive integer!"), _T("Error Fitting Curve"), wxICON_ERROR, this);
		return;
	}

	wxString name;
	Dataset2D* newData = GetCurveFitData(order, plotList[optionsGrid->GetSelectedRows()[0] - 1], name);

	AddCurve(newData, name);
}

//==========================================================================
// Class:			PlotPanel
// Function:		GetCurveFitData
//
// Description:		Fits a curve of the specified order to the specified data
//					and returns a dataset containing the curve.
//
// Input Arguments:
//		order	= const unsigned int&
//		data	= const Dataset2D*
//
// Output Arguments:
//		name	= wxString&
//
// Return Value:
//		None
//
//==========================================================================
Dataset2D* PlotPanel::GetCurveFitData(const unsigned int &order,
	const Dataset2D* data, wxString &name) const
{
	CurveFit::PolynomialFit fitData = CurveFit::DoPolynomialFit(*data, order);

	Dataset2D *newData = new Dataset2D(*data);
	unsigned int i;
	for (i = 0; i < newData->GetNumberOfPoints(); i++)
		newData->GetYPointer()[i] = CurveFit::EvaluateFit(newData->GetXData(i), fitData);

	name = GetCurveFitName(fitData, optionsGrid->GetSelectedRows()[0]);

	delete [] fitData.coefficients;

	return newData;
}

//==========================================================================
// Class:			PlotPanel
// Function:		GetCurveFitName
//
// Description:		Determines an appropriate name for a curve fit dataset.
//
// Input Arguments:
//		fitData	= const CurveFit::PolynomialFit&
//		row		= const unsigned int& specifying the dataset ID that was fit
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString indicating the name for the fit
//
//==========================================================================
wxString PlotPanel::GetCurveFitName(const CurveFit::PolynomialFit &fitData,
	const unsigned int &row) const
{
	wxString name, termString;
	//name.Printf("Order %lu Fit([%i]), R^2 = %0.2f", order, row, fitData.rSquared);
	name.Printf("Fit [%i] (R^2 = %0.2f): ", row, fitData.rSquared);

	unsigned int i;
	for (i = 0; i <= fitData.order; i++)
	{
		if (i == 0)
			termString.Printf("%1.2e", fitData.coefficients[i]);
		else if (i == 1)
			termString.Printf("%0.2ex", fabs(fitData.coefficients[i]));
		else
			termString.Printf("%0.2ex^%i", fabs(fitData.coefficients[i]), i);

		if (i < fitData.order)
		{
			if (fitData.coefficients[i + 1] > 0.0)
				termString.Append(_T(" + "));
			else
				termString.Append(_T(" - "));
		}
		name.Append(termString);
	}

	return name;
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextCopy
//
// Description:		Handles context menu copy command events.
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
void PlotPanel::ContextCopy(wxCommandEvent& WXUNUSED(event))
{
	DoCopy();
}

#if 0
//==========================================================================
// Class:			PlotPanel
// Function:		ContextPaste
//
// Description:		Handles context menu paste command events.
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
void PlotPanel::ContextPaste(wxCommandEvent& WXUNUSED(event))
{
	DoPaste();
}
#endif

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleMajorGridlines
//
// Description:		Toggles major gridlines for the entire plot on and off.
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
void PlotPanel::ContextToggleMajorGridlines(wxCommandEvent& WXUNUSED(event))
{
	if (plotArea->GetMajorGridOn())
		plotArea->SetMajorGridOff();
	else
		plotArea->SetMajorGridOn();

	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleMinorGridlines
//
// Description:		Toggles minor gridlines for the entire plot on and off.
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
void PlotPanel::ContextToggleMinorGridlines(wxCommandEvent& WXUNUSED(event))
{
	if (plotArea->GetMinorGridOn())
		plotArea->SetMinorGridOff();
	else
		plotArea->SetMinorGridOn();

	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleLegend
//
// Description:		Toggles legend visibility on and off.
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
void PlotPanel::ContextToggleLegend(wxCommandEvent& WXUNUSED(event))
{
	if (plotArea->LegendIsVisible())
		plotArea->SetLegendOff();
	else
		plotArea->SetLegendOn();

	plotArea->UpdateDisplay();
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
	plotArea->AutoScale();
	plotArea->UpdateDisplay();
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

	// TODO:  This would be nicer with smart precision so we show enough digits but not too many

	// For each curve, update the cursor values
	int i;
	bool showXDifference(false);
	for (i = 1; i < optionsGrid->GetNumberRows(); i++)
	{
		UpdateSingleCursorValue(i, leftValue, colLeftCursor, leftVisible);
		UpdateSingleCursorValue(i, rightValue, colRightCursor, rightVisible);

		if (leftVisible && rightVisible)
		{
			double left, right;
			if (plotList[i - 1]->GetYAt(leftValue, left) && plotList[i - 1]->GetYAt(rightValue, right))
			{
				optionsGrid->SetCellValue(i, colDifference, wxString::Format("%f", right - left));
				showXDifference = true;
			}
			else
				optionsGrid->SetCellValue(i, colDifference, wxEmptyString);
		}
	}

	if (showXDifference)
		optionsGrid->SetCellValue(0, colDifference, wxString::Format("%f", rightValue - leftValue));
}

//==========================================================================
// Class:			PlotPanel
// Function:		UpdateSingleCursorValue
//
// Description:		Updates a single cursor value.
//
// Input Arguments:
//		row			= const unsigned int& specifying the grid row
//		value		= const double& specifying the value to populate
//		column		= const unsigned int& specifying which grid column to populate
//		isVisible	= const bool& indicating whether or not the cursor is visible
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::UpdateSingleCursorValue(const unsigned int &row,
	double value, const unsigned int &column, const bool &isVisible)
{
	if (isVisible)
	{
		optionsGrid->SetCellValue(0, column, wxString::Format("%f", value));

		bool exact;
		double valueOut;
		if (plotList[row - 1]->GetYAt(value, valueOut, &exact))
		{
			if (exact)
				optionsGrid->SetCellValue(row, column, _T("*") + wxString::Format("%f", valueOut));
			else
				optionsGrid->SetCellValue(row, column, wxString::Format("%f", valueOut));
		}
		else
			optionsGrid->SetCellValue(row, column, wxEmptyString);
	}
	else
	{
		optionsGrid->SetCellValue(0, column, wxEmptyString);
		optionsGrid->SetCellValue(row, column, wxEmptyString);

		// The difference column only exists if both cursors are visible
		optionsGrid->SetCellValue(0, colDifference, wxEmptyString);
		optionsGrid->SetCellValue(row, colDifference, wxEmptyString);
	}
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
	//message.Append(_T("    Valid operations are: +, -, *, /, %, ddt, int, fft and trigonometric functions.\n"));
	message.Append(_T("    Valid operations are: +, -, *, /, %, ddx, int and trigonometric functions.\n"));
	message.Append(_T("    Use () to specify order of operations"));

	AddCurve(::wxGetTextFromUser(message, _T("Specify Math Channel"), defaultInput, this));
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
	double min, max;
	if (!GetCurrentAxisRange(axis, min, max))
		return;

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
		wxMessageBox(_T("ERROR:  Limits must unique!"), _T("Error Setting Limits"), wxICON_ERROR, this);
		return;
	}

	SetNewAxisRange(axis, min, max);
	plotArea->SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotPanel
// Function:		GetCurrentAxisRange
//
// Description:		Returns the range for the specified axis.
//
// Input Arguments:
//		axis	= const PlotContext&
//
// Output Arguments:
//		min		= double&
//		max		= double&
//
// Return Value:
//		bool, true on success, false otherwise
//
//==========================================================================
bool PlotPanel::GetCurrentAxisRange(const PlotContext &axis, double &min, double &max) const
{
	switch (axis)
	{
	case PlotContextXAxis:
		min = plotArea->GetXMin();
		max = plotArea->GetXMax();
		break;

	case PlotContextLeftYAxis:
		min = plotArea->GetLeftYMin();
		max = plotArea->GetLeftYMax();
		break;

	case PlotContextRightYAxis:
		min = plotArea->GetRightYMin();
		max = plotArea->GetRightYMax();
		break;

	default:
	case PlotContextPlotArea:
		// Plot area is not a valid context in which we can set axis limits
		return false;
	}

	return true;
}

//==========================================================================
// Class:			PlotPanel
// Function:		SetNewAxisRange
//
// Description:		Returns the range for the specified axis.
//
// Input Arguments:
//		axis	= const PlotContext&
//		min		= const double&
//		max		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::SetNewAxisRange(const PlotContext &axis, const double &min, const double &max)
{
	switch (axis)
	{
	case PlotContextLeftYAxis:
		plotArea->SetLeftYLimits(min, max);
		break;

	case PlotContextRightYAxis:
		plotArea->SetRightYLimits(min, max);
		break;

	default:
	case PlotContextXAxis:
		plotArea->SetXLimits(min, max);
	}

	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleMajorGridlinesBottom
//
// Description:		Toggles major gridlines for the bottom axis.
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
void PlotPanel::ContextToggleMajorGridlinesBottom(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetBottomMajorGrid(!plotArea->GetBottomMajorGrid());
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleMinorGridlinesBottom
//
// Description:		Toggles major gridlines for the bottom axis.
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
void PlotPanel::ContextToggleMinorGridlinesBottom(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetBottomMinorGrid(!plotArea->GetBottomMinorGrid());
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
	plotArea->AutoScaleBottom();
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
	DisplayAxisRangeDialog(PlotContextXAxis);
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextSetMajorResolutionBottom
//
// Description:		Dispalys a dialog box for setting the axis major resolution.
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
void PlotPanel::ContextSetMajorResolutionBottom(wxCommandEvent& WXUNUSED(event))
{
	double resolution(plotArea->GetBottomMajorResolution());
	wxString resStr;
	do {
		resStr = wxGetTextFromUser(_T("Specify the major resolution (set to 0 for auto):"),
			_T("Set Major Resolution"),
			wxString::Format("%f", std::max(resolution, 0.0)), this);
		if (resStr.IsEmpty())
			return;
		resStr.ToDouble(&resolution);
	} while (resolution < 0.0);

	plotArea->SetBottomMajorResolution(resolution);
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleMajorGridlinesLeft
//
// Description:		Toggles major gridlines for the left axis.
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
void PlotPanel::ContextToggleMajorGridlinesLeft(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetLeftMajorGrid(!plotArea->GetLeftMajorGrid());
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleMinorGridlinesLeft
//
// Description:		Toggles major gridlines for the left axis.
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
void PlotPanel::ContextToggleMinorGridlinesLeft(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetLeftMinorGrid(!plotArea->GetLeftMinorGrid());
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
	plotArea->AutoScaleLeft();
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
	DisplayAxisRangeDialog(PlotContextLeftYAxis);
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextSetMajorResolutionLeft
//
// Description:		Dispalys a dialog box for setting the axis major resolution.
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
void PlotPanel::ContextSetMajorResolutionLeft(wxCommandEvent& WXUNUSED(event))
{
	double resolution(plotArea->GetLeftMajorResolution());
	wxString resStr;
	do {
		resStr = wxGetTextFromUser(_T("Specify the major resolution (set to 0 for auto):"),
			_T("Set Major Resolution"),
			wxString::Format("%f", std::max(resolution, 0.0)), this);
		if (resStr.IsEmpty())
			return;
		resStr.ToDouble(&resolution);
	} while (resolution < 0.0);

	plotArea->SetLeftMajorResolution(resolution);
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleMajorGridlinesRight
//
// Description:		Toggles major gridlines for the right axis.
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
void PlotPanel::ContextToggleMajorGridlinesRight(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetRightMajorGrid(!plotArea->GetRightMajorGrid());
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextToggleMinorGridlinesRight
//
// Description:		Toggles minor gridlines for the right axis.
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
void PlotPanel::ContextToggleMinorGridlinesRight(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetRightMinorGrid(!plotArea->GetRightMinorGrid());
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
	plotArea->AutoScaleRight();
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
	DisplayAxisRangeDialog(PlotContextRightYAxis);
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextSetMajorResolutionRight
//
// Description:		Dispalys a dialog box for setting the axis major resolution.
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
void PlotPanel::ContextSetMajorResolutionRight(wxCommandEvent& WXUNUSED(event))
{
	double resolution(plotArea->GetRightMajorResolution());
	wxString resStr;
	do {
		resStr = wxGetTextFromUser(_T("Specify the major resolution (set to 0 for auto):"),
			_T("Set Major Resolution"),
			wxString::Format("%f", std::max(resolution, 0.0)), this);
		if (resStr.IsEmpty())
			return;
		resStr.ToDouble(&resolution);
	} while (resolution < 0.0);

	plotArea->SetRightMajorResolution(resolution);
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
	colorData.SetColour(plotArea->GetBackgroundColor().ToWxColor());

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
	dialog.SetTitle(_T("Choose Background Color"));
	if (dialog.ShowModal() == wxID_OK)
    {
		Color color;
		color.Set(dialog.GetColourData().GetColour());
		plotArea->SetBackgroundColor(color);
		plotArea->UpdateDisplay();
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
	colorData.SetColour(plotArea->GetGridColor().ToWxColor());

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
	dialog.SetTitle(_T("Choose Background Color"));
	if (dialog.ShowModal() == wxID_OK)
    {
		Color color;
		color.Set(dialog.GetColourData().GetColour());
		plotArea->SetGridColor(color);
		plotArea->UpdateDisplay();
	}
}

//==========================================================================
// Class:			PlotPanel
// Function:		DisplayFilterDialog
//
// Description:		Dispalys a dialog box allowing the user to specify a filter,
//					returns the specified parameters.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		FilterParameters describing the user-specified filter (order = 0 for cancelled dialog)
//
//==========================================================================
/*FilterParameters PlotPanel::DisplayFilterDialog()
{
	FilterDialog dialog(this);
	if (dialog.ShowModal() != wxID_OK)
	{
		FilterParameters parameters;
		parameters.order = 0;
		return parameters;
	}

	return dialog.GetFilterParameters();
}

//==========================================================================
// Class:			PlotPanel
// Function:		ApplyFilter
//
// Description:		Applies the specified filter to the specified dataset.
//
// Input Arguments:
//		parameters	= const FilterParameters&
//		data		= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::ApplyFilter(const FilterParameters &parameters, Dataset2D &data)
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Cutoff frequency may be incorrect!"),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	if (!VVASEMath::XDataConsistentlySpaced(data))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	Filter *filter = GetFilter(parameters, factor / data.GetAverageDeltaX(), data.GetYData(0));

	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
		data.GetYPointer()[i] = filter->Apply(data.GetYData(i));

	// For phaseless filter, re-apply the same filter backwards
	if (parameters.phaseless)
	{
		data.Reverse();
		filter->Initialize(data.GetYData(0));
		for (i = 0; i < data.GetNumberOfPoints(); i++)
			data.GetYPointer()[i] = filter->Apply(data.GetYData(i));
		data.Reverse();
	}

	delete filter;
}

//==========================================================================
// Class:			PlotPanel
// Function:		GetFilter
//
// Description:		Returns a filter matching the specified parameters.
//
// Input Arguments:
//		parameters		= const FilterParameters&
//		sampleRate		= const double& [Hz]
//		initialValue	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		Filter*
//
//==========================================================================
Filter* PlotPanel::GetFilter(const FilterParameters &parameters,
	const double &sampleRate, const double &initialValue) const
{
	return new Filter(sampleRate, Filter::CoefficientsFromString(std::string(parameters.numerator.mb_str())),
		Filter::CoefficientsFromString(std::string(parameters.denominator.mb_str())), initialValue);
}*/

//==========================================================================
// Class:			PlotPanel
// Function:		ContextSetLogarithmicBottom
//
// Description:		Event handler for right Y-axis context menu Set Logarithmic event.
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
void PlotPanel::ContextSetLogarithmicBottom(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetXLogarithmic(!plotArea->GetXLogarithmic());
	plotArea->ClearZoomStack();
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextSetLogarithmicLeft
//
// Description:		Event handler for right Y-axis context menu Set Logarithmic event.
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
void PlotPanel::ContextSetLogarithmicLeft(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetLeftLogarithmic(!plotArea->GetLeftLogarithmic());
	plotArea->ClearZoomStack();
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextSetLogarithmicRight
//
// Description:		Event handler for right Y-axis context menu Set Logarithmic event.
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
void PlotPanel::ContextSetLogarithmicRight(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetRightLogarithmic(!plotArea->GetRightLogarithmic());
	plotArea->ClearZoomStack();
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextEditBottomLabel
//
// Description:		Displays a message box asking the user to specify the text
//					for the label.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::ContextEditBottomLabel(wxCommandEvent& WXUNUSED(event))
{
	TextInputDialog dialog(_T("Specify label text:"), _T("Edit Label"), plotArea->GetXLabel(), this);
	if (dialog.ShowModal() == wxID_OK)
		plotArea->SetXLabel(dialog.GetText());
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextEditLeftLabel
//
// Description:		Displays a message box asking the user to specify the text
//					for the label.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::ContextEditLeftLabel(wxCommandEvent& WXUNUSED(event))
{
	TextInputDialog dialog(_T("Specify label text:"), _T("Edit Label"), plotArea->GetLeftYLabel(), this);
	if (dialog.ShowModal() == wxID_OK)
		plotArea->SetLeftYLabel(dialog.GetText());
}

//==========================================================================
// Class:			PlotPanel
// Function:		ContextEditRightLabel
//
// Description:		Displays a message box asking the user to specify the text
//					for the label.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::ContextEditRightLabel(wxCommandEvent& WXUNUSED(event))
{
	TextInputDialog dialog(_T("Specify label text:"), _T("Edit Label"), plotArea->GetRightYLabel(), this);
	if (dialog.ShowModal() == wxID_OK)
		plotArea->SetRightYLabel(dialog.GetText());
}

//==========================================================================
// Class:			PlotPanel
// Function:		SetMarkerSize
//
// Description:		Sets the marker size for the specified curve.
//
// Input Arguments:
//		curve	= const unsigned int&
//		size	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotPanel::SetMarkerSize(const unsigned int &curve, const int &size)
{
	optionsGrid->SetCellValue(curve + 1, colMarkerSize, wxString::Format("%i", size));
	UpdateCurveProperties(curve);
}

//==========================================================================
// Class:			PlotPanel
// Function:		DoCopy
//
// Description:		Handles "copy to clipboard" actions.
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
void PlotPanel::DoCopy(void)
{
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxBitmapDataObject(plotArea->GetImage()));
		wxTheClipboard->Close();
	}
}

//==========================================================================
// Class:			PlotPanel
// Function:		DoPaste
//
// Description:		Handles "paste from clipboard" actions.
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
/*void PlotPanel::DoPaste(void)
{
	if (wxTheClipboard->Open())
	{
		if (wxTheClipboard->IsSupported(wxDF_TEXT))
		{
			wxTextDataObject data;
			wxTheClipboard->GetData(data);
			LoadText(data.GetText());
		}
		wxTheClipboard->Close();
	}
}*/
