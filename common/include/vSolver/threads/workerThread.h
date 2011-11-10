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

#ifndef _WORKER_THREAD_CLASS_H_
#define _WORKER_THREAD_CLASS_H_

// wxWidgets headers
#include <wx/thread.h>

// VVASE forward declarations
class JOB_QUEUE;
class KINEMATICS;
class Debugger;

class WORKER_THREAD : public wxThread
{
public:
	// Constructor
	WORKER_THREAD(JOB_QUEUE* _JobQueue, Debugger &_debugger, int _Id = wxID_ANY);

	// Destructor
	~WORKER_THREAD();

private:
	// Debugging object
	Debugger &debugger;

	// The queue of jobs to be completed
	JOB_QUEUE* JobQueue;

	// This thread's ID number
	int Id;

	// The thread's entry point
	virtual wxThread::ExitCode Entry(void);

	// The job handler (called when this thread pulls a job from the queue)
	virtual void OnJob(void);

	// The objects that do the number crunching
	KINEMATICS *KinematicAnalysis;
};

#endif// _WORKER_THREAD_CLASS_H_