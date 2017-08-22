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

// Local headers
#include "VVASE/core/car/subsystems/corner.h"
#include "VVASE/core/utilities/debugger.h"
#include "VVASE/core/utilities/binaryReader.h"
#include "VVASE/core/utilities/binaryWriter.h"
#include "VVASE/core/utilities/geometryMath.h"

namespace VVASE
{

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
Corner::Corner(const Location &location) : location(location), hardpoints(static_cast<int>(Hardpoints::Count))
{
	for (auto& hardpoint : hardpoints)
		hardpoint.setZero();

	staticCamber = 0.0;
	staticToe = 0.0;
	actuationAttachment = ActuationAttachment::LowerAArm;
	actuationType = ActuationType::PushPullrod;
	// FIXME!!! Spring and damper!

	if (location == Location::LeftFront ||
		location == Location::RightFront)
	{
		hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)] = Eigen::Vector3d(0.0, 21.875, 0.0);
		hardpoints[static_cast<int>(Corner::Hardpoints::LowerFrontTubMount)] = Eigen::Vector3d(-6.893, 3.0, 5.0);
		hardpoints[static_cast<int>(Corner::Hardpoints::LowerRearTubMount)] = Eigen::Vector3d(5.107, 3.0, 5.0);
		hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)] = Eigen::Vector3d(0.168, 19.8, 5.0);
		hardpoints[static_cast<int>(Corner::Hardpoints::UpperFrontTubMount)] = Eigen::Vector3d(0.893, 7.0, 13.75);
		hardpoints[static_cast<int>(Corner::Hardpoints::UpperRearTubMount)] = Eigen::Vector3d(9.393, 7.0, 13.75);
		hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)] = Eigen::Vector3d(0.893, 19.0, 15.25);
		hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)] = Eigen::Vector3d(-0.482, 7.0, 13.75);
		hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)] = Eigen::Vector3d(-0.896, 19.893, 15.36);
		hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot1)] = Eigen::Vector3d(-0.482, 8.063, 17.625);
		hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot2)] = Eigen::Vector3d(-0.482, 6.438, 16.161);
		hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)] = Eigen::Vector3d(2.768, 7.0, 16.75);
		hardpoints[static_cast<int>(Corner::Hardpoints::OutboardPushrod)] = Eigen::Vector3d(0.107, 17.813, 5.625);
		hardpoints[static_cast<int>(Corner::Hardpoints::InboardDamper)] = Eigen::Vector3d(-9.232, 4.75, 18.75);
		hardpoints[static_cast<int>(Corner::Hardpoints::InboardSpring)] = Eigen::Vector3d(-9.232, 4.75, 18.75);
		hardpoints[static_cast<int>(Corner::Hardpoints::OutboardDamper)] = Eigen::Vector3d(-1.232, 3.375, 19.5);
		hardpoints[static_cast<int>(Corner::Hardpoints::OutboardSpring)] = Eigen::Vector3d(-1.232, 3.375, 19.5);
		hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)] = Eigen::Vector3d(4.518, 5.0, 15.0);
		hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)] = Eigen::Vector3d(4.518, 5.0, 19.0);
		hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)] = Eigen::Vector3d(-1.232, 5.0, 19.0);
	}
	else// Rear
	{
		hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)] = Eigen::Vector3d(66.5, 20.0, 0.0);
		hardpoints[static_cast<int>(Corner::Hardpoints::LowerFrontTubMount)] = Eigen::Vector3d(62.5, 3.2, 5.0);
		hardpoints[static_cast<int>(Corner::Hardpoints::LowerRearTubMount)] = Eigen::Vector3d(72.5, 3.2, 5.0);
		hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)] = Eigen::Vector3d(66.213, 20.35, 5.125);
		hardpoints[static_cast<int>(Corner::Hardpoints::UpperFrontTubMount)] = Eigen::Vector3d(59.159, 5.0, 12.67);
		hardpoints[static_cast<int>(Corner::Hardpoints::UpperRearTubMount)] = Eigen::Vector3d(73.659, 5.0, 12.67);
		hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)] = Eigen::Vector3d(63.5, 18.025, 14.75);
		hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)] = Eigen::Vector3d(74.925, 5.0, 12.67);
		hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)] = Eigen::Vector3d(69.5, 18.025, 14.75);
		hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot1)] = Eigen::Vector3d(62.75, 3.986, 13.255);
		hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot2)] = Eigen::Vector3d(62.75, 4.986, 14.987);
		hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)] = Eigen::Vector3d(61.31, 4.849, 13.918);
		hardpoints[static_cast<int>(Corner::Hardpoints::OutboardPushrod)] = Eigen::Vector3d(65.0, 17.86, 6.1);
		hardpoints[static_cast<int>(Corner::Hardpoints::InboardDamper)] = Eigen::Vector3d(72.25, 1.455, 15.871);
		hardpoints[static_cast<int>(Corner::Hardpoints::InboardSpring)] = Eigen::Vector3d(72.25, 1.455, 15.871);
		hardpoints[static_cast<int>(Corner::Hardpoints::OutboardDamper)] = Eigen::Vector3d(62.75, 1.455, 15.871);
		hardpoints[static_cast<int>(Corner::Hardpoints::OutboardSpring)] = Eigen::Vector3d(62.75, 1.455, 15.871);
		hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)] = Eigen::Vector3d(0.0, 0.0, 0.0);
		hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)] = Eigen::Vector3d(78.0, 4.0, 14.5);
		hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)] = Eigen::Vector3d(63.0, 3.75, 14.5);
		hardpoints[static_cast<int>(Corner::Hardpoints::InboardHalfShaft)] = Eigen::Vector3d(66.5, 7.0, 9.75);
		hardpoints[static_cast<int>(Corner::Hardpoints::OutboardHalfShaft)] = Eigen::Vector3d(66.5, 20.0, 10.25);
	}

	if (location == Location::LeftFront ||
		location == Location::LeftRear)
	{
		for (auto& hardpoint : hardpoints)
			hardpoint.y() *= -1.0;
	}
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
//		vvaseString containg the name of the specified attachment method
//
//==========================================================================
vvaseString Corner::GetActuationAttachmentName(const ActuationAttachment &attachment)
{
	switch (attachment)
	{
	case ActuationAttachment::LowerAArm:
		return _T("Lower A-Arm");
		break;

	case ActuationAttachment::UpperAArm:
		return _T("Upper A-Arm");
		break;

	case ActuationAttachment::Upright:
		return _T("Upright");
		break;

	default:
		assert(0);
		break;
	}

	return vvaseString();
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
//		vvaseString containg the name of the specified actuation type
//
//==========================================================================
vvaseString Corner::GetActuationTypeName(const ActuationType &type)
{
	switch (type)
	{
	case ActuationType::PushPullrod:
		return _T("Push/Pullrod");
		break;

	case ActuationType::OutboardRockerArm:
		return _T("Outboard/Rocker");
		break;

	default:
		assert(0);
		break;
	}

	return vvaseString();
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
//		vvaseString containing the name of the point
//
//==========================================================================
vvaseString Corner::GetHardpointName(const Hardpoints &point)
{
	switch (point)
	{
	case Hardpoints::LowerFrontTubMount:
		return _T("Lower Front Tub Mount");
		break;

	case Hardpoints::LowerRearTubMount:
		return _T("Lower Rear Tub Mount");
		break;

	case Hardpoints::UpperFrontTubMount:
		return _T("Upper Front Tub Mount");
		break;

	case Hardpoints::UpperRearTubMount:
		return _T("Upper Rear Tub Mount");
		break;

	case Hardpoints::OutboardTieRod:
		return _T("Outboard Tie Rod");
		break;

	case Hardpoints::InboardTieRod:
		return _T("Inboard Tie Rod");
		break;

	case Hardpoints::WheelCenter:
		return _T("Wheel Center");
		break;

	case Hardpoints::LowerBallJoint:
		return _T("Lower Ball Joint");
		break;

	case Hardpoints::UpperBallJoint:
		return _T("Upper Ball Joint");
		break;

	case Hardpoints::OutboardPushrod:
		return _T("Outboard Pushrod");
		break;

	case Hardpoints::InboardPushrod:
		return _T("Inboard Pushrod");
		break;

	case Hardpoints::BellCrankPivot1:
		return _T("Bell Crank Pivot 1");
		break;

	case Hardpoints::BellCrankPivot2:
		return _T("Bell Crank Pivot 2");
		break;

	case Hardpoints::OutboardSpring:
		return _T("Outboard Spring");
		break;

	case Hardpoints::InboardSpring:
		return _T("Inboard Spring");
		break;

	case Hardpoints::OutboardDamper:
		return _T("Outboard Damper");
		break;

	case Hardpoints::InboardDamper:
		return _T("Inboard Damper");
		break;

	case Hardpoints::ContactPatch:
		return _T("Contact Patch");
		break;

	case Hardpoints::OutboardBarLink:
		return _T("Outboard Bar Link");
		break;

	case Hardpoints::InboardBarLink:
		return _T("Inboard Bar Link");
		break;

	case Hardpoints::BarArmAtPivot:
		return _T("Bar Arm At Pivot");
		break;

	case Hardpoints::GearEndBarShaft:
		return _T("Gear End Bar Shaft");
		break;

	case Hardpoints::OutboardHalfShaft:
		return _T("Outboard Half Shaft");
		break;

	case Hardpoints::InboardHalfShaft:
		return _T("Inboard Half Shaft");
		break;

	default:
		assert(0);
		break;
	}

	return vvaseString();
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
//		vvaseString containg the name of the specified actuation type
//
//==========================================================================
vvaseString Corner::GetLocationName(const Location &location)
{
	switch (location)
	{
	case Location::LeftFront:
		return _T("Left Front");
		break;

	case Location::RightFront:
		return _T("Right Front");
		break;

	case Location::LeftRear:
		return _T("Left Rear");
		break;

	case Location::RightRear:
		return _T("Right Rear");
		break;

	default:
		assert(0);
		break;
	}

	return vvaseString();
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
	hardpoints[static_cast<int>(Hardpoints::WheelCenter)] = hardpoints[static_cast<int>(Hardpoints::ContactPatch)];
	hardpoints[static_cast<int>(Hardpoints::WheelCenter)].z() = tireDiameter / 2.0;

	// These next two operations have sign changes depending on which
	// side of the car this corner is on:
	//  Camber is always positive for the top leaning out away from the car
	//  Toe is always positive for toe out

	// Rotate the wheel center about the X-axis for camber effects
	double rotationAngle = staticCamber;
	if (location == Location::RightFront || location == Location::RightRear)
		rotationAngle *= -1.0;

	GeometryMath::Rotate(hardpoints[static_cast<int>(Hardpoints::WheelCenter)], hardpoints[static_cast<int>(Hardpoints::ContactPatch)], rotationAngle, Eigen::Vector3d::UnitX());

	// Rotate the wheel center about the Z axis for toe effects
	rotationAngle = staticToe;
	if (location == Location::RightFront || location == Location::RightRear)
		rotationAngle *= -1.0;

	GeometryMath::Rotate(hardpoints[static_cast<int>(Hardpoints::WheelCenter)], hardpoints[static_cast<int>(Hardpoints::ContactPatch)], rotationAngle, Eigen::Vector3d::UnitZ());
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
		for (auto& hardpoint : hardpoints)
			file.Read(hardpoint);
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
	for (i = 0; i < static_cast<int>(Hardpoints::Count); i++)
		hardpoints[i] = corner.hardpoints[i];

	return *this;
}

}// namespace VVASE
