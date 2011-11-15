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
// Class:			ShakerRig
// Function:		ShakerRig
//
// Description:		Constructor for the ShakerRig class.
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
ShakerRig::ShakerRig(const Debugger &_debugger) : debugger(_debugger)
{
}

//==========================================================================
// Class:			ShakerRig
// Function:		ShakerRig
//
// Description:		Copy constructor for the ShakerRig class.
//
// Input Arguments:
//		shakerRig	= const ShakerRig& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ShakerRig::ShakerRig(const ShakerRig &shakerRig) : debugger(shakerRig.debugger)
{
	// Do the copy
	*this = shakerRig;
}

//==========================================================================
// Class:			ShakerRig
// Function:		~ShakerRig
//
// Description:		Destructor for the ShakerRig class.
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
ShakerRig::~ShakerRig()
{
}

//==========================================================================
// Class:			ShakerRig
// Function:		operator =
//
// Description:		Assignment operator for ShakerRig class.
//
// Input Arguments:
//		shakerRig	= const ShakerRig& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		ShakerRig&, reference to this object
//
//==========================================================================
ShakerRig& ShakerRig::operator = (const ShakerRig &shakerRig)
{
	// Check for self-assignment
	if (this == &shakerRig)
		return *this;

	// Perform the assignment
	// Nothing yet...

	return *this;
}