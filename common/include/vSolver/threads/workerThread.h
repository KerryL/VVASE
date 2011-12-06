/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  workerThread.h
// Created:  11/3/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the WORKER_THREAD class.  Derives
//				 from wxWidgets thread classes.  Contains a process for checking a queue
//				 for jobs, pulling jobs from the queue and executing them, and
//				 communicating back to the main thread.
// History:

#ifndef _WORKER_THREAD_H_
#define _WORKER_THREAD_H_

// wxWidgets headers
#include <wx/thread.h>

// Local headers
#include "vSolver/physics/kinematics.h"

// VVASE forward declarations
class JobQueue;

class WorkerThread : public wxThread
{
public:
	// Constructor
	WorkerThread(JobQueue* _jobQueue, int _id = wxID_ANY);

	// Destructor
	~WorkerThread();

private:
	// The queue of jobs to be completed
	JobQueue* jobQueue;

	// This thread's ID number
	int id;

	// The thread's entry point
	virtual wxThread::ExitCode Entry(void);

	// The job handler (called when this thread pulls a job from the queue)
	virtual void OnJob(void);

	// The objects that do the number crunching
	Kinematics kinematicAnalysis;
};

#endif// _WORKER_THREAD_H_