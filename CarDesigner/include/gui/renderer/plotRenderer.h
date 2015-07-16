/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotRenderer.h
// Created:  5/4/2011
// Author:  K. Loux
// Description:  Derived from RenderWindow, this class is used to display plots on
//				 the screen.
// History:

#ifndef PLOT_RENDERER_H_
#define PLOT_RENDERER_H_

// Local headers
#include "vRenderer/renderWindow.h"

// wxWidgets forward declarations
class wxString;

// Local forward declarations
class Dataset2D;
class ZoomBox;
class PlotCursor;
class PlotObject;
class PlotPanel;
class MainFrame;

class PlotRenderer : public RenderWindow
{
public:
	PlotRenderer(PlotPanel &parent, wxWindowID id, int args[], MainFrame &mainFrame);
	~PlotRenderer();

	// Gets properties for actors
	bool GetBottomGrid() const;
	bool GetLeftGrid() const;
	bool GetRightGrid() const;

	Color GetGridColor() const;

	double GetXMin() const;
	double GetXMax() const;
	double GetLeftYMin() const;
	double GetLeftYMax() const;
	double GetRightYMin() const;
	double GetRightYMax() const;

	// Sets properties for actors
	void SetGridOn(const bool &grid = true);
	void SetGridOff();
	void SetBottomGrid(const bool &grid);
	void SetLeftGrid(const bool &grid);
	void SetRightGrid(const bool &grid);

	void SetGridColor(const Color &color);

	void SetCurveProperties(const unsigned int &index, const Color &color,
		const bool &visible, const bool &rightAxis, const unsigned int &size);
	void SetXLimits(const double &min, const double &max);
	void SetLeftYLimits(const double &min, const double &max);
	void SetRightYLimits(const double &min, const double &max);

	void SetXLabel(wxString text);
	void SetLeftYLabel(wxString text);
	void SetRightYLabel(wxString text);
	void SetTitle(wxString text);

	void AddCurve(const Dataset2D &data);
	void RemoveAllCurves();
	void RemoveCurve(const unsigned int &index);

	void AutoScale();
	void AutoScaleBottom();
	void AutoScaleLeft();
	void AutoScaleRight();

	bool GetGridOn();

	// Called to update the screen
	void UpdateDisplay();

	bool GetLeftCursorVisible() const;
	bool GetRightCursorVisible() const;
	double GetLeftCursorValue() const;
	double GetRightCursorValue() const;

	void UpdateCursors();

	MainFrame &GetMainFrame() { return mainFrame; };

private:
	MainFrame &mainFrame;

	// Called from the PlotRenderer constructor only in order to initialize the display
	void CreateActors();

	// The parent panel
	PlotPanel &parent;

	// The actors necessary to create the plot
	PlotObject *plot;

	// Overload of size event
	void OnSize(wxSizeEvent &event);

	// Overload of interaction events
	void OnMouseWheelEvent(wxMouseEvent &event);
	void OnMouseMoveEvent(wxMouseEvent &event);
	void OnRightButtonUpEvent(wxMouseEvent &event);
	void OnLeftButtonUpEvent(wxMouseEvent &event);
	void OnLeftButtonDownEvent(wxMouseEvent &event);

	void OnMouseLeaveWindowEvent(wxMouseEvent &event);
	void OnDoubleClickEvent(wxMouseEvent &event);

	ZoomBox *zoomBox;
	PlotCursor *leftCursor;
	PlotCursor *rightCursor;

	bool draggingLeftCursor;
	bool draggingRightCursor;

	double GetCursorValue(const unsigned int &location);

protected:
	DECLARE_EVENT_TABLE()
};

#endif// PLOT_RENDERER_H_