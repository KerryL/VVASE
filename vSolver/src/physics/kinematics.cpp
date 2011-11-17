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
//	2/24/2008	- Made major changes to class when DRIVETRAIN object was removed.  Removed
//				  all dynamic memory allocation from constructor, K. Loux.
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	3/15/2008	- Finished contact patch line search algorithm, K. Loux.
//	3/23/2008	- Changed units for class members and functions from degrees to radians and
//				  renamed class from Kinematics.  Also introduce OUTPUTS class and CORNER class
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

//==========================================================================
// Class:			Kinematics
// Function:		Kinematics
//
// Description:		Constructor for Kinematics class.
//
// Input Arguments:
//		_debugger	= const Debugger&, reference to the debug message printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Kinematics::Kinematics(const Debugger &_debugger) : debugger(_debugger)
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
//		_originalCar	= const CAR* - for reference only
//		_workingCar		= CAR* to be changed (updated) by this function
//		name			= wxString used to print messages about this car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Kinematics::UpdateKinematics(const CAR* _originalCar, CAR* _workingCar, wxString name)
{
	// Start the timer for this update
	wxStopWatch timer;
	timer.Start();

	// Print the car's name to the debug window
	debugger.Print(_T("UpdateKinematics() for ") + name, Debugger::PriorityMedium);

	// Get the original and working cars
	originalCar = _originalCar;// This one is for reference and won't be changed by this class
	workingCar = _workingCar;

	// Ensure exclusive access to the car objects
	wxMutexLocker originalLock(originalCar->GetMutex());
	wxMutexLocker workingLock(workingCar->GetMutex());
	
	// Copy the information in the original car to the working car.  This minimizes rounding
	// errors in the calculation of suspension points, and it also ensures that changes made
	// to other sub-systems are carried over into the working car.
	*workingCar = *originalCar;

	// Now we copy the pointer to the working car's suspension to the class member
	// This must be AFTER *WorkingCar = *OriginalCar, since this assignment is a deep copy
	// and the address of the suspension will change!
	localSuspension = workingCar->Suspension;

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
		debugger.Print(_T("ERROR (UpdateKinematics):  Unable to determine order of Euler rotations"));
		return;
	}

	// Solve the chassis-mounted points for left-right common items (3rd springs/shocks, sway bars)
	// Front sway bars
	// NOTE:  This is only for points that are NOT in the CORNER class
	switch (localSuspension->FrontBarStyle)
	{
	case SUSPENSION::SwayBarUBar:
		// Rotations
		localSuspension->Hardpoints[SUSPENSION::FrontBarMidPoint].Rotate(inputs.centerOfRotation, rotations,
			inputs.firstRotation, secondRotation);

		// Translations
		localSuspension->Hardpoints[SUSPENSION::FrontBarMidPoint].z += inputs.heave;
		break;

	case SUSPENSION::SwayBarTBar:
	case SUSPENSION::SwayBarGeared:
	case SUSPENSION::SwayBarNone:
	default:
		// Nothing here
		break;
	}

	// Rear sway bars
	switch (localSuspension->RearBarStyle)
	{
	case SUSPENSION::SwayBarUBar:
		// Rotations
		localSuspension->Hardpoints[SUSPENSION::RearBarMidPoint].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);

		// Translations
		localSuspension->Hardpoints[SUSPENSION::RearBarMidPoint].z += inputs.heave;
		break;

	case SUSPENSION::SwayBarTBar:
	case SUSPENSION::SwayBarGeared:
	case SUSPENSION::SwayBarNone:
	default:
		// Nothing here
		break;
	}

	// Third springs
	if (localSuspension->FrontHasThirdSpring)
	{
		// Rotations
		localSuspension->Hardpoints[SUSPENSION::FrontThirdSpringInboard].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);
		localSuspension->Hardpoints[SUSPENSION::FrontThirdShockInboard].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);

		// Translations
		localSuspension->Hardpoints[SUSPENSION::FrontThirdSpringInboard].z += inputs.heave;
		localSuspension->Hardpoints[SUSPENSION::FrontThirdShockInboard].z += inputs.heave;
	}

	if (localSuspension->RearHasThirdSpring)
	{
		// Rotations
		localSuspension->Hardpoints[SUSPENSION::RearThirdSpringInboard].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);
		localSuspension->Hardpoints[SUSPENSION::RearThirdShockInboard].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);

		// Translations
		localSuspension->Hardpoints[SUSPENSION::RearThirdSpringInboard].z += inputs.heave;
		localSuspension->Hardpoints[SUSPENSION::RearThirdShockInboard].z += inputs.heave;
	}

	if (!SolveCorner(localSuspension->RightFront, originalCar->Suspension->RightFront, rotations, secondRotation))
		debugger.Print(_T("ERROR:  Problem solving right front corner!  Increase debug level for more information."),
			Debugger::PriorityHigh);
	if (!SolveCorner(localSuspension->LeftFront, originalCar->Suspension->LeftFront, rotations, secondRotation))
		debugger.Print(_T("ERROR:  Problem solving left front corner!  Increase debug level for more information."),
			Debugger::PriorityHigh);
	if (!SolveCorner(localSuspension->RightRear, originalCar->Suspension->RightRear, rotations, secondRotation))
		debugger.Print(_T("ERROR:  Problem solving right rear corner!  Increase debug level for more information."),
			Debugger::PriorityHigh);
	if (!SolveCorner(localSuspension->LeftRear, originalCar->Suspension->LeftRear, rotations, secondRotation))
		debugger.Print(_T("ERROR:  Problem solving left rear corner!  Increase debug level for more information."),
			Debugger::PriorityHigh);

	// Now the hardpoints have been moved and located - let's look at the outputs
	outputs.Update(originalCar, localSuspension);

	// Get the total elapsed time (includes solving for the outputs)
	long totalTime = timer.Time();

	// Print the time to the output pane
	debugger.Print(Debugger::PriorityLow, "Finished UpdateKinematcs() for %s in %0.3f sec",
		name.c_str(), totalTime / 1000.0);

	return;
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
//		originalCorner	= const CORNER*, the un-perturbed locations of the
//						  suspension hardpoints
//		rotations		= const Vector* specifying the amount to rotate the chassis
//						  about each axis
//		secondRotation	= const Vector::Axis* describing the second axis of rotation
//						  (the first is an input quantity)
//
// Output Arguments:
//		corner	= CORNER*, the corner of the car we are manipulating
//
// Return Value:
//		bool, true for success, false for error(s)
//
//==========================================================================
bool Kinematics::SolveCorner(CORNER &corner, const CORNER &originalCorner,
							 const Vector &rotations, const Vector::Axis &secondRotation)
{
	// Determine if this corner is at the front or the rear of the car
	bool isAtFront = false;
	if (corner.Location == CORNER::LocationRightFront || corner.Location == CORNER::LocationLeftFront)
		isAtFront = true;

	// First move all of the body-fixed points (rotations first)
	corner.Hardpoints[CORNER::LowerFrontTubMount].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.Hardpoints[CORNER::LowerRearTubMount].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.Hardpoints[CORNER::UpperFrontTubMount].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.Hardpoints[CORNER::UpperRearTubMount].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.Hardpoints[CORNER::BarArmAtPivot].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.Hardpoints[CORNER::InboardSpring].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.Hardpoints[CORNER::InboardShock].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);
	corner.Hardpoints[CORNER::InboardTieRod].Rotate(inputs.centerOfRotation,
		rotations, inputs.firstRotation, secondRotation);

	// Now do the translations on the same points
	corner.Hardpoints[CORNER::LowerFrontTubMount].z += inputs.heave;
	corner.Hardpoints[CORNER::LowerRearTubMount].z += inputs.heave;
	corner.Hardpoints[CORNER::UpperFrontTubMount].z += inputs.heave;
	corner.Hardpoints[CORNER::UpperRearTubMount].z += inputs.heave;
	corner.Hardpoints[CORNER::BarArmAtPivot].z += inputs.heave;
	corner.Hardpoints[CORNER::InboardSpring].z += inputs.heave;
	corner.Hardpoints[CORNER::InboardShock].z += inputs.heave;
	corner.Hardpoints[CORNER::InboardTieRod].z += inputs.heave;

	// Depending on the type of actuation, we might have to move some additional points, as well
	if (corner.ActuationType == CORNER::ActuationPushPullrod)
	{
		corner.Hardpoints[CORNER::BellCrankPivot1].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);
		corner.Hardpoints[CORNER::BellCrankPivot2].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);

		corner.Hardpoints[CORNER::BellCrankPivot1].z += inputs.heave;
		corner.Hardpoints[CORNER::BellCrankPivot2].z += inputs.heave;
	}

	// Do the rotation and translation for the inboard half shafts at the same time
	if ((originalCar->HasFrontHalfShafts() && isAtFront) ||
		(originalCar->HasRearHalfShafts() && !isAtFront))
	{
		corner.Hardpoints[CORNER::InboardHalfShaft].Rotate(inputs.centerOfRotation,
			rotations, inputs.firstRotation, secondRotation);
		corner.Hardpoints[CORNER::InboardHalfShaft].z += inputs.heave;
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

	corner.Hardpoints[CORNER::ContactPatch].z = 1.0;// Must be initialized to > Tolerance
	while (iteration <= limit && fabs(corner.Hardpoints[CORNER::ContactPatch].z) > tolerance)
	{
		if (!SUSPENSION::SolveForXY(corner.Hardpoints[CORNER::LowerFrontTubMount],
			corner.Hardpoints[CORNER::LowerRearTubMount], originalCorner.Hardpoints[CORNER::LowerFrontTubMount],
			originalCorner.Hardpoints[CORNER::LowerRearTubMount], originalCorner.Hardpoints[CORNER::LowerBallJoint],
			corner.Hardpoints[CORNER::LowerBallJoint]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for lower ball joint!"), Debugger::PriorityMedium);
			success = false;
		}

		if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::LowerBallJoint],
			corner.Hardpoints[CORNER::UpperFrontTubMount], corner.Hardpoints[CORNER::UpperRearTubMount],
			originalCorner.Hardpoints[CORNER::LowerBallJoint], originalCorner.Hardpoints[CORNER::UpperFrontTubMount],
			originalCorner.Hardpoints[CORNER::UpperRearTubMount], originalCorner.Hardpoints[CORNER::UpperBallJoint],
			corner.Hardpoints[CORNER::UpperBallJoint]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for upper ball joint!"), Debugger::PriorityMedium);
			success = false;
		}

		if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::LowerBallJoint],
			corner.Hardpoints[CORNER::UpperBallJoint], corner.Hardpoints[CORNER::InboardTieRod],
			originalCorner.Hardpoints[CORNER::LowerBallJoint], originalCorner.Hardpoints[CORNER::UpperBallJoint],
			originalCorner.Hardpoints[CORNER::InboardTieRod], originalCorner.Hardpoints[CORNER::OutboardTieRod],
			corner.Hardpoints[CORNER::OutboardTieRod]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for outboard tie rod!"), Debugger::PriorityMedium);
			success = false;
		}

		if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::LowerBallJoint],
			corner.Hardpoints[CORNER::UpperBallJoint], corner.Hardpoints[CORNER::OutboardTieRod],
			originalCorner.Hardpoints[CORNER::LowerBallJoint], originalCorner.Hardpoints[CORNER::UpperBallJoint],
			originalCorner.Hardpoints[CORNER::OutboardTieRod], originalCorner.Hardpoints[CORNER::WheelCenter],
			corner.Hardpoints[CORNER::WheelCenter]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for wheel center!"), Debugger::PriorityMedium);
			success = false;
		}

		// Solve for the contact patch
		originalPlaneNormal = VVASEMath::GetPlaneNormal(originalCorner.Hardpoints[CORNER::LowerBallJoint],
			originalCorner.Hardpoints[CORNER::UpperBallJoint], originalCorner.Hardpoints[CORNER::OutboardTieRod]);
		newPlaneNormal = VVASEMath::GetPlaneNormal(corner.Hardpoints[CORNER::LowerBallJoint],
			corner.Hardpoints[CORNER::UpperBallJoint], corner.Hardpoints[CORNER::OutboardTieRod]);

		// For some operations, if this is on the right-hand side of the car, the sign gets flipped
		double sign = 1.0;
		if (corner.Location == CORNER::LocationRightFront || corner.Location == CORNER::LocationRightRear)
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
		wheelNormal.Rotate(sign * originalCorner.StaticCamber, Vector::AxisX);
		wheelNormal.Rotate(sign * originalCorner.StaticToe, Vector::AxisZ);

		// Do the actual solving for the contact patch
		SUSPENSION::SolveForContactPatch(corner.Hardpoints[CORNER::WheelCenter], wheelNormal,
			originalCorner.Hardpoints[CORNER::ContactPatch].Distance(originalCorner.Hardpoints[CORNER::WheelCenter]),
			corner.Hardpoints[CORNER::ContactPatch]);

		// With the origin on the ground, the error is equal to the Z location of the contact patch
		// FIXME:  This is only true when using flat ground model!!!
		// Use a line search algorithm to adjust the height of the lower ball joint
		// This could get hung up if the relationship between lower ball joint height and contact
		// patch Z location is highly non-linear, but that would be an unusual case (extreme camber gain?)

		// If this is the first iteration, initialize the limit variables
		if (iteration == 1)
		{
			upperLimit = corner.Hardpoints[CORNER::LowerBallJoint].z +
				3.0 * fabs(corner.Hardpoints[CORNER::ContactPatch].z);
			lowerLimit = corner.Hardpoints[CORNER::LowerBallJoint].z -
				3.0 * fabs(corner.Hardpoints[CORNER::ContactPatch].z);
		}

		// Make the adjustment in the guess
		if (corner.Hardpoints[CORNER::ContactPatch].z > tolerance)
			upperLimit = corner.Hardpoints[CORNER::LowerBallJoint].z;
		else if (corner.Hardpoints[CORNER::ContactPatch].z < -tolerance)
			lowerLimit = corner.Hardpoints[CORNER::LowerBallJoint].z;
		corner.Hardpoints[CORNER::LowerBallJoint].z = lowerLimit + (upperLimit - lowerLimit) / 2.0;

		// Increment the iteration counter
		iteration++;
	}

	// Check to make sure we finished the loop because we were within the tolerance (and not
	// because we hit the iteration limit)
	if ((fabs(corner.Hardpoints[CORNER::ContactPatch].z) > tolerance))
	{
		// Print an error and switch the success boolean to indicate a failure
		debugger.Print(_T("Warning (SolveCorner):  Contact patch location did not converge"),
			Debugger::PriorityMedium);
		success = false;
	}

	// Outboard shock/spring actuators
	if (corner.ActuationType == CORNER::ActuationPushPullrod)
	{
		if (corner.ActuationAttachment == CORNER::AttachmentLowerAArm)
		{
			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::LowerBallJoint],
				corner.Hardpoints[CORNER::LowerFrontTubMount], corner.Hardpoints[CORNER::LowerRearTubMount],
				originalCorner.Hardpoints[CORNER::LowerBallJoint], originalCorner.Hardpoints[CORNER::LowerFrontTubMount],
				originalCorner.Hardpoints[CORNER::LowerRearTubMount], originalCorner.Hardpoints[CORNER::OutboardPushrod],
				corner.Hardpoints[CORNER::OutboardPushrod]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard pushrod!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (corner.ActuationAttachment == CORNER::AttachmentUpperAArm)
		{
			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::UpperBallJoint],
				corner.Hardpoints[CORNER::UpperFrontTubMount], corner.Hardpoints[CORNER::UpperRearTubMount],
				originalCorner.Hardpoints[CORNER::UpperBallJoint], originalCorner.Hardpoints[CORNER::UpperFrontTubMount],
				originalCorner.Hardpoints[CORNER::UpperRearTubMount], originalCorner.Hardpoints[CORNER::OutboardPushrod],
				corner.Hardpoints[CORNER::OutboardPushrod]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard pullrod!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (corner.ActuationAttachment == CORNER::AttachmentUpright)
		{
			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::UpperBallJoint],
				corner.Hardpoints[CORNER::LowerBallJoint], corner.Hardpoints[CORNER::OutboardTieRod],
				originalCorner.Hardpoints[CORNER::UpperBallJoint], originalCorner.Hardpoints[CORNER::LowerBallJoint],
				originalCorner.Hardpoints[CORNER::OutboardTieRod], originalCorner.Hardpoints[CORNER::OutboardPushrod],
				corner.Hardpoints[CORNER::OutboardPushrod]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard push/pullrod!"), Debugger::PriorityMedium);
				success = false;
			}
		}

		// Inboard Pushrods
		if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::BellCrankPivot1],
			corner.Hardpoints[CORNER::BellCrankPivot2], corner.Hardpoints[CORNER::OutboardPushrod],
			originalCorner.Hardpoints[CORNER::BellCrankPivot1], originalCorner.Hardpoints[CORNER::BellCrankPivot2],
			originalCorner.Hardpoints[CORNER::OutboardPushrod], originalCorner.Hardpoints[CORNER::InboardPushrod],
			corner.Hardpoints[CORNER::InboardPushrod]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for inboard push/pullrod!"), Debugger::PriorityMedium);
			success = false;
		}

		// Outboard Shocks
		if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::BellCrankPivot1],
			corner.Hardpoints[CORNER::BellCrankPivot2], corner.Hardpoints[CORNER::InboardPushrod],
			originalCorner.Hardpoints[CORNER::BellCrankPivot1], originalCorner.Hardpoints[CORNER::BellCrankPivot2],
			originalCorner.Hardpoints[CORNER::InboardPushrod], originalCorner.Hardpoints[CORNER::OutboardShock],
			corner.Hardpoints[CORNER::OutboardShock]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
			success = false;
		}

		// Outboard Springs
		if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::BellCrankPivot1],
			corner.Hardpoints[CORNER::BellCrankPivot2], corner.Hardpoints[CORNER::InboardPushrod],
			originalCorner.Hardpoints[CORNER::BellCrankPivot1], originalCorner.Hardpoints[CORNER::BellCrankPivot2],
			originalCorner.Hardpoints[CORNER::InboardPushrod], originalCorner.Hardpoints[CORNER::OutboardSpring],
			corner.Hardpoints[CORNER::OutboardSpring]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
			success = false;
		}

		// Sway Bars Outboard
		if ((localSuspension->FrontBarStyle != SUSPENSION::SwayBarNone && isAtFront) ||
			(localSuspension->RearBarStyle != SUSPENSION::SwayBarNone && !isAtFront))
		{
			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::BellCrankPivot1],
				corner.Hardpoints[CORNER::BellCrankPivot2], corner.Hardpoints[CORNER::InboardPushrod],
				originalCorner.Hardpoints[CORNER::BellCrankPivot1], originalCorner.Hardpoints[CORNER::BellCrankPivot2],
				originalCorner.Hardpoints[CORNER::InboardPushrod], originalCorner.Hardpoints[CORNER::OutboardBarLink],
				corner.Hardpoints[CORNER::OutboardBarLink]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard swaybar!"), Debugger::PriorityMedium);
				success = false;
			}
		}

		// Sway Bars Inboard
		if (localSuspension->FrontBarStyle == SUSPENSION::SwayBarUBar && isAtFront)
		{
			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::BarArmAtPivot],
				corner.Hardpoints[CORNER::OutboardBarLink], localSuspension->Hardpoints[SUSPENSION::FrontBarMidPoint],// FIXME:  Do we really need the midpoint?
				originalCorner.Hardpoints[CORNER::BarArmAtPivot], originalCorner.Hardpoints[CORNER::OutboardBarLink],
				originalCar->Suspension->Hardpoints[SUSPENSION::FrontBarMidPoint], originalCorner.Hardpoints[CORNER::InboardBarLink],
				corner.Hardpoints[CORNER::InboardBarLink]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for inboard U-bar (front)!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (localSuspension->RearBarStyle == SUSPENSION::SwayBarUBar && !isAtFront)
		{
			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::BarArmAtPivot],
				corner.Hardpoints[CORNER::OutboardBarLink], localSuspension->Hardpoints[SUSPENSION::RearBarMidPoint],
				originalCorner.Hardpoints[CORNER::BarArmAtPivot], originalCorner.Hardpoints[CORNER::OutboardBarLink],
				originalCar->Suspension->Hardpoints[SUSPENSION::RearBarMidPoint], originalCorner.Hardpoints[CORNER::InboardBarLink],
				corner.Hardpoints[CORNER::InboardBarLink]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for inboard U-bar (rear)!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (localSuspension->FrontBarStyle == SUSPENSION::SwayBarTBar && isAtFront)
		{
			debugger.Print(_T("  T-Bar Front"), Debugger::PriorityHigh);
			// FIXME:  Need to accommodate T-bars
			// Need to solve for InboardBarLink
			// Two of the three points needed are: BarMidPoint and OutboardBarLink
			// What is the third point?
			// Maybe the solution is actually the intersection of two circles and a torus?
		}
		else if (localSuspension->RearBarStyle == SUSPENSION::SwayBarTBar && !isAtFront)
		{
			debugger.Print(_T("  T-Bar Rear"), Debugger::PriorityHigh);
			// FIXME:  Need to accomodate T-bars
		}
		else if ((localSuspension->FrontBarStyle == SUSPENSION::SwayBarGeared && isAtFront) ||
			(localSuspension->RearBarStyle == SUSPENSION::SwayBarGeared && !isAtFront))
		{
			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::BarArmAtPivot],
				corner.Hardpoints[CORNER::OutboardBarLink], corner.Hardpoints[CORNER::GearEndBarShaft],
				originalCorner.Hardpoints[CORNER::BarArmAtPivot], originalCorner.Hardpoints[CORNER::OutboardBarLink],
				originalCorner.Hardpoints[CORNER::GearEndBarShaft], originalCorner.Hardpoints[CORNER::InboardBarLink],
				corner.Hardpoints[CORNER::InboardBarLink]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for geared bar!"), Debugger::PriorityMedium);
				success = false;
			}
		}
	}
	else if (corner.ActuationType == CORNER::ActuationOutboard)
	{
		// Outboard spring/shock units  - no pushrod/bell crank

		if (corner.ActuationAttachment == CORNER::AttachmentLowerAArm)
		{
			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::LowerBallJoint],
				corner.Hardpoints[CORNER::LowerFrontTubMount], corner.Hardpoints[CORNER::LowerRearTubMount],
				originalCorner.Hardpoints[CORNER::LowerBallJoint], originalCorner.Hardpoints[CORNER::LowerFrontTubMount],
				originalCorner.Hardpoints[CORNER::LowerRearTubMount], originalCorner.Hardpoints[CORNER::OutboardSpring],
				corner.Hardpoints[CORNER::OutboardSpring]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
				success = false;
			}

			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::LowerBallJoint],
				corner.Hardpoints[CORNER::LowerFrontTubMount], corner.Hardpoints[CORNER::LowerRearTubMount],
				originalCorner.Hardpoints[CORNER::LowerBallJoint], originalCorner.Hardpoints[CORNER::LowerFrontTubMount],
				originalCorner.Hardpoints[CORNER::LowerRearTubMount], originalCorner.Hardpoints[CORNER::OutboardShock],
				corner.Hardpoints[CORNER::OutboardShock]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (corner.ActuationAttachment == CORNER::AttachmentUpperAArm)
		{
			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::UpperBallJoint],
				corner.Hardpoints[CORNER::UpperFrontTubMount], corner.Hardpoints[CORNER::UpperRearTubMount],
				originalCorner.Hardpoints[CORNER::UpperBallJoint], originalCorner.Hardpoints[CORNER::UpperFrontTubMount],
				originalCorner.Hardpoints[CORNER::UpperRearTubMount], originalCorner.Hardpoints[CORNER::OutboardSpring],
				corner.Hardpoints[CORNER::OutboardSpring]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
				success = false;
			}

			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::UpperBallJoint],
				corner.Hardpoints[CORNER::UpperFrontTubMount], corner.Hardpoints[CORNER::UpperRearTubMount],
				originalCorner.Hardpoints[CORNER::UpperBallJoint], originalCorner.Hardpoints[CORNER::UpperFrontTubMount],
				originalCorner.Hardpoints[CORNER::UpperRearTubMount], originalCorner.Hardpoints[CORNER::OutboardShock],
				corner.Hardpoints[CORNER::OutboardShock]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
				success = false;
			}
		}
		else if (corner.ActuationAttachment == CORNER::AttachmentUpright)
		{
			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::UpperBallJoint],
				corner.Hardpoints[CORNER::LowerBallJoint], corner.Hardpoints[CORNER::OutboardTieRod],
				originalCorner.Hardpoints[CORNER::UpperBallJoint], originalCorner.Hardpoints[CORNER::LowerBallJoint],
				originalCorner.Hardpoints[CORNER::OutboardTieRod], originalCorner.Hardpoints[CORNER::OutboardSpring],
				corner.Hardpoints[CORNER::OutboardSpring]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
				success = false;
			}

			if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::UpperBallJoint],
				corner.Hardpoints[CORNER::LowerBallJoint], corner.Hardpoints[CORNER::OutboardTieRod],
				originalCorner.Hardpoints[CORNER::UpperBallJoint], originalCorner.Hardpoints[CORNER::LowerBallJoint],
				originalCorner.Hardpoints[CORNER::OutboardTieRod], originalCorner.Hardpoints[CORNER::OutboardShock],
				corner.Hardpoints[CORNER::OutboardShock]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
				success = false;
			}
		}
	}

	// Half Shafts
	if ((originalCar->HasFrontHalfShafts() && isAtFront) ||
		(originalCar->HasRearHalfShafts() && !isAtFront))
	{
		if (!SUSPENSION::SolveForPoint(corner.Hardpoints[CORNER::LowerBallJoint],
			corner.Hardpoints[CORNER::UpperBallJoint], corner.Hardpoints[CORNER::OutboardTieRod],
			originalCorner.Hardpoints[CORNER::LowerBallJoint], originalCorner.Hardpoints[CORNER::UpperBallJoint],
			originalCorner.Hardpoints[CORNER::OutboardTieRod], originalCorner.Hardpoints[CORNER::OutboardHalfShaft],
			corner.Hardpoints[CORNER::OutboardHalfShaft]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for outboard half shaft!"), Debugger::PriorityMedium);
			success = false;
		}
	}

	return success;
}