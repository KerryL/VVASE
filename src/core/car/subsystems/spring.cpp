/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  spring.cpp
// Date:  1/3/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
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
