/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  milliken_class.cpp
// Created:  3/23/2008
// Author:  K. Loux
// Description:  Contains class definition for Milliken Moment Method analysis class.  See RCVD
//				 for more information.
// History:

// Standard C++ headers
#include <cstdlib>

// VVASE headers
#include "vSolver/physics/milliken_class.h"
#include "vUtilities/debug_class.h"

//==========================================================================
// Class:			MILLIKEN_ANALYSIS
// Function:		MILLIKEN_ANALYSIS
//
// Description:		Constructor for the MILLIKEN_ANALYSIS class.
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
MILLIKEN_ANALYSIS::MILLIKEN_ANALYSIS(const DEBUGGER &_Debugger) : Debugger(_Debugger)
{
}

//==========================================================================
// Class:			MILLIKEN_ANALYSIS
// Function:		MILLIKEN_ANALYSIS
//
// Description:		Copy constructor for the MILLIKEN_ANALYSIS class.
//
// Input Argurments:
//		MillikenAnalysis	= const MILLIKEN_ANALYSIS& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MILLIKEN_ANALYSIS::MILLIKEN_ANALYSIS(const MILLIKEN_ANALYSIS &MillikenAnalysis) : Debugger(MillikenAnalysis.Debugger)
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
MILLIKEN_ANALYSIS::~MILLIKEN_ANALYSIS()
{
}

//==========================================================================
// Class:			MILLIKEN_ANALYSIS
// Function:		operator =
//
// Description:		Assignment operator for MILLIKEN_ANALYSIS class.
//
// Input Argurments:
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