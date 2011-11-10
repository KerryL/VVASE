/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  workerThread.cpp
// Created:  11/3/2009
// Author:  K. Loux
// Description:  Contains the class definition for the WORKER_THREAD class.  Derives
//				 from wxWidgets thread classes.  Contains a process for checking a queue
//				 for jobs, pulling jobs from the queue and executing them, and
//				 communicating back to the main thread.
// History:

// Standard C++ headers
#include <assert.h>

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

//==========================================================================
// Class:			WORKER_THREAD
// Function:		WORKER_THREAD
//
// Description:		Constructor for the WORKER_THREAD class.
//
// Input Arguments:
//		_JobQueue	= JOB_QUEUE*, pointing to the queue from which this
//					  thread will pull jobs
//		_debugger	= Debugger& reference to the application's debugger object
//		_Id			= int representing this thread's ID number
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
WORKER_THREAD::WORKER_THREAD(JOB_QUEUE* _JobQueue, Debugger &_debugger, int _Id)
							 : debugger(_debugger), JobQueue(_JobQueue), Id(_Id)
{
	// Make sure the job queue exists
	assert(_JobQueue);

	// Create the thread object
	wxThread::Create();

	// Create the analysis object
	KinematicAnalysis = new KINEMATICS(debugger);
}

//==========================================================================
// Class:			WORKER_THREAD
// Function:		WORKER_THREAD
//
// Description:		Constructor for the WORKER_THREAD class.
//
// Input Arguments:
//		_JobQueue	= JOB_QUEUE*, pointing to the queue from which this
//					  thread will pull jobs
//		Debugger	= Debugger& reference to the application's debugger object
//		_Id			= int representing this thread's ID number
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
WORKER_THREAD::~WORKER_THREAD()
{
	// Delete the analysis object
	delete KinematicAnalysis;
	KinematicAnalysis = NULL;
}

//==========================================================================
// Class:			WORKER_THREAD
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
wxThread::ExitCode WORKER_THREAD::Entry(void)
{
	THREAD_JOB::THREAD_COMMANDS Error;

	// Tell the main thread that we successfully started
	JobQueue->Report(THREAD_JOB::COMMAND_THREAD_STARTED, Id);

	// Run the main loop (job handler) until it throws an exception
	try
	{
		while (true)
			OnJob();
	}
	catch (THREAD_JOB::THREAD_COMMANDS& i)
	{
		// Report the error
		JobQueue->Report(Error = i, Id);
	}

	// Return the exit code
	return (wxThread::ExitCode)Error;
}

//==========================================================================
// Class:			WORKER_THREAD
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
void WORKER_THREAD::OnJob()
{
	// Get a job from the queue
	// If the queue is empty, this blocks the thread
	THREAD_JOB Job = JobQueue->Pop();

	// Declare timing variables
	wxDateTime Start;

	// Process each command in the appropriate way
	switch(Job.Command)
	{
	case THREAD_JOB::COMMAND_THREAD_EXIT:
		// Throw the exit command to confirm the quit
		throw THREAD_JOB::COMMAND_THREAD_EXIT;

	case THREAD_JOB::COMMAND_THREAD_KINEMATICS_NORMAL:
	case THREAD_JOB::COMMAND_THREAD_KINEMATICS_ITERATION:
	case THREAD_JOB::COMMAND_THREAD_KINEMATICS_FOR_GA:
		// Do the kinematics calculations
		KinematicAnalysis->SetInputs(static_cast<KINEMATICS_DATA*>(Job.Data)->KinematicInputs);
		KinematicAnalysis->UpdateKinematics(static_cast<KINEMATICS_DATA*>(Job.Data)->OriginalCar,
			static_cast<KINEMATICS_DATA*>(Job.Data)->WorkingCar, Job.Name);

		// Get the outputs
		*(static_cast<KINEMATICS_DATA*>(Job.Data)->Output) = KinematicAnalysis->GetOutputs();

		// Tell the main thread that we've done the job
	    JobQueue->Report(Job.Command, Id, Job.Index);
		break;

	case THREAD_JOB::COMMAND_THREAD_GENETIC_OPTIMIZATION:
		// Store the current time for determining elapsed time
		Start = wxDateTime::UNow();

		// The genetic algorithm object MUST have been initialized prior to the call to this thread
		// Run the GA object - this will only return after the analysis is complete for all generations,
		// and the target object has been updated
		static_cast<OPTIMIZATION_DATA*>(Job.Data)->GeneticAlgorithm->PerformOptimization();

		// Determine elapsed time and print to the screen
		debugger.Print(Debugger::PriorityVeryHigh, "Elapsed Time: %s", wxDateTime::UNow().Subtract(Start).Format().c_str());

		// Tell the main thread that we're done the job
		JobQueue->Report(Job.Command, Id, Job.Index);
		break;

	case THREAD_JOB::COMMAND_THREAD_NULL:
	default:
		break;
	}

	// If the data variable is not NULL, delete it
	if (Job.Data)
	{
		delete Job.Data;
		Job.Data = NULL;
	}

	return;
}