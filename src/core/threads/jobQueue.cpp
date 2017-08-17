/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  jobQueue.cpp
// Date:  11/3/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the JobQueue class.  This queue
//        handles task priorities and is used to communicate from the main thread
//        to the worker threads (add tasks here to be completed by the workers).

// Local headers
#include "VVASE/core/threads/jobQueue.h"
#include "VVASE/core/threads/threadEvent.h"// TODO:  Need to remove this from core
#include "VVASE/core/utilities/debugLog.h"
#include "VVASE/core/threads/threadDefs.h"

namespace VVASE
{

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
//		priority	= const Priority& of the new job
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void JobQueue::AddJob(const ThreadJob& job, const Priority& priority)
{
	MutexLocker lock(mutexQueue);
	DebugLog::GetInstance()->Log(_T("JobQueue::AddJob (locker)"));

	jobs.insert(std::make_pair(priority, job));
	jobReadyCondition.notify_one();
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
	UniqueMutexLocker lock(mutexQueue);
	jobReadyCondition.wait(lock, [this](){ return !jobs.empty(); });
	DebugLog::GetInstance()->Log(_T("JobQueue::Pop (lock)"));

	// Get the first job from the queue (prioritization occurs automatically)
	ThreadJob nextJob(jobs.begin()->second);
	jobs.erase(jobs.begin());

	DebugLog::GetInstance()->Log(_T("JobQueue::Pop (unlock)"));

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
//		ThreadId	= std::thread::id representing the thread's ID
//		ObjectId	= int representing the object's ID
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void JobQueue::Report(const ThreadJob::ThreadCommand& command, std::thread::id threadId, int objectId)
{
	/*wxCommandEvent evt(EVT_THREAD, command);

	evt.SetId(std::hash<std::thread::id>()(threadId));// TODO:  Not guaranteed unique?
	evt.SetInt(static_cast<int>(command));
	evt.SetExtraLong(objectId);

	parent->AddPendingEvent(evt);*/// TODO:  Re-implement some other way
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
	MutexLocker lock(mutexQueue);
	DebugLog::GetInstance()->Log(_T("JobQueue::PendingJobs (locker)"));

	return jobs.size();
}

}// namespace VVASE
