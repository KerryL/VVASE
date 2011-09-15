/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  job_queue_class.h
// Created:  11/3/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the JOB_QUEUE class.  This queue
//				 handles task priorities and is used to communicate from the GUI thread
//				 to the worker threads (add tasks here to be completed by the workers).
// History:

#ifndef _JOB_QUEUE_CLASS_H_
#define _JOB_QUEUE_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vSolver/threads/thread_job_class.h"

// Standard C++ headers
#include <map>

// wxWidgets forward declarations
class wxEvtHandler;

class JOB_QUEUE
{
public:
	// Enumeration of job priority levels
	enum JOB_PRIORITY
	{
		PRIORITY_VERY_HIGH,
		PRIORITY_HIGH,
		PRIORITY_NORMAL,
		PRIORITY_LOW,
		PRIORITY_VERY_LOW,
		PRIORITY_IDLE
	};

	// Constructor
	JOB_QUEUE(wxEvtHandler *_Parent);

	// Adds a job to the queue
	void AddJob(const THREAD_JOB& Job, const JOB_PRIORITY& Priority = PRIORITY_NORMAL);

	// Pulls a job from the queue to be completed
	THREAD_JOB Pop(void);

	// Reports a message back to the main event handler
	void Report(const THREAD_JOB::THREAD_COMMANDS& Command, int ThreadId, int ObjectID = 0);

	// Returns the number of pending jobs
	size_t PendingJobs(void);

	// Returns a pointer to the event handler
	wxEvtHandler *GetParent(void) { return Parent; };

private:
	// The main thread's event handler
    wxEvtHandler *Parent;

	// Use of a multimap allow prioritization - lower keys come first, jobs with equal keys are appended
    std::multimap<JOB_PRIORITY, THREAD_JOB> Jobs;

	// Thread protection objects
    wxMutex MutexQueue;
    wxSemaphore QueueCount;
};

#endif// _JOB_QUEUE_CLASS_H_