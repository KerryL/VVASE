/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  axis.cpp
// Created:  5/23/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating axis objects on a plot.
// History:
//	11/17/2010	- Fixed some bugs related to rendering of ticks and grid lines, K. Loux.

// Local headers
#include "vRenderer/primitives/axis.h"
#include "vRenderer/render_window_class.h"
#include "vMath/car_math.h"

// FTGL headers
#include <FTGL/ftgl.h>

//==========================================================================
// Class:			AXIS
// Function:		AXIS
//
// Description:		Constructor for the AXIS class.
//
// Input Argurments:
//		_RenderWindow	= RENDER_WINDOW& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
AXIS::AXIS(RENDER_WINDOW &_RenderWindow) : PRIMITIVE(_RenderWindow)
{
	Color.Set(0.0, 0.0, 0.0, 1.0);

	Orientation = OrientationBottom;
	TickStyle = TickStyleThrough;

	TickSize = 7;

	Minimum = 0.0;
	Maximum = 1.0;
	MajorResolution = 1.0;
	MinorResolution = 1.0;

	Grid = false;

	Font = NULL;

	GridColor.Set(0.8, 0.8, 0.8, 1.0);
}

//==========================================================================
// Class:			AXIS
// Function:		~AXIS
//
// Description:		Destructor for the AXIS class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
AXIS::~AXIS()
{
}

//==========================================================================
// Class:			AXIS
// Function:		GenerateGeometry
//
// Description:		Creates the OpenGL instructions to create this object in
//					the scene.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void AXIS::GenerateGeometry(void)
{
	// Use glLineWidth() and store it in member variable?

	// Preliminary calculations
	int AxisLength, Tick;
	double OutsideTick = 0.0, InsideTick = 0.0;
	double TickSpacing;// To avoid rounding errors, we convert this back to an int after we do some math
	double GridSpacing;
	double Sign = 1.0;
	int OffsetFromWindowEdge = 75;
	int NumberOfTicks = int((Maximum - Minimum) / MinorResolution + 0.5) - 1;
	int NumberOfGridLines = int((Maximum - Minimum) / MajorResolution + 0.5) - 1;
	int MainAxisLocation;

	// Determine where the ticks should be drawns
	if (TickStyle == TickStyleInside)
		InsideTick = 1.0;
	else if (TickStyle == TickStyleOutside)
		OutsideTick = 1.0;
	else if (TickStyle == TickStyleThrough)
	{
		InsideTick = 0.5;
		OutsideTick = 0.5;
	}

	// Set the sign variable as necessary
	if (Orientation == OrientationTop || Orientation == OrientationRight)
		Sign = -1.0;

	// Compute the MainAxisLocation (X for vertical axis, Y for horizontal axis)
	if (Orientation == OrientationBottom || Orientation == OrientationLeft)
		MainAxisLocation = OffsetFromWindowEdge;
	else if (Orientation == OrientationRight)
		MainAxisLocation = RenderWindow.GetSize().GetWidth() - OffsetFromWindowEdge;
	else// OrientationTop
		MainAxisLocation = RenderWindow.GetSize().GetHeight() - OffsetFromWindowEdge;

	// Set the line width
	glLineWidth(1.5f);

	// Create the axis
	glBegin(GL_LINES);

	// Draw the axis
	if (IsHorizontal())
	{
		AxisLength = RenderWindow.GetSize().GetWidth() - 2 * OffsetFromWindowEdge;
		glVertex2i(OffsetFromWindowEdge, MainAxisLocation);
		glVertex2i(AxisLength + OffsetFromWindowEdge, MainAxisLocation);
	}
	else
	{
		AxisLength = RenderWindow.GetSize().GetHeight() - 2 * OffsetFromWindowEdge;
		glVertex2i(MainAxisLocation, OffsetFromWindowEdge);
		glVertex2i(MainAxisLocation, AxisLength + OffsetFromWindowEdge);
	}

	// Compute the spacing for grids and ticks
	GridSpacing = (double)AxisLength / double(NumberOfGridLines + 1);
	TickSpacing = (double)AxisLength / double(NumberOfTicks + 1);

	// Draw the grids and tick marks
	if (IsHorizontal())
	{
		// Draw the grid
		if (Grid)
		{
			glColor4d(GridColor.GetRed(), GridColor.GetGreen(), GridColor.GetBlue(), GridColor.GetAlpha());

			for (Tick = 0; Tick < NumberOfGridLines; Tick++)
			{
				glVertex2i(OffsetFromWindowEdge + (Tick + 1) * GridSpacing, OffsetFromWindowEdge);
				glVertex2i(OffsetFromWindowEdge + (Tick + 1) * GridSpacing,
					RenderWindow.GetSize().GetHeight() - OffsetFromWindowEdge);
			}

			glColor4d(Color.GetRed(), Color.GetGreen(), Color.GetBlue(), Color.GetAlpha());
		}

		// Draw the tick marks (if they're turned on)
		if (TickStyle != TickStyleNone)
		{
			// The first and last inside ticks do not need to be drawn, thus we start this loop with Tick = 1.
			for (Tick = 1; Tick <= NumberOfTicks; Tick++)
			{
				glVertex2i(OffsetFromWindowEdge + Tick * TickSpacing, MainAxisLocation - TickSize * OutsideTick * Sign);
				glVertex2i(OffsetFromWindowEdge + Tick * TickSpacing, MainAxisLocation + TickSize * InsideTick * Sign);
			}
		}
	}
	else
	{
		// Draw the grid
		if (Grid)
		{
			glColor4d(GridColor.GetRed(), GridColor.GetGreen(), GridColor.GetBlue(), GridColor.GetAlpha());

			for (Tick = 0; Tick < NumberOfGridLines; Tick++)
			{
				glVertex2i(OffsetFromWindowEdge, OffsetFromWindowEdge + (Tick + 1) * GridSpacing);
				glVertex2i(RenderWindow.GetSize().GetWidth() - OffsetFromWindowEdge,
					OffsetFromWindowEdge + (Tick + 1) * GridSpacing);
			}

			glColor4d(Color.GetRed(), Color.GetGreen(), Color.GetBlue(), Color.GetAlpha());
		}

		// Draw the tick marks (if they're turned on)
		if (TickStyle != TickStyleNone)
		{
			for (Tick = 1; Tick <= NumberOfTicks; Tick++)
			{
				glVertex2i(MainAxisLocation - TickSize * OutsideTick * Sign, OffsetFromWindowEdge + Tick * TickSpacing);
				glVertex2i(MainAxisLocation + TickSize * InsideTick * Sign, OffsetFromWindowEdge + Tick * TickSpacing);
			}
		}
	}

	// Complete the axis
	glEnd();

	// Add the text
	if (Font)
	{
		FTBBox BoundingBox;
		double YTranslation, XTranslation;

		// Add the label text
		if (!Label.IsEmpty())
		{
			double FontOffsetFromWindowEdge = OffsetFromWindowEdge / 3.0;

			// Vertical axis need more space for the numbers
			if (!IsHorizontal())
				FontOffsetFromWindowEdge /= 2.0;

			// FIXME:  Change plot dimension if there is a title? or if there is a title and a label for the top axis?

			// Get the bounding box
			BoundingBox = Font->BBox("H");// Some capital letter to assure uniform spacing

			switch (Orientation)
			{
			case OrientationBottom:
				YTranslation = FontOffsetFromWindowEdge;
				break;

			case OrientationLeft:
				YTranslation = FontOffsetFromWindowEdge + BoundingBox.Upper().Y();
				break;

			case OrientationTop:
				YTranslation = RenderWindow.GetSize().GetHeight() - FontOffsetFromWindowEdge - BoundingBox.Upper().Y();
				break;

			case OrientationRight:
				YTranslation = RenderWindow.GetSize().GetWidth() - FontOffsetFromWindowEdge;
				break;

			default:
				assert(0);
				YTranslation = 0.0;// To avoid MSVC++ compiler warning C4701
				break;
			}

			glPushMatrix();
				glLoadIdentity();

				// Get the real bounding box
				BoundingBox = Font->BBox(Label.c_str());

				if (IsHorizontal())
					glTranslated((RenderWindow.GetSize().GetWidth() + BoundingBox.Lower().X()
						- BoundingBox.Upper().X()) / 2.0, YTranslation, 0.0);
				else
				{
					glRotated(90.0, 0.0, 0.0, 1.0);
					glTranslated((BoundingBox.Lower().X() - BoundingBox.Upper().X()
						+ RenderWindow.GetSize().GetHeight()) / 2.0, -YTranslation, 0.0);
				}

				Font->Render(Label.c_str());
			glPopMatrix();
		}

		// Determine the precision to use for displaying the numbers
		// Use just enough precision so that two adjacent ticks are distinguishable
		unsigned int Precision = 2;
		if (log10(MajorResolution) >= 0.0)
			Precision = 0;
		else
			Precision = -log10(MajorResolution) + 1;

		// Add the number values text
		double TextValue;
		wxString ValueLabel;
		double ValueOffsetFromEdge = /*0.0;//*/OffsetFromWindowEdge * 0.8;
		for (Tick = 0; Tick < NumberOfGridLines + 2; Tick++)
		{
			// Determine the label value
			TextValue = Minimum + (double)Tick * MajorResolution;

			// Assign the value to the string
			ValueLabel.Printf("%0.*f", Precision, TextValue);

			glPushMatrix();
				glLoadIdentity();

				// Get the real bounding box
				BoundingBox = Font->BBox(ValueLabel);

				if (IsHorizontal())
				{
					if (Orientation == OrientationBottom)
						YTranslation = ValueOffsetFromEdge - BoundingBox.Upper().Y();
					else
						YTranslation = RenderWindow.GetSize().GetHeight() - ValueOffsetFromEdge;

					XTranslation = OffsetFromWindowEdge + Tick * GridSpacing -
						(BoundingBox.Upper().X() - BoundingBox.Lower().X()) / 2.0;
				}
				else
				{
					if (Orientation == OrientationLeft)
						XTranslation = ValueOffsetFromEdge - BoundingBox.Upper().X();
					else
						XTranslation = RenderWindow.GetSize().GetWidth() - ValueOffsetFromEdge;

					YTranslation = OffsetFromWindowEdge + Tick * GridSpacing -
						(BoundingBox.Upper().Y() - BoundingBox.Lower().Y()) / 2.0;
				}

				glTranslated(XTranslation, YTranslation, 0.0);

				Font->Render(ValueLabel);
			glPopMatrix();
		}
	}

	return;
}

//==========================================================================
// Class:			AXIS
// Function:		IsHorizontal
//
// Description:		Checks to see if this object has horizontal orientation.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for horizontal, false for vertical
//
//==========================================================================
bool AXIS::IsHorizontal(void) const
{
	if (Orientation == OrientationBottom || Orientation == OrientationTop)
		return true;

	return false;
}

//==========================================================================
// Class:			AXIS
// Function:		HasValidParameters
//
// Description:		Checks to see if the information about this object is
//					valid and complete (gives permission to create the object).
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//==========================================================================
bool AXIS::HasValidParameters(void)
{
	// Don't draw if any of the limits are not numbers
	if (VVASEMath::IsNaN(Minimum) || VVASEMath::IsNaN(Maximum))
		return false;

	return true;
}