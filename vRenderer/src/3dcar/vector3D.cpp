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
//		_renderer	= RenderWindow&, pointer to rendering object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Vector3D::Vector3D(RenderWindow &_renderer)
{
	// Create the objects
	shaft = new Cylinder(_renderer);
	tip = new Cone(_renderer);

	// Set capping on for both objects
	shaft->SetCapping(true);
	tip->SetCapping(true);
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
//		_tip			= const Vector& indicating the point this vector points to
//		tail			= const Vector& indicating the point this vector originates
//						  from
//		shaftDiameter	= const double& describing the width of the arrow
//		tipDiameter		= const double& describing the width of the head
//		tipLength		= const double& specifying length of the head
//		resolution		= const integer& specifying the number of sides to use to
//						  approximate the cones and cylinders
//		color			= const Color& describing this object's color
//		show			= bool, visibility flag
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Vector3D::Update(const Vector &_tip, const Vector &tail, const double &shaftDiameter,
					  const double &tipDiameter, double tipLength, const int &resolution,
					  const Color &color, bool show)
{
	// Make sure all vector arguments are valid - if they are not,
	// the object will not be made visible
	if (VVASEMath::IsNaN(_tip) || VVASEMath::IsNaN(tail))
		show = false;

	// Set the visibility flags
	shaft->SetVisibility(show);
	tip->SetVisibility(show);

	// Make sure we want this to be visible before continuing
	if (!show)
		return;

	// Set this object's color
	// The wxColor stores information with unsigned char, but we're looking for
	// doubles, so we divide by 255
	shaft->SetColor(color);
	tip->SetColor(color);

	// Make sure the tip proportion is less than the vector length
	if (tipLength > _tip.Distance(tail))
		tipLength = _tip.Distance(tail) * 0.1;

	// Set the size and resolution of the shaft
	shaft->SetRadius(shaftDiameter / 2.0);
	shaft->SetResolution(resolution);

	// Set the size and resolution of the tip
	tip->SetRadius(tipDiameter / 2.0);
	tip->SetResolution(resolution);

	// Determine the position where the tip meets the shaft
	Vector meetingPosition = tail + ((_tip - tail) * (1.0 - tipLength / _tip.Distance(tail)));

	// Set the positions of the shaft and tip
	shaft->SetEndPoint1(tail);
	shaft->SetEndPoint2(meetingPosition);
	tip->SetBaseCenter(meetingPosition);
	tip->SetTip(_tip);
}