/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  text_class.cpp
// Created:  11/17/2010
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating text objects on a plot.
// History:

// Local headers
#include "vRenderer/primitives/text_class.h"
#include "vRenderer/render_window_class.h"
#include "vMath/car_math.h"

// FTGL headers
#include <FTGL/ftgl.h>

//==========================================================================
// Class:			TEXT_RENDERING
// Function:		TEXT_RENDERING
//
// Description:		Constructor for the TEXT_RENDERING class.
//
// Input Arguments:
//		_RenderWindow	= RENDER_WINDOW& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TEXT_RENDERING::TEXT_RENDERING(RENDER_WINDOW &_RenderWindow) : PRIMITIVE(_RenderWindow)
{
	Color.Set(0.0, 0.0, 0.0, 1.0);

	Angle = 0.0;
	X = 0;
	Y = 0;
	Text = wxEmptyString;

	Centered = false;

	Font = NULL;
}

//==========================================================================
// Class:			TEXT_RENDERING
// Function:		~TEXT_RENDERING
//
// Description:		Destructor for the TEXT_RENDERING class.
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
TEXT_RENDERING::~TEXT_RENDERING()
{
}

//==========================================================================
// Class:			TEXT_RENDERING
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
void TEXT_RENDERING::GenerateGeometry(void)
{
	// Add the text
	if (Font && !Text.IsEmpty())
	{
		glPushMatrix();
			glLoadIdentity();

			// Position the text
			if (Centered)
				glTranslated(X - GetTextWidth() / 2.0 * cos(Angle * VVASEMath::PI / 180.0)
					+ GetTextHeight() / 2.0 * sin(Angle * VVASEMath::PI / 180.0),
					Y - GetTextWidth() / 2.0 * sin(Angle * VVASEMath::PI / 180.0)
					- GetTextHeight() / 2.0 * cos(Angle * VVASEMath::PI / 180.0), 0.0);
			else
				glTranslated(X, Y, 0.0);
			glRotated(Angle, 0.0, 0.0, 1.0);

			// Render the text
			Font->Render(Text.c_str());
		glPopMatrix();
	}

	return;
}

//==========================================================================
// Class:			TEXT_RENDERING
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
bool TEXT_RENDERING::HasValidParameters(void)
{
	// Don't draw if the angle is not a number
	if (VVASEMath::IsNaN(Angle))
		return false;

	return true;
}

//==========================================================================
// Class:			TEXT_RENDERING
// Function:		GetTextHeight
//
// Description:		Returns the height of the bounding box for the current
//					text.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double, height in pixels of the current text
//
//==========================================================================
double TEXT_RENDERING::GetTextHeight(void) const
{
	if (!Font)
		return 0.0;

	FTBBox BoundingBox = Font->BBox(Text.c_str());

	return BoundingBox.Upper().Y() - BoundingBox.Lower().Y();
}

//==========================================================================
// Class:			TEXT_RENDERING
// Function:		GetTextWidth
//
// Description:		Returns the width of the bounding box for the current
//					text.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double, width in pixels of the current text
//
//==========================================================================
double TEXT_RENDERING::GetTextWidth(void) const
{
	if (!Font)
		return 0.0;

	FTBBox BoundingBox = Font->BBox(Text.c_str());

	return BoundingBox.Upper().X() - BoundingBox.Lower().X();
}