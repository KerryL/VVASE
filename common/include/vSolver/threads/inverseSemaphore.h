/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  inverseSemaphore.h
// Created:  4/10/2010
// Author:  K. Loux
// Description:  This class is a synchronization object for multi-threaded applications.
//				 After a counter is set, it blocks the calling thread until the counter
//				 is decremented to zero.
// History:

#ifndef _INVERSE_SEMAPHORE_H_
#define _INVERSE_SEMAPHORE_H_

// wxWidgets headers
#include <wx/thread.h>

class InverseSemaphore
{
public:
	// Constructor
	InverseSemaphore();

	// Destructor
	~InverseSemaphore();

	// Error codes for this object
	enum InverseSemaphoreError
	{
		ErrorNone,		// No errors
		ErrorMutex,		// Error in mutex object (or wxMutexLocker)
		ErrorUnderflow,	// Post() would make count negative
		ErrorBusy		// Wait() called when object was already in use
	};

	// Decrements the counter
	InverseSemaphoreError Post(void);

	// Sets the counter
	InverseSemaphoreError Set(unsigned int _count);

	// Blocks the calling thread until Count is decremented
	InverseSemaphoreError Wait(void);

	// Retrieves the current count
	unsigned int GetCount(void) { return count; };

private:
	// This object's synchronization
	wxMutex countMutex;

	// The remaining count before the calling thread is freed
	unsigned int count;
};

#endif// _INVERSE_SEMAPHORE_H_