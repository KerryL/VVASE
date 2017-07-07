/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  corner.h
// Date:  3/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for Corner class.  This class contains
//        the suspension information for one corner of the car (four instances
//        are required to describe the entire suspension).

#ifndef CORNER_H_
#define CORNER_H_

// Standard C++ headers
#include <vector>

// Local headers
#include "VVASE/core/car/subsystems/spring.h"
#include "VVASE/core/car/subsystems/damper.h"

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Corner
{
public:
	// Enumeration describing the physical location of the corner on the car
	enum Location
	{
		LocationRightFront,
		LocationLeftFront,
		LocationRightRear,
		LocationLeftRear,

		NumberOfLocations
	};

	// Constructor
	Corner(const Location &location);

	// File read/write functions
	void Write(BinaryWriter& file) const;
	void Read(BinaryReader& file, const int& fileVersion);

	// Calculates the wheel center location based on static toe and camber
	void ComputeWheelCenter(const double &tireDiameter);

	// Enumeration that describes the different suspension actuation types
	// we can model
	enum ActuationAttachment
	{
		AttachmentLowerAArm,
		AttachmentUpperAArm,
		AttachmentUpright,

		NumberOfAttachments
	};

	// Enumeration for actuation type
	enum ActuationType
	{
		ActuationPushPullrod,
		ActuationOutboardRockerArm,

		NumberOfActuationTypes
	};

	// Enumberation for suspension hardpoints
	enum Hardpoints
	{
		LowerFrontTubMount,
		LowerRearTubMount,
		LowerBallJoint,
		UpperFrontTubMount,
		UpperRearTubMount,
		UpperBallJoint,
		OutboardTieRod,
		InboardTieRod,
		OutboardPushrod,		// Also pullrod
		InboardPushrod,			// Also pullrod
		BellCrankPivot1,
		BellCrankPivot2,
		OutboardSpring,
		InboardSpring,
		OutboardDamper,
		InboardDamper,
		ContactPatch,
		WheelCenter,			// Not accessable to the user - this value is calcualted
		OutboardBarLink,
		InboardBarLink,
		BarArmAtPivot,			// For T-bar, this is at base of stem, for others, it is where the arm meets the torsion member
		GearEndBarShaft,		// Gear style bars only

		// Drivetrain geometery
		// These were previously a member of the Drivetrain class, but there were
		// some heap allocation problems (crashes and memory leaks as a result of
		// trying to delete the dynamic GearRatio variable) due to creating new Car
		// objects for Kinematics.  Puting these here allows us to declare only
		// new Suspension objects instead of whole cars, which fixes the memory
		// problem and makes some of the code more concise and neater.
		OutboardHalfShaft,
		InboardHalfShaft,

		NumberOfHardpoints
	};

	// For getting the names of the elements of this class
	static wxString GetHardpointName(const Hardpoints &point);
	static wxString GetActuationAttachmentName(const ActuationAttachment &attachment);
	static wxString GetActuationTypeName(const ActuationType &type);
	static wxString GetLocationName(const Location &location);

	// Suspension parameters
	double staticCamber;			// [rad] - positive top away from the center car
	double staticToe;				// [rad] - positive front away the center of the car
	Spring spring;
	Damper damper;
	// Bump stops (with non-linear rates?)
	// Droop limiters

	ActuationAttachment actuationAttachment;
	ActuationType actuationType;
	const Location location;
	std::vector<Eigen::Vector3d> hardpoints;

	Corner& operator=(const Corner& corner);
};

}// namespace VVASE

#endif// CORNER_H_
