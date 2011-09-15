/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  car_math.h
// Created:  3/24/2008
// Author:  K. Loux
// Description:  Contains useful functions that don't fit better in another class.  Hopefully this
//				 file will one day be absolved into a real class instead of just being a kludgy
//				 collection of functions.
// History:
//	4/11/2009	- Changed all functions to take addresses of and use const, K. Loux.
//	11/22/2009	- Moved to vMath.lib, K. Loux.

#ifndef _CAR_MATH_H_
#define _CAR_MATH_H_

// Standard C++ headers
#include <limits>// For QNaN

// vMath forward declarations
class VECTOR;

namespace VVASEMath
{

	// Constant declarations
	const double NEARLY_ZERO = 1.0e-12;
	const double QNAN = std::numeric_limits<double>::quiet_NaN();
	const double PI = 3.141592653589793238462643;

	// Prototypes =====================================================
	// Returns the normal direction to the plane containing the three points
	VECTOR GetPlaneNormal(const VECTOR &Point1, const VECTOR &Point2, const VECTOR &Point3);

	// Determines the axis created by the intersection of two planes
	bool GetIntersectionOfTwoPlanes(const VECTOR &Normal1, const VECTOR &PointOnPlane1, const VECTOR &Normal2,
									const VECTOR &PointOnPlane2, VECTOR &AxisDirection, VECTOR &PointOnAxis);

	// For determining if a number is close enough to zero to regard as zero
	bool IsZero(const double &ToCheck);
	bool IsZero(const VECTOR &ToCheck);

	// Checks to see if a value is not a number
	bool IsNaN(const double &ToCheck);
	bool IsNaN(const VECTOR &ToCheck);

	// Returns a point on a line that is closest to the specified point
	VECTOR NearestPointOnAxis(const VECTOR &PointOnAxis, const VECTOR &DirectionOfAxis,
							  const VECTOR &TargetPoint);

	// Returns a point in a plane that is closest to the specified point
	/*VECTOR NearestPointInPlane(const VECTOR &PointInPlane, const VECTOR &PlaneNormal,
							   const VECTOR &TargetPoint);*/

	// Projects the specified vector on the specified plane
	VECTOR ProjectOntoPlane(const VECTOR &VectorToProject, const VECTOR &PlaneNormal);

	// Returns the intersection of the specified plane and axis
	VECTOR IntersectWithPlane(const VECTOR &PlaneNormal, const VECTOR &PointOnPlane,
							  VECTOR AxisDirection, const VECTOR &PointOnAxis);

	// Ensure the value is between two definined limits
	double Clamp(const double &Value, const double &LowerLimit, const double &UpperLimit);

	// Converts the angle to be between -PI and PI
	double RangeToPlusMinusPi(const double &_Angle);

	// Returns the sign of the argument
	double Sign(const double &Value);
}

#endif// _CAR_MATH_H_