/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  aarm.cpp
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class definition for the AArm class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/color.h"
#include "vRenderer/3dcar/aarm.h"
#include "vRenderer/primitives/sphere.h"
#include "vRenderer/primitives/cylinder.h"
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			AArm
// Function:		AArm
//
// Description:		Constructor for the AArm class.  Performs the entire
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
AArm::AArm(RenderWindow &_renderer)
{
	// Create the actors
	endPoint1 = new Sphere(_renderer);
	endPoint2 = new Sphere(_renderer);
	midPoint = new Sphere(_renderer);
	member1 = new Cylinder(_renderer);
	member2 = new Cylinder(_renderer);

	// Set up the Actors
	endPoint1->SetColor(Color::ColorWhite);
	midPoint->SetColor(Color::ColorWhite);
	endPoint2->SetColor(Color::ColorWhite);
}

//==========================================================================
// Class:			AArm
// Function:		~AArm
//
// Description:		Destructor for the AArm class.
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
AArm::~AArm()
{
}

//==========================================================================
// Class:			AArm
// Function:		Update
//
// Description:		Updates the position and size of the a-arm in the scene
//
// Input Arguments:
//		end1		= const Vector&, location of one end of the a-arm
//		middle		= const Vector&, location of the ball joint on the a-arm
//		end2		= const Vector&, location of the other end of the a-arm
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
void AArm::Update(const Vector &end1, const Vector &middle, const Vector &end2,
				  const double &diameter, const int &resolution, const Color &color, bool show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(end1) || VVASEMath::IsNaN(middle) || VVASEMath::IsNaN(end2))
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
//		actor	= const Primitive* to compare against this object's actors
//
// Output Arguments:
//		None
//
// Return Value:
//		bool representing whether or not the Actor was part of this object
//
//==========================================================================
bool AArm::ContainsThisActor(const Primitive *actor)
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