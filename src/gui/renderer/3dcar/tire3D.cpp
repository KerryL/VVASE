/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  tire3D.cpp
// Date:  5/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class definition for the Tire3D class.

// Local headers
#include "VVASE/gui/renderer/primitives/cylinder.h"
#include "VVASE/gui/renderer/primitives/disk.h"
#include "VVASE/gui/renderer/3dcar/tire3D.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/color.h>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

//==========================================================================
// Class:			Tire3D
// Function:		Tire3D
//
// Description:		Constructor for the Tire3D class.  Performs the entire
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
Tire3D::Tire3D(LibPlot2D::RenderWindow &renderer)
{
	innerSurface = new Cylinder(renderer);
	outerSurface = new Cylinder(renderer);
	sidewall1 = new Disk(renderer);
	sidewall2 = new Disk(renderer);

	innerSurface->SetCapping(false);
	outerSurface->SetCapping(false);
}

//==========================================================================
// Class:			Tire3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the tire
//					in the scene.
//
// Input Arguments:
//		conctactPatch	= const Eigen::Vector3d&, location of the tire's bottom
//		center			= const Eigen::Vector3d&, location of center of the tire
//		originalNomral	= Eigen::Vector3d, the "staring position" of the upright plane
//		targetNormal	= Eigen::Vector3d, the "final position" of the upright plane
//		width			= const double& describing width of the tire
//		insideDiameter	= const double& describing inner diameter of the tire
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
void Tire3D::Update(const Eigen::Vector3d &contactPatch, const Eigen::Vector3d &center,
	Eigen::Vector3d originalNormal,Eigen::Vector3d targetNormal, const double &width,
	const double &insideDiameter, const int &resolution, const LibPlot2D::Color &color, bool show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASE::Math::IsNaN(contactPatch) || VVASE::Math::IsNaN(center) ||
		VVASE::Math::IsNaN(originalNormal) || VVASE::Math::IsNaN(targetNormal))
		show = false;

	// Set the visibility flags
	innerSurface->SetVisibility(show);
	outerSurface->SetVisibility(show);
	sidewall1->SetVisibility(show);
	sidewall2->SetVisibility(show);

	// Make sure we want this to be visible before continuing
	if (!show)
		return;

	// Set this object's color
	innerSurface->SetColor(color);
	outerSurface->SetColor(color);
	sidewall1->SetColor(color);
	sidewall2->SetColor(color);

	// Set the size and resolution of the cylinders
	innerSurface->SetRadius((contactPatch - center).norm());
	innerSurface->SetResolution(resolution);

	outerSurface->SetRadius(insideDiameter / 2.0);
	outerSurface->SetResolution(resolution);

	// Set the size and resolution of the disks
	sidewall1->SetOuterRadius((contactPatch - center).norm());
	sidewall1->SetInnerRadius(insideDiameter / 2.0);
	sidewall1->SetResolution(resolution);

	sidewall2->SetOuterRadius((contactPatch - center).norm());
	sidewall2->SetInnerRadius(insideDiameter / 2.0);
	sidewall2->SetResolution(resolution);

	// Normalize the normal vectors before we use them
	targetNormal.normalize();
	originalNormal.normalize();

	// The orientations are given by the axis direction
	// Because the car's tires start out in this orientation (zero pitch, roll, heave and steer),
	// we can use only the original and final normal vectors for the upright plane to define
	// the wheel orientation.
	Eigen::Vector3d rotationAxis;
	double angleToRotate;// [rad]

	// We allow rotation about any arbitrary axis, so we'll start by determining the axis of rotation
	rotationAxis = originalNormal.cross(targetNormal);

	// Determine the distance to rotate
	// Because the normal vectors were normalized above, the angle between the vectors
	// is the acos of their dot product.
	angleToRotate = acos(originalNormal.dot(targetNormal));

	// The positions of the sidewalls are given by the width of the tire and the
	// axis direction.  To calculate the axis direction, we rotate a reference vector
	// the same way that we rotate the actors.  This reference starts out aligned with
	// the Y-axis (tire orientation with zero camber, toe, pitch, roll, etc.)
	Eigen::Vector3d sidewallPosition;
	Eigen::Vector3d axisDirection(0.0, 1.0, 0.0);

	// Check to make sure our rotation axis is non-zero before we do the rotations
	if (!VVASE::Math::IsZero(rotationAxis.norm()))
		// Rotate the reference axis
		axisDirection.Rotate(angleToRotate, rotationAxis);

	// Compute the sidewall position (positive along axis)
	sidewallPosition = center + axisDirection * (width / 2.0);

	// Set the first portion of the position of the cylinders and disks
	innerSurface->SetEndPoint1(sidewallPosition);
	outerSurface->SetEndPoint1(sidewallPosition);
	sidewall1->SetCenter(sidewallPosition);

	// Compute the sidewall position (positive along axis)
	sidewallPosition = center - axisDirection * (width / 2.0);

	// Set the final portion of the position of the cylinders and disks
	innerSurface->SetEndPoint2(sidewallPosition);
	outerSurface->SetEndPoint2(sidewallPosition);
	sidewall2->SetCenter(sidewallPosition);

	// Set the disk normals
	sidewall1->SetNormal(axisDirection);
	sidewall2->SetNormal(axisDirection);
}

//==========================================================================
// Class:			Tire3D
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
bool Tire3D::ContainsThisActor(const LibPlot2D::Primitive *actor)
{
	// Make the comparison
	if (outerSurface == actor ||
		innerSurface == actor ||
		sidewall1 == actor ||
		sidewall2 == actor)
		return true;
	else
		return false;
}

}// namespace VVASE
