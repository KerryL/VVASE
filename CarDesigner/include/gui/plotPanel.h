/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotPanel.h
// Created:  11/9/2011
// Author:  K. Loux
// Description:  Panel containing plot renderer and grid listing active plot channels.
// History:

#ifndef _PLOT_PANEL_H_
#define _PLOT_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "vUtilities/managedList.h"
#include "vMath/dataset2D.h"

// wxWidgets headers
class wxGrid;
class wxGridEvent;

// Local forward declarations
class PlotRenderer;

class PlotPanel : public wxPanel
{
public:
	// Constructor
	PlotPanel(wxWindow *parent);

	// Destructor
	virtual ~PlotPanel();

	enum PlotContext
	{
		plotContextXAxis,
		plotContextLeftYAxis,
		plotContextRightYAxis,
		plotContextPlotArea
	};

	void CreatePlotContextMenu(const wxPoint &position, const PlotContext &context);
	void DisplayAxisRangeDialog(const PlotContext &axis);

	void UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue);
	void UpdateDisplay(void);

	void ClearAllCurves(void);
	void AddCurve(wxString mathString);
	void AddCurve(Dataset2D *data, wxString name);
	void RemoveCurve(const unsigned int &i);

	void SetXAxisGridText(wxString text);
	
	PlotRenderer* GetRenderer(void) { return renderer; };

	inline unsigned int GetCurveCount(void) { return plotList.GetCount(); };
	
	bool WriteImageToFile(wxString pathAndFileName);

private:
	// Main panel object
	PlotRenderer *renderer;

	// Grid containing all of the plot information
	wxGrid *optionsGrid;

	wxBoxSizer *topSizer;

	void CreateControls(void);

	enum Columns
	{
		colName = 0,
		colColor,
		colSize,
		colLeftCursor,
		colRightCursor,
		colDifference,
		colVisible,
		colRightAxis,

		colCount
	};

	// For displaying a menu that was created by this form
	// NOTE:  When calculating the Position to display this context menu,
	// consider that the coordinates for the calling object might be different
	// from the coordinates for this object!
	void CreateGridContextMenu(const wxPoint &position, const unsigned int &row);

	ManagedList<const Dataset2D> plotList;

	// The event IDs
	enum MainFrameEventID
	{
		idContextAddMathChannel = wxID_HIGHEST + 1000,
		idContextPlotDerivative,
		idContextPlotIntegral,
		idContextPlotRMS,
		idContextPlotFFT,
		idContextRemoveCurve,

		idContextFilterLowPass,
		idContextFilterHighPass,

		idContextFitCurve,

		idPlotContextToggleGridlines,
		idPlotContextAutoScale,
		idPlotContextWriteImageFile,

		idPlotContextBGColor,
		idPlotContextGridColor,

		idPlotContextToggleBottomGridlines,
		idPlotContextSetBottomRange,
		idPlotContextAutoScaleBottom,

		/*idPlotContextToggleTopGridlines,
		idPlotContextSetTopRange,
		idPlotContextAutoScaleTop,*/

		idPlotContextToggleLeftGridlines,
		idPlotContextSetLeftRange,
		idPlotContextAutoScaleLeft,

		idPlotContextToggleRightGridlines,
		idPlotContextSetRightRange,
		idPlotContextAutoScaleRight
	};

	// Event handlers-----------------------------------------------------
	// Grid events
	void GridRightClickEvent(wxGridEvent &event);
	void GridDoubleClickEvent(wxGridEvent &event);
	void GridLeftClickEvent(wxGridEvent &event);
	void GridCellChangeEvent(wxGridEvent &event);

	// Context menu events
	void ContextAddMathChannelEvent(wxCommandEvent &event);
	void ContextPlotDerivativeEvent(wxCommandEvent &event);
	void ContextPlotIntegralEvent(wxCommandEvent &event);
	void ContextPlotRMSEvent(wxCommandEvent &event);
	void ContextPlotFFTEvent(wxCommandEvent &event);
	void ContextRemoveCurveEvent(wxCommandEvent &event);

	void ContextFilterLowPassEvent(wxCommandEvent &event);
	void ContextFilterHighPassEvent(wxCommandEvent &event);

	void ContextFitCurve(wxCommandEvent &event);

	void ContextToggleGridlines(wxCommandEvent &event);
	void ContextAutoScale(wxCommandEvent &event);
	void ContextWriteImageFile(wxCommandEvent &event);

	void ContextPlotBGColor(wxCommandEvent &event);
	void ContextGridColor(wxCommandEvent &event);

	void ContextToggleGridlinesBottom(wxCommandEvent &event);
	void ContextAutoScaleBottom(wxCommandEvent &event);
	void ContextSetRangeBottom(wxCommandEvent &event);

	void ContextToggleGridlinesLeft(wxCommandEvent &event);
	void ContextAutoScaleLeft(wxCommandEvent &event);
	void ContextSetRangeLeft(wxCommandEvent &event);

	void ContextToggleGridlinesRight(wxCommandEvent &event);
	void ContextAutoScaleRight(wxCommandEvent &event);
	void ContextSetRangeRight(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	void DisplayMathChannelDialog(wxString defaultInput = wxEmptyString);

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _PLOT_PANEL_H_