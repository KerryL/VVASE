/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  link.cpp
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class definition for the Link class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/sphere.h"
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/3dcar/link.h"
#include "vRenderer/color.h"
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			Link
// Function:		Link
//
// Description:		Constructor for the Link class.  Performs the entire
//					process necessary to add the object to the scene.
//
// Input Arguments:
//		_renderer	= RenderWindow&, pointer to rendering object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Link::Link(RenderWindow &renderer)
{
	member = new Cylinder(renderer);
	endPoint1 = new Sphere(renderer);
	endPoint2 = new Sphere(renderer);

	endPoint1->SetColor(Color::ColorWhite);
	endPoint2->SetColor(Color::ColorWhite);
}

//==========================================================================
// Class:			Link
// Function:		~Link
//
// Description:		Destructor for the Link class.
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
Link::~Link()
{
}

//==========================================================================
// Class:			Link
// Function:		Update
//
// Description:		Updates the position and size of the link in the scene
//
// Input Arguments:
//		end1		= const Vector&, location of one end of the a-arm
//		end2		= const Vector&, location of the other end of the a-arm
//		radius		= const double& describing the size of the tubing representing
//					  the arm
//		resolution	= const integer& representing the number of planar sides to use
//					  to represent the cylindrical members
//		color		= const Color& describing this object's color
//		show		= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Link::Update(const Vector &end1, const Vector &end2, const double &diameter,
				  const int &resolution, const Color &color, bool show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(end1) || VVASEMath::IsNaN(end2))
		show = false;

	// Set the visibility flag
	member->SetVisibility(show);
	endPoint1->SetVisibility(show);
	endPoint2->SetVisibility(show);

	// Make sure we want this to be visible before continuing
	if (!show)
		return;

	// Set this object's color
	member->SetColor(color);

	// Set up the spheres
	endPoint1->SetRadius(diameter / 2.0 * 1.1);// Make the points just a smidge larger than the members
	endPoint1->SetResolution(resolution);
	endPoint2->SetRadius(diameter / 2.0 * 1.1);// Make the points just a smidge larger than the members
	endPoint2->SetResolution(resolution);

	// Set up the cylinder
	member->SetRadius(diameter / 2.0);
	member->SetResolution(resolution);

	// Set the positions of the spheres and the cylinder
	endPoint1->SetCenter(end1);
	endPoint2->SetCenter(end2);
	member->SetEndPoint1(end1);
	member->SetEndPoint2(end2);
}

//==========================================================================
// Class:			Link
// Function:		ContainsThisActor
//
// Description:		Compares the argument with the actors that make up this
//					object to determine if the argument actor is part of this
//					object or not.
//
// Input Arguments:
//		actor	= const Primitive* to compare against this object's actors
//
// Output Arguments:
//		None
//
// Return Value:
//		bool representing whether or not the Actor was part of this object
//
//==========================================================================
bool Link::ContainsThisActor(const Primitive *actor)
{
	// Make the comparison
	if (endPoint1 == actor ||
		endPoint2 == actor ||
		member == actor)
		return true;
	else
		return false;
}