/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  spring.cpp
// Date:  1/3/2009
// Author:  K. Loux
// Desc:  Contains class definition for Spring class.  This class contains
//        the information and methods required to model a non-linear spring.

// vCar headers
#include "vCar/spring.h"

//==========================================================================
// Class:			Spring
// Function:		Spring
//
// Description:		Constructor for Spring class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Spring::Spring()
{
	rate = 0.0;
}

//==========================================================================
// Class:			Spring
// Function:		~Spring
//
// Description:		Destructor for Spring class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Spring::~Spring()
{
}
