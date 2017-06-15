/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  wheelSetStructures.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains structures that are useful when defined at each corner of the car.
// History:
//	11/22/2009	- Moved to vUtilities.lib, K. Loux.

#ifndef WHEELSET_STRUCTS_H_
#define WHEELSET_STRUCTS_H_

// VVASE headers
#include "vMath/vector.h"

template<typename T>
struct CornerSet
{
	T rightFront;
	T leftFront;
	T rightRear;
	T leftRear;

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
	T front;
	T rear;

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
typedef CornerSet<Vector> VectorSet;

typedef EndSet<int> FrontRearInteger;
typedef EndSet<double> FrontRearDouble;

#endif// WHEELSET_STRUCTS_H_