/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  shaker_rig_class.cpp
// Created:  3/23/2008
// Author:  K. Loux
// Description:  Contains class functionality for shaker rig class.  This is a type of dynamic
//				 analysis that uses a Fourier transform to create Bode plots of the vehicle's
//				 response to different inputs.
// History:

// Standard C++ headers
#include <cstdlib>

// VVASE headers
#include "vSolver/physics/shaker_rig_class.h"
#include "vUtilities/debug_class.h"

//==========================================================================
// Class:			SHAKER_RIG
// Function:		SHAKER_RIG
//
// Description:		Constructor for the SHAKER_RIG class.
//
// Input Argurments:
//		_Debugger	= const DEBUGGER&, reference to the debug message printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
SHAKER_RIG::SHAKER_RIG(const DEBUGGER &_Debugger) : Debugger(_Debugger)
{
}

//==========================================================================
// Class:			SHAKER_RIG
// Function:		SHAKER_RIG
//
// Description:		Copy constructor for the SHAKER_RIG class.
//
// Input Argurments:
//		ShakerRig	= const SHAKER_RIG& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
SHAKER_RIG::SHAKER_RIG(const SHAKER_RIG &ShakerRig) : Debugger(ShakerRig.Debugger)
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
// Input Argurments:
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
// Input Argurments:
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