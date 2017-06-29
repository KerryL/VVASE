/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  triangle.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating triangular objects.

// Local headers
#include "vRenderer/primitives/triangle.h"
#include "vRenderer/renderWindow.h"
#include "vMath/carMath.h"

namespace VVASE
{

//==========================================================================
// Class:			Triangle
// Function:		Triangle
//
// Description:		Constructor for the Triangle class.
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
Triangle::Triangle(RenderWindow &renderWindow) : Primitive(renderWindow)
{
	corner1.Set(0.0, 0.0, 0.0);
	corner2.Set(0.0, 0.0, 0.0);
	corner3.Set(0.0, 0.0, 0.0);
}

//==========================================================================
// Class:			Triangle
// Function:		~Triangle
//
// Description:		Destructor for the Triangle class.
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
Triangle::~Triangle()
{
}

//==========================================================================
// Class:			Triangle
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
void Triangle::GenerateGeometry()
{
	// Used as each triangle is created
	Vector normal = (corner2 - corner1).Cross(corner3 - corner1).Normalize();

	// Set the normal for the triangle
	glNormal3d(normal.x, normal.y, normal.z);

	// This is just one triangle
	glBegin(GL_TRIANGLES);

	// Add the three vertices
	glVertex3d(corner1.x, corner1.y, corner1.z);
	glVertex3d(corner2.x, corner2.y, corner2.z);
	glVertex3d(corner3.x, corner3.y, corner3.z);

	// Complete the triangle
	glEnd();
}

//==========================================================================
// Class:			Triangle
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
bool Triangle::HasValidParameters()
{
	// Triangles must have non-zero edge lengths
	if (!VVASEMath::IsZero(corner1.Distance(corner2)) && !VVASEMath::IsZero(corner1.Distance(corner3))
		&& !VVASEMath::IsZero(corner2.Distance(corner3)))
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			Triangle
// Function:		SetCorner1
//
// Description:		Sets the location of the first corner of the triangle.
//
// Input Arguments:
//		_corner1	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Triangle::SetCorner1(const Vector &corner1)
{
	this->corner1 = corner1;
	modified = true;
}

//==========================================================================
// Class:			Triangle
// Function:		SetCorner2
//
// Description:		Sets the location of the second corner of the triangle.
//
// Input Arguments:
//		corner2	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Triangle::SetCorner2(const Vector &corner2)
{
	this->corner2 = corner2;
	modified = true;
}

//==========================================================================
// Class:			Triangle
// Function:		SetCorner3
//
// Description:		Sets the location of the third corner of the triangle.
//
// Input Arguments:
//		corner3	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Triangle::SetCorner3(const Vector &corner3)
{
	this->corner3 = corner3;
	modified = true;
}

//==========================================================================
// Class:			Triangle
// Function:		IsIntersectedBy
//
// Description:		Determines if this object is intersected by the specified
//					line.
//
// Input Arguments:
//		point		= const Vector&
//		direction	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool Triangle::IsIntersectedBy(const Vector& point, const Vector& direction) const
{
	// TODO:  Implement
	return false;
}

}// namespace VVASE
