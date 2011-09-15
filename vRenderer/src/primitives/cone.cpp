/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  cone.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating conical objects.
// History:
//	6/2/2009	- Modified GenerateGeometry() to make use of openGL matricies for positioning
//				  and orienting the object, K.Loux.

// Local headers
#include "vRenderer/primitives/cone.h"
#include "vRenderer/render_window_class.h"
#include "vMath/car_math.h"
#include "vUtilities/convert_class.h"

//==========================================================================
// Class:			CONE
// Function:		CONE
//
// Description:		Constructor for the CONE class.
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
CONE::CONE(RENDER_WINDOW &_RenderWindow) : PRIMITIVE(_RenderWindow)
{
	// Initialize private data
	DrawCaps = false;
	Radius = 0.0;
	Tip.Set(0.0,0.0,0.0);
	BaseCenter.Set(0.0,0.0,0.0);
	Resolution = 4;
}

//==========================================================================
// Class:			CONE
// Function:		~CONE
//
// Description:		Destructor for the CONE class.
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
CONE::~CONE()
{
}

//==========================================================================
// Class:			CONE
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
void CONE::GenerateGeometry(void)
{
	// Resolution must be at least 3
	if (Resolution < 3)
		Resolution = 3;

	// Determine the height of the cone
	double HalfHeight = BaseCenter.Distance(Tip) / 2.0;

	// Determine the desired axis for the cone
	VECTOR AxisDirection = (Tip - BaseCenter).Normalize();

	// Determine the center of the cone
	VECTOR Center = BaseCenter + AxisDirection * HalfHeight;

	// Our reference direction will be the X-axis direction
	VECTOR ReferenceDirection(1.0, 0.0, 0.0);

	// Determine the angle and axis of rotation
	VECTOR AxisOfRotation = ReferenceDirection.Cross(AxisDirection);
	double Angle = acos(AxisDirection * ReferenceDirection);// [rad]

	// If the axis direction is opposite the reference direction, we need to rotate 180 degrees
	if (VVASEMath::IsZero(AxisDirection + ReferenceDirection))
	{
		Angle = CONVERT::Pi;
		AxisOfRotation.Set(0.0, 1.0, 0.0);
	}

	// Push the current matrix
	glPushMatrix();

		// Translate the current matrix
		glTranslated(Center.X, Center.Y, Center.Z);

		// Rotate the current matrix, if the rotation axis is non-zero
		if (!VVASEMath::IsZero(AxisOfRotation.Length()))
			glRotated(CONVERT::RAD_TO_DEG(Angle), AxisOfRotation.X, AxisOfRotation.Y, AxisOfRotation.Z);

		// Create the cone along the X-axis (must match the reference direction above)
		// (the openGL matricies take care of correct position/orientation in hardware)
		// We'll use a triangle fan to draw the cone
		glBegin(GL_TRIANGLE_FAN);

		// Set the first normal and the tip (center of the fan)
		glNormal3d(1.0, 0.0, 0.0);
		glVertex3d(HalfHeight, 0.0, 0.0);

		// Loop to generate the triangles
		int i;
		VECTOR Point(-HalfHeight, 0.0, 0.0);
		for (i = 0; i <= Resolution; i++)
		{
			// Determine the angle to the current point
			Angle = (double)i * 2.0 * CONVERT::Pi / (double)Resolution;

			// Determine the Y and Z ordinates based on this angle and the radius
			Point.Y = Radius * cos(Angle);
			Point.Z = Radius * sin(Angle);

			// Set the normal for the next two points
			glNormal3d(0.0, Point.Y / Radius, Point.Z / Radius);

			// Add the next point
			glVertex3d(Point.X, Point.Y, Point.Z);
		}

		// End the triangle strip
		glEnd();

		// Draw the end cap, if it is enabled
		if (DrawCaps)
		{
			// Set the normal for the end cap
			glNormal3d(-1.0, 0.0, 0.0);

			// Begin the polygon
			glBegin(GL_POLYGON);

			// Draw a polygon at the base of the cone
			for (i = 0; i <= Resolution; i++)
			{
				// Determine the angle to the current point
				Angle = (double)i * 2.0 * CONVERT::Pi / (double)Resolution;

				// Determine the Y and Z ordinates based on this angle and the radius
				Point.Y = Radius * cos(Angle);
				Point.Z = Radius * sin(Angle);

				// Add the next point
				glVertex3d(Point.X, Point.Y, Point.Z);
			}

			// End the polygon
			glEnd();
		}

	// Pop the matrix
	glPopMatrix();

	return;
}

//==========================================================================
// Class:			CONE
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
bool CONE::HasValidParameters(void)
{
	// Cones must have a non-zero distance tip-to-base, and must have a positive radius
	if (!VVASEMath::IsZero(Tip.Distance(BaseCenter)) && Radius > 0.0)
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			CONE
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the cone.
//
// Input Argurments:
//		_Resolution	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONE::SetResolution(const int &_Resolution)
{
	// Set the resolution to the argument
	Resolution = _Resolution;
	
	// Reset the modified flag
	Modified = true;

	return;
}

//==========================================================================
// Class:			CONE
// Function:		SetCapping
//
// Description:		Sets the flag indicating whether or not the cone should be
//					capped on the base end.
//
// Input Argurments:
//		_DrawCaps	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONE::SetCapping(const bool &_DrawCaps)
{
	// Set the capping flag to the argument
	DrawCaps = _DrawCaps;
	
	// Reset the modified flag
	Modified = true;

	return;
}

//==========================================================================
// Class:			CONE
// Function:		SetTip
//
// Description:		Sets the location of the tip of the cone.
//
// Input Argurments:
//		_Tip	= const VECTOR&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONE::SetTip(const VECTOR &_Tip)
{
	// Set the tip location to the argument
	Tip = _Tip;
	
	// Reset the modified flag
	Modified = true;

	return;
}

//==========================================================================
// Class:			CONE
// Function:		SetBaseCenter
//
// Description:		Sets the location of the center of the cone's base.
//
// Input Argurments:
//		_BaseCenter	= const VECTOR&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONE::SetBaseCenter(const VECTOR &_BaseCenter)
{
	// Set the base location to the argument
	BaseCenter = _BaseCenter;
	
	// Reset the modified flag
	Modified = true;

	return;
}

//==========================================================================
// Class:			CONE
// Function:		SetRadius
//
// Description:		Sets the radius at the base of the cone.
//
// Input Argurments:
//		_Radius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONE::SetRadius(const double &_Radius)
{
	// Set the radius to the argument
	Radius = _Radius;
	
	// Reset the modified flag
	Modified = true;

	return;
}