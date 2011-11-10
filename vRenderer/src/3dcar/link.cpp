/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  link.cpp
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class definition for the LINK class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/sphere.h"
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/3dcar/link.h"
#include "vRenderer/color.h"
#include "vUtilities/convert.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			LINK
// Function:		LINK
//
// Description:		Constructor for the LINK class.  Performs the entire
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
LINK::LINK(RenderWindow &_Renderer)
{
	// Create the objects
	Member = new CYLINDER(_Renderer);
	EndPoint1 = new SPHERE(_Renderer);
	EndPoint2 = new SPHERE(_Renderer);

	// Set up the Actors
	EndPoint1->SetColor(Color::ColorWhite);
	EndPoint2->SetColor(Color::ColorWhite);
}

//==========================================================================
// Class:			LINK
// Function:		~LINK
//
// Description:		Destructor for the LINK class.
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
LINK::~LINK()
{
}

//==========================================================================
// Class:			LINK
// Function:		Update
//
// Description:		Updates the position and size of the link in the scene
//
// Input Arguments:
//		End1		= const Vector&, location of one end of the a-arm
//		End2		= const Vector&, location of the other end of the a-arm
//		Radius		= const double& describing the size of the tubing representing
//					  the arm
//		Resolution	= const integer& representing the number of planar sides to use
//					  to represent the cylindrical members
//		color		= const Color& describing this object's color
//		Show		= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void LINK::Update(const Vector &End1, const Vector &End2, const double &Diameter,
				  const int &Resolution, const Color &color, bool Show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(End1) || VVASEMath::IsNaN(End2))
		Show = false;

	// Set the visibility flag
	Member->SetVisibility(Show);
	EndPoint1->SetVisibility(Show);
	EndPoint2->SetVisibility(Show);

	// Make sure we want this to be visible before continuing
	if (!Show)
		return;

	// Set this object's color
	Member->SetColor(color);

	// Set up the spheres
	EndPoint1->SetRadius(Diameter / 2.0 * 1.1);// Make the points just a smidge larger than the members
	EndPoint1->SetResolution(Resolution);
	EndPoint2->SetRadius(Diameter / 2.0 * 1.1);// Make the points just a smidge larger than the members
	EndPoint2->SetResolution(Resolution);

	// Set up the cylinder
	Member->SetRadius(Diameter / 2.0);
	Member->SetResolution(Resolution);

	// Set the positions of the spheres and the cylinder
	EndPoint1->SetCenter(End1);
	EndPoint2->SetCenter(End2);
	Member->SetEndPoint1(End1);
	Member->SetEndPoint2(End2);

	return;
}

//==========================================================================
// Class:			LINK
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
bool LINK::ContainsThisActor(const Primitive *Actor)
{
	// Make the comparison
	if (EndPoint1 == Actor ||
		EndPoint2 == Actor ||
		Member == Actor)
		return true;
	else
		return false;
}