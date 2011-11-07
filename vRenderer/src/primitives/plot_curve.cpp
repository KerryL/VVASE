/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plot_curve.cpp
// Created:  5/23/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating data curves on a plot.
// History:
//	11/9/2010	- Modified to accomodate 3D plots, K. Loux.

// Local headers
#include "vRenderer/primitives/plot_curve.h"
#include "vRenderer/render_window_class.h"
#include "vRenderer/primitives/axis.h"

//==========================================================================
// Class:			PLOT_CURVE
// Function:		PLOT_CURVE
//
// Description:		Constructor for the PLOT_CURVE class.
//
// Input Arguments:
//		_RenderWindow	= RENDER_WINDOW* pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PLOT_CURVE::PLOT_CURVE(RENDER_WINDOW &_RenderWindow) : PRIMITIVE(_RenderWindow)
{
	XAxis = NULL;
	YAxis = NULL;
	ZAxis = NULL;
}

//==========================================================================
// Class:			PLOT_CURVE
// Function:		PLOT_CURVE
//
// Description:		Copy constructor for the PLOT_CURVE class.
//
// Input Arguments:
//		PlotCurve	= const PLOT_CURVE& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PLOT_CURVE::PLOT_CURVE(const PLOT_CURVE &PlotCurve) : PRIMITIVE(PlotCurve)
{
	// FIXME:  Need to copy the PlotData?
	// Do the copy
	*this = PlotCurve;
}

//==========================================================================
// Class:			PLOT_CURVE
// Function:		~PLOT_CURVE
//
// Description:		Destructor for the PLOT_CURVE class.
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
PLOT_CURVE::~PLOT_CURVE()
{
	// Delete the data in our plot list
	PlotData.Clear();
}

//==========================================================================
// Class:			PLOT_CURVE
// Function:		Constant Declarations
//
// Description:		Declare class constants for the PLOT_CURVE class.
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
const int PLOT_CURVE::OffsetFromWindowEdge = 75;

//==========================================================================
// Class:			PLOT_CURVE
// Function:		GenerateGeometry
//
// Description:		Creates the OpenGL instructions to create this object in
//					the scene.
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
void PLOT_CURVE::GenerateGeometry(void)
{
	// Set the line width
	glLineWidth(1.5f);

	// Create the plot
	glBegin(GL_LINE_STRIP);

	// Create the plot
	int i;
	int Point[3];
	XYZPOINT *XYZPoint;
	// FIXME:  If there are more points than pixels, we should avoid sending single pixel
	// line-draw commands to openGL
	for (i = 0; i < PlotData.GetCount(); i++)
	{
		XYZPoint = PlotData[i];

		// Clip data that lies outside of the plot range and interpolate to the edge of the plot
		if (YAxis)// 3D plot
		{
			// FIXME:  We need a better geometry generator for 3D plots (surfaces)
			if (XYZPoint->X >= XAxis->GetMinimum() && XYZPoint->X <= XAxis->GetMaximum() &&
				XYZPoint->Y >= YAxis->GetMinimum() && XYZPoint->Y <= YAxis->GetMaximum() &&
				XYZPoint->Z >= ZAxis->GetMinimum() && XYZPoint->Z <= ZAxis->GetMaximum())
			{
				RescalePoint(XYZPoint, Point);
				glVertex3iv(Point);
			}
		}
		else// 2D plot
		{
			// No interpolation necessary
			if (XYZPoint->X >= XAxis->GetMinimum() && XYZPoint->X <= XAxis->GetMaximum() &&
				XYZPoint->Z >= ZAxis->GetMinimum() && XYZPoint->Z <= ZAxis->GetMaximum())
			{
				RescalePoint(XYZPoint, Point);
				Point[1] = Point[2];
				glVertex2iv(Point);
			}
			else
			{
				// If the next point in the series (or the previous point in the series) is within
				// the valid drawing area, interpolate on one end, otherwise, interpolate on both ends
				if (i > 0)
				{
					if (PlotData[i - 1]->X >= XAxis->GetMinimum() && PlotData[i - 1]->X <= XAxis->GetMaximum() &&
						PlotData[i - 1]->Z >= ZAxis->GetMinimum() && PlotData[i - 1]->Z <= ZAxis->GetMaximum())
					{
						XYZPOINT InterpolatedPoint = *XYZPoint;

						// Interpolate to find the correct XYZPoint
						if (InterpolatedPoint.X < XAxis->GetMinimum())
						{
							InterpolatedPoint.X = XAxis->GetMinimum();
							InterpolatedPoint.Z = XYZPoint->Z +
								(InterpolatedPoint.X - XYZPoint->X) / (PlotData[i - 1]->X - XYZPoint->X) *
								(PlotData[i - 1]->Z - XYZPoint->Z);
						}
						else if (InterpolatedPoint.X > XAxis->GetMaximum())
						{
							InterpolatedPoint.X = XAxis->GetMaximum();
							InterpolatedPoint.Z = XYZPoint->Z +
								(InterpolatedPoint.X - XYZPoint->X) / (PlotData[i - 1]->X - XYZPoint->X) *
								(PlotData[i - 1]->Z - XYZPoint->Z);
						}

						if (InterpolatedPoint.Z < ZAxis->GetMinimum())
						{
							InterpolatedPoint.Z = ZAxis->GetMinimum();
							InterpolatedPoint.X = XYZPoint->X +
								(InterpolatedPoint.Z - XYZPoint->Z) / (PlotData[i - 1]->Z - XYZPoint->Z) *
								(PlotData[i - 1]->X - XYZPoint->X);
						}
						else if (InterpolatedPoint.Z > ZAxis->GetMaximum())
						{
							InterpolatedPoint.Z = ZAxis->GetMaximum();
							InterpolatedPoint.X = XYZPoint->X +
								(InterpolatedPoint.Z - XYZPoint->Z) / (PlotData[i - 1]->Z - XYZPoint->Z) *
								(PlotData[i - 1]->X - XYZPoint->X);
						}

						RescalePoint(&InterpolatedPoint, Point);
						Point[1] = Point[2];
						glVertex2iv(Point);
						continue;
					}
				}
				if (i < PlotData.GetCount() - 1)
				{
					if (PlotData[i + 1]->X >= XAxis->GetMinimum() && PlotData[i + 1]->X <= XAxis->GetMaximum() &&
						PlotData[i + 1]->Z >= ZAxis->GetMinimum() && PlotData[i + 1]->Z <= ZAxis->GetMaximum())
					{
						XYZPOINT InterpolatedPoint = *XYZPoint;

						// Interpolate to find the correct XYZPoint
						if (InterpolatedPoint.X < XAxis->GetMinimum())
						{
							InterpolatedPoint.X = XAxis->GetMinimum();
							InterpolatedPoint.Z = XYZPoint->Z +
								(InterpolatedPoint.X - XYZPoint->X) / (PlotData[i + 1]->X - XYZPoint->X) *
								(PlotData[i + 1]->Z - XYZPoint->Z);
						}
						else if (InterpolatedPoint.X > XAxis->GetMaximum())
						{
							InterpolatedPoint.X = XAxis->GetMaximum();
							InterpolatedPoint.Z = XYZPoint->Z +
								(InterpolatedPoint.X - XYZPoint->X) / (PlotData[i + 1]->X - XYZPoint->X) *
								(PlotData[i + 1]->Z - XYZPoint->Z);
						}

						if (InterpolatedPoint.Z < ZAxis->GetMinimum())
						{
							InterpolatedPoint.Z = ZAxis->GetMinimum();
							InterpolatedPoint.X = XYZPoint->X +
								(InterpolatedPoint.Z - XYZPoint->Z) / (PlotData[i + 1]->Z - XYZPoint->Z) *
								(PlotData[i + 1]->X - XYZPoint->X);
						}
						else if (InterpolatedPoint.Z > ZAxis->GetMaximum())
						{
							InterpolatedPoint.Z = ZAxis->GetMaximum();
							InterpolatedPoint.X = XYZPoint->X +
								(InterpolatedPoint.Z - XYZPoint->Z) / (PlotData[i + 1]->Z - XYZPoint->Z) *
								(PlotData[i + 1]->X - XYZPoint->X);
						}

						RescalePoint(&InterpolatedPoint, Point);
						Point[1] = Point[2];
						glVertex2iv(Point);
					}
				}
				// FIXME:  If the user zooms in too much, the plot disappears - there must be some way to
				// interpolate between just two points to draw a representative line
			}
		}
	}

	glEnd();

	return;
}

//==========================================================================
// Class:			PLOT_CURVE
// Function:		RescalePoint
//
// Description:		Rescales the onscreen position of the point according to
//					the size of the axis with which this object is associated.
//
// Input Arguments:
//		XYZPoint	= const XYZPOINT containing the location of the point in plot
//					  coordinates
//
// Output Arguments:
//		Point	= int* specifying the location of the object in screen coordinates
//
// Return Value:
//		None
//
//==========================================================================
void PLOT_CURVE::RescalePoint(const XYZPOINT *XYZPoint, int *Point)
{
	if (!XYZPoint || !Point)
		return;

	// Get the plot size
	int PlotHeight = RenderWindow.GetSize().GetHeight() - 2 * OffsetFromWindowEdge;
	int PlotDepth = RenderWindow.GetSize().GetWidth() - 2 * OffsetFromWindowEdge;// FIXME!!!
	int PlotWidth = RenderWindow.GetSize().GetWidth() - 2 * OffsetFromWindowEdge;

	// Do the scaling
	Point[0] = OffsetFromWindowEdge + (XYZPoint->X - XAxis->GetMinimum()) /
		(XAxis->GetMaximum() - XAxis->GetMinimum()) * PlotWidth;
	if (YAxis)
		Point[1] = OffsetFromWindowEdge + (XYZPoint->Y - YAxis->GetMinimum()) /
		(YAxis->GetMaximum() - YAxis->GetMinimum()) * PlotDepth;
	Point[2] = OffsetFromWindowEdge + (XYZPoint->Z - ZAxis->GetMinimum()) /
		(ZAxis->GetMaximum() - ZAxis->GetMinimum()) * PlotHeight;

	return;
}

//==========================================================================
// Class:			PLOT_CURVE
// Function:		AddPoint
//
// Description:		Adds a point to the curve.
//
// Input Arguments:
//		X	= const double&
//		Z	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PLOT_CURVE::AddPoint(const double &X, const double &Z)
{
	XYZPOINT *Point = new XYZPOINT;
	Point->X = X;
	Point->Y = 0.0;
	Point->Z = Z;

	PlotData.Add(Point);

	Modified = true;

	return;
}

//==========================================================================
// Class:			PLOT_CURVE
// Function:		AddPoint
//
// Description:		Adds a point to the curve.
//
// Input Arguments:
//		X	= const double&
//		Y	= const double&
//		Z	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PLOT_CURVE::AddPoint(const double &X, const double &Y, const double &Z)
{
	XYZPOINT *Point = new XYZPOINT;
	Point->X = X;
	Point->Y = Y;
	Point->Z = Z;

	PlotData.Add(Point);

	Modified = true;

	return;
}

//==========================================================================
// Class:			PLOT_CURVE
// Function:		HasValidParameters
//
// Description:		Checks to see if the information about this object is
//					valid and complete (gives permission to create the object).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//==========================================================================
bool PLOT_CURVE::HasValidParameters(void)
{
	if (XAxis != NULL && ZAxis != NULL && PlotData.GetCount() > 1)
	{
		if (XAxis->IsHorizontal() && !ZAxis->IsHorizontal())
			return true;
	}
	// FIXME!!! Add checks for 3D plots

	return false;
}

//==========================================================================
// Class:			PLOT_CURVE
// Function:		operator =
//
// Description:		Assignment operator for PLOT_CURVE class.
//
// Input Arguments:
//		PlotCurve	= const PLOT_CURVE& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		PLOT_CURVE&, reference to this object
//
//==========================================================================
PLOT_CURVE& PLOT_CURVE::operator = (const PLOT_CURVE &PlotCurve)
{
	// Check for self-assignment
	if (this == &PlotCurve)
		return *this;

	// Copy the important information using the base class's assignment operator
	this->PRIMITIVE::operator=(PlotCurve);

	// FIMXE:  Need to handle copy of PlotData?

	return *this;
}