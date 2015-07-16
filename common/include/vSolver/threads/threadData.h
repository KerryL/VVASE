/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  threadData.h
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the THREAD_DATA class.  This is an
//				 abstract class from which objects should be derived to pass data to
//				 different types of threads.  Using this class allows for a cleaner
//				 thread management system, where the queue only needs to know about
//				 this class, not any specific data for threads that perform specific
//				 tasks.
// History:

#ifndef THREAD_DATA_H_
#define THREAD_DATA_H_

// VVASE headers
#include "vSolver/threads/threadJob.h"

class ThreadData
{
public:
	ThreadData();
	virtual ~ThreadData();

	// For checking to make sure the proper data was used with
	// each command type
	virtual bool OkForCommand(ThreadJob::ThreadCommand &command) = 0;
};

#endif// THREAD_DATA_H_