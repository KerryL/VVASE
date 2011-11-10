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
// Description:  Contains the class definition for the JOB_QUEUE class.  This queue
//				 handles task priorities and is used to communicate from the main thread
//				 to the worker threads (add tasks here to be completed by the workers).
// History:

// VVASE headers
#include "vSolver/threads/jobQueue.h"
#include "vSolver/threads/threadEvent.h"

//==========================================================================
// Class:			JOB_QUEUE
// Function:		JOB_QUEUE
//
// Description:		Constructor for the JOB_QUEUE class (default).
//
// Input Arguments:
//		_Parent		= wxEventHandler* pointing to the main application object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
JOB_QUEUE::JOB_QUEUE(wxEvtHandler *_Parent) : Parent(_Parent)
{
}

//==========================================================================
// Class:			JOB_QUEUE
// Function:		AddJob
//
// Description:		Adds a job to the queue
//
// Input Arguments:
//		Job			= const THREAD_JOB& to be added to the queue
//		Priority	= const JOB_PRIORITY& of the new job
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void JOB_QUEUE::AddJob(const THREAD_JOB& Job, const JOB_PRIORITY& Priority)
{
	// Lock the queue (lock expires when this function returns)
	wxMutexLocker lock(MutexQueue);

	// Add the job to the queue
	Jobs.insert(std::pair<JOB_PRIORITY, THREAD_JOB>(Priority, Job));

	// Increment the semaphore
	QueueCount.Post();

	return;
}

//==========================================================================
// Class:			JOB_QUEUE
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
//		THREAD_JOB to be executed
//
//==========================================================================
THREAD_JOB JOB_QUEUE::Pop()
{
	THREAD_JOB NextJob;

	// Wait for the number of pending jobs to be positive
	QueueCount.Wait();

	// Lock the queue
	MutexQueue.Lock();

	// Get the first job from the queue (prioritization occurs automatically)
	NextJob = Jobs.begin()->second;

	// Remove the job from the queue
	Jobs.erase(Jobs.begin());

	// Unlock the queue
	MutexQueue.Unlock();

	return NextJob;
}

//==========================================================================
// Class:			JOB_QUEUE
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
void JOB_QUEUE::Report(const THREAD_JOB::THREAD_COMMANDS& Command, int ThreadId, int ObjectID)
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
// Class:			JOB_QUEUE
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
size_t JOB_QUEUE::PendingJobs()
{
	// Lock the queue while we read the size
	wxMutexLocker lock(MutexQueue);

	return Jobs.size();
}