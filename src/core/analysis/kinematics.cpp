/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  kinematics.cpp
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class functionality for static state class.  This is where
//        most of the functionality of the suspension kinematics end of things
//        will be.

// Local headers
#include "VVASE/core/analysis/kinematics.h"
#include "VVASE/core/car/car.h"
#include "VVASE/core/car/subsystems/corner.h"
#include "VVASE/core/car/subsystems/drivetrain.h"
#include "VVASE/core/car/subsystems/suspension.h"
#include "VVASE/core/car/subsystems/massProperties.h"
#include "VVASE/core/utilities/carMath.h"
#include "VVASE/core/utilities/wheelSetStructures.h"
#include "VVASE/core/utilities/debugLog.h"
#include "VVASE/core/utilities/debugger.h"
#include "VVASE/gui/renderer/3dcar/debugShape.h"
#include "VVASE/core/utilities/geometryMath.h"
#include "VVASE/core/utilities/stopWatch.h"

// Eigen headers
#include <Eigen/Dense>

namespace VVASE
{

//==========================================================================
// Class:			Kinematics
// Function:		UpdateKinematics
//
// Description:		This updates the position of the car to meet the current
//					values of pitch, roll, heave, and steer.
//
// Input Arguments:
//		originalCar		= const Car* - for reference only
//		workingCar		= Car* to be changed (updated) by this function
//		name			= vvaseString used to print messages about this car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Kinematics::UpdateKinematics(const Car* originalCar, Car* workingCar, vvaseString name)
{
	StopWatch timer;
	timer.Start();

	Debugger::GetInstance() << "UpdateKinematics() for " << name << Debugger::Priority::Medium;

	this->originalCar = originalCar;// This one is for reference and won't be changed by this class
	this->workingCar = workingCar;

	// Ensure exclusive access to the car objects
	// NOTE:  Always lock working car first, then lock original car (consistency required to prevent deadlocks)
	MutexLocker workingLock(workingCar->GetMutex());
	DebugLog::GetInstance()->Log(_T("Kinematics::UpdateKinematics (workingLock)"));
	MutexLocker originalLock(originalCar->GetMutex());
	DebugLog::GetInstance()->Log(_T("Kinematics::UpdateKinematics (originalLock)"));

	// Copy the information in the original car to the working car.  This minimizes rounding
	// errors in the calculation of suspension points, and it also ensures that changes made
	// to other sub-systems are carried over into the working car.
	*workingCar = *originalCar;

	// Now we copy the pointer to the working car's suspension to the class member
	// This must be AFTER *WorkingCar = *OriginalCar, since this assignment is a deep copy
	// and the address of the suspension will change!
	localSuspension = workingCar->GetSubsystem<Suspension>();
	const auto* originalSuspension(originalCar->GetSubsystem<Suspension>());

	// Rotate the steering wheel
	MoveSteeringRack(inputs.rackTravel);

	// FIXME:  As it is right now, this section is not compatible with monoshocks

	// Solve the chassis-mounted points for left-right common items (3rd springs/dampers, sway bars)
	// Front sway bars
	// NOTE:  This is only for points that are NOT in the Corner class
	switch (localSuspension->frontBarStyle)
	{
	case Suspension::BarStyle::UBar:
		localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontBarMidPoint)] =
			0.5 * (localSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)]
			+ localSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)]);

			// Fall through

	case Suspension::BarStyle::TBar:
		// Rotations
		ApplyRotations(localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontBarMidPoint)]);
		ApplyRotations(localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontBarPivotAxis)]);

		// Translations
		localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontBarMidPoint)].z() += inputs.heave;
		localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontBarPivotAxis)].z() += inputs.heave;
		break;

	case Suspension::BarStyle::Geared:
	case Suspension::BarStyle::None:
	default:
		// Nothing here
		break;
	}

	// Rear sway bars
	switch (localSuspension->rearBarStyle)
	{
	case Suspension::BarStyle::UBar:
		localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearBarMidPoint)] =
			0.5 * (localSuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)]
			+ localSuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)]);

			// Fall through

	case Suspension::BarStyle::TBar:
		// Rotations
		ApplyRotations(localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearBarMidPoint)]);
		ApplyRotations(localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearBarPivotAxis)]);

		// Translations
		localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearBarMidPoint)].z() += inputs.heave;
		localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearBarPivotAxis)].z() += inputs.heave;
		break;

	case Suspension::BarStyle::Geared:
	case Suspension::BarStyle::None:
	default:
		// Nothing here
		break;
	}

	// Third springs
	if (localSuspension->frontHasThirdSpring)
	{
		// Rotations
		ApplyRotations(localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontThirdSpringInboard)]);
		ApplyRotations(localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontThirdDamperInboard)]);

		// Translations
		localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontThirdSpringInboard)].z() += inputs.heave;
		localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontThirdDamperInboard)].z() += inputs.heave;
	}

	if (localSuspension->rearHasThirdSpring)
	{
		// Rotations
		ApplyRotations(localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearThirdSpringInboard)]);
		ApplyRotations(localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearThirdDamperInboard)]);

		// Translations
		localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearThirdSpringInboard)].z() += inputs.heave;
		localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearThirdDamperInboard)].z() += inputs.heave;
	}

	if (!SolveCorner(localSuspension->rightFront, originalSuspension->rightFront, inputs.tireDeflections.rightFront))
		Debugger::GetInstance() << "ERROR:  Problem solving right front corner!  Increase debug level for more information." <<	Debugger::Priority::High;
	if (!SolveCorner(localSuspension->leftFront, originalSuspension->leftFront, inputs.tireDeflections.leftFront))
		Debugger::GetInstance() << "ERROR:  Problem solving left front corner!  Increase debug level for more information." << Debugger::Priority::High;
	if (!SolveCorner(localSuspension->rightRear, originalSuspension->rightRear, inputs.tireDeflections.rightRear))
		Debugger::GetInstance() << "ERROR:  Problem solving right rear corner!  Increase debug level for more information." << Debugger::Priority::High;
	if (!SolveCorner(localSuspension->leftRear, originalSuspension->leftRear, inputs.tireDeflections.leftRear))
		Debugger::GetInstance() << "ERROR:  Problem solving left rear corner!  Increase debug level for more information." << Debugger::Priority::High;

	// Some things need to be solved AFTER all other corners
	if (localSuspension->frontBarStyle == Suspension::BarStyle::TBar)
	{
		if (!SolveInboardTBarPoints(
			localSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
			localSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
			localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontBarMidPoint)],
			localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontBarPivotAxis)],
			originalSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
			originalSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
			originalSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontBarMidPoint)],
			originalSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontBarPivotAxis)],
			originalSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
			originalSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
			localSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
			localSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)]))
			Debugger::GetInstance() << "ERROR:  Failed to solve for inboard T-bar (front)!" << Debugger::Priority::Medium;
	}

	if (localSuspension->rearBarStyle == Suspension::BarStyle::TBar)
	{
		if (!SolveInboardTBarPoints(
			localSuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
			localSuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
			localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearBarMidPoint)],
			localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearBarPivotAxis)],
			originalSuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
			originalSuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
			originalSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearBarMidPoint)],
			originalSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearBarPivotAxis)],
			originalSuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
			originalSuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
			localSuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
			localSuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)]))
			Debugger::GetInstance() << "ERROR:  Failed to solve for inboard T-bar (rear)!" << Debugger::Priority::Medium;
	}

	UpdateCGs(workingCar);

	outputs.Update(originalCar, localSuspension);
	Debugger::GetInstance() << "Finished UpdateKinematcs() for " << name
		<< " in " << timer.GetElapsedTime<double, std::chrono::seconds>() << " sec" << Debugger::Priority::Low;// TODO:  Set stream precision for time
}

//==========================================================================
// Class:			Kinematics
// Function:		SolveCorner
//
// Description:		This solves for the locations of all of the suspension
//					nodes that exist at each corner of the car.  This includes
//					Everything from the contact patch up to the damper and
//					spring end-points.
//
// Input Arguments:
//		originalCorner	= const Corner*, the un-perturbed locations of the
//						  suspension hardpoints
//		tireDeflection	= const double&
//
// Output Arguments:
//		corner	= Corner*, the corner of the car we are manipulating
//
// Return Value:
//		bool, true for success, false for error(s)
//
//==========================================================================
bool Kinematics::SolveCorner(Corner &corner, const Corner &originalCorner,
	const double& tireDeflection)
{
	// Determine if this corner is at the front or the rear of the car
	bool isAtFront = false;
	if (corner.location == Corner::Location::RightFront || corner.location == Corner::Location::LeftFront)
		isAtFront = true;

	// First move all of the body-fixed points (rotations first)
	ApplyRotations(corner.hardpoints[static_cast<int>(Corner::Hardpoints::LowerFrontTubMount)]);
	ApplyRotations(corner.hardpoints[static_cast<int>(Corner::Hardpoints::LowerRearTubMount)]);
	ApplyRotations(corner.hardpoints[static_cast<int>(Corner::Hardpoints::UpperFrontTubMount)]);
	ApplyRotations(corner.hardpoints[static_cast<int>(Corner::Hardpoints::UpperRearTubMount)]);
	ApplyRotations(corner.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)]);
	ApplyRotations(corner.hardpoints[static_cast<int>(Corner::Hardpoints::InboardSpring)]);
	ApplyRotations(corner.hardpoints[static_cast<int>(Corner::Hardpoints::InboardDamper)]);
	ApplyRotations(corner.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)]);

	// Now do the translations on the same points
	corner.hardpoints[static_cast<int>(Corner::Hardpoints::LowerFrontTubMount)].z() += inputs.heave;
	corner.hardpoints[static_cast<int>(Corner::Hardpoints::LowerRearTubMount)].z() += inputs.heave;
	corner.hardpoints[static_cast<int>(Corner::Hardpoints::UpperFrontTubMount)].z() += inputs.heave;
	corner.hardpoints[static_cast<int>(Corner::Hardpoints::UpperRearTubMount)].z() += inputs.heave;
	corner.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)].z() += inputs.heave;
	corner.hardpoints[static_cast<int>(Corner::Hardpoints::InboardSpring)].z() += inputs.heave;
	corner.hardpoints[static_cast<int>(Corner::Hardpoints::InboardDamper)].z() += inputs.heave;
	corner.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)].z() += inputs.heave;

	// Depending on the type of actuation, we might have to move some additional points, as well
	if (corner.actuationType == Corner::ActuationType::PushPullrod)
	{
		ApplyRotations(corner.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot1)]);
		ApplyRotations(corner.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot2)]);

		corner.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot1)].z() += inputs.heave;
		corner.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot2)].z() += inputs.heave;
	}

	// Do the rotation and translation for the inboard half shafts at the same time
	if ((originalCar->HasFrontHalfShafts() && isAtFront) ||
		(originalCar->HasRearHalfShafts() && !isAtFront))
	{
		ApplyRotations(corner.hardpoints[static_cast<int>(Corner::Hardpoints::InboardHalfShaft)]);
		corner.hardpoints[static_cast<int>(Corner::Hardpoints::InboardHalfShaft)].z() += inputs.heave;
	}

	bool success = true;

	// Solve outboard points and work in through the pushrods and bell cranks.
	// We will iterate the z-location of the lower ball joint and the solution of
	// the first three outboard points until the bottom of the tire is at z = 0.
	Eigen::Vector3d wheelNormal;
	Eigen::Vector3d originalPlaneNormal;
	Eigen::Vector3d newPlaneNormal;
	Eigen::Vector3d wheelRotations;
	double upperLimit(0.0), lowerLimit(0.0);// Initialized here to avoid MSVC++ compiler warning C4701
	int iteration = 1;
	double tolerance = 5.0e-8;
	int limit = 100;

	// NOTE:  Tire deflections here are assumed to be vertical.  For large wheel
	// inclinations, this may introduce some error.

	corner.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].z() = tolerance * 2;// Must be initialized to > Tolerance
	while (iteration <= limit && fabs(corner.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].z() + tireDeflection) > tolerance)
	{
		if (!SolveForXY(Corner::Hardpoints::LowerBallJoint, Corner::Hardpoints::LowerFrontTubMount,
			Corner::Hardpoints::LowerRearTubMount, originalCorner, corner))
		{
			Debugger::GetInstance() << "ERROR:  Failed to solve for lower ball joint!" << Debugger::Priority::Medium;
			success = false;
		}

		if (!SolveForPoint(Corner::Hardpoints::UpperBallJoint, Corner::Hardpoints::LowerBallJoint,
			Corner::Hardpoints::UpperFrontTubMount, Corner::Hardpoints::UpperRearTubMount,originalCorner, corner))
		{
			Debugger::GetInstance() << "ERROR:  Failed to solve for upper ball joint!" << Debugger::Priority::Medium;
			success = false;
		}

		if (!SolveForPoint(Corner::Hardpoints::OutboardTieRod, Corner::Hardpoints::LowerBallJoint,
			Corner::Hardpoints::UpperBallJoint, Corner::Hardpoints::InboardTieRod, originalCorner, corner))
		{
			Debugger::GetInstance() << "ERROR:  Failed to solve for outboard tie rod!" << Debugger::Priority::Medium;
			success = false;
		}

		if (!SolveForPoint(Corner::Hardpoints::WheelCenter, Corner::Hardpoints::LowerBallJoint,
			Corner::Hardpoints::UpperBallJoint, Corner::Hardpoints::OutboardTieRod, originalCorner, corner))
		{
			Debugger::GetInstance() << "ERROR:  Failed to solve for wheel center!" << Debugger::Priority::Medium;
			success = false;
		}

		originalPlaneNormal = VVASE::Math::GetPlaneNormal(originalCorner.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
			originalCorner.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)], originalCorner.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)]);
		newPlaneNormal = VVASE::Math::GetPlaneNormal(corner.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
			corner.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)], corner.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)]);

		// For some operations, if this is on the right-hand side of the car, the sign gets flipped
		double sign = 1.0;
		if (corner.location == Corner::Location::RightFront || corner.location == Corner::Location::RightRear)
			sign *= -1.0;

		// Determine what Euler angles I need to rotate OriginalPlaneNormal through to
		// get NewPlaneNormal.  Do one angle at a time because order of rotations matters.
		wheelRotations = GeometryMath::AnglesBetween(originalPlaneNormal, newPlaneNormal);
		wheelNormal = Eigen::Vector3d::Zero();
		wheelNormal(1) = sign * 1.0;
		GeometryMath::Rotate(wheelNormal, wheelRotations.z(), Eigen::Vector3d::UnitZ());
		GeometryMath::Rotate(wheelNormal, wheelRotations.x(), Eigen::Vector3d::UnitX());

		// Add in the effects of camber and toe settings (contact patch location should not be affected
		// by camber and toe settings - WheelCenter is calculated based on camber and toe, so we need to
		// back that out in the contact patch solver to avoid making contact patch a function of camber
		// and toe).
		GeometryMath::Rotate(wheelNormal, sign * originalCorner.staticCamber, Eigen::Vector3d::UnitX());
		GeometryMath::Rotate(wheelNormal, sign * originalCorner.staticToe, Eigen::Vector3d::UnitZ());

		// Do the actual solving for the contact patch
		SolveForContactPatch(corner.hardpoints[static_cast<int>(Corner::Hardpoints::WheelCenter)], wheelNormal,
			(originalCorner.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)] - originalCorner.hardpoints[static_cast<int>(Corner::Hardpoints::WheelCenter)]).norm(),
			corner.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)]);

		// With the origin on the ground, the error is equal to the Z location of the contact patch
		// FIXME:  This is only true when using flat ground model!!!
		// Use a line search algorithm to adjust the height of the lower ball joint
		// This could get hung up if the relationship between lower ball joint height and contact
		// patch Z location is highly non-linear, but that would be an unusual (impossible?) case (extreme camber gain?)

		// If this is the first iteration, initialize the limit variables
		if (iteration == 1)
		{
			// TODO:  Don't use magic 1" here
			upperLimit = corner.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)].z() + 1.0;
			lowerLimit = corner.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)].z() - 1.0;
		}

		// Make the adjustment in the guess
		if (corner.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].z() + tireDeflection > tolerance)
			upperLimit = corner.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)].z();
		else if (corner.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].z() + tireDeflection < -tolerance)
			lowerLimit = corner.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)].z();
		corner.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)].z() = lowerLimit + (upperLimit - lowerLimit) / 2.0;

		iteration++;
	}

	// Check to make sure we finished the loop because we were within the tolerance (and not
	// because we hit the iteration limit)
	if ((fabs(corner.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].z() + tireDeflection) > tolerance))
	{
		Debugger::GetInstance() << "Warning (SolveCorner):  Contact patch location did not converge" << Debugger::Priority::Medium;
		success = false;
	}

	// Outboard spring/damper actuators
	if (corner.actuationType == Corner::ActuationType::PushPullrod)
	{
		if (corner.actuationAttachment == Corner::ActuationAttachment::LowerAArm)
		{
			if (!SolveForPoint(Corner::Hardpoints::OutboardPushrod, Corner::Hardpoints::LowerBallJoint,
				Corner::Hardpoints::LowerFrontTubMount, Corner::Hardpoints::LowerRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard pushrod!" << Debugger::Priority::Medium;
				success = false;
			}
		}
		else if (corner.actuationAttachment == Corner::ActuationAttachment::UpperAArm)
		{
			if (!SolveForPoint(Corner::Hardpoints::OutboardPushrod, Corner::Hardpoints::UpperBallJoint,
				Corner::Hardpoints::UpperFrontTubMount, Corner::Hardpoints::UpperRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard pullrod!" << Debugger::Priority::Medium;
				success = false;
			}
		}
		else if (corner.actuationAttachment == Corner::ActuationAttachment::Upright)
		{
			if (!SolveForPoint(Corner::Hardpoints::OutboardPushrod, Corner::Hardpoints::UpperBallJoint,
				Corner::Hardpoints::LowerBallJoint, Corner::Hardpoints::OutboardTieRod, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard push/pullrod!" << Debugger::Priority::Medium;
				success = false;
			}
		}

		// Inboard Pushrods
		if (!SolveForPoint(Corner::Hardpoints::InboardPushrod, Corner::Hardpoints::BellCrankPivot1,
			Corner::Hardpoints::BellCrankPivot2, Corner::Hardpoints::OutboardPushrod, originalCorner, corner))
		{
			Debugger::GetInstance() << "ERROR:  Failed to solve for inboard push/pullrod!" << Debugger::Priority::Medium;
			success = false;
		}

		// Outboard Dampers
		if (!SolveForPoint(Corner::Hardpoints::OutboardDamper, Corner::Hardpoints::BellCrankPivot1,
			Corner::Hardpoints::BellCrankPivot2, Corner::Hardpoints::InboardPushrod, originalCorner, corner))
		{
			Debugger::GetInstance() << "ERROR:  Failed to solve for outboard damper!" << Debugger::Priority::Medium;
			success = false;
		}

		// Outboard Springs
		if (!SolveForPoint(Corner::Hardpoints::OutboardSpring, Corner::Hardpoints::BellCrankPivot1,
			Corner::Hardpoints::BellCrankPivot2, Corner::Hardpoints::InboardPushrod, originalCorner, corner))
		{
			Debugger::GetInstance() << "ERROR:  Failed to solve for outboard spring!" << Debugger::Priority::Medium;
			success = false;
		}

		// Sway Bars Outboard
		if ((localSuspension->frontBarStyle != Suspension::BarStyle::None && isAtFront) ||
			(localSuspension->rearBarStyle != Suspension::BarStyle::None && !isAtFront))
		{
			if (!SolveForPoint(Corner::Hardpoints::OutboardBarLink, Corner::Hardpoints::BellCrankPivot1,
				Corner::Hardpoints::BellCrankPivot2, Corner::Hardpoints::InboardPushrod, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard swaybar!" << Debugger::Priority::Medium;
				success = false;
			}
		}
	}
	else if (corner.actuationType == Corner::ActuationType::OutboardRockerArm)// Outboard spring/damper units  - no pushrod/bell crank
	{
		if (corner.actuationAttachment == Corner::ActuationAttachment::LowerAArm)
		{
			if (!SolveForPoint(Corner::Hardpoints::OutboardSpring, Corner::Hardpoints::LowerBallJoint,
				Corner::Hardpoints::LowerFrontTubMount, Corner::Hardpoints::LowerRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard spring!" << Debugger::Priority::Medium;
				success = false;
			}

			if (!SolveForPoint(Corner::Hardpoints::OutboardDamper, Corner::Hardpoints::LowerBallJoint,
				Corner::Hardpoints::LowerFrontTubMount, Corner::Hardpoints::LowerRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard damper!" << Debugger::Priority::Medium;
				success = false;
			}

			if (!SolveForPoint(Corner::Hardpoints::OutboardBarLink, Corner::Hardpoints::LowerBallJoint,
				Corner::Hardpoints::LowerFrontTubMount, Corner::Hardpoints::LowerRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard bar link!" << Debugger::Priority::Medium;
				success = false;
			}
		}
		else if (corner.actuationAttachment == Corner::ActuationAttachment::UpperAArm)
		{
			if (!SolveForPoint(Corner::Hardpoints::OutboardSpring, Corner::Hardpoints::UpperBallJoint,
				Corner::Hardpoints::UpperFrontTubMount, Corner::Hardpoints::UpperRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard spring!" << Debugger::Priority::Medium;
				success = false;
			}

			if (!SolveForPoint(Corner::Hardpoints::OutboardDamper, Corner::Hardpoints::UpperBallJoint,
				Corner::Hardpoints::UpperFrontTubMount, Corner::Hardpoints::UpperRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard damper!" << Debugger::Priority::Medium;
				success = false;
			}

			if (!SolveForPoint(Corner::Hardpoints::OutboardBarLink, Corner::Hardpoints::UpperBallJoint,
				Corner::Hardpoints::UpperFrontTubMount, Corner::Hardpoints::UpperRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard bar link!" << Debugger::Priority::Medium;
				success = false;
			}
		}
		else if (corner.actuationAttachment == Corner::ActuationAttachment::Upright)
		{
			if (!SolveForPoint(Corner::Hardpoints::OutboardSpring, Corner::Hardpoints::UpperBallJoint,
				Corner::Hardpoints::LowerBallJoint, Corner::Hardpoints::OutboardTieRod, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard spring!" << Debugger::Priority::Medium;
				success = false;
			}

			if (!SolveForPoint(Corner::Hardpoints::OutboardDamper, Corner::Hardpoints::UpperBallJoint,
				Corner::Hardpoints::LowerBallJoint, Corner::Hardpoints::OutboardTieRod, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard damper!" << Debugger::Priority::Medium;
				success = false;
			}

			if (!SolveForPoint(Corner::Hardpoints::OutboardBarLink, Corner::Hardpoints::UpperBallJoint,
				Corner::Hardpoints::LowerBallJoint, Corner::Hardpoints::OutboardTieRod, originalCorner, corner))
			{
				Debugger::GetInstance() << "ERROR:  Failed to solve for outboard bar link!" << Debugger::Priority::Medium;
				success = false;
			}
		}
	}

	// Half Shafts
	if ((originalCar->HasFrontHalfShafts() && isAtFront) ||
		(originalCar->HasRearHalfShafts() && !isAtFront))
	{
		if (!SolveForPoint(Corner::Hardpoints::OutboardHalfShaft, Corner::Hardpoints::LowerBallJoint,
			Corner::Hardpoints::UpperBallJoint, Corner::Hardpoints::OutboardTieRod, originalCorner, corner))
		{
			Debugger::GetInstance() << "ERROR:  Failed to solve for outboard half shaft!" << Debugger::Priority::Medium;
			success = false;
		}
	}

	const auto* originalSuspension(originalCar->GetSubsystem<Suspension>());

	// Sway Bars Inboard
	if (localSuspension->frontBarStyle == Suspension::BarStyle::UBar && isAtFront)
	{
		Eigen::Vector3d originalBarMidpoint = 0.5 *
			(originalSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)]
			+ originalSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)]);
		if (!SolveForPoint(corner.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)],
			corner.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)], localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontBarMidPoint)],
			originalCorner.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)], originalCorner.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
			originalBarMidpoint, originalCorner.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
			corner.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)]))
		{
			Debugger::GetInstance() << "ERROR:  Failed to solve for inboard U-bar (front)!" << Debugger::Priority::Medium;
			success = false;
		}
	}
	else if (localSuspension->rearBarStyle == Suspension::BarStyle::UBar && !isAtFront)
	{
		Eigen::Vector3d originalBarMidpoint = 0.5 *
			(originalSuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)]
			+ originalSuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)]);
		if (!SolveForPoint(corner.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)],
			corner.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)], localSuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearBarMidPoint)],
			originalCorner.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)], originalCorner.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
			originalBarMidpoint, originalCorner.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
			corner.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)]))
		{
			Debugger::GetInstance() << "ERROR:  Failed to solve for inboard U-bar (rear)!" << Debugger::Priority::Medium;
			success = false;
		}
	}
	else if ((localSuspension->frontBarStyle == Suspension::BarStyle::Geared && isAtFront) ||
		(localSuspension->rearBarStyle == Suspension::BarStyle::Geared && !isAtFront))
	{
		if (!SolveForPoint(Corner::Hardpoints::InboardBarLink, Corner::Hardpoints::BarArmAtPivot,
			Corner::Hardpoints::OutboardBarLink, Corner::Hardpoints::GearEndBarShaft, originalCorner, corner))
		{
			Debugger::GetInstance() << "ERROR:  Failed to solve for geared bar!" << Debugger::Priority::Medium;
			success = false;
		}
	}

	return success;
}

//==========================================================================
// Class:			Kinematics
// Function:		SolveForPoint
//
// Description:		This is a convenience wrapper around the call to the method
//					that actually performs the geometry math.
//
// Input Arguments:
//		target			= const Corner::Hardpoints& specifying the index of the point we are solving for
//		reference1		= const Corner::Hardpoints& specifying the index of the point defining the first sphere
//		reference2		= const Corner::Hardpoints& specifying the index of the point defining the second sphere
//		reference3		= const Corner::Hardpoints& specifying the index of the point defining the third sphere
//						  first sphere
//		originalCorner	= const Corner&
//		corner			= Corner&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false for error
//
//==========================================================================
bool Kinematics::SolveForPoint(const Corner::Hardpoints &target, const Corner::Hardpoints& reference1,
	const Corner::Hardpoints& reference2, const Corner::Hardpoints& reference3,
	const Corner& originalCorner, Corner& currentCorner)
{
	return SolveForPoint(currentCorner.hardpoints[static_cast<int>(reference1)], currentCorner.hardpoints[static_cast<int>(reference2)],
		currentCorner.hardpoints[static_cast<int>(reference3)], originalCorner.hardpoints[static_cast<int>(reference1)],
		originalCorner.hardpoints[static_cast<int>(reference2)], originalCorner.hardpoints[static_cast<int>(reference3)],
		originalCorner.hardpoints[static_cast<int>(target)], currentCorner.hardpoints[static_cast<int>(target)]);
}

//==========================================================================
// Class:			Kinematics
// Function:		SolveForPoint
//
// Description:		This function performs the calculations required to
//					obtain an analytical solution to the intersection of
//					three spheres problem.  The Original vector provides a
//					means of determining which of the two solutions is the
//					correct one.  If there is no solution (will only occur
//					if suspension would bind, etc.), a warning is returned.
//					More detail on the method used to obtain the solution
//					is below.  In the event of an error, the original value
//					is returned.
//
// Input Arguments:
//		center1			= const Eigen::Vector3d& specifying the center of the first sphere
//		center2			= const Eigen::Vector3d& specifying the center of the second sphere
//		center3			= const Eigen::Vector3d& specifying the center of the third sphere
//		originalCenter1	= const Eigen::Vector3d& specifying the original center of the
//						  first sphere
//		originalCenter2	= const Eigen::Vector3d& specifying the original center of the
//						  second sphere
//		originalCenter3	= const Eigen::Vector3d& specifying the original center of the
//						  third sphere
//		original		= const Eigen::Vector3d& specifying the original location of the point
//						  we are solving for
//
// Output Arguments:
//		current	= Eigen::Vector3d& specifying the result of the intersection of
//				  three spheres algorithm
//
// Return Value:
//		bool, true for success, false for error
//
//==========================================================================
bool Kinematics::SolveForPoint(const Eigen::Vector3d &center1, const Eigen::Vector3d &center2,
	const Eigen::Vector3d &center3, const Eigen::Vector3d &originalCenter1,
	const Eigen::Vector3d &originalCenter2, const Eigen::Vector3d &originalCenter3,
	const Eigen::Vector3d &original, Eigen::Vector3d &current)
{
	GeometryMath::Sphere s1;
	GeometryMath::Sphere s2;
	GeometryMath::Sphere s3;

	s1.center = center1;
	s1.radius = (originalCenter1 - original).norm();
	s2.center = center2;
	s2.radius = (originalCenter2 - original).norm();
	s3.center = center3;
	s3.radius = (originalCenter3 - original).norm();

	Eigen::Vector3d intersections[2];
	if (!GeometryMath::FindThreeSpheresIntersection(s1, s2, s3, intersections))
	{
		Debugger::GetInstance() << "Error (SolveForPoint):  Solution does not exist" << Debugger::Priority::Low;
		return false;
	}

	if (intersections[0] != intersections[0] || intersections[1] != intersections[1])
	{
		Debugger::GetInstance() << "Error (SolveForPoint):  Invalid solution" << Debugger::Priority::Low;
		return false;
	}

	// We now have two solutions.  To choose between them, we must examine the original
	// location of the point and the original sphere centers.  The three sphere centers
	// define a plane, and the the two solutions lie on opposite sides of the plane.
	// The first step in identifying the correct solution is to determine which side of the
	// original plane the original point was on, and which side of the new plane either one
	// of the solutions is on.

	// Get the plane normals
	const Eigen::Vector3d originalNormal = VVASE::Math::GetPlaneNormal(originalCenter1, originalCenter2, originalCenter3);
	const Eigen::Vector3d newNormal = VVASE::Math::GetPlaneNormal(center1, center2, center3);

	// Get a vector from the location of the point to some point in the plane
	const Eigen::Vector3d originalVectorToPlane = originalCenter1 - original;
	const Eigen::Vector3d newVectorToPlane = center1 - intersections[0];

	// The dot products of the normal and the vector to the plane will give an indication
	// of which side of the plane the point is on
	const double originalSide(originalNormal.dot(originalVectorToPlane));
	const double newSide(newNormal.dot(newVectorToPlane));

	// We can compare the sign of the original side with the new side to choose the correct solution
	if ((newSide > 0 && originalSide > 0) || (newSide < 0 && originalSide < 0))
		current = intersections[0];
	else
		current = intersections[1];

	return true;
}

//==========================================================================
// Class:			Kinematics
// Function:		MoveSteeringRack
//
// Description:		This function moves the two inboard points that
//					represent the ends of the steering rack.  Travel is in
//					inches.  Positive travel moves the rack to the right.
//
// Input Arguments:
//		travel	= const double& specifying the distance to move the steering rack [in]
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Kinematics::MoveSteeringRack(const double &travel) const
{
	// The two front inboard tie-rod locations will define the axis along which the
	// rack will slide.  Both points get moved by Travel in the same direction.
	// Here we'll use a parametric equation of the line in 3-space for convenience.

	double t = 1.0;// Parametric parameter
	Eigen::Vector3d slope;

	slope = localSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)]
		-localSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)];

	// We need to calibrate t to the distance between the rack ends so we know how
	// much t we need to move the rack ends by Travel.  Using the left end as the
	// reference, we have the following parametric equations:
	//    x = left.x + Slope.x * t
	//    y = left.y + Slope.y * t
	//    z = left.z + Slope.z * t
	// So with t = 0, we have the original location of the left end of the rack,
	// and if t = 1 we'll get the right end of the rack.

	// We can normalize t to the distance between the ends of the rack and we'll
	// have t with units [1/in].
	t /= (localSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)] -
		localSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)]).norm();

	// Now we multiply by how far we actually want the rack to move...
	t *= travel;// t is now unitless again

	// And we're ready to move the rack.  Since the slope and t calibration are the
	// same for both points, we can just use each point as their own reference points.
	localSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)] += slope * t;
	localSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)] += slope * t;
}

//==========================================================================
// Class:			Kinematics
// Function:		SolveForXY
//
// Description:		Easier-to-call method to interface between Kinematics and
//					the actual solving method.
//
// Input Arguments:
//		target			= const Corner::Hardpoints&
//		reference1		= const Corner::Hardpoints&
//		reference2		= const Corner::Hardpoints&
//		originalCorner	= const Corner&
//		currentCorner	= const Corner&
//
// Output Arguments:
//		current	= Eigen::Vector3d& specifying the result of the intersection of
//				  three spheres algorithm
//
// Return Value:
//		bool, true for success, false for error
//
//==========================================================================
bool Kinematics::SolveForXY(const Corner::Hardpoints &target,
	const Corner::Hardpoints& reference1, const Corner::Hardpoints& reference2,
	const Corner& originalCorner, Corner& currentCorner)
{
	return SolveForXY(currentCorner.hardpoints[static_cast<int>(reference1)], currentCorner.hardpoints[static_cast<int>(reference2)],
		originalCorner.hardpoints[static_cast<int>(reference1)], originalCorner.hardpoints[static_cast<int>(reference2)],
		originalCorner.hardpoints[static_cast<int>(target)], currentCorner.hardpoints[static_cast<int>(target)]);
}

//==========================================================================
// Class:			Kinematics
// Function:		SolveForXY
//
// Description:		This is a modification of the solver for the rest of
//					the suspension hardpoints.  Here, the Z component of
//					current is assumed to be correct and is never modified.
//					Otherwise, it works the same way as SolveForPoint.  In
//					the event of an error, the original value is returned.
//
// Input Arguments:
//		center1			= const Eigen::Vector3d& specifying the center of the first sphere
//		center2			= const Eigen::Vector3d& specifying the center of the second sphere
//		originalCenter1	= const Eigen::Vector3d& specifying the original center of the
//						  first sphere
//		originalCenter2	= const Eigen::Vector3d& specifying the original center of the
//						  second sphere
//		original		= const Eigen::Vector3d& specifying the original location of the point
//						  we are solving for
//
// Output Arguments:
//		current	= Eigen::Vector3d& specifying the result of the intersection of
//				  three spheres algorithm
//
// Return Value:
//		bool, true for success, false for error
//
//==========================================================================
bool Kinematics::SolveForXY(const Eigen::Vector3d &center1, const Eigen::Vector3d &center2,
	const Eigen::Vector3d &originalCenter1, const Eigen::Vector3d &originalCenter2,
	const Eigen::Vector3d &original, Eigen::Vector3d &current)
{
	GeometryMath::Sphere s1;
	GeometryMath::Sphere s2;
	s1.center = center1;
	s1.radius = (originalCenter1 - original).norm();
	s2.center = center2;
	s2.radius = (originalCenter2 - original).norm();

	GeometryMath::Plane p1;
	p1.point = current;
	p1.normal = Eigen::Vector3d(0.0, 0.0, 1.0);

	GeometryMath::Plane p2 = GeometryMath::FindSphereSphereIntersectionPlane(s1, s2);
	GeometryMath::Axis a;
	if (!GeometryMath::FindPlanePlaneIntersection(p1, p2, a))
	{
		Debugger::GetInstance() << "Error (SolveForXY):  Solution does not exist" << Debugger::Priority::Low;
		return false;
	}

	Eigen::Vector3d intersections[2];
	if (!GeometryMath::FindAxisSphereIntersections(a, s1, intersections))
	{
		Debugger::GetInstance() << "Error (SolveForXY):  Solution does not exist" << Debugger::Priority::Low;
		return false;
	}

	if (intersections[0] != intersections[0] || intersections[1] != intersections[1])
	{
		Debugger::GetInstance() << "Error (SolveForXY):  Invalid solution" << Debugger::Priority::Low;
		return false;
	}

	// We now have two solutions.  To choose between them, we must examine the original
	// location of the point and the original sphere centers.  The two sphere centers
	// define a plane (assume plane is perpendicular to the ground plane), and the the two
	// solutions lie on opposite sides of the plane.  The first step in identifying the
	// correct solution is to determine which side of the original plane the original
	// point was on, and which side of the new plane either one of the solutions is on.

	// Get the plane normals
	Eigen::Vector3d pointInPlane = originalCenter1;
	pointInPlane.z() = 0.0;
	const Eigen::Vector3d originalNormal = (originalCenter1 - originalCenter2).cross(originalCenter1 - pointInPlane);
	pointInPlane = center1;
	pointInPlane.z() = 0.0;
	const Eigen::Vector3d newNormal = (center1 - center2).cross(center1 - pointInPlane);

	// Get a vector from the location of the point to some point in the plane
	const Eigen::Vector3d originalVectorToPlane = originalCenter1 - original;
	const Eigen::Vector3d newVectorToPlane = center1 - intersections[0];

	// The dot products of the normal and the vector to the plane will give an indication
	// of which side of the plane the point is on
	const double originalSide(originalNormal.dot(originalVectorToPlane));
	const double newSide(newNormal.dot(newVectorToPlane));

	// We can compare the sign of the original side with the new side to choose the correct solution
	if ((newSide > 0 && originalSide > 0) || (newSide < 0 && originalSide < 0))
		current = intersections[0];
	else
		current = intersections[1];

	return true;
}

//==========================================================================
// Class:			Kinematics
// Function:		SolveForContactPatch
//
// Description:		This function solves for the circle created by the
//					intersection of a sphere of radius TireRadius centered
//					around point WheelCenter, with the plane that passes
//					through WheelCenter and has normal vector
//					WheelPlaneNormal.  The vector returned by the function
//					is the point on the circle with minimum Z value.  In
//					the event of an error, a zero length vector is returned.
//
// Input Arguments:
//		wheelCenter			= const Eigen::Vector3d& specifying the center of the wheel
//		wheelPlaneNormal	= const Eigen::Vector3d& specifying the orientation of the wheel
//		tireRadius			= const double& specifying the loaded radius of the tire
//
// Output Arguments:
//		output	= Eigen::Vector3d& specifying the location of the center of the tire's
//				  contact patch
//
// Return Value:
//		bool, true for success, false for error
//
//==========================================================================
bool Kinematics::SolveForContactPatch(const Eigen::Vector3d &wheelCenter,
	const Eigen::Vector3d &wheelPlaneNormal, const double &tireRadius, Eigen::Vector3d &output)
{
	Eigen::Vector3d minimumZPoint;

	// Equation for sphere as described above:
	// (x - WheelCenter.x)^2 + (y - WheelCenter.y)^2 + (z - WheelCenter.z)^2 - TireRadius^2 = 0
	// Equation for plane as described above:
	// x * WheelPlaneNormal.x + y * WheelPlaneNormal.y + z * WheelPlaneNormal.z - WheelCenter * WheelPlaneNormal = 0
	// Solving the plane equation for y yields an equation for y in terms of x and z.
	// y was chosen here, because WheelPlaneNormal is least likely to have a zero Y component,
	// versus X or Z.
	// y(x, z) = (x * WheelPlaneNormal.x + z * WheelPlaneNormal.z - WheelCenter * WheelPlaneNormal) / WheelPlaneNormal.y
	// Next, we substitute this equation for y back into the sphere equation, leaving us with a
	// quadratic function for z in terms of x.  We now have all of the components necessary to describe
	// the circle we originally set out to find -> z(x) and y(x, z).  To find our minimum Z point, we
	// differentiate z(x) and set equal to zero.  This gives us x as a function of the inputs to
	// SolveForContactPatch only.
	minimumZPoint.x() = (wheelCenter.x() * (pow(wheelPlaneNormal.x(), 4) + 2 * pow(wheelPlaneNormal.x(), 2)
		* pow(wheelPlaneNormal.y(), 2) + pow(wheelPlaneNormal.y(), 4) + pow(wheelPlaneNormal.x(), 2)
		* pow(wheelPlaneNormal.z(), 2) + pow(wheelPlaneNormal.y(), 2) * pow(wheelPlaneNormal.z(), 2))
		- sqrt(pow(tireRadius * wheelPlaneNormal.x() * wheelPlaneNormal.z(), 2)	* (pow(wheelPlaneNormal.x(), 4)
		+ 2 * pow(wheelPlaneNormal.x(), 2) * pow(wheelPlaneNormal.y(), 2) + pow(wheelPlaneNormal.y(), 4)
		+ pow(wheelPlaneNormal.x() * wheelPlaneNormal.z(), 2) + pow(wheelPlaneNormal.y() * wheelPlaneNormal.z(), 2))))
		/ (pow(wheelPlaneNormal.x(), 4) + 2 * pow(wheelPlaneNormal.x() * wheelPlaneNormal.y(), 2)
		+ pow(wheelPlaneNormal.y(), 4) + pow(wheelPlaneNormal.x() * wheelPlaneNormal.z(), 2)
		+ pow(wheelPlaneNormal.y() * wheelPlaneNormal.z(), 2));

	// Now we can plug back into our equations for Y and Z.  For Z, we'll solve the quadratic with
	// the quadratic equation.  Since A is always positive, we know we'll always want the minus
	// solution when we're looking for the minimum.
	double a, b, c;
	a = 1 + pow(wheelPlaneNormal.z() / wheelPlaneNormal.y(), 2);
	b = 2 * (wheelPlaneNormal.z() / wheelPlaneNormal.y() * wheelCenter.y() - wheelCenter.z() - wheelPlaneNormal.z()
		/ pow(wheelPlaneNormal.y(), 2) * (wheelCenter.dot(wheelPlaneNormal) - wheelPlaneNormal.x() * minimumZPoint.x()));
	c = pow(minimumZPoint.x() - wheelCenter.x(), 2) + pow(wheelCenter.dot(wheelPlaneNormal) - wheelPlaneNormal.x()
		* minimumZPoint.x(), 2) / pow(wheelPlaneNormal.y(), 2) - 2 * wheelCenter.y() / wheelPlaneNormal.y()
		* (wheelCenter.dot(wheelPlaneNormal) - wheelPlaneNormal.x() * minimumZPoint.x()) + pow(wheelCenter.y(), 2)
		+ pow(wheelCenter.z(), 2) - pow(tireRadius, 2);
	minimumZPoint.z() = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
	minimumZPoint.y() = (wheelCenter.dot(wheelPlaneNormal) - minimumZPoint.x() * wheelPlaneNormal.x()
		- minimumZPoint.z() * wheelPlaneNormal.z()) / wheelPlaneNormal.y();

	// Check to make sure the solution is valid
	if (minimumZPoint != minimumZPoint)
	{
		output.setZero();
		Debugger::GetInstance() << "Error (SolveForContactPatch):  Invalid solution" << Debugger::Priority::Low;
		return false;
	}

	output = minimumZPoint;

	return true;
}

//==========================================================================
// Class:			Kinematics
// Function:		FindPerpendicularVector
//
// Description:		Returns a vector perpendicular to the specified vector.
//					Solves x dot v = 0 for x.
//
// Input Arguments:
//		v	= Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		Eigen::Vector3d
//
//==========================================================================
// Solves the equation a dot v = 0 for a
Eigen::Vector3d Kinematics::FindPerpendicularVector(const Eigen::Vector3d &v)
{
	// a dot v => ax * vx + ay * vy + az * vz = 0
	// Find the minimum element of v; set the corresponding element of a to zero
	// Find the second minimum element of v; set the corresponding element of a to one
	// Solve for the remaining element of a
	Eigen::Vector3d a;
	if (fabs(v.x()) < fabs(v.y()) && fabs(v.x()) < fabs(v.z()))// x smallest
	{
		a.x() = 0;
		if (fabs(v.y()) < fabs(v.z()))// y second smallest
		{
			a.y() = 1.0;
			a.z() = -v.y() / v.z();
		}
		else// z second smallest
		{
			a.z() = 1.0;
			a.y() = -v.z() / v.y();
		}
	}
	else if (fabs(v.y()) < v.z())// y smallest
	{
		a.y() = 0;
		if (fabs(v.x()) < fabs(v.z()))// x second smallest
		{
			a.x() = 1.0;
			a.z() = -v.x() / v.z();
		}
		else// z second smallest
		{
			a.z() = 1.0;
			a.x() = -v.z() / v.x();
		}
	}
	else// z smallest
	{
		a.z() = 0;
		if (fabs(v.x()) < fabs(v.y()))// x second smallest
		{
			a.x() = 1.0;
			a.y() = -v.x() / v.y();
		}
		else// y second smallest
		{
			a.y() = 1.0;
			a.x() = -v.y() / v.x();
		}
	}

	return a;
}

//==========================================================================
// Class:			Kinematics
// Function:		OptimizeCircleParameter
//
// Description:		Finds reasonable defaults for starting the Newton-Raphson
//					method for T-bar solution.
//
// Input Arguments:
//		center	= const Eigen::Vector3d& specifying the circle center
//		a		= const Eigen::Vector3d& specifying a vector having length equal to circle radius and lying in the circle plane
//		b		= const Eigen::Vector3d& specifying a vector having length equal to circle radius and lying in the circle plane, perpendicular to a
//		target	= const Eigen::Vector3d& original location of point we are solving for
//
// Output Arguments:
//		None
//
// Return Value:
//		dobule specifying parameter of parametric cirlce equation
//
//==========================================================================
double Kinematics::OptimizeCircleParameter(const Eigen::Vector3d &center, const Eigen::Vector3d &a,
	const Eigen::Vector3d &b, const Eigen::Vector3d &target)
{
	double t;
	const unsigned int steps(12);
	const double step(2.0 * M_PI / steps);
	double bestT(-1.0), bestError(0.0);
	unsigned int i;
	Eigen::Vector3d p;
	for (i = 0; i < steps; i++)
	{
		t = step * i;
		p = center + a * cos(t) + b * sin(t);
		if (bestT < 0.0 || (target - p).norm() < bestError)
		{
			bestT = t;
			bestError = (target - p).norm();
		}
	}

	return bestT;
}

//==========================================================================
// Class:			Kinematics
// Function:		SolveInboardTBarPoints
//
// Description:		Solves for three points across the top of the "T" that must
//					be found simultaneously.
//
// Input Arguments:
//		leftOutboard			= const Eigen::Vector3d&
//		rightOutboard			= const Eigen::Vector3d&
//		centerPivot				= const Eigen::Vector3d&
//		pivotAxisPoint			= const Eigen::Vector3d&
//		originalLeftOutboard	= const Eigen::Vector3d&
//		originalRightOutboard	= const Eigen::Vector3d&
//		originalCenterPivot		= const Eigen::Vector3d&
//		originalPivotAxisPoint	= const Eigen::Vector3d&
//		originalLeftInboard		= const Eigen::Vector3d&
//		originalRightInboard	= const Eigen::Vector3d&
//
// Output Arguments:
//		leftInboard				= Eigen::Vector3d&
//		rightInboard			= Eigen::Vector3d&
//
// Return Value:
//		bool, true for success, false for error
//
//==========================================================================
bool Kinematics::SolveInboardTBarPoints(const Eigen::Vector3d &leftOutboard,
	const Eigen::Vector3d &rightOutboard, const Eigen::Vector3d &centerPivot, const Eigen::Vector3d &pivotAxisPoint,
	const Eigen::Vector3d &originalLeftOutboard, const Eigen::Vector3d &originalRightOutboard,
	const Eigen::Vector3d &originalCenterPivot, const Eigen::Vector3d &originalPivotAxisPoint,
	const Eigen::Vector3d &originalLeftInboard, const Eigen::Vector3d &originalRightInboard,
	Eigen::Vector3d &leftInboard, Eigen::Vector3d &rightInboard)
{
	// We'll use parametric version of 3D circle equation
	// Additional parameters a and b are orthogonal to each other and circle plane normal
	// a and b have length equal to circle radius
	Eigen::Vector3d normal;
	double angle, f, g;// f, g and angle are for law of cosines calcs
	double radius;

	Eigen::Vector3d leftCenter, leftA, leftB;
	// on the left
	// sphere1 -> center = leftOutboard, R = leftOutboard - leftInboard
	// sphere2 -> center = centerPivot, R = centerPivot - leftInboard
	// circle1 -> intersection of sphere1 and sphere2
	normal = leftOutboard - centerPivot;
	if ((leftOutboard - centerPivot).norm() > normal.norm())
	{
		Debugger::GetInstance() << "Error (SolveInboardTBarPoints): Center distance exceeds sum of left radii" << Debugger::Priority::Low;
		return false;
	}
	f = (originalLeftOutboard - originalLeftInboard).norm();
	g = (originalCenterPivot - originalLeftInboard).norm();
	angle = acos((f * f + normal.norm() * normal.norm() - g * g) * 0.5 / f / normal.norm());
	leftCenter = leftOutboard - f * cos(angle) * normal.normalized();
	radius = f * sin(angle);
	leftA = FindPerpendicularVector(normal);
	leftB = leftA.cross(normal);
	leftA = leftA.normalized() * radius;
	leftB = leftB.normalized() * radius;

/*#ifdef USE_DEBUG_SHAPE
	DebugShape::Get()->SetSphere1(leftOutboard, (originalLeftOutboard - originalLeftInboard).Length());
	DebugShape::Get()->SetSphere2(centerPivot, (originalCenterPivot - originalLeftInboard).Length());
	//DebugShape::Get()->SetSphere3(rightOutboard, (originalRightOutboard - originalRightInboard).Length());
#endif*/

	Eigen::Vector3d rightCenter, rightA, rightB;
	// on the right
	// sphere1 -> center = rightOutboard, R = rightOutboard - rightInboard
	// sphere2 -> center = centerPivot, R = centerPivot - rightInboard
	// circle2 -> intersection of sphere1 and sphere2
	normal = rightOutboard - centerPivot;
	if ((rightOutboard - centerPivot).norm() > normal.norm())
	{
		Debugger::GetInstance() << "Error (SolveInboardTBarPoints): Center distance exceeds sum of right radii" << Debugger::Priority::Low;
		return false;
	}
	f = (originalRightOutboard - originalRightInboard).norm();
	g = (originalCenterPivot - originalRightInboard).norm();
	angle = acos((f * f + normal.norm() * normal.norm() - g * g) * 0.5 / f / normal.norm());
	rightCenter = rightOutboard - f * cos(angle) * normal.normalized();
	radius = f * sin(angle);
	rightA = FindPerpendicularVector(normal);
	rightB = rightA.cross(normal);
	rightA = rightA.normalized() * radius;
	rightB = rightB.normalized() * radius;

	Eigen::Vector3d centerA, centerB;
	// in the center
	// plane1 -> normal = centerPivot - pivotAxisPoint
	// point1 -> where line (leftInboard - rightInboard) intersects plane1
	// circle3 -> center = centerPivot, normal = centerPivot - pivotAxisPoint, R = (centerPivot - point1).Length()
	normal = originalCenterPivot - originalPivotAxisPoint;
	Eigen::Vector3d originalTopMidPoint = VVASE::Math::IntersectWithPlane(normal, originalCenterPivot,
		originalLeftInboard - originalRightInboard, originalLeftInboard);
	normal = centerPivot - pivotAxisPoint;
	radius = (originalCenterPivot - originalTopMidPoint).norm();
	centerA = FindPerpendicularVector(normal);
	centerB = centerA.cross(normal);
	centerA = centerA.normalized() * radius;
	centerB = centerB.normalized() * radius;

/*#ifdef USE_DEBUG_SHAPE
	DebugShape::Get()->SetDisk1(leftCenter, leftA.Cross(leftB), leftA.Length(), leftA.Length() * 0.9);
	DebugShape::Get()->SetDisk2(rightCenter, rightA.Cross(rightB), rightA.Length(), rightA.Length() * 0.9);
	DebugShape::Get()->SetDisk3(centerPivot, centerA.Cross(centerB), centerA.Length(), centerA.Length() * 0.9);
#endif*/

	// solution satisfies:
	// - pLeft lies on circle1
	// - pRight lies on circle2
	// - pCenter lies on circle3
	// - pLeft, pRight and pCenter are colinear
	// - distance from pLeft to pCenter and pRight to pCenter match originals

	double leftTopLength = (originalLeftInboard - originalTopMidPoint).norm();
	double rightTopLength = (originalRightInboard - originalTopMidPoint).norm();

	// It's possible that a closed-form solution exists, but let's try an iterative method
	unsigned int i(0);
	const unsigned int limit(100);
	const double epsilon(1.0e-8);
	Eigen::Vector3d error(Eigen::Vector3d::Ones() * epsilon);
	Eigen::Matrix3d jacobian(3,3);
	Eigen::Vector3d guess;// parameteric variables for the three points
	Eigen::Vector3d left(Eigen::Vector3d::Zero()), right(Eigen::Vector3d::Zero()), center(Eigen::Vector3d::Zero());
	Eigen::Vector3d delta;

	// Initialize parameteric variables such that result aligns as best
	// as possible with original point locations
	guess(0,0) = OptimizeCircleParameter(leftCenter, leftA, leftB, originalLeftInboard);
	guess(1,0) = OptimizeCircleParameter(rightCenter, rightA, rightB, originalRightInboard);
	guess(2,0) = OptimizeCircleParameter(centerPivot, centerA, centerB, originalTopMidPoint);

	while (i < limit && fabs(error(0,0)) + fabs(error(1,0)) + fabs(error(2,0)) > epsilon)
	{
		left = leftCenter + leftA * cos(guess(0,0)) + leftB * sin(guess(0,0));
		right = rightCenter + rightA * cos(guess(1,0)) + rightB * sin(guess(1,0));
		center = centerPivot + centerA * cos(guess(2,0)) + centerB * sin(guess(2,0));

		error(0,0) = (left - center).norm() - leftTopLength;
		error(1,0) = (right - center).norm() - rightTopLength;
		error(2,0) = (left - right).norm() - leftTopLength - rightTopLength;
		//Debugger::GetInstance().Print(Debugger::Priority::Low, "i = %u; error = \n%s;\nguess = \n%s", i, error.Print().ToUTF8().data(), guess.Print().ToUTF8().data());

		// Compute jacobian
		left = leftCenter + leftA * cos(guess(0,0) + epsilon) + leftB * sin(guess(0,0) + epsilon);
		jacobian(0,0) = ((left - center).norm() - leftTopLength - error(0,0)) / epsilon;
		jacobian(1,0) = ((right - center).norm() - rightTopLength - error(1,0)) / epsilon;
		jacobian(2,0) = ((left - right).norm() - leftTopLength - rightTopLength - error(2,0)) / epsilon;
		left = leftCenter + leftA * cos(guess(0,0)) + leftB * sin(guess(0,0));

		right = rightCenter + rightA * cos(guess(1,0) + epsilon) + rightB * sin(guess(1,0) + epsilon);
		jacobian(0,1) = ((left - center).norm() - leftTopLength - error(0,0)) / epsilon;
		jacobian(1,1) = ((right - center).norm() - rightTopLength - error(1,0)) / epsilon;
		jacobian(2,1) = ((left - right).norm() - leftTopLength - rightTopLength - error(2,0)) / epsilon;
		right = rightCenter + rightA * cos(guess(1,0)) + rightB * sin(guess(1,0));

		center = centerPivot + centerA * cos(guess(2,0) + epsilon) + centerB * sin(guess(2,0) + epsilon);
		jacobian(0,2) = ((left - center).norm() - leftTopLength - error(0,0)) / epsilon;
		jacobian(1,2) = ((right - center).norm() - rightTopLength - error(1,0)) / epsilon;
		jacobian(2,2) = ((left - right).norm() - leftTopLength - rightTopLength - error(2,0)) / epsilon;
		center = centerPivot + centerA * cos(guess(2,0)) + centerB * sin(guess(2,0));

		// Compute next guess
		delta = jacobian.colPivHouseholderQr().solve(error);
		guess -= delta;

		i++;
	}

	if (i == limit)
		Debugger::GetInstance() << "Warning:  Iteration limit reached (SolveInboardTBarPoints)" << Debugger::Priority::Medium;

	rightInboard = right;
	leftInboard = left;

/*#ifdef USE_DEBUG_SHAPE
	DebugShape::Get()->SetPoint1(left);
	DebugShape::Get()->SetPoint2(right);
	DebugShape::Get()->SetPoint3(center);
#endif*/

	// Check constraints
	if (!VVASE::Math::IsZero((left - center).norm() - leftTopLength, epsilon))
	{
		Debugger::GetInstance() << "Warning:  Incorrect left top T-bar length (Error = "
			<< (left - center).norm() - leftTopLength << ")" << Debugger::Priority::Low;
		return false;
	}

	if (!VVASE::Math::IsZero((right - center).norm() - rightTopLength, epsilon))
	{
		Debugger::GetInstance() << "Warning:  Incorrect right top T-bar length (Error = "
			<< (right - center).norm() - rightTopLength << ")" << Debugger::Priority::Low;
		return false;
	}

	if (!VVASE::Math::IsZero((right - left).norm() - leftTopLength - rightTopLength, epsilon))
	{
		Debugger::GetInstance() << "Warning:  Incorrect top T-bar length (Error = " <<
			(right - left).norm() - leftTopLength - rightTopLength << ")" << Debugger::Priority::Low;
		return false;
	}

	return true;
}

//==========================================================================
// Class:			Kinematics
// Function:		UpdateCGs
//
// Description:		Updates the center-of-gravity heights.
//
// Input Arguments:
//		workingCar		= Car*
//
// Output Arguments:
//		workingCar	= Car*
//
// Return Value:
//		bool, true for success, false for error
//
//==========================================================================
void Kinematics::UpdateCGs(Car* workingCar) const
{
	auto* massProperties(workingCar->GetSubsystem<MassProperties>());
	Eigen::Vector3d sprungCG(massProperties->GetSprungMassCG(workingCar->GetSubsystem<Suspension>()));

	// NOTE:  Unsprung CG height is assumed to change only due
	// to tire compliance (tire/upright assembly rotation is not considered)

	massProperties->unsprungCGHeights.leftFront -= inputs.tireDeflections.leftFront;
	massProperties->unsprungCGHeights.rightFront -= inputs.tireDeflections.rightFront;
	massProperties->unsprungCGHeights.leftRear -= inputs.tireDeflections.leftRear;
	massProperties->unsprungCGHeights.rightRear -= inputs.tireDeflections.rightRear;

	ApplyRotations(sprungCG);
	sprungCG.z() += inputs.heave;
	massProperties->totalCGHeight = (sprungCG.z() * massProperties->GetSprungMass()
		+ massProperties->unsprungCGHeights.leftFront * massProperties->unsprungMass.leftFront
		+ massProperties->unsprungCGHeights.rightFront * massProperties->unsprungMass.rightFront
		+ massProperties->unsprungCGHeights.leftRear * massProperties->unsprungMass.leftRear
		+ massProperties->unsprungCGHeights.rightRear * massProperties->unsprungMass.rightRear)
		/ massProperties->GetTotalMass();
}

void Kinematics::ApplyRotations(Eigen::Vector3d& v) const
{
	if (inputs.sequence == RotationSequence::PitchRoll)
	{
		GeometryMath::Rotate(v, inputs.centerOfRotation, inputs.pitch, Eigen::Vector3d::UnitY());
		GeometryMath::Rotate(v, inputs.centerOfRotation, inputs.roll, Eigen::Vector3d::UnitX());
	}
	else//RotationSequence::RollPitch
	{
		GeometryMath::Rotate(v, inputs.centerOfRotation, inputs.roll, Eigen::Vector3d::UnitX());
		GeometryMath::Rotate(v, inputs.centerOfRotation, inputs.pitch, Eigen::Vector3d::UnitY());
	}
}

wxPanel* Kinematics::GetEditPanel()
{
	// TODO:  Implement
	return nullptr;
}

wxPanel* Kinematics::GetNotebookPage()
{
	// TODO:  Implement
	return nullptr;
}

wxTreeNode* Kinematics::GetTreeNode()
{
	// TODO:  Implement
	return nullptr;
}

OptimizationInterface* Kinematics::GetOptimizationInterface()
{
	// TODO:  Implement
	return nullptr;
}

bool Kinematics::Compute(const Car& car)
{
	// TODO:  Implement
	return false;
}

}// namespace VVASE
