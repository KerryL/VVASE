/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  quadrilateral.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating rectangular objects.
// History:

// Local headers
#include "vRenderer/primitives/quadrilateral.h"
#include "vRenderer/renderWindow.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			Quadrilateral
// Function:		Quadrilateral
//
// Description:		Constructor for the Quadrilateral class.
//
// Input Arguments:
//		renderWindow	= RenderWindow& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Quadrilateral::Quadrilateral(RenderWindow &renderWindow) : Primitive(renderWindow)
{
	normal.Set(0.0, 0.0, 0.0);
	axis.Set(0.0, 0.0, 0.0);
	width = 0.0;
	length = 0.0;
}

//==========================================================================
// Class:			Quadrilateral
// Function:		~Quadrilateral
//
// Description:		Destructor for the Quadrilateral class.
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
Quadrilateral::~Quadrilateral()
{
}

//==========================================================================
// Class:			Quadrilateral
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
void Quadrilateral::GenerateGeometry()
{
	// Set the normal direction
	glNormal3d(normal.x, normal.y, normal.z);

	// We'll use a triangle strip to draw the quad
	glBegin(GL_TRIANGLE_STRIP);

	// Calculate the distance from the center to each corner
	double halfDiagonal = sqrt(width * width / 4.0 + length * length / 4.0);

	// Calculate the angle between the axis and each diagonal
	double diagonalAngle = atan2(width, length);

	// Force the axis direction to be perpendicular to the normal
	Vector axisDirection = axis.Cross(normal).Cross(normal);

	// Compute the locations of the four corners of the quad
	Vector corner1 = center + axisDirection.Normalize() * halfDiagonal;
	Vector corner2 = center + axisDirection.Normalize() * halfDiagonal;
	Vector corner3 = center - axisDirection.Normalize() * halfDiagonal;
	Vector corner4 = center - axisDirection.Normalize() * halfDiagonal;

	corner1 -= center;
	corner1.Rotate(diagonalAngle, normal);
	corner1 += center;

	corner2 -= center;
	corner2.Rotate(-diagonalAngle, normal);
	corner2 += center;

	corner3 -= center;
	corner3.Rotate(diagonalAngle, normal);
	corner3 += center;

	corner4 -= center;
	corner4.Rotate(-diagonalAngle, normal);
	corner4 += center;

	// Add the vertices to create two triangles
	// The order is 1, 4, 2, 3 because after the rotations, the corners are located
	// as shown (here, the longer dimension is the length):
	//  1 ---------- 4
	//   |          |
	//  2 ---------- 3
	glVertex3d(corner1.x, corner1.y, corner1.z);
	glVertex3d(corner4.x, corner4.y, corner4.z);
	glVertex3d(corner2.x, corner2.y, corner2.z);
	glVertex3d(corner3.x, corner3.y, corner3.z);

	// Complete the triangle strip
	glEnd();
}

//==========================================================================
// Class:			Quadrilateral
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
bool Quadrilateral::HasValidParameters()
{
	// Quads must have non-zero normal and axis vectors, non-zero dimensions,
	// and non-parallel normal and axis directions
	if (!VVASEMath::IsZero(normal.Length()) && !VVASEMath::IsZero(axis.Length()) &&
		!VVASEMath::IsZero(fabs(axis * normal) / (axis.Length() * normal.Length()) - 1.0)
		&& width > 0.0 && length > 0.0)
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			Quadrilateral
// Function:		SetNormal
//
// Description:		Sets the rectangle's normal direction.
//
// Input Arguments:
//		_normal	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Quadrilateral::SetNormal(const Vector &normal)
{
	this->normal = normal;
	modified = true;
}

//==========================================================================
// Class:			Quadrilateral
// Function:		SetCenter
//
// Description:		Sets the location of the center of the rectangle.
//
// Input Arguments:
//		center	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Quadrilateral::SetCenter(const Vector &center)
{
	this->center = center;
	modified = true;
}

//==========================================================================
// Class:			Quadrilateral
// Function:		SetAxis
//
// Description:		Sets the rectangle's orientation (direction of length dimension).
//					This vector must not be parallel to the normal direction.
//
// Input Arguments:
//		axis	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Quadrilateral::SetAxis(const Vector &axis)
{
	this->axis = axis;
	modified = true;
}

//==========================================================================
// Class:			Quadrilateral
// Function:		SetWidth
//
// Description:		Sets the width of the quadrilateral (perpendicular to
//					the normal direction and to the axis).
//
// Input Arguments:
//		width	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Quadrilateral::SetWidth(const double &width)
{
	this->width = width;
	modified = true;
}

//==========================================================================
// Class:			Quadrilateral
// Function:		SetLength
//
// Description:		Sets the length (along the axis) of the quadrilateral.
//
// Input Arguments:
//		_length	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Quadrilateral::SetLength(const double &length)
{
	this->length = length;
	modified = true;
}