/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  carMath.h
// Date:  3/24/2008
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains useful functions that don't fit better in another class.  Hopefully this
//        file will one day be absolved into a real class instead of just being a kludgy
//        collection of functions.

#ifndef CAR_MATH_H_
#define CAR_MATH_H_

// Standard C++ headers
#include <limits>// For QNaN

// wxWidgets forward declarations
class wxString;

// libPlot2D forward declarations
namespace LibPlot2D
{
class Dataset2D;
}// namespace LibPlot2D

namespace VVASE
{

namespace VVASEMath
{

// Constant declarations
const double NearlyZero = 1.0e-12;
const double QNAN = std::numeric_limits<double>::quiet_NaN();
const double Pi = 3.141592653589793238462643;

// Prototypes =====================================================
// For determining if a number is close enough to zero to regard as zero
bool IsZero(const double &n, const double &eps = NearlyZero);
bool IsZero(const Vector &v, const double &eps = NearlyZero);

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

void Unwrap(Dataset2D &data);

bool XDataConsistentlySpaced(const Dataset2D &data, const double &tolerancePercent = 0.01);
double GetAverageXSpacing(const Dataset2D &data);

// Returns the sign of the argument
double Sign(const double &value);

Dataset2D ApplyBitMask(const Dataset2D &data, const unsigned int &bit);
unsigned int ApplyBitMask(const unsigned &value, const unsigned int &bit);

unsigned int GetPrecision(const double &value,
	const unsigned int &significantDigits = 2, const bool &dropTrailingZeros = true);

unsigned int CountSignificantDigits(const wxString &valueString);

void sprintf(char *dest, const unsigned int &size, const char *format, ...);

unsigned int GetPrecision(const double &minimum, const double &majorResolution, const bool &isLogarithmic = false);

// Returns the normal direction to the plane containing the three points
Vector GetPlaneNormal(const Vector &point1, const Vector &point2, const Vector &point3);

// Determines the axis created by the intersection of two planes
bool GetIntersectionOfTwoPlanes(const Vector &normal1, const Vector &pointOnPlane1,
	const Vector &normal2, const Vector &pointOnPlane2, Vector &axisDirection, Vector &pointOnAxis);

// Returns a point on a line that is closest to the specified point
Vector NearestPointOnAxis(const Vector &pointOnAxis,
	const Vector &directionOfAxis, const Vector &targetPoint);

// Returns a point in a plane that is closest to the specified point
/*Vector NearestPointInPlane(const Vector &pointInPlane, const Vector &planeNormal,
	const Vector &targetPoint);*/

// Projects the specified vector on the specified plane
Vector ProjectOntoPlane(const Vector &vectorToProject, const Vector &planeNormal);

// Returns the intersection of the specified plane and axis
Vector IntersectWithPlane(const Vector &planeNormal, const Vector &pointOnPlane,
	Vector axisDirection, const Vector &pointOnAxis);
}// namespace Math

// Template methods must be defined here:
//==========================================================================
// Namespace:		VVASEMath
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
// Namespace:		VVASEMath
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
// Namespace:		VVASEMath
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
