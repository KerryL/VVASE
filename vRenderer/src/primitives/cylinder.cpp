/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  cylinder.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating cylindrical objects.
// History:
//	6/2/2009	- modified GenerateGeometry() to make use of openGL matrices for positioning
//				  and orienting the object, K.Loux.

// Local headers
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/renderWindow.h"
#include "vMath/carMath.h"
#include "vUtilities/convert.h"

//==========================================================================
// Class:			CYLINDER
// Function:		CYLINDER
//
// Description:		Constructor for the CYLINDER class.
//
// Input Arguments:
//		_RenderWindow	= RenderWindow& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
CYLINDER::CYLINDER(RenderWindow &_RenderWindow) : Primitive(_RenderWindow)
{
	// Initialize private data
	DrawCaps = false;
	Radius = 0.0;
	EndPoint1.Set(0.0,0.0,0.0);
	EndPoint2.Set(0.0,0.0,0.0);
	Resolution = 4;
}

//==========================================================================
// Class:			CYLINDER
// Function:		~CYLINDER
//
// Description:		Destructor for the CYLINDER class.
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
CYLINDER::~CYLINDER()
{
}

//==========================================================================
// Class:			CYLINDER
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
void CYLINDER::GenerateGeometry(void)
{
	// Resolution must be at least 3
	if (Resolution < 3)
		Resolution = 3;

	// Determine the height of the cylinder
	double HalfHeight = EndPoint1.Distance(EndPoint2) / 2.0;

	// Determine the desired axis for the cylinder
	Vector AxisDirection = (EndPoint2 - EndPoint1).Normalize();

	// Determine the center of the cylinder
	Vector Center = EndPoint1 + AxisDirection * HalfHeight;

	// Our reference direction will be the X-axis direction
	Vector ReferenceDirection(1.0, 0.0, 0.0);

	// Determine the angle and axis of rotation
	Vector AxisOfRotation = ReferenceDirection.Cross(AxisDirection);
	double Angle = acos(AxisDirection * ReferenceDirection);// [rad]

	// Push the current matrix
	glPushMatrix();

		// Translate the current matrix
		glTranslated(Center.x, Center.y, Center.z);

		// Rotate the current matrix, if the rotation axis is non-zero
		if (!VVASEMath::IsZero(AxisOfRotation.Length()))
			glRotated(Convert::RAD_TO_DEG(Angle), AxisOfRotation.x, AxisOfRotation.y, AxisOfRotation.z);

		// Create the cylinder along the X-axis (must match the reference direction above)
		// (the openGL matrices take care of correct position/orientation in hardware)
		// We'll use a triangle strip to draw the cylinder
		glBegin(GL_TRIANGLE_STRIP);

		// Loop to generate the triangles
		int i;
		Vector Point(HalfHeight, 0.0, 0.0);
		for (i = 0; i <= Resolution; i++)
		{
			// Determine the angle to the current point
			Angle = (double)i * 2.0 * VVASEMath::Pi / (double)Resolution;

			// Determine the Y and Z ordinates based on this angle and the radius
			Point.y = Radius * cos(Angle);
			Point.z = Radius * sin(Angle);

			// Set the normal for the next two points
			glNormal3d(0.0, Point.y / Radius, Point.z / Radius);

			// Add the next two points
			glVertex3d(Point.x, Point.y, Point.z);
			glVertex3d(-Point.x, Point.y, Point.z);
		}

		// End the triangle strip
		glEnd();

		// Draw the end caps, if they are enabled
		if (DrawCaps)
		{
			// Set the normal for the first end cap
			glNormal3d(1.0, 0.0, 0.0);

			// Begin the polygon
			glBegin(GL_POLYGON);

			// Draw a polygon at the positive end of the cylinder
			for (i = 0; i <= Resolution; i++)
			{
				// Determine the angle to the current point
				Angle = (double)i * 2.0 * VVASEMath::Pi / (double)Resolution;

				// Determine the Y and Z ordinates based on this angle and the radius
				Point.y = Radius * cos(Angle);
				Point.z = Radius * sin(Angle);

				// Add the next point
				glVertex3d(Point.x, Point.y, Point.z);
			}

			// End the polygon
			glEnd();

			// Set the normal for the second end cap
			glNormal3d(-1.0, 0.0, 0.0);

			// Begin the polygon
			glBegin(GL_POLYGON);

			// Draw a polygon at the negative end of the cylinder
			for (i = 0; i <= Resolution; i++)
			{
				// Determine the angle to the current point
				Angle = (double)i * 2.0 * VVASEMath::Pi / (double)Resolution;

				// Determine the Y and Z ordinates based on this angle and the radius
				Point.y = Radius * cos(Angle);
				Point.z = Radius * sin(Angle);

				// Add the next point
				glVertex3d(-Point.x, Point.y, Point.z);
			}

			// End the polygon
			glEnd();
		}

	// Pop the matrix
	glPopMatrix();

	return;
}

//==========================================================================
// Class:			CYLINDER
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
bool CYLINDER::HasValidParameters(void)
{
	// Cylinders must have non-zero heights and a positive radius
	if (!VVASEMath::IsZero(EndPoint1.Distance(EndPoint2)) && Radius > 0.0)
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			CYLINDER
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the cylinder.
//
// Input Arguments:
//		_Resolution	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CYLINDER::SetResolution(const int &_Resolution)
{
	// Set the resolution to the argument
	Resolution = _Resolution;
	
	// Reset the modified flag
	modified = true;

	return;
}

//==========================================================================
// Class:			CYLINDER
// Function:		SetCapping
//
// Description:		Sets the flag indicating whether or not the cylinder should be
//					capped on the base end.
//
// Input Arguments:
//		_DrawCaps	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CYLINDER::SetCapping(const bool &_DrawCaps)
{
	// Set the capping flag to the argument
	DrawCaps = _DrawCaps;
	
	// Reset the modified flag
	modified = true;

	return;
}

//==========================================================================
// Class:			CYLINDER
// Function:		SetEndPoint1
//
// Description:		Sets the location of the first end point.
//
// Input Arguments:
//		_EndPoint1	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CYLINDER::SetEndPoint1(const Vector &_EndPoint1)
{
	// Set the end point to the argument
	EndPoint1 = _EndPoint1;
	
	// Reset the modified flag
	modified = true;

	return;
}

//==========================================================================
// Class:			CYLINDER
// Function:		SetEndPoint2
//
// Description:		Sets the location of the second end point.
//
// Input Arguments:
//		_EndPoint2	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CYLINDER::SetEndPoint2(const Vector &_EndPoint2)
{
	// Set the end point to the argument
	EndPoint2 = _EndPoint2;
	
	// Reset the modified flag
	modified = true;

	return;
}

//==========================================================================
// Class:			CYLINDER
// Function:		SetRadius
//
// Description:		Sets the radius at the base of the CYLINDER.
//
// Input Arguments:
//		_Radius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CYLINDER::SetRadius(const double &_Radius)
{
	// Set the radius to the argument
	Radius = _Radius;
	
	// Reset the modified flag
	modified = true;

	return;
}