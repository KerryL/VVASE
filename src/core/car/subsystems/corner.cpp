/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  corner.cpp
// Date:  3/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class functionality for corner class.  This class contains
//        the suspension information for one corner of the car (four instances
//        are required to describe the entire suspension).

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "vCar/corner.h"
#include "vUtilities/debugger.h"
#include "vUtilities/machineDefinitions.h"
#include "vUtilities/binaryReader.h"
#include "vUtilities/binaryWriter.h"

//==========================================================================
// Class:			Corner
// Function:		Corner
//
// Description:		Constructor for the Corner class.
//
// Input Arguments:
//		location	= const Location& describing which corner this object
//					  represents
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Corner::Corner(const Location &location) : location(location), hardpoints(NumberOfHardpoints)
{
	int i;
	for (i = 0; i < NumberOfHardpoints; i++)
		hardpoints[i].Set(0.0, 0.0, 0.0);

	staticCamber = 0.0;
	staticToe = 0.0;
	actuationAttachment = AttachmentLowerAArm;
	actuationType = ActuationPushPullrod;
	// FIXME!!! Spring and damper!
}

//==========================================================================
// Class:			Corner
// Function:		GetActuationAttachmentName
//
// Description:		Returns the name of the specified actuation attachment method.
//
// Input Arguments:
//		attachment	= const ActuationAttachment& of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containg the name of the specified attachment method
//
//==========================================================================
wxString Corner::GetActuationAttachmentName(const ActuationAttachment &attachment)
{
	switch (attachment)
	{
	case AttachmentLowerAArm:
		return _T("Lower A-Arm");
		break;

	case AttachmentUpperAArm:
		return _T("Upper A-Arm");
		break;

	case AttachmentUpright:
		return _T("Upright");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			Corner
// Function:		GetActuationTypeName
//
// Description:		Returns the name of the specified actuation type.
//
// Input Arguments:
//		type	= const ActuationType& of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containg the name of the specified actuation type
//
//==========================================================================
wxString Corner::GetActuationTypeName(const ActuationType &type)
{
	switch (type)
	{
	case ActuationPushPullrod:
		return _T("Push/Pullrod");
		break;

	case ActuationOutboardRockerArm:
		return _T("Outboard/Rocker");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			Corner
// Function:		GetHardpointName
//
// Description:		Returns the name of the point.
//
// Input Arguments:
//		point	= const Hardpoints&, specifying the point to retrieve
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the point
//
//==========================================================================
wxString Corner::GetHardpointName(const Hardpoints &point)
{
	switch (point)
	{
	case LowerFrontTubMount:
		return _T("Lower Front Tub Mount");
		break;

	case LowerRearTubMount:
		return _T("Lower Rear Tub Mount");
		break;

	case UpperFrontTubMount:
		return _T("Upper Front Tub Mount");
		break;

	case UpperRearTubMount:
		return _T("Upper Rear Tub Mount");
		break;

	case OutboardTieRod:
		return _T("Outboard Tie Rod");
		break;

	case InboardTieRod:
		return _T("Inboard Tie Rod");
		break;

	case WheelCenter:
		return _T("Wheel Center");
		break;

	case LowerBallJoint:
		return _T("Lower Ball Joint");
		break;

	case UpperBallJoint:
		return _T("Upper Ball Joint");
		break;

	case OutboardPushrod:
		return _T("Outboard Pushrod");
		break;

	case InboardPushrod:
		return _T("Inboard Pushrod");
		break;

	case BellCrankPivot1:
		return _T("Bell Crank Pivot 1");
		break;

	case BellCrankPivot2:
		return _T("Bell Crank Pivot 2");
		break;

	case OutboardSpring:
		return _T("Outboard Spring");
		break;

	case InboardSpring:
		return _T("Inboard Spring");
		break;

	case OutboardDamper:
		return _T("Outboard Damper");
		break;

	case InboardDamper:
		return _T("Inboard Damper");
		break;

	case ContactPatch:
		return _T("Contact Patch");
		break;

	case OutboardBarLink:
		return _T("Outboard Bar Link");
		break;

	case InboardBarLink:
		return _T("Inboard Bar Link");
		break;

	case BarArmAtPivot:
		return _T("Bar Arm At Pivot");
		break;

	case GearEndBarShaft:
		return _T("Gear End Bar Shaft");
		break;

	case OutboardHalfShaft:
		return _T("Outboard Half Shaft");
		break;

	case InboardHalfShaft:
		return _T("Inboard Half Shaft");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			Corner
// Function:		GetLocationName
//
// Description:		Returns the name of the specified location.
//
// Input Arguments:
//		location	= const Location& of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containg the name of the specified actuation type
//
//==========================================================================
wxString Corner::GetLocationName(const Location &location)
{
	switch (location)
	{
	case LocationLeftFront:
		return _T("Left Front");
		break;

	case LocationRightFront:
		return _T("Right Front");
		break;

	case LocationLeftRear:
		return _T("Left Rear");
		break;

	case LocationRightRear:
		return _T("Right Rear");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			Corner
// Function:		ComputeWheelCenter
//
// Description:		Computes the location of the wheel center based on the
//					static toe and camber.
//
// Input Arguments:
//		tireDiameter	= const double& specifying the tire diameter at this corner
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Corner::ComputeWheelCenter(const double &tireDiameter)
{
	// Get the "unperturbed" wheel center from the diameter and the contact patch location
	hardpoints[WheelCenter] = hardpoints[ContactPatch];
	hardpoints[WheelCenter].z = tireDiameter / 2.0;

	// These next two operations have sign changes depending on which
	// side of the car this corner is on:
	//  Camber is always positive for the top leaning out away from the car
	//  Toe is always positive for toe out

	// Rotate the wheel center about the X-axis for camber effects
	double rotationAngle = staticCamber;
	if (location == LocationRightFront || location == LocationRightRear)
		rotationAngle *= -1.0;

	hardpoints[WheelCenter].Rotate(hardpoints[ContactPatch], rotationAngle, Vector::AxisX);

	// Rotate the wheel center about the Z axis for toe effects
	rotationAngle = staticToe;
	if (location == LocationRightFront || location == LocationRightRear)
		rotationAngle *= -1.0;

	hardpoints[WheelCenter].Rotate(hardpoints[ContactPatch], rotationAngle, Vector::AxisZ);
}

//==========================================================================
// Class:			Corner
// Function:		Write
//
// Description:		Writes this corner to file.
//
// Input Arguments:
//		file	= BinaryWriter&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Corner::Write(BinaryWriter& file) const
{
	// Write the components that make up this object to file
	file.Write(staticCamber);
	file.Write(staticToe);
	file.Write(spring.rate);
	//Damper damper;

	file.Write((unsigned int)actuationAttachment);
	file.Write((unsigned int)actuationType);
	file.Write(hardpoints);
}

//==========================================================================
// Class:			Corner
// Function:		Read
//
// Description:		Read from file to fill this corner.
//
// Input Arguments:
//		file		= BinaryReader&
//		fileVersion	= const int& specifying which file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Corner::Read(BinaryReader& file, const int& fileVersion)
{
	// Read this object from file accoring to the file version we're using
	if (fileVersion >= 4)
	{
		file.Read(staticCamber);
		file.Read(staticToe);
		file.Read(spring.rate);
		//Damper damper;
	}
	else if (fileVersion >= 0)
	{
		file.Read(staticCamber);
		file.Read(staticToe);
		//Spring spring;
		//Damper damper;
	}
	else
		assert(0);

	unsigned int temp;
	file.Read(temp);
	actuationAttachment = static_cast<ActuationAttachment>(temp);

	file.Read(temp);
	actuationType = static_cast<ActuationType>(temp);

	if (fileVersion < 5)
		file.Read(temp);// not used, but was previously written

	if (fileVersion >= 5)
		file.Read(hardpoints);
	else
	{
		unsigned int i;
		for (i = 0; i < NumberOfHardpoints; i++)
			file.Read(hardpoints[i]);
	}
}

//==========================================================================
// Class:			Corner
// Function:		operator=
//
// Description:		Assignment operator.  This is necessary due to the references
//					contained within this object.
//
// Input Arguments:
//		corner	= const Corner& to be copied
//
// Output Arguments:
//		None
//
// Return Value:
//		Corner&, reference to this
//
//==========================================================================
Corner& Corner::operator=(const Corner& corner)
{
	// Check for self assignment
	if (this == &corner)
		return *this;

	// Copy settings
	staticCamber = corner.staticCamber;
	staticToe = corner.staticToe;
	spring = corner.spring;
	damper = corner.damper;

	actuationAttachment = corner.actuationAttachment;
	actuationType = corner.actuationType;

	// Copy the hardpoints
	int i;
	for (i = 0; i < NumberOfHardpoints; i++)
		hardpoints[i] = corner.hardpoints[i];

	return *this;
}
