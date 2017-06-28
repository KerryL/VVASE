/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  plane3D.cpp
// Date:  5/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class definition for the Plane3D class.

// Local headers
#include "vRenderer/primitives/quadrilateral.h"
#include "vRenderer/3dcar/plane3D.h"
#include "vRenderer/color.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			Plane3D
// Function:		Plane3D
//
// Description:		Constructor for the Plane3D class.  Performs the entire
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
Plane3D::Plane3D(RenderWindow &renderer)
{
	plane = new Quadrilateral(renderer);

	Vector xAxis(1.0, 0.0, 0.0);
	Vector normal(0.0, 0.0, 1.0);
	plane->SetAxis(xAxis);
	plane->SetNormal(normal);
}

//==========================================================================
// Class:			Plane3D
// Function:		~Plane3D
//
// Description:		Destructor for the Plane3D class.
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
Plane3D::~Plane3D()
{
}

//==========================================================================
// Class:			Plane3D
// Function:		Update
//
// Description:		Updates the position and size of the plane in the scene
//
// Input Arguments:
//		length	= const double&, length of the plane in the x-direction
//		width	= const double&, length of the plane in the y-direction
//		xOffset	= const double&, offset of the center of the plane in the
//				  x-direction
//		color	= const Color& describing this object's color
//		show	= const bool&, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Plane3D::Update(const double &length, const double &width, const double &xOffset,
				   const Color &color, const bool &show)
{
	// Set the visibility flag
	plane->SetVisibility(show);

	// Make sure we want this to be visible before continuing
	if (!show)
		return;

	// Set this object's color
	plane->SetColor(color);

	// Set the correct position and size of the plane
	Vector center(xOffset, 0.0, 0.0);
	plane->SetCenter(center);
	plane->SetLength(length);
	plane->SetWidth(width);
}

//==========================================================================
// Class:			Plane3D
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
//		bool representing whether or not the actor was part of this object
//
//==========================================================================
bool Plane3D::ContainsThisActor(const Primitive *actor)
{
	if (plane == actor)
		return true;
	else
		return false;
}
