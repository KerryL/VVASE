/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  triangle_class.cpp
// Created:  1/5/2009
// Author:  K. Loux
// Description:  Contains class definition for the TRIANGLE3D class.  This is used for
//				 bell cranks, uprights, etc.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/triangle.h"
#include "vRenderer/3dcar/triangle_class.h"
#include "vRenderer/color_class.h"
#include "vUtilities/convert_class.h"
#include "vMath/vector_class.h"
#include "vMath/car_math.h"

//==========================================================================
// Class:			TRIANGLE3D
// Function:		TRIANGLE3D
//
// Description:		Constructor for the TRIANGLE3D class.  Performs the entire
//					process necessary to add the object to the scene.
//
// Input Argurments:
//		_Renderer	= &RENDER_WINDOW, pointer to rendering object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TRIANGLE3D::TRIANGLE3D(RENDER_WINDOW &_Renderer)
{
	// Create the objects
	Triangle = new TRIANGLE(_Renderer);
}

//==========================================================================
// Class:			TRIANGLE3D
// Function:		~TRIANGLE3D
//
// Description:		Destructor for the TRIANGLE3D class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TRIANGLE3D::~TRIANGLE3D()
{
}

//==========================================================================
// Class:			TRIANGLE3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the tire
//					in the scene.  Note that the third argument is passed by
//					value, not reference like the first two VECTORs.
//
// Input Argurments:
//		Node1		= const VECTOR&, location of the first vertex
//		Node2		= const VECTOR&, location of the second vertex
//		Node3		= const VECTOR&, location of the third vertex
//		Color		= const COLOR& specifying the color of this object
//		Show		= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TRIANGLE3D::Update(const VECTOR &Node1, const VECTOR &Node2, const VECTOR &Node3,
					  const COLOR &Color, bool Show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(Node1) || VVASEMath::IsNaN(Node2) || VVASEMath::IsNaN(Node3))
		Show = false;

	// Set the visibility flag
	Triangle->SetVisibility(Show);

	// Make sure we want this to be visible before continuing
	if (!Show)
		return;

	// Set this object's color
	Triangle->SetColor(Color);

	// Set the locations of the points
	Triangle->SetCorner1(Node1);
	Triangle->SetCorner2(Node2);
	Triangle->SetCorner3(Node3);

	return;
}

//==========================================================================
// Class:			TRIANGLE3D
// Function:		ContainsThisActor
//
// Description:		Compares the argument with the actors that make up this
//					object to determine if the argument actor is part of this
//					object or not.
//
// Input Argurments:
//		Actor	= const PRIMITIVE* to compare against this object's actors
//
// Output Arguments:
//		None
//
// Return Value:
//		bool representing wether or not the Actor was part of this object
//
//==========================================================================
bool TRIANGLE3D::ContainsThisActor(const PRIMITIVE *Actor)
{
	// Make the comparison
	if (Triangle == Actor)
		return true;
	else
		return false;
}