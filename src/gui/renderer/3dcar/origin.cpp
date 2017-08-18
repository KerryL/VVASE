/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  origin.cpp
// Date:  5/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class definition for the Origin class.

// Local headers
#include "VVASE/gui/renderer/3dcar/origin.h"
#include "VVASE/gui/renderer/3dcar/vector3D.h"

// LibPlot2D headers
#include <lp2d/renderer/color.h>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

//==========================================================================
// Class:			Origin
// Function:		Origin
//
// Description:		Constructor for the Origin class.  Performs the entire
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
Origin::Origin(LibPlot2D::RenderWindow &renderer)
{
	xDirection = std::make_unique<Vector3D>(renderer);
	yDirection = std::make_unique<Vector3D>(renderer);
	zDirection = std::make_unique<Vector3D>(renderer);
}

//==========================================================================
// Class:			Origin
// Function:		Update
//
// Description:		Updates the size of the origin marker
//
// Input Arguments:
//		shaftLength		= const double& describing the arrow length
//		shaftDiameter	= const double& describing the width of the arrow
//		tipLength		= const double& describing the length of the head
//		tipDiameter		= const double& describing the width of the head
//		resolution		= const integer& specifying the number of sides to use to
//						  approximate the cones and cylinders
//		show			= const bool&, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Origin::Update(const double &shaftLength, const double &shaftDiameter,
					const double &tipLength, const double &tipDiameter, const int &resolution,
					const bool &show)
{
	// All of them start at the origin
	Eigen::Vector3d origin(0.0, 0.0, 0.0);
	Eigen::Vector3d x(1.0, 0.0, 0.0);
	Eigen::Vector3d y(0.0, 1.0, 0.0);
	Eigen::Vector3d z(0.0, 0.0, 1.0);

	// Scale the vectors to meet the specified lengths
	x *= tipLength + shaftLength;
	y *= tipLength + shaftLength;
	z *= tipLength + shaftLength;

	// Call the update functions for each of the three vectors
	xDirection->Update(x, origin, shaftDiameter, tipDiameter, tipLength, resolution, LibPlot2D::Color::ColorRed, show);
	yDirection->Update(y, origin, shaftDiameter, tipDiameter, tipLength, resolution, LibPlot2D::Color::ColorGreen, show);
	zDirection->Update(z, origin, shaftDiameter, tipDiameter, tipLength, resolution, LibPlot2D::Color::ColorBlue, show);
}

//==========================================================================
// Class:			Origin
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
bool Origin::ContainsThisActor(const LibPlot2D::Primitive *actor)
{
	if (xDirection->ContainsThisActor(actor) ||
		yDirection->ContainsThisActor(actor) ||
		zDirection->ContainsThisActor(actor))
		return true;

	return false;
}

}// namespace VVASE
