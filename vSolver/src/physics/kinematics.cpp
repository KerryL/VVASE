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
//				  renamed class from KINEMATICS.  Also introduce OUTPUTS class and CORNER class
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
#include "vUtilities/wheelAetStructures.h"

//==========================================================================
// Class:			KINEMATICS
// Function:		KINEMATICS
//
// Description:		Constructor for KINEMATICS class.
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
KINEMATICS::KINEMATICS(const Debugger &_debugger) : debugger(_debugger)
{
	// Initialize the other pointers to zero
	OriginalCar = NULL;
	WorkingCar = NULL;
	Static = NULL;
}

//==========================================================================
// Class:			KINEMATICS
// Function:		~KINEMATICS
//
// Description:		Destructor for KINEMATICS class.
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
KINEMATICS::~KINEMATICS()
{
}

//==========================================================================
// Class:			KINEMATICS
// Function:		UpdateKinematics
//
// Description:		This updates the position of the car to meet the current
//					values of pitch, roll, heave, and steer.
//
// Input Arguments:
//		_OriginalCar	= const CAR* - for reference only
//		_WorkingCar		= CAR* to be changed (updated) by this function
//		Name			= wxString used to print messages about this car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KINEMATICS::UpdateKinematics(const CAR* _OriginalCar, CAR* _WorkingCar, wxString Name)
{
	// Start the timer for this update
	wxStopWatch Timer;
	Timer.Start();

	// Print the car's name to the debug window
	debugger.Print(_T("UpdateKinematics() for ") + Name, Debugger::PriorityMedium);

	// Get the original and working cars
	OriginalCar = _OriginalCar;// This one is for reference and won't be changed by this class
	WorkingCar = _WorkingCar;

	// Ensure exclusive access to the car objects
	wxMutexLocker OriginalLock(OriginalCar->GetMutex());
	wxMutexLocker WorkingLock(WorkingCar->GetMutex());
	
	// Copy the information in the original car to the working car.  This minimizes rounding
	// errors in the calculation of suspension points, and it also ensures that changes made
	// to other sub-systems are carried over into the working car.
	*WorkingCar = *OriginalCar;

	// Now we copy the pointer to the working car's suspension to the class member
	// This must be AFTER *WorkingCar = *OriginalCar, since this assignment is a deep copy
	// and the address of the suspension will change!
	Static = WorkingCar->Suspension;

	// Rotate the steering wheel
	Static->MoveSteeringRack(Inputs.RackTravel);

	// FIXME:  As it is right now, this section is not compatabile with monoshocks

	Vector Rotations;// (X = First, Y = Second, Z = Third)
	Vector::Axis SecondRotation;

	// Determine the order to perform the Euler rotations
	if (Inputs.FirstRotation == Vector::AxisX)// Do roll first, then pitch
	{
		SecondRotation = Vector::AxisY;
		Rotations.Set(Inputs.Roll, Inputs.Pitch, 0.0);
	}
	else if (Inputs.FirstRotation == Vector::AxisY)// Do pitch first, then roll
	{
		SecondRotation = Vector::AxisX;
		Rotations.Set(Inputs.Pitch, Inputs.Roll, 0.0);
	}
	else
	{
		debugger.Print(_T("ERROR (UpdateKinematics):  Unable to determine order of Euler rotations"));
		return;
	}

	// Solve the chassis-mounted points for left-right common items (3rd springs/shocks, sway bars)
	// Front sway bars
	// NOTE:  This is only for points that are NOT in the CORNER class
	switch (Static->FrontBarStyle)
	{
	case SUSPENSION::SwayBarUBar:
		// Rotations
		Static->Hardpoints[SUSPENSION::FrontBarMidPoint].Rotate(Inputs.CenterOfRotation, Rotations,
			Inputs.FirstRotation, SecondRotation);

		// Translations
		Static->Hardpoints[SUSPENSION::FrontBarMidPoint].z += Inputs.Heave;
		break;

	case SUSPENSION::SwayBarTBar:
	case SUSPENSION::SwayBarGeared:
	case SUSPENSION::SwayBarNone:
	default:
		// Nothing here
		break;
	}

	// Rear sway bars
	switch (Static->RearBarStyle)
	{
	case SUSPENSION::SwayBarUBar:
		// Rotations
		Static->Hardpoints[SUSPENSION::RearBarMidPoint].Rotate(Inputs.CenterOfRotation, Rotations,
			Inputs.FirstRotation, SecondRotation);

		// Translations
		Static->Hardpoints[SUSPENSION::RearBarMidPoint].z += Inputs.Heave;
		break;

	case SUSPENSION::SwayBarTBar:
	case SUSPENSION::SwayBarGeared:
	case SUSPENSION::SwayBarNone:
	default:
		// Nothing here
		break;
	}

	// Third springs
	if (Static->FrontHasThirdSpring)
	{
		// Rotations
		Static->Hardpoints[SUSPENSION::FrontThirdSpringInboard].Rotate(Inputs.CenterOfRotation, Rotations,
			Inputs.FirstRotation, SecondRotation);
		Static->Hardpoints[SUSPENSION::FrontThirdShockInboard].Rotate(Inputs.CenterOfRotation, Rotations,
			Inputs.FirstRotation, SecondRotation);

		// Translations
		Static->Hardpoints[SUSPENSION::FrontThirdSpringInboard].z += Inputs.Heave;
		Static->Hardpoints[SUSPENSION::FrontThirdShockInboard].z += Inputs.Heave;
	}

	if (Static->RearHasThirdSpring)
	{
		// Rotations
		Static->Hardpoints[SUSPENSION::RearThirdSpringInboard].Rotate(Inputs.CenterOfRotation, Rotations,
			Inputs.FirstRotation, SecondRotation);
		Static->Hardpoints[SUSPENSION::RearThirdShockInboard].Rotate(Inputs.CenterOfRotation, Rotations,
			Inputs.FirstRotation, SecondRotation);

		// Translations
		Static->Hardpoints[SUSPENSION::RearThirdSpringInboard].z += Inputs.Heave;
		Static->Hardpoints[SUSPENSION::RearThirdShockInboard].z += Inputs.Heave;
	}

	if (!SolveCorner(Static->RightFront, OriginalCar->Suspension->RightFront, Rotations, SecondRotation))
		debugger.Print(_T("ERROR:  Problem solving right front corner!  Increase debug level for more information."),
			Debugger::PriorityHigh);
	if (!SolveCorner(Static->LeftFront, OriginalCar->Suspension->LeftFront, Rotations, SecondRotation))
		debugger.Print(_T("ERROR:  Problem solving left front corner!  Increase debug level for more information."),
			Debugger::PriorityHigh);
	if (!SolveCorner(Static->RightRear, OriginalCar->Suspension->RightRear, Rotations, SecondRotation))
		debugger.Print(_T("ERROR:  Problem solving right rear corner!  Increase debug level for more information."),
			Debugger::PriorityHigh);
	if (!SolveCorner(Static->LeftRear, OriginalCar->Suspension->LeftRear, Rotations, SecondRotation))
		debugger.Print(_T("ERROR:  Problem solving left rear corner!  Increase debug level for more information."),
			Debugger::PriorityHigh);

	// Now the hardpoints have been moved and located - let's look at the outputs
	Outputs.Update(OriginalCar, Static);

	// Get the total elapsed time (includes solving for the outputs)
	long TotalTime = Timer.Time();

	// Print the time to the output pane
	debugger.Print(Debugger::PriorityLow, "Finished UpdateKinematcs() for %s in %0.3f sec",
		Name.c_str(), TotalTime / 1000.0);

	return;
}

//==========================================================================
// Class:			KINEMATICS
// Function:		SolveCorner
//
// Description:		This solves for the locations of all of the suspension
//					nodes that exist at each corner of the car.  This includes
//					Everything from the contact patch up to the shock and
//					spring end-points.
//
// Input Arguments:
//		OriginalCorner	= const CORNER*, the un-perturbed locations of the
//						  suspension hardpoints
//		Rotations		= const Vector* specifying the amount to rotate the chassis
//						  about each axis
//		SecondRotation	= const Vector::Axis* describing the second axis of rotation
//						  (the first is an input quantity)
//
// Output Arguments:
//		Corner	= CORNER*, the corner of the car we are manipulating
//
// Return Value:
//		bool, true for success, false for error(s)
//
//==========================================================================
bool KINEMATICS::SolveCorner(CORNER &Corner, const CORNER &OriginalCorner,
							 const Vector &Rotations, const Vector::Axis &SecondRotation)
{
	// Determine if this corner is at the front or the rear of the car
	bool IsAtFront = false;
	if (Corner.Location == CORNER::LocationRightFront || Corner.Location == CORNER::LocationLeftFront)
		IsAtFront = true;

	// First move all of the body-fixed points (rotations first)
	Corner.Hardpoints[CORNER::LowerFrontTubMount].Rotate(Inputs.CenterOfRotation,
		Rotations, Inputs.FirstRotation, SecondRotation);
	Corner.Hardpoints[CORNER::LowerRearTubMount].Rotate(Inputs.CenterOfRotation,
		Rotations, Inputs.FirstRotation, SecondRotation);
	Corner.Hardpoints[CORNER::UpperFrontTubMount].Rotate(Inputs.CenterOfRotation,
		Rotations, Inputs.FirstRotation, SecondRotation);
	Corner.Hardpoints[CORNER::UpperRearTubMount].Rotate(Inputs.CenterOfRotation,
		Rotations, Inputs.FirstRotation, SecondRotation);
	Corner.Hardpoints[CORNER::BarArmAtPivot].Rotate(Inputs.CenterOfRotation,
		Rotations, Inputs.FirstRotation, SecondRotation);
	Corner.Hardpoints[CORNER::InboardSpring].Rotate(Inputs.CenterOfRotation,
		Rotations, Inputs.FirstRotation, SecondRotation);
	Corner.Hardpoints[CORNER::InboardShock].Rotate(Inputs.CenterOfRotation,
		Rotations, Inputs.FirstRotation, SecondRotation);
	Corner.Hardpoints[CORNER::InboardTieRod].Rotate(Inputs.CenterOfRotation,
		Rotations, Inputs.FirstRotation, SecondRotation);

	// Now do the translations on the same points
	Corner.Hardpoints[CORNER::LowerFrontTubMount].z += Inputs.Heave;
	Corner.Hardpoints[CORNER::LowerRearTubMount].z += Inputs.Heave;
	Corner.Hardpoints[CORNER::UpperFrontTubMount].z += Inputs.Heave;
	Corner.Hardpoints[CORNER::UpperRearTubMount].z += Inputs.Heave;
	Corner.Hardpoints[CORNER::BarArmAtPivot].z += Inputs.Heave;
	Corner.Hardpoints[CORNER::InboardSpring].z += Inputs.Heave;
	Corner.Hardpoints[CORNER::InboardShock].z += Inputs.Heave;
	Corner.Hardpoints[CORNER::InboardTieRod].z += Inputs.Heave;

	// Depending on the type of actuation, we might have to move some additional points, as well
	if (Corner.ActuationType == CORNER::ActuationPushPullrod)
	{
		Corner.Hardpoints[CORNER::BellCrankPivot1].Rotate(Inputs.CenterOfRotation,
			Rotations, Inputs.FirstRotation, SecondRotation);
		Corner.Hardpoints[CORNER::BellCrankPivot2].Rotate(Inputs.CenterOfRotation,
			Rotations, Inputs.FirstRotation, SecondRotation);

		Corner.Hardpoints[CORNER::BellCrankPivot1].z += Inputs.Heave;
		Corner.Hardpoints[CORNER::BellCrankPivot2].z += Inputs.Heave;
	}

	// Do the rotation and translation for the inboard half shafts at the same time
	if ((OriginalCar->HasFrontHalfShafts() && IsAtFront) ||
		(OriginalCar->HasRearHalfShafts() && !IsAtFront))
	{
		Corner.Hardpoints[CORNER::InboardHalfShaft].Rotate(Inputs.CenterOfRotation,
			Rotations, Inputs.FirstRotation, SecondRotation);
		Corner.Hardpoints[CORNER::InboardHalfShaft].z += Inputs.Heave;
	}

	// Declare and initialize the return value to true
	bool Success = true;

	// Solve outboard points and work in through the pushrods and bell cranks.
	// We will iterate the z-location of the lower ball joint and the solution of
	// the first three outboard points until the bottom of the tire is at z = 0.
	Vector WheelNormal;
	Vector OriginalPlaneNormal;
	Vector NewPlaneNormal;
	Vector WheelRotations;
	double UpperLimit(0.0), LowerLimit(0.0);// Initialized here to avoid MSVC++ compiler warning C4701
	int Iteration = 1;
	double Tolerance = 5e-8;
	int Limit = 100;

	Corner.Hardpoints[CORNER::ContactPatch].z = 1.0;// Must be initialized to > Tolerance
	while (Iteration <= Limit && fabs(Corner.Hardpoints[CORNER::ContactPatch].z) > Tolerance)
	{
		if (!SUSPENSION::SolveForXY(Corner.Hardpoints[CORNER::LowerFrontTubMount],
			Corner.Hardpoints[CORNER::LowerRearTubMount], OriginalCorner.Hardpoints[CORNER::LowerFrontTubMount],
			OriginalCorner.Hardpoints[CORNER::LowerRearTubMount], OriginalCorner.Hardpoints[CORNER::LowerBallJoint],
			Corner.Hardpoints[CORNER::LowerBallJoint]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for lower ball joint!"), Debugger::PriorityMedium);
			Success = false;
		}

		if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::LowerBallJoint],
			Corner.Hardpoints[CORNER::UpperFrontTubMount], Corner.Hardpoints[CORNER::UpperRearTubMount],
			OriginalCorner.Hardpoints[CORNER::LowerBallJoint], OriginalCorner.Hardpoints[CORNER::UpperFrontTubMount],
			OriginalCorner.Hardpoints[CORNER::UpperRearTubMount], OriginalCorner.Hardpoints[CORNER::UpperBallJoint],
			Corner.Hardpoints[CORNER::UpperBallJoint]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for upper ball joint!"), Debugger::PriorityMedium);
			Success = false;
		}

		if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::LowerBallJoint],
			Corner.Hardpoints[CORNER::UpperBallJoint], Corner.Hardpoints[CORNER::InboardTieRod],
			OriginalCorner.Hardpoints[CORNER::LowerBallJoint], OriginalCorner.Hardpoints[CORNER::UpperBallJoint],
			OriginalCorner.Hardpoints[CORNER::InboardTieRod], OriginalCorner.Hardpoints[CORNER::OutboardTieRod],
			Corner.Hardpoints[CORNER::OutboardTieRod]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for outboard tie rod!"), Debugger::PriorityMedium);
			Success = false;
		}

		if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::LowerBallJoint],
			Corner.Hardpoints[CORNER::UpperBallJoint], Corner.Hardpoints[CORNER::OutboardTieRod],
			OriginalCorner.Hardpoints[CORNER::LowerBallJoint], OriginalCorner.Hardpoints[CORNER::UpperBallJoint],
			OriginalCorner.Hardpoints[CORNER::OutboardTieRod], OriginalCorner.Hardpoints[CORNER::WheelCenter],
			Corner.Hardpoints[CORNER::WheelCenter]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for wheel center!"), Debugger::PriorityMedium);
			Success = false;
		}

		// Solve for the contact patch
		OriginalPlaneNormal = VVASEMath::GetPlaneNormal(OriginalCorner.Hardpoints[CORNER::LowerBallJoint],
			OriginalCorner.Hardpoints[CORNER::UpperBallJoint], OriginalCorner.Hardpoints[CORNER::OutboardTieRod]);
		NewPlaneNormal = VVASEMath::GetPlaneNormal(Corner.Hardpoints[CORNER::LowerBallJoint],
			Corner.Hardpoints[CORNER::UpperBallJoint], Corner.Hardpoints[CORNER::OutboardTieRod]);

		// For some operations, if this is on the right-hand side of the car, the sign gets flipped
		double Sign = 1.0;
		if (Corner.Location == CORNER::LocationRightFront || Corner.Location == CORNER::LocationRightRear)
			Sign *= -1.0;

		// Determine what Euler angles I need to rotate OriginalPlaneNormal through to
		// get NewPlaneNormal.  Do one angle at a time because order of rotations matters.
		WheelRotations = OriginalPlaneNormal.AnglesTo(NewPlaneNormal);
		WheelNormal.Set(0.0, Sign * 1.0, 0.0);
		WheelNormal.Rotate(WheelRotations.z, Vector::AxisZ);
		WheelNormal.Rotate(WheelRotations.x, Vector::AxisX);

		// Add in the effects of camber and toe settings (contact patch location should not be affected
		// by camber and toe settings - WheelCenter is calculated based on camber and toe, so we need to
		// back that out in the contact patch solver to avoid making contact patch a function of camber
		// and toe).
		WheelNormal.Rotate(Sign * OriginalCorner.StaticCamber, Vector::AxisX);
		WheelNormal.Rotate(Sign * OriginalCorner.StaticToe, Vector::AxisZ);

		// Do the actual solving for the contact patch
		SUSPENSION::SolveForContactPatch(Corner.Hardpoints[CORNER::WheelCenter], WheelNormal,
			OriginalCorner.Hardpoints[CORNER::ContactPatch].Distance(OriginalCorner.Hardpoints[CORNER::WheelCenter]),
			Corner.Hardpoints[CORNER::ContactPatch]);

		// With the origin on the ground, the error is equal to the Z location of the contact patch
		// FIXME:  This is only true when using flat ground model!!!
		// Use a line search algorithm to adjust the height of the lower ball joint
		// This could get hung up if the relationship between lower ball joint height and contact
		// patch Z location is highly non-linear, but that would be an unusual case (extreme camber gain?)

		// If this is the first iteration, initialize the limit variables
		if (Iteration == 1)
		{
			UpperLimit = Corner.Hardpoints[CORNER::LowerBallJoint].z +
				3.0 * fabs(Corner.Hardpoints[CORNER::ContactPatch].z);
			LowerLimit = Corner.Hardpoints[CORNER::LowerBallJoint].z -
				3.0 * fabs(Corner.Hardpoints[CORNER::ContactPatch].z);
		}

		// Make the adjustment in the guess
		if (Corner.Hardpoints[CORNER::ContactPatch].z > Tolerance)
			UpperLimit = Corner.Hardpoints[CORNER::LowerBallJoint].z;
		else if (Corner.Hardpoints[CORNER::ContactPatch].z < -Tolerance)
			LowerLimit = Corner.Hardpoints[CORNER::LowerBallJoint].z;
		Corner.Hardpoints[CORNER::LowerBallJoint].z = LowerLimit + (UpperLimit - LowerLimit) / 2.0;

		// Increment the iteration counter
		Iteration++;
	}

	// Check to make sure we finished the loop because we were within the tolerance (and not
	// because we hit the iteration limit)
	if ((fabs(Corner.Hardpoints[CORNER::ContactPatch].z) > Tolerance))
	{
		// Print an error and switch the success boolean to indicate a failure
		debugger.Print(_T("Warning (SolveCorner):  Contact patch location did not converge"),
			Debugger::PriorityMedium);
		Success = false;
	}

	// Outboard shock/spring actuators
	if (Corner.ActuationType == CORNER::ActuationPushPullrod)
	{
		if (Corner.ActuationAttachment == CORNER::AttachmentLowerAArm)
		{
			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::LowerBallJoint],
				Corner.Hardpoints[CORNER::LowerFrontTubMount], Corner.Hardpoints[CORNER::LowerRearTubMount],
				OriginalCorner.Hardpoints[CORNER::LowerBallJoint], OriginalCorner.Hardpoints[CORNER::LowerFrontTubMount],
				OriginalCorner.Hardpoints[CORNER::LowerRearTubMount], OriginalCorner.Hardpoints[CORNER::OutboardPushrod],
				Corner.Hardpoints[CORNER::OutboardPushrod]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard pushrod!"), Debugger::PriorityMedium);
				Success = false;
			}
		}
		else if (Corner.ActuationAttachment == CORNER::AttachmentUpperAArm)
		{
			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::UpperBallJoint],
				Corner.Hardpoints[CORNER::UpperFrontTubMount], Corner.Hardpoints[CORNER::UpperRearTubMount],
				OriginalCorner.Hardpoints[CORNER::UpperBallJoint], OriginalCorner.Hardpoints[CORNER::UpperFrontTubMount],
				OriginalCorner.Hardpoints[CORNER::UpperRearTubMount], OriginalCorner.Hardpoints[CORNER::OutboardPushrod],
				Corner.Hardpoints[CORNER::OutboardPushrod]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard pullrod!"), Debugger::PriorityMedium);
				Success = false;
			}
		}
		else if (Corner.ActuationAttachment == CORNER::AttachmentUpright)
		{
			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::UpperBallJoint],
				Corner.Hardpoints[CORNER::LowerBallJoint], Corner.Hardpoints[CORNER::OutboardTieRod],
				OriginalCorner.Hardpoints[CORNER::UpperBallJoint], OriginalCorner.Hardpoints[CORNER::LowerBallJoint],
				OriginalCorner.Hardpoints[CORNER::OutboardTieRod], OriginalCorner.Hardpoints[CORNER::OutboardPushrod],
				Corner.Hardpoints[CORNER::OutboardPushrod]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard push/pullrod!"), Debugger::PriorityMedium);
				Success = false;
			}
		}

		// Inboard Pushrods
		if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::BellCrankPivot1],
			Corner.Hardpoints[CORNER::BellCrankPivot2], Corner.Hardpoints[CORNER::OutboardPushrod],
			OriginalCorner.Hardpoints[CORNER::BellCrankPivot1], OriginalCorner.Hardpoints[CORNER::BellCrankPivot2],
			OriginalCorner.Hardpoints[CORNER::OutboardPushrod], OriginalCorner.Hardpoints[CORNER::InboardPushrod],
			Corner.Hardpoints[CORNER::InboardPushrod]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for inboard push/pullrod!"), Debugger::PriorityMedium);
			Success = false;
		}

		// Outboard Shocks
		if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::BellCrankPivot1],
			Corner.Hardpoints[CORNER::BellCrankPivot2], Corner.Hardpoints[CORNER::InboardPushrod],
			OriginalCorner.Hardpoints[CORNER::BellCrankPivot1], OriginalCorner.Hardpoints[CORNER::BellCrankPivot2],
			OriginalCorner.Hardpoints[CORNER::InboardPushrod], OriginalCorner.Hardpoints[CORNER::OutboardShock],
			Corner.Hardpoints[CORNER::OutboardShock]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
			Success = false;
		}

		// Outboard Springs
		if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::BellCrankPivot1],
			Corner.Hardpoints[CORNER::BellCrankPivot2], Corner.Hardpoints[CORNER::InboardPushrod],
			OriginalCorner.Hardpoints[CORNER::BellCrankPivot1], OriginalCorner.Hardpoints[CORNER::BellCrankPivot2],
			OriginalCorner.Hardpoints[CORNER::InboardPushrod], OriginalCorner.Hardpoints[CORNER::OutboardSpring],
			Corner.Hardpoints[CORNER::OutboardSpring]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
			Success = false;
		}

		// Sway Bars Outboard
		if ((Static->FrontBarStyle != SUSPENSION::SwayBarNone && IsAtFront) ||
			(Static->RearBarStyle != SUSPENSION::SwayBarNone && !IsAtFront))
		{
			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::BellCrankPivot1],
				Corner.Hardpoints[CORNER::BellCrankPivot2], Corner.Hardpoints[CORNER::InboardPushrod],
				OriginalCorner.Hardpoints[CORNER::BellCrankPivot1], OriginalCorner.Hardpoints[CORNER::BellCrankPivot2],
				OriginalCorner.Hardpoints[CORNER::InboardPushrod], OriginalCorner.Hardpoints[CORNER::OutboardBarLink],
				Corner.Hardpoints[CORNER::OutboardBarLink]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard swaybar!"), Debugger::PriorityMedium);
				Success = false;
			}
		}

		// Sway Bars Inboard
		if (Static->FrontBarStyle == SUSPENSION::SwayBarUBar && IsAtFront)
		{
			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::BarArmAtPivot],
				Corner.Hardpoints[CORNER::OutboardBarLink], Static->Hardpoints[SUSPENSION::FrontBarMidPoint],// FIXME:  Do we really need the midpoint?
				OriginalCorner.Hardpoints[CORNER::BarArmAtPivot], OriginalCorner.Hardpoints[CORNER::OutboardBarLink],
				OriginalCar->Suspension->Hardpoints[SUSPENSION::FrontBarMidPoint], OriginalCorner.Hardpoints[CORNER::InboardBarLink],
				Corner.Hardpoints[CORNER::InboardBarLink]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for inboard U-bar (front)!"), Debugger::PriorityMedium);
				Success = false;
			}
		}
		else if (Static->RearBarStyle == SUSPENSION::SwayBarUBar && !IsAtFront)
		{
			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::BarArmAtPivot],
				Corner.Hardpoints[CORNER::OutboardBarLink], Static->Hardpoints[SUSPENSION::RearBarMidPoint],
				OriginalCorner.Hardpoints[CORNER::BarArmAtPivot], OriginalCorner.Hardpoints[CORNER::OutboardBarLink],
				OriginalCar->Suspension->Hardpoints[SUSPENSION::RearBarMidPoint], OriginalCorner.Hardpoints[CORNER::InboardBarLink],
				Corner.Hardpoints[CORNER::InboardBarLink]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for inboard U-bar (rear)!"), Debugger::PriorityMedium);
				Success = false;
			}
		}
		else if (Static->FrontBarStyle == SUSPENSION::SwayBarTBar && IsAtFront)
		{
			debugger.Print(_T("  T-Bar Front"), Debugger::PriorityHigh);
			// FIXME:  Need to accomodate T-bars
			// Need to solve for InboardBarLink
			// Two of the three points needed are: BarMidPoint and OutboardBarLink
			// What is the third point?
			// Maybe the solution is actually the intersection of two circles and a torus?
		}
		else if (Static->RearBarStyle == SUSPENSION::SwayBarTBar && !IsAtFront)
		{
			debugger.Print(_T("  T-Bar Rear"), Debugger::PriorityHigh);
			// FIXME:  Need to accomodate T-bars
		}
		else if ((Static->FrontBarStyle == SUSPENSION::SwayBarGeared && IsAtFront) ||
			(Static->RearBarStyle == SUSPENSION::SwayBarGeared && !IsAtFront))
		{
			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::BarArmAtPivot],
				Corner.Hardpoints[CORNER::OutboardBarLink], Corner.Hardpoints[CORNER::GearEndBarShaft],
				OriginalCorner.Hardpoints[CORNER::BarArmAtPivot], OriginalCorner.Hardpoints[CORNER::OutboardBarLink],
				OriginalCorner.Hardpoints[CORNER::GearEndBarShaft], OriginalCorner.Hardpoints[CORNER::InboardBarLink],
				Corner.Hardpoints[CORNER::InboardBarLink]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for geared bar!"), Debugger::PriorityMedium);
				Success = false;
			}
		}
	}
	else if (Corner.ActuationType == CORNER::ActuationOutboard)
	{
		// Outboard spring/shock units  - no pushrod/bell crank

		if (Corner.ActuationAttachment == CORNER::AttachmentLowerAArm)
		{
			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::LowerBallJoint],
				Corner.Hardpoints[CORNER::LowerFrontTubMount], Corner.Hardpoints[CORNER::LowerRearTubMount],
				OriginalCorner.Hardpoints[CORNER::LowerBallJoint], OriginalCorner.Hardpoints[CORNER::LowerFrontTubMount],
				OriginalCorner.Hardpoints[CORNER::LowerRearTubMount], OriginalCorner.Hardpoints[CORNER::OutboardSpring],
				Corner.Hardpoints[CORNER::OutboardSpring]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
				Success = false;
			}

			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::LowerBallJoint],
				Corner.Hardpoints[CORNER::LowerFrontTubMount], Corner.Hardpoints[CORNER::LowerRearTubMount],
				OriginalCorner.Hardpoints[CORNER::LowerBallJoint], OriginalCorner.Hardpoints[CORNER::LowerFrontTubMount],
				OriginalCorner.Hardpoints[CORNER::LowerRearTubMount], OriginalCorner.Hardpoints[CORNER::OutboardShock],
				Corner.Hardpoints[CORNER::OutboardShock]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
				Success = false;
			}
		}
		else if (Corner.ActuationAttachment == CORNER::AttachmentUpperAArm)
		{
			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::UpperBallJoint],
				Corner.Hardpoints[CORNER::UpperFrontTubMount], Corner.Hardpoints[CORNER::UpperRearTubMount],
				OriginalCorner.Hardpoints[CORNER::UpperBallJoint], OriginalCorner.Hardpoints[CORNER::UpperFrontTubMount],
				OriginalCorner.Hardpoints[CORNER::UpperRearTubMount], OriginalCorner.Hardpoints[CORNER::OutboardSpring],
				Corner.Hardpoints[CORNER::OutboardSpring]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
				Success = false;
			}

			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::UpperBallJoint],
				Corner.Hardpoints[CORNER::UpperFrontTubMount], Corner.Hardpoints[CORNER::UpperRearTubMount],
				OriginalCorner.Hardpoints[CORNER::UpperBallJoint], OriginalCorner.Hardpoints[CORNER::UpperFrontTubMount],
				OriginalCorner.Hardpoints[CORNER::UpperRearTubMount], OriginalCorner.Hardpoints[CORNER::OutboardShock],
				Corner.Hardpoints[CORNER::OutboardShock]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
				Success = false;
			}
		}
		else if (Corner.ActuationAttachment == CORNER::AttachmentUpright)
		{
			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::UpperBallJoint],
				Corner.Hardpoints[CORNER::LowerBallJoint], Corner.Hardpoints[CORNER::OutboardTieRod],
				OriginalCorner.Hardpoints[CORNER::UpperBallJoint], OriginalCorner.Hardpoints[CORNER::LowerBallJoint],
				OriginalCorner.Hardpoints[CORNER::OutboardTieRod], OriginalCorner.Hardpoints[CORNER::OutboardSpring],
				Corner.Hardpoints[CORNER::OutboardSpring]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard spring!"), Debugger::PriorityMedium);
				Success = false;
			}

			if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::UpperBallJoint],
				Corner.Hardpoints[CORNER::LowerBallJoint], Corner.Hardpoints[CORNER::OutboardTieRod],
				OriginalCorner.Hardpoints[CORNER::UpperBallJoint], OriginalCorner.Hardpoints[CORNER::LowerBallJoint],
				OriginalCorner.Hardpoints[CORNER::OutboardTieRod], OriginalCorner.Hardpoints[CORNER::OutboardShock],
				Corner.Hardpoints[CORNER::OutboardShock]))
			{
				// Print an error and switch the success boolean to indicate a failure
				debugger.Print(_T("ERROR:  Failed to solve for outboard shock!"), Debugger::PriorityMedium);
				Success = false;
			}
		}
	}

	// Half Shafts
	if ((OriginalCar->HasFrontHalfShafts() && IsAtFront) ||
		(OriginalCar->HasRearHalfShafts() && !IsAtFront))
	{
		if (!SUSPENSION::SolveForPoint(Corner.Hardpoints[CORNER::LowerBallJoint],
			Corner.Hardpoints[CORNER::UpperBallJoint], Corner.Hardpoints[CORNER::OutboardTieRod],
			OriginalCorner.Hardpoints[CORNER::LowerBallJoint], OriginalCorner.Hardpoints[CORNER::UpperBallJoint],
			OriginalCorner.Hardpoints[CORNER::OutboardTieRod], OriginalCorner.Hardpoints[CORNER::OutboardHalfShaft],
			Corner.Hardpoints[CORNER::OutboardHalfShaft]))
		{
			// Print an error and switch the success boolean to indicate a failure
			debugger.Print(_T("ERROR:  Failed to solve for outboard half shaft!"), Debugger::PriorityMedium);
			Success = false;
		}
	}

	return Success;
}