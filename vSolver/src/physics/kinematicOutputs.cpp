/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  kinematicOutputs.cpp
// Created:  3/23/2008
// Author:  K. Loux
// Description:  Contains class definition for outputs class.  This class does the calculations
//				 for all of the kinematic simulation outputs.  That includes any kind of wheel
//				 angle/orientation, chassis attitude, spring/shock positions, but doesn't include
//				 any thing that requires forces to calculate (force-based roll center, etc.).
// History:
//	3/24/2008	- Created CORNER_OUTPUTS structure, K. Loux.
//	2/16/2009	- Changed CORNER and SUSPENSION to use enumeration style array of points instead
//				  of having each point declared individually, K. Loux.
//	2/26/2009	- Fixed calculation in steer and camber angles.  Also added effects of static camber
//				  and toe settings, K. Loux.
//	3/11/2009	- Finished implementation of enum/array style data members, K. Loux.
//	3/15/2009	- Changed the way kinematic centers are calculated to the "intersection of two planes"
//				  method, K. Loux.
//	4/19/2009	- Added threading for UpdateCorner(), K. Loux.
//	4/21/2009	- Removed threading (it's slower), K. Loux.
//	11/1/2010	- Revised and completed installation ratio calculations, K. Loux.

// wxWidgets headers
#include <wx/wx.h>

// CarDesigner headers
#include "vCar/car.h"
#include "vCar/brakes.h"
#include "vCar/drivetrain.h"
#include "vCar/mass.h"
#include "vCar/suspension.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vMath/carMath.h"
#include "vUtilities/wheelSetStructures.h"
#include "vUtilities/debugger.h"

//==========================================================================
// Class:			KinematicOutputs
// Function:		KinematicOutputs
//
// Description:		Constructor for KinematicOutputs class.
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
KinematicOutputs::KinematicOutputs()
{
	InitializeAllOutputs();
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		~KinematicOutputs
//
// Description:		Destructor for KinematicOutputs class.
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
KinematicOutputs::~KinematicOutputs()
{
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		Constant Declarations
//
// Description:		Constant declarations for KinematicOutputs class.
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
const Debugger *KinematicOutputs::debugger = NULL;

//==========================================================================
// Class:			KinematicOutputs
// Function:		InitializeAllOutputs
//
// Description:		Initializes all outputs to QNAN.
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
void KinematicOutputs::InitializeAllOutputs(void)
{
	// Initialize all outputs to QNAN
	int i;

	// Corner Doubles
	for (i = 0; i < NumberOfCornerOutputDoubles; i++)
	{
		rightFront[i] = VVASEMath::QNAN;
		leftFront[i] = VVASEMath::QNAN;
		rightRear[i] = VVASEMath::QNAN;
		leftRear[i] = VVASEMath::QNAN;
	}

	// Corner Vectors
	for (i = 0; i< NumberOfCornerOutputVectors; i++)
	{
		rightFrontVectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
		leftFrontVectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
		rightRearVectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
		leftRearVectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
	}

	// Whole-car Doubles
	for (i = 0; i < NumberOfOutputDoubles; i++)
		OutputsDouble[i] = VVASEMath::QNAN;

	// Whole-car Vectors
	for (i = 0; i < NumberOfOutputVectors; i++)
		vectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		Update
//
// Description:		Takes all of the virtual measurements and saves them in
//					Outputs.  Every member of Outputs is included here.
//					Measurements are take on Current.
//
// Input Arguments:
//		original	= const CAR*, pointing to object describing the car's
//					  un-perturbed state
//		current		= const SUSPENSION*, pointing to object describing the new
//					  state of the car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::Update(const CAR *original, const SUSPENSION *current)
{
	// Copy the car pointer to our class variable
	currentCar = original;

	// Re-initialize all outputs before doing the computations
	InitializeAllOutputs();

	// Update each corner's outputs
	UpdateCorner(&original->Suspension->RightFront, &current->RightFront);
	UpdateCorner(&original->Suspension->LeftFront, &current->LeftFront);
	UpdateCorner(&original->Suspension->RightRear, &current->RightRear);
	UpdateCorner(&original->Suspension->LeftRear, &current->LeftRear);

	// Net Steer [rad]
	OutputsDouble[FrontNetSteer] = rightFront[Steer] - leftFront[Steer];
	OutputsDouble[RearNetSteer] = rightRear[Steer] - leftRear[Steer];

	// Net Scrub [in]
	OutputsDouble[FrontNetScrub] = rightFront[Scrub] + leftFront[Scrub];
	OutputsDouble[RearNetScrub] = rightRear[Scrub] + leftRear[Scrub];

	// ARB Twist [rad]
	// Initialize the twist in case the car has no sway bars
	OutputsDouble[FrontARBTwist] = 0.0;
	OutputsDouble[RearARBTwist] = 0.0;
	Vector arm1Direction, arm2Direction;
	Vector swayBarAxis;
	if (current->FrontBarStyle == SUSPENSION::SwayBarUBar)
	{
		// Project these directions onto the plane whose normal is the sway bar axis
		swayBarAxis = current->RightFront.Hardpoints[CORNER::BarArmAtPivot] -
			current->LeftFront.Hardpoints[CORNER::BarArmAtPivot];

		// The references for U-bar twist are the arms at the end of the bar
		arm1Direction = VVASEMath::ProjectOntoPlane(current->RightFront.Hardpoints[CORNER::BarArmAtPivot] -
			current->RightFront.Hardpoints[CORNER::InboardBarLink], SwayBarAxis);
		arm2Direction = VVASEMath::ProjectOntoPlane(current->LeftFront.Hardpoints[CORNER::BarArmAtPivot] -
			current->LeftFront.Hardpoints[CORNER::InboardBarLink], SwayBarAxis);

		// The angle between these vectors, when projected onto the plane that is normal
		// to the swaybar axis is given by the dot product
		OutputsDouble[FrontARBTwist] = acos((arm1Direction * arm2Direction) /
			(arm1Direction.Length() * arm2Direction.Length()));
	}
	else if (current->FrontBarStyle == SUSPENSION::SwayBarTBar)
	{
		// FIXME!!!
	}
	else if (current->FrontBarStyle == SUSPENSION::SwayBarGeared)
	{
		// FIXME!!!
	}

	if (current->RearBarStyle == SUSPENSION::SwayBarUBar)
	{
		// Project these directions onto the plane whose normal is the swaybar axis
		swayBarAxis = current->RightRear.Hardpoints[CORNER::BarArmAtPivot] -
			current->LeftRear.Hardpoints[CORNER::BarArmAtPivot];

		// The references for U-bar twist are the arms at the end of the bar
		arm1Direction = VVASEMath::ProjectOntoPlane(current->RightRear.Hardpoints[CORNER::BarArmAtPivot] -
			current->RightRear.Hardpoints[CORNER::InboardBarLink], swayBarAxis);
		arm2Direction = VVASEMath::ProjectOntoPlane(current->LeftRear.Hardpoints[CORNER::BarArmAtPivot] -
			current->LeftRear.Hardpoints[CORNER::InboardBarLink], swayBarAxis);

		// The angle between these vectors, when projected onto the plane that is normal
		// to the swaybar axis is given by the dot product
		OutputsDouble[RearARBTwist] = acos((arm1Direction * arm2Direction) /
			(arm1Direction.Length() * arm2Direction.Length()));
	}
	else if (current->RearBarStyle == SUSPENSION::SwayBarTBar)
	{
		// FIXME!!!
	}
	else if (current->RearBarStyle == SUSPENSION::SwayBarGeared)
	{
		// FIXME!!!
	}

	// Kinematic Roll Centers and Direction Vectors [in], [-]
	//  Wm. C. Mitchell makes clear the assumptions that are made when calculating kinematic
	//  roll centers in his SAE paper "Asymmetric Roll Centers" (983085).  My interpretation
	//  of these is this:  Kinematic roll centers assume that your tires are pinned to the
	//  ground.  In other words, it ignores the lateral forces created by the tires on the
	//  pavement.  It is the point around which the car would roll if your tires couldn't
	//  move instantaneously.  The advantage to force based roll centers is that you don't
	//  need to assume that cornering forces are being generated equally on the right and
	//  left sides of the car.
	//  The roll center (in 3D) is defined as the intersection of the line from the tire to
	//  the instant center when projected onto the plane that is normal to the ground plane
	//  and contains both wheel centers.  To find this point, we find the "instant planes"
	//  for both side of the car (plane containing the instant axis and the contact patch
	//  point) and intersect them.  This give us an axis, and we find the intersection of
	//  this axis with the appropriate plane to find the actual kinematic center point.
	Vector rightPlaneNormal;
	Vector leftPlaneNormal;
	Vector planeNormal(1.0, 0.0, 0.0);// For projecting the vectors to find the kinematic centers

	// Front
	// Find the normal vectors
	rightPlaneNormal = VVASEMath::GetPlaneNormal(current->RightFront.Hardpoints[CORNER::ContactPatch],
		rightFrontVectors[InstantCenter], rightFrontVectors[InstantCenter] + rightFrontVectors[InstantAxisDirection]);
	leftPlaneNormal = VVASEMath::GetPlaneNormal(current->LeftFront.Hardpoints[CORNER::ContactPatch],
		leftFrontVectors[InstantCenter], leftFrontVectors[InstantCenter] + leftFrontVectors[InstantAxisDirection]);

	// Get the intersection of the planes
	if (!VVASEMath::GetIntersectionOfTwoPlanes(rightPlaneNormal, current->RightFront.Hardpoints[CORNER::ContactPatch],
		leftPlaneNormal, current->LeftFront.Hardpoints[CORNER::ContactPatch],
		vectors[FrontRollAxisDirection], vectors[FrontKinematicRC]))
		debugger->Print(_T("Warning (KinematicOutputs::Update):  Front Kinematic Roll Center is undefined"),
			Debugger::PriorityHigh);
	else
		// We now have the axis direction and a point on the axis, but we want a specific
		// point on the axis.  To do that, we determine the place where this vector passes through
		// the appropriate plane.
		vectors[FrontKinematicRC] = VVASEMath::IntersectWithPlane(planeNormal,
			(current->RightFront.Hardpoints[CORNER::WheelCenter] +
			current->LeftFront.Hardpoints[CORNER::WheelCenter]) / 2.0,
			vectors[FrontRollAxisDirection], vectors[FrontKinematicRC]);

	// Rear
	// Find the normal vectors
	rightPlaneNormal = VVASEMath::GetPlaneNormal(current->RightRear.Hardpoints[CORNER::ContactPatch],
		rightRearVectors[InstantCenter], rightRearVectors[InstantCenter] + rightRearVectors[InstantAxisDirection]);
	leftPlaneNormal = VVASEMath::GetPlaneNormal(current->LeftRear.Hardpoints[CORNER::ContactPatch],
		leftRearVectors[InstantCenter], leftRearVectors[InstantCenter] + leftRearVectors[InstantAxisDirection]);

	// Get the intersection of the planes
	if (!VVASEMath::GetIntersectionOfTwoPlanes(rightPlaneNormal, current->RightRear.Hardpoints[CORNER::ContactPatch],
		leftPlaneNormal, current->LeftRear.Hardpoints[CORNER::ContactPatch],
		vectors[RearRollAxisDirection], vectors[RearKinematicRC]))
		debugger->Print(_T("Warning (KinematicOutputs::Update):  Rear Kinematic Roll Center is undefined"),
			Debugger::PriorityHigh);
	else
		// Just like we did on for the front, intersect this vector with the wheel plane
		vectors[RearKinematicRC] = VVASEMath::IntersectWithPlane(planeNormal,
			(current->RightRear.Hardpoints[CORNER::WheelCenter] +
			current->LeftRear.Hardpoints[CORNER::WheelCenter]) / 2.0,
			vectors[RearRollAxisDirection], vectors[RearKinematicRC]);

	// Kinematic Pitch Centers and Directions [in], [-]
	// All of the same assumptions that we have for roll centers apply here.
	// The method is also the same as the roll center calculations.
	Vector frontPlaneNormal;
	Vector rearPlaneNormal;
	planeNormal.Set(0.0, 1.0, 0.0);// For projecting the vectors to find the kinematic centers

	// Right
	// Find the normal vectors
	frontPlaneNormal = VVASEMath::GetPlaneNormal(current->RightFront.Hardpoints[CORNER::ContactPatch],
		rightFrontVectors[InstantCenter], rightFrontVectors[InstantCenter] + rightFrontVectors[InstantAxisDirection]);
	rearPlaneNormal = VVASEMath::GetPlaneNormal(current->RightRear.Hardpoints[CORNER::ContactPatch],
		rightRearVectors[InstantCenter], rightRearVectors[InstantCenter] + rightRearVectors[InstantAxisDirection]);

	// Get the intersection of the planes
	if (!VVASEMath::GetIntersectionOfTwoPlanes(frontPlaneNormal, current->RightFront.Hardpoints[CORNER::ContactPatch],
		rearPlaneNormal, current->RightRear.Hardpoints[CORNER::ContactPatch],
		vectors[RightPitchAxisDirection], vectors[RightKinematicPC]))
		debugger->Print(_T("Warning (KinematicOutputs::Update):  Right Kinematic Pitch Center is undefined"),
			Debugger::PriorityHigh);
	else
		// We now have the axis direction and a point on the axis, but we want a specific
		// point on the axis.  To do that, we determine the place where this vector passes through
		// the appropriate plane.
		vectors[RightKinematicPC] = VVASEMath::IntersectWithPlane(planeNormal,
			(current->RightFront.Hardpoints[CORNER::WheelCenter] +
			current->RightRear.Hardpoints[CORNER::WheelCenter]) / 2.0,
			vectors[RightPitchAxisDirection], vectors[RightKinematicPC]);

	// Left
	// Find the normal vectors
	frontPlaneNormal = VVASEMath::GetPlaneNormal(current->LeftFront.Hardpoints[CORNER::ContactPatch],
		leftFrontVectors[InstantCenter], leftFrontVectors[InstantCenter] + leftFrontVectors[InstantAxisDirection]);
	rearPlaneNormal = VVASEMath::GetPlaneNormal(current->LeftRear.Hardpoints[CORNER::ContactPatch],
		leftRearVectors[InstantCenter], leftRearVectors[InstantCenter] + leftRearVectors[InstantAxisDirection]);

	// Get the intersection of the planes
	if (!VVASEMath::GetIntersectionOfTwoPlanes(frontPlaneNormal, current->LeftFront.Hardpoints[CORNER::ContactPatch],
		rearPlaneNormal, current->LeftRear.Hardpoints[CORNER::ContactPatch],
		vectors[LeftPitchAxisDirection], vectors[LeftKinematicPC]))
		debugger->Print(_T("Warning (KinematicOutputs::Update):  Left Kinematic Pitch Center is undefined"),
			Debugger::PriorityHigh);
	else
		// Just like we did for the right side, intersect this vector with the wheel plane
		vectors[LeftKinematicPC] = VVASEMath::IntersectWithPlane(planeNormal,
			(current->LeftFront.Hardpoints[CORNER::WheelCenter] +
			current->LeftRear.Hardpoints[CORNER::WheelCenter]) / 2.0,
			vectors[LeftPitchAxisDirection], vectors[LeftKinematicPC]);

	// For the left side, we flip the sign on the axis direction
	vectors[LeftPitchAxisDirection] *= -1.0;

	// Front track at ground [in]
	OutputsDouble[FrontTrackGround] = current->RightFront.Hardpoints[CORNER::ContactPatch].Distance(
		current->LeftFront.Hardpoints[CORNER::ContactPatch]);

	// Rear track at ground [in]
	OutputsDouble[RearTrackGround] = current->RightRear.Hardpoints[CORNER::ContactPatch].Distance(
		current->LeftRear.Hardpoints[CORNER::ContactPatch]);

	// Right wheelbase at ground [in]
	OutputsDouble[RightWheelbaseGround] = current->RightFront.Hardpoints[CORNER::ContactPatch].Distance(
		current->RightRear.Hardpoints[CORNER::ContactPatch]);

	// Left wheelbase at ground [in]
	OutputsDouble[LeftWheelbaseGround] = current->LeftFront.Hardpoints[CORNER::ContactPatch].Distance(
		current->LeftRear.Hardpoints[CORNER::ContactPatch]);

	// Front track at hub [in]
	OutputsDouble[FrontTrackHub] = current->RightFront.Hardpoints[CORNER::WheelCenter].Distance(
		current->LeftFront.Hardpoints[CORNER::WheelCenter]);

	// Rear track at hub [in]
	OutputsDouble[RearTrackHub] = current->RightRear.Hardpoints[CORNER::WheelCenter].Distance(
		current->LeftRear.Hardpoints[CORNER::WheelCenter]);

	// Right wheelbase at hub [in]
	OutputsDouble[RightWheelbaseHub] = current->RightFront.Hardpoints[CORNER::WheelCenter].Distance(
		current->RightRear.Hardpoints[CORNER::WheelCenter]);

	// Left wheelbase at hub [in]
	OutputsDouble[LeftWheelbaseHub] = current->LeftFront.Hardpoints[CORNER::WheelCenter].Distance(
		current->LeftRear.Hardpoints[CORNER::WheelCenter]);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		UpdateCorner
//
// Description:		Performs measurements that are repeated for every wheel
//					on the car.
//
// Input Arguments:
//		originalCorner	= const CORNER*, pointer to the un-perturbed state of the
//						  corner
//		currentCorner	= const CORNER*, pointer to the current state of the corner
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::UpdateCorner(const CORNER *originalCorner, const CORNER *currentCorner)
{
	// Assign pointers to the corner outputs and our sign conventions
	double *cornerDoubles;
	Vector *cornerVectors;
	short sign;
	bool isAtFront = false;

	if (originalCorner->Location == CORNER::LocationRightFront)
	{
		cornerDoubles = rightFront;
		cornerVectors = rightFrontVectors;
		sign = 1;
		isAtFront = true;
	}
	else if (originalCorner->Location == CORNER::LocationLeftFront)
	{
		cornerDoubles = leftFront;
		cornerVectors = leftFrontVectors;
		sign = -1;
		isAtFront = true;
	}
	else if (originalCorner->Location == CORNER::LocationRightRear)
	{
		cornerDoubles = rightRear;
		cornerVectors = rightRearVectors;
		sign = 1;
	}
	else if (originalCorner->Location == CORNER::LocationLeftRear)
	{
		cornerDoubles = leftRear;
		cornerVectors = leftRearVectors;
		sign = -1;
	}
	else
	{
		// Not one of our recognized locations!!!
		debugger->Print(_T("ERROR:  Corner location not recognized!"), Debugger::PriorityHigh);
		return;
	}

	// Caster [rad]
	cornerDoubles[Caster] = VVASEMath::RangeToPlusMinusPi(atan2(
		currentCorner->Hardpoints[CORNER::UpperBallJoint].x - currentCorner->Hardpoints[CORNER::LowerBallJoint].x,
		currentCorner->Hardpoints[CORNER::UpperBallJoint].z - currentCorner->Hardpoints[CORNER::LowerBallJoint].z));

	// KPI [rad]
	cornerDoubles[KPI] = VVASEMath::RangeToPlusMinusPi(sign * atan2(
		currentCorner->Hardpoints[CORNER::LowerBallJoint].y - currentCorner->Hardpoints[CORNER::UpperBallJoint].y,
		currentCorner->Hardpoints[CORNER::UpperBallJoint].z - currentCorner->Hardpoints[CORNER::LowerBallJoint].z));

	// Caster Trail [in]
	// Note on Caster Trail:  In RCVD p. 713, it is noted that sometimes trail is
	// measured perpendicular to the steering axis (instead of as a horizontal
	// distance, like we do here) because this more accurately describes the
	// moment arm that connects the tire forces to the kingpin.
	cornerDoubles[CasterTrail] = currentCorner->Hardpoints[CORNER::ContactPatch].x -
		(currentCorner->Hardpoints[CORNER::UpperBallJoint].x - currentCorner->Hardpoints[CORNER::UpperBallJoint].z *
		(currentCorner->Hardpoints[CORNER::UpperBallJoint].x - currentCorner->Hardpoints[CORNER::LowerBallJoint].x) /
		(currentCorner->Hardpoints[CORNER::UpperBallJoint].z - currentCorner->Hardpoints[CORNER::LowerBallJoint].z));

	// Scrub Radius [in]
	cornerDoubles[ScrubRadius] = sign * (currentCorner->Hardpoints[CORNER::ContactPatch].y -
		 currentCorner->Hardpoints[CORNER::UpperBallJoint].y - currentCorner->Hardpoints[CORNER::UpperBallJoint].z *
		(currentCorner->Hardpoints[CORNER::LowerBallJoint].y - currentCorner->Hardpoints[CORNER::UpperBallJoint].y) /
		(currentCorner->Hardpoints[CORNER::UpperBallJoint].z - currentCorner->Hardpoints[CORNER::LowerBallJoint].z));

	// Spindle Length [in]
	//  Spindle length is the distance between the wheel center and the steer axis, at the
	//  height of the wheel center.
	double t = (currentCorner->Hardpoints[CORNER::WheelCenter].z - currentCorner->Hardpoints[CORNER::LowerBallJoint].z) /
		(currentCorner->Hardpoints[CORNER::UpperBallJoint].z - currentCorner->Hardpoints[CORNER::LowerBallJoint].z);
	Vector PointOnSteerAxis = currentCorner->Hardpoints[CORNER::LowerBallJoint] +
		(currentCorner->Hardpoints[CORNER::UpperBallJoint] - currentCorner->Hardpoints[CORNER::LowerBallJoint]) * t;
	cornerDoubles[SpindleLength] = (PointOnSteerAxis.y - currentCorner->Hardpoints[CORNER::WheelCenter].y) /
		fabs(PointOnSteerAxis.y - currentCorner->Hardpoints[CORNER::WheelCenter].y) * sign *
		currentCorner->Hardpoints[CORNER::WheelCenter].Distance(PointOnSteerAxis);

	// Camber and Steer Angle [rad]
	Vector originalWheelPlaneNormal;
	Vector newWheelPlaneNormal;
	Vector angles;

	originalWheelPlaneNormal = VVASEMath::GetPlaneNormal(
		originalCorner->Hardpoints[CORNER::LowerBallJoint],
		originalCorner->Hardpoints[CORNER::UpperBallJoint],
		originalCorner->Hardpoints[CORNER::OutboardTieRod]);
	newWheelPlaneNormal = VVASEMath::GetPlaneNormal(
		currentCorner->Hardpoints[CORNER::LowerBallJoint],
		currentCorner->Hardpoints[CORNER::UpperBallJoint],
		currentCorner->Hardpoints[CORNER::OutboardTieRod]);

	// Calculate the wheel angles to get the steer angle
	angles = originalWheelPlaneNormal.AnglesTo(newWheelPlaneNormal);
	cornerDoubles[Steer] = angles.z;

	// Rotate the NewWheelPlaneNormal back about Z by the steer angle in preparation for solving for camber
	newWheelPlaneNormal.Rotate(cornerDoubles[Steer], Vector::AxisZ);

	// Calculate the wheel angles again, this time we want the camber angle
	angles = originalWheelPlaneNormal.AnglesTo(newWheelPlaneNormal);
	cornerDoubles[Camber] = sign * angles.x;

	// Add in the effects of static camber and toe settings
	cornerDoubles[Camber] += currentCorner->StaticCamber;
	cornerDoubles[Steer] += sign * currentCorner->StaticToe;

	// Report Camber and Steer angles between -PI and PI
	cornerDoubles[Camber] = VVASEMath::RangeToPlusMinusPi(cornerDoubles[Camber]);
	cornerDoubles[Steer] = VVASEMath::RangeToPlusMinusPi(cornerDoubles[Steer]);

	// Spring Displacement [in] - positive is compression
	cornerDoubles[Spring] = originalCorner->Hardpoints[CORNER::InboardSpring].Distance(
		originalCorner->Hardpoints[CORNER::OutboardSpring]) -
		currentCorner->Hardpoints[CORNER::InboardSpring].Distance(
		currentCorner->Hardpoints[CORNER::OutboardSpring]);

	// Shock Displacement [in] - positive is compression
	cornerDoubles[Shock] = originalCorner->Hardpoints[CORNER::InboardShock].Distance(
		originalCorner->Hardpoints[CORNER::OutboardShock]) -
		currentCorner->Hardpoints[CORNER::InboardShock].Distance(
		currentCorner->Hardpoints[CORNER::OutboardShock]);

	// Scrub [in]
	cornerDoubles[Scrub] = sign * (currentCorner->Hardpoints[CORNER::ContactPatch].y -
		originalCorner->Hardpoints[CORNER::ContactPatch].y);

	// Axle Plunge [in] - positive is shortened
	if ((currentCar->HasFrontHalfShafts() && isAtFront) || (currentCar->HasRearHalfShafts() && !isAtFront))
		cornerDoubles[AxlePlunge] =
			originalCorner->Hardpoints[CORNER::InboardHalfShaft].Distance(
			originalCorner->Hardpoints[CORNER::OutboardHalfShaft]) -
			currentCorner->Hardpoints[CORNER::InboardHalfShaft].Distance(
			currentCorner->Hardpoints[CORNER::OutboardHalfShaft]);

	// Kinematic Instant Centers and Direction Vectors [in], [-]
	//  The instant centers here will be defined as the point that lies both on the
	//  instantaneous axis of rotation and on the plane that is perpendicular to the
	//  ground and the vehicle center-plane and contains the wheel center.  This means
	//  that the instant center will share an x-ordinate with the wheel center.  The
	//  instantaneous axis of rotation is given by the intersection of the upper and
	//  lower control arm planes.  The direction vector can be determined by taking
	//  the cross product of the normal vectors for the upper and lower control arm
	//  planes.
	Vector upperPlaneNormal;
	Vector lowerPlaneNormal;

	upperPlaneNormal = VVASEMath::GetPlaneNormal(currentCorner->Hardpoints[CORNER::UpperBallJoint],
		currentCorner->Hardpoints[CORNER::UpperFrontTubMount], currentCorner->Hardpoints[CORNER::UpperRearTubMount]);
	lowerPlaneNormal = VVASEMath::GetPlaneNormal(currentCorner->Hardpoints[CORNER::LowerBallJoint],
		currentCorner->Hardpoints[CORNER::LowerFrontTubMount], currentCorner->Hardpoints[CORNER::LowerRearTubMount]);

	if (!VVASEMath::GetIntersectionOfTwoPlanes(upperPlaneNormal, currentCorner->Hardpoints[CORNER::UpperBallJoint],
		lowerPlaneNormal, currentCorner->Hardpoints[CORNER::LowerBallJoint], 
		cornerVectors[InstantAxisDirection], cornerVectors[InstantCenter]))
		debugger->Print(_T("Warning (KinematicOutputs::UpdateCorner):  Instant Center is undefined"),
			Debugger::PriorityHigh);
	else
	{
		// We now have the axis direction and a point on the axis, but we want a specific
		// point on the axis.  To do that, we determine the place where this vector passes through
		// the appropriate plane.
		Vector planeNormal(1.0, 0.0, 0.0);

		cornerVectors[InstantCenter] = VVASEMath::IntersectWithPlane(planeNormal, currentCorner->Hardpoints[CORNER::WheelCenter],
			cornerVectors[InstantAxisDirection], cornerVectors[InstantCenter]);
	}

	// Apply the sign correction, if necessary
	cornerVectors[InstantAxisDirection] *= sign;

	// About the installation ratios (= 1 / motion ratio):
	//  This is derived from calculating forces acting through the system of bodies.
	//  Given a force of one unit upwards (positive z-direction), what is the
	//  force applied to the spring?  Use the instant center of rotation of
	//  the upper and lower control arms to get the leverage of the wheel over the
	//  push/pullrod.  Determine how the force is reacted through the push/pullrod,
	//  then how the bell crank leverage changes the force.  Then we employ the principle
	//  of virtual work to find the relationship between displacements based on the
	//  relationship between the forces.  More detail is given for the spring motion
	//  ratio, but the shock motion ratio is computed using the same process.

	// Spring Installation Ratio [inches Spring/inches Wheel]
	// First, we need to apply a force to the wheel center, and determine the portion of
	// the force that is reacted through the control arm.
	// Note that this procedure varies slightly depending on what component the pushrod is
	// attached to on the outboard suspension.
	Vector force(0.0, 0.0, 1.0);// Applied to the wheel center

	Vector momentDirection;
	double momentMagnitude;
	Vector momentArm;

	Vector pointOnAxis;
	Vector forceDirection;

	// This changes depending on what is actuating the shock/spring (outer in..else)
	// and also with what the pushrod or spring/shock attach to on the outer suspension (inner
	// if..else)
	if (currentCorner->ActuationType == CORNER::ActuationPushPullrod)
	{
		// If the actuator attaches to the lower control arm
		if (currentCorner->ActuationAttachment == CORNER::AttachmentLowerAArm)
		{
			// Identify the rotation axis and a point on that axis
			pointOnAxis = cornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			momentArm = currentCorner->Hardpoints[CORNER::WheelCenter] - pointOnAxis;

			// The magnitude of the moment is determined by the cross product of the moment arm
			// and the force vector, but then we also take the dot product with the axis direction.
			// This reduces the total moment to account for the portion of it that is reacted through
			// structure and does not actually contribute to the moment.
			momentDirection = cornerVectors[InstantAxisDirection];
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();

			// The force at the ball joint is determined by dividing the moment by a new moment arm,
			// which is a vector from the axis of rotation to the ball joint (instead of the wheel
			// center like we used above).  We don't just take any point this time, instead we take
			// the point on the axis that is closest to the new force application point (ball joint).
			momentArm = currentCorner->Hardpoints[CORNER::LowerBallJoint] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->Hardpoints[CORNER::LowerBallJoint]);

			// Calculate the force acting on the lower ball joint
			force =  momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

			// The next step is calculating the force through the pushrod
			// Again, find the pivot axis and moment arm.
			pointOnAxis = currentCorner->Hardpoints[CORNER::LowerFrontTubMount];
			momentArm = currentCorner->Hardpoints[CORNER::LowerBallJoint] - pointOnAxis;
			momentDirection = currentCorner->Hardpoints[CORNER::LowerRearTubMount] - pointOnAxis;

			// Given the above information, we can calculate the moment magnitude in the same manner
			// as we did previously
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();

			// Again, calculate the force required of the pushrod on the lower A-arm to create the moment
			momentArm = currentCorner->Hardpoints[CORNER::OutboardPushrod] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->Hardpoints[CORNER::OutboardPushrod]);
			force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();
		}
		else if (currentCorner->ActuationAttachment == CORNER::AttachmentUpperAArm)
		{
			// Identify the rotation axis and a point on that axis
			pointOnAxis = cornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			momentArm = currentCorner->Hardpoints[CORNER::WheelCenter] - pointOnAxis;

			// The magnitude of the moment is determined by the cross product of the moment arm
			// and the force vector, but then we also take the dot product with the axis direction.
			// This reduces the total moment to account for the portion of it that is reacted through
			// structure and does not actually contribute to the moment.
			momentDirection = cornerVectors[InstantAxisDirection];
			momentMagnitude = momentArm.Cross(Force) * momentDirection.Normalize();

			// The force at the ball joint is determined by dividing the moment by a new moment arm,
			// which is a vector from the axis of rotation to the ball joint (instead of the wheel
			// center like we used above).  We don't just take any point this time, instead we take
			// the point on the axis that is closest to the new force application point (ball joint).
			momentArm = currentCorner->Hardpoints[CORNER::UpperBallJoint] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->Hardpoints[CORNER::UpperBallJoint]);

			// Calculate the force acting on the upper ball joint
			force =  momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

			// The next step is calculating the force through the pullrod
			// Again, find the pivot axis and moment arm.
			pointOnAxis = currentCorner->Hardpoints[CORNER::UpperFrontTubMount];
			momentArm = currentCorner->Hardpoints[CORNER::UpperBallJoint] - pointOnAxis;
			momentDirection = currentCorner->Hardpoints[CORNER::UpperRearTubMount] - pointOnAxis;

			// Given the above information, we can calculate the moment magnitude in the same manner
			// as we did previously
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();

			// Again, calculate the force required of the pushrod on the upper A-arm to create the moment
			momentArm = currentCorner->Hardpoints[CORNER::OutboardPushrod] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->Hardpoints[CORNER::OutboardPushrod]);
			force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();
		}
		else if (currentCorner->ActuationAttachment == CORNER::AttachmentUpright)
		{
			// Identify the rotation axis and a point on that axis
			pointOnAxis = cornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			momentArm = currentCorner->Hardpoints[CORNER::WheelCenter] - pointOnAxis;

			// The magnitude of the moment is determined by the cross product of the moment arm
			// and the force vector, but then we also take the dot product with the axis direction.
			// This reduces the total moment to account for the portion of it that is reacted through
			// structure and does not actually contribute to the moment.
			momentDirection = cornerVectors[InstantAxisDirection];
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();

			// The force at the ball joint is determined by dividing the moment by a new moment arm,
			// which is a vector from the axis of rotation to the ball joint (instead of the wheel
			// center like we used above).  We don't just take any point this time, instead we take
			// the point on the axis that is closest to the new force application point (ball joint).
			momentArm = currentCorner->Hardpoints[CORNER::OutboardPushrod] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->Hardpoints[CORNER::OutboardPushrod]);

			// Calculate the force acting on the pushrod
			force =  momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();
		}

		// After computing the force through the push/pullrod, the procedure is the same regardless
		// of how the suspension is configured.

		// The force from the pushrod creating the moment is not the only portion of the pushrod force.
		// The next step is to compute the total force through the pushrod.
		forceDirection = (currentCorner->Hardpoints[CORNER::InboardPushrod]
			- currentCorner->Hardpoints[CORNER::OutboardPushrod]).Normalize();
		force = forceDirection * force.Length() / (force.Normalize() * forceDirection);

		// Now we can calculate the forces acting on the shock and spring from the rotation of the
		// bellcrank about its axis.
		pointOnAxis = currentCorner->Hardpoints[CORNER::BellCrankPivot1];
		momentArm = currentCorner->Hardpoints[CORNER::InboardPushrod] - pointOnAxis;
		momentDirection = currentCorner->Hardpoints[CORNER::BellCrankPivot2] - pointOnAxis;
		momentMagnitude = momentArm.Cross(Force) * momentDirection.Normalize();

		// Spring force
		momentArm = currentCorner->Hardpoints[CORNER::OutboardSpring] - VVASEMath::NearestPointOnAxis(
			pointOnAxis, momentDirection, currentCorner->Hardpoints[CORNER::OutboardSpring]);
		force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

		// Determine the force required in the direction of the spring (like we did for the force
		// through the push/pullrod)
		forceDirection = (currentCorner->Hardpoints[CORNER::InboardSpring]
		- currentCorner->Hardpoints[CORNER::OutboardSpring]).Normalize();
		force = forceDirection * force.Length() / (force.Normalize() * forceDirection);

		// From the principle of virtual work we have these relationships:
		//  VirtualWork = ForceAtWheelCenter * VirtualDisplacement1
		//  VirtualWork = ReactionAtSpring * VirtualDisplacement2
		// From the first equation, we know VirtualWork is 1.0 * VirtualDisplacement1.z.
		// We can choose 1.0 for VirtualDisplacement1.z, so now we have the following:
		//  VirtualDisplacement2 = 1.0 / Magnitude(ReactionAtSpring);
		// We add the sign of the dot product between the force and force direction to provide a means
		// of identifying cases where the shock/spring move the "wrong" way (extend in jounce).
		cornerDoubles[SpringInstallationRatio] = 1.0 / force.Length()
			* VVASEMath::Sign(force.Normalize() * forceDirection.Normalize());

		// Shock force
		momentArm = currentCorner->Hardpoints[CORNER::OutboardShock] - VVASEMath::NearestPointOnAxis(
			pointOnAxis, momentDirection, currentCorner->Hardpoints[CORNER::OutboardShock]);
		force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

		// Determine the force required in the direction of the shock
		forceDirection = (currentCorner->Hardpoints[CORNER::InboardShock]
		- currentCorner->Hardpoints[CORNER::OutboardShock]).Normalize();
		force = forceDirection * force.Length() / (force.Normalize() * forceDirection);
		cornerDoubles[ShockInstallationRatio] = 1.0 / force.Length()
			* VVASEMath::Sign(force.Normalize() * forceDirection.Normalize());
	}
	else if (currentCorner->ActuationType == CORNER::ActuationOutboard)
	{
		// This part is identical to the beginning of the previous section,
		// but the spring/shock take the place of the pushrod, so the installation
		// ratios are calculated with those forces.

		// If the actuator attaches to the lower control arm
		if (currentCorner->ActuationAttachment == CORNER::AttachmentLowerAArm)
		{
			// Identify the rotation axis and a point on that axis
			pointOnAxis = cornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			momentArm = currentCorner->Hardpoints[CORNER::WheelCenter] - pointOnAxis;

			// The magnitude of the moment is determined by the cross product of the moment arm
			// and the force vector, but then we also take the dot product with the axis direction.
			// This reduces the total moment to account for the portion of it that is reacted through
			// structure and does not actually contribute to the moment.
			momentDirection = cornerVectors[InstantAxisDirection];
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();

			// The force at the ball joint is determined by dividing the moment by a new moment arm,
			// which is a vector from the axis of rotation to the ball joint (instead of the wheel
			// center like we used above).  We don't just take any point this time, instead we take
			// the point on the axis that is closest to the new force application point (ball joint).
			momentArm = currentCorner->Hardpoints[CORNER::LowerBallJoint] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->Hardpoints[CORNER::LowerBallJoint]);

			// Calculate the force acting on the lower ball joint
			force =  momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

			// The next step is calculating the force through shock and spring.
			// Again, find the pivot axis and moment arm.
			pointOnAxis = currentCorner->Hardpoints[CORNER::LowerFrontTubMount];
			momentArm = currentCorner->Hardpoints[CORNER::LowerBallJoint] - pointOnAxis;
			momentDirection = currentCorner->Hardpoints[CORNER::LowerRearTubMount] - pointOnAxis;

			// Given the above information, we can calculate the moment magnitude in the same manner
			// as we did previously
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();
		}
		else if (currentCorner->ActuationAttachment == CORNER::AttachmentUpperAArm)
		{
			// Identify the rotation axis and a point on that axis
			pointOnAxis = cornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			momentArm = currentCorner->Hardpoints[CORNER::WheelCenter] - pointOnAxis;

			// The magnitude of the moment is determined by the cross product of the moment arm
			// and the force vector, but then we also take the dot product with the axis direction.
			// This reduces the total moment to account for the portion of it that is reacted through
			// structure and does not actually contribute to the moment.
			momentDirection = cornerVectors[InstantAxisDirection];
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();

			// The force at the ball joint is determined by dividing the moment by a new moment arm,
			// which is a vector from the axis of rotation to the ball joint (instead of the wheel
			// center like we used above).  We don't just take any point this time, instead we take
			// the point on the axis that is closest to the new force application point (ball joint).
			momentArm = currentCorner->Hardpoints[CORNER::UpperBallJoint] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->Hardpoints[CORNER::UpperBallJoint]);

			// Calculate the force acting on the upper ball joint
			force =  momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

			// The next step is calculating the force through shock and spring.
			// Again, find the pivot axis and moment arm.
			pointOnAxis = currentCorner->Hardpoints[CORNER::UpperFrontTubMount];
			momentArm = currentCorner->Hardpoints[CORNER::UpperBallJoint] - pointOnAxis;
			momentDirection = currentCorner->Hardpoints[CORNER::UpperRearTubMount] - pointOnAxis;

			// Given the above information, we can calculate the moment magnitude in the same manner
			// as we did previously
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();
		}
		else if (currentCorner->ActuationAttachment == CORNER::AttachmentUpright)
		{
			// Identify the rotation axis and a point on that axis
			pointOnAxis = cornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			momentArm = currentCorner->Hardpoints[CORNER::WheelCenter] - pointOnAxis;

			// The magnitude of the moment is determined by the cross product of the moment arm
			// and the force vector, but then we also take the dot product with the axis direction.
			// This reduces the total moment to account for the portion of it that is reacted through
			// structure and does not actually contribute to the moment.
			momentDirection = cornerVectors[InstantAxisDirection];
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();
		}

		// The rest of the procedure is identical regardless of the suspension configuration
		// Resolve the moment into forces acting on the spring and shock

		// Spring force
		momentArm = currentCorner->Hardpoints[CORNER::OutboardSpring] - VVASEMath::NearestPointOnAxis(
			pointOnAxis, momentDirection, currentCorner->Hardpoints[CORNER::OutboardSpring]);
		force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

		// Determine the force required in the direction of the spring (like we did for the force
		// through the push/pullrod)
		forceDirection = (currentCorner->Hardpoints[CORNER::InboardSpring]
		- currentCorner->Hardpoints[CORNER::OutboardSpring]).Normalize();
		force = forceDirection * force.Length() / (force.Normalize() * forceDirection);

		// From the principle of virtual work we have these relationships:
		//  VirtualWork = forceAtWheelCenter * VirtualDisplacement1
		//  VirtualWork = ReactionAtSpring * VirtualDisplacement2
		// From the first equation, we know VirtualWork is 1.0 * VirtualDisplacement1.z.
		// We can choose 1.0 for VirtualDisplacement1.z, so now we have the following:
		//  VirtualDisplacement2 = 1.0 / Magnitude(ReactionAtSpring);
		// We add the sign of the dot product between the force and force direction to provide a means
		// of identifying cases where the shock/spring move the "wrong" way (extend in jounce).
		cornerDoubles[SpringInstallationRatio] = 1.0 / force.Length()
			* VVASEMath::Sign(force.Normalize() * forceDirection.Normalize());

		// Shock force
		momentArm = currentCorner->Hardpoints[CORNER::OutboardShock] - VVASEMath::NearestPointOnAxis(
			pointOnAxis, momentDirection, currentCorner->Hardpoints[CORNER::OutboardShock]);
		force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

		// Determine the force required in the direction of the shock
		forceDirection = (currentCorner->Hardpoints[CORNER::InboardShock]
		- currentCorner->Hardpoints[CORNER::OutboardShock]).Normalize();
		force = forceDirection * force.Length() / (force.Normalize() * forceDirection);
		cornerDoubles[ShockInstallationRatio] = 1.0 / force.Length()
			* VVASEMath::Sign(force.Normalize() * forceDirection.Normalize());
	}

	// Side View Swing Arm Length [in]
	// Find the plane that contains the wheel center and has the Y direction as a
	// normal, and find the intersection of the Instant Axis and that plane. This
	// vector's X-coordinate is the SVSA length.
	Vector planeNormal(0.0, 1.0, 0.0);
	Vector intersection;

	// Get the intersection of the Instant Center with this plane
	intersection = VVASEMath::IntersectWithPlane(planeNormal, currentCorner->Hardpoints[CORNER::WheelCenter],
		cornerVectors[InstantAxisDirection], cornerVectors[InstantCenter]);
	cornerDoubles[SideViewSwingArmLength] = fabs(intersection.x);

	// We'll need this information when calculating the anti-geometry down below:
	double sideViewSwingArmHeight = intersection.z;

	// Front View Swing Arm Length [in]
	// The procedure is identical to finding the SVSA length, except the plane we intersect
	// has the X-axis as a normal and we use the Y-coordinate as the length.
	planeNormal.Set(1.0, 0.0, 0.0);

	// Get the intersection of the Instant Center with this plane
	intersection = VVASEMath::IntersectWithPlane(planeNormal, currentCorner->Hardpoints[CORNER::WheelCenter],
		cornerVectors[InstantAxisDirection], cornerVectors[InstantCenter]);
	cornerDoubles[FrontViewSwingArmLength] = fabs(intersection.y);

	// Anti-brake [%]
	// Note that the equation changes if the brakes are inboard vs. outboard.  This
	// is because the way the torques are reacted is different:  In the case of outboard
	// brakes, the control arms are required to react some of the torque, whereas this
	// is not required when the brakes are inboard and the torque can be reacted directly
	// by the chassis.  The nomenclature for the anti-geometry comes from RCVD p. 617.
	// NOTE:  We are required to know the sprung mass CG height and the wheelbase here.
	//        we assume that the static wheelbase and CG height are still accurate here (FIXME!!!)
	double wheelbase = (currentCar->Suspension->RightRear.Hardpoints[CORNER::ContactPatch].x
		- currentCar->Suspension->RightFront.Hardpoints[CORNER::ContactPatch].x
		+ currentCar->Suspension->LeftRear.Hardpoints[CORNER::ContactPatch].x
		- currentCar->Suspension->LeftFront.Hardpoints[CORNER::ContactPatch].x) / 2.0;
	double cgHeight = currentCar->MassProperties->CenterOfGravity.z;
	double reactionPathAngleTangent;

	// Determine if we are talking about anti-dive (front) or anti-lift (rear)
	if (isAtFront)
	{
		// Is the braking torque reacted directly by the chassis, or does it first
		// travel through the control arms?
		if (currentCar->Brakes->FrontBrakesInboard)
		{
			// Compute the tangent of the reaction path angle
			reactionPathAngleTangent = (sideViewSwingArmHeight -
				currentCorner->Hardpoints[CORNER::WheelCenter].z) / cornerDoubles[SideViewSwingArmLength];

			// Compute the anti-dive
			cornerDoubles[AntiBrakePitch] = reactionPathAngleTangent * wheelbase / cgHeight
				* currentCar->Brakes->PercentFrontBraking * 100.0;
		}
		else// Outboard brakes
		{
			// Compute the tangent of the reaction path angle
			reactionPathAngleTangent = sideViewSwingArmHeight / cornerDoubles[SideViewSwingArmLength];

			// Compute the anti-dive
			cornerDoubles[AntiBrakePitch] = reactionPathAngleTangent / (cgHeight / Wheelbase
				* currentCar->Brakes->PercentFrontBraking) * 100.0;
		}
	}
	else// Anti-lift (rear)
	{
		// Is the braking torque reacted directly by the chassis, or does it first
		// travel through the control arms?
		if (CurrentCar->Brakes->RearBrakesInboard)
		{
			// Compute the tangent of the reaction path angle
			ReactionPathAngleTangent = (SideViewSwingArmHeight -
				CurrentCorner->Hardpoints[CORNER::WheelCenter].z) / CornerDoubles[SideViewSwingArmLength];

			// Compute the anti-lift
			CornerDoubles[AntiBrakePitch] = ReactionPathAngleTangent * Wheelbase / CGHeight
				* (1.0 - CurrentCar->Brakes->PercentFrontBraking) * 100.0;
		}
		else// Outboard brakes
		{
			// Compute the tangent of the reaction path angle
			ReactionPathAngleTangent = SideViewSwingArmHeight / CornerDoubles[SideViewSwingArmLength];

			// Compute the anti-lift
			CornerDoubles[AntiBrakePitch] = ReactionPathAngleTangent / (CGHeight / Wheelbase
				* (1.0 - CurrentCar->Brakes->PercentFrontBraking)) * 100.0;
		}
	}

	// If the anti-brake geometry is undefined at this point, we had a divide by zero.  This
	// is the same as zero percent anti-brake, so we will clean this up by setting it to zero.
	if (VVASEMath::IsNaN(CornerDoubles[AntiBrakePitch]))
		CornerDoubles[AntiBrakePitch] = 0.0;

	// Anti-drive [%]
	// If this end of the car doesn't deliver power to the ground, then it doesn't have
	// anti-drive geometry.  Only if the longitudinal forces are present does anti-geometry
	// exist.
	// FIXME:  Do we need a % front traction for use with AWD?
	// FIXME:  This will change with independant vs. solid axle suspensions (currently we assume indepenant)
	if (CurrentCar->Drivetrain->DriveType == DRIVETRAIN::DRIVE_ALL_WHEEL ||
		(CurrentCar->Drivetrain->DriveType == DRIVETRAIN::DRIVE_FRONT_WHEEL && IsAtFront) ||
		(CurrentCar->Drivetrain->DriveType == DRIVETRAIN::DRIVE_REAR_WHEEL && !IsAtFront))
	{
		// Compute the tangent of the reaction path angle
		ReactionPathAngleTangent = (SideViewSwingArmHeight -
			CurrentCorner->Hardpoints[CORNER::WheelCenter].z) / CornerDoubles[SideViewSwingArmLength];

		// Compute the anti-lift
		CornerDoubles[AntiDrivePitch] = ReactionPathAngleTangent * Wheelbase / CGHeight * 100.0;

		// If the anti-drive geometry is undefined at this point, we had a divide by zero.
		// This is the same as zero percent anti-drive, so we will clean this up by setting
		// it to zero.
		if (VVASEMath::IsNaN(CornerDoubles[AntiDrivePitch]))
			CornerDoubles[AntiDrivePitch] = 0.0;
	}

	return;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetCornerDoubleName
//
// Description:		Returns a string containing the name of the specified output.
//
// Input Arguments:
//		_Output	= const CORNER_OUTPUTS_DOUBLE& specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the specified output
//
//==========================================================================
wxString KinematicOutputs::GetCornerDoubleName(const CORNER_OUTPUTS_DOUBLE &_Output)
{
	// The return value
	wxString Name;

	// Return the name based on the specified output index
	switch (_Output)
	{
	case Caster:
		Name = _T("Caster");
		break;

	case Camber:
		Name = _T("Camber");
		break;

	case KPI:
		Name = _T("KPI");
		break;

	case Steer:
		Name = _T("Steer");
		break;

	case Spring:
		Name = _T("Spring");
		break;

	case Shock:
		Name = _T("Shock");
		break;

	case AxlePlunge:
		Name = _T("Axle Plunge");
		break;

	case CasterTrail:
		Name = _T("Caster Trail");
		break;

	case ScrubRadius:
		Name = _T("Scrub Radius");
		break;

	case Scrub:
		Name = _T("Scrub");
		break;

	case SpringInstallationRatio:
		Name = _T("Spring Installation Ratio");
		break;

	case ShockInstallationRatio:
		Name = _T("Shock Installation Ratio");
		break;

	case SpindleLength:
		Name = _T("Spindle Length");
		break;

	case SideViewSwingArmLength:
		Name = _T("SVSA Length");
		break;

	case FrontViewSwingArmLength:
		Name = _T("FVSA Length");
		break;

	case AntiBrakePitch:
		Name = _T("Anti-Brake");
		break;

	case AntiDrivePitch:
		Name = _T("Anti-Drive");
		break;

	default:
		assert(0);
		break;
	}

	return Name;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetCornerVectorName
//
// Description:		Returns a string containing the name of the specified output.
//
// Input Arguments:
//		_Output	= const CORNER_OUTPUTS_Vector& specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the specified output
//
//==========================================================================
wxString KinematicOutputs::GetCornerVectorName(const CORNER_OUTPUTS_Vector &_Output)
{
	// The return value
	wxString Name;

	// Return the name based on the specified output index
	switch (_Output)
	{
	case InstantCenter:
		Name = _T("Instant Center");
		break;

	case InstantAxisDirection:
		Name = _T("Instant Axis Direction");
		break;

	default:
		assert(0);
		break;
	}

	return Name;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetDoubleName
//
// Description:		Returns a string containing the name of the specified output.
//
// Input Arguments:
//		_Output	= const OUTPUTS_DOUBLE& specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the specified output
//
//==========================================================================
wxString KinematicOutputs::GetDoubleName(const OUTPUTS_DOUBLE &_Output)
{
	// The return value
	wxString Name;

	// Return the name based on the specified output index
	switch (_Output)
	{
	case FrontARBTwist:
		Name = _T("Front ARB Twist");
		break;

	case RearARBTwist:
		Name = _T("Rear ARB Twist");
		break;

	case FrontThirdSpring:
		Name = _T("Front Third Spring");
		break;

	case FrontThirdShock:
		Name = _T("Front Third Shock");
		break;

	case RearThirdSpring:
		Name = _T("Rear Third Spring");
		break;

	case RearThirdShock:
		Name = _T("Rear Third Shock");
		break;

	case FrontNetSteer:
		Name = _T("Front Net Steer");
		break;

	case RearNetSteer:
		Name = _T("Rear Net Steer");
		break;

	case FrontNetScrub:
		Name = _T("Front Net Scrub");
		break;

	case RearNetScrub:
		Name = _T("Rear Net Scrub");
		break;

	case FrontARBMotionRatio:
		Name = _T("Front ARB Motion Ratio");
		break;

	case RearARBMotionRatio:
		Name = _T("Rear ARB Motion Ratio");
		break;

	case FrontTrackGround:
		Name = _T("Front Ground Track");
		break;

	case RearTrackGround:
		Name = _T("Rear Ground Track");
		break;

	case RightWheelbaseGround:
		Name = _T("Right Ground Wheelbase");
		break;

	case LeftWheelbaseGround:
		Name = _T("Left Ground Wheelbase");
		break;

	case FrontTrackHub:
		Name = _T("Front Hub Track");
		break;

	case RearTrackHub:
		Name = _T("Rear Hub Track");
		break;

	case RightWheelbaseHub:
		Name = _T("Right Hub Wheelbase");
		break;

	case LeftWheelbaseHub:
		Name = _T("Left Hub Wheelbase");
		break;

	default:
		assert(0);
		break;
	}

	return Name;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetVectorName
//
// Description:		Returns a string containing the name of the specified output.
//
// Input Arguments:
//		_Output	= const OUTPUTS_Vector& specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the specified output
//
//==========================================================================
wxString KinematicOutputs::GetVectorName(const OUTPUTS_Vector &_Output)
{
	// The return value
	wxString Name;

	// Return the name based on the specified output index
	switch (_Output)
	{
	case FrontKinematicRC:
		Name = _T("Front Kinematic Roll Center");
		break;

	case RearKinematicRC:
		Name = _T("Rear Kinematic Roll Center");
		break;

	case RightKinematicPC:
		Name = _T("Right Kinematic Pitch Center");
		break;

	case LeftKinematicPC:
		Name = _T("Left Kinematic Pitch Center");
		break;

	case FrontRollAxisDirection:
		Name = _T("Front Roll Axis Direction");
		break;

	case RearRollAxisDirection:
		Name = _T("Rear Roll Axis Direction");
		break;

	case RightPitchAxisDirection:
		Name = _T("Right Pitch Axis Direction");
		break;

	case LeftPitchAxisDirection:
		Name = _T("Left Pitch Axis Direction");
		break;

	default:
		assert(0);
		break;
	}

	return Name;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetOutputValue
//
// Description:		Returns the value of the output from the complete list of
//					class outputs.
//
// Input Arguments:
//		_Output	= const &OUTPUTS_COMPLETE specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying the value of the requested output
//
//==========================================================================
double KinematicOutputs::GetOutputValue(const OUTPUTS_COMPLETE &_Output) const
{
	// The return value
	double Value;

	// Temporary Vector for extracting componenets
	Vector Temp;

	// Temporary OUTPUTS_COMPLETE for math
	OUTPUTS_COMPLETE NewOutputIndex;

	// Depending on the specified OUTPUTS_COMPLETE, choose which output to return
	if (_Output <= EndRightFrontDoubles)
		Value = RightFront[_Output - StartRightFrontDoubles];
	else if (_Output <= EndRightFrontVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartRightFrontVectors);
		Temp = RightFrontVectors[int(NewOutputIndex / 3)];

		// Choose the value based on the selected componenet
		switch (NewOutputIndex % 3)
		{
		case 0:
			Value = Temp.x;
			break;

		case 1:
			Value = Temp.y;
			break;

		case 2:
		default:
			Value = Temp.z;
			break;
		}
	}
	else if (_Output <= EndLeftFrontDoubles)
		Value = LeftFront[_Output - StartLeftFrontDoubles];
	else if (_Output <= EndLeftFrontVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartLeftFrontVectors);
		Temp = LeftFrontVectors[int(NewOutputIndex / 3)];

		// Choose the value based on the selected componenet
		switch (NewOutputIndex % 3)
		{
		case 0:
			Value = Temp.x;
			break;

		case 1:
			Value = Temp.y;
			break;

		case 2:
		default:
			Value = Temp.z;
			break;
		}
	}
	else if (_Output <= EndRightRearDoubles)
		Value = RightRear[_Output - StartRightRearDoubles];
	else if (_Output <= EndRightRearVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartRightRearVectors);
		Temp = RightRearVectors[int(NewOutputIndex / 3)];

		// Choose the value based on the selected componenet
		switch (NewOutputIndex % 3)
		{
		case 0:
			Value = Temp.x;
			break;

		case 1:
			Value = Temp.y;
			break;

		case 2:
		default:
			Value = Temp.z;
			break;
		}
	}
	else if (_Output <= EndLeftRearDoubles)
		Value = LeftRear[_Output - StartLeftRearDoubles];
	else if (_Output <= EndLeftRearVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartLeftRearVectors);
		Temp = LeftRearVectors[int(NewOutputIndex / 3)];

		// Choose the value based on the selected componenet
		switch (NewOutputIndex % 3)
		{
		case 0:
			Value = Temp.x;
			break;

		case 1:
			Value = Temp.y;
			break;

		case 2:
		default:
			Value = Temp.z;
			break;
		}
	}
	else if (_Output <= EndDoubles)
		Value = Doubles[_Output - StartDoubles];
	else if (_Output <= EndVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartVectors);
		Temp = Vectors[int(NewOutputIndex / 3)];

		// Choose the value based on the selected componenet
		switch (NewOutputIndex % 3)
		{
		case 0:
			Value = Temp.x;
			break;

		case 1:
			Value = Temp.y;
			break;

		case 2:
		default:
			Value = Temp.z;
			break;
		}
	}
	else
		Value = 0.0;

	return Value;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetOutputUnitType
//
// Description:		Returns the unit type of the output from the complete
//					list of class outputs.
//
// Input Arguments:
//		_Output	= const &OUTPUTS_COMPLETE specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		Convert::UnitType specifying the unit type of the requested output
//
//==========================================================================
Convert::UnitType KinematicOutputs::GetOutputUnitType(const OUTPUTS_COMPLETE &_Output)
{
	// The value to return
	Convert::UnitType UnitType;

	// For some simple math
	OUTPUTS_COMPLETE NewOutputIndex;

	// Depending on the specified Output, choose the units string
	if (_Output <= EndRightFrontDoubles)
		UnitType = GetCornerDoubleUnitType((CORNER_OUTPUTS_DOUBLE)(_Output - StartRightFrontDoubles));
	else if (_Output <= EndRightFrontVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartRightFrontVectors);
		UnitType = GetCornerVectorUnitType((CORNER_OUTPUTS_Vector)int(NewOutputIndex / 3));
	}
	else if (_Output <= EndLeftFrontDoubles)
		UnitType = GetCornerDoubleUnitType((CORNER_OUTPUTS_DOUBLE)(_Output - StartLeftFrontDoubles));
	else if (_Output <= EndLeftFrontVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartLeftFrontVectors);
		UnitType = GetCornerVectorUnitType((CORNER_OUTPUTS_Vector)int(NewOutputIndex / 3));
	}
	else if (_Output <= EndRightRearDoubles)
		UnitType = GetCornerDoubleUnitType((CORNER_OUTPUTS_DOUBLE)(_Output - StartRightRearDoubles));
	else if (_Output <= EndRightRearVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartRightRearVectors);
		UnitType = GetCornerVectorUnitType((CORNER_OUTPUTS_Vector)int(NewOutputIndex / 3));
	}
	else if (_Output <= EndLeftRearDoubles)
		UnitType = GetCornerDoubleUnitType((CORNER_OUTPUTS_DOUBLE)(_Output - StartLeftRearDoubles));
	else if (_Output <= EndLeftRearVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartLeftRearVectors);
		UnitType = GetCornerVectorUnitType((CORNER_OUTPUTS_Vector)int(NewOutputIndex / 3));
	}
	else if (_Output <= EndDoubles)
		UnitType = GetDoubleUnitType((OUTPUTS_DOUBLE)(_Output - StartDoubles));
	else if (_Output <= EndVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartVectors);
		UnitType = GetVectorUnitType((OUTPUTS_Vector)int(NewOutputIndex / 3));
	}
	else
		UnitType = Convert::UnitTypeUnknown;

	return UnitType;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetOutputName
//
// Description:		Returns the name of the output from the complete list of
//					class outputs.
//
// Input Arguments:
//		_Output	= const OUTPUTS_COMPLETE& specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString specifying the name of the requested output
//
//==========================================================================
wxString KinematicOutputs::GetOutputName(const OUTPUTS_COMPLETE &_Output)
{
	// The value to return
	wxString Name;

	// For some simple math
	OUTPUTS_COMPLETE NewOutputIndex;

	// Depending on the specified PLOT_ID, choose the name of the string
	// Vectors are a special case - depending on which component of the vector is chosen,
	// we need to append a different string to the end of the Name
	if (_Output <= EndRightFrontDoubles)
	{
		Name = GetCornerDoubleName((CORNER_OUTPUTS_DOUBLE)(_Output - StartRightFrontDoubles));
		Name.Prepend(_T("Right Front "));
	}
	else if (_Output <= EndRightFrontVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE((int)_Output - (int)StartRightFrontVectors);
		Name = GetCornerVectorName((CORNER_OUTPUTS_Vector)int(NewOutputIndex / 3));

		// Append the appropriate tag, depending on the specified component
		switch (NewOutputIndex % 3)
		{
		case 0:
			Name.append(_T(" (X)"));
			break;

		case 1:
			Name.append(_T(" (Y)"));
			break;

		case 2:
		default:
			Name.append(_T(" (Z)"));
			break;
		}

		Name.Prepend(_T("Right Front "));
	}
	else if (_Output <= EndLeftFrontDoubles)
	{
		Name = GetCornerDoubleName((CORNER_OUTPUTS_DOUBLE)(_Output - StartLeftFrontDoubles));
		Name.Prepend(_T("Left Front "));
	}
	else if (_Output <= EndLeftFrontVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE((int)_Output - (int)StartLeftFrontVectors);
		Name = GetCornerVectorName((CORNER_OUTPUTS_Vector)int(NewOutputIndex / 3));

		// Append the appropriate tag, depending on the specified component
		switch (NewOutputIndex % 3)
		{
		case 0:
			Name.append(_T(" (X)"));
			break;

		case 1:
			Name.append(_T(" (Y)"));
			break;

		case 2:
		default:
			Name.append(_T(" (Z)"));
			break;
		}

		Name.Prepend(_T("Left Front "));
	}
	else if (_Output <= EndRightRearDoubles)
	{
		Name = GetCornerDoubleName((CORNER_OUTPUTS_DOUBLE)(_Output - StartRightRearDoubles));
		Name.Prepend(_T("Right Rear "));
	}
	else if (_Output <= EndRightRearVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE((int)_Output - (int)StartRightRearVectors);
		Name = GetCornerVectorName((CORNER_OUTPUTS_Vector)int(NewOutputIndex / 3));

		// Append the appropriate tag, depending on the specified component
		switch (NewOutputIndex % 3)
		{
		case 0:
			Name.append(_T(" (X)"));
			break;

		case 1:
			Name.append(_T(" (Y)"));
			break;

		case 2:
		default:
			Name.append(_T(" (Z)"));
			break;
		}

		Name.Prepend(_T("Right Rear "));
	}
	else if (_Output <= EndLeftRearDoubles)
	{
		Name = GetCornerDoubleName((CORNER_OUTPUTS_DOUBLE)(_Output - StartLeftRearDoubles));
		Name.Prepend(_T("Left Rear "));
	}
	else if (_Output <= EndLeftRearVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE((int)_Output - (int)StartLeftRearVectors);
		Name = GetCornerVectorName((CORNER_OUTPUTS_Vector)int(NewOutputIndex / 3));

		// Append the appropriate tag, depending on the specified component
		switch (NewOutputIndex % 3)
		{
		case 0:
			Name.append(_T(" (X)"));
			break;

		case 1:
			Name.append(_T(" (Y)"));
			break;

		case 2:
		default:
			Name.append(_T(" (Z)"));
			break;
		}

		Name.Prepend(_T("Left Rear "));
	}
	else if (_Output <= EndDoubles)
		Name = GetDoubleName((OUTPUTS_DOUBLE)(_Output - StartDoubles));
	else if (_Output <= EndVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE((int)_Output - (int)StartVectors);
		Name = GetVectorName((OUTPUTS_Vector)int(NewOutputIndex / 3));

		// Append the appropriate tag, depending on the specified component
		switch (NewOutputIndex % 3)
		{
		case 0:
			Name.append(_T(" (X)"));
			break;

		case 1:
			Name.append(_T(" (Y)"));
			break;

		case 2:
		default:
			Name.append(_T(" (Z)"));
			break;
		}
	}
	else
		Name = _T("Unrecognized name");

	return Name;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetCornerDoubleUnitType
//
// Description:		Returns a the type of units for the specified output.
//
// Input Arguments:
//		_Output	= const CORNER_OUTPUTS_DOUBLE& specifying the
//				  output in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		Convert::UnitType describing the units of the specified output
//
//==========================================================================
Convert::UnitType KinematicOutputs::GetCornerDoubleUnitType(const CORNER_OUTPUTS_DOUBLE &_Output)
{
	// The return value
	Convert::UnitType UnitType;

	// Determine the units based on it's type
	switch (_Output)
	{
		// Angles
	case Caster:
	case Camber:
	case KPI:
	case Steer:
		UnitType = Convert::UnitTypeAngle;
		break;

		// Distances
	case Spring:
	case Shock:
	case AxlePlunge:
	case CasterTrail:
	case ScrubRadius:
	case Scrub:
	case SpindleLength:
	case SideViewSwingArmLength:
	case FrontViewSwingArmLength:
		UnitType = Convert::UnitTypeDistance;
		break;

		// Unitless
	case SpringInstallationRatio:
	case ShockInstallationRatio:
	case AntiBrakePitch:
	case AntiDrivePitch:
		UnitType = Convert::UnitTypeUnitless;
		break;

		// Unknown
	default:
		UnitType = Convert::UnitTypeUnknown;
		break;
	}

	return UnitType;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetCornerVectorUnitType
//
// Description:		Returns a the type of units for the specified output.
//
// Input Arguments:
//		_Output	= const CORNER_OUTPUTS_Vector& specifying the
//				  output in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		Convert::UnitType describing the units of the specified output
//
//==========================================================================
Convert::UnitType KinematicOutputs::GetCornerVectorUnitType(const CORNER_OUTPUTS_Vector &_Output)
{
	// The return value
	Convert::UnitType UnitType;

	// Determine the units based on it's type
	switch (_Output)
	{
		// Distances
	case InstantCenter:
		UnitType = Convert::UnitTypeDistance;
		break;

		// Unitless (no conversion)
	case InstantAxisDirection:
		UnitType = Convert::UnitTypeUnitless;
		break;

		// Unknown
	default:
		UnitType = Convert::UnitTypeUnknown;
		break;
	}

	return UnitType;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetDoubleUnitType
//
// Description:		Returns a the type of units for the specified output.
//
// Input Arguments:
//		_Output	= const OUTPUTS_DOUBLE& specifying the
//				  output in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		Convert::UnitType describing the units of the specified output
//
//==========================================================================
Convert::UnitType KinematicOutputs::GetDoubleUnitType(const OUTPUTS_DOUBLE &_Output)
{
	// The return value
	Convert::UnitType UnitType;

	// Determine the units based on it's type
	switch (_Output)
	{
		// Distances
	case FrontThirdSpring:
	case FrontThirdShock:
	case RearThirdSpring:
	case RearThirdShock:
	case FrontNetScrub:
	case RearNetScrub:
	case FrontTrackGround:
	case RearTrackGround:
	case RightWheelbaseGround:
	case LeftWheelbaseGround:
	case FrontTrackHub:
	case RearTrackHub:
	case RightWheelbaseHub:
	case LeftWheelbaseHub:
		UnitType = Convert::UnitTypeDistance;
		break;

		// Angles
	case FrontARBTwist:
	case RearARBTwist:
	case FrontNetSteer:
	case RearNetSteer:
		UnitType = Convert::UnitTypeAngle;
		break;

		// Unitless (no conversion)
	case FrontARBMotionRatio:
	case RearARBMotionRatio:
		UnitType = Convert::UnitTypeUnitless;
		break;

		// Unknown units
	default:
		UnitType = Convert::UnitTypeUnknown;
		break;
	}

	return UnitType;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetVectorUnitType
//
// Description:		Returns a the type of units for the specified output.
//
// Input Arguments:
//		_Output	= const OUTPUTS_Vector& specifying the
//				  output in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		Convert::UnitType describing the units of the specified output
//
//==========================================================================
Convert::UnitType KinematicOutputs::GetVectorUnitType(const OUTPUTS_Vector &_Output)
{
	// The return value
	Convert::UnitType UnitType;

	// Determine the units based on it's type
	switch (_Output)
	{
		// Distances
	case FrontKinematicRC:
	case RearKinematicRC:
	case RightKinematicPC:
	case LeftKinematicPC:
		UnitType = Convert::UnitTypeDistance;
		break;

		// Unitless
	case FrontRollAxisDirection:
	case RearRollAxisDirection:
	case RightPitchAxisDirection:
	case LeftPitchAxisDirection:
		UnitType = Convert::UnitTypeUnitless;
		break;

		// Unknown
	default:
		UnitType = Convert::UnitTypeUnknown;
		break;
	}

	return UnitType;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		OutputsCompleteIndex
//
// Description:		Returns an index refering to the list of OUTPUTS_COMPLETE
//					for the specified output.
//
// Input Arguments:
//		Location		= const &CORNER::Location specifying the relevant corner of the car
//		CornerDouble	= const &CORNER_OUTPUTS_DOUBLE
//		CornerVector	= const &CORNER_OUTPUTS_Vector
//		Double			= const &OUTPUTS_DOUBLE
//		vector			= const &OUTPUTS_Vector
//		Axis			= const &Vector::Axis
//
// Output Arguments:
//		None
//
// Return Value:
//		OUTPUTS_COMPLETE describing the index for the specified output
//
//==========================================================================
KinematicOutputs::OUTPUTS_COMPLETE KinematicOutputs::OutputsCompleteIndex(const CORNER::LOCATION &Location,
																			const CORNER_OUTPUTS_DOUBLE &CornerDouble,
																			const CORNER_OUTPUTS_Vector &CornerVector,
																			const OUTPUTS_DOUBLE &Double,
																			const OUTPUTS_Vector &vector,
																			const Vector::Axis &Axis)
{
	OUTPUTS_COMPLETE CompleteIndex = NumberOfOutputScalars;

	// Switch based on the corner
	switch (Location)
	{
	case CORNER::LocationLeftFront:
		if (CornerDouble != NumberOfCornerOutputDoubles)
			CompleteIndex = (KinematicOutputs::OUTPUTS_COMPLETE)(StartLeftFrontDoubles + CornerDouble);
		else
			CompleteIndex = (KinematicOutputs::OUTPUTS_COMPLETE)(StartLeftFrontVectors + CornerVector * 3 + Axis);
		break;

	case CORNER::LocationRightFront:
		if (CornerDouble != NumberOfCornerOutputDoubles)
			CompleteIndex = (KinematicOutputs::OUTPUTS_COMPLETE)(StartRightFrontDoubles + CornerDouble);
		else
			CompleteIndex = (KinematicOutputs::OUTPUTS_COMPLETE)(StartRightFrontVectors + CornerVector * 3 + Axis);
		break;

	case CORNER::LocationLeftRear:
		if (CornerDouble != NumberOfCornerOutputDoubles)
			CompleteIndex = (KinematicOutputs::OUTPUTS_COMPLETE)(StartLeftRearDoubles + CornerDouble);
		else
			CompleteIndex = (KinematicOutputs::OUTPUTS_COMPLETE)(StartLeftRearVectors + CornerVector * 3 + Axis);
		break;

	case CORNER::LocationRightRear:
		if (CornerDouble != NumberOfCornerOutputDoubles)
			CompleteIndex = (KinematicOutputs::OUTPUTS_COMPLETE)(StartRightRearDoubles + CornerDouble);
		else
			CompleteIndex = (KinematicOutputs::OUTPUTS_COMPLETE)(StartRightRearVectors + CornerVector * 3 + Axis);
		break;

	default:// Not a corner output
		if (Double != NumberOfOutputDoubles)
			CompleteIndex = (KinematicOutputs::OUTPUTS_COMPLETE)(StartDoubles + Double);
		else
			CompleteIndex = (KinematicOutputs::OUTPUTS_COMPLETE)(StartVectors + vector * 3 + Axis);
		break;
	}

	return CompleteIndex;
}