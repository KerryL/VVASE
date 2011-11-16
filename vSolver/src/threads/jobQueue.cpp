/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011
_class
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
JobQueue::JobQueue(wxEvtHandler *_parent) : parent(_parent)
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
	// Lock the queue (lock expires when this function returns)
	wxMutexLocker lock(mutexQueue);

	// Add the job to the queue
	jobs.insert(std::pair<JobPriority, ThreadJob>(priority, job));

	// Increment the semaphore
	queueCount.Post();

	return;
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

	// Wait for the number of pending jobs to be positive
	queueCount.Wait();

	// Lock the queue
	mutexQueue.Lock();

	// Get the first job from the queue (prioritization occurs automatically)
	nextJob = jobs.begin()->second;

	// Remove the job from the queue
	jobs.erase(jobs.begin());

	// Unlock the queue
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
//		Command		= const THREAD_COMMANDS& to report
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
void JobQueue::Report(const THREAD_JOB::THREAD_COMMANDS& Command, int ThreadId, int ObjectID)
{
	// Create event with the specified command
	wxCommandEvent evt(EVT_THREAD, Command);

	// Set the thread ID
	evt.SetId(ThreadId);

	// The type of command that has completed is also important
	evt.SetInt((int)Command);

	// The object's ID number gets passed back as well
	evt.SetExtraLong(ObjectID);

	// Add it to the parent's event queue
	Parent->AddPendingEvent(evt);

	return;
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
	// Lock the queue while we read the size
	wxMutexLocker lock(MutexQueue);

	return Jobs.size();
}