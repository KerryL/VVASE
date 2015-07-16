/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotObject.h
// Created:  5/4/2011
// Author:  K. Loux
// Description:  Intermediate class for creating plots from arrays of data.
// History:

#ifndef PLOT_OBJECT_H_
#define PLOT_OBJECT_H_

// wxWidgets headers
#include <wx/wx.h>

// Standard C++ headers
#include <vector>

// Local forward declarations
class PlotRenderer;
class Axis;
class TextRendering;
class PlotCurve;
class Dataset2D;
class Color;

// FTGL forward declarations
class FTFont;

class PlotObject
{
public:
	PlotObject(PlotRenderer &renderer);
	~PlotObject();

	void Update();

	void RemoveExistingPlots();
	void RemovePlot(const unsigned int &index);
	void AddCurve(const Dataset2D &data);

	// Accessors for the axes limits
	inline double GetXMin() const { return xMin; }
	inline double GetXMax() const { return xMax; }
	inline double GetLeftYMin() const { return yLeftMin; }
	inline double GetLeftYMax() const { return yLeftMax; }
	inline double GetRightYMin() const { return yRightMin; }
	inline double GetRightYMax() const { return yRightMax; }

	inline double GetXMinOriginal() const { return xMinOriginal; }
	inline double GetXMaxOriginal() const { return xMaxOriginal; }
	inline double GetLeftYMinOriginal() const { return yLeftMinOriginal; }
	inline double GetLeftYMaxOriginal() const { return yLeftMaxOriginal; }
	inline double GetRightYMinOriginal() const { return yRightMinOriginal; }
	inline double GetRightYMaxOriginal() const { return yRightMaxOriginal; }

	void SetXMin(const double &xMin);
	void SetXMax(const double &xMax);
	void SetLeftYMin(const double &yMin);
	void SetLeftYMax(const double &yMax);
	void SetRightYMin(const double &yMin);
	void SetRightYMax(const double &yMax);

	void SetCurveProperties(const unsigned int &index, const Color &color,
		const bool &visible, const bool &rightAxis, const unsigned int &size);
	void SetGrid(const bool &gridOn);
	void SetXGrid(const bool &gridOn);
	void SetLeftYGrid(const bool &gridOn);
	void SetRightYGrid(const bool &gridOn);

	void SetXLabel(wxString text);
	void SetLeftYLabel(wxString text);
	void SetRightYLabel(wxString text);
	void SetTitle(wxString text);

	void SetGridColor(const Color &color);
	Color GetGridColor() const;

	void ResetAutoScaling();
	void SetAutoScaleBottom() { autoScaleX = true; }
	void SetAutoScaleLeft() { autoScaleLeftY = true; }
	void SetAutoScaleRight() { autoScaleRightY = true; }

	const Axis* GetBottomAxis() const { return axisBottom; }
	const Axis* GetTopAxis() const { return axisTop; }
	const Axis* GetLeftYAxis() const { return axisLeft; }
	const Axis* GetRightYAxis() const { return axisRight; }
	bool GetGrid();

private:
	PlotRenderer &renderer;

	// The actors (the non-plot actors that are always present)
	Axis *axisTop;
	Axis *axisBottom;
	Axis *axisLeft;
	Axis *axisRight;

	TextRendering *titleObject;

	FTFont *axisFont;
	FTFont *titleFont;

	// The minimums and maximums for the axis
	double xMin, xMax, yLeftMin, yLeftMax, yRightMin, yRightMax;
	double xMinOriginal, xMaxOriginal, yLeftMinOriginal, yLeftMaxOriginal,
		yRightMinOriginal, yRightMaxOriginal;

	bool autoScaleX;
	bool autoScaleLeftY;
	bool autoScaleRightY;

	// The actual plot objects
	std::vector<PlotCurve*> plotList;
	std::vector<const Dataset2D*> dataList;

	void FormatPlot();

	// Handles the spacing of the axis ticks
	double AutoScaleAxis(double &min, double &max, int maxTicks,
		const bool &forceLimits = false);
};

#endif// PLOT_OBJECT_H_