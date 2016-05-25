/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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
//	3/24/2008	- Created CornerOutputs structure, K. Loux.
//	2/16/2009	- Changed Corner and Suspension to use enumeration style array of points instead
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
#include "vCar/massProperties.h"
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
void KinematicOutputs::InitializeAllOutputs()
{
	int i;

	for (i = 0; i < NumberOfCornerOutputDoubles; i++)
	{
		rightFront[i] = VVASEMath::QNAN;
		leftFront[i] = VVASEMath::QNAN;
		rightRear[i] = VVASEMath::QNAN;
		leftRear[i] = VVASEMath::QNAN;
	}

	for (i = 0; i< NumberOfCornerOutputVectors; i++)
	{
		rightFrontVectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
		leftFrontVectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
		rightRearVectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
		leftRearVectors[i].Set(VVASEMath::QNAN, VVASEMath::QNAN, VVASEMath::QNAN);
	}

	for (i = 0; i < NumberOfOutputDoubles; i++)
		doubles[i] = VVASEMath::QNAN;

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
//		original	= const Car*, pointing to object describing the car's
//					  un-perturbed state
//		current		= const Suspension*, pointing to object describing the new
//					  state of the car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::Update(const Car *original, const Suspension *current)
{
	originalCar = original;// TODO:  Can we remove these at the class level?  Only used within UpdateCorner
	currentSuspension = current;// TODO:  Can we remove these at the class level?  Only used within UpdateCorner

	InitializeAllOutputs();

	// Update each corner's outputs
	UpdateCorner(&original->suspension->rightFront, &current->rightFront);
	UpdateCorner(&original->suspension->leftFront, &current->leftFront);
	UpdateCorner(&original->suspension->rightRear, &current->rightRear);
	UpdateCorner(&original->suspension->leftRear, &current->leftRear);

	ComputeNetSteer();
	ComputeNetScrub();

	ComputeFrontARBTwist(original, current);
	ComputeRearARBTwist(original, current);

	ComputeFrontRollCenter(current);
	ComputeRearRollCenter(current);

	ComputeLeftPitchCenter(current);
	ComputeRightPitchCenter(current);

	ComputeTrack(current);
	ComputeWheelbase(current);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeNetSteer
//
// Description:		Computes the net steer values.
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
void KinematicOutputs::ComputeNetSteer()
{
	doubles[FrontNetSteer] = rightFront[Steer] - leftFront[Steer];
	doubles[RearNetSteer] = rightRear[Steer] - leftRear[Steer];
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeNetScrub
//
// Description:		Computes the net scrub values.
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
void KinematicOutputs::ComputeNetScrub()
{
	doubles[FrontNetScrub] = rightFront[Scrub] + leftFront[Scrub];
	doubles[RearNetScrub] = rightRear[Scrub] + leftRear[Scrub];
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeFrontARBTwist
//
// Description:		Calls the method to compute front ARB twist.
//
// Input Arguments:
//		original	= const Car*
//		current		= const Suspension*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeFrontARBTwist(const Car *original, const Suspension *current)
{
	doubles[FrontARBTwist] = ComputeARBTwist(original->suspension->leftFront,
		original->suspension->rightFront, current->leftFront, current->rightFront, current->frontBarStyle,
		original->suspension->hardpoints[Suspension::FrontBarMidPoint],
		original->suspension->hardpoints[Suspension::FrontBarPivotAxis],
		current->hardpoints[Suspension::FrontBarMidPoint], current->hardpoints[Suspension::FrontBarPivotAxis]);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeRearARBTwist
//
// Description:		Calls the method to compute front ARB twist.
//
// Input Arguments:
//		original	= const Car*
//		current		= const Suspension*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeRearARBTwist(const Car *original, const Suspension *current)
{
	doubles[RearARBTwist] = ComputeARBTwist(original->suspension->leftRear,
		original->suspension->rightRear, current->leftRear, current->rightRear, current->rearBarStyle,
		original->suspension->hardpoints[Suspension::RearBarMidPoint],
		original->suspension->hardpoints[Suspension::RearBarPivotAxis],
		current->hardpoints[Suspension::RearBarMidPoint], current->hardpoints[Suspension::RearBarPivotAxis]);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeFrontRollCenter
//
// Description:		Calls the method to calculate the kinematic center.
//
// Input Arguments:
//		current	= const Suspension*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeFrontRollCenter(const Suspension *current)
{
	Vector normal(1.0, 0.0, 0.0);
	if (!ComputeKinematicCenter(current->leftFront, current->rightFront, leftFrontVectors,
		rightFrontVectors, normal, vectors[FrontKinematicRC], vectors[FrontRollAxisDirection]))
		Debugger::GetInstance() << "Warning:  Front Kinematic Roll Center is undefined" << Debugger::PriorityHigh;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeRearRollCenter
//
// Description:		Calls the method to calculate the kinematic center.
//
// Input Arguments:
//		current	= const Suspension*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeRearRollCenter(const Suspension *current)
{
	Vector normal(1.0, 0.0, 0.0);
	if (!ComputeKinematicCenter(current->leftRear, current->rightRear, leftRearVectors,
		rightRearVectors, normal, vectors[RearKinematicRC], vectors[RearRollAxisDirection]))
		Debugger::GetInstance() << "Warning:  Rear Kinematic Roll Center is undefined" << Debugger::PriorityHigh;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeLeftPitchCenter
//
// Description:		Calls the method to calculate the kinematic center.
//
// Input Arguments:
//		current	= const Suspension*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeLeftPitchCenter(const Suspension *current)
{
	Vector normal(0.0, 1.0, 0.0);
	ComputeKinematicCenter(current->leftFront, current->leftRear, leftFrontVectors,
		leftRearVectors, normal, vectors[LeftKinematicPC], vectors[LeftPitchAxisDirection]);// No warning - undefined PCs is OK?

	// For the left side, we flip the sign on the axis direction
	vectors[LeftPitchAxisDirection] *= -1.0;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeRightPitchCenter
//
// Description:		Calls the method to calculate the kinematic center.
//
// Input Arguments:
//		current	= const Suspension*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeRightPitchCenter(const Suspension *current)
{
	Vector normal(0.0, 1.0, 0.0);
	ComputeKinematicCenter(current->rightFront, current->rightRear, rightFrontVectors,
		rightRearVectors, normal, vectors[RightKinematicPC], vectors[RightPitchAxisDirection]);// No warning - undefined PCs is OK?
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeTrack
//
// Description:		Computes ground and hub track values.
//
// Input Arguments:
//		current	= const Suspension*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeTrack(const Suspension *current)
{
	doubles[FrontTrackGround] = current->rightFront.hardpoints[Corner::ContactPatch].Distance(
		current->leftFront.hardpoints[Corner::ContactPatch]);
	doubles[RearTrackGround] = current->rightRear.hardpoints[Corner::ContactPatch].Distance(
		current->leftRear.hardpoints[Corner::ContactPatch]);
	doubles[FrontTrackHub] = current->rightFront.hardpoints[Corner::WheelCenter].Distance(
		current->leftFront.hardpoints[Corner::WheelCenter]);
	doubles[RearTrackHub] = current->rightRear.hardpoints[Corner::WheelCenter].Distance(
		current->leftRear.hardpoints[Corner::WheelCenter]);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeWheelbase
//
// Description:		Computes ground and hub wheelbase values.
//
// Input Arguments:
//		current	= const Suspension*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeWheelbase(const Suspension *current)
{
	doubles[RightWheelbaseGround] = current->rightFront.hardpoints[Corner::ContactPatch].Distance(
		current->rightRear.hardpoints[Corner::ContactPatch]);
	doubles[LeftWheelbaseGround] = current->leftFront.hardpoints[Corner::ContactPatch].Distance(
		current->leftRear.hardpoints[Corner::ContactPatch]);
	doubles[RightWheelbaseHub] = current->rightFront.hardpoints[Corner::WheelCenter].Distance(
		current->rightRear.hardpoints[Corner::WheelCenter]);
	doubles[LeftWheelbaseHub] = current->leftFront.hardpoints[Corner::WheelCenter].Distance(
		current->leftRear.hardpoints[Corner::WheelCenter]);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeARBTwist
//
// Description:		Calls proper method for calculating ARB twist based on
//					bar style.
//
// Input Arguments:
//		originalLeft		= const Corner&
//		originalRight		= const Corner&
//		currentLeft			= const Corner&
//		currentRight		= const Corner&
//		barStyle			= const Suspension::BarStyle&
//		originalMidPoint	= const Vector&
//		originalPivot		= const Vector&
//		currentMidPoint		= const Vector&
//		currentPivot		= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		double [rad]
//
//==========================================================================
double KinematicOutputs::ComputeARBTwist(const Corner& originalLeft,
	const Corner& originalRight, const Corner& currentLeft,
	const Corner& currentRight, const Suspension::BarStyle &barStyle,
	const Vector& originalMidPoint, const Vector& originalPivot,
	const Vector& currentMidPoint, const Vector& currentPivot) const
{
	if (barStyle == Suspension::SwayBarUBar)
		return ComputeUBarTwist(originalLeft, originalRight, currentLeft, currentRight);
	else if (barStyle == Suspension::SwayBarTBar)
		return ComputeTBarTwist(originalLeft, originalRight, currentLeft, currentRight,
		originalMidPoint, originalPivot, currentMidPoint, currentPivot);
	else if (barStyle == Suspension::SwayBarGeared)
		return ComputeGearedBarTwist(originalLeft, originalRight, currentLeft, currentRight);

	return 0.0;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeUBarTwist
//
// Description:		Computes ARB twist for U-bars.
//
// Input Arguments:
//		originalLeft		= const Corner&
//		originalRight		= const Corner&
//		currentLeft			= const Corner&
//		currentRight		= const Corner&
//
// Output Arguments:
//		None
//
// Return Value:
//		double [rad]
//
//==========================================================================
double KinematicOutputs::ComputeUBarTwist(const Corner& originalLeft,
	const Corner& originalRight, const Corner& currentLeft, const Corner& currentRight) const
{
	Vector swayBarAxis;
	Vector arm1Direction, arm2Direction;
	double originalSwayBarAngle;

	// First, for the original configuration of the suspension
	// Project these directions onto the plane whose normal is the sway bar axis
	swayBarAxis = originalRight.hardpoints[Corner::BarArmAtPivot] -
		originalLeft.hardpoints[Corner::BarArmAtPivot];

	// The references for U-bar twist are the arms at the end of the bar
	arm1Direction = VVASEMath::ProjectOntoPlane(originalRight.hardpoints[Corner::BarArmAtPivot] -
		originalRight.hardpoints[Corner::InboardBarLink], swayBarAxis);
	arm2Direction = VVASEMath::ProjectOntoPlane(originalLeft.hardpoints[Corner::BarArmAtPivot] -
		originalLeft.hardpoints[Corner::InboardBarLink], swayBarAxis);

	// The angle between these vectors, when projected onto the plane that is normal
	// to the swaybar axis is given by the dot product
	originalSwayBarAngle = acos(VVASEMath::Clamp((arm1Direction * arm2Direction) /
		(arm1Direction.Length() * arm2Direction.Length()), -1.0, 1.0));

	// And again as it sits now
	// Project these directions onto the plane whose normal is the sway bar axis
	swayBarAxis = currentRight.hardpoints[Corner::BarArmAtPivot] -
		currentLeft.hardpoints[Corner::BarArmAtPivot];

	// The references for U-bar twist are the arms at the end of the bar
	arm1Direction = VVASEMath::ProjectOntoPlane(currentRight.hardpoints[Corner::BarArmAtPivot] -
		currentRight.hardpoints[Corner::InboardBarLink], swayBarAxis);
	arm2Direction = VVASEMath::ProjectOntoPlane(currentLeft.hardpoints[Corner::BarArmAtPivot] -
		currentLeft.hardpoints[Corner::InboardBarLink], swayBarAxis);

	// The angle between these vectors, when projected onto the plane that is normal
	// to the swaybar axis is given by the dot product
	return acos(VVASEMath::Clamp((arm1Direction * arm2Direction) /
		(arm1Direction.Length() * arm2Direction.Length()), -1.0, 1.0)) - originalSwayBarAngle;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeTBarTwist
//
// Description:		Computes ARB twist for T-bars.
//
// Input Arguments:
//		originalLeft		= const Corner&
//		originalRight		= const Corner&
//		currentLeft			= const Corner&
//		currentRight		= const Corner&
//		originalMidPoint	= const Vector&
//		originalPivot		= const Vector&
//		currentMidPoint		= const Vector&
//		currentPivot		= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		double [rad]
//
//==========================================================================
double KinematicOutputs::ComputeTBarTwist(const Corner& originalLeft,
	const Corner& originalRight, const Corner& currentLeft, const Corner& currentRight,
	const Vector& originalMidPoint, const Vector& originalPivot,
	const Vector& currentMidPoint, const Vector& currentPivot) const
{
	Vector swayBarAxis;
	Vector arm1Direction, arm2Direction;
	double originalSwayBarAngle;

	// First, for the original configuration of the suspension
	Vector stemPlaneNormal = originalMidPoint - originalPivot;
	Vector topMidPoint = VVASEMath::IntersectWithPlane(stemPlaneNormal,
		originalMidPoint, originalLeft.hardpoints[Corner::InboardBarLink]
			- originalRight.hardpoints[Corner::InboardBarLink],
		originalLeft.hardpoints[Corner::InboardBarLink]);

	// Project these directions onto the plane whose normal is the sway bar axis
	swayBarAxis = originalMidPoint - topMidPoint;

	// The references for T-bar twist are the bar pivot axis and the top arm
	arm1Direction = VVASEMath::ProjectOntoPlane(topMidPoint -
		originalRight.hardpoints[Corner::InboardBarLink], swayBarAxis);

	// The angle between these vectors, when projected onto the plane that is normal
	// to the swaybar axis is given by the dot product
	originalSwayBarAngle = acos(VVASEMath::Clamp((arm1Direction * stemPlaneNormal) /
		(arm1Direction.Length() * stemPlaneNormal.Length()), -1.0, 1.0));

	// And again as it sits now
	stemPlaneNormal = currentMidPoint - currentPivot;
	topMidPoint = VVASEMath::IntersectWithPlane(stemPlaneNormal,
		currentMidPoint, currentLeft.hardpoints[Corner::InboardBarLink]
			- currentRight.hardpoints[Corner::InboardBarLink],
		currentLeft.hardpoints[Corner::InboardBarLink]);

	// Project these directions onto the plane whose normal is the sway bar axis
	swayBarAxis = currentMidPoint - topMidPoint;

	// The references for T-bar twist are the bar pivot axis and the top arm
	arm1Direction = VVASEMath::ProjectOntoPlane(topMidPoint -
		currentRight.hardpoints[Corner::InboardBarLink], swayBarAxis);

	// The angle between these vectors, when projected onto the plane that is normal
	// to the swaybar axis is given by the dot product
	return acos(VVASEMath::Clamp((arm1Direction * stemPlaneNormal) /
		(arm1Direction.Length() * stemPlaneNormal.Length()), -1.0, 1.0)) - originalSwayBarAngle;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeGearedBarTwist
//
// Description:		Computes ARB twist for geared bars.
//
// Input Arguments:
//		originalLeft		= const Corner&
//		originalRight		= const Corner&
//		currentLeft			= const Corner&
//		currentRight		= const Corner&
//
// Output Arguments:
//		None
//
// Return Value:
//		double [rad]
//
//==========================================================================
double KinematicOutputs::ComputeGearedBarTwist(const Corner& /*originalLeft*/,
	const Corner& /*originalRight*/, const Corner& /*currentLeft*/, const Corner& /*currentRight*/) const
{
	// TODO:  Impelement
	Debugger::GetInstance() << "Geared ARB calculations not yet implemented" << Debugger::PriorityMedium;
	return 0.0;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeKinematicCenter
//
// Description:		Calculates the specified kinematic center.
//
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
//
// Input Arguments:
//		corner1			= const Corner&
//		corner2			= const Corner&
//		cornerVectors1	= const Vector*
//		cornerVectors2	= const Vector*
//		planeNormal		= const Vector&
//
// Output Arguments:
//		center			= Vector&
//		direction		= Vector&
//
// Return Value:
//		None
//
//==========================================================================
bool KinematicOutputs::ComputeKinematicCenter(const Corner &corner1, const Corner &corner2,
	const Vector *cornerVectors1, const Vector *cornerVectors2,
	const Vector &planeNormal, Vector &center, Vector &direction) const
{
	Vector normal1, normal2;

	normal1 = VVASEMath::GetPlaneNormal(corner1.hardpoints[Corner::ContactPatch], cornerVectors1[InstantCenter],
		cornerVectors1[InstantCenter] + cornerVectors1[InstantAxisDirection]);
	normal2 = VVASEMath::GetPlaneNormal(corner2.hardpoints[Corner::ContactPatch], cornerVectors2[InstantCenter],
		cornerVectors2[InstantCenter] + cornerVectors2[InstantAxisDirection]);

	if (!VVASEMath::GetIntersectionOfTwoPlanes(normal2, corner2.hardpoints[Corner::ContactPatch],
		normal1, corner1.hardpoints[Corner::ContactPatch], direction, center))
		return false;

	// We now have the axis direction and a point on the axis, but we want a specific
	// point on the axis.  To do that, we determine the place where this vector passes through
	// the appropriate plane.
	center = VVASEMath::IntersectWithPlane(planeNormal, (corner2.hardpoints[Corner::WheelCenter] +
		corner1.hardpoints[Corner::WheelCenter]) * 0.5, direction, center);

	return true;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		UpdateCorner
//
// Description:		Performs measurements that are repeated for every wheel
//					on the car.
//
// Input Arguments:
//		originalCorner	= const Corner*, pointer to the un-perturbed state of the
//						  corner
//		currentCorner	= const Corner*, pointer to the current state of the corner
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::UpdateCorner(const Corner *originalCorner, const Corner *currentCorner)
{
	// Assign pointers to the corner outputs and our sign conventions
	double *cornerDoubles;
	Vector *cornerVectors;
	short sign;
	bool isAtFront = false;

	if (originalCorner->location == Corner::LocationRightFront)
	{
		cornerDoubles = rightFront;
		cornerVectors = rightFrontVectors;
		sign = 1;
		isAtFront = true;
	}
	else if (originalCorner->location == Corner::LocationLeftFront)
	{
		cornerDoubles = leftFront;
		cornerVectors = leftFrontVectors;
		sign = -1;
		isAtFront = true;
	}
	else if (originalCorner->location == Corner::LocationRightRear)
	{
		cornerDoubles = rightRear;
		cornerVectors = rightRearVectors;
		sign = 1;
	}
	else if (originalCorner->location == Corner::LocationLeftRear)
	{
		cornerDoubles = leftRear;
		cornerVectors = leftRearVectors;
		sign = -1;
	}
	else
	{
		Debugger::GetInstance() << "ERROR:  Corner location not recognized!" << Debugger::PriorityHigh;
		return;
	}

	ComputeCaster(*currentCorner, cornerDoubles);
	ComputeKingPinInclincation(*currentCorner, sign, cornerDoubles);
	ComputeCasterTrail(*currentCorner, cornerDoubles);
	ComputeScrubRadius(*currentCorner, sign, cornerDoubles);
	ComputeSpindleLength(*currentCorner, sign, cornerDoubles);
	ComputeCamberAndSteer(*originalCorner, *currentCorner, sign, cornerDoubles);
	ComputeSpringDisplacement(*originalCorner, *currentCorner, cornerDoubles);
	ComputeDamperDisplacement(*originalCorner, *currentCorner, cornerDoubles);
	ComputeScrub(*originalCorner, *currentCorner, sign, cornerDoubles);

	if ((originalCar->HasFrontHalfShafts() && isAtFront) ||
		(originalCar->HasRearHalfShafts() && !isAtFront))
		ComputeAxlePlunge(*originalCorner, *currentCorner, cornerDoubles);

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

	upperPlaneNormal = VVASEMath::GetPlaneNormal(currentCorner->hardpoints[Corner::UpperBallJoint],
		currentCorner->hardpoints[Corner::UpperFrontTubMount], currentCorner->hardpoints[Corner::UpperRearTubMount]);
	lowerPlaneNormal = VVASEMath::GetPlaneNormal(currentCorner->hardpoints[Corner::LowerBallJoint],
		currentCorner->hardpoints[Corner::LowerFrontTubMount], currentCorner->hardpoints[Corner::LowerRearTubMount]);

	if (!VVASEMath::GetIntersectionOfTwoPlanes(upperPlaneNormal, currentCorner->hardpoints[Corner::UpperBallJoint],
		lowerPlaneNormal, currentCorner->hardpoints[Corner::LowerBallJoint], 
		cornerVectors[InstantAxisDirection], cornerVectors[InstantCenter]))
		Debugger::GetInstance() << "Warning (KinematicOutputs::UpdateCorner):  Instant Center is undefined" << Debugger::PriorityHigh;
	else
	{
		// We now have the axis direction and a point on the axis, but we want a specific
		// point on the axis.  To do that, we determine the place where this vector passes through
		// the appropriate plane.
		Vector planeNormal(1.0, 0.0, 0.0);

		cornerVectors[InstantCenter] = VVASEMath::IntersectWithPlane(planeNormal, currentCorner->hardpoints[Corner::WheelCenter],
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
	// ARB Installation Ratio [rad bar twist/inches Wheel]
	// TODO:  Why wheel center and not contact patch?
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
	if (currentCorner->actuationType == Corner::ActuationPushPullrod)
	{
		if (currentCorner->actuationAttachment == Corner::AttachmentLowerAArm)
		{
			// Identify the rotation axis and a point on that axis
			pointOnAxis = cornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			momentArm = currentCorner->hardpoints[Corner::WheelCenter] - pointOnAxis;

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
			momentArm = currentCorner->hardpoints[Corner::LowerBallJoint] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->hardpoints[Corner::LowerBallJoint]);

			// Calculate the force acting on the lower ball joint
			force =  momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

			// The next step is calculating the force through the pushrod
			// Again, find the pivot axis and moment arm.
			pointOnAxis = currentCorner->hardpoints[Corner::LowerFrontTubMount];
			momentArm = currentCorner->hardpoints[Corner::LowerBallJoint] - pointOnAxis;
			momentDirection = currentCorner->hardpoints[Corner::LowerRearTubMount] - pointOnAxis;

			// Given the above information, we can calculate the moment magnitude in the same manner
			// as we did previously
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();

			// Again, calculate the force required of the pushrod on the lower A-arm to create the moment
			momentArm = currentCorner->hardpoints[Corner::OutboardPushrod] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->hardpoints[Corner::OutboardPushrod]);
			force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();
		}
		else if (currentCorner->actuationAttachment == Corner::AttachmentUpperAArm)
		{
			// Identify the rotation axis and a point on that axis
			pointOnAxis = cornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			momentArm = currentCorner->hardpoints[Corner::WheelCenter] - pointOnAxis;

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
			momentArm = currentCorner->hardpoints[Corner::UpperBallJoint] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->hardpoints[Corner::UpperBallJoint]);

			// Calculate the force acting on the upper ball joint
			force =  momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

			// The next step is calculating the force through the pullrod
			// Again, find the pivot axis and moment arm.
			pointOnAxis = currentCorner->hardpoints[Corner::UpperFrontTubMount];
			momentArm = currentCorner->hardpoints[Corner::UpperBallJoint] - pointOnAxis;
			momentDirection = currentCorner->hardpoints[Corner::UpperRearTubMount] - pointOnAxis;

			// Given the above information, we can calculate the moment magnitude in the same manner
			// as we did previously
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();

			// Again, calculate the force required of the pushrod on the upper A-arm to create the moment
			momentArm = currentCorner->hardpoints[Corner::OutboardPushrod] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->hardpoints[Corner::OutboardPushrod]);
			force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();
		}
		else if (currentCorner->actuationAttachment == Corner::AttachmentUpright)
		{
			// Identify the rotation axis and a point on that axis
			pointOnAxis = cornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			momentArm = currentCorner->hardpoints[Corner::WheelCenter] - pointOnAxis;

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
			momentArm = currentCorner->hardpoints[Corner::OutboardPushrod] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->hardpoints[Corner::OutboardPushrod]);

			// Calculate the force acting on the pushrod
			force =  momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();
		}

		// After computing the force through the push/pullrod, the procedure is the same regardless
		// of how the suspension is configured.

		// The force from the pushrod creating the moment is not the only portion of the pushrod force.
		// The next step is to compute the total force through the pushrod.
		forceDirection = (currentCorner->hardpoints[Corner::InboardPushrod]
			- currentCorner->hardpoints[Corner::OutboardPushrod]).Normalize();
		force = forceDirection * force.Length() / (force.Normalize() * forceDirection);

		// Now we can calculate the forces acting on the shock and spring from the rotation of the
		// bellcrank about its axis.
		pointOnAxis = currentCorner->hardpoints[Corner::BellCrankPivot1];
		momentArm = currentCorner->hardpoints[Corner::InboardPushrod] - pointOnAxis;
		momentDirection = currentCorner->hardpoints[Corner::BellCrankPivot2] - pointOnAxis;
		momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();

		// Spring force
		momentArm = currentCorner->hardpoints[Corner::OutboardSpring] - VVASEMath::NearestPointOnAxis(
			pointOnAxis, momentDirection, currentCorner->hardpoints[Corner::OutboardSpring]);
		force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

		// Determine the force required in the direction of the spring (like we did for the force
		// through the push/pullrod)
		forceDirection = (currentCorner->hardpoints[Corner::InboardSpring]
			- currentCorner->hardpoints[Corner::OutboardSpring]).Normalize();
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
		momentArm = currentCorner->hardpoints[Corner::OutboardDamper] - VVASEMath::NearestPointOnAxis(
			pointOnAxis, momentDirection, currentCorner->hardpoints[Corner::OutboardDamper]);
		force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

		// Determine the force required in the direction of the damper
		forceDirection = (currentCorner->hardpoints[Corner::InboardDamper]
			- currentCorner->hardpoints[Corner::OutboardDamper]).Normalize();
		force = forceDirection * force.Length() / (force.Normalize() * forceDirection);
		cornerDoubles[DamperInstallationRatio] = 1.0 / force.Length()
			* VVASEMath::Sign(force.Normalize() * forceDirection.Normalize());

		// ARB link force
		if (((originalCorner->location == Corner::LocationLeftFront ||
			originalCorner->location == Corner::LocationRightFront) &&
			originalCar->suspension->frontBarStyle != Suspension::SwayBarNone) ||
			((originalCorner->location == Corner::LocationLeftRear ||
			originalCorner->location == Corner::LocationRightRear) &&
			originalCar->suspension->rearBarStyle != Suspension::SwayBarNone))
		{
			momentArm = currentCorner->hardpoints[Corner::OutboardBarLink] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->hardpoints[Corner::OutboardBarLink]);
			force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

			// Determine the force required in the direction of the link
			forceDirection = (currentCorner->hardpoints[Corner::InboardBarLink]
				- currentCorner->hardpoints[Corner::OutboardBarLink]).Normalize();
			force = forceDirection * force.Length() / (force.Normalize() * forceDirection);

			// Different procedures for U-bars and T-bars
			if (((originalCorner->location == Corner::LocationLeftFront ||
				originalCorner->location == Corner::LocationRightFront) &&
				originalCar->suspension->frontBarStyle == Suspension::SwayBarUBar) ||
				((originalCorner->location == Corner::LocationLeftRear ||
				originalCorner->location == Corner::LocationRightRear) &&
				originalCar->suspension->rearBarStyle == Suspension::SwayBarUBar))
			{
				if (currentCorner->location == Corner::LocationLeftFront ||
					currentCorner->location == Corner::LocationRightFront)
					momentDirection = currentCorner->hardpoints[Corner::BarArmAtPivot] - currentSuspension->hardpoints[Suspension::FrontBarMidPoint];
				else
					momentDirection = currentCorner->hardpoints[Corner::BarArmAtPivot] - currentSuspension->hardpoints[Suspension::RearBarMidPoint];

				momentArm = currentCorner->hardpoints[Corner::InboardBarLink] - VVASEMath::NearestPointOnAxis(
					currentCorner->hardpoints[Corner::BarArmAtPivot], momentDirection, currentCorner->hardpoints[Corner::InboardBarLink]);
			}
			else if (((originalCorner->location == Corner::LocationLeftFront ||
				originalCorner->location == Corner::LocationRightFront) &&
				originalCar->suspension->frontBarStyle == Suspension::SwayBarTBar) ||
				((originalCorner->location == Corner::LocationLeftRear ||
				originalCorner->location == Corner::LocationRightRear) &&
				originalCar->suspension->rearBarStyle == Suspension::SwayBarTBar))
			{
				Vector normal, pivot, oppositeInboard;
				if (currentCorner->location == Corner::LocationLeftFront ||
					currentCorner->location == Corner::LocationRightFront)
				{
					pivot = currentSuspension->hardpoints[Suspension::FrontBarMidPoint];
					normal = currentCorner->hardpoints[Corner::BarArmAtPivot] - pivot;// TODO:  Using bar arm as pivot incorrectly

					if (currentCorner->location == Corner::LocationLeftFront)
						oppositeInboard = currentSuspension->rightFront.hardpoints[Corner::InboardBarLink];
					else
						oppositeInboard = currentSuspension->leftFront.hardpoints[Corner::InboardBarLink];
				}
				else
				{
					pivot = currentSuspension->hardpoints[Suspension::RearBarMidPoint];
					normal = currentCorner->hardpoints[Corner::BarArmAtPivot] - pivot;// TODO:  Using bar arm as pivot incorrectly

					if (currentCorner->location == Corner::LocationLeftRear)
						oppositeInboard = currentSuspension->rightRear.hardpoints[Corner::InboardBarLink];
					else
						oppositeInboard = currentSuspension->leftRear.hardpoints[Corner::InboardBarLink];
				}

				Vector topMidPoint = VVASEMath::IntersectWithPlane(normal, pivot,
					currentCorner->hardpoints[Corner::InboardBarLink] - oppositeInboard,
					currentCorner->hardpoints[Corner::InboardBarLink]);

				momentDirection = pivot - topMidPoint;
				momentArm = topMidPoint - currentCorner->hardpoints[Corner::InboardBarLink];
			}

			// Torque at bar
			// Force is not perpendicular to momentDirection - some portion of force goes into strain energy in the strucutre
			// What value of the bar torque results in the required magnitude of the force in the link?
			forceDirection = momentDirection.Cross(momentArm).Normalize();
			force = forceDirection * force.Length() / (force.Normalize() * forceDirection);
			Vector torque = momentArm.Cross(force);
			
			// At this point, torque is the bar torque (in inch-lbf) in response to the unit force at the wheel (in lbf)
			// If the wheel moves by amount dx, then the bar moves by amount dTheta
			// By the principle of virtual work, dW = F * dx = T * dTheta => dTheta / dx = F / T
			//assert(VVASEMath::IsZero(torque.Cross(momentDirection).Length(), 1.0e-10));
			cornerDoubles[ARBInstallationRatio] = 1.0 / torque.Length();
		}
	}
	else if (currentCorner->actuationType == Corner::ActuationOutboardRockerArm)
	{
		// This part is identical to the beginning of the previous section,
		// but the spring/shock take the place of the pushrod, so the installation
		// ratios are calculated with those forces.

		// If the actuator attaches to the lower control arm
		if (currentCorner->actuationAttachment == Corner::AttachmentLowerAArm)
		{
			// Identify the rotation axis and a point on that axis
			pointOnAxis = cornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			momentArm = currentCorner->hardpoints[Corner::WheelCenter] - pointOnAxis;

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
			momentArm = currentCorner->hardpoints[Corner::LowerBallJoint] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->hardpoints[Corner::LowerBallJoint]);

			// Calculate the force acting on the lower ball joint
			force =  momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

			// The next step is calculating the force through shock and spring.
			// Again, find the pivot axis and moment arm.
			pointOnAxis = currentCorner->hardpoints[Corner::LowerFrontTubMount];
			momentArm = currentCorner->hardpoints[Corner::LowerBallJoint] - pointOnAxis;
			momentDirection = currentCorner->hardpoints[Corner::LowerRearTubMount] - pointOnAxis;

			// Given the above information, we can calculate the moment magnitude in the same manner
			// as we did previously
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();
		}
		else if (currentCorner->actuationAttachment == Corner::AttachmentUpperAArm)
		{
			// Identify the rotation axis and a point on that axis
			pointOnAxis = cornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			momentArm = currentCorner->hardpoints[Corner::WheelCenter] - pointOnAxis;

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
			momentArm = currentCorner->hardpoints[Corner::UpperBallJoint] - VVASEMath::NearestPointOnAxis(
				pointOnAxis, momentDirection, currentCorner->hardpoints[Corner::UpperBallJoint]);

			// Calculate the force acting on the upper ball joint
			force =  momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

			// The next step is calculating the force through shock and spring.
			// Again, find the pivot axis and moment arm.
			pointOnAxis = currentCorner->hardpoints[Corner::UpperFrontTubMount];
			momentArm = currentCorner->hardpoints[Corner::UpperBallJoint] - pointOnAxis;
			momentDirection = currentCorner->hardpoints[Corner::UpperRearTubMount] - pointOnAxis;

			// Given the above information, we can calculate the moment magnitude in the same manner
			// as we did previously
			momentMagnitude = momentArm.Cross(force) * momentDirection.Normalize();
		}
		else if (currentCorner->actuationAttachment == Corner::AttachmentUpright)
		{
			// Identify the rotation axis and a point on that axis
			pointOnAxis = cornerVectors[InstantCenter];

			// The moment arm is any vector from the instantaneous axis of rotation to the point
			// where the force is being applied (the wheel center here).
			momentArm = currentCorner->hardpoints[Corner::WheelCenter] - pointOnAxis;

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
		momentArm = currentCorner->hardpoints[Corner::OutboardSpring] - VVASEMath::NearestPointOnAxis(
			pointOnAxis, momentDirection, currentCorner->hardpoints[Corner::OutboardSpring]);
		force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

		// Determine the force required in the direction of the spring (like we did for the force
		// through the push/pullrod)
		forceDirection = (currentCorner->hardpoints[Corner::InboardSpring]
			- currentCorner->hardpoints[Corner::OutboardSpring]).Normalize();
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
		momentArm = currentCorner->hardpoints[Corner::OutboardDamper] - VVASEMath::NearestPointOnAxis(
			pointOnAxis, momentDirection, currentCorner->hardpoints[Corner::OutboardDamper]);
		force = momentDirection.Cross(momentArm).Normalize() * momentMagnitude / momentArm.Length();

		// Determine the force required in the direction of the damper
		forceDirection = (currentCorner->hardpoints[Corner::InboardDamper]
			- currentCorner->hardpoints[Corner::OutboardDamper]).Normalize();
		force = forceDirection * force.Length() / (force.Normalize() * forceDirection);
		cornerDoubles[DamperInstallationRatio] = 1.0 / force.Length()
			* VVASEMath::Sign(force.Normalize() * forceDirection.Normalize());

		// TODO:  Need to specify attachment options for sway bars with outboard spring/damper
		// Also, should consider handling rocker arms
		//cornerDoubles[ARBInstallationRatio] = -1.0;
	}

	// Side View Swing Arm Length [in]
	// Find the plane that contains the wheel center and has the Y direction as a
	// normal, and find the intersection of the Instant Axis and that plane. This
	// vector's X-coordinate is the SVSA length.
	Vector planeNormal(0.0, 1.0, 0.0);
	Vector intersection;

	// Get the intersection of the Instant Center with this plane
	intersection = VVASEMath::IntersectWithPlane(planeNormal, currentCorner->hardpoints[Corner::WheelCenter],
		cornerVectors[InstantAxisDirection], cornerVectors[InstantCenter]);
	cornerDoubles[SideViewSwingArmLength] = fabs(intersection.x);

	// We'll need this information when calculating the anti-geometry down below:
	double sideViewSwingArmHeight = intersection.z;

	// Front View Swing Arm Length [in]
	// The procedure is identical to finding the SVSA length, except the plane we intersect
	// has the X-axis as a normal and we use the Y-coordinate as the length.
	planeNormal.Set(1.0, 0.0, 0.0);

	// Get the intersection of the Instant Center with this plane
	intersection = VVASEMath::IntersectWithPlane(planeNormal, currentCorner->hardpoints[Corner::WheelCenter],
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
	double wheelbase = (currentSuspension->rightRear.hardpoints[Corner::ContactPatch].x
		- currentSuspension->rightFront.hardpoints[Corner::ContactPatch].x
		+ currentSuspension->leftRear.hardpoints[Corner::ContactPatch].x
		- currentSuspension->leftFront.hardpoints[Corner::ContactPatch].x) / 2.0;
	double cgHeight = originalCar->massProperties->centerOfGravity.z;
	double reactionPathAngleTangent;

	// Determine if we are talking about anti-dive (front) or anti-lift (rear)
	if (isAtFront)
	{
		// Is the braking torque reacted directly by the chassis, or does it first
		// travel through the control arms?
		if (originalCar->brakes->frontBrakesInboard)
		{
			// Compute the tangent of the reaction path angle
			reactionPathAngleTangent = (sideViewSwingArmHeight -
				currentCorner->hardpoints[Corner::WheelCenter].z) / cornerDoubles[SideViewSwingArmLength];

			// Compute the anti-dive
			cornerDoubles[AntiBrakePitch] = reactionPathAngleTangent * wheelbase / cgHeight
				* originalCar->brakes->percentFrontBraking * 100.0;
		}
		else// Outboard brakes
		{
			// Compute the tangent of the reaction path angle
			reactionPathAngleTangent = sideViewSwingArmHeight / cornerDoubles[SideViewSwingArmLength];

			// Compute the anti-dive
			cornerDoubles[AntiBrakePitch] = reactionPathAngleTangent / (cgHeight / wheelbase
				* originalCar->brakes->percentFrontBraking) * 100.0;
		}
	}
	else// Anti-lift (rear)
	{
		// Is the braking torque reacted directly by the chassis, or does it first
		// travel through the control arms?
		if (originalCar->brakes->rearBrakesInboard)
		{
			// Compute the tangent of the reaction path angle
			reactionPathAngleTangent = (sideViewSwingArmHeight -
				currentCorner->hardpoints[Corner::WheelCenter].z) / cornerDoubles[SideViewSwingArmLength];

			// Compute the anti-lift
			cornerDoubles[AntiBrakePitch] = reactionPathAngleTangent * wheelbase / cgHeight
				* (1.0 - originalCar->brakes->percentFrontBraking) * 100.0;
		}
		else// Outboard brakes
		{
			// Compute the tangent of the reaction path angle
			reactionPathAngleTangent = sideViewSwingArmHeight / cornerDoubles[SideViewSwingArmLength];

			// Compute the anti-lift
			cornerDoubles[AntiBrakePitch] = reactionPathAngleTangent / (cgHeight / wheelbase
				* (1.0 - originalCar->brakes->percentFrontBraking)) * 100.0;
		}
	}

	// If the anti-brake geometry is undefined at this point, we had a divide by zero.  This
	// is the same as zero percent anti-brake, so we will clean this up by setting it to zero.
	if (VVASEMath::IsNaN(cornerDoubles[AntiBrakePitch]))
		cornerDoubles[AntiBrakePitch] = 0.0;

	// Anti-drive [%]
	// If this end of the car doesn't deliver power to the ground, then it doesn't have
	// anti-drive geometry.  Only if the longitudinal forces are present does anti-geometry
	// exist.
	// FIXME:  Do we need a % front traction for use with AWD?
	// FIXME:  This will change with independent vs. solid axle suspensions (currently we assume independent)
	if (originalCar->drivetrain->driveType == Drivetrain::DriveAllWheel ||
		(originalCar->drivetrain->driveType == Drivetrain::DriveFrontWheel && isAtFront) ||
		(originalCar->drivetrain->driveType == Drivetrain::DriveRearWheel && !isAtFront))
	{
		// Compute the tangent of the reaction path angle
		reactionPathAngleTangent = (sideViewSwingArmHeight -
			currentCorner->hardpoints[Corner::WheelCenter].z) / cornerDoubles[SideViewSwingArmLength];

		// Compute the anti-lift
		cornerDoubles[AntiDrivePitch] = reactionPathAngleTangent * wheelbase / cgHeight * 100.0;

		// If the anti-drive geometry is undefined at this point, we had a divide by zero.
		// This is the same as zero percent anti-drive, so we will clean this up by setting
		// it to zero.
		if (VVASEMath::IsNaN(cornerDoubles[AntiDrivePitch]))
			cornerDoubles[AntiDrivePitch] = 0.0;
	}
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeCaster
//
// Description:		Computes the caster angle for the specified corner.
//
// Input Arguments:
//		currentCorner	= const Corner&
//		cornerDoubles	= double*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeCaster(const Corner &corner, double *cornerDoubles)
{
	cornerDoubles[Caster] = VVASEMath::RangeToPlusMinusPi(atan2(
		corner.hardpoints[Corner::UpperBallJoint].x - corner.hardpoints[Corner::LowerBallJoint].x,
		corner.hardpoints[Corner::UpperBallJoint].z - corner.hardpoints[Corner::LowerBallJoint].z));
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeKingPinInclincation
//
// Description:		Computes the KPI for the specified corner.
//
// Input Arguments:
//		currentCorner	= const Corner&
//		sign			= const short&
//		cornerDoubles	= double*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeKingPinInclincation(const Corner &corner,
	const short &sign, double *cornerDoubles)
{
	cornerDoubles[KPI] = VVASEMath::RangeToPlusMinusPi(sign * atan2(
		corner.hardpoints[Corner::LowerBallJoint].y - corner.hardpoints[Corner::UpperBallJoint].y,
		corner.hardpoints[Corner::UpperBallJoint].z - corner.hardpoints[Corner::LowerBallJoint].z));
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeCasterTrail
//
// Description:		Computes the caster trail for the specified corner.
//
// Input Arguments:
//		currentCorner	= const Corner&
//		cornerDoubles	= double*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeCasterTrail(const Corner &corner, double *cornerDoubles)
{
	// Note on Caster Trail:  In RCVD p. 713, it is noted that sometimes trail is
	// measured perpendicular to the steering axis (instead of as a horizontal
	// distance, like we do here) because this more accurately describes the
	// moment arm that connects the tire forces to the kingpin.
	cornerDoubles[CasterTrail] = corner.hardpoints[Corner::ContactPatch].x -
		(corner.hardpoints[Corner::UpperBallJoint].x - corner.hardpoints[Corner::UpperBallJoint].z *
		(corner.hardpoints[Corner::UpperBallJoint].x - corner.hardpoints[Corner::LowerBallJoint].x) /
		(corner.hardpoints[Corner::UpperBallJoint].z - corner.hardpoints[Corner::LowerBallJoint].z));
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeScrubRadius
//
// Description:		Computes the scrub radius for the specified corner.
//
// Input Arguments:
//		currentCorner	= const Corner&
//		sign			= const short&
//		cornerDoubles	= double*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeScrubRadius(const Corner &corner,
	const short &sign, double *cornerDoubles)
{
	cornerDoubles[ScrubRadius] = sign * (corner.hardpoints[Corner::ContactPatch].y -
		 corner.hardpoints[Corner::UpperBallJoint].y - corner.hardpoints[Corner::UpperBallJoint].z *
		(corner.hardpoints[Corner::LowerBallJoint].y - corner.hardpoints[Corner::UpperBallJoint].y) /
		(corner.hardpoints[Corner::UpperBallJoint].z - corner.hardpoints[Corner::LowerBallJoint].z));
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeSpindleLength
//
// Description:		Computes the spindle length for the specified corner.
//					Spindle length isthe distance between the wheel center and
//					steer axis, measured at the height of the wheel center.
//
// Input Arguments:
//		currentCorner	= const Corner&
//		sign			= const short&
//		cornerDoubles	= double*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeSpindleLength(const Corner &corner,
	const short &sign, double *cornerDoubles)
{
	double t = (corner.hardpoints[Corner::WheelCenter].z - corner.hardpoints[Corner::LowerBallJoint].z) /
		(corner.hardpoints[Corner::UpperBallJoint].z - corner.hardpoints[Corner::LowerBallJoint].z);
	Vector pointOnSteerAxis = corner.hardpoints[Corner::LowerBallJoint] +
		(corner.hardpoints[Corner::UpperBallJoint] - corner.hardpoints[Corner::LowerBallJoint]) * t;
	cornerDoubles[SpindleLength] = (pointOnSteerAxis.y - corner.hardpoints[Corner::WheelCenter].y) /
		fabs(pointOnSteerAxis.y - corner.hardpoints[Corner::WheelCenter].y) * sign *
		corner.hardpoints[Corner::WheelCenter].Distance(pointOnSteerAxis);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeCamberAndSteer
//
// Description:		Computes the camber and steer angles for the specified corner.
//
// Input Arguments:
//		originalCorner	= const Corner&
//		currentCorner	= const Corner&
//		sign			= const short&
//		cornerDoubles	= double*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeCamberAndSteer(const Corner &originalCorner,
	const Corner &currentCorner, const short &sign, double *cornerDoubles)
{
	Vector originalWheelPlaneNormal;
	Vector newWheelPlaneNormal;
	Vector angles;

	originalWheelPlaneNormal = VVASEMath::GetPlaneNormal(
		originalCorner.hardpoints[Corner::LowerBallJoint],
		originalCorner.hardpoints[Corner::UpperBallJoint],
		originalCorner.hardpoints[Corner::OutboardTieRod]);
	newWheelPlaneNormal = VVASEMath::GetPlaneNormal(
		currentCorner.hardpoints[Corner::LowerBallJoint],
		currentCorner.hardpoints[Corner::UpperBallJoint],
		currentCorner.hardpoints[Corner::OutboardTieRod]);

	// Calculate the wheel angles to get the steer angle
	angles = originalWheelPlaneNormal.AnglesTo(newWheelPlaneNormal);
	cornerDoubles[Steer] = angles.z;

	// Rotate the NewWheelPlaneNormal back about Z by the steer angle in preparation for solving for camber
	newWheelPlaneNormal.Rotate(cornerDoubles[Steer], Vector::AxisZ);

	// Calculate the wheel angles again, this time we want the camber angle
	angles = originalWheelPlaneNormal.AnglesTo(newWheelPlaneNormal);
	cornerDoubles[Camber] = sign * angles.x;

	// Add in the effects of static camber and toe settings
	cornerDoubles[Camber] += currentCorner.staticCamber;
	cornerDoubles[Steer] += sign * currentCorner.staticToe;

	cornerDoubles[Camber] = VVASEMath::RangeToPlusMinusPi(cornerDoubles[Camber]);
	cornerDoubles[Steer] = VVASEMath::RangeToPlusMinusPi(cornerDoubles[Steer]);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeSpringDisplacement
//
// Description:		Computes the spring displacement for the specified corner.
//					Positive values indicate compression.
//
// Input Arguments:
//		originalCorner	= const Corner&
//		currentCorner	= const Corner&
//		cornerDoubles	= double*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeSpringDisplacement(const Corner &originalCorner,
	const Corner &currentCorner, double *cornerDoubles)
{
	// positive is compression
	cornerDoubles[Spring] = originalCorner.hardpoints[Corner::InboardSpring].Distance(
		originalCorner.hardpoints[Corner::OutboardSpring]) -
		currentCorner.hardpoints[Corner::InboardSpring].Distance(
		currentCorner.hardpoints[Corner::OutboardSpring]);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeDamperDisplacement
//
// Description:		Computes the damper displacement for the specified corner.
//					Positive values indicate compression.
//
// Input Arguments:
//		originalCorner	= const Corner&
//		currentCorner	= const Corner&
//		cornerDoubles	= double*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeDamperDisplacement(const Corner &originalCorner,
	const Corner &currentCorner, double *cornerDoubles)
{
	cornerDoubles[Damper] = originalCorner.hardpoints[Corner::InboardDamper].Distance(
		originalCorner.hardpoints[Corner::OutboardDamper]) -
		currentCorner.hardpoints[Corner::InboardDamper].Distance(
		currentCorner.hardpoints[Corner::OutboardDamper]);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeScrub
//
// Description:		Computes the scrub for the specified corner.
//
// Input Arguments:
//		originalCorner	= const Corner&
//		currentCorner	= const Corner&
//		sign			= const short&
//		cornerDoubles	= double*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeScrub(const Corner &originalCorner,
	const Corner &currentCorner, const short &sign, double *cornerDoubles)
{
	cornerDoubles[Scrub] = sign * (currentCorner.hardpoints[Corner::ContactPatch].y -
		originalCorner.hardpoints[Corner::ContactPatch].y);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		ComputeAxlePlunge
//
// Description:		Computes the axle plunge for the specified corner.
//					Positive values indicate that the axle is being shortenend.
//
// Input Arguments:
//		originalCorner	= const Corner&
//		currentCorner	= const Corner&
//		cornerDoubles	= double*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void KinematicOutputs::ComputeAxlePlunge(const Corner &originalCorner,
	const Corner &currentCorner, double *cornerDoubles)
{
	cornerDoubles[AxlePlunge] =
		originalCorner.hardpoints[Corner::InboardHalfShaft].Distance(
		originalCorner.hardpoints[Corner::OutboardHalfShaft]) -
		currentCorner.hardpoints[Corner::InboardHalfShaft].Distance(
		currentCorner.hardpoints[Corner::OutboardHalfShaft]);
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetCornerDoubleName
//
// Description:		Returns a string containing the name of the specified output.
//
// Input Arguments:
//		output	= const CornerOutputsDouble& specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the specified output
//
//==========================================================================
wxString KinematicOutputs::GetCornerDoubleName(const CornerOutputsDouble &output)
{
	wxString name;

	// Return the name based on the specified output index
	switch (output)
	{
	case Caster:
		name = _T("Caster");
		break;

	case Camber:
		name = _T("Camber");
		break;

	case KPI:
		name = _T("KPI");
		break;

	case Steer:
		name = _T("Steer");
		break;

	case Spring:
		name = _T("Spring");
		break;

	case Damper:
		name = _T("Damper");
		break;

	case AxlePlunge:
		name = _T("Axle Plunge");
		break;

	case CasterTrail:
		name = _T("Caster Trail");
		break;

	case ScrubRadius:
		name = _T("Scrub Radius");
		break;

	case Scrub:
		name = _T("Scrub");
		break;

	case SpringInstallationRatio:
		name = _T("Spring Installation Ratio");
		break;

	case DamperInstallationRatio:
		name = _T("Damper Installation Ratio");
		break;

	case ARBInstallationRatio:
		name = _T("ARB Installation Ratio");
		break;

	case SpindleLength:
		name = _T("Spindle Length");
		break;

	case SideViewSwingArmLength:
		name = _T("SVSA Length");
		break;

	case FrontViewSwingArmLength:
		name = _T("FVSA Length");
		break;

	case AntiBrakePitch:
		name = _T("Anti-Brake");
		break;

	case AntiDrivePitch:
		name = _T("Anti-Drive");
		break;

	default:
		assert(0);
		break;
	}

	return name;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetCornerVectorName
//
// Description:		Returns a string containing the name of the specified output.
//
// Input Arguments:
//		output	= const CornerOutputsVector& specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the specified output
//
//==========================================================================
wxString KinematicOutputs::GetCornerVectorName(const CornerOutputsVector &output)
{
	wxString name;

	// Return the name based on the specified output index
	switch (output)
	{
	case InstantCenter:
		name = _T("Instant Center");
		break;

	case InstantAxisDirection:
		name = _T("Instant Axis Direction");
		break;

	default:
		assert(0);
		break;
	}

	return name;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetDoubleName
//
// Description:		Returns a string containing the name of the specified output.
//
// Input Arguments:
//		output	= const OutputsDouble& specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the specified output
//
//==========================================================================
wxString KinematicOutputs::GetDoubleName(const OutputsDouble &output)
{
	wxString name;

	// Return the name based on the specified output index
	switch (output)
	{
	case FrontARBTwist:
		name = _T("Front ARB Twist");
		break;

	case RearARBTwist:
		name = _T("Rear ARB Twist");
		break;

	case FrontThirdSpring:
		name = _T("Front Third Spring");
		break;

	case FrontThirdDamper:
		name = _T("Front Third Damper");
		break;

	case RearThirdSpring:
		name = _T("Rear Third Spring");
		break;

	case RearThirdDamper:
		name = _T("Rear Third Damper");
		break;

	case FrontNetSteer:
		name = _T("Front Net Steer");
		break;

	case RearNetSteer:
		name = _T("Rear Net Steer");
		break;

	case FrontNetScrub:
		name = _T("Front Net Scrub");
		break;

	case RearNetScrub:
		name = _T("Rear Net Scrub");
		break;

	case FrontTrackGround:
		name = _T("Front Ground Track");
		break;

	case RearTrackGround:
		name = _T("Rear Ground Track");
		break;

	case RightWheelbaseGround:
		name = _T("Right Ground Wheelbase");
		break;

	case LeftWheelbaseGround:
		name = _T("Left Ground Wheelbase");
		break;

	case FrontTrackHub:
		name = _T("Front Hub Track");
		break;

	case RearTrackHub:
		name = _T("Rear Hub Track");
		break;

	case RightWheelbaseHub:
		name = _T("Right Hub Wheelbase");
		break;

	case LeftWheelbaseHub:
		name = _T("Left Hub Wheelbase");
		break;

	default:
		assert(0);
		break;
	}

	return name;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetVectorName
//
// Description:		Returns a string containing the name of the specified output.
//
// Input Arguments:
//		output	= const OutputsVector& specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the specified output
//
//==========================================================================
wxString KinematicOutputs::GetVectorName(const OutputsVector &output)
{
	wxString name;

	// Return the name based on the specified output index
	switch (output)
	{
	case FrontKinematicRC:
		name = _T("Front Kinematic Roll Center");
		break;

	case RearKinematicRC:
		name = _T("Rear Kinematic Roll Center");
		break;

	case RightKinematicPC:
		name = _T("Right Kinematic Pitch Center");
		break;

	case LeftKinematicPC:
		name = _T("Left Kinematic Pitch Center");
		break;

	case FrontRollAxisDirection:
		name = _T("Front Roll Axis Direction");
		break;

	case RearRollAxisDirection:
		name = _T("Rear Roll Axis Direction");
		break;

	case RightPitchAxisDirection:
		name = _T("Right Pitch Axis Direction");
		break;

	case LeftPitchAxisDirection:
		name = _T("Left Pitch Axis Direction");
		break;

	default:
		assert(0);
		break;
	}

	return name;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetOutputValue
//
// Description:		Returns the value of the output from the complete list of
//					class outputs.
//
// Input Arguments:
//		output	= const &OutputsComplete specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying the value of the requested output
//
//==========================================================================
double KinematicOutputs::GetOutputValue(const OutputsComplete &output) const
{
	double value;
	Vector temp;
	OutputsComplete newOutputIndex;

	// Depending on the specified OUTPUTS_COMPLETE, choose which output to return
	if (output <= EndRightFrontDoubles)
		value = rightFront[output - StartRightFrontDoubles];
	else if (output <= EndRightFrontVectors)
	{
		newOutputIndex = OutputsComplete(output - StartRightFrontVectors);
		temp = rightFrontVectors[int(newOutputIndex / 3)];

		// Choose the value based on the selected componenet
		switch (newOutputIndex % 3)
		{
		case 0:
			value = temp.x;
			break;

		case 1:
			value = temp.y;
			break;

		case 2:
		default:
			value = temp.z;
			break;
		}
	}
	else if (output <= EndLeftFrontDoubles)
		value = leftFront[output - StartLeftFrontDoubles];
	else if (output <= EndLeftFrontVectors)
	{
		newOutputIndex = OutputsComplete(output - StartLeftFrontVectors);
		temp = leftFrontVectors[int(newOutputIndex / 3)];

		// Choose the value based on the selected componenet
		switch (newOutputIndex % 3)
		{
		case 0:
			value = temp.x;
			break;

		case 1:
			value = temp.y;
			break;

		case 2:
		default:
			value = temp.z;
			break;
		}
	}
	else if (output <= EndRightRearDoubles)
		value = rightRear[output - StartRightRearDoubles];
	else if (output <= EndRightRearVectors)
	{
		newOutputIndex = OutputsComplete(output - StartRightRearVectors);
		temp = rightRearVectors[int(newOutputIndex / 3)];

		// Choose the value based on the selected componenet
		switch (newOutputIndex % 3)
		{
		case 0:
			value = temp.x;
			break;

		case 1:
			value = temp.y;
			break;

		case 2:
		default:
			value = temp.z;
			break;
		}
	}
	else if (output <= EndLeftRearDoubles)
		value = leftRear[output - StartLeftRearDoubles];
	else if (output <= EndLeftRearVectors)
	{
		newOutputIndex = OutputsComplete(output - StartLeftRearVectors);
		temp = leftRearVectors[int(newOutputIndex / 3)];

		// Choose the value based on the selected componenet
		switch (newOutputIndex % 3)
		{
		case 0:
			value = temp.x;
			break;

		case 1:
			value = temp.y;
			break;

		case 2:
		default:
			value = temp.z;
			break;
		}
	}
	else if (output <= EndDoubles)
		value = doubles[output - StartDoubles];
	else if (output <= EndVectors)
	{
		newOutputIndex = OutputsComplete(output - StartVectors);
		temp = vectors[int(newOutputIndex / 3)];

		// Choose the value based on the selected componenet
		switch (newOutputIndex % 3)
		{
		case 0:
			value = temp.x;
			break;

		case 1:
			value = temp.y;
			break;

		case 2:
		default:
			value = temp.z;
			break;
		}
	}
	else
		value = 0.0;

	return value;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetOutputUnitType
//
// Description:		Returns the unit type of the output from the complete
//					list of class outputs.
//
// Input Arguments:
//		output	= const &OutputsComplete specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		UnitConverter::UnitType specifying the unit type of the requested output
//
//==========================================================================
UnitConverter::UnitType KinematicOutputs::GetOutputUnitType(const OutputsComplete &output)
{
	UnitConverter::UnitType unitType;

	OutputsComplete newOutputIndex;

	// Depending on the specified Output, choose the units string
	if (output <= EndRightFrontDoubles)
		unitType = GetCornerDoubleUnitType((CornerOutputsDouble)(output - StartRightFrontDoubles));
	else if (output <= EndRightFrontVectors)
	{
		newOutputIndex = OutputsComplete(output - StartRightFrontVectors);
		unitType = GetCornerVectorUnitType((CornerOutputsVector)int(newOutputIndex / 3));
	}
	else if (output <= EndLeftFrontDoubles)
		unitType = GetCornerDoubleUnitType((CornerOutputsDouble)(output - StartLeftFrontDoubles));
	else if (output <= EndLeftFrontVectors)
	{
		newOutputIndex = OutputsComplete(output - StartLeftFrontVectors);
		unitType = GetCornerVectorUnitType((CornerOutputsVector)int(newOutputIndex / 3));
	}
	else if (output <= EndRightRearDoubles)
		unitType = GetCornerDoubleUnitType((CornerOutputsDouble)(output - StartRightRearDoubles));
	else if (output <= EndRightRearVectors)
	{
		newOutputIndex = OutputsComplete(output - StartRightRearVectors);
		unitType = GetCornerVectorUnitType((CornerOutputsVector)int(newOutputIndex / 3));
	}
	else if (output <= EndLeftRearDoubles)
		unitType = GetCornerDoubleUnitType((CornerOutputsDouble)(output - StartLeftRearDoubles));
	else if (output <= EndLeftRearVectors)
	{
		newOutputIndex = OutputsComplete(output - StartLeftRearVectors);
		unitType = GetCornerVectorUnitType((CornerOutputsVector)int(newOutputIndex / 3));
	}
	else if (output <= EndDoubles)
		unitType = GetDoubleUnitType((OutputsDouble)(output - StartDoubles));
	else if (output <= EndVectors)
	{
		newOutputIndex = OutputsComplete(output - StartVectors);
		unitType = GetVectorUnitType((OutputsVector)int(newOutputIndex / 3));
	}
	else
		unitType = UnitConverter::UnitTypeUnknown;

	return unitType;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetOutputName
//
// Description:		Returns the name of the output from the complete list of
//					class outputs.
//
// Input Arguments:
//		output	= const OutputsComplete& specifying the output in which we are
//				  interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString specifying the name of the requested output
//
//==========================================================================
wxString KinematicOutputs::GetOutputName(const OutputsComplete &output)
{
	wxString name;
	OutputsComplete newOutputIndex;

	// Depending on the specified PLOT_ID, choose the name of the string
	// Vectors are a special case - depending on which component of the vector is chosen,
	// we need to append a different string to the end of the Name
	if (output <= EndRightFrontDoubles)
	{
		name = GetCornerDoubleName((CornerOutputsDouble)(output - StartRightFrontDoubles));
		name.Prepend(_T("Right Front "));
	}
	else if (output <= EndRightFrontVectors)
	{
		newOutputIndex = OutputsComplete((int)output - (int)StartRightFrontVectors);
		name = GetCornerVectorName((CornerOutputsVector)int(newOutputIndex / 3));

		// Append the appropriate tag, depending on the specified component
		switch (newOutputIndex % 3)
		{
		case 0:
			name.append(_T(" (X)"));
			break;

		case 1:
			name.append(_T(" (Y)"));
			break;

		case 2:
		default:
			name.append(_T(" (Z)"));
			break;
		}

		name.Prepend(_T("Right Front "));
	}
	else if (output <= EndLeftFrontDoubles)
	{
		name = GetCornerDoubleName((CornerOutputsDouble)(output - StartLeftFrontDoubles));
		name.Prepend(_T("Left Front "));
	}
	else if (output <= EndLeftFrontVectors)
	{
		newOutputIndex = OutputsComplete((int)output - (int)StartLeftFrontVectors);
		name = GetCornerVectorName((CornerOutputsVector)int(newOutputIndex / 3));

		// Append the appropriate tag, depending on the specified component
		switch (newOutputIndex % 3)
		{
		case 0:
			name.append(_T(" (X)"));
			break;

		case 1:
			name.append(_T(" (Y)"));
			break;

		case 2:
		default:
			name.append(_T(" (Z)"));
			break;
		}

		name.Prepend(_T("Left Front "));
	}
	else if (output <= EndRightRearDoubles)
	{
		name = GetCornerDoubleName((CornerOutputsDouble)(output - StartRightRearDoubles));
		name.Prepend(_T("Right Rear "));
	}
	else if (output <= EndRightRearVectors)
	{
		newOutputIndex = OutputsComplete((int)output - (int)StartRightRearVectors);
		name = GetCornerVectorName((CornerOutputsVector)int(newOutputIndex / 3));

		// Append the appropriate tag, depending on the specified component
		switch (newOutputIndex % 3)
		{
		case 0:
			name.append(_T(" (X)"));
			break;

		case 1:
			name.append(_T(" (Y)"));
			break;

		case 2:
		default:
			name.append(_T(" (Z)"));
			break;
		}

		name.Prepend(_T("Right Rear "));
	}
	else if (output <= EndLeftRearDoubles)
	{
		name = GetCornerDoubleName((CornerOutputsDouble)(output - StartLeftRearDoubles));
		name.Prepend(_T("Left Rear "));
	}
	else if (output <= EndLeftRearVectors)
	{
		newOutputIndex = OutputsComplete((int)output - (int)StartLeftRearVectors);
		name = GetCornerVectorName((CornerOutputsVector)int(newOutputIndex / 3));

		// Append the appropriate tag, depending on the specified component
		switch (newOutputIndex % 3)
		{
		case 0:
			name.append(_T(" (X)"));
			break;

		case 1:
			name.append(_T(" (Y)"));
			break;

		case 2:
		default:
			name.append(_T(" (Z)"));
			break;
		}

		name.Prepend(_T("Left Rear "));
	}
	else if (output <= EndDoubles)
		name = GetDoubleName((OutputsDouble)(output - StartDoubles));
	else if (output <= EndVectors)
	{
		newOutputIndex = OutputsComplete((int)output - (int)StartVectors);
		name = GetVectorName((OutputsVector)int(newOutputIndex / 3));

		// Append the appropriate tag, depending on the specified component
		switch (newOutputIndex % 3)
		{
		case 0:
			name.append(_T(" (X)"));
			break;

		case 1:
			name.append(_T(" (Y)"));
			break;

		case 2:
		default:
			name.append(_T(" (Z)"));
			break;
		}
	}
	else
		name = _T("Unrecognized name");

	return name;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetCornerDoubleUnitType
//
// Description:		Returns a the type of units for the specified output.
//
// Input Arguments:
//		output	= const CornerOutputsDouble& specifying the
//				  output in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		UnitConverter::UnitType describing the units of the specified output
//
//==========================================================================
UnitConverter::UnitType KinematicOutputs::GetCornerDoubleUnitType(const CornerOutputsDouble &output)
{
	UnitConverter::UnitType unitType;

	// Determine the units based on it's type
	switch (output)
	{
		// Angles
	case Caster:
	case Camber:
	case KPI:
	case Steer:
		unitType = UnitConverter::UnitTypeAngle;
		break;

		// Distances
	case Spring:
	case Damper:
	case AxlePlunge:
	case CasterTrail:
	case ScrubRadius:
	case Scrub:
	case SpindleLength:
	case SideViewSwingArmLength:
	case FrontViewSwingArmLength:
		unitType = UnitConverter::UnitTypeDistance;
		break;

		// Unitless
	case SpringInstallationRatio:
	case DamperInstallationRatio:
	case AntiBrakePitch:
	case AntiDrivePitch:
		unitType = UnitConverter::UnitTypeUnitless;
		break;

		// Angles per Displacement
	case ARBInstallationRatio:
		unitType = UnitConverter::UnitTypeAnglePerDistance;
		break;

		// Unknown
	default:
		unitType = UnitConverter::UnitTypeUnknown;
		break;
	}

	return unitType;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetCornerVectorUnitType
//
// Description:		Returns a the type of units for the specified output.
//
// Input Arguments:
//		output	= const CornerOutputsVector& specifying the
//				  output in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		UnitConverter::UnitType describing the units of the specified output
//
//==========================================================================
UnitConverter::UnitType KinematicOutputs::GetCornerVectorUnitType(const CornerOutputsVector &output)
{
	UnitConverter::UnitType unitType;

	// Determine the units based on it's type
	switch (output)
	{
		// Distances
	case InstantCenter:
		unitType = UnitConverter::UnitTypeDistance;
		break;

		// Unitless (no conversion)
	case InstantAxisDirection:
		unitType = UnitConverter::UnitTypeUnitless;
		break;

		// Unknown
	default:
		unitType = UnitConverter::UnitTypeUnknown;
		break;
	}

	return unitType;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetDoubleUnitType
//
// Description:		Returns a the type of units for the specified output.
//
// Input Arguments:
//		output	= const OutputsDouble& specifying the
//				  output in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		UnitConverter::UnitType describing the units of the specified output
//
//==========================================================================
UnitConverter::UnitType KinematicOutputs::GetDoubleUnitType(const OutputsDouble &output)
{
	UnitConverter::UnitType unitType;

	// Determine the units based on it's type
	switch (output)
	{
		// Distances
	case FrontThirdSpring:
	case FrontThirdDamper:
	case RearThirdSpring:
	case RearThirdDamper:
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
		unitType = UnitConverter::UnitTypeDistance;
		break;

		// Angles
	case FrontARBTwist:
	case RearARBTwist:
	case FrontNetSteer:
	case RearNetSteer:
		unitType = UnitConverter::UnitTypeAngle;
		break;

		// Unknown units
	default:
		unitType = UnitConverter::UnitTypeUnknown;
		break;
	}

	return unitType;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		GetVectorUnitType
//
// Description:		Returns a the type of units for the specified output.
//
// Input Arguments:
//		output	= const OutputsVector& specifying the
//				  output in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		UnitConverter::UnitType describing the units of the specified output
//
//==========================================================================
UnitConverter::UnitType KinematicOutputs::GetVectorUnitType(const OutputsVector &output)
{
	UnitConverter::UnitType unitType;

	// Determine the units based on it's type
	switch (output)
	{
		// Distances
	case FrontKinematicRC:
	case RearKinematicRC:
	case RightKinematicPC:
	case LeftKinematicPC:
		unitType = UnitConverter::UnitTypeDistance;
		break;

		// Unitless
	case FrontRollAxisDirection:
	case RearRollAxisDirection:
	case RightPitchAxisDirection:
	case LeftPitchAxisDirection:
		unitType = UnitConverter::UnitTypeUnitless;
		break;

		// Unknown
	default:
		unitType = UnitConverter::UnitTypeUnknown;
		break;
	}

	return unitType;
}

//==========================================================================
// Class:			KinematicOutputs
// Function:		OutputsCompleteIndex
//
// Description:		Returns an index refering to the list of OutputsComplete
//					for the specified output.
//
// Input Arguments:
//		location		= const &Corner::Location specifying the relevant corner of the car
//		cornerDouble	= const &CornerOutputsDouble
//		cornerVector	= const &CornerOutputsVector
//		double			= const &OutputsDouble
//		vector			= const &OutputsVector
//		axis			= const &Vector::Axis
//
// Output Arguments:
//		None
//
// Return Value:
//		OUTPUTS_COMPLETE describing the index for the specified output
//
//==========================================================================
KinematicOutputs::OutputsComplete KinematicOutputs::OutputsCompleteIndex(
	const Corner::Location &location, const CornerOutputsDouble &cornerDouble,
	const CornerOutputsVector &cornerVector, const OutputsDouble &midDouble,
	const OutputsVector &vector, const Vector::Axis &axis)
{
	OutputsComplete completeIndex = NumberOfOutputScalars;

	// Switch based on the corner
	switch (location)
	{
	case Corner::LocationLeftFront:
		if (cornerDouble != NumberOfCornerOutputDoubles)
			completeIndex = (KinematicOutputs::OutputsComplete)(StartLeftFrontDoubles + cornerDouble);
		else
			completeIndex = (KinematicOutputs::OutputsComplete)(StartLeftFrontVectors + cornerVector * 3 + axis);
		break;

	case Corner::LocationRightFront:
		if (cornerDouble != NumberOfCornerOutputDoubles)
			completeIndex = (KinematicOutputs::OutputsComplete)(StartRightFrontDoubles + cornerDouble);
		else
			completeIndex = (KinematicOutputs::OutputsComplete)(StartRightFrontVectors + cornerVector * 3 + axis);
		break;

	case Corner::LocationLeftRear:
		if (cornerDouble != NumberOfCornerOutputDoubles)
			completeIndex = (KinematicOutputs::OutputsComplete)(StartLeftRearDoubles + cornerDouble);
		else
			completeIndex = (KinematicOutputs::OutputsComplete)(StartLeftRearVectors + cornerVector * 3 + axis);
		break;

	case Corner::LocationRightRear:
		if (cornerDouble != NumberOfCornerOutputDoubles)
			completeIndex = (KinematicOutputs::OutputsComplete)(StartRightRearDoubles + cornerDouble);
		else
			completeIndex = (KinematicOutputs::OutputsComplete)(StartRightRearVectors + cornerVector * 3 + axis);
		break;

	default:// Not a corner output
		if (midDouble != NumberOfOutputDoubles)
			completeIndex = (KinematicOutputs::OutputsComplete)(StartDoubles + midDouble);
		else
			completeIndex = (KinematicOutputs::OutputsComplete)(StartVectors + vector * 3 + axis);
		break;
	}

	return completeIndex;
}
