/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  swaybar3d_class.cpp
// Created:  1/11/2009
// Author:  K. Loux
// Description:  Contains class definition for the SWAYBAR3D class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/3dcar/swaybar3d_class.h"
#include "vRenderer/color_class.h"
#include "vUtilities/convert_class.h"
#include "vMath/vector_class.h"
#include "vMath/car_math.h"

//==========================================================================
// Class:			SWAYBAR3D
// Function:		SWAYBAR3D
//
// Description:		Constructor for the SWAYBAR3D class.  Performs the entire
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
SWAYBAR3D::SWAYBAR3D(RENDER_WINDOW &_Renderer)
{
	// Create the objects
	TorqueArm1 = new CYLINDER(_Renderer);
	TorqueArm2 = new CYLINDER(_Renderer);
	TorsionMember = new CYLINDER(_Renderer);

	// Set up the cylinders
	TorqueArm1->SetCapping(true);
	TorqueArm2->SetCapping(true);
	TorsionMember->SetCapping(true);

	// Set the resolutions of the torque arms
	// We set it to four to make it look like a rectangular prism
	TorqueArm1->SetResolution(4);
	TorqueArm2->SetResolution(4);
}

//==========================================================================
// Class:			SWAYBAR3D
// Function:		~SWAYBAR3D
//
// Description:		Destructor for the SWAYBAR3D class.
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
SWAYBAR3D::~SWAYBAR3D()
{
}

//==========================================================================
// Class:			SWAYBAR3D
// Function:		Update
//
// Description:		Updates the position, orientation, and size of the tire
//					in the scene.  Note that the third argument is passed by
//					value, not reference like the first two VECTORs.
//
// Input Arguments:
//		LeftLink				= const VECTOR&, point where link meets the left arm
//		RightLink				= const VECTOR&, point where link meets the right arm
//		TorsionMemberTopRight	= const VECTOR&, either the top OR the right end of the
//								  sway bar (depends on bar style)
//		TorsionMemberBottomLeft	= const VECTOR&, either the bottom OR the left end of the
//								  sway bar (depends on bar style)
//		BarStyle				= const SUSPENSION::BAR_STYLE& defining the type of swaybar
//		Dimension				= const double& describing the size of the members
//		Resolution				= const integer& representing the number of planar sides to use
//								  to represent the cylinders
//		Color					= const COLOR& describing this object's color
//		Show					= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void SWAYBAR3D::Update(const VECTOR &RightLink, const VECTOR &LeftLink, const VECTOR &TorsionMemberTopRight,
					   const VECTOR &TorsionMemberBottomLeft, const SUSPENSION::BAR_STYLE &BarStyle,
					   const double &Dimension, const int &Resolution, const COLOR &Color, bool Show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(RightLink) || VVASEMath::IsNaN(LeftLink) ||
		VVASEMath::IsNaN(TorsionMemberTopRight) || VVASEMath::IsNaN(TorsionMemberBottomLeft))
		Show = false;

	// Check to make sure the sway bar exists
	if (BarStyle == SUSPENSION::SwayBarNone ||
		BarStyle == SUSPENSION::SwayBarGeared)// FIXME:  Geared bars not yet implemented!
		Show = false;

	// Set the visibility flags
	TorqueArm1->SetVisibility(Show);
	TorqueArm2->SetVisibility(Show);
	TorsionMember->SetVisibility(Show);

	// Make sure we want this to be visible before continuing
	if (!Show)
		return;

	// Set this object's color
	TorqueArm1->SetColor(Color);
	TorqueArm2->SetColor(Color);
	TorsionMember->SetColor(Color);

	// Set the size of the source objects
	TorqueArm1->SetRadius(Dimension / 2.0);
	TorqueArm2->SetRadius(Dimension / 2.0);
	TorsionMember->SetRadius(Dimension / 2.0);

	// Set the resoltuion of the torsion member
	TorsionMember->SetResolution(Resolution);

	// Position the torsion member
	TorsionMember->SetEndPoint1(TorsionMemberBottomLeft);
	TorsionMember->SetEndPoint2(TorsionMemberTopRight);

	// Position the right torque arm
	TorqueArm1->SetEndPoint1(TorsionMemberTopRight);
	TorqueArm1->SetEndPoint2(RightLink);

	// Position the left torque arm
	TorqueArm2->SetEndPoint1(LeftLink);

	// The geometry depends on what kind of swaybar it is
	if (BarStyle == SUSPENSION::SwayBarUBar)
		TorqueArm2->SetEndPoint2(TorsionMemberBottomLeft);
	else// if (BarStyle == SUSPENSION::SwayBarTBar)
		TorqueArm2->SetEndPoint2(TorsionMemberTopRight);

	// FIXME:  Use real rectangular prisms
}

//==========================================================================
// Class:			SWAYBAR3D
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
bool SWAYBAR3D::ContainsThisActor(const PRIMITIVE *Actor)
{
	// Make the comparison
	if (TorqueArm1 == Actor ||
		TorqueArm2 == Actor ||
		TorsionMember == Actor)
		return true;
	else
		return false;
}