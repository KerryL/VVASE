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
//	11/7/2011	- Corrected camelCase, K. Loux.

#ifndef _CAR_MATH_H_
#define _CAR_MATH_H_

// Standard C++ headers
#include <limits>// For QNaN

// vMath forward declarations
class Vector;

namespace VVASEMath
{

	// Constant declarations
	const double NearlyZero = 1.0e-12;
	const double QNAN = std::numeric_limits<double>::quiet_NaN();
	const double Pi = 3.141592653589793238462643;

	// Prototypes =====================================================
	// Returns the normal direction to the plane containing the three points
	Vector GetPlaneNormal(const Vector &point1, const Vector &point2, const Vector &point3);

	// Determines the axis created by the intersection of two planes
	bool GetIntersectionOfTwoPlanes(const Vector &normal1, const Vector &pointOnPlane1, const Vector &normal2,
									const Vector &pointOnPlane2, Vector &axisDirection, Vector &pointOnAxis);

	// For determining if a number is close enough to zero to regard as zero
	bool IsZero(const double &n);
	bool IsZero(const Vector &v);

	// Checks to see if a value is not a number
	bool IsNaN(const double &n);
	bool IsNaN(const Vector &v);

	// Returns a point on a line that is closest to the specified point
	Vector NearestPointOnAxis(const Vector &pointOnAxis, const Vector &directionOfAxis,
							  const Vector &targetPoint);

	// Returns a point in a plane that is closest to the specified point
	/*Vector NearestPointInPlane(const Vector &pointInPlane, const Vector &planeNormal,
							   const Vector &targetPoint);*/

	// Projects the specified vector on the specified plane
	Vector ProjectOntoPlane(const Vector &vectorToProject, const Vector &planeNormal);

	// Returns the intersection of the specified plane and axis
	Vector IntersectWithPlane(const Vector &planeNormal, const Vector &pointOnPlane,
							  Vector axisDirection, const Vector &pointOnAxis);

	// Ensure the value is between two definined limits
	double Clamp(const double &n, const double &lowerLimit, const double &upperLimit);

	// Converts the angle to be between -PI and PI
	double RangeToPlusMinusPi(const double &_angle);

	// Returns the sign of the argument
	double Sign(const double &n);
}

#endif// _CAR_MATH_H_