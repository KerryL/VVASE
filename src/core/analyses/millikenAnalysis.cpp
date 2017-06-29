/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  millikenAnalysis.cpp
// Date:  3/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class definition for Milliken Moment Method analysis class.
//        See RCVD for more information.

// Standard C++ headers
#include <cstdlib>

// Local headers
#include "vSolver/physics/millikenAnalysis.h"
#include "vUtilities/debugger.h"

namespace VVASE
{

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

}// namespace VVASE
