/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  jobQueue.cpp
// Created:  11/3/2009
// Author:  K. Loux
// Description:  Contains the class definition for the JobQueue class.  This queue
//				 handles task priorities and is used to communicate from the main thread
//				 to the worker threads (add tasks here to be completed by the workers).
// History:

// VVASE headers
#include "vSolver/threads/jobQueue.h"
#include "vSolver/threads/threadEvent.h"
#include "vUtilities/debugLog.h"

//==========================================================================
// Class:			JobQueue
// Function:		JobQueue
//
// Description:		Constructor for the JobQueue class (default).
//
// Input Arguments:
//		_parent		= wxEventHandler* pointing to the main application object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
JobQueue::JobQueue(wxEvtHandler *parent) : parent(parent)
{
}

//==========================================================================
// Class:			JobQueue
// Function:		AddJob
//
// Description:		Adds a job to the queue
//
// Input Arguments:
//		job			= const ThreadJob& to be added to the queue
//		priority	= const JobPriority& of the new job
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void JobQueue::AddJob(const ThreadJob& job, const JobPriority& priority)
{
	wxMutexLocker lock(mutexQueue);
	DebugLog::GetInstance()->Log(_T("JobQueue::AddJob (locker)"));

	jobs.insert(std::pair<JobPriority, ThreadJob>(priority, job));

	queueCount.Post();
}

//==========================================================================
// Class:			JobQueue
// Function:		Pop
//
// Description:		Pulls the next job from the queue.  Prioritization happens
//					in the std::map object, so prioritization is not seen
//					in this function.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		ThreadJob to be executed
//
//==========================================================================
ThreadJob JobQueue::Pop()
{
	ThreadJob nextJob;

	queueCount.Wait();

	mutexQueue.Lock();
	DebugLog::GetInstance()->Log(_T("JobQueue::Pop (lock)"));

	// Get the first job from the queue (prioritization occurs automatically)
	nextJob = jobs.begin()->second;

	jobs.erase(jobs.begin());

	DebugLog::GetInstance()->Log(_T("JobQueue::Pop (unlock)"));
	mutexQueue.Unlock();

	return nextJob;
}

//==========================================================================
// Class:			JobQueue
// Function:		Report
//
// Description:		Posts an event to the main thread.
//
// Input Arguments:
//		Command		= const ThreadCommand& to report
//		Message		= const wxString& containing string information
//		ThreadI		= int representing the thread's ID
//		ObjectID	= int representing the object's ID
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void JobQueue::Report(const ThreadJob::ThreadCommand& command, int threadId, int objectID)
{
	wxCommandEvent evt(EVT_THREAD, command);

	evt.SetId(threadId);
	evt.SetInt((int)command);
	evt.SetExtraLong(objectID);

	parent->AddPendingEvent(evt);
}

//==========================================================================
// Class:			JobQueue
// Function:		PendingJobs
//
// Description:		Returns the number of pending jobs.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		size_t indicating the number of pending jobs
//
//==========================================================================
size_t JobQueue::PendingJobs()
{
	wxMutexLocker lock(mutexQueue);
	DebugLog::GetInstance()->Log(_T("JobQueue::PendingJobs (locker)"));

	return jobs.size();
}