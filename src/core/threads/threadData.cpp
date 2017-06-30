/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  threadData.cpp
// Date:  1/12/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the ThreadData class.  This is an
//        abstract class from which objects should be derived to pass data to
//        different types of threads.  Using this class allows for a cleaner
//        thread management system, where the queue only needs to know about
//        this class, not any specific data for threads that perform specific
//        tasks.

// Local headers
#include "VVASE/core/threads/threadData.h"

namespace VVASE
{

//==========================================================================
// Class:			ThreadData
// Function:		ThreadData
//
// Description:		Constructor for the ThreadData class (basic).
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
ThreadData::ThreadData()
{
}

//==========================================================================
// Class:			ThreadData
// Function:		~ThreadData
//
// Description:		Destructor for the ThreadData class.
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
ThreadData::~ThreadData()
{
}

}// namespace VVASE
