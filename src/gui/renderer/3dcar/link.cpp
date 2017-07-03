/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  link.cpp
// Date:  5/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class definition for the Link class.

// Local headers
#include "vRenderer/primitives/sphere.h"
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/3dcar/link.h"
#include "vRenderer/color.h"
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"

namespace VVASE
{

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
//		end1		= const Eigen::Vector3d&, location of one end of the a-arm
//		end2		= const Eigen::Vector3d&, location of the other end of the a-arm
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
void Link::Update(const Eigen::Vector3d &end1, const Eigen::Vector3d &end2, const double &diameter,
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

//==========================================================================
// Class:			Link
// Function:		FindClosestPoint
//
// Description:		Finds the closest input point to the specified vector.
//
// Input Arguments:
//		point		= const Eigen::Vector3d&
//		direction	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		Eigen::Vector3d
//
//==========================================================================
Eigen::Vector3d Link::FindClosestPoint(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	Eigen::Vector3d endPoint1Center(endPoint1->GetCenter());
	Eigen::Vector3d endPoint2Center(endPoint2->GetCenter());
	Eigen::Vector3d endPoint1Test(VVASEMath::NearestPointOnAxis(point, direction, endPoint1Center));
	Eigen::Vector3d endPoint2Test(VVASEMath::NearestPointOnAxis(point, direction, endPoint2Center));

	if (endPoint1Center.Distance(endPoint1Test) < endPoint2Center.Distance(endPoint2Test))
		return endPoint1Center;

	return endPoint2Center;
}

}// namespace VVASE
