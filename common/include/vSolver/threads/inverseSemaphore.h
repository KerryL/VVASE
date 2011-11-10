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

#ifndef _INVERSE_SEMAPHORE_CLASS_H_
#define _INVERSE_SEMAPHORE_CLASS_H_

// wxWidgets headers
#include <wx/thread.h>

class INVERSE_SEMAPHORE
{
public:
	// Constructor
	INVERSE_SEMAPHORE();

	// Destructor
	~INVERSE_SEMAPHORE();

	// Error codes for this object
	enum INVERSE_SEMAPHORE_ERROR
	{
		ErrorNone,		// No errors
		ErrorMutex,		// Error in mutex object (or wxMutexLocker)
		ErrorUnderflow,	// Post() would make count negative
		ErrorBusy		// Wait() called when object was already in use
	};

	// Decrements the counter
	INVERSE_SEMAPHORE_ERROR Post(void);

	// Sets the counter
	INVERSE_SEMAPHORE_ERROR Set(unsigned int _Count);

	// Blocks the calling thread until Count is decremented
	INVERSE_SEMAPHORE_ERROR Wait(void);

	// Retrieves the current count
	unsigned int GetCount(void) { return Count; };

private:
	// This object's synchronization
	wxMutex CountMutex;

	// The remining count before the calling thread is freed
	volatile unsigned int Count;
};

#endif// _INVERSE_SEMAPHORE_CLASS_H_