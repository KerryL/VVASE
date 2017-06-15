/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  origin.cpp
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class definition for the Origin class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/3dcar/origin.h"
#include "vRenderer/3dcar/vector3D.h"
#include "vRenderer/color.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			Origin
// Function:		Origin
//
// Description:		Constructor for the Origin class.  Performs the entire
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
Origin::Origin(RenderWindow &renderer)
{
	xDirection = new Vector3D(renderer);
	yDirection = new Vector3D(renderer);
	zDirection = new Vector3D(renderer);
}

//==========================================================================
// Class:			Origin
// Function:		~Origin
//
// Description:		Destructor for the Origin class.
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
Origin::~Origin()
{
	delete xDirection;
	delete yDirection;
	delete zDirection;
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
	Vector origin(0.0, 0.0, 0.0);
	Vector x(1.0, 0.0, 0.0);
	Vector y(0.0, 1.0, 0.0);
	Vector z(0.0, 0.0, 1.0);

	// Scale the vectors to meet the specified lengths
	x *= tipLength + shaftLength;
	y *= tipLength + shaftLength;
	z *= tipLength + shaftLength;

	// Call the update functions for each of the three vectors
	xDirection->Update(x, origin, shaftDiameter, tipDiameter, tipLength, resolution, Color::ColorRed, show);
	yDirection->Update(y, origin, shaftDiameter, tipDiameter, tipLength, resolution, Color::ColorGreen, show);
	zDirection->Update(z, origin, shaftDiameter, tipDiameter, tipLength, resolution, Color::ColorBlue, show);
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
//		actor	= const Primitive* to compare against this object's actors
//
// Output Arguments:
//		None
//
// Return Value:
//		bool representing whether or not the actor was part of this object
//
//==========================================================================
bool Origin::ContainsThisActor(const Primitive *actor)
{
	if (xDirection->ContainsThisActor(actor) ||
		yDirection->ContainsThisActor(actor) ||
		zDirection->ContainsThisActor(actor))
		return true;

	return false;
}
