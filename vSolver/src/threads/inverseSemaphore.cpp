/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  inverseSemaphore.cpp
// Created:  4/10/2010
// Author:  K. Loux
// Description:  This class is a synchronization object for multi-threaded applications.
//				 After a counter is set, it blocks the calling thread until the counter
//				 is decremented to zero.
// History:

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "vSolver/threads/inverseSemaphore.h"
#include "vUtilities/debugLog.h"

//==========================================================================
// Class:			InverseSemaphore
// Function:		InverseSemaphore
//
// Description:		Constructor for the InverseSemaphore class.
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
InverseSemaphore::InverseSemaphore()
{
	// Initialize the count to zero
	count = 0;
}

//==========================================================================
// Class:			InverseSemaphore
// Function:		~InverseSemaphore
//
// Description:		Destructor for the InverseSemaphore class.
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
InverseSemaphore::~InverseSemaphore()
{
	// FIXME:  Check to make sure we have no pending threads?
}

//==========================================================================
// Class:			InverseSemaphore
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
//		InverseSemaphoreError (error codes for this class)
//
//==========================================================================
InverseSemaphore::InverseSemaphoreError InverseSemaphore::Post(void)
{
	// Lock the mutex
	wxMutexLocker lock(countMutex);
	DebugLog::GetInstance()->Log(_T("InverseSemaphore::Post (locker)"));

	// Make sure we acquired the lock
	if (!lock.IsOk())
		return ErrorMutex;

	// Make sure we're not already at zero
	if (count == 0)
		return ErrorUnderflow;

	// Decrement the counter and return success
	count--;

	return ErrorNone;
}

//==========================================================================
// Class:			InverseSemaphore
// Function:		Set
//
// Description:		Sets the counter to the desired value.
//
// Input Arguments:
//		_count	= unsigned int to which the counter will be set
//
// Output Arguments:
//		None
//
// Return Value:
//		InverseSemaphore_ERROR (error codes for this class)
//
//==========================================================================
InverseSemaphore::InverseSemaphoreError InverseSemaphore::Set(unsigned int _count)
{
	// Lock the mutex
	wxMutexLocker lock(countMutex);
	DebugLog::GetInstance()->Log(_T("InverseSemaphore::Set (locker)"));

	// Make sure the lock was successfully acquired
	if (!lock.IsOk())
		return ErrorMutex;

	// Make sure the count is zero
	if (count != 0)
		return ErrorBusy;

	// Set the count to the desired value
	count = _count;

	return ErrorNone;
}

//==========================================================================
// Class:			InverseSemaphore
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
//		InverseSemaphoreError (error codes for this class)
//
//==========================================================================
InverseSemaphore::InverseSemaphoreError InverseSemaphore::Wait(void)
{
	// Wait for the count to return to zero
	while (count > 0)
	{
		// Wait and yield to other threads
		// FIXME:  Should we sleep and yield or do nothing? (currently sleeping and yielding)
		wxSafeYield();
		wxMilliSleep(50);
	}

	return ErrorNone;
}