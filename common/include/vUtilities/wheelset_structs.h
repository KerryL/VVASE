/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  wheelset_structs.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains structures that are useful when defined at each corner of the car.
// History:
//	11/22/2009	- Moved to vUtilities.lib, K. Loux.

#ifndef _WHEELSET_STRUCTS_H_
#define _WHEELSET_STRUCTS_H_

// VVASE headers
#include "vMath/vector_class.h"

// WHEEL_SET - four doubles
struct WHEEL_SET
{
	double RightFront;
	double LeftFront;
	double RightRear;
	double LeftRear;
};

// VECTOR_SET - four VECTORs
struct VECTOR_SET
{
	VECTOR RightFront;
	VECTOR LeftFront;
	VECTOR RightRear;
	VECTOR LeftRear;
};

// FRONT_REAR_INT - two integers
struct FRONT_REAR_INTEGER
{
	int Front;
	int Rear;
};

// FRONT_REAR_DOUBLE - two doubles
struct FRONT_REAR_DOUBLE
{
	double Front;
	double Rear;
};

#endif// _WHEELSET_STRUCTS_H_