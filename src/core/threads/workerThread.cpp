/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  workerThread.cpp
// Date:  11/3/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the WorkerThread class.  Derives
//        from wxWidgets thread classes.  Contains a process for checking a queue
//        for jobs, pulling jobs from the queue and executing them, and
//        communicating back to the main thread.

// Local headers
#include "VVASE/core/threads/workerThread.h"
#include "VVASE/core/threads/jobQueue.h"
#include "VVASE/core/threads/threadJob.h"
#include "VVASE/core/threads/kinematicsData.h"
#include "VVASE/core/threads/optimizationData.h"
#include "VVASE/core/analysis/kinematics.h"
#include "../optimization/geneticAlgorithm.h"
#include "VVASE/core/car/car.h"
#include "VVASE/core/utilities/debugger.h"
#include "VVASE/core/utilities/debugLog.h"
#include "VVASE/core/utilities/stopWatch.h"

// Standard C++ headers
#include <cassert>

namespace VVASE
{

//==========================================================================
// Class:			WorkerThread
// Function:		WorkerThread
//
// Description:		Constructor for the WorkerThread class.
//
// Input Arguments:
//		jobQueue	= JobQueue&, pointing to the queue from which this
//					  thread will pull jobs
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
WorkerThread::WorkerThread(JobQueue& jobQueue)
	: jobQueue(jobQueue)
{
	thread = std::thread(&WorkerThread::ThreadEntry, this);
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
	if (thread.joinable())
		thread.join();
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
//		void
//
//==========================================================================
void WorkerThread::ThreadEntry()
{
	ThreadJob::ThreadCommand error;

	// Tell the main thread that we successfully started
	jobQueue.Report(ThreadJob::CommandThreadStarted, std::this_thread::get_id());

	// Run the main loop (job handler) until it throws an exception
	try
	{
		while (true)
			OnJob();
	}
	catch (ThreadJob::ThreadCommand& i)
	{
		error = i;
		jobQueue.Report(i, std::this_thread::get_id());
	}
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
	ThreadJob job(jobQueue.Pop());

	StopWatch timer;
	timer.Start();

	switch(job.command)
	{
	case ThreadJob::CommandThreadExit:
		throw ThreadJob::CommandThreadExit;

	case ThreadJob::CommandThreadKinematicsNormal:
	case ThreadJob::CommandThreadKinematicsSweep:
	case ThreadJob::CommandThreadKinematicsGA:
		// Do the kinematics calculations
		DebugLog::GetInstance()->Log(_T("SetInputs - Start"), 1);
		kinematicAnalysis.SetInputs(static_cast<KinematicsData*>(job.data.get())->kinematicInputs);
		DebugLog::GetInstance()->Log(_T("SetInputs - End"), -1);
		DebugLog::GetInstance()->Log(_T("UpdateKinematics - Start"), 1);
		kinematicAnalysis.UpdateKinematics(static_cast<KinematicsData*>(job.data.get())->originalCar,
			static_cast<KinematicsData*>(job.data.get())->workingCar, job.name);
		DebugLog::GetInstance()->Log(_T("UpdateKinematics - End"), -1);

		// Get the outputs
		DebugLog::GetInstance()->Log(_T("GetOutputs - Start"), 1);
		*(static_cast<KinematicsData*>(job.data.get())->output) = kinematicAnalysis.GetOutputs();
		DebugLog::GetInstance()->Log(_T("GetOutputs - End"), -1);

	    jobQueue.Report(job.command, std::this_thread::get_id(), job.index);
		break;

	case ThreadJob::CommandThreadGeneticOptimization:
		// The genetic algorithm object MUST have been initialized prior to the call to this thread
		// Run the GA object - this will only return after the analysis is complete for all generations,
		// and the target object has been updated
		DebugLog::GetInstance()->Log(_T("Optimization - Start"), 1);
		static_cast<OptimizationData*>(job.data.get())->geneticAlgorithm->PerformOptimization();
		DebugLog::GetInstance()->Log(_T("Optimization - End"), -1);
		Debugger::GetInstance() << "Elapsed Time: %s"
			<< timer.GetElapsedTime<double, std::chrono::seconds>() << Debugger::Priority::VeryHigh;

		jobQueue.Report(job.command, std::this_thread::get_id(), job.index);
		break;

	case ThreadJob::CommandThreadNull:
	default:
		break;
	}
}

}// namespace VVASE
