/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  cylinder.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating cylindrical objects.
// History:
//	6/2/2009	- modified GenerateGeometry() to make use of openGL matrices for positioning
//				  and orienting the object, K.Loux.

// Local headers
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/renderWindow.h"
#include "vMath/carMath.h"
#include "vUtilities/unitConverter.h"

//==========================================================================
// Class:			Cylinder
// Function:		Cylinder
//
// Description:		Constructor for the Cylinder class.
//
// Input Arguments:
//		_renderWindow	= RenderWindow& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Cylinder::Cylinder(RenderWindow &_renderWindow) : Primitive(_renderWindow)
{
	// Initialize private data
	drawCaps = false;
	radius = 0.0;
	endPoint1.Set(0.0,0.0,0.0);
	endPoint2.Set(0.0,0.0,0.0);
	resolution = 4;
}

//==========================================================================
// Class:			Cylinder
// Function:		~Cylinder
//
// Description:		Destructor for the Cylinder class.
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
Cylinder::~Cylinder()
{
}

//==========================================================================
// Class:			Cylinder
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
void Cylinder::GenerateGeometry(void)
{
	// Resolution must be at least 3
	if (resolution < 3)
		resolution = 3;

	// Determine the height of the cylinder
	double halfHeight = endPoint1.Distance(endPoint2) / 2.0;

	// Determine the desired axis for the cylinder
	Vector axisDirection = (endPoint2 - endPoint1).Normalize();

	// Determine the center of the cylinder
	Vector center = endPoint1 + axisDirection * halfHeight;

	// Our reference direction will be the X-axis direction
	Vector referenceDirection(1.0, 0.0, 0.0);

	// Determine the angle and axis of rotation
	Vector axisOfRotation = referenceDirection.Cross(axisDirection);
	double angle = acos(axisDirection * referenceDirection);// [rad]

	// Push the current matrix
	glPushMatrix();

		// Translate the current matrix
		glTranslated(center.x, center.y, center.z);

		// Rotate the current matrix, if the rotation axis is non-zero
		if (!VVASEMath::IsZero(axisOfRotation.Length()))
			glRotated(UnitConverter::RAD_TO_DEG(angle), axisOfRotation.x, axisOfRotation.y, axisOfRotation.z);

		// Create the cylinder along the X-axis (must match the reference direction above)
		// (the openGL matrices take care of correct position/orientation in hardware)
		// We'll use a triangle strip to draw the cylinder
		glBegin(GL_TRIANGLE_STRIP);

		// Loop to generate the triangles
		int i;
		Vector point(halfHeight, 0.0, 0.0);
		for (i = 0; i <= resolution; i++)
		{
			// Determine the angle to the current point
			angle = (double)i * 2.0 * VVASEMath::Pi / (double)resolution;

			// Determine the Y and Z ordinates based on this angle and the radius
			point.y = radius * cos(angle);
			point.z = radius * sin(angle);

			// Set the normal for the next two points
			glNormal3d(0.0, point.y / radius, point.z / radius);

			// Add the next two points
			glVertex3d(point.x, point.y, point.z);
			glVertex3d(-point.x, point.y, point.z);
		}

		// End the triangle strip
		glEnd();

		// Draw the end caps, if they are enabled
		if (drawCaps)
		{
			// Set the normal for the first end cap
			glNormal3d(1.0, 0.0, 0.0);

			// Begin the polygon
			glBegin(GL_POLYGON);

			// Draw a polygon at the positive end of the cylinder
			for (i = 0; i <= resolution; i++)
			{
				// Determine the angle to the current point
				angle = (double)i * 2.0 * VVASEMath::Pi / (double)resolution;

				// Determine the Y and Z ordinates based on this angle and the radius
				point.y = radius * cos(angle);
				point.z = radius * sin(angle);

				// Add the next point
				glVertex3d(point.x, point.y, point.z);
			}

			// End the polygon
			glEnd();

			// Set the normal for the second end cap
			glNormal3d(-1.0, 0.0, 0.0);

			// Begin the polygon
			glBegin(GL_POLYGON);

			// Draw a polygon at the negative end of the cylinder
			for (i = 0; i <= resolution; i++)
			{
				// Determine the angle to the current point
				angle = (double)i * 2.0 * VVASEMath::Pi / (double)resolution;

				// Determine the Y and Z ordinates based on this angle and the radius
				point.y = radius * cos(angle);
				point.z = radius * sin(angle);

				// Add the next point
				glVertex3d(-point.x, point.y, point.z);
			}

			// End the polygon
			glEnd();
		}

	// Pop the matrix
	glPopMatrix();
}

//==========================================================================
// Class:			Cylinder
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
bool Cylinder::HasValidParameters(void)
{
	// Cylinders must have non-zero heights and a positive radius
	if (!VVASEMath::IsZero(endPoint1.Distance(endPoint2)) && radius > 0.0)
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the cylinder.
//
// Input Arguments:
//		_resolution	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetResolution(const int &_resolution)
{
	// Set the resolution to the argument
	resolution = _resolution;
	
	// Reset the modified flag
	modified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetCapping
//
// Description:		Sets the flag indicating whether or not the cylinder should be
//					capped on the base end.
//
// Input Arguments:
//		_drawCaps	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetCapping(const bool &_drawCaps)
{
	// Set the capping flag to the argument
	drawCaps = _drawCaps;
	
	// Reset the modified flag
	modified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetEndPoint1
//
// Description:		Sets the location of the first end point.
//
// Input Arguments:
//		_endPoint1	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetEndPoint1(const Vector &_endPoint1)
{
	// Set the end point to the argument
	endPoint1 = _endPoint1;
	
	// Reset the modified flag
	modified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetEndPoint2
//
// Description:		Sets the location of the second end point.
//
// Input Arguments:
//		_endPoint2	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetEndPoint2(const Vector &_endPoint2)
{
	// Set the end point to the argument
	endPoint2 = _endPoint2;
	
	// Reset the modified flag
	modified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetRadius
//
// Description:		Sets the radius at the base of the Cylinder.
//
// Input Arguments:
//		_radius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetRadius(const double &_radius)
{
	// Set the radius to the argument
	radius = _radius;
	
	// Reset the modified flag
	modified = true;
}