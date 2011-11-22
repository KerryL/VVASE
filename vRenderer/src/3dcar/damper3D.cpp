/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  damper3D.cpp
// Created:  1/5/2009
// Author:  K. Loux
// Description:  Contains class definition for the Damper3D class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/sphere.h"
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/3dcar/damper3D.h"
#include "vRenderer/color.h"
#include "vUtilities/convert.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			Damper3D
// Function:		Damper3D
//
// Description:		Constructor for the Damper3D class.  Performs the entire
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
Damper3D::Damper3D(RenderWindow &_renderer)
{
	// Create the objects
	inboardEndPoint = new Sphere(_renderer);
	outboardEndPoint = new Sphere(_renderer);
	body = new Cylinder(_renderer);
	shaft = new Cylinder(_renderer);

	// Set up the cylinders
	body->SetCapping(true);
	shaft->SetCapping(true);

	// Set up the Actors
	inboardEndPoint->SetColor(Color::ColorWhite);
	outboardEndPoint->SetColor(Color::ColorWhite);
}

//==========================================================================
// Class:			Damper3D
// Function:		~Damper3D
//
// Description:		Destructor for the Damper3D class.
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
Damper3D::~Damper3D()
{
}

//==========================================================================
// Class:			Damper3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the damper
//					in the scene.
//
// Input Arguments:
//		outboardEnd		= const Vector&, location of the end of the shaft
//		inboardEnd		= const Vector&, location of the end of the body
//		bodyDiameter	= const double& describing the diameter of the damper body
//		shaftDiameter	= const double& describing the diameter of the shaft
//		bodyLength		= const double& describing the length of the body section
//		resolution		= const integer& representing the number of planar sides to use
//						  to represent the cylinders
//		bodyColor		= const Color& describing this object's body color
//		shaftColor		= const Color& describing this object's shaft color
//		show			= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Damper3D::Update(const Vector &outboardEnd, const Vector &inboardEnd, const double &bodyDiameter,
					  const double &shaftDiameter, const double &bodyLength, const int &resolution,
					  const Color &bodyColor, const Color &shaftColor, bool show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(outboardEnd) || VVASEMath::IsNaN(inboardEnd))
		show = false;

	// Set the visibility flags
	inboardEndPoint->SetVisibility(true);
	outboardEndPoint->SetVisibility(true);
	body->SetVisibility(true);
	shaft->SetVisibility(true);

	// Make sure we want this to be visible before continuing
	if (!show)
		return;

	// Set this object's color
	body->SetColor(bodyColor);
	shaft->SetColor(shaftColor);

	// Set the size and resolution of the spheres
	inboardEndPoint->SetRadius(shaftDiameter / 2.0 * 1.1);
	outboardEndPoint->SetRadius(shaftDiameter / 2.0 * 1.1);
	inboardEndPoint->SetResolution(resolution);
	outboardEndPoint->SetResolution(resolution);

	// Make sure the shock is longer than just the body
	double displayBodyLength;
	if (outboardEnd.Distance(inboardEnd) > bodyLength)
		// Use the specified body length
		displayBodyLength = bodyLength;
	else
		// Otherwise, split the difference between the shock body and the shaft
		displayBodyLength = outboardEnd.Distance(inboardEnd) / 2.0;

	// Set the size and resolution of the CylinderSources
	body->SetRadius(bodyDiameter / 2.0);
	body->SetResolution(resolution);

	shaft->SetRadius(shaftDiameter / 2.0);
	shaft->SetResolution(resolution);

	// Find the point where the body and shaft will meet
	Vector shaftDirection = outboardEnd - inboardEnd;
	Vector pointOfIntersection = shaftDirection.Normalize()	* displayBodyLength + inboardEnd;

	// Set the position of the body - the body should be on the inboard (chassis) side
	body->SetEndPoint1(inboardEnd);
	body->SetEndPoint2(pointOfIntersection);

	// Set the position of the shaft
	shaft->SetEndPoint1(pointOfIntersection);
	shaft->SetEndPoint2(outboardEnd);

	// Set the positions of the spheres
	inboardEndPoint->SetCenter(inboardEnd);
	outboardEndPoint->SetCenter(outboardEnd);
}

//==========================================================================
// Class:			Damper3D
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
bool Damper3D::ContainsThisActor(const Primitive *actor)
{
	// Make the comparison
	if (inboardEndPoint == actor ||
		outboardEndPoint == actor ||
		body == actor ||
		shaft == actor)
		return true;
	else
		return false;
}