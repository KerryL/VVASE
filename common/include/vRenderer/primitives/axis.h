/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  axis.h
// Created:  5/23/2009
// Author:  K. Loux
// Description:  Derived from PRIMITVE, this class is used to draw plot axis.
// History:

#ifndef _AXIS_H_
#define _AXIS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vRenderer/primitives/primitive.h"

// FTGL forward declarations
class FTFont;

class AXIS : public PRIMITIVE
{
public:
	// Constructor
	AXIS(RENDER_WINDOW &_RenderWindow);

	// Destructor
	~AXIS();

	// Mandatory overloads from PRIMITIVE - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Enumeration for the axis orientations
	enum AXIS_ORIENTATION
	{
		OrientationBottom,
		OrientationTop,
		OrientationLeft,
		OrientationRight
	};

	// Enumeration for the tick styles
	enum TICK_STYLE
	{
		TickStyleThrough,
		TickStyleInside,
		TickStyleOutside,
		TickStyleNone
	};

	// Set option methods
	void SetOrientation(AXIS_ORIENTATION _Orientation) { Orientation = _Orientation; Modified = true; };
	void SetMinimum(double _Minimum) { Minimum = _Minimum; Modified = true; };
	void SetMaximum(double _Maximum) { Maximum = _Maximum; Modified = true; };
	void SetMajorResolution(double _MajorResolution) { MajorResolution = _MajorResolution; Modified = true; };
	void SetMinorResolution(double _MinorResolution) { MinorResolution = _MinorResolution; Modified = true; };
	void SetGrid(bool _Grid) { Grid = _Grid; Modified = true; };
	void SetLabel(wxString _Label) { Label = _Label; Modified = true; };
	void SetFont(FTFont *_Font) { Font = _Font; Modified = true; };
	void SetGridColor(COLOR _GridColor) { GridColor = _GridColor; Modified = true; };
	void SetTickStyle(TICK_STYLE _TickStyle) { TickStyle = _TickStyle; Modified = true; };
	void SetTickSize(int _TickSize) { TickSize = _TickSize; Modified = true; };

	// Get option methods
	double GetMinimum(void) const { return Minimum; };
	double GetMaximum(void) const { return Maximum; };
	bool IsHorizontal(void) const;

private:
	// This object's orientation
	AXIS_ORIENTATION Orientation;

	// This object's range
	double Minimum;
	double Maximum;

	// For the tick and grid spacing
	double MajorResolution;
	double MinorResolution;

	// The tick options
	TICK_STYLE TickStyle;
	bool Grid;
	int TickSize;

	// Color of the grid
	COLOR GridColor;

	// The axis label and font
	wxString Label;
	FTFont *Font;
};

#endif// _AXIS_H_