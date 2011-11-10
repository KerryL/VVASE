/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

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
// Class:			MILLIKEN_ANALYSIS
// Function:		MILLIKEN_ANALYSIS
//
// Description:		Constructor for the MILLIKEN_ANALYSIS class.
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
MILLIKEN_ANALYSIS::MILLIKEN_ANALYSIS(const Debugger &_debugger) : debugger(_debugger)
{
}

//==========================================================================
// Class:			MILLIKEN_ANALYSIS
// Function:		MILLIKEN_ANALYSIS
//
// Description:		Copy constructor for the MILLIKEN_ANALYSIS class.
//
// Input Arguments:
//		MillikenAnalysis	= const MILLIKEN_ANALYSIS& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MILLIKEN_ANALYSIS::MILLIKEN_ANALYSIS(const MILLIKEN_ANALYSIS &MillikenAnalysis) : debugger(MillikenAnalysis.debugger)
{
	// Do the copy
	*this = MillikenAnalysis;
}

//==========================================================================
// Class:			MILLIKEN_ANALYSIS
// Function:		~MILLIKEN_ANALYSIS
//
// Description:		Destructor for the MILLIKEN_ANALYSIS class.
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
MILLIKEN_ANALYSIS::~MILLIKEN_ANALYSIS()
{
}

//==========================================================================
// Class:			MILLIKEN_ANALYSIS
// Function:		operator =
//
// Description:		Assignment operator for MILLIKEN_ANALYSIS class.
//
// Input Arguments:
//		MillikenAnalysis	= const MILLIKEN_ANALYSIS& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		MILLIKEN_ANALYSIS&, reference to this object
//
//==========================================================================
MILLIKEN_ANALYSIS& MILLIKEN_ANALYSIS::operator = (const MILLIKEN_ANALYSIS &MillikenAnalysis)
{
	// Check for self-assignment
	if (this == &MillikenAnalysis)
		return *this;

	// Perform the assignment
	// Nothing yet...

	return *this;
}