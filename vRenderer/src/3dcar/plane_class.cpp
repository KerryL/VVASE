/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plane_class.cpp
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class definition for the PLANE class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/quadrilateral.h"
#include "vRenderer/3dcar/plane_class.h"
#include "vRenderer/color_class.h"
#include "vMath/vector_class.h"

//==========================================================================
// Class:			PLANE
// Function:		PLANE
//
// Description:		Constructor for the PLANE class.  Performs the entire
//					process necessary to add the object to the scene.
//
// Input Argurments:
//		_Renderer	= RENDER_WINDOW&, pointer to rendering object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PLANE::PLANE(RENDER_WINDOW &_Renderer)
{
	// Create objects
	Plane = new QUADRILATERAL(_Renderer);

	// Set up the plane
	VECTOR XAxis(1.0, 0.0, 0.0);
	VECTOR Normal(0.0, 0.0, 1.0);
	Plane->SetAxis(XAxis);
	Plane->SetNormal(Normal);
}

//==========================================================================
// Class:			PLANE
// Function:		~PLANE
//
// Description:		Destructor for the PLANE class.
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
PLANE::~PLANE()
{
}

//==========================================================================
// Class:			PLANE
// Function:		Update
//
// Description:		Updates the position and size of the plane in the scene
//
// Input Argurments:
//		Length	= const double&, length of the plane in the x-direction
//		Width	= const double&, length of the plane in the y-direction
//		XOffset	= const double&, offset of the center of the plane in the
//				  x-direction
//		Color	= const COLOR& describing this object's color
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
				   const COLOR &Color, const bool &Show)
{
	// Set the visibility flag
	Plane->SetVisibility(Show);

	// Make sure we want this to be visible before continuing
	if (!Show)
		return;

	// Set this object's color
	Plane->SetColor(Color);

	// Set the correct position and size of the plane
	VECTOR Center(XOffset, 0.0, 0.0);
	Plane->SetCenter(Center);
	Plane->SetLength(Length);
	Plane->SetWidth(Width);

	return;
}