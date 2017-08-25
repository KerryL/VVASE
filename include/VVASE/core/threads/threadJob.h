/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  threadJob.h
// Date:  11/3/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the ThreadJob class.  This is the
//        class containing information that is stored in the job queue, i.e. this
//        class is used for communication from the GUI thread to the worker threads
//        (in that direction only - reverse communication is handled with events).

#ifndef THREAD_JOB_H_
#define THREAD_JOB_H_

// Local headers
#include "VVASE/core/utilities/vvaseString.h"

// Standard C++ headers
#include <memory>

namespace VVASE
{

// Local forward declarations
class ThreadData;

class ThreadJob
{
public:
	// Commands to send to worker thread
	enum ThreadCommand
	{
		CommandThreadExit = -1,
		CommandThreadNull =  0,
		CommandThreadStarted,
		CommandThreadKinematicsNormal,
		CommandThreadKinematicsSweep,
		CommandThreadKinematicsGA,
		CommandThreadGeneticOptimization
	};
// TODO:  Make this more object oriented

	ThreadJob();
	ThreadJob(const ThreadJob &threadJob);
	ThreadJob(ThreadJob &&threadJob);
	ThreadJob(ThreadCommand command);
	ThreadJob(ThreadCommand command, std::unique_ptr<ThreadData> data,
		const vvaseString &name, int &index);
	~ThreadJob() = default;

	ThreadCommand command;

	std::unique_ptr<ThreadData> data;
	vvaseString name;
	int index;

	// Operators
	ThreadJob& operator=(const ThreadJob &job);
	ThreadJob& operator=(ThreadJob &&job);
};

}// namespace VVASE

#endif// THREAD_JOB_H_
