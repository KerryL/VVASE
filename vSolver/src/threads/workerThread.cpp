/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  workerThread.cpp
// Created:  11/3/2009
// Author:  K. Loux
// Description:  Contains the class definition for the WorkerThread class.  Derives
//				 from wxWidgets thread classes.  Contains a process for checking a queue
//				 for jobs, pulling jobs from the queue and executing them, and
//				 communicating back to the main thread.
// History:

// Standard C++ headers
#include <cassert>

// VVASE headers
#include "vSolver/threads/workerThread.h"
#include "vSolver/threads/jobQueue.h"
#include "vSolver/threads/threadJob.h"
#include "vSolver/threads/kinematicsData.h"
#include "vSolver/threads/optimizationData.h"
#include "vSolver/physics/kinematics.h"
#include "vSolver/optimization/geneticAlgorithm.h"
#include "vCar/car.h"
#include "vUtilities/debugger.h"
#include "vUtilities/debugLog.h"

//==========================================================================
// Class:			WorkerThread
// Function:		WorkerThread
//
// Description:		Constructor for the WorkerThread class.
//
// Input Arguments:
//		_jobQueue	= JobQueue*, pointing to the queue from which this
//					  thread will pull jobs
//		_Id			= int representing this thread's ID number
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
WorkerThread::WorkerThread(JobQueue* _jobQueue, int _id)
							 : jobQueue(_jobQueue), id(_id)
{
	// Make sure the job queue exists
	assert(_jobQueue);

	// Create the thread object
	wxThread::Create();
}

//==========================================================================
// Class:			WorkerThread
// Function:		~WorkerThread
//
// Description:		Destructor for the WorkerThread class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
WorkerThread::~WorkerThread()
{
}

//==========================================================================
// Class:			WorkerThread
// Function:		Entry
//
// Description:		Main work loop for each thread.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxThread::ExitCode
//
//==========================================================================
wxThread::ExitCode WorkerThread::Entry(void)
{
	ThreadJob::ThreadCommand error;

	// Tell the main thread that we successfully started
	jobQueue->Report(ThreadJob::CommandThreadStarted, id);

	// Run the main loop (job handler) until it throws an exception
	try
	{
		while (true)
			OnJob();
	}
	catch (ThreadJob::ThreadCommand& i)
	{
		// Report the error
		jobQueue->Report(error = i, id);
	}

	// Return the exit code
	return (wxThread::ExitCode)error;
}

//==========================================================================
// Class:			WorkerThread
// Function:		OnJob
//
// Description:		The function that process jobs as they enter the queue.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void WorkerThread::OnJob()
{
	// Get a job from the queue
	// If the queue is empty, this blocks the thread
	ThreadJob job = jobQueue->Pop();

	// Declare timing variables
	wxDateTime start;

	// Process each command in the appropriate way
	switch(job.command)
	{
	case ThreadJob::CommandThreadExit:
		// Throw the exit command to confirm the quit
		throw ThreadJob::CommandThreadExit;

	case ThreadJob::CommandThreadKinematicsNormal:
	case ThreadJob::CommandThreadKinematicsIteration:
	case ThreadJob::CommandThreadKinematicsGA:
		// Do the kinematics calculations
		DebugLog::GetInstance()->Log(_T("SetInputs - Start"), 1);
		kinematicAnalysis.SetInputs(static_cast<KinematicsData*>(job.data)->kinematicInputs);
		DebugLog::GetInstance()->Log(_T("SetInputs - End"), -1);
		DebugLog::GetInstance()->Log(_T("UpdateKinematics - Start"), 1);
		kinematicAnalysis.UpdateKinematics(static_cast<KinematicsData*>(job.data)->originalCar,
			static_cast<KinematicsData*>(job.data)->workingCar, job.name);
		DebugLog::GetInstance()->Log(_T("UpdateKinematics - End"), -1);

		// Get the outputs
		DebugLog::GetInstance()->Log(_T("GetOutputs - Start"), 1);
		*(static_cast<KinematicsData*>(job.data)->output) = kinematicAnalysis.GetOutputs();
		DebugLog::GetInstance()->Log(_T("GetOutputs - End"), -1);

		// Tell the main thread that we've done the job
	    jobQueue->Report(job.command, id, job.index);
		break;

	case ThreadJob::CommandThreadGeneticOptimization:
		// Store the current time for determining elapsed time
		start = wxDateTime::UNow();

		// The genetic algorithm object MUST have been initialized prior to the call to this thread
		// Run the GA object - this will only return after the analysis is complete for all generations,
		// and the target object has been updated
		DebugLog::GetInstance()->Log(_T("Optimization - Start"), 1);
		static_cast<OptimizationData*>(job.data)->geneticAlgorithm->PerformOptimization();
		DebugLog::GetInstance()->Log(_T("Optimization - End"), -1);

		// Determine elapsed time and print to the screen
		Debugger::GetInstance() << "Elapsed Time: %s"
			<< wxDateTime::UNow().Subtract(start).Format() << Debugger::PriorityVeryHigh;

		// Tell the main thread that we're done the job
		jobQueue->Report(job.command, id, job.index);
		break;

	case ThreadJob::CommandThreadNull:
	default:
		break;
	}

	// If the data variable is not NULL, delete it
	if (job.data)
	{
		delete job.data;
		job.data = NULL;
	}
}
