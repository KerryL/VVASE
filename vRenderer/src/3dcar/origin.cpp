/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  origin_class.cpp
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class definition for the ORIGIN class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/3dcar/origin_class.h"
#include "vRenderer/3dcar/vector3d_class.h"
#include "vRenderer/color_class.h"
#include "vMath/vector_class.h"

//==========================================================================
// Class:			ORIGIN
// Function:		ORIGIN
//
// Description:		Constructor for the ORIGIN class.  Performs the entire
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
ORIGIN::ORIGIN(RenderWindow &_Renderer)
{
	// Create the objects
	XDirection = new Vector3D(_Renderer);
	YDirection = new Vector3D(_Renderer);
	ZDirection = new Vector3D(_Renderer);
}

//==========================================================================
// Class:			ORIGIN
// Function:		~ORIGIN
//
// Description:		Destructor for the ORIGIN class.
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
ORIGIN::~ORIGIN()
{
	delete XDirection;
	delete YDirection;
	delete ZDirection;
}

//==========================================================================
// Class:			ORIGIN
// Function:		Update
//
// Description:		Updates the size of the origin marker
//
// Input Arguments:
//		ShaftLength		= const double& describing the arrow length
//		ShaftDiameter	= const double& describing the width of the arrow
//		TipLength		= const double& descibing the length of the head
//		TipDiameter		= const double& describing the width of the head
//		Resolution		= const integer& specifying the number of sides to use to
//						  approximate the cones and cylinders
//		Show			= const bool&, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ORIGIN::Update(const double &ShaftLength, const double &ShaftDiameter,
					const double &TipLength, const double &TipDiameter, const int &Resolution,
					const bool &Show)
{
	// All of them start at the origin
	Vector Origin(0.0, 0.0, 0.0);
	Vector X(1.0, 0.0, 0.0);
	Vector Y(0.0, 1.0, 0.0);
	Vector Z(0.0, 0.0, 1.0);

	// Scale the vectors to meet the specified lengths
	X *= TipLength + ShaftLength;
	Y *= TipLength + ShaftLength;
	Z *= TipLength + ShaftLength;

	// Call the update functions for each of the three vectors
	XDirection->Update(X, Origin, ShaftDiameter, TipDiameter, TipLength, Resolution, Color::ColorRed, Show);
	YDirection->Update(Y, Origin, ShaftDiameter, TipDiameter, TipLength, Resolution, Color::ColorGreen, Show);
	ZDirection->Update(Z, Origin, ShaftDiameter, TipDiameter, TipLength, Resolution, Color::ColorBlue, Show);

	return;
}