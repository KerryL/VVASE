/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  sphere.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating spherical objects.
// History:
//	6/2/2009	- Modified GenerateGeometry() to make use of openGL matricies for positioning
//				  and orienting the object, K.Loux.
//	6/2/2009	- Changed maximum resolution from 5 to 3 - it turns out spheres were the cause
//				  of the dramatic slow-down between VTK and the new code, with the recursive calls
//				  being the culprit, K. Loux.
//	6/3/2009	- Added icosohedron for use in lieu of octohedron as base polygon to begin
//				  subdivision, K. Loux.

// Local headers
#include "vRenderer/primitives/sphere.h"
#include "vRenderer/render_window_class.h"
#include "vUtilities/convert_class.h"

// For choosing between an octohedron (8 sides) or a icosohedron (20 sides) as the base polygon
// for the recursive subdivision to create the sphere.
#define ICOSOHEDRON

//==========================================================================
// Class:			SPHERE
// Function:		SPHERE
//
// Description:		Constructor for the SPHERE class.
//
// Input Argurments:
//		_RenderWindow	= RENDER_WINDOW& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
SPHERE::SPHERE(RENDER_WINDOW &_RenderWindow) : PRIMITIVE(_RenderWindow)
{
	// Initialize the private data
	Center.Set(0.0,0.0,0.0);
	Radius = 0.0;
	Resolution = 4;
}

//==========================================================================
// Class:			SPHERE
// Function:		~SPHERE
//
// Description:		Destructor for the SPHERE class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
SPHERE::~SPHERE()
{
}

//==========================================================================
// Class:			SPHERE
// Function:		GenerateGeometry
//
// Description:		Creates the OpenGL instructions to create this object in
//					the scene.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void SPHERE::GenerateGeometry(void)
{
	// Resolution must be at least 0
	if (Resolution < 0)
		Resolution = 0;

#ifdef ICOSOHEDRON
	// To avoid performance issues, don't let the resolution go above 2
	if (Resolution > 2)
		Resolution = 2;

	// Find twelve vertecies that define an icosohedron cirumscribed within the sphere
	double t = (1.0 + sqrt(5.0)) / 2.0;
	double s = sqrt(1 + t * t);
	VECTOR Vertex[12];
	Vertex[0].Set(t, 1.0, 0.0);
	Vertex[1].Set(-t, 1.0, 0.0);
	Vertex[2].Set(t, -1.0, 0.0);
	Vertex[3].Set(-t, -1.0, 0.0);
	Vertex[4].Set(1.0, 0.0, t);
	Vertex[5].Set(1.0, 0.0, -t);
	Vertex[6].Set(-1, 0.0, t);
	Vertex[7].Set(-1.0, 0.0, -t);
	Vertex[8].Set(0.0, t, 1.0);
	Vertex[9].Set(0.0, -t, 1.0);
	Vertex[10].Set(0.0, t, -1.0);
	Vertex[11].Set(0.0, -t, -1.0);

	// Scale all of the vertecies up to make the radius correct
	// Also, include the s term that was not included above
	int i;
	for (i = 0; i < 12; i++)
		Vertex[i] *= Radius / s;
#else
	// To avoid performance issues, don't let the resolution go above 3
	if (Resolution > 3)
		Resolution = 3;

	// Find six vertecies that define an octohedron circumscribed within the sphere
	VECTOR Top(0.0, 0.0, Radius), Bottom(0.0, 0.0, -Radius);
	VECTOR Equator1(Radius, 0.0, 0.0), Equator2(0.0, Radius, 0.0);
	VECTOR Equator3(-Radius, 0.0, 0.0), Equator4(0.0, -Radius, 0.0);
#endif

	// Push the current matrix
	glPushMatrix();

		// Translate the current matrix
		glTranslated(Center.X, Center.Y, Center.Z);

		// Begin the triangles sequence
		glBegin(GL_TRIANGLES);

#ifdef ICOSOHEDRON
		// Begin recursive subdivision of all twenty faces of the icosohedron
		RecursiveSubdivision(Vertex[0], Vertex[8], Vertex[4], Resolution);
		RecursiveSubdivision(Vertex[0], Vertex[5], Vertex[10], Resolution);
		RecursiveSubdivision(Vertex[2], Vertex[4], Vertex[9], Resolution);
		RecursiveSubdivision(Vertex[2], Vertex[11], Vertex[5], Resolution);
		RecursiveSubdivision(Vertex[1], Vertex[6], Vertex[8], Resolution);

		RecursiveSubdivision(Vertex[1], Vertex[10], Vertex[7], Resolution);
		RecursiveSubdivision(Vertex[3], Vertex[9], Vertex[6], Resolution);
		RecursiveSubdivision(Vertex[3], Vertex[7], Vertex[11], Resolution);
		RecursiveSubdivision(Vertex[0], Vertex[10], Vertex[8], Resolution);
		RecursiveSubdivision(Vertex[1], Vertex[8], Vertex[10], Resolution);

		RecursiveSubdivision(Vertex[2], Vertex[9], Vertex[11], Resolution);
		RecursiveSubdivision(Vertex[3], Vertex[11], Vertex[9], Resolution);
		RecursiveSubdivision(Vertex[4], Vertex[2], Vertex[0], Resolution);
		RecursiveSubdivision(Vertex[5], Vertex[0], Vertex[2], Resolution);
		RecursiveSubdivision(Vertex[6], Vertex[1], Vertex[3], Resolution);

		RecursiveSubdivision(Vertex[7], Vertex[3], Vertex[1], Resolution);
		RecursiveSubdivision(Vertex[8], Vertex[6], Vertex[4], Resolution);
		RecursiveSubdivision(Vertex[9], Vertex[4], Vertex[6], Resolution);
		RecursiveSubdivision(Vertex[10], Vertex[5], Vertex[7], Resolution);
		RecursiveSubdivision(Vertex[11], Vertex[7], Vertex[5], Resolution);
#else
		// Begin recursive subdivision of all eight faces of the octohedron
		RecursiveSubdivision(Top, Equator1, Equator4, Resolution);
		RecursiveSubdivision(Top, Equator2, Equator1, Resolution);
		RecursiveSubdivision(Top, Equator3, Equator2, Resolution);
		RecursiveSubdivision(Top, Equator4, Equator3, Resolution);

		RecursiveSubdivision(Bottom, Equator1, Equator2, Resolution);
		RecursiveSubdivision(Bottom, Equator2, Equator3, Resolution);
		RecursiveSubdivision(Bottom, Equator3, Equator4, Resolution);
		RecursiveSubdivision(Bottom, Equator4, Equator1, Resolution);
#endif

		// Complete the triangles
		glEnd();

	// Pop the matrix
	glPopMatrix();

	return;
}

//==========================================================================
// Class:			SPHERE
// Function:		RecursiveSubdivision
//
// Description:		Recursive method for sub-dividing a triangle into four
//					smaller triangles.  The number of recursive calls determines
//					the final resolution of the sphere.
//
// Input Argurments:
//		Corner1	= const VECTOR& specifying the first corner of the triangle
//		Corner2	= const VECTOR& specifying the second corner of the triangle
//		Corner3	= const VECTOR& specifying the third corner of the triangle
//		Level	= int specifying remaining number of recursive calls
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//==========================================================================
void SPHERE::RecursiveSubdivision(const VECTOR &Corner1, const VECTOR &Corner2,
								  const VECTOR &Corner3, int Level)
{
	// If level is less than 1, add the triangle to the scene instead of
	// continuing with the sub-division
	if (Level < 1)
	{
		AddVertex(Corner1);
		AddVertex(Corner2);
		AddVertex(Corner3);

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
	VECTOR MidPoint1 = Corner1 + (Corner2 - Corner1).Normalize() * Corner1.Distance(Corner2) / 2.0;
	VECTOR MidPoint2 = Corner1 + (Corner3 - Corner1).Normalize() * Corner1.Distance(Corner3) / 2.0;
	VECTOR MidPoint3 = Corner3 + (Corner2 - Corner3).Normalize() * Corner3.Distance(Corner2) / 2.0;

	// These locations now need to be normalized such that they lie at a
	// distance Radius from the center
	MidPoint1 = MidPoint1.Normalize() * Radius;
	MidPoint2 = MidPoint2.Normalize() * Radius;
	MidPoint3 = MidPoint3.Normalize() * Radius;

	// Call this method for each of the four sub-triangles, with one less level
	// Note that the order in which the points are supplied is important to make
	// sure that the triangles are created in a consistent manner (clock-wise).
	Level--;
	RecursiveSubdivision(Corner1, MidPoint2, MidPoint1, Level);
	RecursiveSubdivision(Corner2, MidPoint1, MidPoint3, Level);
	RecursiveSubdivision(Corner3, MidPoint3, MidPoint2, Level);
	RecursiveSubdivision(MidPoint1, MidPoint2, MidPoint3, Level);

	return;
}

//==========================================================================
// Class:			SPHERE
// Function:		AddVertex
//
// Description:		Computes and sets the normal vector, then adds the
//					specified vertex to the OpenGL call list.
//
// Input Argurments:
//		Vertex	= const VECTOR& to be added
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void SPHERE::AddVertex(const VECTOR &Vertex)
{
	// Compute and set the normal
	VECTOR Normal = Vertex.Normalize();
	glNormal3d(Normal.X, Normal.Y, Normal.Z);

	// Add the vertex
	glVertex3d(Vertex.X, Vertex.Y, Vertex.Z);
}

//==========================================================================
// Class:			SPHERE
// Function:		HasValidParameters
//
// Description:		Checks to see if the information about this object is
//					valid and complete (gives permission to create the object).
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//==========================================================================
bool SPHERE::HasValidParameters(void)
{
	// Spheres must have a positive radius
	if (Radius > 0.0)
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			SPHERE
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the sphere.
//
// Input Argurments:
//		_Resolution	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void SPHERE::SetResolution(const int &_Resolution)
{
	// Set the resolution to the argument
	Resolution = _Resolution;
	
	// Reset the modified flag
	Modified = true;

	return;
}

//==========================================================================
// Class:			SPHERE
// Function:		SetCenter
//
// Description:		Sets the location of the center of the sphere.
//
// Input Argurments:
//		_Center	= const VECTOR&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void SPHERE::SetCenter(const VECTOR &_Center)
{
	// Set the center point to the argument
	Center = _Center;
	
	// Reset the modified flag
	Modified = true;

	return;
}

//==========================================================================
// Class:			SPHERE
// Function:		SetRadius
//
// Description:		Sets the radius at the base of the sphere.
//
// Input Argurments:
//		_Radius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void SPHERE::SetRadius(const double &_Radius)
{
	// Set the radius to the argument
	Radius = _Radius;
	
	// Reset the modified flag
	Modified = true;

	return;
}