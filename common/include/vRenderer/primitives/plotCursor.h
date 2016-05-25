/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotCursor.h
// Created:  5/5/2011
// Author:  K. Loux
// Description:  Represents an oscilloscope cursor on-screen.
// History:
//  5/12/2011 - Renamed to PlotCursor from Cursor due to conflict in X.h, K. Loux

#ifndef CURSOR_H_
#define CURSOR_H_

// Local headers
#include "vRenderer/primitives/primitive.h"

// Local forward declarations
class Axis;

class PlotCursor : public Primitive
{
public:
	PlotCursor(RenderWindow &renderWindow, const Axis &axis);

	void Calculate();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();

	void SetLocation(const int& location);
	double GetValue() const { return value; }

	bool IsUnder(const unsigned int &pixel);

	// Assignment operator (to avoid Warning C4512 due to const reference member)
	PlotCursor& operator=(const PlotCursor &target);

private:
	// The axis we are associated with (perpendicular to)
	const Axis &axis;

	// Current value where this object meets the axis
	double value;
	int locationAlongAxis;
};

#endif// CURSOR_H_