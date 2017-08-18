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
#include "VVASE/gui/renderer/primitives/triangle.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

namespace VVASE
{

//==========================================================================
// Class:			Triangle
// Function:		Triangle
//
// Description:		Constructor for the Triangle class.
//
// Input Arguments:
//		renderWindow	= LibPlot2D::RenderWindow& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Triangle::Triangle(LibPlot2D::RenderWindow &renderWindow) : Primitive(renderWindow)
{
	corner1.setZero();
	corner2.setZero();
	corner3.setZero();
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
	const Eigen::Vector3d normal((corner2 - corner1).cross(corner3 - corner1).normalized());

	// Set the normal for the triangle
	glNormal3d(normal.x(), normal.y(), normal.z());

	// This is just one triangle
	glBegin(GL_TRIANGLES);

	// Add the three vertices
	glVertex3d(corner1.x(), corner1.y(), corner1.z());
	glVertex3d(corner2.x(), corner2.y(), corner2.z());
	glVertex3d(corner3.x(), corner3.y(), corner3.z());

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
	if (!VVASE::Math::IsZero((corner1 - corner2).norm()) &&
		!VVASE::Math::IsZero((corner1 - corner3).norm()) &&
		!VVASE::Math::IsZero((corner2 - corner3).norm()))
		return true;

	return false;
}

//==========================================================================
// Class:			Triangle
// Function:		SetCorner1
//
// Description:		Sets the location of the first corner of the triangle.
//
// Input Arguments:
//		_corner1	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Triangle::SetCorner1(const Eigen::Vector3d &corner1)
{
	this->corner1 = corner1;
	mModified = true;
}

//==========================================================================
// Class:			Triangle
// Function:		SetCorner2
//
// Description:		Sets the location of the second corner of the triangle.
//
// Input Arguments:
//		corner2	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Triangle::SetCorner2(const Eigen::Vector3d &corner2)
{
	this->corner2 = corner2;
	mModified = true;
}

//==========================================================================
// Class:			Triangle
// Function:		SetCorner3
//
// Description:		Sets the location of the third corner of the triangle.
//
// Input Arguments:
//		corner3	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Triangle::SetCorner3(const Eigen::Vector3d &corner3)
{
	this->corner3 = corner3;
	mModified = true;
}

//==========================================================================
// Class:			Triangle
// Function:		IsIntersectedBy
//
// Description:		Determines if this object is intersected by the specified
//					line.
//
// Input Arguments:
//		point		= const Eigen::Vector3d&
//		direction	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool Triangle::IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	// TODO:  Implement
	return false;
}

}// namespace VVASE
