/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  thread_data_class.h
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the THREAD_DATA class.  This is an
//				 abstract class from which objects should be derived to pass data to
//				 different types of threads.  Using this class allows for a cleaner
//				 thread management system, where the queue only needs to know about
//				 this class, not any specific data for threads that perform specific
//				 tasks.
// History:

#ifndef _THREAD_DATA_CLASS_H_
#define _THREAD_DATA_CLASS_H_

// VVASE headers
#include "vSolver/threads/thread_job_class.h"

class THREAD_DATA
{
public:
	// Constructors
	THREAD_DATA();

	// Destructor
	virtual ~THREAD_DATA();

	// For checking to make sure the proper data was used with
	// each command type
	virtual bool OkForCommand(THREAD_JOB::THREAD_COMMANDS &Command) = 0;
};

#endif// _THREAD_DATA_CLASS_H_