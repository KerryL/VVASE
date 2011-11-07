/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  inverse_semaphore_class.cpp
// Created:  4/10/2010
// Author:  K. Loux
// Description:  This class is a synchronization object for multi-threaded applications.
//				 After a counter is set, it blocks the calling thread until the counter
//				 is decremented to zero.
// History:

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "vSolver/threads/inverse_semaphore_class.h"

//==========================================================================
// Class:			INVERSE_SEMAPHORE
// Function:		INVERSE_SEMAPHORE
//
// Description:		Constructor for the INVERSE_SEMAPHORE class.
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
INVERSE_SEMAPHORE::INVERSE_SEMAPHORE()
{
	// Initialize the count to zero
	Count = 0;
}

//==========================================================================
// Class:			INVERSE_SEMAPHORE
// Function:		~INVERSE_SEMAPHORE
//
// Description:		Destructor for the INVERSE_SEMAPHORE class.
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
INVERSE_SEMAPHORE::~INVERSE_SEMAPHORE()
{
	// FIXME:  Check to make sure we have no pending threads?
}

//==========================================================================
// Class:			INVERSE_SEMAPHORE
// Function:		Post
//
// Description:		Decrements the counter for this object.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		INVERSE_SEMAPHORE_ERROR (error codes for this class)
//
//==========================================================================
INVERSE_SEMAPHORE::INVERSE_SEMAPHORE_ERROR INVERSE_SEMAPHORE::Post(void)
{
	// Lock the mutex
	wxMutexLocker Lock(CountMutex);

	// Make sure we aquired the lock
	if (!Lock.IsOk())
		return ErrorMutex;

	// Make sure we're not already at zero
	if (Count == 0)
		return ErrorUnderflow;

	// Decrement the counter and return success
	Count--;

	return ErrorNone;
}

//==========================================================================
// Class:			INVERSE_SEMAPHORE
// Function:		Set
//
// Description:		Sets the counter to the desired value.
//
// Input Arguments:
//		_Count	= unsigned int to which the counter will be set
//
// Output Arguments:
//		None
//
// Return Value:
//		INVERSE_SEMAPHORE_ERROR (error codes for this class)
//
//==========================================================================
INVERSE_SEMAPHORE::INVERSE_SEMAPHORE_ERROR INVERSE_SEMAPHORE::Set(unsigned int _Count)
{
	// Lock the mutex
	wxMutexLocker Lock(CountMutex);

	// Make sure the lock was successfully aquired
	if (!Lock.IsOk())
		return ErrorMutex;

	// Make sure the count is zero
	if (Count != 0)
		return ErrorBusy;

	// Set the count to the desired value
	Count = _Count;

	return ErrorNone;
}

//==========================================================================
// Class:			INVERSE_SEMAPHORE
// Function:		Wait
//
// Description:		Blocks the calling thread until the counter returns to zero.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		INVERSE_SEMAPHORE_ERROR (error codes for this class)
//
//==========================================================================
INVERSE_SEMAPHORE::INVERSE_SEMAPHORE_ERROR INVERSE_SEMAPHORE::Wait(void)
{
	// Wait for the count to return to zero
	while (Count > 0)
	{
		// Wait and yeild to other threads
		// FIXME:  Should we sleep and yeild or do nothing? (currently sleeping and yielding)
		wxSafeYield();
		wxMilliSleep(50);
	}

	return ErrorNone;
}