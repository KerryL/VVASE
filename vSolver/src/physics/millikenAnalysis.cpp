/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  millikenAnalysis.cpp
// Created:  3/23/2008
// Author:  K. Loux
// Description:  Contains class definition for Milliken Moment Method analysis class.  See RCVD
//				 for more information.
// History:

// Standard C++ headers
#include <cstdlib>

// VVASE headers
#include "vSolver/physics/millikenAnalysis.h"
#include "vUtilities/debugger.h"

//==========================================================================
// Class:			MillikenAnalysis
// Function:		MillikenAnalysis
//
// Description:		Constructor for the MillikenAnalysis class.
//
// Input Arguments:
//		debugger	= const Debugger&, reference to the debug message printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MillikenAnalysis::MillikenAnalysis(const Debugger &debugger) : debugger(debugger)
{
}

//==========================================================================
// Class:			MillikenAnalysis
// Function:		MillikenAnalysis
//
// Description:		Copy constructor for the MillikenAnalysis class.
//
// Input Arguments:
//		MillikenAnalysis	= const MillikenAnalysis& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MillikenAnalysis::MillikenAnalysis(const MillikenAnalysis &millikenAnalysis) : debugger(millikenAnalysis.debugger)
{
	// Do the copy
	*this = millikenAnalysis;
}

//==========================================================================
// Class:			MillikenAnalysis
// Function:		~MillikenAnalysis
//
// Description:		Destructor for the MillikenAnalysis class.
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
MillikenAnalysis::~MillikenAnalysis()
{
}

//==========================================================================
// Class:			MillikenAnalysis
// Function:		operator =
//
// Description:		Assignment operator for MillikenAnalysis class.
//
// Input Arguments:
//		millikenAnalysis	= const MillikenAnalysis& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		MillikenAnalysis&, reference to this object
//
//==========================================================================
MillikenAnalysis& MillikenAnalysis::operator = (const MillikenAnalysis &millikenAnalysis)
{
	// Check for self-assignment
	if (this == &millikenAnalysis)
		return *this;

	// Perform the assignment
	// Nothing yet...

	return *this;
}