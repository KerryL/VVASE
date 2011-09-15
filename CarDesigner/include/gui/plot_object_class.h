/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plot_object_class.h
// Created:  1/22/2009
// Author:  K. Loux
// Description:  Intermediate class for creating plots from arrays of data.
// History:
//	5/23/2009	- Re-wrote to remove VTK dependencies, K. Loux.
//	11/9/2010	- Renamed from PLOT2D, K. Loux.

#ifndef _PLOT_OBJECT_CLASS_H_
#define _PLOT_OBJECT_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vUtilities/object_list_class.h"

// VVASE forward declarations
class PLOT_RENDERER;
class AXIS;
class TEXT_RENDERING;
class ITERATION;
class PLOT_CURVE;
class DEBUGGER;

// FTGL forward declarations
class FTFont;

class PLOT_OBJECT
{
public:
	// Constructor
	PLOT_OBJECT(PLOT_RENDERER &_Renderer, const ITERATION &_DataSource, const DEBUGGER &_Debugger);

	// Destructor
	~PLOT_OBJECT();

	// Updates the actor on the screen
	void Update(void);

	// Removes all of the current plot data
	void RemoveExistingPlots(void);

	// Accessors for the axes max
	inline double GetXMin(void) const { return XMin; };
	inline double GetXMax(void) const { return XMax; };
	inline double GetYMin(void) const { return YMin; };
	inline double GetYMax(void) const { return YMax; };
	inline double GetZMin(void) const { return ZMin; };
	inline double GetZMax(void) const { return ZMax; };

	inline double GetXMinOriginal(void) const { return XMinOriginal; };
	inline double GetXMaxOriginal(void) const { return XMaxOriginal; };
	inline double GetYMinOriginal(void) const { return YMinOriginal; };
	inline double GetYMaxOriginal(void) const { return YMaxOriginal; };
	inline double GetZMinOriginal(void) const { return ZMinOriginal; };
	inline double GetZMaxOriginal(void) const { return ZMaxOriginal; };

	void SetXMin(const double &_XMin);
	void SetXMax(const double &_XMax);
	void SetYMin(const double &_YMin);
	void SetYMax(const double &_YMax);
	void SetZMin(const double &_ZMin);
	void SetZMax(const double &_ZMax);

	void ResetAutoScaling(void);

	// FIXME:  Associate with left/right axis or top/bottom axis (multiple scales for each axis)

private:
	// Debugger printing utility
	const DEBUGGER &Debugger;

	// The associated iteration
	const ITERATION &DataSource;

	// The renderer object
	PLOT_RENDERER &Renderer;

	// The actors (the non-plot actors that are always present)
	AXIS *AxisTop;
	AXIS *AxisBottom;
	AXIS *AxisLeft;
	AXIS *AxisRight;

	TEXT_RENDERING *TitleObject;
	// FIXME:  Add legend

	// The font objects
	FTFont *AxisFont;
	FTFont *TitleFont;

	// The Z-axis label (including units)
	wxString ZLabel;

	// The minimums and maximums for the axis
	double XMin, XMax, YMin, YMax, ZMin, ZMax;
	double XMinOriginal, XMaxOriginal, YMinOriginal, YMaxOriginal, ZMinOriginal, ZMaxOriginal;

	// Flag to indicate how the limits are set
	bool AutoScaleX;
	bool AutoScaleY;
	bool AutoScaleZ;

	// The actual plot objects
	OBJECT_LIST<PLOT_CURVE> PlotList;

	// Handles all of the formatting for the plot
	void FormatPlot(void);

	// Handles the spacing of the axis ticks
	double AutoScaleAxis(double &Min, double &Max, int MaxTicks, const bool &ForceLimits = false);
};

#endif// _PLOT_OBJECT_CLASS_H_