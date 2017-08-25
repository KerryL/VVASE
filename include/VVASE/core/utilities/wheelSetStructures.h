/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  wheelSetStructures.h
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains structures that are useful when defined at each corner of the car.

#ifndef WHEELSET_STRUCTS_H_
#define WHEELSET_STRUCTS_H_

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

template<typename T>
struct CornerSet
{
	T rightFront = T();
	T leftFront = T();
	T rightRear = T();
	T leftRear = T();

	bool operator==(const CornerSet<T>& target) const
	{
		if (rightFront == target.rightFront &&
			leftFront == target.leftFront &&
			rightRear == target.rightRear &&
			leftRear == target.leftRear)
			return true;

		return false;
	}

	bool operator!=(const CornerSet<T>& target) const
	{
		return !(*this == target);
	}
};

template<typename T>
struct EndSet
{
	T front = T();
	T rear = T();

	bool operator==(const EndSet<T> &target) const
	{
		if (front == target.front &&
			rear == target.rear)
			return true;

		return false;
	}

	bool operator!=(const EndSet<T>& target) const
	{
		return !(*this == target);
	}
};

// Predefined types
typedef CornerSet<double> WheelSet;
typedef CornerSet<Eigen::Vector3d> VectorSet;

typedef EndSet<int> FrontRearInteger;
typedef EndSet<double> FrontRearDouble;

}// namespace VVASE

#endif// WHEELSET_STRUCTS_H_
