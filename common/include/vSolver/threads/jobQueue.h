/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  jobQueue.h
// Created:  11/3/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the JOB_QUEUE class.  This queue
//				 handles task priorities and is used to communicate from the GUI thread
//				 to the worker threads (add tasks here to be completed by the workers).
// History:

#ifndef _JOB_QUEUE_H_
#define _JOB_QUEUE_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vSolver/threads/threadJob.h"

// Standard C++ headers
#include <map>

// wxWidgets forward declarations
class wxEvtHandler;

class JobQueue
{
public:
	// Enumeration of job priority levels
	enum JobPriority
	{
		PriorityVeryHigh,
		PriorityHigh,
		PriorityNormal,
		PriorityLow,
		PriorityVeryLow,
		PriorityIdle
	};

	// Constructor
	JobQueue(wxEvtHandler *_parent);

	// Adds a job to the queue
	void AddJob(const ThreadJob& job, const JobPriority& priority = PriorityNormal);

	// Pulls a job from the queue to be completed
	ThreadJob Pop(void);

	// Reports a message back to the main event handler
	void Report(const ThreadJob::ThreadCommands& command, int threadId, int objectID = 0);

	// Returns the number of pending jobs
	size_t PendingJobs(void);

	// Returns a pointer to the event handler
	wxEvtHandler *GetParent(void) { return parent; };

private:
	// The main thread's event handler
    wxEvtHandler *parent;

	// Use of a multimap allow prioritization - lower keys come first, jobs with equal keys are appended
    std::multimap<JobPriority, ThreadJob> jobs;

	// Thread protection objects
    wxMutex mutexQueue;
    wxSemaphore queueCount;
};

#endif// _JOB_QUEUE_H_