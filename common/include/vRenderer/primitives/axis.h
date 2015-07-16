/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  axis.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive, this class is used to draw plot axis.
// History:

#ifndef AXIS_H_
#define AXIS_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "vRenderer/primitives/primitive.h"

// FTGL forward declarations
class FTFont;

class Axis : public Primitive
{
public:
	Axis(RenderWindow &renderWindow);
	~Axis();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();

	// Enumeration for the axis orientations
	enum AxisOrientation
	{
		OrientationBottom,
		OrientationTop,
		OrientationLeft,
		OrientationRight
	};

	// Enumeration for the tick styles
	enum TickStyle
	{
		TickStyleThrough,
		TickStyleInside,
		TickStyleOutside,
		TickStyleNone
	};

	// Set option methods
	void SetOrientation(const AxisOrientation &orientation) { this->orientation = orientation; modified = true; }
	void SetMinimum(const double &minimum) { this->minimum = minimum; modified = true; }
	void SetMaximum(const double &maximum) { this->maximum = maximum; modified = true; }
	void SetMajorResolution(const double &majorResolution) { this->majorResolution = majorResolution; modified = true; }
	void SetMinorResolution(const double &minorResolution) { this->minorResolution = minorResolution; modified = true; }
	void SetGrid(const bool &grid) { this->grid = grid; modified = true; }
	void SetLabel(wxString label) { this->label = label; modified = true; }
	void SetFont(FTFont *font) { this->font = font; modified = true; }
	void SetGridColor(const Color &gridColor) { this->gridColor = gridColor; modified = true; }
	void SetTickStyle(const TickStyle &tickStyle) { this->tickStyle = tickStyle; modified = true; }
	void SetTickSize(const int &tickSize) { this->tickSize = tickSize; modified = true; }
	void SetOffsetFromWindowEdge(const unsigned int &offset) { offsetFromWindowEdge = offset; modified = true; }
	
	void SetAxisAtMinEnd(const Axis *min) { minAxis = min; modified = true; }
	void SetAxisAtMaxEnd(const Axis *max) { maxAxis = max; modified = true; }
	void SetOppositeAxis(const Axis *opposite) { oppositeAxis = opposite; modified = true; }

	// Get option methods
	inline double GetMinimum() const { return minimum; }
	inline double GetMaximum() const { return maximum; }
	bool IsHorizontal() const;
	inline unsigned int GetOffsetFromWindowEdge() const { return offsetFromWindowEdge; }
	inline bool GetGrid() const { return grid; }
	inline Color GetGridColor() const { return gridColor; }
	
	inline const Axis* GetAxisAtMinEnd() const { return minAxis; }
	inline const Axis* GetAxisAtMaxEnd() const { return maxAxis; }
	inline const Axis* GetOppositeAxis() const { return oppositeAxis; }
	
	inline wxString GetLabel() const { return label; }

private:
	// This object's orientation
	AxisOrientation orientation;

	// This object's range
	double minimum;
	double maximum;

	// For the tick and grid spacing
	double majorResolution;
	double minorResolution;

	// The tick options
	TickStyle tickStyle;
	bool grid;
	int tickSize;

	Color gridColor;

	// Distance for edge of plot render window to the axis
	unsigned int offsetFromWindowEdge;// [pixels]
	
	// Pointers to the axes at either end of this axis
	const Axis *minAxis;
	const Axis *maxAxis;
	const Axis *oppositeAxis;

	// The axis label and font
	wxString label;
	FTFont *font;
};

#endif// AXIS_H_