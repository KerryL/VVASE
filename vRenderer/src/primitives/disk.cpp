/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  disk.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating disk objects.
// History:
//	6/3/2009	- Modified GenerateGeometry() to make use of openGL matrices for positioning
//				  and orienting the object, K.Loux.

// Local headers
#include "vRenderer/primitives/disk.h"
#include "vRenderer/renderWindow.h"
#include "vMath/carMath.h"
#include "vUtilities/convert.h"

//==========================================================================
// Class:			DISK
// Function:		DISK
//
// Description:		Constructor for the DISK class.
//
// Input Arguments:
//		_RenderWindow	= RenderWindow* pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DISK::DISK(RenderWindow &_RenderWindow) : Primitive(_RenderWindow)
{
	// Initialize private data
	OuterRadius = 0.0;
	InnerRadius = 0.0;
	Center.Set(0.0, 0.0, 0.0);
	Normal.Set(0.0, 0.0, 0.0);
	Resolution = 4;
}

//==========================================================================
// Class:			DISK
// Function:		~DISK
//
// Description:		Destructor for the DISK class.
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
DISK::~DISK()
{
}

//==========================================================================
// Class:			DISK
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
void DISK::GenerateGeometry(void)
{
	// Resolution must be at least 3
	if (Resolution < 3)
		Resolution = 3;

	// Our reference direction will be the X-axis direction
	Vector ReferenceDirection(1.0, 0.0, 0.0);

	// Determine the angle and axis of rotation
	Vector AxisOfRotation = ReferenceDirection.Cross(Normal);
	double Angle = acos(Normal * ReferenceDirection);// [rad]

	// Push the current matrix
	glPushMatrix();

		// Translate the current matrix
		glTranslated(Center.x, Center.y, Center.z);

		// Rotate the current matrix, if the rotation axis is non-zero
		if (!VVASEMath::IsZero(AxisOfRotation.Length()))
			glRotated(Convert::RAD_TO_DEG(Angle), AxisOfRotation.x, AxisOfRotation.y, AxisOfRotation.z);

		// Set the normal direction
		glNormal3d(Normal.x, Normal.y, Normal.z);

		// We'll use a triangle strip to draw the disk
		glBegin(GL_TRIANGLE_STRIP);

		// Loop to generate the triangles
		Vector InsidePoint(0.0, 0.0, 0.0);
		Vector OutsidePoint(0.0, 0.0, 0.0);
		int i;
		for (i = 0; i <= Resolution; i++)
		{
			// Determine the angle to the current set of points
			Angle = (double)i * 2.0 * VVASEMath::Pi / (double)Resolution;

			// Determine the Y and Z ordinates based on this angle and the radii
			OutsidePoint.y = OuterRadius * cos(Angle);
			OutsidePoint.z = OuterRadius * sin(Angle);

			InsidePoint.y = InnerRadius * cos(Angle);
			InsidePoint.z = InnerRadius * sin(Angle);

			// Add the next two points
			glVertex3d(OutsidePoint.x, OutsidePoint.y, OutsidePoint.z);
			glVertex3d(InsidePoint.x, InsidePoint.y, InsidePoint.z);
		}

		// Complete the triangle strip
		glEnd();

	// Pop the matrix
	glPopMatrix();

	return;
}

//==========================================================================
// Class:			DISK
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
bool DISK::HasValidParameters(void)
{
	// Disks must have a positive
	if (OuterRadius > 0.0 && !VVASEMath::IsZero(Normal.Length()))
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			DISK
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the disk.
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
void DISK::SetResolution(const int &_Resolution)
{
	// Set the resolution to the argument
	Resolution = _Resolution;
	
	// Reset the modified flag
	modified = true;

	return;
}

//==========================================================================
// Class:			DISK
// Function:		SetOuterRadius
//
// Description:		Sets the outer radius of the disk.
//
// Input Arguments:
//		_OuterRadius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DISK::SetOuterRadius(const double &_OuterRadius)
{
	// Set the outer radius to the argument
	OuterRadius = _OuterRadius;
	
	// Reset the modified flag
	modified = true;

	return;
}

//==========================================================================
// Class:			DISK
// Function:		SetInnerRadius
//
// Description:		Sets the inner radius of the disk.
//
// Input Arguments:
//		_InnerRadius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DISK::SetInnerRadius(const double &_InnerRadius)
{
	// Set the inner radius to the argument
	InnerRadius = _InnerRadius;
	
	// Reset the modified flag
	modified = true;

	return;
}

//==========================================================================
// Class:			DISK
// Function:		SetCenter
//
// Description:		Sets the location of the center of the disk.
//
// Input Arguments:
//		_Center	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DISK::SetCenter(const Vector &_Center)
{
	// Set the center point to the argument
	Center = _Center;
	
	// Reset the modified flag
	modified = true;

	return;
}

//==========================================================================
// Class:			DISK
// Function:		SetNormal
//
// Description:		Sets the disk's normal direction.
//
// Input Arguments:
//		_Normal	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DISK::SetNormal(const Vector &_Normal)
{
	// Set the normal vector to the argument
	Normal = _Normal;
	
	// Reset the modified flag
	modified = true;

	return;
}