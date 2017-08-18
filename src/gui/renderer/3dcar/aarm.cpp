/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  aarm.cpp
// Date:  5/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class definition for the AArm class.

// Local headers
#include "VVASE/gui/renderer/3dcar/aarm.h"
#include "VVASE/gui/renderer/primitives/sphere.h"
#include "VVASE/gui/renderer/primitives/cylinder.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/color.h>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

//==========================================================================
// Class:			AArm
// Function:		AArm
//
// Description:		Constructor for the AArm class.  Performs the entire
//					process necessary to add the object to the scene.
//
// Input Arguments:
//		renderer	= RenderWindow&, pointer to rendering object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
AArm::AArm(LibPlot2D::RenderWindow &renderer)
{
	// Create the actors
	endPoint1 = new Sphere(renderer);
	endPoint2 = new Sphere(renderer);
	midPoint = new Sphere(renderer);
	member1 = new Cylinder(renderer);
	member2 = new Cylinder(renderer);

	// Set up the Actors
	endPoint1->SetColor(LibPlot2D::Color::ColorWhite);
	midPoint->SetColor(LibPlot2D::Color::ColorWhite);
	endPoint2->SetColor(LibPlot2D::Color::ColorWhite);
}

//==========================================================================
// Class:			AArm
// Function:		Update
//
// Description:		Updates the position and size of the a-arm in the scene
//
// Input Arguments:
//		end1		= const Eigen::Vector3d&, location of one end of the a-arm
//		middle		= const Eigen::Vector3d&, location of the ball joint on the a-arm
//		end2		= const Eigen::Vector3d&, location of the other end of the a-arm
//		diameter	= const double& describing the size of the tubing representing
//					  the arm
//		resolution	= const integer& representing the number of planar sides to use
//					  to represent the cylindrical members
//		color		= const Color& describing the color to paint this object
//		show		= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void AArm::Update(const Eigen::Vector3d &end1, const Eigen::Vector3d &middle, const Eigen::Vector3d &end2,
	const double &diameter, const int &resolution, const LibPlot2D::Color &color, bool show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASE::Math::IsNaN(end1) || VVASE::Math::IsNaN(middle) || VVASE::Math::IsNaN(end2))
		show = false;

	// Make sure we want this tire to be visible
	if (show)
	{
		// Set visibility ON
		endPoint1->SetVisibility(true);
		midPoint->SetVisibility(true);
		endPoint2->SetVisibility(true);
		member1->SetVisibility(true);
		member2->SetVisibility(true);
	}
	else
	{
		// Set visibility OFF
		endPoint1->SetVisibility(false);
		midPoint->SetVisibility(false);
		endPoint2->SetVisibility(false);
		member1->SetVisibility(false);
		member2->SetVisibility(false);

		return;
	}

	// Set this object's color
	member1->SetColor(color);
	member2->SetColor(color);

	// Set up the spheres
	endPoint1->SetRadius(diameter / 2.0 * 1.1);// Make the points just a smidge larger than the members
	midPoint->SetRadius(diameter / 2.0 * 1.1);
	endPoint2->SetRadius(diameter / 2.0 * 1.1);

	endPoint1->SetResolution(resolution);
	midPoint->SetResolution(resolution);
	endPoint2->SetResolution(resolution);

	// Set up the CylinderSources
	member1->SetRadius(diameter / 2.0);
	member1->SetEndPoint1(end1);
	member1->SetEndPoint2(middle);
	member1->SetResolution(resolution);

	member2->SetRadius(diameter / 2.0);
	member2->SetEndPoint1(end2);
	member2->SetEndPoint2(middle);
	member2->SetResolution(resolution);

	// Set the position of the spheres
	endPoint1->SetCenter(end1);
	midPoint->SetCenter(middle);
	endPoint2->SetCenter(end2);
}

//==========================================================================
// Class:			AArm
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
bool AArm::ContainsThisActor(const LibPlot2D::Primitive *actor)
{
	// Make the comparison
	if (endPoint1 == actor ||
		member1 == actor ||
		midPoint == actor ||
		member2 == actor ||
		endPoint2 == actor)
		return true;
	else
		return false;
}

//==========================================================================
// Class:			AArm
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
Eigen::Vector3d AArm::FindClosestPoint(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	Eigen::Vector3d endPoint1Center(endPoint1->GetCenter());
	Eigen::Vector3d endPoint2Center(endPoint2->GetCenter());
	Eigen::Vector3d midPointCenter(midPoint->GetCenter());
	Eigen::Vector3d endPoint1Test(VVASE::Math::NearestPointOnAxis(point, direction, endPoint1Center));
	Eigen::Vector3d endPoint2Test(VVASE::Math::NearestPointOnAxis(point, direction, endPoint2Center));
	Eigen::Vector3d midPointTest(VVASE::Math::NearestPointOnAxis(point, direction, midPointCenter));

	if ((endPoint1Center - endPoint1Test).norm() < (endPoint2Center - endPoint2Test).norm())
	{
		if ((endPoint1Center - endPoint1Test).norm() < (midPointCenter - midPointTest).norm())
			return endPoint1Center;
		else
			return midPointCenter;
	}
	else if ((endPoint2Center - endPoint2Test).norm() < (midPointCenter - midPointTest).norm())
		return endPoint2Center;

	return midPointCenter;
}

}// namespace VVASE
