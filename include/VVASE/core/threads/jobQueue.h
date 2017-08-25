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

// We need to fix the use of macros in Win32 libraries
#ifdef _MSC_VER
#undef AddJob
#endif// _MSC_VER

// Local headers
#include "VVASE/core/threads/threadJob.h"

// Standard C++ headers
#include <map>
#include <condition_variable>
#include <thread>

namespace VVASE
{

class JobQueue
{
public:
	// Enumeration of job priority levels
	enum class Priority
	{
		VeryHigh,
		High,
		Normal,
		Low,
		VeryLow,
		Idle
	};

	virtual ~JobQueue() = default;

	virtual void Report(const ThreadJob::ThreadCommand& /*command*/, const std::thread::id& /*threadId*/, int /*objectId*/ = 0) {};

	void AddJob(ThreadJob&& job, const Priority& priority = Priority::Normal);

	ThreadJob Pop();

	size_t PendingJobs();

private:
	// Use of a multimap allow prioritization - lower keys come first, jobs with equal keys are appended
    std::multimap<Priority, ThreadJob> jobs;// TODO:  Is prioritization necessary?  If yes, maybe have separate queues for each priority?  This doesn't seem particularly efficient...

	// Thread protection objects
    std::mutex mutexQueue;
	std::condition_variable jobReadyCondition;
};

}// namespace VVASE

#endif// JOB_QUEUE_H_
