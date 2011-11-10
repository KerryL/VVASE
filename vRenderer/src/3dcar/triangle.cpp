/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

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
// Input Arguments:
//		_Renderer	= &RenderWindow, pointer to rendering object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TRIANGLE3D::TRIANGLE3D(RenderWindow &_Renderer)
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
TRIANGLE3D::~TRIANGLE3D()
{
}

//==========================================================================
// Class:			TRIANGLE3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the tire
//					in the scene.  Note that the third argument is passed by
//					value, not reference like the first two Vectors.
//
// Input Arguments:
//		Node1		= const Vector&, location of the first vertex
//		Node2		= const Vector&, location of the second vertex
//		Node3		= const Vector&, location of the third vertex
//		color		= const Color& specifying the color of this object
//		Show		= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TRIANGLE3D::Update(const Vector &Node1, const Vector &Node2, const Vector &Node3,
					  const Color &color, bool Show)
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
	Triangle->SetColor(color);

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
bool TRIANGLE3D::ContainsThisActor(const Primitive *Actor)
{
	// Make the comparison
	if (Triangle == Actor)
		return true;
	else
		return false;
}