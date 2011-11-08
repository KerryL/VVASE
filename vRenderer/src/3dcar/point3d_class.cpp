/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  point3d_class.cpp
// Created:  1/5/2009
// Author:  K. Loux
// Description:  Contains class definition for the POINT3D class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/sphere.h"
#include "vRenderer/3dcar/point3d_class.h"
#include "vRenderer/color_class.h"
#include "vMath/vector_class.h"
#include "vMath/car_math.h"

//==========================================================================
// Class:			POINT3D
// Function:		POINT3D
//
// Description:		Constructor for the POINT3D class.  Performs the entire
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
POINT3D::POINT3D(RenderWindow &_Renderer)
{
	// Create the objects
	Point = new SPHERE(_Renderer);
}

//==========================================================================
// Class:			POINT3D
// Function:		~POINT3D
//
// Description:		Destructor for the POINT3D class.
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
POINT3D::~POINT3D()
{
}

//==========================================================================
// Class:			POINT3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the spring
//					in the scene.
//
// Input Arguments:
//		Position	= const Vector&, location of the point
//		Diameter	= const double& describing the width of the spring
//		Resolution	= const integer& representing the number of planar sides to use
//					  to represent the cylinders
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
void POINT3D::Update(const Vector &Position, const double &Diameter, const int &Resolution,
					 const Color &color, bool Show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(Position))
		Show = false;

	// Set the visibility flag
	Point->SetVisibility(Show);

	// Make sure we want this to be visible before continuing
	if (!Show)
		return;

	// Set this object's color
	Point->SetColor(color);

	// Set the size and resolution of the point
	Point->SetRadius(Diameter / 2.0);
	Point->SetResolution(Resolution);

	// Set the position of the actor
	Point->SetCenter(Position);

	return;
}