/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  driver_class.cpp
// Created:  11/4/2007
// Author:  K. Loux
// Description:  Contains class functionality for driver class.
// History:
//	3/9/2008	- Changed the structure of the DEBUGGER class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// VVASE headers
#include "vCar/driver_class.h"
#include "vUtilities/debug_class.h"

//==========================================================================
// Class:			DRIVER
// Function:		DRIVER
//
// Description:		Constructor for the DRIVER class.
//
// Input Arguments:
//		_Debugger	= const DEBUGGER& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DRIVER::DRIVER(const DEBUGGER &_Debugger) : Debugger(_Debugger)
{
}

//==========================================================================
// Class:			DRIVER
// Function:		DRIVER
//
// Description:		Copy constructor for the DRIVER class.
//
// Input Arguments:
//		Driver	= const DRIVER& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DRIVER::DRIVER(const DRIVER &Driver) : Debugger(Driver.Debugger)
{
	// Do the copy
	*this = Driver;
}

//==========================================================================
// Class:			DRIVER
// Function:		~DRIVER
//
// Description:		Destructor
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
DRIVER::~DRIVER()
{
}

//==========================================================================
// Class:			DRIVER
// Function:		CalculateInputs
//
// Description:		Solves for the driver inputs
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
void DRIVER::CalculateInputs(void)
{
	return;
}

//==========================================================================
// Class:			DRIVER
// Function:		operator =
//
// Description:		Assignment operator for DRIVER class.
//
// Input Arguments:
//		Driver	= const DRIVER& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		DRIVER&, reference to this object
//
//==========================================================================
DRIVER& DRIVER::operator = (const DRIVER &Driver)
{
	// Check for self-assignment
	if (this == &Driver)
		return *this;

	// Perform the assignment
	DriverInputs = Driver.DriverInputs;

	return *this;
}