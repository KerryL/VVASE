/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  wheelSetStructures.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains structures that are useful when defined at each corner of the car.
// History:
//	11/22/2009	- Moved to vUtilities.lib, K. Loux.

#ifndef _WHEELSET_STRUCTS_H_
#define _WHEELSET_STRUCTS_H_

// VVASE headers
#include "vMath/vector.h"

// WheelSet - four doubles
struct WheelSet
{
	double rightFront;
	double leftFront;
	double rightRear;
	double leftRear;
};

// VectorSet - four Vectors
struct VectorSet
{
	Vector rightFront;
	Vector leftFront;
	Vector rightRear;
	Vector leftRear;
};

// FrontRearInteger - two integers
struct FrontRearInteger
{
	int front;
	int rear;
};

// FrontRearDouble - two doubles
struct FrontRearDouble
{
	double front;
	double rear;
};

#endif// _WHEELSET_STRUCTS_H_