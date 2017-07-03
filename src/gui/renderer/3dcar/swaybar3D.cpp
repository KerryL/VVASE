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
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/3dcar/swaybar3D.h"
#include "vRenderer/color.h"
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"

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
//		renderer	= RenderWindow&, pointer to rendering object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Swaybar3D::Swaybar3D(RenderWindow &renderer)
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
// Function:		~Swaybar3D
//
// Description:		Destructor for the Swaybar3D class.
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
Swaybar3D::~Swaybar3D()
{
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
//		color					= const Color& describing this object's color
//		show					= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Swaybar3D::Update(const Eigen::Vector3d &rightLink, const Eigen::Vector3d &leftLink, const Eigen::Vector3d &torsionMemberTopRight,
					   const Eigen::Vector3d &torsionMemberBottomLeft,  const Eigen::Vector3d &midPoint, const Eigen::Vector3d &axisPivot,
					   const Suspension::BarStyle &barStyle, const double &dimension,
					   const int &resolution, const Color &color, bool show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(rightLink) || VVASEMath::IsNaN(leftLink) ||
		VVASEMath::IsNaN(torsionMemberTopRight) || VVASEMath::IsNaN(torsionMemberBottomLeft))
		show = false;

	// Check to make sure the sway bar exists
	if (barStyle == Suspension::SwayBarNone ||
		barStyle == Suspension::SwayBarGeared)// FIXME:  Geared bars not yet implemented!
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

	if (barStyle == Suspension::SwayBarUBar)
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
	else if (barStyle == Suspension::SwayBarTBar)
	{
		Eigen::Vector3d stemPlaneNormal = (midPoint - axisPivot).Normalize();
		Eigen::Vector3d topMidPoint = VVASEMath::IntersectWithPlane(stemPlaneNormal, midPoint,
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
//		actor	= const Primitive* to compare against this object's actors
//
// Output Arguments:
//		None
//
// Return Value:
//		bool representing whether or not the Actor was part of this object
//
//==========================================================================
bool Swaybar3D::ContainsThisActor(const Primitive *actor)
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
