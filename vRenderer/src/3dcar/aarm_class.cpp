/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  aarm_class.cpp
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class definition for the AARM class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/color_class.h"
#include "vRenderer/3dcar/aarm_class.h"
#include "vRenderer/primitives/sphere.h"
#include "vRenderer/primitives/cylinder.h"
#include "vUtilities/convert_class.h"
#include "vMath/vector_class.h"
#include "vMath/car_math.h"

//==========================================================================
// Class:			AARM
// Function:		AARM
//
// Description:		Constructor for the AARM class.  Performs the entire
//					process necessary to add the object to the scene.
//
// Input Arguments:
//		_Renderer	= RENDER_WINDOW&, pointer to rendering object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
AARM::AARM(RENDER_WINDOW &_Renderer)
{
	// Create the actors
	EndPoint1 = new SPHERE(_Renderer);
	EndPoint2 = new SPHERE(_Renderer);
	MidPoint = new SPHERE(_Renderer);
	Member1 = new CYLINDER(_Renderer);
	Member2 = new CYLINDER(_Renderer);

	// Set up the Actors
	EndPoint1->SetColor(COLOR::ColorWhite);
	MidPoint->SetColor(COLOR::ColorWhite);
	EndPoint2->SetColor(COLOR::ColorWhite);
}

//==========================================================================
// Class:			AARM
// Function:		~AARM
//
// Description:		Destructor for the AARM class.
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
AARM::~AARM()
{
}

//==========================================================================
// Class:			AARM
// Function:		Update
//
// Description:		Updates the position and size of the a-arm in the scene
//
// Input Arguments:
//		End1		= const VECTOR&, location of one end of the a-arm
//		Middle		= const VECTOR&, location of the ball joint on the a-arm
//		End2		= const VECTOR&, location of the other end of the a-arm
//		Diameter	= const double& describing the size of the tubing representing
//					  the arm
//		Resolution	= const integer& representing the number of planar sides to use
//					  to represent the cylindrical members
//		Color		= const COLOR& describing the color to paint this object
//		Show		= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void AARM::Update(const VECTOR &End1, const VECTOR &Middle, const VECTOR &End2,
				  const double &Diameter, const int &Resolution, const COLOR &Color, bool Show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(End1) || VVASEMath::IsNaN(Middle) || VVASEMath::IsNaN(End2))
		Show = false;

	// Make sure we want this tire to be visible
	if (Show)
	{
		// Set visibility ON
		EndPoint1->SetVisibility(true);
		MidPoint->SetVisibility(true);
		EndPoint2->SetVisibility(true);
		Member1->SetVisibility(true);
		Member2->SetVisibility(true);
	}
	else
	{
		// Set visibility OFF
		EndPoint1->SetVisibility(false);
		MidPoint->SetVisibility(false);
		EndPoint2->SetVisibility(false);
		Member1->SetVisibility(false);
		Member2->SetVisibility(false);

		return;
	}

	// Set this object's color
	Member1->SetColor(Color);
	Member2->SetColor(Color);

	// Set up the spheres
	EndPoint1->SetRadius(Diameter / 2.0 * 1.1);// Make the points just a smidge larger than the members
	MidPoint->SetRadius(Diameter / 2.0 * 1.1);
	EndPoint2->SetRadius(Diameter / 2.0 * 1.1);

	EndPoint1->SetResolution(Resolution);
	MidPoint->SetResolution(Resolution);
	EndPoint2->SetResolution(Resolution);

	// Set up the CylinderSources
	Member1->SetRadius(Diameter / 2.0);
	Member1->SetEndPoint1(End1);
	Member1->SetEndPoint2(Middle);
	Member1->SetResolution(Resolution);

	Member2->SetRadius(Diameter / 2.0);
	Member2->SetEndPoint1(End2);
	Member2->SetEndPoint2(Middle);
	Member2->SetResolution(Resolution);

	// Set the position of the spheres
	EndPoint1->SetCenter(End1);
	MidPoint->SetCenter(Middle);
	EndPoint2->SetCenter(End2);

	return;
}

//==========================================================================
// Class:			AARM
// Function:		ContainsThisActor
//
// Description:		Compares the argument with the actors that make up this
//					object to determine if the argument actor is part of this
//					object or not.
//
// Input Arguments:
//		Actor	= const PRIMITIVE* to compare against this object's actors
//
// Output Arguments:
//		None
//
// Return Value:
//		bool representing wether or not the Actor was part of this object
//
//==========================================================================
bool AARM::ContainsThisActor(const PRIMITIVE *Actor)
{
	// Make the comparison
	if (EndPoint1 == Actor ||
		Member1 == Actor ||
		MidPoint == Actor ||
		Member2 == Actor ||
		EndPoint2 == Actor)
		return true;
	else
		return false;
}