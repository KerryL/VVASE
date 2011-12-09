/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  carMath.cpp
// Created:  3/24/2008
// Author:  K. Loux
// Description:  Contains useful functions that don't fit better in another class.  Hopefully this
//				 file will one day be absolved into a real class instead of just being a kludgy
//				 collection of functions.  The goal would be to be able to use Debugger classes again.
// History:
//	11/22/2009	- Moved to vMath.lib, K. Loux.
//	11/7/2011	- Corrected camelCase, K. Loux.

// Standard C++ headers
#include <cstdlib>
#include <cassert>

// VVASE headers
#include "vMath/carMath.h"
#include "vMath/vector.h"
#include "vMath/matrix.h"

//==========================================================================
// Namespace:		VVASEMath
// Function:		GetPlaneNormal
//
// Description:		Calculates the direction that is normal to the plane
//					that passes through the three input points.
//
// Input Arguments:
//		point1	= const Vector& describing location of first point on the plane
//		point2	= const Vector& describing location of second point on the plane
//		point3	= const Vector& describing location of third point on the plane
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector indicating direction that is normal to the plane that is
//		defined by the three input arguments
//
//==========================================================================
Vector VVASEMath::GetPlaneNormal(const Vector &point1, const Vector &point2, const Vector &point3)
{
	// Check for existence of a solution
	if (point1 == point2 || point1 == point3 || point2 == point3)
	{
		Vector noSolution(QNAN, QNAN, QNAN);
		//Debugger->Print(_T("Warning (GetPlaneNormal):  Coincident points"), Debugger::PRIORITY_LOW);

		return noSolution;
	}

	// To find the plane normal, we subtract the locations of two points to obtain
	// a vector that lies in the plane.  Repeat with a different pair of points to
	// obtain another vector that lies in the plane.  The cross product of these
	// vectors yields the plane normal.

	return (point1 - point2).Cross(point1 - point3).Normalize();
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		GetIntersectionOfTwoPlanes
//
// Description:		Calculates the axis that is created by the intersection
//					of two planes.
//
// Input Arguments:
//		normal1			= const Vector& describing normal direciton of first plane
//		pointOnPlane1	= const Vector& describing a point on the first plane
//		normal2			= const Vector& describing normal direciton of second plane
//		pointOnPlane2	= const Vector& describing a point on the second plane
//
// Output Arguments:
//		axisDirection	= Vector& describing the direction of the axis
//		pointOnAxis		= Vector& describing a point on the axis
//
// Return Value:
//		bool indicating whether or not a solution was found
//
//==========================================================================
bool VVASEMath::GetIntersectionOfTwoPlanes(const Vector &normal1, const Vector &pointOnPlane1,
										   const Vector &normal2, const Vector &pointOnPlane2,
										   Vector &axisDirection, Vector &pointOnAxis)
{
	// Make sure the planes are not parallel - this ensures the existence of a solution
	if (!IsZero(normal1 - normal2))
	{
		// The direction is simply the cross product of the two normal vectors
		axisDirection = normal1.Cross(normal2).Normalize();

		// Now find a point on that axis
		// This comes from the vector equation of a plane:
		// If the normal vector of a plane is known, then all points on the plane satisfy
		// planeNormal * point = someConstant.  Since we know a point that lies on each plane,
		// we can solve for that constant for each plane, then solve two simultaneous systems
		// of equations to find a common point between the two planes.
		double planeConstant1 = normal1 * pointOnPlane1;
		double planeConstant2 = normal2 * pointOnPlane2;

		// To ensure numeric stability (avoid dividing by numbers close to zero),
		// we will be "smart" about solving these equations.  Since we have two
		// equations and three unknowns, we need to choose one of the components
		// of the points ourselves.  To do this, we look at the axis direction,
		// and we choose the component that is farthest from zero.  For example,
		// if the axis direction is (1.0, 0.0, 0.0), we set X = 0 because we know
		// the axis must pass through the Y-Z plane.
		if (fabs(axisDirection.x) > fabs(axisDirection.y) && fabs(axisDirection.x) > fabs(axisDirection.z))
		{
			// Choose x = 0
			pointOnAxis.x = 0.0;

			// Again, to ensure numerical stability we need to be smart about whether we
			// solve y or z next and whether we use the first or second plane's normal
			// vector in the denominator of the last solved component.
			// FIxME:  This can probably be made to be safer (can we prove we will never have a divide by zero?)
			if (fabs(normal1.y) > fabs(normal1.z))
			{
				pointOnAxis.z = (planeConstant1 * normal2.y - planeConstant2 * normal1.y)
					/ (normal2.y * normal1.z - normal2.z * normal1.y);
				pointOnAxis.y = (planeConstant1 - normal1.z * pointOnAxis.z) / normal1.y;
			}
			else
			{
				pointOnAxis.y = (planeConstant1 * normal2.z - planeConstant2 * normal1.z)
					/ (normal2.z * normal1.y - normal2.y * normal1.z);
				pointOnAxis.z = (planeConstant1 - normal1.y * pointOnAxis.y) / normal1.z;
			}
		}
		else if (fabs(axisDirection.y) > fabs(axisDirection.x) && fabs(axisDirection.y) > fabs(axisDirection.z))
		{
			// Choose y = 0
			pointOnAxis.y = 0.0;

			// Solve the other two components
			if (fabs(normal1.x) > fabs(normal1.z))
			{
				pointOnAxis.z = (planeConstant1 * normal2.x - planeConstant2 * normal1.x)
					/ (normal2.x * normal1.z - normal2.z * normal1.x);
				pointOnAxis.x = (planeConstant1 - normal1.z * pointOnAxis.z) / normal1.x;
			}
			else
			{
				pointOnAxis.x = (planeConstant1 * normal2.z - planeConstant2 * normal1.z)
					/ (normal2.z * normal1.x - normal2.x * normal1.z);
				pointOnAxis.z = (planeConstant1 - normal1.x * pointOnAxis.x) / normal1.z;
			}
		}
		else
		{
			// Choose z = 0
			pointOnAxis.z = 0.0;

			// Solve the other two components
			if (fabs(normal1.x) > fabs(normal1.y))
			{
				pointOnAxis.y = (planeConstant1 * normal2.x - planeConstant2 * normal1.x)
					/ (normal2.x * normal1.y - normal2.y * normal1.x);
				pointOnAxis.x = (planeConstant1 - normal1.y * pointOnAxis.y) / normal1.x;
			}
			else
			{
				pointOnAxis.x = (planeConstant1 * normal2.y - planeConstant2 * normal1.y)
					/ (normal2.y * normal1.x - normal2.x * normal1.y);
				pointOnAxis.y = (planeConstant1 - normal1.x * pointOnAxis.x) / normal1.y;
			}
		}

		return true;
	}

	// No solution exists - set the output arguments to QNAN and return false
	axisDirection.Set(QNAN, QNAN, QNAN);
	pointOnAxis.Set(QNAN, QNAN, QNAN);

	return false;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		IsZero
//
// Description:		Returns true if a number is small enough to regard as zero.
//
// Input Arguments:
//		n	= const double& to be checked for being close to zero
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the number is less than NEARLY_ZERO
//
//==========================================================================
bool VVASEMath::IsZero(const double &n)
{
	if (fabs(n) < NearlyZero)
		return true;
	else
		return false;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		IsZero
//
// Description:		Returns true if a number is small enough to regard as zero.
//					This function checks the magnitude of the Vector.
//
// Input Arguments:
//		v	= const Vector& to be checked for being close to zero
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the magnitude is less than NEARLY_ZERO
//
//==========================================================================
bool VVASEMath::IsZero(const Vector &v)
{
	// Check each component of the vector
	if (v.Length() < NearlyZero)
		return true;

	return false;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		IsNaN
//
// Description:		Determines if the specified number is or is not a number.
//
// Input Arguments:
//		n	= const double& to check
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is NOT a number
//
//==========================================================================
bool VVASEMath::IsNaN(const double &n)
{
	return n != n;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		IsNaN
//
// Description:		Determines if the specified number is or is not a number.
//					Vector version - returns false if any component is NaN.
//
// Input Arguments:
//		v	= Vector& to be checked for containing valid numbers
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is NOT a number
//
//==========================================================================
bool VVASEMath::IsNaN(const Vector &v)
{
	return IsNaN(v.x) || IsNaN(v.y) || IsNaN(v.z);
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
// Input Arguments:
//		pointOnAxis		= const Vector& describing any point on the axis
//		directionOfAxis	= const Vector& that points in the direction of the axis
//		targetPoint		= const Vector& describing the point to compare to the
//						  axis
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector describing the point on the axis that is closest to
//		targetPoint
//
//==========================================================================
Vector VVASEMath::NearestPointOnAxis(const Vector &pointOnAxis, const Vector &directionOfAxis,
						  const Vector &targetPoint)
{
	double t;
	Vector temp;

	// The shortest distance is going to to a point on the axis where the line between
	// TargetPoint and that point is perpendicular to the axis.  This means their dot
	// product will be zero.  If we use a parametric equation for the line, we can use
	// the dot product to find the value of the parameteric parameter (t) in the
	// equation of the line.

	t = (directionOfAxis * (targetPoint - pointOnAxis)) / (directionOfAxis * directionOfAxis);
	temp = pointOnAxis + directionOfAxis * t;

	return temp;
}

/*
//==========================================================================
// Namespace:		VVASEMath
// Function:		NearestPointInPlane
//
// Description:		Returns the point in a specified plane that is closest to
//					the specified point.
//
// Input Arguments:
//		pointInPlane	= const Vector& describing any point on the plane
//		planeNormal		= const Vector& defining the plane's orientation
//		targetPoint		= const Vector& describing the point to compare to the
//						  axis
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector describing the point on in the plane that is closest to targetPoint
//
//==========================================================================
Vector VVASEMath::NearestPointInPlane(const Vector &pointInPlane, const Vector &planeNormal,
						   const Vector &targetPoint)
{
	double t;
	Vector temp;

	t = (directionOfAxis * (targetPoint - pointOnAxis)) / (directionOfAxis * directionOfAxis);
	temp = pointOnAxis + directionOfAxis * t;

	return temp;
}*/

//==========================================================================
// Namespace:		VVASEMath
// Function:		ProjectOntoPlane
//
// Description:		Returns the vector after it is projected onto the
//					specified plane.  Note that this assumes the plane passes
//					through the origin.
//
// Input Arguments:
//		vectorToProject	= const Vector& to be projected
//		planeNormal		= const Vector& that defines the plane orientation
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector after the projection
//
//==========================================================================
Vector VVASEMath::ProjectOntoPlane(const Vector &vectorToProject, const Vector &planeNormal)
{
	// Create the projection matrix
	/*Matrix projectionMatrix(3, 3);
	projectionMatrix.MakeIdentity();
	projectionMatrix -= planeNormal.OuterProduct(planeNormal);

	// Do the projection
	Vector projectedVector = vectorToProject - planeNormal * (vectorToProject * planeNormal);*/

	// Above method is equally valid, but this uses much less overhead
	return vectorToProject - planeNormal * (vectorToProject * planeNormal);
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		IntersectWithPlane
//
// Description:		Returns the point where the specified axis passes through
//					the specified plane.
//
// Input Arguments:
//		planeNormal		= const Vector& indicating the direction that is normal to
//						  the plane
//		pointOnPlane	= const Vector& specifying a point that lies on the plane
//		axisDirection	= Vector specifying the direction of the axis
//		pointOnAxis		= const Vector& specifying a point on the axis
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector where the intersection occurs
//
//==========================================================================
Vector VVASEMath::IntersectWithPlane(const Vector &planeNormal, const Vector &pointOnPlane,
									 Vector axisDirection, const Vector &pointOnAxis)
{
	// The return vector
	Vector intersection(QNAN, QNAN, QNAN);

	// Determine what will be used as the denominator to calculate the parameter
	// in our parametric equation
	double denominator = planeNormal * axisDirection;

	// Make sure this isn't zero (if it is, then there is no solution!)
	if (IsZero(denominator))
		return intersection;

	// If we didn't return yet, then a solution does exist.  Determine the paramter
	// in the parametric equation of the line: P = PointOnAxis + t * AxisDirection.Normalize()
	double t = planeNormal * (pointOnPlane - pointOnAxis) / denominator;

	// Use the parametric equation to find the point
	intersection = pointOnAxis + axisDirection.Normalize() * t;

	return intersection;
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
// Input Arguments:
//		n		= const double& reference to the value which we want to clamp
//		lowerLimit	= const double& lower bound of allowable values
//		upperLimit	= const double& upper bound of allowable values
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the clamped value
//
//==========================================================================
double VVASEMath::Clamp(const double &n, const double &lowerLimit, const double &upperLimit)
{
	// Make sure the arguments are valid
	assert(lowerLimit < upperLimit);

	if (n < lowerLimit)
		return lowerLimit;
	else if (n > upperLimit)
		return upperLimit;

	return n;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		RangeToPlusMinusPi
//
// Description:		Adds or subtracts 2 * PI to the specified angle until the
//					angle is between -pi and pi.
//
// Input Arguments:
//		angle		= const double& reference to the angle we want to bound
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the re-ranged angle
//
//==========================================================================
double VVASEMath::RangeToPlusMinusPi(const double &_angle)
{
	double angle = _angle;
	while (angle <= Pi)
		angle += 2 * Pi;
	while (angle > Pi)
		angle -= 2 * Pi;

	return angle;
}

//==========================================================================
// Namespace:		VVASEMath
// Function:		Sign
//
// Description:		Returns 1 for positive, -1 for negative and 0 for zero.
//
// Input Arguments:
//		n		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double VVASEMath::Sign(const double &n)
{
	if (n > 0.0)
		return 1.0;
	else if (n < 0.0)
		return -1.0;
	else
		return 0.0;
}