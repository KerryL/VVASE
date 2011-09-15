/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  car_math.cpp
// Created:  3/24/2008
// Author:  K. Loux
// Description:  Contains useful functions that don't fit better in another class.  Hopefully this
//				 file will one day be absolved into a real class instead of just being a kludgy
//				 collection of functions.  The goal would be to be able to use DEBUGGER classes again.
// History:
//	11/22/2009	- Moved to vMath.lib, K. Loux.

// Standard C++ headers
#include <cstdlib>
#include <assert.h>

// VVASE headers
#include "vMath/car_math.h"
#include "vMath/vector_class.h"
#include "vMath/matrix_class.h"

//==========================================================================
// Namespace:		VVASEMath
// Function:		GetPlaneNormal
//
// Description:		Calculates the direction that is normal to the plane
//					that passes through the three input points.
//
// Input Argurments:
//		Point1	= const VECTOR& describing location of first point on the plane
//		Point2	= const VECTOR& describing location of second point on the plane
//		Point3	= const VECTOR& describing location of third point on the plane
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR indicating direction that is normal to the plane that is
//		defined by the three input arguments
//
//==========================================================================
VECTOR VVASEMath::GetPlaneNormal(const VECTOR &Point1, const VECTOR &Point2, const VECTOR &Point3)
{
	// Check for existence of a solution
	if (Point1 == Point2 || Point1 == Point3 || Point2 == Point3)
	{
		VECTOR NoSolution(QNAN, QNAN, QNAN);
		//Debugger->Print(_T("Warning (GetPlaneNormal):  Coincident points"), DEBUGGER::PRIORITY_LOW);

		return NoSolution;
	}

	// To find the plane normal, we subtract the locations of two points to obtain
	// a vector that lies in the plane.  Repeat with a different pair of points to
	// obtain another vector that lies in the plane.  The cross product of these
	// vectors yields the plane normal.

	return (Point1 - Point2).Cross(Point1 - Point3).Normalize();
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		GetIntersectionOfTwoPlanes
//
// Description:		Calculates the axis that is created by the intersection
//					of two planes.
//
// Input Argurments:
//		Normal1			= const VECTOR& describing normal direciton of first plane
//		PointOnPlane1	= const VECTOR& describing a point on the first plane
//		Normal2			= const VECTOR& describing normal direciton of second plane
//		PointOnPlane2	= const VECTOR& describing a point on the second plane
//
// Output Arguments:
//		AxisDirection	= VECTOR& describing the direction of the axis
//		PointOnAxis		= VECTOR& describing a point on the axis
//
// Return Value:
//		bool indicating whether or not a solution was found
//
//==========================================================================
bool VVASEMath::GetIntersectionOfTwoPlanes(const VECTOR &Normal1, const VECTOR &PointOnPlane1,
										   const VECTOR &Normal2, const VECTOR &PointOnPlane2,
										   VECTOR &AxisDirection, VECTOR &PointOnAxis)
{
	// Make sure the planes are not parallel - this ensures the existence of a solution
	if (!IsZero(Normal1 - Normal2))
	{
		// The direction is simply the cross product of the two normal vectors
		AxisDirection = Normal1.Cross(Normal2).Normalize();

		// Now find a point on that axis
		// This comes from the vector equation of a plane:
		// If the normal vector of a plane is known, then all points on the plane satisfy
		// PlaneNormal * Point = SomeConstant.  Since we know a point that lies on each plane,
		// we can solve for that constant for each plane, then solve two simultaneous systems
		// of equations to find a common point between the two planes.
		double PlaneConstant1 = Normal1 * PointOnPlane1;
		double PlaneConstant2 = Normal2 * PointOnPlane2;

		// To ensure numeric stability (avoid dividing by numbers close to zero),
		// we will be "smart" about solving these equations.  Since we have two
		// equations and three unknowns, we need to choose one of the components
		// of the points ourselves.  To do this, we look at the axis direction,
		// and we choose the component that is farthest from zero.  For example,
		// if the axis direction is (1.0, 0.0, 0.0), we set X = 0 because we know
		// the axis must pass through the Y-Z plane.
		if (fabs(AxisDirection.X) > fabs(AxisDirection.Y) && fabs(AxisDirection.X) > fabs(AxisDirection.Z))
		{
			// Choose X = 0
			PointOnAxis.X = 0.0;

			// Again, to ensure numerical stability we need to be smart about whether we
			// solve Y or Z next and whether we use the first or second plane's normal
			// vector in the denominator of the last solved component.
			// FIXME:  This can probably be made to be safer (can we prove we will never have a divide by zero?)
			if (fabs(Normal1.Y) > fabs(Normal1.Z))
			{
				PointOnAxis.Z = (PlaneConstant1 * Normal2.Y - PlaneConstant2 * Normal1.Y)
					/ (Normal2.Y * Normal1.Z - Normal2.Z * Normal1.Y);
				PointOnAxis.Y = (PlaneConstant1 - Normal1.Z * PointOnAxis.Z) / Normal1.Y;
			}
			else
			{
				PointOnAxis.Y = (PlaneConstant1 * Normal2.Z - PlaneConstant2 * Normal1.Z)
					/ (Normal2.Z * Normal1.Y - Normal2.Y * Normal1.Z);
				PointOnAxis.Z = (PlaneConstant1 - Normal1.Y * PointOnAxis.Y) / Normal1.Z;
			}
		}
		else if (fabs(AxisDirection.Y) > fabs(AxisDirection.X) && fabs(AxisDirection.Y) > fabs(AxisDirection.Z))
		{
			// Choose Y = 0
			PointOnAxis.Y = 0.0;

			// Solve the other two components
			if (fabs(Normal1.X) > fabs(Normal1.Z))
			{
				PointOnAxis.Z = (PlaneConstant1 * Normal2.X - PlaneConstant2 * Normal1.X)
					/ (Normal2.X * Normal1.Z - Normal2.Z * Normal1.X);
				PointOnAxis.X = (PlaneConstant1 - Normal1.Z * PointOnAxis.Z) / Normal1.X;
			}
			else
			{
				PointOnAxis.X = (PlaneConstant1 * Normal2.Z - PlaneConstant2 * Normal1.Z)
					/ (Normal2.Z * Normal1.X - Normal2.X * Normal1.Z);
				PointOnAxis.Z = (PlaneConstant1 - Normal1.X * PointOnAxis.X) / Normal1.Z;
			}
		}
		else
		{
			// Choose Z = 0
			PointOnAxis.Z = 0.0;

			// Solve the other two components
			if (fabs(Normal1.X) > fabs(Normal1.Y))
			{
				PointOnAxis.Y = (PlaneConstant1 * Normal2.X - PlaneConstant2 * Normal1.X)
					/ (Normal2.X * Normal1.Y - Normal2.Y * Normal1.X);
				PointOnAxis.X = (PlaneConstant1 - Normal1.Y * PointOnAxis.Y) / Normal1.X;
			}
			else
			{
				PointOnAxis.X = (PlaneConstant1 * Normal2.Y - PlaneConstant2 * Normal1.Y)
					/ (Normal2.Y * Normal1.X - Normal2.X * Normal1.Y);
				PointOnAxis.Y = (PlaneConstant1 - Normal1.X * PointOnAxis.X) / Normal1.Y;
			}
		}

		return true;
	}

	// No solution exists - set the output arguments to QNAN and return false
	AxisDirection.Set(QNAN, QNAN, QNAN);
	PointOnAxis.Set(QNAN, QNAN, QNAN);

	return false;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		IsZero
//
// Description:		Returns true if a number is small enough to regard as zero.
//
// Input Argurments:
//		ToCheck	= const double& to be checked for being close to zero
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the number is less than NEARLY_ZERO
//
//==========================================================================
bool VVASEMath::IsZero(const double &ToCheck)
{
	if (fabs(ToCheck) < NEARLY_ZERO)
		return true;
	else
		return false;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		IsZero
//
// Description:		Returns true if a number is small enough to regard as zero.
//					This function checks the magnitude of the VECTOR.
//
// Input Argurments:
//		ToCheck	= const VECTOR& to be checked for being close to zero
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the magnitude is less than NEARLY_ZERO
//
//==========================================================================
bool VVASEMath::IsZero(const VECTOR &ToCheck)
{
	// Check each component of the vector
	if (ToCheck.Length() < NEARLY_ZERO)
		return true;

	return false;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		IsNaN
//
// Description:		Determines if the specified number is or is not a number.
//
// Input Argurments:
//		ToCheck	= const double& to check
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is NOT a number
//
//==========================================================================
bool VVASEMath::IsNaN(const double &ToCheck)
{
	return ToCheck != ToCheck;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		IsNaN
//
// Description:		Determines if the specified number is or is not a number.
//					Vector version - returns false if any component is NaN.
//
// Input Argurments:
//		ToCheck	= VECTOR& to be checked for containing valid numbers
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is NOT a number
//
//==========================================================================
bool VVASEMath::IsNaN(const VECTOR &ToCheck)
{
	return IsNaN(ToCheck.X) || IsNaN(ToCheck.Y) || IsNaN(ToCheck.Z);
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		NearestPointOnAxis
//
// Description:		Returns the point on the given line that is closest to
//					the specified point. The arguments are a point on the
//					axis, the direction along the axis, and the point that
//					is not on the axis.
//
// Input Argurments:
//		PointOnAxis		= const VECTOR& describing any point on the axis
//		DirectionOfAxis	= const VECTOR& that points in the direction of the axis
//		TargetPoint		= const VECTOR& describing the point to compare to the
//						  axis
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR describing the point on the axis that is closest to
//		TargetPoint
//
//==========================================================================
VECTOR VVASEMath::NearestPointOnAxis(const VECTOR &PointOnAxis, const VECTOR &DirectionOfAxis,
						  const VECTOR &TargetPoint)
{
	double t;
	VECTOR Temp;

	// The shortest distance is going to to a point on the axis where the line between
	// TargetPoint and that point is perpendicular to the axis.  This means their dot
	// product will be zero.  If we use a parametric equation for the line, we can use
	// the dot product to find the value of the parameteric parameter (t) in the
	// equation of the line.

	t = (DirectionOfAxis * (TargetPoint - PointOnAxis)) / (DirectionOfAxis * DirectionOfAxis);
	Temp = PointOnAxis + DirectionOfAxis * t;

	return Temp;
}

/*
//==========================================================================
// Namespace:		VVASEMath
// Function:		NearestPointInPlane
//
// Description:		Returns the point in a specified plane that is closest to
//					the specified point.
//
// Input Argurments:
//		PointInPlane	= const VECTOR& describing any point on the plane
//		PlaneNormal		= const VECTOR& defining the plane's orientation
//		TargetPoint		= const VECTOR& describing the point to compare to the
//						  axis
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR describing the point on in the plane that is closest to TargetPoint
//
//==========================================================================
VECTOR VVASEMath::NearestPointInPlane(const VECTOR &PointInPlane, const VECTOR &PlaneNormal,
						   const VECTOR &TargetPoint)
{
	double t;
	VECTOR Temp;

	t = (DirectionOfAxis * (TargetPoint - PointOnAxis)) / (DirectionOfAxis * DirectionOfAxis);
	Temp = PointOnAxis + DirectionOfAxis * t;

	return Temp;
}*/

//==========================================================================
// Namespace:		VVASEMath
// Function:		ProjectOntoPlane
//
// Description:		Returns the vector after it is projected onto the
//					specified plane.  Note that this assumes the plane passes
//					through the origin.
//
// Input Argurments:
//		VectorToProject	= const VECTOR& to be projected
//		PlaneNormal		= const VECTOR& that defines the plane orientation
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR after the projection
//
//==========================================================================
VECTOR VVASEMath::ProjectOntoPlane(const VECTOR &VectorToProject, const VECTOR &PlaneNormal)
{
	// Create the projection matrix
	/*MATRIX ProjectionMatrix(3, 3);
	ProjectionMatrix.MakeIdentity();
	ProjectionMatrix -= PlaneNormal.OuterProduct(PlaneNormal);

	// Do the projection
	VECTOR ProjectedVector = VectorToProject - PlaneNormal * (VectorToProject * PlaneNormal);*/

	// Above method is equally valid, but this uses much less overhead

	return VectorToProject - PlaneNormal * (VectorToProject * PlaneNormal);
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		IntersectWithPlane
//
// Description:		Returns the point where the specified axis passes through
//					the specified plane.
//
// Input Argurments:
//		PlaneNormal		= const VECTOR& indicating the direction that is normal to
//						  the plane
//		PointOnPlane	= const VECTOR& specifying a point that lies on the plane
//		AxisDirection	= VECTOR specifying the direction of the axis
//		PointOnAxis		= const VECTOR& specifying a point on the axis
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR where the intersection occurs
//
//==========================================================================
VECTOR VVASEMath::IntersectWithPlane(const VECTOR &PlaneNormal, const VECTOR &PointOnPlane,
									 VECTOR AxisDirection, const VECTOR &PointOnAxis)
{
	// The return vector
	VECTOR Intersection(QNAN, QNAN, QNAN);

	// Determine what will be used as the denominator to calculate the parameter
	// in our parametric equation
	double Denominator = PlaneNormal * AxisDirection;

	// Make sure this isn't zero (if it is, then there is no solution!)
	if (IsZero(Denominator))
		return Intersection;

	// If we didn't return yet, then a solution does exist.  Determine the paramter
	// in the parametric equation of the line: P = PointOnAxis + t * AxisDirection.Normalize()
	double t = PlaneNormal * (PointOnPlane - PointOnAxis) / Denominator;

	// Use the parametric equation to find the point
	Intersection = PointOnAxis + AxisDirection.Normalize() * t;

	return Intersection;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		Clamp
//
// Description:		Ensures the specified value is between the limits.  In the
//					event that the value is out of the specified bounds, the
//					value that is returned is equal to the limit that the value
//					has exceeded.
//
// Input Argurments:
//		Value		= const double& reference to the value which we want to clamp
//		LowerLimit	= const double& lower bound of allowable values
//		UpperLimit	= const double& upper bound of allowable values
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the clamped value
//
//==========================================================================
double VVASEMath::Clamp(const double &Value, const double &LowerLimit, const double &UpperLimit)
{
	// Make sure the arguments are valid
	assert(LowerLimit < UpperLimit);

	if (Value < LowerLimit)
		return LowerLimit;
	else if (Value > UpperLimit)
		return UpperLimit;

	return Value;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		RangeToPlusMinusPi
//
// Description:		Adds or subtracts 2 * PI to the specified angle until the
//					angle is between -pi and pi.
//
// Input Argurments:
//		Angle		= const double& reference to the angle we want to bound
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the re-ranged angle
//
//==========================================================================
double VVASEMath::RangeToPlusMinusPi(const double &_Angle)
{
	double Angle = _Angle;
	while (Angle <= PI)
		Angle += 2 * PI;
	while (Angle > PI)
		Angle -= 2 * PI;

	return Angle;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		Sign
//
// Description:		Returns 1 for positive, -1 for negative and 0 for zero.
//
// Input Argurments:
//		Value		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double VVASEMath::Sign(const double &Value)
{
	if (Value > 0.0)
		return 1.0;
	else if (Value < 0.0)
		return -1.0;
	else
		return 0.0;
}