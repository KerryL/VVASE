/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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
InverseSemaphore::InverseSemaphoreError InverseSemaphore::Post()
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
//		count	= unsigned int to which the counter will be set
//
// Output Arguments:
//		None
//
// Return Value:
//		InverseSemaphore_ERROR (error codes for this class)
//
//==========================================================================
InverseSemaphore::InverseSemaphoreError InverseSemaphore::Set(unsigned int count)
{
	wxMutexLocker lock(countMutex);
	DebugLog::GetInstance()->Log(_T("InverseSemaphore::Set (locker)"));

	if (!lock.IsOk())
		return ErrorMutex;

	if (count != 0)
		return ErrorBusy;

	this->count = count;

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
InverseSemaphore::InverseSemaphoreError InverseSemaphore::Wait()
{
	countMutex.Lock();
	unsigned int testCount(count);
	countMutex.Unlock();
	
	// Wait for the count to return to zero
	while (testCount > 0)
	{
		// Wait and yield to other threads
		wxThread::Sleep(100);
		countMutex.Lock();
		testCount = count;
		countMutex.Unlock();
	}

	return ErrorNone;
}