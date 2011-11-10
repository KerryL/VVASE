/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plane.cpp
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class definition for the PLANE class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/quadrilateral.h"
#include "vRenderer/3dcar/plane.h"
#include "vRenderer/color.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			PLANE
// Function:		PLANE
//
// Description:		Constructor for the PLANE class.  Performs the entire
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
PLANE::PLANE(RenderWindow &_Renderer)
{
	// Create objects
	Plane = new QUADRILATERAL(_Renderer);

	// Set up the plane
	Vector XAxis(1.0, 0.0, 0.0);
	Vector Normal(0.0, 0.0, 1.0);
	Plane->SetAxis(XAxis);
	Plane->SetNormal(Normal);
}

//==========================================================================
// Class:			PLANE
// Function:		~PLANE
//
// Description:		Destructor for the PLANE class.
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
PLANE::~PLANE()
{
}

//==========================================================================
// Class:			PLANE
// Function:		Update
//
// Description:		Updates the position and size of the plane in the scene
//
// Input Arguments:
//		Length	= const double&, length of the plane in the x-direction
//		Width	= const double&, length of the plane in the y-direction
//		XOffset	= const double&, offset of the center of the plane in the
//				  x-direction
//		color	= const Color& describing this object's color
//		Show	= const bool&, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PLANE::Update(const double &Length, const double &Width, const double &XOffset,
				   const Color &color, const bool &Show)
{
	// Set the visibility flag
	Plane->SetVisibility(Show);

	// Make sure we want this to be visible before continuing
	if (!Show)
		return;

	// Set this object's color
	Plane->SetColor(color);

	// Set the correct position and size of the plane
	Vector Center(XOffset, 0.0, 0.0);
	Plane->SetCenter(Center);
	Plane->SetLength(Length);
	Plane->SetWidth(Width);

	return;
}