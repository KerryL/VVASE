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
#include "VVASE/gui/renderer/primitives/sphere.h"
#include "VVASE/gui/renderer/primitives/cylinder.h"
#include "VVASE/gui/renderer/3dcar/link.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/color.h>

// Eigen headers
#include <Eigen/Eigen>

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
//		renderer	= LibPlot2D::RenderWindow&, pointer to rendering object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Link::Link(LibPlot2D::RenderWindow &renderer)
{
	member = new Cylinder(renderer);
	endPoint1 = new Sphere(renderer);
	endPoint2 = new Sphere(renderer);

	endPoint1->SetColor(LibPlot2D::Color::ColorWhite);
	endPoint2->SetColor(LibPlot2D::Color::ColorWhite);
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
//		color		= const LibPlot2D::Color& describing this object's color
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
				  const int &resolution, const LibPlot2D::Color &color, bool show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASE::Math::IsNaN(end1) || VVASE::Math::IsNaN(end2))
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
//		actor	= const LibPlot2D::Primitive* to compare against this object's actors
//
// Output Arguments:
//		None
//
// Return Value:
//		bool representing whether or not the Actor was part of this object
//
//==========================================================================
bool Link::ContainsThisActor(const LibPlot2D::Primitive *actor)
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
	Eigen::Vector3d endPoint1Test(VVASE::Math::NearestPointOnAxis(point, direction, endPoint1Center));
	Eigen::Vector3d endPoint2Test(VVASE::Math::NearestPointOnAxis(point, direction, endPoint2Center));

	if ((endPoint1Center - endPoint1Test).norm() < (endPoint2Center - endPoint2Test).norm())
		return endPoint1Center;

	return endPoint2Center;
}

}// namespace VVASE
