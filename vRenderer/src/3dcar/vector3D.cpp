/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  vector3D.cpp
// Created:  3/14/2009
// Author:  K. Loux
// Description:  Contains class definition for the Vector3D class.  This class contains
//				 and maintains the VTK objects that create actors representing vectors.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

// VVASE headers
#include "vRenderer/primitives/cylinder.h"
#include "vRenderer/primitives/cone.h"
#include "vRenderer/3dcar/vector3D.h"
#include "vRenderer/color.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"
#include "vUtilities/convert.h"

//==========================================================================
// Class:			Vector3D
// Function:		Vector3D
//
// Description:		Constructor for the Vector3D class.  Performs the entire
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
Vector3D::Vector3D(RenderWindow &_Renderer)
{
	// Create the objects
	Shaft = new CYLINDER(_Renderer);
	Tip = new CONE(_Renderer);

	// Set capping on for both objects
	Shaft->SetCapping(true);
	Tip->SetCapping(true);
}

//==========================================================================
// Class:			Vector3D
// Function:		~Vector3D
//
// Description:		Destructor for the Vector3D class.
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
Vector3D::~Vector3D()
{
}

//==========================================================================
// Class:			Vector3D
// Function:		Update
//
// Description:		Updates the size of the Vector3D marker
//
// Input Arguments:
//		_Tip			= const Vector& indicating the point this vector points to
//		_Tail			= const Vector& indicating the point this vector originates
//						  from
//		ShaftDiameter	= const double& describing the width of the arrow
//		TipDiameter		= const double& describing the width of the head
//		TipLength		= const double& specifying length of the head
//		Resolution		= const integer& specifying the number of sides to use to
//						  approximate the cones and cylinders
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
void Vector3D::Update(const Vector &_Tip, const Vector &_Tail, const double &ShaftDiameter,
					  const double &TipDiameter, double TipLength, const int &Resolution,
					  const Color &color, bool Show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(_Tip) || VVASEMath::IsNaN(_Tail))
		Show = false;

	// Set the visibility flags
	Shaft->SetVisibility(Show);
	Tip->SetVisibility(Show);

	// Make sure we want this to be visible before continuing
	if (!Show)
		return;

	// Set this object's color
	// The wxColor stores information with unsigned char, but we're looking for
	// doubles, so we divide by 255
	Shaft->SetColor(color);
	Tip->SetColor(color);

	// Make sure the tip proportion is less than the vector length
	if (TipLength > _Tip.Distance(_Tail))
		TipLength = _Tip.Distance(_Tail) * 0.1;

	// Set the size and resolution of the shaft
	Shaft->SetRadius(ShaftDiameter / 2.0);
	Shaft->SetResolution(Resolution);

	// Set the size and resolution of the tip
	Tip->SetRadius(TipDiameter / 2.0);
	Tip->SetResolution(Resolution);

	// Determine the position where the tip meets the shaft
	Vector MeetingPosition = _Tail + ((_Tip - _Tail) * (1.0 - TipLength / _Tip.Distance(_Tail)));

	// Set the positions of the shaft and tip
	Shaft->SetEndPoint1(_Tail);
	Shaft->SetEndPoint2(MeetingPosition);
	Tip->SetBaseCenter(MeetingPosition);
	Tip->SetTip(_Tip);

	return;
}