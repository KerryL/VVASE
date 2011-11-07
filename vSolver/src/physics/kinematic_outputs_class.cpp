/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  kinematic_outputs_class.cpp
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
#include "vCar/car_class.h"
#include "vCar/brakes_class.h"
#include "vCar/drivetrain_class.h"
#include "vCar/mass_class.h"
#include "vCar/suspension_class.h"
#include "vSolver/physics/kinematic_outputs_class.h"
#include "vMath/car_math.h"
#include "vUtilities/wheelset_structs.h"
#include "vUtilities/debug_class.h"

//==========================================================================
// Class:			KINEMATIC_OUTPUTS
// Function:		KINEMATIC_OUTPUTS
//
// Description:		Constructor for KINEMATIC_OUTPUTS class.
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
KINEMATIC_OUTPUTS::KINEMATIC_OUTPUTS()
{
	InitializeAllOutputs();
}

//==========================================================================
// Class:			KINEMATIC_OUTPUTS
// Function:		~KINEMATIC_OUTPUTS
//
// Description:		Destructor for KINEMATIC_OUTPUTS class.
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
KINEMATIC_OUTPUTS::~KINEMATIC_OUTPUTS()
{
}

//==========================================================================
// Class:			KINEMATIC_OUTPUTS
// Function:		Constant Declarations
//
// Description:		Constant declarations for KINEMATIC_OUTPUTS class.
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
const DEBUGGER *KINEMATIC_OUTPUTS::Debugger = NULL;

//==========================================================================
// Class:			KINEMATIC_OUTPUTS
// Function:		InitializeAllOutputs
//
// Description:		Initializes all outputs to QNAN
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
void KINEMATIC_OUTPUTS::InitializeAllOutputs(void)
{
	// Initialize all outputs to QNAN
	int i;

	// Corner Doubles
	for (i = 0; i < NumberOfCornerOutputDoubles; i++)
	{
		RightFront[i] = VVASEMath::QNAN;
		LeftFront[i] = VVASEMath::QNAN;
		RightRear[i] = VVASEMath::QNAN;
		LeftRear[i] = VVASEMath::QNAN;
	}

	// Corner Vectors
	for (i = 0; i< NumberOfCornerOutputVectors; i++)
	{
		RightFrontVectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
		LeftFrontVectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
		RightRearVectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
		LeftRearVectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
	}

	// Whole-car Doubles
	for (i = 0; i < NumberOfOutputDoubles; i++)
		Doubles[i] = VVASEMath::QNAN;

	// Whole-car Vectors
	for (i = 0; i < NumberOfOutputVectors; i++)
		Vectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
}

//==========================================================================
// Class:			KINEMATIC_OUTPUTS
// Function:		Update
//
// Description:		Takes all of the virtual measurements and saves them in
//					Outputs.  Every member of Outputs is included here.
//					Measurements are take on Current.
//
// Input Arguments:
//		Original	= cosnt CAR*, pointing to object describing the car's
//					  un-perturbed state
//		Current		= const SUSPENSION*, pointing to object describing the new
//					  state of the car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KINEMATIC_OUTPUTS::Update(const CAR *Original, const SUSPENSION *Current)
{
	// Copy the car pointer to our class variable
	CurrentCar = Original;

	// Re-initialize all outputs before doing the computations
	InitializeAllOutputs();

	// Update each corner's outputs
	UpdateCorner(&Original->Suspension->RightFront, &Current->RightFront);
	UpdateCorner(&Original->Suspension->LeftFront, &Current->LeftFront);
	UpdateCorner(&Original->Suspension->RightRear, &Current->RightRear);
	UpdateCorner(&Original->Suspension->LeftRear, &Current->LeftRear);

	// Net Steer [rad]
	Doubles[FrontNetSteer] = RightFront[Steer] - LeftFront[Steer];
	Doubles[RearNetSteer] = RightRear[Steer] - LeftRear[Steer];

	// Net Scrub [in]
	Doubles[FrontNetScrub] = RightFront[Scrub] + LeftFront[Scrub];
	Doubles[RearNetScrub] = RightRear[Scrub] + LeftRear[Scrub];

	// ARB Twist [rad]
	// Initialize the twist in case the car has no sway bars
	Doubles[FrontARBTwist] = 0.0;
	Doubles[RearARBTwist] = 0.0;
	VECTOR Arm1Direction, Arm2Direction;
	VECTOR SwayBarAxis;
	if (Current->FrontBarStyle == SUSPENSION::SwayBarUBar)
	{
		// Project these directions onto the plane whose normal is the sway bar axis
		SwayBarAxis = Current->RightFront.Hardpoints[CORNER::BarArmAtPivot] -
			Current->LeftFront.Hardpoints[CORNER::BarArmAtPivot];

		// The references for U-bar twist are the arms at the end of the bar
		Arm1Direction = VVASEMath::ProjectOntoPlane(Current->RightFront.Hardpoints[CORNER::BarArmAtPivot] -
			Current->RightFront.Hardpoints[CORNER::InboardBarLink], SwayBarAxis);
		Arm2Direction = VVASEMath::ProjectOntoPlane(Current->LeftFront.Hardpoints[CORNER::BarArmAtPivot] -
			Current->LeftFront.Hardpoints[CORNER::InboardBarLink], SwayBarAxis);

		// The angle between these vectors, when projected onto the plane that is normal
		// to the swaybar axis is given by the dot product
		Doubles[FrontARBTwist] = acos((Arm1Direction * Arm2Direction) /
			(Arm1Direction.Length() * Arm2Direction.Length()));
	}
	else if (Current->FrontBarStyle == SUSPENSION::SwayBarTBar)
	{
		// FIXME!!!
	}
	else if (Current->FrontBarStyle == SUSPENSION::SwayBarGeared)
	{
		// FIXME!!!
	}

	if (Current->RearBarStyle == SUSPENSION::SwayBarUBar)
	{
		// Project these directions onto the plane whose normal is the swaybar axis
		SwayBarAxis = Current->RightRear.Hardpoints[CORNER::BarArmAtPivot] -
			Current->LeftRear.Hardpoints[CORNER::BarArmAtPivot];

		// The references for U-bar twist are the arms at the end of the bar
		Arm1Direction = VVASEMath::ProjectOntoPlane(Current->RightRear.Hardpoints[CORNER::BarArmAtPivot] -
			Current->RightRear.Hardpoints[CORNER::InboardBarLink], SwayBarAxis);
		Arm2Direction = VVASEMath::ProjectOntoPlane(Current->LeftRear.Hardpoints[CORNER::BarArmAtPivot] -
			Current->LeftRear.Hardpoints[CORNER::InboardBarLink], SwayBarAxis);

		// The angle between these vectors, when projected onto the plane that is normal
		// to the swaybar axis is given by the dot product
		Doubles[RearARBTwist] = acos((Arm1Direction * Arm2Direction) /
			(Arm1Direction.Length() * Arm2Direction.Length()));
	}
	else if (Current->RearBarStyle == SUSPENSION::SwayBarTBar)
	{
		// FIXME!!!
	}
	else if (Current->RearBarStyle == SUSPENSION::SwayBarGeared)
	{
		// FIXME!!!
	}

	// Kinematic Roll Centers and Direction Vectors [in], [-]
	//  Wm. C. Mitchell makes clear the assumptions that are made when calculating kinematic
	//  roll centers in his SAE paper "Asymmetric Roll Centers" (983085).  My interpertation
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
	VECTOR RightPlaneNormal;
	VECTOR LeftPlaneNormal;
	VECTOR PlaneNormal(1.0, 0.0, 0.0);// For projecting the vectors to find the kinematic centers

	// Front
	// Find the normal vectors
	RightPlaneNormal = VVASEMath::GetPlaneNormal(Current->RightFront.Hardpoints[CORNER::ContactPatch],
		RightFrontVectors[InstantCenter], RightFrontVectors[InstantCenter] + RightFrontVectors[InstantAxisDirection]);
	LeftPlaneNormal = VVASEMath::GetPlaneNormal(Current->LeftFront.Hardpoints[CORNER::ContactPatch],
		LeftFrontVectors[InstantCenter], LeftFrontVectors[InstantCenter] + LeftFrontVectors[InstantAxisDirection]);

	// Get the intersection of the planes
	if (!VVASEMath::GetIntersectionOfTwoPlanes(RightPlaneNormal, Current->RightFront.Hardpoints[CORNER::ContactPatch],
		LeftPlaneNormal, Current->LeftFront.Hardpoints[CORNER::ContactPatch],
		Vectors[FrontRollAxisDirection], Vectors[FrontKinematicRC]))
		Debugger->Print(_T("Warning (KINEMATIC_OUTPUTS::Update):  Front Kinematic Roll Center is undefined"),
			DEBUGGER::PriorityHigh);
	else
		// We now have the axis direction and a point on the axis, but we want a specific
		// point on the axis.  To do that, we determine the place where this vector passes through
		// the appropriate plane.
		Vectors[FrontKinematicRC] = VVASEMath::IntersectWithPlane(PlaneNormal,
			(Current->RightFront.Hardpoints[CORNER::WheelCenter] +
			Current->LeftFront.Hardpoints[CORNER::WheelCenter]) / 2.0,
			Vectors[FrontRollAxisDirection], Vectors[FrontKinematicRC]);

	// Rear
	// Find the normal vectors
	RightPlaneNormal = VVASEMath::GetPlaneNormal(Current->RightRear.Hardpoints[CORNER::ContactPatch],
		RightRearVectors[InstantCenter], RightRearVectors[InstantCenter] + RightRearVectors[InstantAxisDirection]);
	LeftPlaneNormal = VVASEMath::GetPlaneNormal(Current->LeftRear.Hardpoints[CORNER::ContactPatch],
		LeftRearVectors[InstantCenter], LeftRearVectors[InstantCenter] + LeftRearVectors[InstantAxisDirection]);

	// Get the intersection of the planes
	if (!VVASEMath::GetIntersectionOfTwoPlanes(RightPlaneNormal, Current->RightRear.Hardpoints[CORNER::ContactPatch],
		LeftPlaneNormal, Current->LeftRear.Hardpoints[CORNER::ContactPatch],
		Vectors[RearRollAxisDirection], Vectors[RearKinematicRC]))
		Debugger->Print(_T("Warning (KINEMATIC_OUTPUTS::Update):  Rear Kinematic Roll Center is undefined"),
			DEBUGGER::PriorityHigh);
	else
		// Just like we did on for the front, intersect this vector with the wheel plane
		Vectors[RearKinematicRC] = VVASEMath::IntersectWithPlane(PlaneNormal,
			(Current->RightRear.Hardpoints[CORNER::WheelCenter] +
			Current->LeftRear.Hardpoints[CORNER::WheelCenter]) / 2.0,
			Vectors[RearRollAxisDirection], Vectors[RearKinematicRC]);

	// Kinematic Pitch Centers and Directions [in], [-]
	// All of the same assumptions that we have for roll centers apply here.
	// The method is also the same as the roll center calculations.
	VECTOR FrontPlaneNormal;
	VECTOR RearPlaneNormal;
	PlaneNormal.Set(0.0, 1.0, 0.0);// For projecting the vectors to find the kinematic centers

	// Right
	// Find the normal vectors
	FrontPlaneNormal = VVASEMath::GetPlaneNormal(Current->RightFront.Hardpoints[CORNER::ContactPatch],
		RightFrontVectors[InstantCenter], RightFrontVectors[InstantCenter] + RightFrontVectors[InstantAxisDirection]);
	RearPlaneNormal = VVASEMath::GetPlaneNormal(Current->RightRear.Hardpoints[CORNER::ContactPatch],
		RightRearVectors[InstantCenter], RightRearVectors[InstantCenter] + RightRearVectors[InstantAxisDirection]);

	// Get the intersection of the planes
	if (!VVASEMath::GetIntersectionOfTwoPlanes(FrontPlaneNormal, Current->RightFront.Hardpoints[CORNER::ContactPatch],
		RearPlaneNormal, Current->RightRear.Hardpoints[CORNER::ContactPatch],
		Vectors[RightPitchAxisDirection], Vectors[RightKinematicPC]))
		Debugger->Print(_T("Warning (KINEMATIC_OUTPUTS::Update):  Right Kinematic Pitch Center is undefined"),
			DEBUGGER::PriorityHigh);
	else
		// We now have the axis direction and a point on the axis, but we want a specific
		// point on the axis.  To do that, we determine the place where this vector passes through
		// the appropriate plane.
		Vectors[RightKinematicPC] = VVASEMath::IntersectWithPlane(PlaneNormal,
			(Current->RightFront.Hardpoints[CORNER::WheelCenter] +
			Current->RightRear.Hardpoints[CORNER::WheelCenter]) / 2.0,
			Vectors[RightPitchAxisDirection], Vectors[RightKinematicPC]);

	// Left
	// Find the normal vectors
	FrontPlaneNormal = VVASEMath::GetPlaneNormal(Current->LeftFront.Hardpoints[CORNER::ContactPatch],
		LeftFrontVectors[InstantCenter], LeftFrontVectors[InstantCenter] + LeftFrontVectors[InstantAxisDirection]);
	RearPlaneNormal = VVASEMath::GetPlaneNormal(Current->LeftRear.Hardpoints[CORNER::ContactPatch],
		LeftRearVectors[InstantCenter], LeftRearVectors[InstantCenter] + LeftRearVectors[InstantAxisDirection]);

	// Get the intersection of the planes
	if (!VVASEMath::GetIntersectionOfTwoPlanes(FrontPlaneNormal, Current->LeftFront.Hardpoints[CORNER::ContactPatch],
		RearPlaneNormal, Current->LeftRear.Hardpoints[CORNER::ContactPatch],
		Vectors[LeftPitchAxisDirection], Vectors[LeftKinematicPC]))
		Debugger->Print(_T("Warning (KINEMATIC_OUTPUTS::Update):  Left Kinematic Pitch Center is undefined"),
			DEBUGGER::PriorityHigh);
	else
		// Just like we did for the right side, intersect this vector with the wheel plane
		Vectors[LeftKinematicPC] = VVASEMath::IntersectWithPlane(PlaneNormal,
			(Current->LeftFront.Hardpoints[CORNER::WheelCenter] +
			Current->LeftRear.Hardpoints[CORNER::WheelCenter]) / 2.0,
			Vectors[LeftPitchAxisDirection], Vectors[LeftKinematicPC]);

	// For the left side, we flip the sign on the axis direction
	Vectors[LeftPitchAxisDirection] *= -1.0;

	// Front track at ground [in]
	Doubles[FrontTrackGround] = Current->RightFront.Hardpoints[CORNER::ContactPatch].Distance(
		Current->LeftFront.Hardpoints[CORNER::ContactPatch]);

	// Rear track at ground [in]
	Doubles[RearTrackGround] = Current->RightRear.Hardpoints[CORNER::ContactPatch].Distance(
		Current->LeftRear.Hardpoints[CORNER::ContactPatch]);

	// Right wheelbase at ground [in]
	Doubles[RightWheelbaseGround] = Current->RightFront.Hardpoints[CORNER::ContactPatch].Distance(
		Current->RightRear.Hardpoints[CORNER::ContactPatch]);

	// Left wheelbase at ground [in]
	Doubles[LeftWheelbaseGround] = Current->LeftFront.Hardpoints[CORNER::ContactPatch].Distance(
		Current->LeftRear.Hardpoints[CORNER::ContactPatch]);

	// Front track at hub [in]
	Doubles[FrontTrackHub] = Current->RightFront.Hardpoints[CORNER::WheelCenter].Distance(
		Current->LeftFront.Hardpoints[CORNER::WheelCenter]);

	// Rear track at hub [in]
	Doubles[RearTrackHub] = Current->RightRear.Hardpoints[CORNER::WheelCenter].Distance(
		Current->LeftRear.Hardpoints[CORNER::WheelCenter]);

	// Right wheelbase at hub [in]
	Doubles[RightWheelbaseHub] = Current->RightFront.Hardpoints[CORNER::WheelCenter].Distance(
		Current->RightRear.Hardpoints[CORNER::WheelCenter]);

	// Left wheelbase at hub [in]
	Doubles[LeftWheelbaseHub] = Current->LeftFront.Hardpoints[CORNER::WheelCenter].Distance(
		Current->LeftRear.Hardpoints[CORNER::WheelCenter]);

	return;
}

//==========================================================================
// Class:			KINEMATIC_OUTPUTS
// Function:		UpdateCorner
//
// Description:		Performs measurements that are repeated for every wheel
//					on the car.
//
// Input Arguments:
//		OriginalCorner	= const CORNER*, pointer to the un-perturbed state of the
//						  corner
//		CurrentCorner	= const CORNER*, pointer to the current state of the corner
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KINEMATIC_OUTPUTS::UpdateCorner(const CORNER *OriginalCorner, const CORNER *CurrentCorner)
{
	// Assign pointers to the corner outputs and our sign conventions
	double *CornerDoubles;
	VECTOR *CornerVectors;
	short Sign;
	bool IsAtFront = false;

	if (OriginalCorner->Location == CORNER::LocationRightFront)
	{
		CornerDoubles = RightFront;
		CornerVectors = RightFrontVectors;
		Sign = 1;
		IsAtFront = true;
	}
	else if (OriginalCorner->Location == CORNER::LocationLeftFront)
	{
		CornerDoubles = LeftFront;
		CornerVectors = LeftFrontVectors;
		Sign = -1;
		IsAtFront = true;
	}
	else if (OriginalCorner->Location == CORNER::LocationRightRear)
	{
		CornerDoubles = RightRear;
		CornerVectors = RightRearVectors;
		Sign = 1;
	}
	else if (OriginalCorner->Location == CORNER::LocationLeftRear)
	{
		CornerDoubles = LeftRear;
		CornerVectors = LeftRearVectors;
		Sign = -1;
	}
	else
	{
		// Not one of our recognized locations!!!
		Debugger->Print(_T("ERROR:  Corner location not regognized!"), DEBUGGER::PriorityHigh);
		return;
	}

	// Caster [rad]
	CornerDoubles[Caster] = VVASEMath::RangeToPlusMinusPi(atan2(
		CurrentCorner->Hardpoints[CORNER::UpperBallJoint].X - CurrentCorner->Hardpoints[CORNER::LowerBallJoint].X,
		CurrentCorner->Hardpoints[CORNER::UpperBallJoint].Z - CurrentCorner->Hardpoints[CORNER::LowerBallJoint].Z));

	// KPI [rad]
	CornerDoubles[KPI] = VVASEMath::RangeToPlusMinusPi(Sign * atan2(
		CurrentCorner->Hardpoints[CORNER::LowerBallJoint].Y - CurrentCorner->Hardpoints[CORNER::UpperBallJoint].Y,
		CurrentCorner->Hardpoints[CORNER::UpperBallJoint].Z - CurrentCorner->Hardpoints[CORNER::LowerBallJoint].Z));

	// Caster Trail [in]
	// Note on Caster Trail:  In RCVD p. 713, it is noted that sometimes trail is
	// meausred perpendicular to the steering axis (instead of as a horizontal
	// distance, like we do here) because this more accurately describes the
	// moment arm that connects the tire forces to the kingpin.
	CornerDoubles[CasterTrail] = CurrentCorner->Hardpoints[CORNER::ContactPatch].X -
		(CurrentCorner->Hardpoints[CORNER::UpperBallJoint].X - CurrentCorner->Hardpoints[CORNER::UpperBallJoint].Z *
		(CurrentCorner->Hardpoints[CORNER::UpperBallJoint].X - CurrentCorner->Hardpoints[CORNER::LowerBallJoint].X) /
		(CurrentCorner->Hardpoints[CORNER::UpperBallJoint].Z - CurrentCorner->Hardpoints[CORNER::LowerBallJoint].Z));

	// Scrub Radius [in]
	CornerDoubles[ScrubRadius] = Sign * (CurrentCorner->Hardpoints[CORNER::ContactPatch].Y -
		 CurrentCorner->Hardpoints[CORNER::UpperBallJoint].Y - CurrentCorner->Hardpoints[CORNER::UpperBallJoint].Z *
		(CurrentCorner->Hardpoints[CORNER::LowerBallJoint].Y - CurrentCorner->Hardpoints[CORNER::UpperBallJoint].Y) /
		(CurrentCorner->Hardpoints[CORNER::UpperBallJoint].Z - CurrentCorner->Hardpoints[CORNER::LowerBallJoint].Z));

	// Spindle Length [in]
	//  Spindle length is the distance between the wheel center and the steer axis, at the
	//  height of the wheel center.
	double t = (CurrentCorner->Hardpoints[CORNER::WheelCenter].Z - CurrentCorner->Hardpoints[CORNER::LowerBallJoint].Z) /
		(CurrentCorner->Hardpoints[CORNER::UpperBallJoint].Z - CurrentCorner->Hardpoints[CORNER::LowerBallJoint].Z);
	VECTOR PointOnSteerAxis = CurrentCorner->Hardpoints[CORNER::LowerBallJoint] +
		(CurrentCorner->Hardpoints[CORNER::UpperBallJoint] - CurrentCorner->Hardpoints[CORNER::LowerBallJoint]) * t;
	CornerDoubles[SpindleLength] = (PointOnSteerAxis.Y - CurrentCorner->Hardpoints[CORNER::WheelCenter].Y) /
		fabs(PointOnSteerAxis.Y - CurrentCorner->Hardpoints[CORNER::WheelCenter].Y) * Sign *
		CurrentCorner->Hardpoints[CORNER::WheelCenter].Distance(PointOnSteerAxis);

	// Camber and Steer Angle [rad]
	VECTOR OriginalWheelPlaneNormal;
	VECTOR NewWheelPlaneNormal;
	VECTOR Angles;

	OriginalWheelPlaneNormal = VVASEMath::GetPlaneNormal(
		OriginalCorner->Hardpoints[CORNER::LowerBallJoint],
		OriginalCorner->Hardpoints[CORNER::UpperBallJoint],
		OriginalCorner->Hardpoints[CORNER::OutboardTieRod]);
	NewWheelPlaneNormal = VVASEMath::GetPlaneNormal(
		CurrentCorner->Hardpoints[CORNER::LowerBallJoint],
		CurrentCorner->Hardpoints[CORNER::UpperBallJoint],
		CurrentCorner->Hardpoints[CORNER::OutboardTieRod]);

	// Calculate the wheel angles to get the steer angle
	Angles = OriginalWheelPlaneNormal.AnglesTo(NewWheelPlaneNormal);
	CornerDoubles[Steer] = Angles.Z;

	// Rotate the NewWheelPlaneNormal back about Z by the steer angle in preparation for solving for camber
	NewWheelPlaneNormal.Rotate(CornerDoubles[Steer], VECTOR::AxisZ);

	// Calculate the wheel angles again, this time we want the camber angle
	Angles = OriginalWheelPlaneNormal.AnglesTo(NewWheelPlaneNormal);
	CornerDoubles[Camber] = Sign * Angles.X;

	// Add in the effects of static camber and toe settings
	CornerDoubles[Camber] += CurrentCorner->StaticCamber;
	CornerDoubles[Steer] += Sign * CurrentCorner->StaticToe;

	// Report Camber and Steer angles between -PI and PI
	CornerDoubles[Camber] = VVASEMath::RangeToPlusMinusPi(CornerDoubles[Camber]);
	CornerDoubles[Steer] = VVASEMath::RangeToPlusMinusPi(CornerDoubles[Steer]);

	// Spring Displacement [in] - positive is compression
	CornerDoubles[Spring] = OriginalCorner->Hardpoints[CORNER::InboardSpring].Distance(
		OriginalCorner->Hardpoints[CORNER::OutboardSpring]) -
		CurrentCorner->Hardpoints[CORNER::InboardSpring].Distance(
		CurrentCorner->Hardpoints[CORNER::OutboardSpring]);

	// Shock Displacement [in] - positive is compression
	CornerDoubles[Shock] = OriginalCorner->Hardpoints[CORNER::InboardShock].Distance(
		OriginalCorner->Hardpoints[CORNER::OutboardShock]) -
		CurrentCorner->Hardpoints[CORNER::InboardShock].Distance(
		CurrentCorner->Hardpoints[CORNER::OutboardShock]);

	// Scrub [in]
	CornerDoubles[Scrub] = Sign * (CurrentCorner->Hardpoints[CORNER::ContactPatch].Y -
		OriginalCorner->Hardpoints[CORNER::ContactPatch].Y);

	// Axle Plunge [in] - positive is shortened
	if ((CurrentCar->HasFrontHalfShafts() && IsAtFront) || (CurrentCar->HasRearHalfShafts() && !IsAtFront))
		CornerDoubles[AxlePlunge] =
			OriginalCorner->Hardpoints[CORNER::InboardHalfShaft].Distance(
			OriginalCorner->Hardpoints[CORNER::OutboardHalfShaft]) -
			CurrentCorner->Hardpoints[CORNER::InboardHalfShaft].Distance(
			CurrentCorner->Hardpoints[CORNER::OutboardHalfShaft]);

	// Kinematic Instant Centers and Direction Vectors [in], [-]
	//  The instant centers here will be defined as the point that lies both on the
	//  instantaneous axis of rotation and on the plane that is perpendicular to the
	//  ground and the vehicle center-plane and contains the wheel center.  This means
	//  that the instant center will share an x-ordinate with the wheel center.  The
	//  instantaneous axis of rotation is given by the intersection of the upper and
	//  lower control arm planes.  The direction vector can be determined by taking
	//  the cross product of the normal vectors for the upper and lower control arm
	//  planes.
	VECTOR UpperPlaneNormal;
	VECTOR LowerPlaneNormal;

	UpperPlaneNormal = VVASEMath::GetPlaneNormal(CurrentCorner->Hardpoints[CORNER::UpperBallJoint],
		CurrentCorner->Hardpoints[CORNER::UpperFrontTubMount], CurrentCorner->Hardpoints[CORNER::UpperRearTubMount]);
	LowerPlaneNormal = VVASEMath::GetPlaneNormal(CurrentCorner->Hardpoints[CORNER::LowerBallJoint],
		CurrentCorner->Hardpoints[CORNER::LowerFrontTubMount], CurrentCorner->Hardpoints[CORNER::LowerRearTubMount]);

	if (!VVASEMath::GetIntersectionOfTwoPlanes(UpperPlaneNormal, CurrentCorner->Hardpoints[CORNER::UpperBallJoint],
		LowerPlaneNormal, CurrentCorner->Hardpoints[CORNER::LowerBallJoint], 
		CornerVectors[InstantAxisDirection], CornerVectors[InstantCenter]))
		Debugger->Print(_T("Warning (KINEMATIC_OUTPUTS::UpdateCorner):  Instant Center is undefined"),
			DEBUGGER::PriorityHigh);
	else
	{
		// We now have the axis direction and a point on the axis, but we want a specific
		// point on the axis.  To do that, we determine the place where this vector passes through
		// the appropriate plane.
		VECTOR PlaneNormal(1.0, 0.0, 0.0);

		CornerVectors[InstantCenter] = VVASEMath::IntersectWithPlane(PlaneNormal, CurrentCorner->Hardpoints[CORNER::WheelCenter],
			CornerVectors[InstantAxisDirection], CornerVectors[InstantCenter]);
	}

	// Apply the sign correction, if necessary
	CornerVectors[InstantAxisDirection] *= Sign;

	// About the installation ratios (= 1 / motion ratio):
	//  This is derived from calculating forces acting through the system of bodies.
	//  Given a force of one unit upwards (positive z-direction), what is the
	//  force applied to the spring?  Use the instant center of rotation of
	//  the upper and lower control arms to get the leverage of the wheel over the
	//  push/pullrod.  Determine how the force is reacted through the push/pullrod,
	//  then how the bell crank leverage changes the force.  Then we empoly the principle
	//  of virtual work to find the relatationship between displacements based on the
	//  relationship between the forces.  More detail is given for the spring motion
	//  ratio, but the shock motion ratio is computed using the same process.

	// Spring Installation Ratio [inches Spring/inches Wheel]
	// First, we need to apply a force to the wheel center, and determine the portion of
	// the force that is reacted through the control arm.
	// Note that this procedure varies slightly depending on what component the pushrod is
	// attached to on the outboard suspension.
	VECTOR Force(0.0, 0.0, 1.0);// Applied to the wheel center

	VECTOR MomentDirection;
	double MomentMagnitude;
	VECTOR MomentArm;

	VECTOR PointOnAxis;
	VECTOR ForceDirection;

	// This changes depending on what is actuating the shock/spring (outer in..else)
	// and also with what the pushrod or spring/shock attach to on the outer suspension (inner
	// if..else)
	if (CurrentCorner->ActuationType == CORNER::ActuationPushPullrod)
	{
		// If the actuator attaches to the lower control arm
		if (CurrentCorner->ActuationAttachment == CORNER::AttachmentLowerAArm)
		{
			// Identify the rotation axis and a point on that axis
			PointOnAxis = CornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			MomentArm = CurrentCorner->Hardpoints[CORNER::WheelCenter] - PointOnAxis;

			// The magnitude of the moment is determined by the cross product of the moment arm
			// and the force vector, but then we also take the dot product with the axis direction.
			// This reduces the total moment to account for the portion of it that is reacted through
			// structure and does not actually contribute to the moment.
			MomentDirection = CornerVectors[InstantAxisDirection];
			MomentMagnitude = MomentArm.Cross(Force) * MomentDirection.Normalize();

			// The force at the ball joint is determined by dividing the moment by a new moment arm,
			// which is a vector from the axis of rotation to the ball joint (instead of the wheel
			// center like we used above).  We don't just take any point this time, instead we take
			// the point on the axis that is closest to the new force application point (ball joint).
			MomentArm = CurrentCorner->Hardpoints[CORNER::LowerBallJoint] - VVASEMath::NearestPointOnAxis(
				PointOnAxis, MomentDirection, CurrentCorner->Hardpoints[CORNER::LowerBallJoint]);

			// Calculate the force acting on the lower ball joint
			Force =  MomentDirection.Cross(MomentArm).Normalize() * MomentMagnitude / MomentArm.Length();

			// The next step is calculating the force through the pushrod
			// Again, find the pivot axis and moment arm.
			PointOnAxis = CurrentCorner->Hardpoints[CORNER::LowerFrontTubMount];
			MomentArm = CurrentCorner->Hardpoints[CORNER::LowerBallJoint] - PointOnAxis;
			MomentDirection = CurrentCorner->Hardpoints[CORNER::LowerRearTubMount] - PointOnAxis;

			// Given the above information, we can calculate the moment magnitude in the same manner
			// as we did previously
			MomentMagnitude = MomentArm.Cross(Force) * MomentDirection.Normalize();

			// Again, calculate the force required of the pushrod on the lower A-arm to create the moment
			MomentArm = CurrentCorner->Hardpoints[CORNER::OutboardPushrod] - VVASEMath::NearestPointOnAxis(
				PointOnAxis, MomentDirection, CurrentCorner->Hardpoints[CORNER::OutboardPushrod]);
			Force = MomentDirection.Cross(MomentArm).Normalize() * MomentMagnitude / MomentArm.Length();
		}
		else if (CurrentCorner->ActuationAttachment == CORNER::AttachmentUpperAArm)
		{
			// Identify the rotation axis and a point on that axis
			PointOnAxis = CornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			MomentArm = CurrentCorner->Hardpoints[CORNER::WheelCenter] - PointOnAxis;

			// The magnitude of the moment is determined by the cross product of the moment arm
			// and the force vector, but then we also take the dot product with the axis direction.
			// This reduces the total moment to account for the portion of it that is reacted through
			// structure and does not actually contribute to the moment.
			MomentDirection = CornerVectors[InstantAxisDirection];
			MomentMagnitude = MomentArm.Cross(Force) * MomentDirection.Normalize();

			// The force at the ball joint is determined by dividing the moment by a new moment arm,
			// which is a vector from the axis of rotation to the ball joint (instead of the wheel
			// center like we used above).  We don't just take any point this time, instead we take
			// the point on the axis that is closest to the new force application point (ball joint).
			MomentArm = CurrentCorner->Hardpoints[CORNER::UpperBallJoint] - VVASEMath::NearestPointOnAxis(
				PointOnAxis, MomentDirection, CurrentCorner->Hardpoints[CORNER::UpperBallJoint]);

			// Calculate the force acting on the upper ball joint
			Force =  MomentDirection.Cross(MomentArm).Normalize() * MomentMagnitude / MomentArm.Length();

			// The next step is calculating the force through the pullrod
			// Again, find the pivot axis and moment arm.
			PointOnAxis = CurrentCorner->Hardpoints[CORNER::UpperFrontTubMount];
			MomentArm = CurrentCorner->Hardpoints[CORNER::UpperBallJoint] - PointOnAxis;
			MomentDirection = CurrentCorner->Hardpoints[CORNER::UpperRearTubMount] - PointOnAxis;

			// Given the above information, we can calculate the moment magnitude in the same manner
			// as we did previously
			MomentMagnitude = MomentArm.Cross(Force) * MomentDirection.Normalize();

			// Again, calculate the force required of the pushrod on the upper A-arm to create the moment
			MomentArm = CurrentCorner->Hardpoints[CORNER::OutboardPushrod] - VVASEMath::NearestPointOnAxis(
				PointOnAxis, MomentDirection, CurrentCorner->Hardpoints[CORNER::OutboardPushrod]);
			Force = MomentDirection.Cross(MomentArm).Normalize() * MomentMagnitude / MomentArm.Length();
		}
		else if (CurrentCorner->ActuationAttachment == CORNER::AttachmentUpright)
		{
			// Identify the rotation axis and a point on that axis
			PointOnAxis = CornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			MomentArm = CurrentCorner->Hardpoints[CORNER::WheelCenter] - PointOnAxis;

			// The magnitude of the moment is determined by the cross product of the moment arm
			// and the force vector, but then we also take the dot product with the axis direction.
			// This reduces the total moment to account for the portion of it that is reacted through
			// structure and does not actually contribute to the moment.
			MomentDirection = CornerVectors[InstantAxisDirection];
			MomentMagnitude = MomentArm.Cross(Force) * MomentDirection.Normalize();

			// The force at the ball joint is determined by dividing the moment by a new moment arm,
			// which is a vector from the axis of rotation to the ball joint (instead of the wheel
			// center like we used above).  We don't just take any point this time, instead we take
			// the point on the axis that is closest to the new force application point (ball joint).
			MomentArm = CurrentCorner->Hardpoints[CORNER::OutboardPushrod] - VVASEMath::NearestPointOnAxis(
				PointOnAxis, MomentDirection, CurrentCorner->Hardpoints[CORNER::OutboardPushrod]);

			// Calculate the force acting on the pushrod
			Force =  MomentDirection.Cross(MomentArm).Normalize() * MomentMagnitude / MomentArm.Length();
		}

		// After computing the force through the push/pullrod, the procedure is the same regardless
		// of how the suspension is configured.

		// The force from the pushrod creating the moment is not the only portion of the pushrod force.
		// The next step is to compute the total force through the pushrod.
		ForceDirection = (CurrentCorner->Hardpoints[CORNER::InboardPushrod]
			- CurrentCorner->Hardpoints[CORNER::OutboardPushrod]).Normalize();
		Force = ForceDirection * Force.Length() / (Force.Normalize() * ForceDirection);

		// Now we can calculate the forces acting on the shock and spring from the rotation of the
		// bellcrank about its axis.
		PointOnAxis = CurrentCorner->Hardpoints[CORNER::BellCrankPivot1];
		MomentArm = CurrentCorner->Hardpoints[CORNER::InboardPushrod] - PointOnAxis;
		MomentDirection = CurrentCorner->Hardpoints[CORNER::BellCrankPivot2] - PointOnAxis;
		MomentMagnitude = MomentArm.Cross(Force) * MomentDirection.Normalize();

		// Spring force
		MomentArm = CurrentCorner->Hardpoints[CORNER::OutboardSpring] - VVASEMath::NearestPointOnAxis(
			PointOnAxis, MomentDirection, CurrentCorner->Hardpoints[CORNER::OutboardSpring]);
		Force = MomentDirection.Cross(MomentArm).Normalize() * MomentMagnitude / MomentArm.Length();

		// Determine the force required in the direction of the spring (like we did for the force
		// through the push/pullrod)
		ForceDirection = (CurrentCorner->Hardpoints[CORNER::InboardSpring]
		- CurrentCorner->Hardpoints[CORNER::OutboardSpring]).Normalize();
		Force = ForceDirection * Force.Length() / (Force.Normalize() * ForceDirection);

		// From the principle of virtual work we have these relationships:
		//  VirtualWork = ForceAtWheelCenter * VirtualDisplacement1
		//  VirtualWork = ReactionAtSpring * VirtualDisplacement2
		// From the first equation, we know VirtualWork is 1.0 * VirtualDisplacement1.Z.
		// We can choose 1.0 for VirtualDisplacement1.Z, so now we have the following:
		//  VirtualDisplacement2 = 1.0 / Magnitude(ReactionAtSpring);
		// We add the sign of the dot product between the force and force direction to provide a means
		// of identifying cases where the shock/spring move the "wrong" way (extend in jounce).
		CornerDoubles[SpringInstallationRatio] = 1.0 / Force.Length()
			* VVASEMath::Sign(Force.Normalize() * ForceDirection.Normalize());

		// Shock force
		MomentArm = CurrentCorner->Hardpoints[CORNER::OutboardShock] - VVASEMath::NearestPointOnAxis(
			PointOnAxis, MomentDirection, CurrentCorner->Hardpoints[CORNER::OutboardShock]);
		Force = MomentDirection.Cross(MomentArm).Normalize() * MomentMagnitude / MomentArm.Length();

		// Determine the force required in the direction of the shock
		ForceDirection = (CurrentCorner->Hardpoints[CORNER::InboardShock]
		- CurrentCorner->Hardpoints[CORNER::OutboardShock]).Normalize();
		Force = ForceDirection * Force.Length() / (Force.Normalize() * ForceDirection);
		CornerDoubles[ShockInstallationRatio] = 1.0 / Force.Length()
			* VVASEMath::Sign(Force.Normalize() * ForceDirection.Normalize());
	}
	else if (CurrentCorner->ActuationType == CORNER::ActuationOutboard)
	{
		// This part is identical to the beginning of the previous section,
		// but the spring/shock take the place of the pushrod, so the installation
		// ratios are calculated with those forces.

		// If the actuator attaches to the lower control arm
		if (CurrentCorner->ActuationAttachment == CORNER::AttachmentLowerAArm)
		{
			// Identify the rotation axis and a point on that axis
			PointOnAxis = CornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			MomentArm = CurrentCorner->Hardpoints[CORNER::WheelCenter] - PointOnAxis;

			// The magnitude of the moment is determined by the cross product of the moment arm
			// and the force vector, but then we also take the dot product with the axis direction.
			// This reduces the total moment to account for the portion of it that is reacted through
			// structure and does not actually contribute to the moment.
			MomentDirection = CornerVectors[InstantAxisDirection];
			MomentMagnitude = MomentArm.Cross(Force) * MomentDirection.Normalize();

			// The force at the ball joint is determined by dividing the moment by a new moment arm,
			// which is a vector from the axis of rotation to the ball joint (instead of the wheel
			// center like we used above).  We don't just take any point this time, instead we take
			// the point on the axis that is closest to the new force application point (ball joint).
			MomentArm = CurrentCorner->Hardpoints[CORNER::LowerBallJoint] - VVASEMath::NearestPointOnAxis(
				PointOnAxis, MomentDirection, CurrentCorner->Hardpoints[CORNER::LowerBallJoint]);

			// Calculate the force acting on the lower ball joint
			Force =  MomentDirection.Cross(MomentArm).Normalize() * MomentMagnitude / MomentArm.Length();

			// The next step is calculating the force through shock and spring.
			// Again, find the pivot axis and moment arm.
			PointOnAxis = CurrentCorner->Hardpoints[CORNER::LowerFrontTubMount];
			MomentArm = CurrentCorner->Hardpoints[CORNER::LowerBallJoint] - PointOnAxis;
			MomentDirection = CurrentCorner->Hardpoints[CORNER::LowerRearTubMount] - PointOnAxis;

			// Given the above information, we can calculate the moment magnitude in the same manner
			// as we did previously
			MomentMagnitude = MomentArm.Cross(Force) * MomentDirection.Normalize();
		}
		else if (CurrentCorner->ActuationAttachment == CORNER::AttachmentUpperAArm)
		{
			// Identify the rotation axis and a point on that axis
			PointOnAxis = CornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			MomentArm = CurrentCorner->Hardpoints[CORNER::WheelCenter] - PointOnAxis;

			// The magnitude of the moment is determined by the cross product of the moment arm
			// and the force vector, but then we also take the dot product with the axis direction.
			// This reduces the total moment to account for the portion of it that is reacted through
			// structure and does not actually contribute to the moment.
			MomentDirection = CornerVectors[InstantAxisDirection];
			MomentMagnitude = MomentArm.Cross(Force) * MomentDirection.Normalize();

			// The force at the ball joint is determined by dividing the moment by a new moment arm,
			// which is a vector from the axis of rotation to the ball joint (instead of the wheel
			// center like we used above).  We don't just take any point this time, instead we take
			// the point on the axis that is closest to the new force application point (ball joint).
			MomentArm = CurrentCorner->Hardpoints[CORNER::UpperBallJoint] - VVASEMath::NearestPointOnAxis(
				PointOnAxis, MomentDirection, CurrentCorner->Hardpoints[CORNER::UpperBallJoint]);

			// Calculate the force acting on the upper ball joint
			Force =  MomentDirection.Cross(MomentArm).Normalize() * MomentMagnitude / MomentArm.Length();

			// The next step is calculating the force through shock and spring.
			// Again, find the pivot axis and moment arm.
			PointOnAxis = CurrentCorner->Hardpoints[CORNER::UpperFrontTubMount];
			MomentArm = CurrentCorner->Hardpoints[CORNER::UpperBallJoint] - PointOnAxis;
			MomentDirection = CurrentCorner->Hardpoints[CORNER::UpperRearTubMount] - PointOnAxis;

			// Given the above information, we can calculate the moment magnitude in the same manner
			// as we did previously
			MomentMagnitude = MomentArm.Cross(Force) * MomentDirection.Normalize();
		}
		else if (CurrentCorner->ActuationAttachment == CORNER::AttachmentUpright)
		{
			// Identify the rotation axis and a point on that axis
			PointOnAxis = CornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			MomentArm = CurrentCorner->Hardpoints[CORNER::WheelCenter] - PointOnAxis;

			// The magnitude of the moment is determined by the cross product of the moment arm
			// and the force vector, but then we also take the dot product with the axis direction.
			// This reduces the total moment to account for the portion of it that is reacted through
			// structure and does not actually contribute to the moment.
			MomentDirection = CornerVectors[InstantAxisDirection];
			MomentMagnitude = MomentArm.Cross(Force) * MomentDirection.Normalize();
		}

		// The rest of the procedure is identical regardless of the suspension configuration
		// Resolve the moment into forces acting on the spring and shock

		// Spring force
		MomentArm = CurrentCorner->Hardpoints[CORNER::OutboardSpring] - VVASEMath::NearestPointOnAxis(
			PointOnAxis, MomentDirection, CurrentCorner->Hardpoints[CORNER::OutboardSpring]);
		Force = MomentDirection.Cross(MomentArm).Normalize() * MomentMagnitude / MomentArm.Length();

		// Determine the force required in the direction of the spring (like we did for the force
		// through the push/pullrod)
		ForceDirection = (CurrentCorner->Hardpoints[CORNER::InboardSpring]
		- CurrentCorner->Hardpoints[CORNER::OutboardSpring]).Normalize();
		Force = ForceDirection * Force.Length() / (Force.Normalize() * ForceDirection);

		// From the principle of virtual work we have these relationships:
		//  VirtualWork = ForceAtWheelCenter * VirtualDisplacement1
		//  VirtualWork = ReactionAtSpring * VirtualDisplacement2
		// From the first equation, we know VirtualWork is 1.0 * VirtualDisplacement1.Z.
		// We can choose 1.0 for VirtualDisplacement1.Z, so now we have the following:
		//  VirtualDisplacement2 = 1.0 / Magnitude(ReactionAtSpring);
		// We add the sign of the dot product between the force and force direction to provide a means
		// of identifying cases where the shock/spring move the "wrong" way (extend in jounce).
		CornerDoubles[SpringInstallationRatio] = 1.0 / Force.Length()
			* VVASEMath::Sign(Force.Normalize() * ForceDirection.Normalize());

		// Shock force
		MomentArm = CurrentCorner->Hardpoints[CORNER::OutboardShock] - VVASEMath::NearestPointOnAxis(
			PointOnAxis, MomentDirection, CurrentCorner->Hardpoints[CORNER::OutboardShock]);
		Force = MomentDirection.Cross(MomentArm).Normalize() * MomentMagnitude / MomentArm.Length();

		// Determine the force required in the direction of the shock
		ForceDirection = (CurrentCorner->Hardpoints[CORNER::InboardShock]
		- CurrentCorner->Hardpoints[CORNER::OutboardShock]).Normalize();
		Force = ForceDirection * Force.Length() / (Force.Normalize() * ForceDirection);
		CornerDoubles[ShockInstallationRatio] = 1.0 / Force.Length()
			* VVASEMath::Sign(Force.Normalize() * ForceDirection.Normalize());
	}

	// Side View Swing Arm Length [in]
	// Find the plane that contains the wheel center and has the Y direction as a
	// normal, and find the intersection of the Instant Axis and that plane. This
	// vector's X-coordinate is the SVSA length.
	VECTOR PlaneNormal(0.0, 1.0, 0.0);
	VECTOR Intersection;

	// Get the intersection of the Instant Center with this plane
	Intersection = VVASEMath::IntersectWithPlane(PlaneNormal, CurrentCorner->Hardpoints[CORNER::WheelCenter],
		CornerVectors[InstantAxisDirection], CornerVectors[InstantCenter]);
	CornerDoubles[SideViewSwingArmLength] = fabs(Intersection.X);

	// We'll need this information when calculating the anti-geometry down below:
	double SideViewSwingArmHeight = Intersection.Z;

	// Front View Swing Arm Length [in]
	// The procedure is identical to finding the SVSA length, except the plane we intersect
	// has the X-axis as a normal and we use the Y-coordinate as the length.
	PlaneNormal.Set(1.0, 0.0, 0.0);

	// Get the intersection of the Instant Center with this plane
	Intersection = VVASEMath::IntersectWithPlane(PlaneNormal, CurrentCorner->Hardpoints[CORNER::WheelCenter],
		CornerVectors[InstantAxisDirection], CornerVectors[InstantCenter]);
	CornerDoubles[FrontViewSwingArmLength] = fabs(Intersection.Y);

	// Anti-brake [%]
	// Note that the equation changes if the brakes are inboard vs. outboard.  This
	// is because the way the torques are reacted is different:  In the case of outboard
	// brakes, the control arms are required to react some of the torque, whereas this
	// is not required when the brakes are inboard and the torqe can be reacted directly
	// by the chassis.  The nomenclature for the anti-geometry comes from RCVD p. 617.
	// NOTE:  We are required to know the sprung mass CG height and the wheelbase here.
	//        we assume that the static wheelbase and CG height are still accurate here (FIXME!!!)
	double Wheelbase = (CurrentCar->Suspension->RightRear.Hardpoints[CORNER::ContactPatch].X
		- CurrentCar->Suspension->RightFront.Hardpoints[CORNER::ContactPatch].X
		+ CurrentCar->Suspension->LeftRear.Hardpoints[CORNER::ContactPatch].X
		- CurrentCar->Suspension->LeftFront.Hardpoints[CORNER::ContactPatch].X) / 2.0;
	double CGHeight = CurrentCar->MassProperties->CenterOfGravity.Z;
	double ReactionPathAngleTangent;

	// Determine if we are talking about anti-dive (front) or anti-lift (rear)
	if (IsAtFront)
	{
		// Is the braking torque reacted directly by the chassis, or does it first
		// travel through the control arms?
		if (CurrentCar->Brakes->FrontBrakesInboard)
		{
			// Compute the tangent of the reaction path angle
			ReactionPathAngleTangent = (SideViewSwingArmHeight -
				CurrentCorner->Hardpoints[CORNER::WheelCenter].Z) / CornerDoubles[SideViewSwingArmLength];

			// Compute the anti-dive
			CornerDoubles[AntiBrakePitch] = ReactionPathAngleTangent * Wheelbase / CGHeight
				* CurrentCar->Brakes->PercentFrontBraking * 100.0;
		}
		else// Outboard brakes
		{
			// Compute the tangent of the reaction path angle
			ReactionPathAngleTangent = SideViewSwingArmHeight / CornerDoubles[SideViewSwingArmLength];

			// Compute the anti-dive
			CornerDoubles[AntiBrakePitch] = ReactionPathAngleTangent / (CGHeight / Wheelbase
				* CurrentCar->Brakes->PercentFrontBraking) * 100.0;
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
				CurrentCorner->Hardpoints[CORNER::WheelCenter].Z) / CornerDoubles[SideViewSwingArmLength];

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
			CurrentCorner->Hardpoints[CORNER::WheelCenter].Z) / CornerDoubles[SideViewSwingArmLength];

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
// Class:			KINEMATIC_OUTPUTS
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
wxString KINEMATIC_OUTPUTS::GetCornerDoubleName(const CORNER_OUTPUTS_DOUBLE &_Output)
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
// Class:			KINEMATIC_OUTPUTS
// Function:		GetCornerVectorName
//
// Description:		Returns a string containing the name of the specified output.
//
// Input Arguments:
//		_Output	= const CORNER_OUTPUTS_VECTOR& specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the specified output
//
//==========================================================================
wxString KINEMATIC_OUTPUTS::GetCornerVectorName(const CORNER_OUTPUTS_VECTOR &_Output)
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
// Class:			KINEMATIC_OUTPUTS
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
wxString KINEMATIC_OUTPUTS::GetDoubleName(const OUTPUTS_DOUBLE &_Output)
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
// Class:			KINEMATIC_OUTPUTS
// Function:		GetVectorName
//
// Description:		Returns a string containing the name of the specified output.
//
// Input Arguments:
//		_Output	= const OUTPUTS_VECTOR& specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the specified output
//
//==========================================================================
wxString KINEMATIC_OUTPUTS::GetVectorName(const OUTPUTS_VECTOR &_Output)
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
// Class:			KINEMATIC_OUTPUTS
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
double KINEMATIC_OUTPUTS::GetOutputValue(const OUTPUTS_COMPLETE &_Output) const
{
	// The return value
	double Value;

	// Temporary VECTOR for extracting componenets
	VECTOR Temp;

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
			Value = Temp.X;
			break;

		case 1:
			Value = Temp.Y;
			break;

		case 2:
		default:
			Value = Temp.Z;
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
			Value = Temp.X;
			break;

		case 1:
			Value = Temp.Y;
			break;

		case 2:
		default:
			Value = Temp.Z;
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
			Value = Temp.X;
			break;

		case 1:
			Value = Temp.Y;
			break;

		case 2:
		default:
			Value = Temp.Z;
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
			Value = Temp.X;
			break;

		case 1:
			Value = Temp.Y;
			break;

		case 2:
		default:
			Value = Temp.Z;
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
			Value = Temp.X;
			break;

		case 1:
			Value = Temp.Y;
			break;

		case 2:
		default:
			Value = Temp.Z;
			break;
		}
	}
	else
		Value = 0.0;

	return Value;
}

//==========================================================================
// Class:			KINEMATIC_OUTPUTS
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
//		CONVERT::UNIT_TYPE specifying the unit type of the requested output
//
//==========================================================================
CONVERT::UNIT_TYPE KINEMATIC_OUTPUTS::GetOutputUnitType(const OUTPUTS_COMPLETE &_Output)
{
	// The value to return
	CONVERT::UNIT_TYPE UnitType;

	// For some simple math
	OUTPUTS_COMPLETE NewOutputIndex;

	// Depending on the specified Output, choose the units string
	if (_Output <= EndRightFrontDoubles)
		UnitType = GetCornerDoubleUnitType((CORNER_OUTPUTS_DOUBLE)(_Output - StartRightFrontDoubles));
	else if (_Output <= EndRightFrontVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartRightFrontVectors);
		UnitType = GetCornerVectorUnitType((CORNER_OUTPUTS_VECTOR)int(NewOutputIndex / 3));
	}
	else if (_Output <= EndLeftFrontDoubles)
		UnitType = GetCornerDoubleUnitType((CORNER_OUTPUTS_DOUBLE)(_Output - StartLeftFrontDoubles));
	else if (_Output <= EndLeftFrontVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartLeftFrontVectors);
		UnitType = GetCornerVectorUnitType((CORNER_OUTPUTS_VECTOR)int(NewOutputIndex / 3));
	}
	else if (_Output <= EndRightRearDoubles)
		UnitType = GetCornerDoubleUnitType((CORNER_OUTPUTS_DOUBLE)(_Output - StartRightRearDoubles));
	else if (_Output <= EndRightRearVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartRightRearVectors);
		UnitType = GetCornerVectorUnitType((CORNER_OUTPUTS_VECTOR)int(NewOutputIndex / 3));
	}
	else if (_Output <= EndLeftRearDoubles)
		UnitType = GetCornerDoubleUnitType((CORNER_OUTPUTS_DOUBLE)(_Output - StartLeftRearDoubles));
	else if (_Output <= EndLeftRearVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartLeftRearVectors);
		UnitType = GetCornerVectorUnitType((CORNER_OUTPUTS_VECTOR)int(NewOutputIndex / 3));
	}
	else if (_Output <= EndDoubles)
		UnitType = GetDoubleUnitType((OUTPUTS_DOUBLE)(_Output - StartDoubles));
	else if (_Output <= EndVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE(_Output - StartVectors);
		UnitType = GetVectorUnitType((OUTPUTS_VECTOR)int(NewOutputIndex / 3));
	}
	else
		UnitType = CONVERT::UNIT_TYPE_UNKNOWN;

	return UnitType;
}

//==========================================================================
// Class:			KINEMATIC_OUTPUTS
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
wxString KINEMATIC_OUTPUTS::GetOutputName(const OUTPUTS_COMPLETE &_Output)
{
	// The value to return
	wxString Name;

	// For some simple math
	OUTPUTS_COMPLETE NewOutputIndex;

	// Depending on the specified PLOT_ID, choose the name of the string
	// Vectors are a special case - depending on which componenet of the vector is chosen,
	// we need to append a different string to the end of the Name
	if (_Output <= EndRightFrontDoubles)
	{
		Name = GetCornerDoubleName((CORNER_OUTPUTS_DOUBLE)(_Output - StartRightFrontDoubles));
		Name.Prepend(_T("Right Front "));
	}
	else if (_Output <= EndRightFrontVectors)
	{
		NewOutputIndex = OUTPUTS_COMPLETE((int)_Output - (int)StartRightFrontVectors);
		Name = GetCornerVectorName((CORNER_OUTPUTS_VECTOR)int(NewOutputIndex / 3));

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
		Name = GetCornerVectorName((CORNER_OUTPUTS_VECTOR)int(NewOutputIndex / 3));

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
		Name = GetCornerVectorName((CORNER_OUTPUTS_VECTOR)int(NewOutputIndex / 3));

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
		Name = GetCornerVectorName((CORNER_OUTPUTS_VECTOR)int(NewOutputIndex / 3));

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
		Name = GetVectorName((OUTPUTS_VECTOR)int(NewOutputIndex / 3));

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
// Class:			KINEMATIC_OUTPUTS
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
//		CONVERT::UNIT_TYPE describing the units of the specified output
//
//==========================================================================
CONVERT::UNIT_TYPE KINEMATIC_OUTPUTS::GetCornerDoubleUnitType(const CORNER_OUTPUTS_DOUBLE &_Output)
{
	// The return value
	CONVERT::UNIT_TYPE UnitType;

	// Determine the units based on it's type
	switch (_Output)
	{
		// Angles
	case Caster:
	case Camber:
	case KPI:
	case Steer:
		UnitType = CONVERT::UNIT_TYPE_ANGLE;
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
		UnitType = CONVERT::UNIT_TYPE_DISTANCE;
		break;

		// Unitless
	case SpringInstallationRatio:
	case ShockInstallationRatio:
	case AntiBrakePitch:
	case AntiDrivePitch:
		UnitType = CONVERT::UNIT_TYPE_UNITLESS;
		break;

		// Unknown
	default:
		UnitType = CONVERT::UNIT_TYPE_UNKNOWN;
		break;
	}

	return UnitType;
}

//==========================================================================
// Class:			KINEMATIC_OUTPUTS
// Function:		GetCornerVectorUnitType
//
// Description:		Returns a the type of units for the specified output.
//
// Input Arguments:
//		_Output	= const CORNER_OUTPUTS_VECTOR& specifying the
//				  output in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		CONVERT::UNIT_TYPE describing the units of the specified output
//
//==========================================================================
CONVERT::UNIT_TYPE KINEMATIC_OUTPUTS::GetCornerVectorUnitType(const CORNER_OUTPUTS_VECTOR &_Output)
{
	// The return value
	CONVERT::UNIT_TYPE UnitType;

	// Determine the units based on it's type
	switch (_Output)
	{
		// Distances
	case InstantCenter:
		UnitType = CONVERT::UNIT_TYPE_DISTANCE;
		break;

		// Unitless (no conversion)
	case InstantAxisDirection:
		UnitType = CONVERT::UNIT_TYPE_UNITLESS;
		break;

		// Unknown
	default:
		UnitType = CONVERT::UNIT_TYPE_UNKNOWN;
		break;
	}

	return UnitType;
}

//==========================================================================
// Class:			KINEMATIC_OUTPUTS
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
//		CONVERT::UNIT_TYPE describing the units of the specified output
//
//==========================================================================
CONVERT::UNIT_TYPE KINEMATIC_OUTPUTS::GetDoubleUnitType(const OUTPUTS_DOUBLE &_Output)
{
	// The return value
	CONVERT::UNIT_TYPE UnitType;

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
		UnitType = CONVERT::UNIT_TYPE_DISTANCE;
		break;

		// Angles
	case FrontARBTwist:
	case RearARBTwist:
	case FrontNetSteer:
	case RearNetSteer:
		UnitType = CONVERT::UNIT_TYPE_ANGLE;
		break;

		// Unitless (no conversion)
	case FrontARBMotionRatio:
	case RearARBMotionRatio:
		UnitType = CONVERT::UNIT_TYPE_UNITLESS;
		break;

		// Unknown units
	default:
		UnitType = CONVERT::UNIT_TYPE_UNKNOWN;
		break;
	}

	return UnitType;
}

//==========================================================================
// Class:			KINEMATIC_OUTPUTS
// Function:		GetVectorUnitType
//
// Description:		Returns a the type of units for the specified output.
//
// Input Arguments:
//		_Output	= const OUTPUTS_VECTOR& specifying the
//				  output in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		CONVERT::UNIT_TYPE describing the units of the specified output
//
//==========================================================================
CONVERT::UNIT_TYPE KINEMATIC_OUTPUTS::GetVectorUnitType(const OUTPUTS_VECTOR &_Output)
{
	// The return value
	CONVERT::UNIT_TYPE UnitType;

	// Determine the units based on it's type
	switch (_Output)
	{
		// Distances
	case FrontKinematicRC:
	case RearKinematicRC:
	case RightKinematicPC:
	case LeftKinematicPC:
		UnitType = CONVERT::UNIT_TYPE_DISTANCE;
		break;

		// Unitless
	case FrontRollAxisDirection:
	case RearRollAxisDirection:
	case RightPitchAxisDirection:
	case LeftPitchAxisDirection:
		UnitType = CONVERT::UNIT_TYPE_UNITLESS;
		break;

		// Unknown
	default:
		UnitType = CONVERT::UNIT_TYPE_UNKNOWN;
		break;
	}

	return UnitType;
}

//==========================================================================
// Class:			KINEMATIC_OUTPUTS
// Function:		OutputsCompleteIndex
//
// Description:		Returns an index refering to the list of OUTPUTS_COMPLETE
//					for the specified output.
//
// Input Arguments:
//		Location		= const &CORNER::Location specifying the relevant corner of the car
//		CornerDouble	= const &CORNER_OUTPUTS_DOUBLE
//		CornerVector	= const &CORNER_OUTPUTS_VECTOR
//		Double			= const &OUTPUTS_DOUBLE
//		Vector			= const &OUTPUTS_VECTOR
//		Axis			= const &VECTOR::AXIS
//
// Output Arguments:
//		None
//
// Return Value:
//		OUTPUTS_COMPLETE describing the index for the specified output
//
//==========================================================================
KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE KINEMATIC_OUTPUTS::OutputsCompleteIndex(const CORNER::LOCATION &Location,
																			const CORNER_OUTPUTS_DOUBLE &CornerDouble,
																			const CORNER_OUTPUTS_VECTOR &CornerVector,
																			const OUTPUTS_DOUBLE &Double,
																			const OUTPUTS_VECTOR &Vector,
																			const VECTOR::AXIS &Axis)
{
	OUTPUTS_COMPLETE CompleteIndex = NumberOfOutputScalars;

	// Switch based on the corner
	switch (Location)
	{
	case CORNER::LocationLeftFront:
		if (CornerDouble != NumberOfCornerOutputDoubles)
			CompleteIndex = (KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)(StartLeftFrontDoubles + CornerDouble);
		else
			CompleteIndex = (KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)(StartLeftFrontVectors + CornerVector * 3 + Axis);
		break;

	case CORNER::LocationRightFront:
		if (CornerDouble != NumberOfCornerOutputDoubles)
			CompleteIndex = (KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)(StartRightFrontDoubles + CornerDouble);
		else
			CompleteIndex = (KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)(StartRightFrontVectors + CornerVector * 3 + Axis);
		break;

	case CORNER::LocationLeftRear:
		if (CornerDouble != NumberOfCornerOutputDoubles)
			CompleteIndex = (KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)(StartLeftRearDoubles + CornerDouble);
		else
			CompleteIndex = (KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)(StartLeftRearVectors + CornerVector * 3 + Axis);
		break;

	case CORNER::LocationRightRear:
		if (CornerDouble != NumberOfCornerOutputDoubles)
			CompleteIndex = (KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)(StartRightRearDoubles + CornerDouble);
		else
			CompleteIndex = (KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)(StartRightRearVectors + CornerVector * 3 + Axis);
		break;

	default:// Not a corner output
		if (Double != NumberOfOutputDoubles)
			CompleteIndex = (KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)(StartDoubles + Double);
		else
			CompleteIndex = (KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)(StartVectors + Vector * 3 + Axis);
		break;
	}

	return CompleteIndex;
}