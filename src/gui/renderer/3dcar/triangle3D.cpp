/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  triangle3D.cpp
// Date:  1/5/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class definition for the Triangle3D class.  This is used for
//        bell cranks, uprights, etc.

// Local headers
#include "vRenderer/primitives/triangle.h"
#include "vRenderer/3dcar/triangle3D.h"
#include "vRenderer/color.h"
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"

namespace VVASE
{

//==========================================================================
// Class:			Triangle3D
// Function:		Triangle3D
//
// Description:		Constructor for the Triangle3D class.  Performs the entire
//					process necessary to add the object to the scene.
//
// Input Arguments:
//		renderer	= &RenderWindow, pointer to rendering object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Triangle3D::Triangle3D(RenderWindow &renderer)
{
	triangle = new Triangle(renderer);
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
//		node1		= const Eigen::Vector3d&, location of the first vertex
//		node2		= const Eigen::Vector3d&, location of the second vertex
//		node3		= const Eigen::Vector3d&, location of the third vertex
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
void Triangle3D::Update(const Eigen::Vector3d &node1, const Eigen::Vector3d &node2, const Eigen::Vector3d &node3,
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

}// namespace VVASE
