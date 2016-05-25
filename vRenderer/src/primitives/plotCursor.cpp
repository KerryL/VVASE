/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotCursor.cpp
// Created:  5/5/2011
// Author:  K. Loux
// Description:  Represents an oscilloscope cursor on-screen.
// History:
//  5/12/2011 - Renamed to PlotCursor from Cursor due to conflict in X.h, K. Loux

// Local headers
#include "vRenderer/primitives/plotCursor.h"
#include "vRenderer/primitives/axis.h"
#include "vRenderer/renderWindow.h"
#include "vRenderer/line.h"

//==========================================================================
// Class:			PlotCursor
// Function:		PlotCursor
//
// Description:		Constructor for the PlotCursor class.
//
// Input Arguments:
//		renderWindow	= RenderWindow&
//		axis			= Axis& with which we are associated
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PlotCursor::PlotCursor(RenderWindow &renderWindow, const Axis &axis)
	: Primitive(renderWindow), axis(axis)
{
	isVisible = false;
	color = Color::ColorBlack;
}

//==========================================================================
// Class:			PlotCursor
// Function:		Calculate
//
// Description:		Updates the value associated with the cursor.
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
void PlotCursor::Calculate()
{
	// Update the value of the cursor (required for accuracy when zoom changes, for example)
	value = axis.PixelToValue(locationAlongAxis);
}

//==========================================================================
// Class:			PlotCursor
// Function:		GenerateGeometry
//
// Description:		Generates OpenGL commands to draw the cursor.
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
void PlotCursor::GenerateGeometry()
{
	Line line;
	if (axis.IsHorizontal())
	{
		line.Draw(locationAlongAxis, axis.GetOffsetFromWindowEdge(),
			locationAlongAxis, renderWindow.GetSize().GetHeight()
			- axis.GetOppositeAxis()->GetOffsetFromWindowEdge());
	}
	else
	{
		line.Draw(axis.GetOffsetFromWindowEdge(), locationAlongAxis,
			renderWindow.GetSize().GetWidth()
			- axis.GetOppositeAxis()->GetOffsetFromWindowEdge(), locationAlongAxis);
	}

	Calculate();
}

//==========================================================================
// Class:			PlotCursor
// Function:		HasValidParameters
//
// Description:		Checks to see if we're OK to draw the cursor.
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
bool PlotCursor::HasValidParameters()
{
	// Make sure the value is within the axis limits
	if (value >= axis.GetMinimum() && value <= axis.GetMaximum() &&
		axis.GetAxisAtMaxEnd() && axis.GetAxisAtMinEnd())
		return true;

	// If the parameters aren't valid, also hide this to prevent the cursor values from updating
	isVisible = false;

	return false;
}

//==========================================================================
// Class:			PlotCursor
// Function:		IsUnder
//
// Description:		Checks to see if the cursor is located under the specified
//					point.
//
// Input Arguments:
//		pixel	= const unsigned int& describing the location of the point
//				  in screen coordinates
//
// Output Arguments:
//		None
//
// Return Value:
//		True is the cursor is under the specified point, false otherwise
//
//==========================================================================
bool PlotCursor::IsUnder(const unsigned int &pixel)
{
	// Apparent line width for clicking
	int width = 2;// [pixels]

	if (abs(int(locationAlongAxis - pixel)) <= width && isVisible)
		return true;

	return false;
}

//==========================================================================
// Class:			PlotCursor
// Function:		SetLocation
//
// Description:		Sets the x position where the cursor should appear on the plot.
//
// Input Arguments:
//		location	= const int& location to set
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotCursor::SetLocation(const int& location)
{
	locationAlongAxis = location;
	value = axis.PixelToValue(location);
	modified = true;
}

//==========================================================================
// Class:			PlotCursor
// Function:		operator=
//
// Description:		Sets the x-value where the cursor should appear on the plot.
//
// Input Arguments:
//		_value	= const double& value to set
//
// Output Arguments:
//		None
//
// Return Value:
//		PlotCursor& reference to this object
//
//==========================================================================
PlotCursor& PlotCursor::operator=(const PlotCursor &target)
{
	if (&target == this)
		return *this;

	value = target.value;
	locationAlongAxis = target.locationAlongAxis;

	return *this;
}
