/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  jobQueue.h
// Date:  11/3/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the JobQueue class.  This queue
//        handles task priorities and is used to communicate from the GUI thread
//        to the worker threads (add tasks here to be completed by the workers).

#ifndef JOB_QUEUE_H_
#define JOB_QUEUE_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "vSolver/threads/threadJob.h"

// Standard C++ headers
#include <map>

// wxWidgets forward declarations
class wxEvtHandler;

namespace VVASE
{

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

	JobQueue(wxEvtHandler *parent);

	void AddJob(const ThreadJob& job, const JobPriority& priority = PriorityNormal);

	ThreadJob Pop();

	// Reports a message back to the main event handler
	void Report(const ThreadJob::ThreadCommand& command, int threadId, int objectID = 0);

	size_t PendingJobs();

	wxEvtHandler *GetParent() { return parent; }

private:
    wxEvtHandler *parent;

	// Use of a multimap allow prioritization - lower keys come first, jobs with equal keys are appended
    std::multimap<JobPriority, ThreadJob> jobs;

	// Thread protection objects
    wxMutex mutexQueue;
    wxSemaphore queueCount;
};

}// namespace VVASE

#endif// JOB_QUEUE_H_
