/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  carMath.h
// Date:  3/24/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains useful functions that don't fit better in another class.  Hopefully this
//        file will one day be absolved into a real class instead of just being a kludgy
//        collection of functions.

#ifndef CAR_MATH_H_
#define CAR_MATH_H_

// Local headers
#include "vvaseString.h"

// Standard C++ headers
#include <limits>// For QNaN
#include <cmath>

// Eigen headers
#include <Eigen/Eigen>

// libPlot2D forward declarations
namespace LibPlot2D
{
class Dataset2D;
}// namespace LibPlot2D

namespace VVASE
{

namespace Math
{

enum class Axis
{
	X,
	Y,
	Z
};

// Constant declarations
const double NearlyZero = 1.0e-12;
const double QNAN = std::numeric_limits<double>::quiet_NaN();
const double Pi = M_PI;

// Prototypes =====================================================
// For determining if a number is close enough to zero to regard as zero
bool IsZero(const double &n, const double &eps = NearlyZero);
bool IsZero(const Eigen::VectorXd &v, const double &eps = NearlyZero);

template<typename T>
bool IsNaN(const T &value);

template<typename T>
bool IsInf(const T &value);

template<typename T>
bool IsValid(const T &value);

// Ensure the value is between two definined limits
double Clamp(const double &value, const double &lowerLimit, const double &upperLimit);

double RangeToPlusMinusPi(const double &angle);
double RangeToPlusMinus180(const double &angle);

void Unwrap(LibPlot2D::Dataset2D &data);

bool XDataConsistentlySpaced(const LibPlot2D::Dataset2D &data, const double &tolerancePercent = 0.01);
double GetAverageXSpacing(const LibPlot2D::Dataset2D &data);

// Returns the sign of the argument
double Sign(const double &value);

LibPlot2D::Dataset2D ApplyBitMask(const LibPlot2D::Dataset2D &data, const unsigned int &bit);
unsigned int ApplyBitMask(const unsigned &value, const unsigned int &bit);

unsigned int GetPrecision(const double &value,
	const unsigned int &significantDigits = 2, const bool &dropTrailingZeros = true);

unsigned int CountSignificantDigits(const vvaseString &valueString);

void sprintf(char *dest, const unsigned int &size, const char *format, ...);

unsigned int GetPrecision(const double &minimum, const double &majorResolution, const bool &isLogarithmic = false);

// Returns the normal direction to the plane containing the three points
Eigen::Vector3d GetPlaneNormal(const Eigen::Vector3d &point1, const Eigen::Vector3d &point2, const Eigen::Vector3d &point3);

// Determines the axis created by the intersection of two planes
bool GetIntersectionOfTwoPlanes(const Eigen::Vector3d &normal1, const Eigen::Vector3d &pointOnPlane1,
	const Eigen::Vector3d &normal2, const Eigen::Vector3d &pointOnPlane2, Eigen::Vector3d &axisDirection, Eigen::Vector3d &pointOnAxis);

// Returns a point on a line that is closest to the specified point
Eigen::Vector3d NearestPointOnAxis(const Eigen::Vector3d &pointOnAxis,
	const Eigen::Vector3d &directionOfAxis, const Eigen::Vector3d &targetPoint);

// Returns a point in a plane that is closest to the specified point
/*Eigen::Vector3d NearestPointInPlane(const Eigen::Vector3d &pointInPlane, const Eigen::Vector3d &planeNormal,
	const Eigen::Vector3d &targetPoint);*/

// Projects the specified vector on the specified plane
Eigen::Vector3d ProjectOntoPlane(const Eigen::Vector3d &vectorToProject, const Eigen::Vector3d &planeNormal);

// Returns the intersection of the specified plane and axis
Eigen::Vector3d IntersectWithPlane(const Eigen::Vector3d &planeNormal, const Eigen::Vector3d &pointOnPlane,
	Eigen::Vector3d axisDirection, const Eigen::Vector3d &pointOnAxis);

vvaseString GetAxisName(const Axis& axis);
}// namespace Math

// Template methods must be defined here:
//==========================================================================
// Namespace:		VVASE::Math
// Function:		IsNaN
//
// Description:		Determines if the specified number is or is not a number.
//
// Input Arguments:
//		value	= const T& to check
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is NOT a number
//
//==========================================================================
template<typename T>
bool Math::IsNaN(const T &value)
{
	return value != value;
}

//==========================================================================
// Namespace:		VVASE::Math
// Function:		IsInf
//
// Description:		Determines if the specified number is infinite.
//
// Input Arguments:
//		value	= const T&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is ininite
//
//==========================================================================
template<typename T>
bool Math::IsInf(const T &value)
{
	return std::numeric_limits<T>::has_infinity &&
		value == std::numeric_limits<T>::infinity();
}

//==========================================================================
// Namespace:		VVASE::Math
// Function:		IsValid
//
// Description:		Determines if the specified value is a valid number.
//
// Input Arguments:
//		value	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is valid
//
//==========================================================================
template<typename T>
bool Math::IsValid(const T &value)
{
	return !IsNaN<T>(value) && !IsInf<T>(value);
}

}// namespace VVASE

#endif// CAR_MATH_H_
