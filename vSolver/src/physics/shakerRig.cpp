/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  shakerRig.cpp
// Created:  3/23/2008
// Author:  K. Loux
// Description:  Contains class functionality for shaker rig class.  This is a type of dynamic
//				 analysis that uses a Fourier transform to create Bode plots of the vehicle's
//				 response to different inputs.
// History:

// Standard C++ headers
#include <cstdlib>

// VVASE headers
#include "vSolver/physics/shakerRig.h"
#include "vUtilities/debugger.h"

//==========================================================================
// Class:			SHAKER_RIG
// Function:		SHAKER_RIG
//
// Description:		Constructor for the SHAKER_RIG class.
//
// Input Arguments:
//		_debugger	= const Debugger&, reference to the debug message printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
SHAKER_RIG::SHAKER_RIG(const Debugger &_debugger) : debugger(_debugger)
{
}

//==========================================================================
// Class:			SHAKER_RIG
// Function:		SHAKER_RIG
//
// Description:		Copy constructor for the SHAKER_RIG class.
//
// Input Arguments:
//		ShakerRig	= const SHAKER_RIG& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
SHAKER_RIG::SHAKER_RIG(const SHAKER_RIG &ShakerRig) : debugger(ShakerRig.debugger)
{
	// Do the copy
	*this = ShakerRig;
}

//==========================================================================
// Class:			SHAKER_RIG
// Function:		~SHAKER_RIG
//
// Description:		Destructor for the SHAKER_RIG class.
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
SHAKER_RIG::~SHAKER_RIG()
{
}

//==========================================================================
// Class:			SHAKER_RIG
// Function:		operator =
//
// Description:		Assignment operator for SHAKER_RIG class.
//
// Input Arguments:
//		ShakerRig	= const SHAKER_RIG& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		SHAKER_RIG&, reference to this object
//
//==========================================================================
SHAKER_RIG& SHAKER_RIG::operator = (const SHAKER_RIG &ShakerRig)
{
	// Check for self-assignment
	if (this == &ShakerRig)
		return *this;

	// Perform the assignment
	// Nothing yet...

	return *this;
}