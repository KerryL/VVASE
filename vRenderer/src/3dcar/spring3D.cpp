/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  spring3D.cpp
// Created:  1/5/2009
// Author:  K. Loux
// Description:  Contains class definition for the SPRING3D class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/primitives/sphere.h"
#include "vRenderer/3dcar/spring3D.h"
#include "vRenderer/color.h"
#include "vUtilities/convert.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			SPRING3D
// Function:		SPRING3D
//
// Description:		Constructor for the SPRING3D class.  Performs the entire
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
SPRING3D::SPRING3D(RenderWindow &_Renderer)
{
	// Create the objects
	Spring = new CYLINDER(_Renderer);
	EndPoint1 = new SPHERE(_Renderer);
	EndPoint2 = new SPHERE(_Renderer);

	// Set up the cylinder
	Spring->SetCapping(false);

	// Set up the spheres
	EndPoint1->SetColor(Color::ColorWhite);
	EndPoint2->SetColor(Color::ColorWhite);
}

//==========================================================================
// Class:			SPRING3D
// Function:		~SPRING3D
//
// Description:		Destructor for the SPRING3D class.
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
SPRING3D::~SPRING3D()
{
}

//==========================================================================
// Class:			SPRING3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the spring
//					in the scene.
//
// Input Arguments:
//		End1			= const Vector&, location of one end of the spring
//		End2			= const Vector&, location of the other end of the spring
//		Diameter		= const double& describing the width of the spring
//		PointDiameter	= const double& describing diameter of the end point actors
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
void SPRING3D::Update(const Vector &End1, const Vector &End2, const double &Diameter,
					  const double &PointDiameter, const int &Resolution,
					  const Color &color, bool Show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(End1) || VVASEMath::IsNaN(End2))
		Show = false;

	// Set the visibility flags
	Spring->SetVisibility(Show);
	EndPoint1->SetVisibility(Show);
	EndPoint2->SetVisibility(Show);

	// Make sure we want this to be visible before continuing
	if (!Show)
		return;

	// Set this object's color
	Spring->SetColor(color);

	// Set the size and resolution of the CylinderSource
	Spring->SetRadius(Diameter / 2.0);
	Spring->SetResolution(Resolution);

	// Set the size and resolution of the end points
	EndPoint1->SetRadius(PointDiameter / 2.0);
	EndPoint1->SetResolution(Resolution);
	EndPoint2->SetRadius(PointDiameter / 2.0);
	EndPoint2->SetResolution(Resolution);

	// Set the position of the actors
	Spring->SetEndPoint1(End1);
	Spring->SetEndPoint2(End2);
	EndPoint1->SetCenter(End1);
	EndPoint2->SetCenter(End2);

	return;
}

//==========================================================================
// Class:			SPRING3D
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
bool SPRING3D::ContainsThisActor(const Primitive *Actor)
{
	// Make the comparison
	if (Spring == Actor ||
		EndPoint1 == Actor ||
		EndPoint2 == Actor)
		return true;
	else
		return false;
}