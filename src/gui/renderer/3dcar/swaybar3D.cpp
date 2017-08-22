/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  swaybar3D.cpp
// Date:  1/11/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class definition for the Swaybar3D class.

// Local headers
#include "VVASE/gui/renderer/primitives/cylinder.h"
#include "VVASE/gui/renderer/3dcar/swaybar3D.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/color.h>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

//==========================================================================
// Class:			Swaybar3D
// Function:		Swaybar3D
//
// Description:		Constructor for the Swaybar3D class.  Performs the entire
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
Swaybar3D::Swaybar3D(LibPlot2D::RenderWindow &renderer)
{
	torqueArm1 = new Cylinder(renderer);
	torqueArm2 = new Cylinder(renderer);
	torsionMember = new Cylinder(renderer);

	torqueArm1->SetCapping(true);
	torqueArm2->SetCapping(true);
	torsionMember->SetCapping(true);

	// We set it to four to make it look like a rectangular prism
	torqueArm1->SetResolution(4);
	torqueArm2->SetResolution(4);
}

//==========================================================================
// Class:			Swaybar3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the tire
//					in the scene.  Note that the third argument is passed by
//					value, not reference like the first two Vectors.
//
// Input Arguments:
//		leftLink				= const Eigen::Vector3d&, point where link meets the left arm
//		rightLink				= const Eigen::Vector3d&, point where link meets the right arm
//		torsionMemberTopRight	= const Eigen::Vector3d&, either the top OR the right end of the
//								  sway bar (depends on bar style)
//		torsionMemberBottomLeft	= const Eigen::Vector3d&, either the bottom OR the left end of the
//								  sway bar (depends on bar style)
//		midPoint				= const Eigen::Vector3d&, pivot point for T-bars (not used for other bars)
//		axisPivot				= const Eigen::Vector3d&, defines pivot axis for T-bars (not used for other bars)
//		barStyle				= const Suspension::BarStyle& defining the type of swaybar
//		dimension				= const double& describing the size of the members
//		resolution				= const integer& representing the number of planar sides to use
//								  to represent the cylinders
//		color					= const LibPlot2D::Color& describing this object's color
//		show					= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Swaybar3D::Update(const Eigen::Vector3d &rightLink, const Eigen::Vector3d &leftLink,
	const Eigen::Vector3d &torsionMemberTopRight, const Eigen::Vector3d &torsionMemberBottomLeft,
	const Eigen::Vector3d &midPoint, const Eigen::Vector3d &axisPivot,
	const Suspension::BarStyle &barStyle, const double &dimension,
	const int &resolution, const LibPlot2D::Color &color, bool show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASE::Math::IsNaN(rightLink) || VVASE::Math::IsNaN(leftLink) ||
		VVASE::Math::IsNaN(torsionMemberTopRight) || VVASE::Math::IsNaN(torsionMemberBottomLeft))
		show = false;

	// Check to make sure the sway bar exists
	if (barStyle == Suspension::BarStyle::None ||
		barStyle == Suspension::BarStyle::Geared)// FIXME:  Geared bars not yet implemented!
		show = false;

	// Set the visibility flags
	torqueArm1->SetVisibility(show);
	torqueArm2->SetVisibility(show);
	torsionMember->SetVisibility(show);

	// Make sure we want this to be visible before continuing
	if (!show)
		return;

	// Set this object's color
	torqueArm1->SetColor(color);
	torqueArm2->SetColor(color);
	torsionMember->SetColor(color);

	// Set the size of the source objects
	torqueArm1->SetRadius(dimension / 2.0);
	torqueArm2->SetRadius(dimension / 2.0);
	torsionMember->SetRadius(dimension / 2.0);

	// Set the resolution of the torsion member
	torsionMember->SetResolution(resolution);

	if (barStyle == Suspension::BarStyle::UBar)
	{
		// Position the torsion member
		torsionMember->SetEndPoint1(torsionMemberBottomLeft);
		torsionMember->SetEndPoint2(torsionMemberTopRight);

		// Position the right torque arm
		torqueArm1->SetEndPoint1(torsionMemberTopRight);
		torqueArm1->SetEndPoint2(rightLink);

		// Position the left torque arm
		torqueArm2->SetEndPoint1(leftLink);
		torqueArm2->SetEndPoint2(torsionMemberBottomLeft);
	}
	else if (barStyle == Suspension::BarStyle::TBar)
	{
		Eigen::Vector3d stemPlaneNormal = (midPoint - axisPivot).normalized();
		Eigen::Vector3d topMidPoint = VVASE::Math::IntersectWithPlane(stemPlaneNormal, midPoint,
			rightLink - leftLink, leftLink);

		// Position the torsion member
		torsionMember->SetEndPoint1(midPoint);
		torsionMember->SetEndPoint2(topMidPoint);

		torqueArm1->SetEndPoint1(rightLink);
		torqueArm1->SetEndPoint2(leftLink);

		// Position the left torque arm
		torqueArm2->SetVisibility(false);
	}
}

//==========================================================================
// Class:			Swaybar3D
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
bool Swaybar3D::ContainsThisActor(const LibPlot2D::Primitive *actor)
{
	// Make the comparison
	if (torqueArm1 == actor ||
		torqueArm2 == actor ||
		torsionMember == actor)
		return true;
	else
		return false;
}

}// namespace VVASE
