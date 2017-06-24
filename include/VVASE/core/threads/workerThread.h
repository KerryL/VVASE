/*===================================================================================
                                       VVASE
                         Copyright Kerry R. Loux 2008-2017
===================================================================================*/

// File:  workerThread.h
// Date:  11/3/2009
// Auth:  K. Loux
// Desc:  Contains the class declaration for the WORKER_THREAD class.  Derives
//        from wxWidgets thread classes.  Contains a process for checking a queue
//        for jobs, pulling jobs from the queue and executing them, and
//        communicating back to the main thread.

#ifndef WORKER_THREAD_H_
#define WORKER_THREAD_H_

// wxWidgets headers
#include <wx/thread.h>

// Local headers
#include "vSolver/physics/kinematics.h"

namespace VVASE
{

// Local forward declarations
class JobQueue;

class WorkerThread : public wxThread
{
public:
	WorkerThread(JobQueue* jobQueue, int id = wxID_ANY);
	~WorkerThread();

private:
	JobQueue* jobQueue;

	int id;
	virtual wxThread::ExitCode Entry();

	// The job handler (called when this thread pulls a job from the queue)
	virtual void OnJob();

	Kinematics kinematicAnalysis;
};

}// namespace VVASE

#endif// WORKER_THREAD_H_

