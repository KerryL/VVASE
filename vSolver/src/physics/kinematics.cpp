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
	// Initialize the other pointers to zero
	originalCar = NULL;
	workingCar = NULL;
	localSuspension = NULL;
}

//==========================================================================
// Class:			Kinematics
// Function:		~Kinematics
//
// Description:		Destructor for Kinematics class.
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
Kinematics::~Kinematics()
{
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
	// Start the timer for this update
	wxStopWatch timer;
	timer.Start();

	// Print the car's name to the debug window
	Debugger::GetInstance().Print(_T("UpdateKinematics() for ") + name, Debugger::PriorityMedium);

	// Get the original and working cars
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
	localSuspension->MoveSteeringRack(inputs.rackTravel);

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
		// Rotations
		localSuspension->hardpoints[Suspension::FrontBarMidPoint].Rotate(inputs.centerOfRotation, rotations,
			inputs.firstRotation, secondRotation);

		// Translations
		localSuspension->hardpoints[Suspension::FrontBarMidPoint].z += inputs.heave;
		break;

	case Suspension::SwayBarTBar:
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
		// Rotations
		localSuspension->hardpoints[Suspension::RearBarMidPoint].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);

		// Translations
		localSuspension->hardpoints[Suspension::RearBarMidPoint].z += inputs.heave;
		break;

	case Suspension::SwayBarTBar:
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

	// Now the hardpoints have been moved and located - let's look at the outputs
	outputs.Update(originalCar, localSuspension);

	// Get the total elapsed time (includes solving for the outputs)
	long totalTime = timer.Time();

	// Print the time to the output pane
	Debugger::GetInstance().Print(Debugger::PriorityLow, "Finished UpdateKinematcs() for %s in %0.3f sec",
		name.c_str(), totalTime / 1000.0);
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

	// Declare and initialize the return value to true
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

	corner.hardpoints[Corner::ContactPatch].z = 1.0;// Must be initialized to > Tolerance
	while (iteration <= limit && fabs(corner.hardpoints[Corner::ContactPatch].z) > tolerance)
	{
		if (!Suspension::SolveForXY(corner.hardpoints[Corner::LowerFrontTubMount],
			corner.hardpoints[Corner::LowerRearTubMount], originalCorner.hardpoints[Corner::LowerFrontTubMount],
			originalCorner.hardpoints[Corner::LowerRearTubMount], originalCorner.hardpoints[Corner::LowerBallJoint],
			corner.hardpoints[Corner::LowerBallJoint]))
		{
			// Print an error and switch the success boolean to indicate a failure
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for lower ball joint!"), Debugger::PriorityMedium);
			success = false;
		}

		if (!Suspension::SolveForPoint(corner.hardpoints[Corner::LowerBallJoint],
			corner.hardpoints[Corner::UpperFrontTubMount], corner.hardpoints[Corner::UpperRearTubMount],
			originalCorner.hardpoints[Corner::LowerBallJoint], originalCorner.hardpoints[Corner::UpperFrontTubMount],
			originalCorner.hardpoints[Corner::UpperRearTubMount], originalCorner.hardpoints[Corner::UpperBallJoint],
			corner.hardpoints[Corner::UpperBallJoint]))
		{
			// Print an error and switch the success boolean to indicate a failure
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for upper ball joint!"), Debugger::PriorityMedium);
			success = false;
		}

		if (!Suspension::SolveForPoint(corner.hardpoints[Corner::LowerBallJoint],
			corner.hardpoints[Corner::UpperBallJoint], corner.hardpoints[Corner::InboardTieRod],
			originalCorner.hardpoints[Corner::LowerBallJoint], originalCorner.hardpoints[Corner::UpperBallJoint],
			originalCorner.hardpoints[Corner::InboardTieRod], originalCorner.hardpoints[Corner::OutboardTieRod],
			corner.hardpoints[Corner::OutboardTieRod]))
		{
			// Print an error and switch the success boolean to indicate a failure
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard tie rod!"), Debugger::PriorityMedium);
			success = false;
		}

		if (!Suspension::SolveForPoint(corner.hardpoints[Corner::LowerBallJoint],
			corner.hardpoints[Corner::UpperBallJoint], corner.hardpoints[Corner::OutboardTieRod],
			originalCorner.hardpoints[Corner::LowerBallJoint], originalCorner.hardpoints[Corner::UpperBallJoint],
			originalCorner.hardpoints[Corner::OutboardTieRod], originalCorner.hardpoints[Corner::WheelCenter],
			corner.hardpoints[Corner::WheelCenter]))
		{
			// Print an error and switch the success boolean to indicate a failure
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for wheel center!"), Debugger::PriorityMedium);
			success = false;
		}

		// Solve for the contact patch
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
		Suspension::SolveForContactPatch(corner.hardpoints[Corner::WheelCenter], wheelNormal,
			originalCorner.hardpoints[Corner::ContactPatch].Distance(originalCorner.hardpoints[Corner::WheelCenter]),
			corner.hardpoints[Corner::ContactPatch]);

		// With the origin on the ground, the error is equal to the Z location of the contact patch
		// FIXME:  This is only true when using flat ground model!!!
		// Use a line search algorithm to adjust the height of the lower ball joint
		// This could get hung up if the relationship between lower ball joint height and contact
		// patch Z location is highly non-linear, but that would be an unusual case (extreme camber gain?)

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

		// Increment the iteration counter
		iteration++;
	}

	// Check to make sure we finished the loop because we were within the tolerance (and not
	// because we hit the iteration limit)
	if ((fabs(corner.hardpoints[Corner::ContactPatch].z) > tolerance))
	{
		// Print an error and switch the success boolean to indicate a failure
		Debugger::GetInstance().Print(_T("Warning (SolveCorner):  Contact patch location did not converge"),
			Debugger::PriorityMedium);
		success = false;
	}

	// Outboard shock/spring actuators
	if (corner.actuationType == Corner::ActuationPushPullrod)
	{
		if (corner.actuationAttachment == Corner::AttachmentLowerAArm)
		{
			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::LowerBallJoint],
				corner.hardpoints[Corner::LowerFrontTubMount], corner.hardpoints[Corner::LowerRearTubMount],
				originalCorner.hardpoints[Corner::LowerBallJoint], originalCorner.hardpoints[Corner::LowerFrontTubMount],
				originalCorner.hardpoints[Corner::LowerRearTubMount], originalCorner.hardpoints[Corner::OutboardPushrod],
				corner.hardpoints[Corner::OutboardPushrod]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard pushrod!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (corner.actuationAttachment == Corner::AttachmentUpperAArm)
		{
			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::UpperBallJoint],
				corner.hardpoints[Corner::UpperFrontTubMount], corner.hardpoints[Corner::UpperRearTubMount],
				originalCorner.hardpoints[Corner::UpperBallJoint], originalCorner.hardpoints[Corner::UpperFrontTubMount],
				originalCorner.hardpoints[Corner::UpperRearTubMount], originalCorner.hardpoints[Corner::OutboardPushrod],
				corner.hardpoints[Corner::OutboardPushrod]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard pullrod!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (corner.actuationAttachment == Corner::AttachmentUpright)
		{
			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::UpperBallJoint],
				corner.hardpoints[Corner::LowerBallJoint], corner.hardpoints[Corner::OutboardTieRod],
				originalCorner.hardpoints[Corner::UpperBallJoint], originalCorner.hardpoints[Corner::LowerBallJoint],
				originalCorner.hardpoints[Corner::OutboardTieRod], originalCorner.hardpoints[Corner::OutboardPushrod],
				corner.hardpoints[Corner::OutboardPushrod]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard push/pullrod!"), Debugger::PriorityMedium);
				success = false;
			}
		}

		// Inboard Pushrods
		if (!Suspension::SolveForPoint(corner.hardpoints[Corner::BellCrankPivot1],
			corner.hardpoints[Corner::BellCrankPivot2], corner.hardpoints[Corner::OutboardPushrod],
			originalCorner.hardpoints[Corner::BellCrankPivot1], originalCorner.hardpoints[Corner::BellCrankPivot2],
			originalCorner.hardpoints[Corner::OutboardPushrod], originalCorner.hardpoints[Corner::InboardPushrod],
			corner.hardpoints[Corner::InboardPushrod]))
		{
			// Print an error and switch the success boolean to indicate a failure
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for inboard push/pullrod!"), Debugger::PriorityMedium);
			success = false;
		}

		// Outboard Shocks
		if (!Suspension::SolveForPoint(corner.hardpoints[Corner::BellCrankPivot1],
			corner.hardpoints[Corner::BellCrankPivot2], corner.hardpoints[Corner::InboardPushrod],
			originalCorner.hardpoints[Corner::BellCrankPivot1], originalCorner.hardpoints[Corner::BellCrankPivot2],
			originalCorner.hardpoints[Corner::InboardPushrod], originalCorner.hardpoints[Corner::OutboardShock],
			corner.hardpoints[Corner::OutboardShock]))
		{
			// Print an error and switch the success boolean to indicate a failure
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
			success = false;
		}

		// Outboard Springs
		if (!Suspension::SolveForPoint(corner.hardpoints[Corner::BellCrankPivot1],
			corner.hardpoints[Corner::BellCrankPivot2], corner.hardpoints[Corner::InboardPushrod],
			originalCorner.hardpoints[Corner::BellCrankPivot1], originalCorner.hardpoints[Corner::BellCrankPivot2],
			originalCorner.hardpoints[Corner::InboardPushrod], originalCorner.hardpoints[Corner::OutboardSpring],
			corner.hardpoints[Corner::OutboardSpring]))
		{
			// Print an error and switch the success boolean to indicate a failure
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
			success = false;
		}

		// Sway Bars Outboard
		if ((localSuspension->frontBarStyle != Suspension::SwayBarNone && isAtFront) ||
			(localSuspension->rearBarStyle != Suspension::SwayBarNone && !isAtFront))
		{
			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::BellCrankPivot1],
				corner.hardpoints[Corner::BellCrankPivot2], corner.hardpoints[Corner::InboardPushrod],
				originalCorner.hardpoints[Corner::BellCrankPivot1], originalCorner.hardpoints[Corner::BellCrankPivot2],
				originalCorner.hardpoints[Corner::InboardPushrod], originalCorner.hardpoints[Corner::OutboardBarLink],
				corner.hardpoints[Corner::OutboardBarLink]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard swaybar!"), Debugger::PriorityMedium);
				success = false;
			}
		}

		// Sway Bars Inboard
		if (localSuspension->frontBarStyle == Suspension::SwayBarUBar && isAtFront)
		{
			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::BarArmAtPivot],
				corner.hardpoints[Corner::OutboardBarLink], localSuspension->hardpoints[Suspension::FrontBarMidPoint],// FIXME:  Do we really need the midpoint?
				originalCorner.hardpoints[Corner::BarArmAtPivot], originalCorner.hardpoints[Corner::OutboardBarLink],
				originalCar->suspension->hardpoints[Suspension::FrontBarMidPoint], originalCorner.hardpoints[Corner::InboardBarLink],
				corner.hardpoints[Corner::InboardBarLink]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for inboard U-bar (front)!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (localSuspension->rearBarStyle == Suspension::SwayBarUBar && !isAtFront)
		{
			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::BarArmAtPivot],
				corner.hardpoints[Corner::OutboardBarLink], localSuspension->hardpoints[Suspension::RearBarMidPoint],
				originalCorner.hardpoints[Corner::BarArmAtPivot], originalCorner.hardpoints[Corner::OutboardBarLink],
				originalCar->suspension->hardpoints[Suspension::RearBarMidPoint], originalCorner.hardpoints[Corner::InboardBarLink],
				corner.hardpoints[Corner::InboardBarLink]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for inboard U-bar (rear)!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (localSuspension->frontBarStyle == Suspension::SwayBarTBar && isAtFront)
		{
			Debugger::GetInstance().Print(_T("  T-Bar Front"), Debugger::PriorityHigh);
			// FIXME:  Need to accommodate T-bars
			// Need to solve for InboardBarLink
			// Two of the three points needed are: BarMidPoint and OutboardBarLink
			// What is the third point?
			// Maybe the solution is actually the intersection of two circles and a torus?
		}
		else if (localSuspension->rearBarStyle == Suspension::SwayBarTBar && !isAtFront)
		{
			Debugger::GetInstance().Print(_T("  T-Bar Rear"), Debugger::PriorityHigh);
			// FIXME:  Need to accommodate T-bars
		}
		else if ((localSuspension->frontBarStyle == Suspension::SwayBarGeared && isAtFront) ||
			(localSuspension->rearBarStyle == Suspension::SwayBarGeared && !isAtFront))
		{
			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::BarArmAtPivot],
				corner.hardpoints[Corner::OutboardBarLink], corner.hardpoints[Corner::GearEndBarShaft],
				originalCorner.hardpoints[Corner::BarArmAtPivot], originalCorner.hardpoints[Corner::OutboardBarLink],
				originalCorner.hardpoints[Corner::GearEndBarShaft], originalCorner.hardpoints[Corner::InboardBarLink],
				corner.hardpoints[Corner::InboardBarLink]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for geared bar!"), Debugger::PriorityMedium);
				success = false;
			}
		}
	}
	else if (corner.actuationType == Corner::ActuationOutboard)
	{
		// Outboard spring/shock units  - no pushrod/bell crank

		if (corner.actuationAttachment == Corner::AttachmentLowerAArm)
		{
			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::LowerBallJoint],
				corner.hardpoints[Corner::LowerFrontTubMount], corner.hardpoints[Corner::LowerRearTubMount],
				originalCorner.hardpoints[Corner::LowerBallJoint], originalCorner.hardpoints[Corner::LowerFrontTubMount],
				originalCorner.hardpoints[Corner::LowerRearTubMount], originalCorner.hardpoints[Corner::OutboardSpring],
				corner.hardpoints[Corner::OutboardSpring]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
				success = false;
			}

			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::LowerBallJoint],
				corner.hardpoints[Corner::LowerFrontTubMount], corner.hardpoints[Corner::LowerRearTubMount],
				originalCorner.hardpoints[Corner::LowerBallJoint], originalCorner.hardpoints[Corner::LowerFrontTubMount],
				originalCorner.hardpoints[Corner::LowerRearTubMount], originalCorner.hardpoints[Corner::OutboardShock],
				corner.hardpoints[Corner::OutboardShock]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (corner.actuationAttachment == Corner::AttachmentUpperAArm)
		{
			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::UpperBallJoint],
				corner.hardpoints[Corner::UpperFrontTubMount], corner.hardpoints[Corner::UpperRearTubMount],
				originalCorner.hardpoints[Corner::UpperBallJoint], originalCorner.hardpoints[Corner::UpperFrontTubMount],
				originalCorner.hardpoints[Corner::UpperRearTubMount], originalCorner.hardpoints[Corner::OutboardSpring],
				corner.hardpoints[Corner::OutboardSpring]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
				success = false;
			}

			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::UpperBallJoint],
				corner.hardpoints[Corner::UpperFrontTubMount], corner.hardpoints[Corner::UpperRearTubMount],
				originalCorner.hardpoints[Corner::UpperBallJoint], originalCorner.hardpoints[Corner::UpperFrontTubMount],
				originalCorner.hardpoints[Corner::UpperRearTubMount], originalCorner.hardpoints[Corner::OutboardShock],
				corner.hardpoints[Corner::OutboardShock]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (corner.actuationAttachment == Corner::AttachmentUpright)
		{
			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::UpperBallJoint],
				corner.hardpoints[Corner::LowerBallJoint], corner.hardpoints[Corner::OutboardTieRod],
				originalCorner.hardpoints[Corner::UpperBallJoint], originalCorner.hardpoints[Corner::LowerBallJoint],
				originalCorner.hardpoints[Corner::OutboardTieRod], originalCorner.hardpoints[Corner::OutboardSpring],
				corner.hardpoints[Corner::OutboardSpring]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
				success = false;
			}

			if (!Suspension::SolveForPoint(corner.hardpoints[Corner::UpperBallJoint],
				corner.hardpoints[Corner::LowerBallJoint], corner.hardpoints[Corner::OutboardTieRod],
				originalCorner.hardpoints[Corner::UpperBallJoint], originalCorner.hardpoints[Corner::LowerBallJoint],
				originalCorner.hardpoints[Corner::OutboardTieRod], originalCorner.hardpoints[Corner::OutboardShock],
				corner.hardpoints[Corner::OutboardShock]))
			{
				// Print an error and switch the success boolean to indicate a failure
				Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
				success = false;
			}
		}
	}

	// Half Shafts
	if ((originalCar->HasFrontHalfShafts() && isAtFront) ||
		(originalCar->HasRearHalfShafts() && !isAtFront))
	{
		if (!Suspension::SolveForPoint(corner.hardpoints[Corner::LowerBallJoint],
			corner.hardpoints[Corner::UpperBallJoint], corner.hardpoints[Corner::OutboardTieRod],
			originalCorner.hardpoints[Corner::LowerBallJoint], originalCorner.hardpoints[Corner::UpperBallJoint],
			originalCorner.hardpoints[Corner::OutboardTieRod], originalCorner.hardpoints[Corner::OutboardHalfShaft],
			corner.hardpoints[Corner::OutboardHalfShaft]))
		{
			// Print an error and switch the success boolean to indicate a failure
			Debugger::GetInstance().Print(_T("ERROR:  Failed to solve for outboard half shaft!"), Debugger::PriorityMedium);
			success = false;
		}
	}

	return success;
}