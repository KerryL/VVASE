/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  triangle3D.cpp
// Created:  1/5/2009
// Author:  K. Loux
// Description:  Contains class definition for the Triangle3D class.  This is used for
//				 bell cranks, uprights, etc.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/triangle.h"
#include "vRenderer/3dcar/triangle3D.h"
#include "vRenderer/color.h"
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			Triangle3D
// Function:		Triangle3D
//
// Description:		Constructor for the Triangle3D class.  Performs the entire
//					process necessary to add the object to the scene.
//
// Input Arguments:
//		_renderer	= &RenderWindow, pointer to rendering object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Triangle3D::Triangle3D(RenderWindow &_renderer)
{
	// Create the objects
	triangle = new Triangle(_renderer);
}

//==========================================================================
// Class:			Triangle3D
// Function:		~Triangle3D
//
// Description:		Destructor for the Triangle3D class.
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
Triangle3D::~Triangle3D()
{
}

//==========================================================================
// Class:			Triangle3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the tire
//					in the scene.  Note that the third argument is passed by
//					value, not reference like the first two Vectors.
//
// Input Arguments:
//		node1		= const Vector&, location of the first vertex
//		node2		= const Vector&, location of the second vertex
//		node3		= const Vector&, location of the third vertex
//		color		= const Color& specifying the color of this object
//		show		= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Triangle3D::Update(const Vector &node1, const Vector &node2, const Vector &node3,
					  const Color &color, bool show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(node1) || VVASEMath::IsNaN(node2) || VVASEMath::IsNaN(node3))
		show = false;

	// Set the visibility flag
	triangle->SetVisibility(show);

	// Make sure we want this to be visible before continuing
	if (!show)
		return;

	// Set this object's color
	triangle->SetColor(color);

	// Set the locations of the points
	triangle->SetCorner1(node1);
	triangle->SetCorner2(node2);
	triangle->SetCorner3(node3);
}

//==========================================================================
// Class:			Triangle3D
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
bool Triangle3D::ContainsThisActor(const Primitive *actor)
{
	// Make the comparison
	if (triangle == actor)
		return true;
	else
		return false;
}