/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  spring3D.cpp
// Date:  1/5/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class definition for the Spring3D class.

// Local headers
#include "VVASE/gui/renderer/primitives/cylinder.h"
#include "VVASE/gui/renderer/primitives/sphere.h"
#include "VVASE/gui/renderer/3dcar/spring3D.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/color.h>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

//==========================================================================
// Class:			Spring3D
// Function:		Spring3D
//
// Description:		Constructor for the Spring3D class.  Performs the entire
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
Spring3D::Spring3D(LibPlot2D::RenderWindow &renderer)
{
	spring = new Cylinder(renderer);
	endPoint1 = new Sphere(renderer);
	endPoint2 = new Sphere(renderer);

	spring->SetCapping(false);

	endPoint1->SetColor(LibPlot2D::Color::ColorWhite);
	endPoint2->SetColor(LibPlot2D::Color::ColorWhite);
}

//==========================================================================
// Class:			Spring3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the spring
//					in the scene.
//
// Input Arguments:
//		end1			= const Eigen::Vector3d&, location of one end of the spring
//		end2			= const Eigen::Vector3d&, location of the other end of the spring
//		diameter		= const double& describing the width of the spring
//		pointDiameter	= const double& describing diameter of the end point actors
//		resolution		= const integer& representing the number of planar sides to use
//						  to represent the cylinders
//		color			= const LibPlot2D::Color& describing this object's color
//		show			= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Spring3D::Update(const Eigen::Vector3d &end1, const Eigen::Vector3d &end2,
	const double &diameter, const double &pointDiameter, const int &resolution,
	const LibPlot2D::Color &color, bool show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASE::Math::IsNaN(end1) || VVASE::Math::IsNaN(end2))
		show = false;

	// Set the visibility flags
	spring->SetVisibility(show);
	endPoint1->SetVisibility(show);
	endPoint2->SetVisibility(show);

	// Make sure we want this to be visible before continuing
	if (!show)
		return;

	// Set this object's color
	spring->SetColor(color);

	// Set the size and resolution of the CylinderSource
	spring->SetRadius(diameter / 2.0);
	spring->SetResolution(resolution);

	// Set the size and resolution of the end points
	endPoint1->SetRadius(pointDiameter / 2.0);
	endPoint1->SetResolution(resolution);
	endPoint2->SetRadius(pointDiameter / 2.0);
	endPoint2->SetResolution(resolution);

	// Set the position of the actors
	spring->SetEndPoint1(end1);
	spring->SetEndPoint2(end2);
	endPoint1->SetCenter(end1);
	endPoint2->SetCenter(end2);
}

//==========================================================================
// Class:			Spring3D
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
//		bool representing whether or not the actor was part of this object
//
//==========================================================================
bool Spring3D::ContainsThisActor(const LibPlot2D::Primitive *actor)
{
	// Make the comparison
	if (spring == actor ||
		endPoint1 == actor ||
		endPoint2 == actor)
		return true;
	else
		return false;
}

//==========================================================================
// Class:			Spring3D
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
Eigen::Vector3d Spring3D::FindClosestPoint(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
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
