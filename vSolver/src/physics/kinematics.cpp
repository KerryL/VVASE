/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  kinematics.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionality for static state class.  This is where
//				 most of the functionality of the suspension kinematics end of things
//				 will be.
// History:
//	2/24/2008	- Made major changes to class when Drivetrain object was removed.  Removed
//				  all dynamic memory allocation from constructor, K. Loux.
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	3/15/2008	- Finished contact patch line search algorithm, K. Loux.
//	3/23/2008	- Changed units for class members and functions from degrees to radians and
//				  renamed class from Kinematics.  Also introduce Outputs class and Corner class
//				  (major restructuring of kinematic solvers), K. Loux.
//	3/24/2009	- Moved (physical location) to physics folder, K. Loux.
//	4/19/2009	- Added threading for SolveCorner(), K. Loux.
//	4/21/2009	- Removed threading (it's slower), K. Loux.

// wxWidgets headers
#include <wx/wx.h>
#include <wx/stopwatch.h>

// VVASE headers
#include "vSolver/physics/kinematics.h"
#include "vCar/car.h"
#include "vCar/corner.h"
#include "vCar/drivetrain.h"
#include "vCar/suspension.h"
#include "vMath/carMath.h"
#include "vUtilities/wheelSetStructures.h"
#include "vUtilities/debugLog.h"
#include "vUtilities/debugger.h"
#include "vMath/matrix.h"
#include "vRenderer/3dcar/debugShape.h"

//==========================================================================
// Class:			Kinematics
// Function:		Kinematics
//
// Description:		Constructor for Kinematics class.
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
Kinematics::Kinematics()
{
	originalCar = NULL;
	workingCar = NULL;
	localSuspension = NULL;
}

//==========================================================================
// Class:			Kinematics
// Function:		UpdateKinematics
//
// Description:		This updates the position of the car to meet the current
//					values of pitch, roll, heave, and steer.
//
// Input Arguments:
//		_originalCar	= const Car* - for reference only
//		_workingCar		= Car* to be changed (updated) by this function
//		name			= wxString used to print messages about this car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Kinematics::UpdateKinematics(const Car* _originalCar, Car* _workingCar, wxString name)
{
	wxStopWatch timer;
	timer.Start();

	Debugger::GetInstance().Print(_T("UpdateKinematics() for ") + name, Debugger::PriorityMedium);

	originalCar = _originalCar;// This one is for reference and won't be changed by this class
	workingCar = _workingCar;

	// Ensure exclusive access to the car objects
	// NOTE:  Always lock working car first, then lock original car (consistency required to prevent deadlocks)
	wxMutexLocker workingLock(workingCar->GetMutex());
	DebugLog::GetInstance()->Log(_T("Kinematics::UpdateKinematics (workingLock)"));
	wxMutexLocker originalLock(originalCar->GetMutex());
	DebugLog::GetInstance()->Log(_T("Kinematics::UpdateKinematics (originalLock)"));
	
	// Copy the information in the original car to the working car.  This minimizes rounding
	// errors in the calculation of suspension points, and it also ensures that changes made
	// to other sub-systems are carried over into the working car.
	*workingCar = *originalCar;

	// Now we copy the pointer to the working car's suspension to the class member
	// This must be AFTER *WorkingCar = *OriginalCar, since this assignment is a deep copy
	// and the address of the suspension will change!
	localSuspension = workingCar->suspension;

	// Rotate the steering wheel
	MoveSteeringRack(inputs.rackTravel);

	// FIXME:  As it is right now, this section is not compatible with monoshocks

	Vector rotations;// (X = First, Y = Second, Z = Third)
	Vector::Axis secondRotation;

	// Determine the order to perform the Euler rotations
	if (inputs.firstRotation == Vector::AxisX)// Do roll first, then pitch
	{
		secondRotation = Vector::AxisY;
		rotations.Set(inputs.roll, inputs.pitch, 0.0);
	}
	else if (inputs.firstRotation == Vector::AxisY)// Do pitch first, then roll
	{
		secondRotation = Vector::AxisX;
		rotations.Set(inputs.pitch, inputs.roll, 0.0);
	}
	else
	{
		Debugger::GetInstance().Print(
			_T("ERROR (UpdateKinematics):  Unable to determine order of Euler rotations"));
		return;
	}

	// Solve the chassis-mounted points for left-right common items (3rd springs/shocks, sway bars)
	// Front sway bars
	// NOTE:  This is only for points that are NOT in the Corner class
	switch (localSuspension->frontBarStyle)
	{
	case Suspension::SwayBarUBar:
		localSuspension->hardpoints[Suspension::FrontBarMidPoint] =
			0.5 * (localSuspension->leftFront.hardpoints[Corner::BarArmAtPivot]
			+ localSuspension->rightFront.hardpoints[Corner::BarArmAtPivot]);

			// Fall through

	case Suspension::SwayBarTBar:
		// Rotations
		localSuspension->hardpoints[Suspension::FrontBarMidPoint].Rotate(inputs.centerOfRotation, rotations,
			inputs.firstRotation, secondRotation);
		localSuspension->hardpoints[Suspension::FrontBarPivotAxis].Rotate(inputs.centerOfRotation, rotations,
			inputs.firstRotation, secondRotation);

		// Translations
		localSuspension->hardpoints[Suspension::FrontBarMidPoint].z += inputs.heave;
		localSuspension->hardpoints[Suspension::FrontBarPivotAxis].z += inputs.heave;
		break;

	case Suspension::SwayBarGeared:
	case Suspension::SwayBarNone:
	default:
		// Nothing here
		break;
	}

	// Rear sway bars
	switch (localSuspension->rearBarStyle)
	{
	case Suspension::SwayBarUBar:
		localSuspension->hardpoints[Suspension::RearBarMidPoint] =
			0.5 * (localSuspension->leftRear.hardpoints[Corner::BarArmAtPivot]
			+ localSuspension->rightRear.hardpoints[Corner::BarArmAtPivot]);

			// Fall through

	case Suspension::SwayBarTBar:
		// Rotations
		localSuspension->hardpoints[Suspension::RearBarMidPoint].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);
		localSuspension->hardpoints[Suspension::RearBarPivotAxis].Rotate(inputs.centerOfRotation, rotations,
			inputs.firstRotation, secondRotation);

		// Translations
		localSuspension->hardpoints[Suspension::RearBarMidPoint].z += inputs.heave;
		localSuspension->hardpoints[Suspension::RearBarPivotAxis].z += inputs.heave;
		break;

	case Suspension::SwayBarGeared:
	case Suspension::SwayBarNone:
	default:
		// Nothing here
		break;
	}

	// Third springs
	if (localSuspension->frontHasThirdSpring)
	{
		// Rotations
		localSuspension->hardpoints[Suspension::FrontThirdSpringInboard].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);
		localSuspension->hardpoints[Suspension::FrontThirdShockInboard].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);

		// Translations
		localSuspension->hardpoints[Suspension::FrontThirdSpringInboard].z += inputs.heave;
		localSuspension->hardpoints[Suspension::FrontThirdShockInboard].z += inputs.heave;
	}

	if (localSuspension->rearHasThirdSpring)
	{
		// Rotations
		localSuspension->hardpoints[Suspension::RearThirdSpringInboard].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);
		localSuspension->hardpoints[Suspension::RearThirdShockInboard].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);

		// Translations
		localSuspension->hardpoints[Suspension::RearThirdSpringInboard].z += inputs.heave;
		localSuspension->hardpoints[Suspension::RearThirdShockInboard].z += inputs.heave;
	}

	if (!SolveCorner(localSuspension->rightFront, originalCar->suspension->rightFront, rotations, secondRotation))
		Debugger::GetInstance().Print(_T("ERROR:  Problem solving right front corner!  Increase debug level for more information."),
			Debugger::PriorityHigh);
	if (!SolveCorner(localSuspension->leftFront, originalCar->suspension->leftFront, rotations, secondRotation))
		Debugger::GetInstance().Print(_T("ERROR:  Problem solving left front corner!  Increase debug level for more information."),
			Debugger::PriorityHigh);
	if (!SolveCorner(localSuspension->rightRear, originalCar->suspension->rightRear, rotations, secondRotation))
		Debugger::GetInstance().Print(_T("ERROR:  Problem solving right rear corner!  Increase debug level for more information."),
			Debugger::PriorityHigh);
	if (!SolveCorner(localSuspension->leftRear, originalCar->suspension->leftRear, rotations, secondRotation))
		Debugger::GetInstance().Print(_T("ERROR:  Problem solving left rear corner!  Increase debug level for more information."),
			Debugger::PriorityHigh);

	// Some things need to be solved AFTER all other corners
	if (localSuspension->frontBarStyle == Suspension::SwayBarTBar)
	{
		if (!SolveInboardTBarPoints(
			localSuspension->leftFront.hardpoints[Corner::OutboardBarLink],
			localSuspension->rightFront.hardpoints[Corner::OutboardBarLink],
			localSuspension->hardpoints[Suspension::FrontBarMidPoint],
			localSuspension->hardpoints[Suspension::FrontBarPivotAxis],
			originalCar->suspension->leftFront.hardpoints[Corner::OutboardBarLink],
			originalCar->suspension->rightFront.hardpoints[Corner::OutboardBarLink],
			originalCar->suspension->hardpoints[Suspension::FrontBarMidPoint],
			originalCar->suspension->hardpoints[Suspension::FrontBarPivotAxis],
			originalCar->suspension->leftFront.hardpoints[Corner::InboardBarLink],
			originalCar->suspension->rightFront.hardpoints[Corner::InboardBarLink],
			localSuspension->leftFront.hardpoints[Corner::InboardBarLink],
			localSuspension->rightFront.hardpoints[Corner::InboardBarLink]))
		{
			// Print an error and switch the success boolean to indicate a failure
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for inboard T-bar (front)!"), Debugger::PriorityMedium);
		}
	}

	if (localSuspension->rearBarStyle == Suspension::SwayBarTBar)
	{
		if (!SolveInboardTBarPoints(
			localSuspension->leftRear.hardpoints[Corner::OutboardBarLink],
			localSuspension->rightRear.hardpoints[Corner::OutboardBarLink],
			localSuspension->hardpoints[Suspension::RearBarMidPoint],
			localSuspension->hardpoints[Suspension::RearBarPivotAxis],
			originalCar->suspension->leftRear.hardpoints[Corner::OutboardBarLink],
			originalCar->suspension->rightRear.hardpoints[Corner::OutboardBarLink],
			originalCar->suspension->hardpoints[Suspension::RearBarMidPoint],
			originalCar->suspension->hardpoints[Suspension::RearBarPivotAxis],
			originalCar->suspension->leftRear.hardpoints[Corner::InboardBarLink],
			originalCar->suspension->rightRear.hardpoints[Corner::InboardBarLink],
			localSuspension->leftRear.hardpoints[Corner::InboardBarLink],
			localSuspension->rightRear.hardpoints[Corner::InboardBarLink]))
		{
			// Print an error and switch the success boolean to indicate a failure
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for inboard T-bar (rear)!"), Debugger::PriorityMedium);
		}
	}

	outputs.Update(originalCar, localSuspension);
	long totalTime = timer.Time();
	Debugger::GetInstance().Print(Debugger::PriorityLow, "Finished UpdateKinematcs() for %s in %0.3f sec",
		name.ToUTF8().data(), totalTime / 1000.0);
}

//==========================================================================
// Class:			Kinematics
// Function:		SolveCorner
//
// Description:		This solves for the locations of all of the suspension
//					nodes that exist at each corner of the car.  This includes
//					Everything from the contact patch up to the shock and
//					spring end-points.
//
// Input Arguments:
//		originalCorner	= const Corner*, the un-perturbed locations of the
//						  suspension hardpoints
//		rotations		= const Vector* specifying the amount to rotate the chassis
//						  about each axis
//		secondRotation	= const Vector::Axis* describing the second axis of rotation
//						  (the first is an input quantity)
//
// Output Arguments:
//		corner	= Corner*, the corner of the car we are manipulating
//
// Return Value:
//		bool, true for success, false for error(s)
//
//==========================================================================
bool Kinematics::SolveCorner(Corner &corner, const Corner &originalCorner,
	const Vector &rotations, const Vector::Axis &secondRotation)
{
	// Determine if this corner is at the front or the rear of the car
	bool isAtFront = false;
	if (corner.location == Corner::LocationRightFront || corner.location == Corner::LocationLeftFront)
		isAtFront = true;

	// First move all of the body-fixed points (rotations first)
	corner.hardpoints[Corner::LowerFrontTubMount].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.hardpoints[Corner::LowerRearTubMount].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.hardpoints[Corner::UpperFrontTubMount].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.hardpoints[Corner::UpperRearTubMount].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.hardpoints[Corner::BarArmAtPivot].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.hardpoints[Corner::InboardSpring].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.hardpoints[Corner::InboardShock].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.hardpoints[Corner::InboardTieRod].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);

	// Now do the translations on the same points
	corner.hardpoints[Corner::LowerFrontTubMount].z += inputs.heave;
	corner.hardpoints[Corner::LowerRearTubMount].z += inputs.heave;
	corner.hardpoints[Corner::UpperFrontTubMount].z += inputs.heave;
	corner.hardpoints[Corner::UpperRearTubMount].z += inputs.heave;
	corner.hardpoints[Corner::BarArmAtPivot].z += inputs.heave;
	corner.hardpoints[Corner::InboardSpring].z += inputs.heave;
	corner.hardpoints[Corner::InboardShock].z += inputs.heave;
	corner.hardpoints[Corner::InboardTieRod].z += inputs.heave;

	// Depending on the type of actuation, we might have to move some additional points, as well
	if (corner.actuationType == Corner::ActuationPushPullrod)
	{
		corner.hardpoints[Corner::BellCrankPivot1].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);
		corner.hardpoints[Corner::BellCrankPivot2].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);

		corner.hardpoints[Corner::BellCrankPivot1].z += inputs.heave;
		corner.hardpoints[Corner::BellCrankPivot2].z += inputs.heave;
	}

	// Do the rotation and translation for the inboard half shafts at the same time
	if ((originalCar->HasFrontHalfShafts() && isAtFront) ||
		(originalCar->HasRearHalfShafts() && !isAtFront))
	{
		corner.hardpoints[Corner::InboardHalfShaft].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);
		corner.hardpoints[Corner::InboardHalfShaft].z += inputs.heave;
	}

	bool success = true;

	// Solve outboard points and work in through the pushrods and bell cranks.
	// We will iterate the z-location of the lower ball joint and the solution of
	// the first three outboard points until the bottom of the tire is at z = 0.
	Vector wheelNormal;
	Vector originalPlaneNormal;
	Vector newPlaneNormal;
	Vector wheelRotations;
	double upperLimit(0.0), lowerLimit(0.0);// Initialized here to avoid MSVC++ compiler warning C4701
	int iteration = 1;
	double tolerance = 5.0e-8;
	int limit = 100;

	corner.hardpoints[Corner::ContactPatch].z = tolerance * 2;// Must be initialized to > Tolerance
	while (iteration <= limit && fabs(corner.hardpoints[Corner::ContactPatch].z) > tolerance)
	{
		if (!SolveForXY(Corner::LowerBallJoint, Corner::LowerFrontTubMount,
			Corner::LowerRearTubMount, originalCorner, corner))
		{
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for lower ball joint!"), Debugger::PriorityMedium);
			success = false;
		}

		if (!SolveForPoint(Corner::UpperBallJoint, Corner::LowerBallJoint,
			Corner::UpperFrontTubMount, Corner::UpperRearTubMount,originalCorner, corner))
		{
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for upper ball joint!"), Debugger::PriorityMedium);
			success = false;
		}

		if (!SolveForPoint(Corner::OutboardTieRod, Corner::LowerBallJoint,
			Corner::UpperBallJoint, Corner::InboardTieRod, originalCorner, corner))
		{
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard tie rod!"), Debugger::PriorityMedium);
			success = false;
		}

		if (!SolveForPoint(Corner::WheelCenter, Corner::LowerBallJoint,
			Corner::UpperBallJoint, Corner::OutboardTieRod, originalCorner, corner))
		{
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for wheel center!"), Debugger::PriorityMedium);
			success = false;
		}

		originalPlaneNormal = VVASEMath::GetPlaneNormal(originalCorner.hardpoints[Corner::LowerBallJoint],
			originalCorner.hardpoints[Corner::UpperBallJoint], originalCorner.hardpoints[Corner::OutboardTieRod]);
		newPlaneNormal = VVASEMath::GetPlaneNormal(corner.hardpoints[Corner::LowerBallJoint],
			corner.hardpoints[Corner::UpperBallJoint], corner.hardpoints[Corner::OutboardTieRod]);

		// For some operations, if this is on the right-hand side of the car, the sign gets flipped
		double sign = 1.0;
		if (corner.location == Corner::LocationRightFront || corner.location == Corner::LocationRightRear)
			sign *= -1.0;

		// Determine what Euler angles I need to rotate OriginalPlaneNormal through to
		// get NewPlaneNormal.  Do one angle at a time because order of rotations matters.
		wheelRotations = originalPlaneNormal.AnglesTo(newPlaneNormal);
		wheelNormal.Set(0.0, sign * 1.0, 0.0);
		wheelNormal.Rotate(wheelRotations.z, Vector::AxisZ);
		wheelNormal.Rotate(wheelRotations.x, Vector::AxisX);

		// Add in the effects of camber and toe settings (contact patch location should not be affected
		// by camber and toe settings - WheelCenter is calculated based on camber and toe, so we need to
		// back that out in the contact patch solver to avoid making contact patch a function of camber
		// and toe).
		wheelNormal.Rotate(sign * originalCorner.staticCamber, Vector::AxisX);
		wheelNormal.Rotate(sign * originalCorner.staticToe, Vector::AxisZ);

		// Do the actual solving for the contact patch
		SolveForContactPatch(corner.hardpoints[Corner::WheelCenter], wheelNormal,
			originalCorner.hardpoints[Corner::ContactPatch].Distance(originalCorner.hardpoints[Corner::WheelCenter]),
			corner.hardpoints[Corner::ContactPatch]);

		// With the origin on the ground, the error is equal to the Z location of the contact patch
		// FIXME:  This is only true when using flat ground model!!!
		// Use a line search algorithm to adjust the height of the lower ball joint
		// This could get hung up if the relationship between lower ball joint height and contact
		// patch Z location is highly non-linear, but that would be an unusual (impossible?) case (extreme camber gain?)

		// If this is the first iteration, initialize the limit variables
		if (iteration == 1)
		{
			upperLimit = corner.hardpoints[Corner::LowerBallJoint].z +
				3.0 * fabs(corner.hardpoints[Corner::ContactPatch].z);
			lowerLimit = corner.hardpoints[Corner::LowerBallJoint].z -
				3.0 * fabs(corner.hardpoints[Corner::ContactPatch].z);
		}

		// Make the adjustment in the guess
		if (corner.hardpoints[Corner::ContactPatch].z > tolerance)
			upperLimit = corner.hardpoints[Corner::LowerBallJoint].z;
		else if (corner.hardpoints[Corner::ContactPatch].z < -tolerance)
			lowerLimit = corner.hardpoints[Corner::LowerBallJoint].z;
		corner.hardpoints[Corner::LowerBallJoint].z = lowerLimit + (upperLimit - lowerLimit) / 2.0;

		iteration++;
	}

	// Check to make sure we finished the loop because we were within the tolerance (and not
	// because we hit the iteration limit)
	if ((fabs(corner.hardpoints[Corner::ContactPatch].z) > tolerance))
	{
		Debugger::GetInstance().Print(_T("Warning (SolveCorner):  Contact patch location did not converge"),
			Debugger::PriorityMedium);
		success = false;
	}

	// Outboard shock/spring actuators
	if (corner.actuationType == Corner::ActuationPushPullrod)
	{
		if (corner.actuationAttachment == Corner::AttachmentLowerAArm)
		{
			if (!SolveForPoint(Corner::OutboardPushrod, Corner::LowerBallJoint,
				Corner::LowerFrontTubMount, Corner::LowerRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard pushrod!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (corner.actuationAttachment == Corner::AttachmentUpperAArm)
		{
			if (!SolveForPoint(Corner::OutboardPushrod, Corner::UpperBallJoint,
				Corner::UpperFrontTubMount, Corner::UpperRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard pullrod!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (corner.actuationAttachment == Corner::AttachmentUpright)
		{
			if (!SolveForPoint(Corner::OutboardPushrod, Corner::UpperBallJoint,
				Corner::LowerBallJoint, Corner::OutboardTieRod, originalCorner, corner))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard push/pullrod!"), Debugger::PriorityMedium);
				success = false;
			}
		}

		// Inboard Pushrods
		if (!SolveForPoint(Corner::InboardPushrod, Corner::BellCrankPivot1,
			Corner::BellCrankPivot2, Corner::OutboardPushrod, originalCorner, corner))
		{
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for inboard push/pullrod!"), Debugger::PriorityMedium);
			success = false;
		}

		// Outboard Shocks
		if (!SolveForPoint(Corner::OutboardShock, Corner::BellCrankPivot1,
			Corner::BellCrankPivot2, Corner::InboardPushrod, originalCorner, corner))
		{
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
			success = false;
		}

		// Outboard Springs
		if (!SolveForPoint(Corner::OutboardSpring, Corner::BellCrankPivot1,
			Corner::BellCrankPivot2, Corner::InboardPushrod, originalCorner, corner))
		{
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
			success = false;
		}

		// Sway Bars Outboard
		if ((localSuspension->frontBarStyle != Suspension::SwayBarNone && isAtFront) ||
			(localSuspension->rearBarStyle != Suspension::SwayBarNone && !isAtFront))
		{
			if (!SolveForPoint(Corner::OutboardBarLink, Corner::BellCrankPivot1,
				Corner::BellCrankPivot2, Corner::InboardPushrod, originalCorner, corner))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard swaybar!"), Debugger::PriorityMedium);
				success = false;
			}
		}

		// Sway Bars Inboard
		if (localSuspension->frontBarStyle == Suspension::SwayBarUBar && isAtFront)
		{
			Vector originalBarMidpoint = 0.5 *
				(originalCar->suspension->leftFront.hardpoints[Corner::BarArmAtPivot]
				+ originalCar->suspension->rightFront.hardpoints[Corner::BarArmAtPivot]);
			if (!SolveForPoint(corner.hardpoints[Corner::BarArmAtPivot],
				corner.hardpoints[Corner::OutboardBarLink], localSuspension->hardpoints[Suspension::FrontBarMidPoint],
				originalCorner.hardpoints[Corner::BarArmAtPivot], originalCorner.hardpoints[Corner::OutboardBarLink],
				originalBarMidpoint, originalCorner.hardpoints[Corner::InboardBarLink],
				corner.hardpoints[Corner::InboardBarLink]))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for inboard U-bar (front)!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (localSuspension->rearBarStyle == Suspension::SwayBarUBar && !isAtFront)
		{
			Vector originalBarMidpoint = 0.5 *
				(originalCar->suspension->leftRear.hardpoints[Corner::BarArmAtPivot]
				+ originalCar->suspension->rightRear.hardpoints[Corner::BarArmAtPivot]);
			if (!SolveForPoint(corner.hardpoints[Corner::BarArmAtPivot],
				corner.hardpoints[Corner::OutboardBarLink], localSuspension->hardpoints[Suspension::RearBarMidPoint],
				originalCorner.hardpoints[Corner::BarArmAtPivot], originalCorner.hardpoints[Corner::OutboardBarLink],
				originalBarMidpoint, originalCorner.hardpoints[Corner::InboardBarLink],
				corner.hardpoints[Corner::InboardBarLink]))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for inboard U-bar (rear)!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if ((localSuspension->frontBarStyle == Suspension::SwayBarGeared && isAtFront) ||
			(localSuspension->rearBarStyle == Suspension::SwayBarGeared && !isAtFront))
		{
			if (!SolveForPoint(Corner::InboardBarLink, Corner::BarArmAtPivot,
				Corner::OutboardBarLink, Corner::GearEndBarShaft, originalCorner, corner))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for geared bar!"), Debugger::PriorityMedium);
				success = false;
			}
		}
	}
	else if (corner.actuationType == Corner::ActuationOutboard)// Outboard spring/shock units  - no pushrod/bell crank
	{
		if (corner.actuationAttachment == Corner::AttachmentLowerAArm)
		{
			if (!SolveForPoint(Corner::OutboardSpring, Corner::LowerBallJoint,
				Corner::LowerFrontTubMount, Corner::LowerRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
				success = false;
			}

			if (!SolveForPoint(Corner::OutboardShock, Corner::LowerBallJoint,
				Corner::LowerFrontTubMount, Corner::LowerRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (corner.actuationAttachment == Corner::AttachmentUpperAArm)
		{
			if (!SolveForPoint(Corner::OutboardSpring, Corner::UpperBallJoint,
				Corner::UpperFrontTubMount, Corner::UpperRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
				success = false;
			}

			if (!SolveForPoint(Corner::OutboardShock, Corner::UpperBallJoint,
				Corner::UpperFrontTubMount, Corner::UpperRearTubMount, originalCorner, corner))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (corner.actuationAttachment == Corner::AttachmentUpright)
		{
			if (!SolveForPoint(Corner::OutboardSpring, Corner::UpperBallJoint,
				Corner::LowerBallJoint, Corner::OutboardTieRod, originalCorner, corner))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
				success = false;
			}

			if (!SolveForPoint(Corner::OutboardShock, Corner::UpperBallJoint,
				Corner::LowerBallJoint, Corner::OutboardTieRod, originalCorner, corner))
			{
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
				success = false;
			}
		}
	}

	// Half Shafts
	if ((originalCar->HasFrontHalfShafts() && isAtFront) ||
		(originalCar->HasRearHalfShafts() && !isAtFront))
	{
		if (!SolveForPoint(Corner::OutboardHalfShaft, Corner::LowerBallJoint,
			Corner::UpperBallJoint, Corner::OutboardTieRod, originalCorner, corner))
		{
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard half shaft!"), Debugger::PriorityMedium);
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
	return SolveForPoint(currentCorner.hardpoints[reference1], currentCorner.hardpoints[reference2],
		currentCorner.hardpoints[reference3], originalCorner.hardpoints[reference1],
		originalCorner.hardpoints[reference2], originalCorner.hardpoints[reference3],
		originalCorner.hardpoints[target], currentCorner.hardpoints[target]);
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
//		center1			= const Vector& specifying the center of the first sphere
//		center2			= const Vector& specifying the center of the second sphere
//		center3			= const Vector& specifying the center of the third sphere
//		originalCenter1	= const Vector& specifying the original center of the
//						  first sphere
//		originalCenter2	= const Vector& specifying the original center of the
//						  second sphere
//		originalCenter3	= const Vector& specifying the original center of the
//						  third sphere
//		original		= const Vector& specifying the original location of the point
//						  we are solving for
//
// Output Arguments:
//		current	= Vector& specifying the result of the intersection of
//				  three spheres algorithm
//
// Return Value:
//		bool, true for success, false for error
//		
//==========================================================================
bool Kinematics::SolveForPoint(const Vector &center1, const Vector &center2,
	const Vector &center3, const Vector &originalCenter1,
	const Vector &originalCenter2, const Vector &originalCenter3,
	const Vector &original, Vector &current)
{
	// Compute the circle radii
	double r1 = originalCenter1.Distance(original);
	double r2 = originalCenter2.Distance(original);
	double r3 = originalCenter3.Distance(original);

	// Check for the existence of a solution
	if (center1.Distance(center2) > r1 + r2 || center1.Distance(center3) > r1 + r3 ||
		center2.Distance(center3) > r2 + r3)
	{
		Debugger::GetInstance().Print(_T("Error (SolveForPoint): Center distance exceeds sum of radii"), Debugger::PriorityLow);

		return false;
	}
	else if (center1.Distance(center2) + min(r1, r2) < max(r1, r2) ||
		center1.Distance(center3) + min(r1, r3) < max(r1, r3) ||
		center2.Distance(center3) + min(r2, r3) < max(r2, r3))
	{
		Debugger::GetInstance().Print(_T("Error (SolveForPoint): Center distance and smaller radius less than larger radius"),
			Debugger::PriorityLow);

		return false;
	}

	// The method:
	//  1.	The intersection of two spheres creates a circle.  That circle lies on a plane.
	//		Determine this plane for any two spheres. This plane is determined by subtracting
	//		the equations of two spheres.  This is different from (better than) substitution,
	//		because this will ensure that the higher order terms drop out.
	//  2.	Determine the same plane as in step 1 for a different set of two spheres.
	//  3.	Find the line created by the intersection of the planes found in steps 1 and 2.
	//		Lines only have one degree of freedom, so this will be two equations in the same
	//		variable.
	//  4.  The intersection of the line and any sphere will yield two points (unless the
	//		spheres don't intersect or they intersect at only one point).  These points are
	//		the solutions.  Here, we employ the quadratic equation and the equation of the
	//		line determined in step 3.

	// Declare our plane constants
	double a1, b1, c1, d1, a2, b2, c2, d2;

	// Step 1 (Plane defined by intersection of spheres 1 and 2)
	a1 = center1.x - center2.x;
	b1 = center1.y - center2.y;
	c1 = center1.z - center2.z;
	d1 = (pow(center2.Length(), 2) - pow(center1.Length(), 2) - r2 * r2 + r1 * r1) / 2.0;

	// Step 2 (Plane defined by intersection of spheres 1 and 3)
	a2 = center1.x - center3.x;
	b2 = center1.y - center3.y;
	c2 = center1.z - center3.z;
	d2 = (pow(center3.Length(), 2) - pow(center1.Length(), 2) - r3 * r3 + r1 * r1) / 2.0;

	// Step 3 (Line defined by intersection of planes from steps 1 and 2)
	// The if..else stuff avoid numerical instabilities - we'll choose the denominators
	// farthest from zero for all divisions (denominators are below):
	double den1 = b1 * c2 - b2 * c1;
	double den2 = a1 * c2 - a2 * c1;
	double den3 = a1 * b2 - a2 * b1;

	// Let's declare our answers now
	Vector solution1, solution2;

	// And our quadratic equation coefficients
	double a, b, c;

	// In which order do we want to solve for the components of the points?
	if (max(max(fabs(den1), fabs(den2)), fabs(den3)) == fabs(den1))
	{
		if (max(max(max(fabs(c1), fabs(c2)), fabs(b1)), fabs(b2)) == fabs(c1))
		{
			// Solve X first (use first set of plane coefficients)
			double myx = (a2 * c1 - a1 * c2) / den1;
			double byx = (c1 * d2 - c2 * d1) / den1;
			
			a = 1.0 + myx * myx + pow(a1 + b1 * myx, 2) / (c1 * c1);
			b = 2.0 * (myx * (byx - center1.y) - center1.x + (a1 + b1 * myx) / c1
				* ((b1 * byx + d1) / c1 + center1.z));
			c = center1.x * center1.x + pow(byx - center1.y, 2) - r1 * r1
				+ pow((b1 * byx + d1) / c1 + center1.z, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Y next
			solution1.y = solution1.x * myx + byx;
			solution2.y = solution2.x * myx + byx;
			// Solve Z last
			solution1.z = (-a1 * solution1.x - b1 * solution1.y - d1) / c1;
			solution2.z = (-a1 * solution2.x - b1 * solution2.y - d1) / c1;
		}
		else if (max(max(max(fabs(c1), fabs(c2)), fabs(b1)), fabs(b2)) == fabs(c2))
		{
			// Solve X first (use second set of plane coefficients)
			double myx = (a2 * c1 - a1 * c2) / den1;
			double byx = (c1 * d2 - c2 * d1) / den1;
			a = 1.0 + myx * myx + pow(a2 + b2 * myx, 2) / (c2 * c2);
			b = 2.0 * (myx * (byx - center1.y) - center1.x + (a2 + b2 * myx) / c2
				* ((b2 * byx + d2) / c2 + center1.z));
			c = center1.x * center1.x + pow(byx - center1.y, 2) - r1 * r1
				+ pow((b2 * byx + d2) / c2 + center1.z, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Y next
			solution1.y = solution1.x * myx + byx;
			solution2.y = solution2.x * myx + byx;
			// Solve Z last
			solution1.z = (-a2 * solution1.x - b2 * solution1.y - d2) / c2;
			solution2.z = (-a2 * solution2.x - b2 * solution2.y - d2) / c2;
		}
		else if (max(max(max(fabs(c1), fabs(c2)), fabs(b1)), fabs(b2)) == fabs(b1))
		{
			// Solve X first (use first set of plane coefficients)
			double mzx = (a1 * b2 - a2 * b1) / den1;
			double bzx = (b2 * d1 - b1 * d2) / den1;
			a = 1.0 + mzx * mzx + pow(a1 + c1 * mzx, 2) / (b1 * b1);
			b = 2.0 * (mzx * (bzx - center1.z) - center1.x + (a1 + c1 * mzx) / b1
				* ((c1 * bzx + d1) / b1 + center1.y));
			c = center1.x * center1.x + pow(bzx - center1.z, 2) - r1 * r1
				+ pow((c1 * bzx + d1) / b1 + center1.y, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Z next
			solution1.z = solution1.x * mzx + bzx;
			solution2.z = solution2.x * mzx + bzx;
			// Solve Y last
			solution1.y = (-a1 * solution1.x - c1 * solution1.z - d1) / b1;
			solution2.y = (-a1 * solution2.x - c1 * solution2.z - d1) / b1;
		}
		else// if (max(max(max(fabs(c1), fabs(c2)), fabs(b1)), fabs(b2)) == fabs(b2))
		{
			// Solve X first (use second set of plane coefficients)
			double mzx = (a1 * b2 - a2 * b1) / den1;
			double bzx = (b2 * d1 - b1 * d2) / den1;
			a = 1.0 + mzx * mzx + pow(a2 + c2 * mzx, 2) / (b2 * b2);
			b = 2.0 * (mzx * (bzx - center1.z) - center1.x + (a2 + c2 * mzx) / b2
				* ((c2 * bzx + d2) / b2 + center1.y));
			c = center1.x * center1.x + pow(bzx - center1.z, 2) - r1 * r1
				+ pow((c2 * bzx + d2) / b2 + center1.y, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Z next
			solution1.z = solution1.x * mzx + bzx;
			solution2.z = solution2.x * mzx + bzx;
			// Solve Y last
			solution1.y = (-a2 * solution1.x - c2 * solution1.z - d2) / b2;
			solution2.y = (-a2 * solution2.x - c2 * solution2.z - d2) / b2;
		}
	}
	else if (max(max(fabs(den1), fabs(den2)), fabs(den3)) == fabs(den2))
	{
		if (max(max(max(fabs(a1), fabs(a2)), fabs(c1)), fabs(c2)) == fabs(a1))
		{
			// Solve Y first (use first set of plane coefficients)
			double mzy = (a2 * b1 - a1 * b2) / den2;
			double bzy = (a2 * d1 - a1 * d2) / den2;
			a = 1 + mzy * mzy + pow(b1 + c1 * mzy, 2) / (a1 * a1);
			b = 2 * (mzy * (bzy - center1.z) - center1.y + (b1 + c1 * mzy) / a1
				* ((c1 * bzy + d1) / a1 + center1.x));
			c = center1.y * center1.y + pow(bzy - center1.z, 2) - r1 * r1
				+ pow((c1 * bzy + d1) / a1 + center1.x, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Z next
			solution1.z = solution1.y * mzy + bzy;
			solution2.z = solution2.y * mzy + bzy;
			// Solve X last
			solution1.x = (-b1 * solution1.y - c1 * solution1.z - d1) / a1;
			solution2.x = (-b1 * solution2.y - c1 * solution2.z - d1) / a1;
		}
		else if (max(max(max(fabs(a1), fabs(a2)), fabs(c1)), fabs(c2)) == fabs(a2))
		{
			// Solve Y first (use second set of plane coefficients)
			double mzy = (a2 * b1 - a1 * b2) / den2;
			double bzy = (a2 * d1 - a1 * d2) / den2;
			a = 1 + mzy * mzy + pow(b2 + c2 * mzy, 2) / (a2 * a2);
			b = 2 * (mzy * (bzy - center1.z) - center1.y + (b2 + c2 * mzy) / a2
				* ((c2 * bzy + d2) / a2 + center1.x));
			c = center1.y * center1.y + pow(bzy - center1.z, 2) - r1 * r1
				+ pow((c2 * bzy + d2) / a2 + center1.x, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Z next
			solution1.z = solution1.y * mzy + bzy;
			solution2.z = solution2.y * mzy + bzy;
			// Solve X last
			solution1.x = (-b2 * solution1.y - c2 * solution1.z - d2) / a2;
			solution2.x = (-b2 * solution2.y - c2 * solution2.z - d2) / a2;
		}
		else if (max(max(max(fabs(a1), fabs(a2)), fabs(c1)), fabs(c2)) == fabs(c1))
		{
			// Solve Y first (use first set of plane coefficients)
			double mxy = (b2 * c1 - b1 * c2) / den2;
			double bxy = (c1 * d2 - c2 * d1) / den2;
			a = 1 + mxy * mxy + pow(b1 + a1 * mxy, 2) / (c1 * c1);
			b = 2 * (mxy * (bxy - center1.x) - center1.y + (b1 + a1 * mxy) / c1
				* ((a1 * bxy + d1) / c1 + center1.z));
			c = center1.y * center1.y + pow(bxy - center1.x, 2) - r1 * r1
				+ pow((a1 * bxy + d1) / c1 + center1.z, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve X next
			solution1.x = solution1.y * mxy + bxy;
			solution2.x = solution2.y * mxy + bxy;
			// Solve Z last
			solution1.z = (-a1 * solution1.x - b1 * solution1.y - d1) / c1;
			solution2.z = (-a1 * solution2.x - b1 * solution2.y - d1) / c1;
		}
		else// if (max(max(max(fabs(a1), fabs(a2)), fabs(c1)), fabs(c2)) == fabs(c2))
		{
			// Solve Y first (use second set of plane coefficients)
			double mxy = (b2 * c1 - b1 * c2) / den2;
			double bxy = (c1 * d2 - c2 * d1) / den2;
			a = 1 + mxy * mxy + pow(b2 + a2 * mxy, 2) / (c2 * c2);
			b = 2 * (mxy * (bxy - center1.x) - center1.y + (b2 + a2 * mxy) / c2
				* ((a2 * bxy + d2) / c2 + center1.z));
			c = center1.y * center1.y + pow(bxy - center1.x, 2) - r1 * r1
				+ pow((a2 * bxy + d2) / c2 + center1.z, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve X next
			solution1.x = solution1.y * mxy + bxy;
			solution2.x = solution2.y * mxy + bxy;
			// Solve Z last
			solution1.z = (-a2 * solution1.x - b2 * solution1.y - d2) / c2;
			solution2.z = (-a2 * solution2.x - b2 * solution2.y - d2) / c2;
		}
	}
	else// if (max(max(fabs(den1), fabs(den2)), fabs(den3)) == fabs(den3))
	{
		if (max(max(max(fabs(a1), fabs(a2)), fabs(b1)), fabs(b2)) == fabs(a1))
		{
			// Solve Z first (use first set of plane coefficients)
			double myz = (a2 * c1 - a1 * c2) / den3;
			double byz = (a2 * d1 - a1 * d2) / den3;
			a = 1 + myz * myz + pow(c1 + b1 * myz, 2) / (a1 * a1);
			b = 2 * (myz * (byz - center1.y) - center1.z + (c1 + b1 * myz) / a1
				* ((b1 * byz + d1) / a1 + center1.x));
			c = center1.z * center1.z + pow(byz - center1.y, 2) - r1 * r1
				+ pow((b1 * byz + d1) / a1 + center1.x, 2);
			// First solution
			solution1.z = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.z = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Y next
			solution1.y = solution1.z * myz + byz;
			solution2.y = solution2.z * myz + byz;
			// Solve X last
			solution1.x = (-b1 * solution1.y - c1 * solution1.z - d1) / a1;
			solution2.x = (-b1 * solution2.y - c1 * solution2.z - d1) / a1;
		}
		else if (max(max(max(fabs(a1), fabs(a2)), fabs(b1)), fabs(b2)) == fabs(a2))
		{
			// Solve Z first (use second set of plane coefficients)
			double myz = (a2 * c1 - a1 * c2) / den3;
			double byz = (a2 * d1 - a1 * d2) / den3;
			a = 1 + myz * myz + pow(c2 + b2 * myz, 2) / (a2 * a2);
			b = 2 * (myz * (byz - center1.y) - center1.z + (c2 + b2 * myz) / a2
				* ((b2 * byz + d2) / a2 + center1.x));
			c = center1.z * center1.z + pow(byz - center1.y, 2) - r1 * r1
				+ pow((b2 * byz + d2) / a2 + center1.x, 2);
			// First solution
			solution1.z = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.z = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Y next
			solution1.y = solution1.z * myz + byz;
			solution2.y = solution2.z * myz + byz;
			// Solve X last
			solution1.x = (-b2 * solution1.y - c2 * solution1.z - d2) / a2;
			solution2.x = (-b2 * solution2.y - c2 * solution2.z - d2) / a2;
		}
		else if (max(max(max(fabs(a1), fabs(a2)), fabs(b1)), fabs(b2)) == fabs(b1))
		{
			// Solve Z first (use first set of plane coefficients)
			double mxz = (b1 * c2 - b2 * c1) / den3;
			double bxz = (b1 * d2 - b2 * d1) / den3;
			a = 1 + mxz * mxz + pow(c1 + a1 * mxz, 2) / (b1 * b1);
			b = 2 * (mxz * (bxz - center1.x) - center1.z + (c1 + a1 * mxz) / b1
				* ((a1 * bxz + d1) / b1 + center1.y));
			c = center1.z * center1.z + pow(bxz - center1.x, 2) - r1 * r1
				+ pow((a1 * bxz + d1) / b1 + center1.y, 2);
			// First solution
			solution1.z = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.z = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve X next
			solution1.x = solution1.z * mxz + bxz;
			solution2.x = solution2.z * mxz + bxz;
			// Solve Y last
			solution1.y = (-a1 * solution1.x - c1 * solution1.z - d1) / b1;
			solution2.y = (-a1 * solution2.x - c1 * solution2.z - d1) / b1;
		}
		else// if (max(max(max(fabs(a1), fabs(a2)), fabs(b1)), fabs(b2)) == fabs(b2))
		{
			// Solve Z first (use second set of plane coefficients)
			double mxz = (b1 * c2 - b2 * c1) / den3;
			double bxz = (b1 * d2 - b2 * d1) / den3;
			a = 1 + mxz * mxz + pow(c2 + a2 * mxz, 2) / (b2 * b2);
			b = 2 * (mxz * (bxz - center1.x) - center1.z + (c2 + a2 * mxz) / b2
				* ((a2 * bxz + d2) / b2 + center1.y));
			c = center1.z * center1.z + pow(bxz - center1.x, 2) - r1 * r1
				+ pow((a2 * bxz + d2) / b2 + center1.y, 2);
			// First solution
			solution1.z = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.z = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve X next
			solution1.x = solution1.z * mxz + bxz;
			solution2.x = solution2.z * mxz + bxz;
			// Solve Y last
			solution1.y = (-a2 * solution1.x - c2 * solution1.z - d2) / b2;
			solution2.y = (-a2 * solution2.x - c2 * solution2.z - d2) / b2;
		}
	}

	// Make sure the solution is valid
	if (solution1 != solution1 || solution2 != solution2)
	{
		Debugger::GetInstance().Print(_T("Error (SolveForPoint): Invalid solution"), Debugger::PriorityLow);

		return false;
	}

	// We now have two solutions.  To choose between them, we must examine the original
	// location of the point and the original sphere centers.  The three sphere centers
	// define a plane, and the the two solutions lie on opposite sides of the plane.
	// The first step in identifying the correct solution is to determine which side of the
	// original plane the original point was on, and which side of the new plane either one
	// of the solutions is on.

	// Get the plane normals
	Vector originalNormal = VVASEMath::GetPlaneNormal(originalCenter1, originalCenter2, originalCenter3);
	Vector newNormal = VVASEMath::GetPlaneNormal(center1, center2, center3);

	// Get a vector from the location of the point to some point in the plane
	Vector originalVectorToPlane = originalCenter1 - original;
	Vector newVectorToPlane = center1 - solution1;

	// The dot products of the normal and the vector to the plane will give an indication
	// of which side of the plane the point is on
	double originalSide = originalNormal * originalVectorToPlane;
	double newSide = newNormal * newVectorToPlane;

	// We can compare the sign of the original side with the new side to choose the correct solution
	if ((newSide > 0 && originalSide > 0) || (newSide < 0 && originalSide < 0))
		current = solution1;
	else
		current = solution2;

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
	Vector slope;

	slope = localSuspension->rightFront.hardpoints[Corner::InboardTieRod]
		-localSuspension->leftFront.hardpoints[Corner::InboardTieRod];

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
	t /= localSuspension->rightFront.hardpoints[Corner::InboardTieRod].Distance(
		localSuspension->leftFront.hardpoints[Corner::InboardTieRod]);

	// Now we multiply by how far we actually want the rack to move...
	t *= travel;// t is now unitless again

	// And we're ready to move the rack.  Since the slope and t calibration are the
	// same for both points, we can just use each point as their own reference points.
	localSuspension->leftFront.hardpoints[Corner::InboardTieRod] += slope * t;
	localSuspension->rightFront.hardpoints[Corner::InboardTieRod] += slope * t;
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
//		current	= Vector& specifying the result of the intersection of
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
	return SolveForXY(currentCorner.hardpoints[reference1], currentCorner.hardpoints[reference2],
		originalCorner.hardpoints[reference1], originalCorner.hardpoints[reference2],
		originalCorner.hardpoints[target], currentCorner.hardpoints[target]);
}

//==========================================================================
// Class:			Kinematics
// Function:		SolveForXY
//
// Description:		This is a modification of the solver for the rest of
//					the suspension hardpoints.  Here, the Z component of
//					Original is assumed to be correct and is never modified.
//					Otherwise, it works the same way as SolveForPoint.  In
//					the event of an error, the original value is returned.
//
// Input Arguments:
//		center1			= const Vector& specifying the center of the first sphere
//		center2			= const Vector& specifying the center of the second sphere
//		originalCenter1	= const Vector& specifying the original center of the
//						  first sphere
//		originalCenter2	= const Vector& specifying the original center of the
//						  second sphere
//		original		= const Vector& specifying the original location of the point
//						  we are solving for
//
// Output Arguments:
//		current	= Vector& specifying the result of the intersection of
//				  three spheres algorithm
//
// Return Value:
//		bool, true for success, false for error
//		
//==========================================================================
bool Kinematics::SolveForXY(const Vector &center1, const Vector &center2,
	const Vector &originalCenter1, const Vector &originalCenter2,
	const Vector &original, Vector &current)
{
	// Compute the circle radii
	double r1 = originalCenter1.Distance(original);
	double r2 = originalCenter2.Distance(original);

	// Check for the existence of a solution
	if (center1.Distance(center2) > r1 + r2)
	{
		Debugger::GetInstance().Print(_T("Error (SolveForXY): Center distance exceeds sum of radii"), Debugger::PriorityLow);

		return false;
	}
	else if (center1.Distance(center2) + min(r1, r2) < max(r1, r2))
	{
		Debugger::GetInstance().Print(_T("Error (SolveForXY): Center distance and smaller radius less than larger radius"),
			Debugger::PriorityLow);

		return false;
	}

	// Declare our plane constants
	double a1, b1, c1, d1;

	// Step 1 (Plane defined by intersection of spheres 1 and 2)
	a1 = center1.x - center2.x;
	b1 = center1.y - center2.y;
	c1 = center1.z - center2.z;
	d1 = (pow(center2.Length(), 2) - pow(center1.Length(), 2) - r2 * r2 + r1 * r1) / 2.0;

	// Step 2 (Line defined by intersection of planes from steps 1 and <0 0 1>)
	// The if..else stuff avoid numerical instabilities - we'll choose the denominators
	// farthest from zero for all divisions.

	// Let's declare our answers now
	Vector solution1, solution2;

	// And our quadratic equation coefficients
	double a, b, c;

	// In which order do we want to solve for the components of the points?
	// TODO:  This code can probably be leaned out, but it works fine...
	if (max(fabs(b1), fabs(a1)) == fabs(b1))
	{
		if (max(max(fabs(c1), fabs(1.0)), fabs(b1)) == fabs(c1))
		{
			// Solve X first (use first set of plane coefficients)
			double myx = -a1 / b1;
			double byx = -(c1 * current.z + d1) / b1;
			
			a = 1.0 + myx * myx + pow(a1 + b1 * myx, 2) / (c1 * c1);
			b = 2.0 * (myx * (byx - center1.y) - center1.x + (a1 + b1 * myx) / c1
				* ((b1 * byx + d1) / c1 + center1.z));
			c = center1.x * center1.x + pow(byx - center1.y, 2) - r1 * r1
				+ pow((b1 * byx + d1) / c1 + center1.z, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Y next
			solution1.y = solution1.x * myx + byx;
			solution2.y = solution2.x * myx + byx;
			// Solve Z last
			solution1.z = (-a1 * solution1.x - b1 * solution1.y - d1) / c1;
			solution2.z = (-a1 * solution2.x - b1 * solution2.y - d1) / c1;
		}
		else if (max(max(fabs(c1), fabs(1.0)), fabs(b1)) == fabs(1.0))
		{
			// Solve X first (use second set of plane coefficients)
			double myx = 0.0;
			double byx = -c1 * current.z / b1;
			a = 1.0 + myx * myx;
			b = 2.0 * (myx * (byx - center1.y) - center1.x);
			c = center1.x * center1.x + pow(byx - center1.y, 2) - r1 * r1
				+ pow(center1.z - current.z, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Y next
			solution1.y = solution1.x * myx + byx;
			solution2.y = solution2.x * myx + byx;
			// Solve Z last
			solution1.z = current.z;
			solution2.z = current.z;
		}
		else// if (max(max(fabs(c1), fabs(1.0)), fabs(b1)) == fabs(b1))
		{
			// Solve X first (use first set of plane coefficients)
			double mzx = 0.0;
			double bzx = current.z;
			a = 1.0 + mzx * mzx + pow(a1 + c1 * mzx, 2) / (b1 * b1);
			b = 2.0 * (mzx * (bzx - center1.z) - center1.x + (a1 + c1 * mzx) / b1
				* ((c1 * bzx + d1) / b1 + center1.y));
			c = center1.x * center1.x + pow(bzx - center1.z, 2) - r1 * r1
				+ pow((c1 * bzx + d1) / b1 + center1.y, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Z next
			solution1.z = solution1.x * mzx + bzx;
			solution2.z = solution2.x * mzx + bzx;
			// Solve Y last
			solution1.y = (-a1 * solution1.x - c1 * solution1.z - d1) / b1;
			solution2.y = (-a1 * solution2.x - c1 * solution2.z - d1) / b1;
		}
	}
	else// if (max(fabs(b1), fabs(a1)) == fabs(den2))
	{
		if (max(max(fabs(a1), fabs(c1)), fabs(1.0)) == fabs(a1))
		{
			// Solve Y first (use first set of plane coefficients)
			double mzy = 0.0;
			double bzy = current.z;
			a = 1 + mzy * mzy + pow(b1 + c1 * mzy, 2) / (a1 * a1);
			b = 2 * (mzy * (bzy - center1.z) - center1.y + (b1 + c1 * mzy) / a1
				* ((c1 * bzy + d1) / a1 + center1.x));
			c = center1.y * center1.y + pow(bzy - center1.z, 2) - r1 * r1
				+ pow((c1 * bzy + d1) / a1 + center1.x, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Z next
			solution1.z = solution1.y * mzy + bzy;
			solution2.z = solution2.y * mzy + bzy;
			// Solve X last
			solution1.x = (-b1 * solution1.y - c1 * solution1.z - d1) / a1;
			solution2.x = (-b1 * solution2.y - c1 * solution2.z - d1) / a1;
		}
		else if (max(max(fabs(a1), fabs(c1)), fabs(1.0)) == fabs(c1))
		{
			// Solve Y first (use first set of plane coefficients)
			double mxy = -b1 / a1;
			double bxy = -(c1 * current.z + d1) / a1;
			a = 1 + mxy * mxy + pow(b1 + a1 * mxy, 2) / (c1 * c1);
			b = 2 * (mxy * (bxy - center1.x) - center1.y + (b1 + a1 * mxy) / c1
				* ((a1 * bxy + d1) / c1 + center1.z));
			c = center1.y * center1.y + pow(bxy - center1.x, 2) - r1 * r1
				+ pow((a1 * bxy + d1) / c1 + center1.z, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve X next
			solution1.x = solution1.y * mxy + bxy;
			solution2.x = solution2.y * mxy + bxy;
			// Solve Z last
			solution1.z = (-a1 * solution1.x - b1 * solution1.y - d1) / c1;
			solution2.z = (-a1 * solution2.x - b1 * solution2.y - d1) / c1;
		}
		else// if (max(max(fabs(a1), fabs(c1)), fabs(1.0)) == fabs(1.0))
		{
			// Solve Y first (use second set of plane coefficients)
			double mxy = -b1 / a1;
			double bxy = -(c1 * current.z + d1) / a1;
			a = 1 + mxy * mxy;
			b = 2 * (mxy * (bxy - center1.x) - center1.y);
			c = center1.y * center1.y + pow(bxy - center1.x, 2) - r1 * r1
				+ pow(center1.z - current.z, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve X next
			solution1.x = solution1.y * mxy + bxy;
			solution2.x = solution2.y * mxy + bxy;
			// Solve Z last
			solution1.z = current.z;
			solution2.z = current.z;
		}
	}

	// Make sure the solution is valid
	if (solution1 != solution1 || solution2 != solution2)
	{
		Debugger::GetInstance().Print(_T("Error (SolveForXY): Invalid solution"), Debugger::PriorityLow);

		return false;
	}

	// We now have two solutions.  To choose between them, we must examine the original
	// location of the point and the original sphere centers.  The two sphere centers
	// define a plane (assume plane is perpendicular to the ground plane), and the the two
	// solutions lie on opposite sides of the plane.  The first step in identifying the
	// correct solution is to determine which side of the original plane the original
	// point was on, and which side of the new plane either one of the solutions is on.

	// Get the plane normals
	Vector pointInPlane = originalCenter1;
	pointInPlane.z = 0.0;
	Vector originalNormal = (originalCenter1 - originalCenter2).Cross(originalCenter1 - pointInPlane);
	pointInPlane = center1;
	pointInPlane.z = 0.0;
	Vector newNormal = (center1 - center2).Cross(center1 - pointInPlane);

	// Get a vector from the location of the point to some point in the plane
	Vector originalVectorToPlane = originalCenter1 - original;
	Vector newVectorToPlane = center1 - solution1;

	// The dot products of the normal and the vector to the plane will give an indication
	// of which side of the plane the point is on
	double originalSide = originalNormal * originalVectorToPlane;
	double newSide = newNormal * newVectorToPlane;

	// We can compare the sign of the original side with the new side to choose the correct solution
	if ((newSide > 0 && originalSide > 0) || (newSide < 0 && originalSide < 0))
		current = solution1;
	else
		current = solution2;

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
//		wheelCenter			= const Vector& specifying the center of the wheel
//		wheelPlaneNormal	= const Vector& specifying the orientation of the wheel
//		tireRadius			= const double& specifying the loaded radius of the tire
//
// Output Arguments:
//		output	= Vector& specifying the location of the center of the tire's
//				  contact patch
//
// Return Value:
//		bool, true for success, false for error
//		
//==========================================================================
bool Kinematics::SolveForContactPatch(const Vector &wheelCenter,
	const Vector &wheelPlaneNormal, const double &tireRadius, Vector &output)
{
	Vector minimumZPoint;

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
	minimumZPoint.x = (wheelCenter.x * (pow(wheelPlaneNormal.x, 4) + 2 * pow(wheelPlaneNormal.x, 2)
		* pow(wheelPlaneNormal.y, 2) + pow(wheelPlaneNormal.y, 4) + pow(wheelPlaneNormal.x, 2)
		* pow(wheelPlaneNormal.z, 2) + pow(wheelPlaneNormal.y, 2) * pow(wheelPlaneNormal.z, 2))
		- sqrt(pow(tireRadius * wheelPlaneNormal.x * wheelPlaneNormal.z, 2)	* (pow(wheelPlaneNormal.x, 4)
		+ 2 * pow(wheelPlaneNormal.x, 2) * pow(wheelPlaneNormal.y, 2) + pow(wheelPlaneNormal.y, 4)
		+ pow(wheelPlaneNormal.x * wheelPlaneNormal.z, 2) + pow(wheelPlaneNormal.y * wheelPlaneNormal.z, 2))))
		/ (pow(wheelPlaneNormal.x, 4) + 2 * pow(wheelPlaneNormal.x * wheelPlaneNormal.y, 2)
		+ pow(wheelPlaneNormal.y, 4) + pow(wheelPlaneNormal.x * wheelPlaneNormal.z, 2)
		+ pow(wheelPlaneNormal.y * wheelPlaneNormal.z, 2));

	// Now we can plug back into our equations for Y and Z.  For Z, we'll solve the quadratic with
	// the quadratic equation.  Since A is always positive, we know we'll always want the minus
	// solution when we're looking for the minimum.
	double a, b, c;
	a = 1 + pow(wheelPlaneNormal.z / wheelPlaneNormal.y, 2);
	b = 2 * (wheelPlaneNormal.z / wheelPlaneNormal.y * wheelCenter.y - wheelCenter.z - wheelPlaneNormal.z
		/ pow(wheelPlaneNormal.y, 2) * (wheelCenter * wheelPlaneNormal - wheelPlaneNormal.x * minimumZPoint.x));
	c = pow(minimumZPoint.x - wheelCenter.x, 2) + pow(wheelCenter * wheelPlaneNormal - wheelPlaneNormal.x
		* minimumZPoint.x, 2) / pow(wheelPlaneNormal.y, 2) - 2 * wheelCenter.y / wheelPlaneNormal.y
		* (wheelCenter * wheelPlaneNormal - wheelPlaneNormal.x * minimumZPoint.x) + pow(wheelCenter.y, 2)
		+ pow(wheelCenter.z, 2) - pow(tireRadius, 2);
	minimumZPoint.z = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
	minimumZPoint.y = (wheelCenter * wheelPlaneNormal - minimumZPoint.x * wheelPlaneNormal.x
		- minimumZPoint.z * wheelPlaneNormal.z) / wheelPlaneNormal.y;

	// Check to make sure the solution is valid
	if (minimumZPoint != minimumZPoint)
	{
		// Return a zero-length vector
		output.Set(0.0, 0.0, 0.0);
		Debugger::GetInstance().Print(_T("Error (SolveForContactPatch):  Invalid solution"), Debugger::PriorityLow);

		return false;
	}

	// Assign the solution
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
//		v	= Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector
//		
//==========================================================================
// Solves the equation a dot v = 0 for a
Vector Kinematics::FindPerpendicularVector(const Vector &v)
{
	// a dot v => ax * vx + ay * vy + az * vz = 0
	// Find the minimum element of v; set the corresponding element of a to zero
	// Find the second minimum element of v; set the corresponding element of a to one
	// Solve for the remaining element of a
	Vector a;
	if (fabs(v.x) < fabs(v.y) && fabs(v.x) < fabs(v.z))// x smallest
	{
		a.x = 0;
		if (fabs(v.y) < fabs(v.z))// y second smallest
		{
			a.y = 1.0;
			a.z = -v.y / v.z;
		}
		else// z second smallest
		{
			a.z = 1.0;
			a.y = -v.z / v.y;
		}
	}
	else if (fabs(v.y) < v.z)// y smallest
	{
		a.y = 0;
		if (fabs(v.x) < fabs(v.z))// x second smallest
		{
			a.x = 1.0;
			a.z = -v.x / v.z;
		}
		else// z second smallest
		{
			a.z = 1.0;
			a.x = -v.z / v.x;
		}
	}
	else// z smallest
	{
		a.z = 0;
		if (fabs(v.x) < fabs(v.y))// x second smallest
		{
			a.x = 1.0;
			a.y = -v.x / v.y;
		}
		else// y second smallest
		{
			a.y = 1.0;
			a.x = -v.y / v.x;
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
//		center	= const Vector& specifying the circle center
//		a		= const Vector& specifying a vector having length equal to circle radius and lying in the circle plane
//		b		= const Vector& specifying a vector having length equal to circle radius and lying in the circle plane, perpendicular to a
//		target	= const Vector& original location of point we are solving for
//
// Output Arguments:
//		None
//
// Return Value:
//		dobule specifying parameter of parametric cirlce equation
//		
//==========================================================================
double Kinematics::OptimizeCircleParameter(const Vector &center, const Vector &a,
	const Vector &b, const Vector &target)
{
	double t;
	const unsigned int steps(12);
	const double step(2.0 * M_PI / steps);
	double bestT(-1.0), bestError(0.0);
	unsigned int i;
	Vector p;
	for (i = 0; i < steps; i++)
	{
		t = step * i;
		p = center + a * cos(t) + b * sin(t);
		if (bestT < 0.0 || (target - p).Length() < bestError)
		{
			bestT = t;
			bestError = (target - p).Length();
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
//		
//
// Output Arguments:
//		
//
// Return Value:
//		bool, true for success, false for error
//		
//==========================================================================
bool Kinematics::SolveInboardTBarPoints(const Vector &leftOutboard,
	const Vector &rightOutboard, const Vector &centerPivot, const Vector &pivotAxisPoint,
	const Vector &originalLeftOutboard, const Vector &originalRightOutboard,
	const Vector &originalCenterPivot, const Vector &originalPivotAxisPoint,
	const Vector &originalLeftInboard, const Vector &originalRightInboard,
	Vector &leftInboard, Vector &rightInboard)
{
	// We'll use parametric version of 3D circle equation
	// Additional parameters a and b are orthogonal to each other and circle plane normal
	// a and b have length equal to circle radius
	Vector normal;
	double angle, f, g;// f, g and angle are for law of cosines calcs
	double radius;

	Vector leftCenter, leftA, leftB;
	// on the left
	// sphere1 -> center = leftOutboard, R = leftOutboard - leftInboard
	// sphere2 -> center = centerPivot, R = centerPivot - leftInboard
	// circle1 -> intersection of sphere1 and sphere2
	normal = leftOutboard - centerPivot;
	if ((leftOutboard - centerPivot).Length() > normal.Length())
	{
		Debugger::GetInstance().Print(_T("Error (SolveInboardTBarPoints): Center distance exceeds sum of left radii"), Debugger::PriorityLow);
		return false;
	}
	f = (originalLeftOutboard - originalLeftInboard).Length();
	g = (originalCenterPivot - originalLeftInboard).Length();
	angle = acos((f * f + normal.Length() * normal.Length() - g * g) * 0.5 / f / normal.Length());
	leftCenter = leftOutboard - f * cos(angle) * normal.Normalize();
	radius = f * sin(angle);
	leftA = FindPerpendicularVector(normal);
	leftB = leftA.Cross(normal);
	leftA = leftA.Normalize() * radius;
	leftB = leftB.Normalize() * radius;

/*#ifdef USE_DEBUG_SHAPE
	DebugShape::Get()->SetSphere1(leftOutboard, (originalLeftOutboard - originalLeftInboard).Length());
	DebugShape::Get()->SetSphere2(centerPivot, (originalCenterPivot - originalLeftInboard).Length());
	//DebugShape::Get()->SetSphere3(rightOutboard, (originalRightOutboard - originalRightInboard).Length());
#endif*/

	Vector rightCenter, rightA, rightB;
	// on the right
	// sphere1 -> center = rightOutboard, R = rightOutboard - rightInboard
	// sphere2 -> center = centerPivot, R = centerPivot - rightInboard
	// circle2 -> intersection of sphere1 and sphere2
	normal = rightOutboard - centerPivot;
	if ((rightOutboard - centerPivot).Length() > normal.Length())
	{
		Debugger::GetInstance().Print(_T("Error (SolveInboardTBarPoints): Center distance exceeds sum of right radii"), Debugger::PriorityLow);
		return false;
	}
	f = (originalRightOutboard - originalRightInboard).Length();
	g = (originalCenterPivot - originalRightInboard).Length();
	angle = acos((f * f + normal.Length() * normal.Length() - g * g) * 0.5 / f / normal.Length());
	rightCenter = rightOutboard - f * cos(angle) * normal.Normalize();
	radius = f * sin(angle);
	rightA = FindPerpendicularVector(normal);
	rightB = rightA.Cross(normal);
	rightA = rightA.Normalize() * radius;
	rightB = rightB.Normalize() * radius;

	Vector centerA, centerB;
	// in the center
	// plane1 -> normal = centerPivot - pivotAxisPoint
	// point1 -> where line (leftInboard - rightInboard) intersects plane1
	// circle3 -> center = centerPivot, normal = centerPivot - pivotAxisPoint, R = (centerPivot - point1).Length()
	normal = originalCenterPivot - originalPivotAxisPoint;
	Vector originalTopMidPoint = VVASEMath::IntersectWithPlane(normal, originalCenterPivot,
		originalLeftInboard - originalRightInboard, originalLeftInboard);
	normal = centerPivot - pivotAxisPoint;
	radius = (originalCenterPivot - originalTopMidPoint).Length();
	centerA = FindPerpendicularVector(normal);
	centerB = centerA.Cross(normal);
	centerA = centerA.Normalize() * radius;
	centerB = centerB.Normalize() * radius;

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

	double leftTopLength = (originalLeftInboard - originalTopMidPoint).Length();
	double rightTopLength = (originalRightInboard - originalTopMidPoint).Length();

	// It's possible that a closed-form solution exists, but let's try an iterative method
	unsigned int i(0);
	const unsigned int limit(100);
	const double epsilon(1.0e-8);
	Matrix error(3, 1, epsilon, epsilon, epsilon);
	Matrix jacobian(3,3);
	Matrix guess(3,1);// parameteric variables for the three points
	Vector left(0.0, 0.0, 0.0), right(0.0, 0.0, 0.0), center(0.0, 0.0, 0.0);
	Matrix delta;

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

		error(0,0) = (left - center).Length() - leftTopLength;
		error(1,0) = (right - center).Length() - rightTopLength;
		error(2,0) = (left - right).Length() - leftTopLength - rightTopLength;
		//Debugger::GetInstance().Print(Debugger::PriorityLow, "i = %u; error = \n%s;\nguess = \n%s", i, error.Print().ToUTF8().data(), guess.Print().ToUTF8().data());

		// Compute jacobian
		left = leftCenter + leftA * cos(guess(0,0) + epsilon) + leftB * sin(guess(0,0) + epsilon);
		jacobian(0,0) = ((left - center).Length() - leftTopLength - error(0,0)) / epsilon;
		jacobian(1,0) = ((right - center).Length() - rightTopLength - error(1,0)) / epsilon;
		jacobian(2,0) = ((left - right).Length() - leftTopLength - rightTopLength - error(2,0)) / epsilon;
		left = leftCenter + leftA * cos(guess(0,0)) + leftB * sin(guess(0,0));

		right = rightCenter + rightA * cos(guess(1,0) + epsilon) + rightB * sin(guess(1,0) + epsilon);
		jacobian(0,1) = ((left - center).Length() - leftTopLength - error(0,0)) / epsilon;
		jacobian(1,1) = ((right - center).Length() - rightTopLength - error(1,0)) / epsilon;
		jacobian(2,1) = ((left - right).Length() - leftTopLength - rightTopLength - error(2,0)) / epsilon;
		right = rightCenter + rightA * cos(guess(1,0)) + rightB * sin(guess(1,0));

		center = centerPivot + centerA * cos(guess(2,0) + epsilon) + centerB * sin(guess(2,0) + epsilon);
		jacobian(0,2) = ((left - center).Length() - leftTopLength - error(0,0)) / epsilon;
		jacobian(1,2) = ((right - center).Length() - rightTopLength - error(1,0)) / epsilon;
		jacobian(2,2) = ((left - right).Length() - leftTopLength - rightTopLength - error(2,0)) / epsilon;
		center = centerPivot + centerA * cos(guess(2,0)) + centerB * sin(guess(2,0));

		// Compute next guess
		if (!jacobian.LeftDivide(error, delta))
		{
			Debugger::GetInstance().Print("Error:  Failed to invert jacobian", Debugger::PriorityLow);
			return false;
		}
		guess -= delta;

		i++;
	}

	if (i == limit)
		Debugger::GetInstance().Print(_T("Warning:  Iteration limit reached (SolveInboardTBarPoints)"), Debugger::PriorityMedium);

	rightInboard = right;
	leftInboard = left;

/*#ifdef USE_DEBUG_SHAPE
	DebugShape::Get()->SetPoint1(left);
	DebugShape::Get()->SetPoint2(right);
	DebugShape::Get()->SetPoint3(center);
#endif*/

	// Check constraints
	if (!VVASEMath::IsZero((left - center).Length() - leftTopLength, epsilon))
	{
		Debugger::GetInstance().Print(Debugger::PriorityLow, "Warning:  Incorrect left top T-bar length (Error = %f)", (left - center).Length() - leftTopLength);
		return false;
	}

	if (!VVASEMath::IsZero((right - center).Length() - rightTopLength, epsilon))
	{
		Debugger::GetInstance().Print(Debugger::PriorityLow, "Warning:  Incorrect right top T-bar length (Error = %f)", (right - center).Length() - rightTopLength);
		return false;
	}

	if (!VVASEMath::IsZero((right - left).Length() - leftTopLength - rightTopLength, epsilon))
	{
		Debugger::GetInstance().Print(Debugger::PriorityLow, "Warning:  Incorrect top T-bar length (Error = %f)", (right - left).Length() - leftTopLength - rightTopLength);
		return false;
	}

	return true;
}