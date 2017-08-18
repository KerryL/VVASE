/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  point3D.cpp
// Date:  1/5/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class definition for the Point3D class.

// Local headers
#include "VVASE/gui/renderer/primitives/sphere.h"
#include "VVASE/gui/renderer/3dcar/point3D.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/color.h>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

//==========================================================================
// Class:			Point3D
// Function:		Point3D
//
// Description:		Constructor for the Point3D class.  Performs the entire
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
Point3D::Point3D(LibPlot2D::RenderWindow &renderer)
{
	point = new Sphere(renderer);
}

//==========================================================================
// Class:			Point3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the spring
//					in the scene.
//
// Input Arguments:
//		position	= const Eigen::Vector3d&, location of the point
//		diameter	= const double& describing the width of the spring
//		resolution	= const integer& representing the number of planar sides to use
//					  to represent the cylinders
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
void Point3D::Update(const Eigen::Vector3d &position, const double &diameter, const int &resolution,
	const LibPlot2D::Color &color, bool show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASE::Math::IsNaN(position))
		show = false;

	// Set the visibility flag
	point->SetVisibility(show);

	// Make sure we want this to be visible before continuing
	if (!show)
		return;

	// Set this object's color
	point->SetColor(color);

	// Set the size and resolution of the point
	point->SetRadius(diameter / 2.0);
	point->SetResolution(resolution);

	// Set the position of the actor
	point->SetCenter(position);
}

//==========================================================================
// Class:			Point3D
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
bool Point3D::ContainsThisActor(const LibPlot2D::Primitive *actor)
{
	if (point == actor)
		return true;
	else
		return false;
}

}// namespace VVASE
