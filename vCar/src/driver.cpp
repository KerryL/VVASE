/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  driver.cpp
// Created:  11/4/2007
// Author:  K. Loux
// Description:  Contains class functionality for driver class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

// VVASE headers
#include "vCar/driver.h"
#include "vUtilities/debugger.h"

//==========================================================================
// Class:			Driver
// Function:		Driver
//
// Description:		Constructor for the Driver class.
//
// Input Arguments:
//		_debugger	= const Debugger& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Driver::Driver(const Debugger &_debugger) : debugger(_debugger)
{
}

//==========================================================================
// Class:			Driver
// Function:		Driver
//
// Description:		Copy constructor for the Driver class.
//
// Input Arguments:
//		driver	= const Driver& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Driver::Driver(const Driver &driver) : debugger(driver.debugger)
{
	// Do the copy
	*this = driver;
}

//==========================================================================
// Class:			Driver
// Function:		~Driver
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
Driver::~Driver()
{
}

//==========================================================================
// Class:			Driver
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
void Driver::CalculateInputs(void)
{
}

//==========================================================================
// Class:			Driver
// Function:		operator =
//
// Description:		Assignment operator for Driver class.
//
// Input Arguments:
//		driver	= const Driver& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Driver&, reference to this object
//
//==========================================================================
Driver& Driver::operator = (const Driver &driver)
{
	// Check for self-assignment
	if (this == &driver)
		return *this;

	// Perform the assignment
	driverInputs = driver.driverInputs;

	return *this;
}