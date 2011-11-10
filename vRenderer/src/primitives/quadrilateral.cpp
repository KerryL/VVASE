/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  quadrilateral.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating rectangular objects.
// History:

// Local headers
#include "vRenderer/primitives/quadrilateral.h"
#include "vRenderer/renderWindow.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			QUADRILATERAL
// Function:		QUADRILATERAL
//
// Description:		Constructor for the QUADRILATERAL class.
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
QUADRILATERAL::QUADRILATERAL(RenderWindow &_RenderWindow) : Primitive(_RenderWindow)
{
	// Initialize private data
	Normal.Set(0.0, 0.0, 0.0);
	Axis.Set(0.0, 0.0, 0.0);
	Width = 0.0;
	Length = 0.0;
}

//==========================================================================
// Class:			QUADRILATERAL
// Function:		~QUADRILATERAL
//
// Description:		Destructor for the QUADRILATERAL class.
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
QUADRILATERAL::~QUADRILATERAL()
{
}

//==========================================================================
// Class:			QUADRILATERAL
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
void QUADRILATERAL::GenerateGeometry(void)
{
	// Set the normal direction
	glNormal3d(Normal.x, Normal.y, Normal.z);

	// We'll use a triangle strip to draw the quad
	glBegin(GL_TRIANGLE_STRIP);

	// Calculate the distance from the center to each corner
	double HalfDiagonal = sqrt(Width * Width / 4.0 + Length * Length / 4.0);

	// Calculate the angle between the axis and each diagonal
	double DiagonalAngle = atan2(Width, Length);

	// Force the axis direction to be perpendicular to the normal
	Vector AxisDirection = Axis.Cross(Normal).Cross(Normal);

	// Compute the locations of the four corners of the quad
	Vector Corner1 = Center + AxisDirection.Normalize() * HalfDiagonal;
	Vector Corner2 = Center + AxisDirection.Normalize() * HalfDiagonal;
	Vector Corner3 = Center - AxisDirection.Normalize() * HalfDiagonal;
	Vector Corner4 = Center - AxisDirection.Normalize() * HalfDiagonal;

	Corner1 -= Center;
	Corner1.Rotate(DiagonalAngle, Normal);
	Corner1 += Center;

	Corner2 -= Center;
	Corner2.Rotate(-DiagonalAngle, Normal);
	Corner2 += Center;

	Corner3 -= Center;
	Corner3.Rotate(DiagonalAngle, Normal);
	Corner3 += Center;

	Corner4 -= Center;
	Corner4.Rotate(-DiagonalAngle, Normal);
	Corner4 += Center;

	// Add the vertecies to create two triangles
	// The order is 1, 4, 2, 3 because after the rotations, the corners are located
	// as shown (here, the longer dimension is the length):
	//  1 ---------- 4
	//   |          |
	//  2 ---------- 3
	glVertex3d(Corner1.x, Corner1.y, Corner1.z);
	glVertex3d(Corner4.x, Corner4.y, Corner4.z);
	glVertex3d(Corner2.x, Corner2.y, Corner2.z);
	glVertex3d(Corner3.x, Corner3.y, Corner3.z);

	// Complete the triangle strip
	glEnd();

	return;
}

//==========================================================================
// Class:			QUADRILATERAL
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
bool QUADRILATERAL::HasValidParameters(void)
{
	// Quads must have non-zero normal and axis vectors, non-zero dimensions,
	// and non-parallel normal and axis directions
	if (!VVASEMath::IsZero(Normal.Length()) && !VVASEMath::IsZero(Axis.Length()) &&
		!VVASEMath::IsZero(fabs(Axis * Normal) / (Axis.Length() * Normal.Length()) - 1.0)
		&& Width > 0.0 && Length > 0.0)
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			QUADRILATERAL
// Function:		SetNormal
//
// Description:		Sets the rectangle's normal direction.
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
void QUADRILATERAL::SetNormal(const Vector &_Normal)
{
	// Set the normal direction to the argument
	Normal = _Normal;
	
	// Reset the modified flag
	modified = true;

	return;
}

//==========================================================================
// Class:			QUADRILATERAL
// Function:		SetCenter
//
// Description:		Sets the location of the center of the rectangle.
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
void QUADRILATERAL::SetCenter(const Vector &_Center)
{
	// Set the center point to the argument
	Center = _Center;
	
	// Reset the modified flag
	modified = true;

	return;
}

//==========================================================================
// Class:			QUADRILATERAL
// Function:		SetAxis
//
// Description:		Sets the rectangle's orientation (direction of length dimension).
//					This vector must not be parallel to the normal direction.
//
// Input Arguments:
//		_Axis	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void QUADRILATERAL::SetAxis(const Vector &_Axis)
{
	// Set the axis direction to the argument
	Axis = _Axis;
	
	// Reset the modified flag
	modified = true;

	return;
}

//==========================================================================
// Class:			QUADRILATERAL
// Function:		SetWidth
//
// Description:		Sets the width of the quadrilateral (perpendicular to
//					the normal direction and to the axis).
//
// Input Arguments:
//		_Width	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void QUADRILATERAL::SetWidth(const double &_Width)
{
	// Set the width to the argument
	Width = _Width;
	
	// Reset the modified flag
	modified = true;

	return;
}

//==========================================================================
// Class:			QUADRILATERAL
// Function:		SetLength
//
// Description:		Sets the length (along the axis) of the quadrilateral.
//
// Input Arguments:
//		_Length	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void QUADRILATERAL::SetLength(const double &_Length)
{
	// Set the length to the argument
	Length = _Length;
	
	// Reset the modified flag
	modified = true;

	return;
}