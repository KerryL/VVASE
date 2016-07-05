/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  sphere.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from Primitive for creating spherical objects.
// History:
//	6/2/2009	- Modified GenerateGeometry() to make use of openGL matrices for positioning
//				  and orienting the object, K.Loux.
//	6/2/2009	- Changed maximum resolution from 5 to 3 - it turns out spheres were the cause
//				  of the dramatic slow-down between VTK and the new code, with the recursive calls
//				  being the culprit, K. Loux.
//	6/3/2009	- Added icosohedron for use in lieu of octohedron as base polygon to begin
//				  subdivision, K. Loux.

// Standard C++ headers
#include <cassert>

// Local headers
#include "vRenderer/primitives/sphere.h"
#include "vRenderer/renderWindow.h"
#include "vUtilities/unitConverter.h"
#include "vMath/carMath.h"

// For choosing between an octohedron (8 sides) or a icosohedron (20 sides) as the base polygon
// for the recursive subdivision to create the sphere.
#define ICOSOHEDRON

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
Sphere::Sphere(RenderWindow &renderWindow) : Primitive(renderWindow)
{
	center.Set(0.0, 0.0, 0.0);
	radius = 0.0;
	resolution = 4;
}

//==========================================================================
// Class:			Sphere
// Function:		~Sphere
//
// Description:		Destructor for the Sphere class.
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
Sphere::~Sphere()
{
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
	Vector vertex[12];
	vertex[0].Set(t, 1.0, 0.0);
	vertex[1].Set(-t, 1.0, 0.0);
	vertex[2].Set(t, -1.0, 0.0);
	vertex[3].Set(-t, -1.0, 0.0);
	vertex[4].Set(1.0, 0.0, t);
	vertex[5].Set(1.0, 0.0, -t);
	vertex[6].Set(-1, 0.0, t);
	vertex[7].Set(-1.0, 0.0, -t);
	vertex[8].Set(0.0, t, 1.0);
	vertex[9].Set(0.0, -t, 1.0);
	vertex[10].Set(0.0, t, -1.0);
	vertex[11].Set(0.0, -t, -1.0);

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
	Vector top(0.0, 0.0, radius), bottom(0.0, 0.0, -radius);
	Vector equator1(radius, 0.0, 0.0), equator2(0.0, radius, 0.0);
	Vector equator3(-radius, 0.0, 0.0), equator4(0.0, -radius, 0.0);
#endif

	glPushMatrix();

		glTranslated(center.x, center.y, center.z);

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
//		corner1	= const Vector& specifying the first corner of the triangle
//		corner2	= const Vector& specifying the second corner of the triangle
//		corner3	= const Vector& specifying the third corner of the triangle
//		level	= int specifying remaining number of recursive calls
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//==========================================================================
void Sphere::RecursiveSubdivision(const Vector &corner1, const Vector &corner2,
	const Vector &corner3, int level)
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
	Vector midPoint1 = corner1 + (corner2 - corner1).Normalize() * corner1.Distance(corner2) / 2.0;
	Vector midPoint2 = corner1 + (corner3 - corner1).Normalize() * corner1.Distance(corner3) / 2.0;
	Vector midPoint3 = corner3 + (corner2 - corner3).Normalize() * corner3.Distance(corner2) / 2.0;

	// These locations now need to be normalized such that they lie at a
	// distance of 'radius' from the center
	midPoint1 = midPoint1.Normalize() * radius;
	midPoint2 = midPoint2.Normalize() * radius;
	midPoint3 = midPoint3.Normalize() * radius;

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
//		vertex	= const Vector& to be added
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Sphere::AddVertex(const Vector &vertex)
{
	// Compute and set the normal
	Vector normal = vertex.Normalize();
	glNormal3d(normal.x, normal.y, normal.z);

	// Add the vertex
	glVertex3d(vertex.x, vertex.y, vertex.z);
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
	modified = true;
}

//==========================================================================
// Class:			Sphere
// Function:		SetCenter
//
// Description:		Sets the location of the center of the sphere.
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
void Sphere::SetCenter(const Vector &center)
{
	this->center = center;
	modified = true;
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
	modified = true;
}

//==========================================================================
// Class:			Sphere
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
bool Sphere::IsIntersectedBy(const Vector& point, const Vector& direction) const
{
	assert(VVASEMath::IsZero(1.0 - direction.Length()));

	if (pow((point - center).Length(), 2) > pow(direction * (point - center), 2) + radius * radius)
		return false;

	return true;
}
