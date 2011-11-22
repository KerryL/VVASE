/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  cone.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating conical objects.
// History:
//	6/2/2009	- modified GenerateGeometry() to make use of openGL matrices for positioning
//				  and orienting the object, K.Loux.

// Local headers
#include "vRenderer/primitives/cone.h"
#include "vRenderer/renderWindow.h"
#include "vMath/carMath.h"
#include "vUtilities/convert.h"

//==========================================================================
// Class:			Cone
// Function:		Cone
//
// Description:		Constructor for the Cone class.
//
// Input Arguments:
//		_renderWindow	= RenderWindow& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Cone::Cone(RenderWindow &_renderWindow) : Primitive(_renderWindow)
{
	// Initialize private data
	drawCaps = false;
	radius = 0.0;
	tip.Set(0.0,0.0,0.0);
	baseCenter.Set(0.0,0.0,0.0);
	resolution = 4;
}

//==========================================================================
// Class:			Cone
// Function:		~Cone
//
// Description:		Destructor for the Cone class.
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
Cone::~Cone()
{
}

//==========================================================================
// Class:			Cone
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
void Cone::GenerateGeometry(void)
{
	// Resolution must be at least 3
	if (resolution < 3)
		resolution = 3;

	// Determine the height of the cone
	double halfHeight = baseCenter.Distance(tip) / 2.0;

	// Determine the desired axis for the cone
	Vector axisDirection = (tip - baseCenter).Normalize();

	// Determine the center of the cone
	Vector center = baseCenter + axisDirection * halfHeight;

	// Our reference direction will be the X-axis direction
	Vector referenceDirection(1.0, 0.0, 0.0);

	// Determine the angle and axis of rotation
	Vector axisOfRotation = referenceDirection.Cross(axisDirection);
	double angle = acos(axisDirection * referenceDirection);// [rad]

	// If the axis direction is opposite the reference direction, we need to rotate 180 degrees
	if (VVASEMath::IsZero(axisDirection + referenceDirection))
	{
		angle = Convert::Pi;
		axisOfRotation.Set(0.0, 1.0, 0.0);
	}

	// Push the current matrix
	glPushMatrix();

		// Translate the current matrix
		glTranslated(center.x, center.y, center.z);

		// Rotate the current matrix, if the rotation axis is non-zero
		if (!VVASEMath::IsZero(axisOfRotation.Length()))
			glRotated(Convert::RAD_TO_DEG(angle), axisOfRotation.x, axisOfRotation.y, axisOfRotation.z);

		// Create the cone along the X-axis (must match the reference direction above)
		// (the openGL matrices take care of correct position/orientation in hardware)
		// We'll use a triangle fan to draw the cone
		glBegin(GL_TRIANGLE_FAN);

		// Set the first normal and the tip (center of the fan)
		glNormal3d(1.0, 0.0, 0.0);
		glVertex3d(halfHeight, 0.0, 0.0);

		// Loop to generate the triangles
		int i;
		Vector point(-halfHeight, 0.0, 0.0);
		for (i = 0; i <= resolution; i++)
		{
			// Determine the angle to the current point
			angle = (double)i * 2.0 * Convert::Pi / (double)resolution;

			// Determine the Y and Z ordinates based on this angle and the radius
			point.y = radius * cos(angle);
			point.z = radius * sin(angle);

			// Set the normal for the next two points
			glNormal3d(0.0, point.y / radius, point.z / radius);

			// Add the next point
			glVertex3d(point.x, point.y, point.z);
		}

		// End the triangle strip
		glEnd();

		// Draw the end cap, if it is enabled
		if (drawCaps)
		{
			// Set the normal for the end cap
			glNormal3d(-1.0, 0.0, 0.0);

			// Begin the polygon
			glBegin(GL_POLYGON);

			// Draw a polygon at the base of the cone
			for (i = 0; i <= resolution; i++)
			{
				// Determine the angle to the current point
				angle = (double)i * 2.0 * Convert::Pi / (double)resolution;

				// Determine the Y and Z ordinates based on this angle and the radius
				point.y = radius * cos(angle);
				point.z = radius * sin(angle);

				// Add the next point
				glVertex3d(point.x, point.y, point.z);
			}

			// End the polygon
			glEnd();
		}

	// Pop the matrix
	glPopMatrix();
}

//==========================================================================
// Class:			Cone
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
bool Cone::HasValidParameters(void)
{
	// Cones must have a non-zero distance tip-to-base, and must have a positive radius
	if (!VVASEMath::IsZero(tip.Distance(baseCenter)) && radius > 0.0)
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			Cone
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the cone.
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
void Cone::SetResolution(const int &_resolution)
{
	// Set the resolution to the argument
	resolution = _resolution;
	
	// Reset the modified flag
	modified = true;
}

//==========================================================================
// Class:			Cone
// Function:		SetCapping
//
// Description:		Sets the flag indicating whether or not the cone should be
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
void Cone::SetCapping(const bool &_drawCaps)
{
	// Set the capping flag to the argument
	drawCaps = _drawCaps;
	
	// Reset the modified flag
	modified = true;
}

//==========================================================================
// Class:			Cone
// Function:		SetTip
//
// Description:		Sets the location of the tip of the cone.
//
// Input Arguments:
//		_tip	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cone::SetTip(const Vector &_tip)
{
	// Set the tip location to the argument
	tip = _tip;
	
	// Reset the modified flag
	modified = true;
}

//==========================================================================
// Class:			Cone
// Function:		SetBaseCenter
//
// Description:		Sets the location of the center of the cone's base.
//
// Input Arguments:
//		_baseCenter	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cone::SetBaseCenter(const Vector &_baseCenter)
{
	// Set the base location to the argument
	baseCenter = _baseCenter;
	
	// Reset the modified flag
	modified = true;
}

//==========================================================================
// Class:			Cone
// Function:		SetRadius
//
// Description:		Sets the radius at the base of the cone.
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
void Cone::SetRadius(const double &_radius)
{
	// Set the radius to the argument
	radius = _radius;
	
	// Reset the modified flag
	modified = true;
}