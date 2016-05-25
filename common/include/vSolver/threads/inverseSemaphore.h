/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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

#ifndef INVERSE_SEMAPHORE_H_
#define INVERSE_SEMAPHORE_H_

// wxWidgets headers
#include <wx/thread.h>

class InverseSemaphore
{
public:
	InverseSemaphore();
	~InverseSemaphore();

	// Error codes for this object
	enum InverseSemaphoreError
	{
		ErrorNone,		// No errors
		ErrorMutex,		// Error in mutex object (or wxMutexLocker)
		ErrorUnderflow,	// Post() would make count negative
		ErrorBusy		// Wait() called when object was already in use
	};

	InverseSemaphoreError Post();
	InverseSemaphoreError Set(unsigned int count);

	InverseSemaphoreError Wait();

	unsigned int GetCount() { return count; }

private:
	wxMutex countMutex;

	unsigned int count;
};

#endif// INVERSE_SEMAPHORE_H_