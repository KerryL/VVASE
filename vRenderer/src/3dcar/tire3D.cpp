/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  tire3D.cpp
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class definition for the TIRE3D class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/primitives/disk.h"
#include "vRenderer/3dcar/tire3D.h"
#include "vRenderer/color.h"
#include "vUtilities/convert.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			TIRE3D
// Function:		TIRE3D
//
// Description:		Constructor for the TIRE3D class.  Performs the entire
//					process necessary to add the object to the scene.
//
// Input Arguments:
//		_Renderer	= RenderWindow&, pointer to rendering object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TIRE3D::TIRE3D(RenderWindow &_Renderer)
{
	// Create the objects
	InnerSurface = new CYLINDER(_Renderer);
	OuterSurface = new CYLINDER(_Renderer);
	Sidewall1 = new DISK(_Renderer);
	Sidewall2 = new DISK(_Renderer);

	// Set up the cylinders
	InnerSurface->SetCapping(false);
	OuterSurface->SetCapping(false);
}

//==========================================================================
// Class:			TIRE3D
// Function:		~TIRE3D
//
// Description:		Destructor for the TIRE3D class.
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
TIRE3D::~TIRE3D()
{
}

//==========================================================================
// Class:			TIRE3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the tire
//					in the scene.
//
// Input Arguments:
//		ConctactPatch	= const Vector&, location of the tire's bottom
//		Center			= const Vector&, location of center of the tire
//		OriginalNomral	= Vector, the "staring position" of the upright plane
//		TargetNormal	= Vector, the "final position" of the upright plane
//		Width			= const double& describing width of the tire
//		InsideDiameter	= const double& describing inner diameter of the tire
//		Resolution		= const integer& representing the number of planar sides to use
//						  to represent the cylinders
//		color			= const Color& describing this object's color
//		Show			= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TIRE3D::Update(const Vector &ContactPatch, const Vector &Center, Vector OriginalNormal,
					Vector TargetNormal, const double &Width, const double &InsideDiameter,
					const int &Resolution, const Color &color, bool Show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(ContactPatch) || VVASEMath::IsNaN(Center) ||
		VVASEMath::IsNaN(OriginalNormal) || VVASEMath::IsNaN(TargetNormal))
		Show = false;

	// Set the visibility flags
	InnerSurface->SetVisibility(Show);
	OuterSurface->SetVisibility(Show);
	Sidewall1->SetVisibility(Show);
	Sidewall2->SetVisibility(Show);

	// Make sure we want this to be visible before continuing
	if (!Show)
		return;

	// Set this object's color
	InnerSurface->SetColor(color);
	OuterSurface->SetColor(color);
	Sidewall1->SetColor(color);
	Sidewall2->SetColor(color);

	// Set the size and resolution of the cylinders
	InnerSurface->SetRadius(ContactPatch.Distance(Center));
	InnerSurface->SetResolution(Resolution);

	OuterSurface->SetRadius(InsideDiameter / 2.0);
	OuterSurface->SetResolution(Resolution);

	// Set the size and resolution of the disks
	Sidewall1->SetOuterRadius(ContactPatch.Distance(Center));
	Sidewall1->SetInnerRadius(InsideDiameter / 2.0);
	Sidewall1->SetResolution(Resolution);

	Sidewall2->SetOuterRadius(ContactPatch.Distance(Center));
	Sidewall2->SetInnerRadius(InsideDiameter / 2.0);
	Sidewall2->SetResolution(Resolution);

	// Normalize the normal vectors before we use them
	TargetNormal = TargetNormal.Normalize();
	OriginalNormal = OriginalNormal.Normalize();

	// The orientations are given by the axis direction
	// Because the car's tires start out in this orientation (zero pitch, roll, heave and steer),
	// we can use only the original and final normal vectors for the upright plane to define
	// the wheel orientation.
	Vector RotationAxis;
	double AngleToRotate;// [rad]

	// We allow rotation about any arbitrary axis, so we'll start by determining the axis of rotation
	RotationAxis = OriginalNormal.Cross(TargetNormal);

	// Determine the distance to rotate
	// Because the normal vectors were normalized above, the angle between the vectors
	// is the acos of their dot product.
	AngleToRotate = acos(OriginalNormal * TargetNormal);

	// The positions of the sidewalls are given by the width of the tire and the
	// axis direction.  To calculate the axis direction, we rotate a reference vector
	// the same way that we rotate the actors.  This reference starts out aligned with
	// the Y-axis (tire orientation with zero camber, toe, pitch, roll, etc.)
	Vector SidewallPosition;
	Vector AxisDirection(0.0, 1.0, 0.0);

	// Check to make sure our rotation axis is non-zero before we do the rotations
	if (!VVASEMath::IsZero(RotationAxis.Length()))
		// Rotate the reference axis
		AxisDirection.Rotate(AngleToRotate, RotationAxis);

	// Compute the sidewall position (positive along axis)
	SidewallPosition = Center + AxisDirection * (Width / 2.0);

	// Set the first portion of the position of the cylinders and disks
	InnerSurface->SetEndPoint1(SidewallPosition);
	OuterSurface->SetEndPoint1(SidewallPosition);
	Sidewall1->SetCenter(SidewallPosition);

	// Compute the sidewall position (positive along axis)
	SidewallPosition = Center - AxisDirection * (Width / 2.0);

	// Set the final portion of the position of the cylinders and disks
	InnerSurface->SetEndPoint2(SidewallPosition);
	OuterSurface->SetEndPoint2(SidewallPosition);
	Sidewall2->SetCenter(SidewallPosition);

	// Set the disk normals
	Sidewall1->SetNormal(AxisDirection);
	Sidewall2->SetNormal(AxisDirection);

	return;
}

//==========================================================================
// Class:			TIRE3D
// Function:		ContainsThisActor
//
// Description:		Compares the argument with the actors that make up this
//					object to determine if the argument actor is part of this
//					object or not.
//
// Input Arguments:
//		Actor	= const Primitive* to compare against this object's actors
//
// Output Arguments:
//		None
//
// Return Value:
//		bool representing whether or not the Actor was part of this object
//
//==========================================================================
bool TIRE3D::ContainsThisActor(const Primitive *Actor)
{
	// Make the comparison
	if (OuterSurface == Actor ||
		InnerSurface == Actor ||
		Sidewall1 == Actor ||
		Sidewall2 == Actor)
		return true;
	else
		return false;
}