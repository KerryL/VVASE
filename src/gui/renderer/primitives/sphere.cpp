/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  sphere.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating spherical objects.

// Standard C++ headers
#include <cassert>

// Local headers
#include "VVASE/gui/renderer/primitives/sphere.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

// For choosing between an octohedron (8 sides) or a icosohedron (20 sides) as the base polygon
// for the recursive subdivision to create the sphere.
#define ICOSOHEDRON

namespace VVASE
{

//==========================================================================
// Class:			Sphere
// Function:		Sphere
//
// Description:		Constructor for the Sphere class.
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
Sphere::Sphere(LibPlot2D::RenderWindow &renderWindow) : Primitive(renderWindow)
{
	center.setZero();
	radius = 0.0;
	resolution = 4;
}

//==========================================================================
// Class:			Sphere
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
void Sphere::GenerateGeometry()
{
	if (resolution < 0)
		resolution = 0;

#ifdef ICOSOHEDRON
	// To avoid performance issues, don't let the resolution go above 2
	if (resolution > 2)
		resolution = 2;

	// Find twelve vertices that define an icosohedron circumscribed within the sphere
	double t = (1.0 + sqrt(5.0)) / 2.0;
	double s = sqrt(1 + t * t);
	Eigen::Vector3d vertex[12];
	vertex[0] = Eigen::Vector3d(t, 1.0, 0.0);
	vertex[1] = Eigen::Vector3d(-t, 1.0, 0.0);
	vertex[2] = Eigen::Vector3d(t, -1.0, 0.0);
	vertex[3] = Eigen::Vector3d(-t, -1.0, 0.0);
	vertex[4] = Eigen::Vector3d(1.0, 0.0, t);
	vertex[5] = Eigen::Vector3d(1.0, 0.0, -t);
	vertex[6] = Eigen::Vector3d(-1, 0.0, t);
	vertex[7] = Eigen::Vector3d(-1.0, 0.0, -t);
	vertex[8] = Eigen::Vector3d(0.0, t, 1.0);
	vertex[9] = Eigen::Vector3d(0.0, -t, 1.0);
	vertex[10] = Eigen::Vector3d(0.0, t, -1.0);
	vertex[11] = Eigen::Vector3d(0.0, -t, -1.0);

	// Scale all of the vertices up to make the radius correct
	// Also, include the s term that was not included above
	int i;
	for (i = 0; i < 12; i++)
		vertex[i] *= radius / s;
#else
	// To avoid performance issues, don't let the resolution go above 3
	if (resolution > 3)
		resolution = 3;

	// Find six vertices that define an octohedron circumscribed within the sphere
	Eigen::Vector3d top(0.0, 0.0, radius), bottom(0.0, 0.0, -radius);
	Eigen::Vector3d equator1(radius, 0.0, 0.0), equator2(0.0, radius, 0.0);
	Eigen::Vector3d equator3(-radius, 0.0, 0.0), equator4(0.0, -radius, 0.0);
#endif

	glPushMatrix();

		glTranslated(center.x(), center.y(), center.z());

		glBegin(GL_TRIANGLES);

#ifdef ICOSOHEDRON
		// Begin recursive subdivision of all twenty faces of the icosohedron
		RecursiveSubdivision(vertex[0], vertex[8], vertex[4], resolution);
		RecursiveSubdivision(vertex[0], vertex[5], vertex[10], resolution);
		RecursiveSubdivision(vertex[2], vertex[4], vertex[9], resolution);
		RecursiveSubdivision(vertex[2], vertex[11], vertex[5], resolution);
		RecursiveSubdivision(vertex[1], vertex[6], vertex[8], resolution);

		RecursiveSubdivision(vertex[1], vertex[10], vertex[7], resolution);
		RecursiveSubdivision(vertex[3], vertex[9], vertex[6], resolution);
		RecursiveSubdivision(vertex[3], vertex[7], vertex[11], resolution);
		RecursiveSubdivision(vertex[0], vertex[10], vertex[8], resolution);
		RecursiveSubdivision(vertex[1], vertex[8], vertex[10], resolution);

		RecursiveSubdivision(vertex[2], vertex[9], vertex[11], resolution);
		RecursiveSubdivision(vertex[3], vertex[11], vertex[9], resolution);
		RecursiveSubdivision(vertex[4], vertex[2], vertex[0], resolution);
		RecursiveSubdivision(vertex[5], vertex[0], vertex[2], resolution);
		RecursiveSubdivision(vertex[6], vertex[1], vertex[3], resolution);

		RecursiveSubdivision(vertex[7], vertex[3], vertex[1], resolution);
		RecursiveSubdivision(vertex[8], vertex[6], vertex[4], resolution);
		RecursiveSubdivision(vertex[9], vertex[4], vertex[6], resolution);
		RecursiveSubdivision(vertex[10], vertex[5], vertex[7], resolution);
		RecursiveSubdivision(vertex[11], vertex[7], vertex[5], resolution);
#else
		// Begin recursive subdivision of all eight faces of the octohedron
		RecursiveSubdivision(top, equator1, equator4, resolution);
		RecursiveSubdivision(top, equator2, equator1, resolution);
		RecursiveSubdivision(top, equator3, equator2, resolution);
		RecursiveSubdivision(top, equator4, equator3, resolution);

		RecursiveSubdivision(bottom, equator1, equator2, resolution);
		RecursiveSubdivision(bottom, equator2, equator3, resolution);
		RecursiveSubdivision(bottom, equator3, equator4, resolution);
		RecursiveSubdivision(bottom, equator4, equator1, resolution);
#endif

		glEnd();

	glPopMatrix();
}

//==========================================================================
// Class:			Sphere
// Function:		RecursiveSubdivision
//
// Description:		Recursive method for sub-dividing a triangle into four
//					smaller triangles.  The number of recursive calls determines
//					the final resolution of the sphere.
//
// Input Arguments:
//		corner1	= const Eigen::Vector3d& specifying the first corner of the triangle
//		corner2	= const Eigen::Vector3d& specifying the second corner of the triangle
//		corner3	= const Eigen::Vector3d& specifying the third corner of the triangle
//		level	= int specifying remaining number of recursive calls
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//==========================================================================
void Sphere::RecursiveSubdivision(const Eigen::Vector3d &corner1, const Eigen::Vector3d &corner2,
	const Eigen::Vector3d &corner3, int level)
{
	// If level is less than 1, add the triangle to the scene instead of
	// continuing with the sub-division
	if (level < 1)
	{
		AddVertex(corner1);
		AddVertex(corner2);
		AddVertex(corner3);

		return;
	}

	// Compute the three points that divide this triangle into four sub-triangles
	// The division works like this:
	/*-----------------------
	      Corner 1
	         /\
	        /  \
	       /    \
	   MP1/\----/\MP2
	     /  \  /  \
	    /    \/MP3 \
	    ------------
	   Corner 2    Corner 3
	-----------------------*/
	Eigen::Vector3d midPoint1 = corner1 + (corner2 - corner1).normalized() * (corner1 - corner2).norm() / 2.0;
	Eigen::Vector3d midPoint2 = corner1 + (corner3 - corner1).normalized() * (corner1 - corner3).norm() / 2.0;
	Eigen::Vector3d midPoint3 = corner3 + (corner2 - corner3).normalized() * (corner3 - corner2).norm() / 2.0;

	// These locations now need to be normalized such that they lie at a
	// distance of 'radius' from the center
	midPoint1 = midPoint1.normalized() * radius;
	midPoint2 = midPoint2.normalized() * radius;
	midPoint3 = midPoint3.normalized() * radius;

	// Call this method for each of the four sub-triangles, with one less level
	// Note that the order in which the points are supplied is important to make
	// sure that the triangles are created in a consistent manner (clock-wise).
	level--;
	RecursiveSubdivision(corner1, midPoint2, midPoint1, level);
	RecursiveSubdivision(corner2, midPoint1, midPoint3, level);
	RecursiveSubdivision(corner3, midPoint3, midPoint2, level);
	RecursiveSubdivision(midPoint1, midPoint2, midPoint3, level);
}

//==========================================================================
// Class:			Sphere
// Function:		AddVertex
//
// Description:		Computes and sets the normal vector, then adds the
//					specified vertex to the OpenGL call list.
//
// Input Arguments:
//		vertex	= const Eigen::Vector3d& to be added
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Sphere::AddVertex(const Eigen::Vector3d &vertex)
{
	// Compute and set the normal
	Eigen::Vector3d normal = vertex.normalized();
	glNormal3d(normal.x(), normal.y(), normal.z());

	// Add the vertex
	glVertex3d(vertex.x(), vertex.y(), vertex.z());
}

//==========================================================================
// Class:			Sphere
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
bool Sphere::HasValidParameters()
{
	// Spheres must have a positive radius
	if (radius > 0.0)
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			Sphere
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the sphere.
//
// Input Arguments:
//		resolution	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Sphere::SetResolution(const int &resolution)
{
	this->resolution = resolution;
	mModified = true;
}

//==========================================================================
// Class:			Sphere
// Function:		SetCenter
//
// Description:		Sets the location of the center of the sphere.
//
// Input Arguments:
//		center	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Sphere::SetCenter(const Eigen::Vector3d &center)
{
	this->center = center;
	mModified = true;
}

//==========================================================================
// Class:			Sphere
// Function:		SetRadius
//
// Description:		Sets the radius at the base of the sphere.
//
// Input Arguments:
//		radius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Sphere::SetRadius(const double &radius)
{
	this->radius = radius;
	mModified = true;
}

//==========================================================================
// Class:			Sphere
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
bool Sphere::IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	assert(VVASE::Math::IsZero(1.0 - direction.norm()));

	if (pow((point - center).norm(), 2) > pow(direction.dot(point - center), 2) + radius * radius)
		return false;

	return true;
}

void Sphere::Update(const unsigned int& i)
{
}

}// namespace VVASE
