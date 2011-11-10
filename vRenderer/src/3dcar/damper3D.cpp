/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  damper3d_class.cpp
// Created:  1/5/2009
// Author:  K. Loux
// Description:  Contains class definition for the DAMPER3D class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/sphere.h"
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/3dcar/damper3d_class.h"
#include "vRenderer/color_class.h"
#include "vUtilities/convert_class.h"
#include "vMath/vector_class.h"
#include "vMath/car_math.h"

//==========================================================================
// Class:			DAMPER3D
// Function:		DAMPER3D
//
// Description:		Constructor for the DAMPER3D class.  Performs the entire
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
DAMPER3D::DAMPER3D(RenderWindow &_Renderer)
{
	// Create the objects
	InboardEndPoint = new SPHERE(_Renderer);
	OutboardEndPoint = new SPHERE(_Renderer);
	Body = new CYLINDER(_Renderer);
	Shaft = new CYLINDER(_Renderer);

	// Set up the cylinders
	Body->SetCapping(true);
	Shaft->SetCapping(true);

	// Set up the Actors
	InboardEndPoint->SetColor(Color::ColorWhite);
	OutboardEndPoint->SetColor(Color::ColorWhite);
}

//==========================================================================
// Class:			DAMPER3D
// Function:		~DAMPER3D
//
// Description:		Destructor for the DAMPER3D class.
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
DAMPER3D::~DAMPER3D()
{
}

//==========================================================================
// Class:			DAMPER3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the damper
//					in the scene.
//
// Input Arguments:
//		OutboardEnd		= const Vector&, location of the end of the shaft
//		InboardEnd		= const Vector&, location of the end of the body
//		BodyDiameter	= const double& describing the diameter of the damper body
//		ShaftDiameter	= const double& describing the diameter of the shaft
//		BodyLength		= const double& describing the length of the body section
//		Resolution		= const integer& representing the number of planar sides to use
//						  to represent the cylinders
//		BodyColor		= const Color& describing this object's body color
//		ShaftColor		= const Color& describing this object's shaft color
//		Show			= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DAMPER3D::Update(const Vector &OutboardEnd, const Vector &InboardEnd, const double &BodyDiameter,
					  const double &ShaftDiameter, const double &BodyLength, const int &Resolution,
					  const Color &BodyColor, const Color &ShaftColor, bool Show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(OutboardEnd) || VVASEMath::IsNaN(InboardEnd))
		Show = false;

	// Set the visibility flags
	InboardEndPoint->SetVisibility(true);
	OutboardEndPoint->SetVisibility(true);
	Body->SetVisibility(true);
	Shaft->SetVisibility(true);

	// Make sure we want this to be visible before continuing
	if (!Show)
		return;

	// Set this object's color
	Body->SetColor(BodyColor);
	Shaft->SetColor(ShaftColor);

	// Set the size and resolution of the spheres
	InboardEndPoint->SetRadius(ShaftDiameter / 2.0 * 1.1);
	OutboardEndPoint->SetRadius(ShaftDiameter / 2.0 * 1.1);
	InboardEndPoint->SetResolution(Resolution);
	OutboardEndPoint->SetResolution(Resolution);

	// Make sure the shock is longer than just the body
	double DisplayBodyLength;
	if (OutboardEnd.Distance(InboardEnd) > BodyLength)
		// Use the specified body length
		DisplayBodyLength = BodyLength;
	else
		// Otherwise, split the difference between the shock body and the shaft
		DisplayBodyLength = OutboardEnd.Distance(InboardEnd) / 2.0;

	// Set the size and resolution of the CylinderSources
	Body->SetRadius(BodyDiameter / 2.0);
	Body->SetResolution(Resolution);

	Shaft->SetRadius(ShaftDiameter / 2.0);
	Shaft->SetResolution(Resolution);

	// Find the point where the body and shaft will meet
	Vector ShaftDirection = OutboardEnd - InboardEnd;
	Vector PointOfIntersection = ShaftDirection.Normalize()	* DisplayBodyLength + InboardEnd;

	// Set the position of the body - the body should be on the inboard (chassis) side
	Body->SetEndPoint1(InboardEnd);
	Body->SetEndPoint2(PointOfIntersection);

	// Set the position of the shaft
	Shaft->SetEndPoint1(PointOfIntersection);
	Shaft->SetEndPoint2(OutboardEnd);

	// Set the positions of the spheres
	InboardEndPoint->SetCenter(InboardEnd);
	OutboardEndPoint->SetCenter(OutboardEnd);

	return;
}

//==========================================================================
// Class:			DAMPER3D
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
bool DAMPER3D::ContainsThisActor(const Primitive *Actor)
{
	// Make the comparison
	if (InboardEndPoint == Actor ||
		OutboardEndPoint == Actor ||
		Body == Actor ||
		Shaft == Actor)
		return true;
	else
		return false;
}