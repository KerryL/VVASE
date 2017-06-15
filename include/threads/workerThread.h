/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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

#ifndef WORKER_THREAD_H_
#define WORKER_THREAD_H_

// wxWidgets headers
#include <wx/thread.h>

// Local headers
#include "vSolver/physics/kinematics.h"

// VVASE forward declarations
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

#endif// WORKER_THREAD_H_