/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  threadJob.cpp
// Date:  11/3/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the ThreadJob class.  This is the
//        class containing information that is stored in the job queue, i.e. this
//        class is used for communication from the GUI thread to the worker threads
//        (in that direction only - reverse communication is handled with events).

// Local headers
#include "VVASE/core/threads/threadJob.h"
#include "VVASE/core/threads/threadData.h"

// Standard C++ headers
#include <cassert>

namespace VVASE
{

//==========================================================================
// Class:			ThreadJob
// Function:		ThreadJob
//
// Description:		Constructor for the ThreadJob class (default).
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
ThreadJob::ThreadJob() : command(ThreadJob::CommandThreadNull)
{
}

//==========================================================================
// Class:			ThreadJob
// Function:		ThreadJob
//
// Description:		Constructor for the ThreadJob class (basic).
//
// Input Arguments:
//		command		= ThreadCommand specifying the command type for this job
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ThreadJob::ThreadJob(ThreadCommand command) : command(command)
{
	// Only permit certain types of jobs
	assert(command == ThreadJob::CommandThreadExit ||
		command == ThreadJob::CommandThreadStarted ||
		command == ThreadJob::CommandThreadNull);
}

//==========================================================================
// Class:			ThreadJob
// Function:		ThreadJob
//
// Description:		Constructor for the ThreadJob class.
//
// Input Arguments:
//		command	= ThreadCommand specifying the command type for this job
//		data	= std::unique_ptr<ThreadData>
//		name	= const vvaseString& Name of the car
//		index	= int& representing the object index for the associated object
//				  in the MainFrame
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ThreadJob::ThreadJob(ThreadCommand command, std::unique_ptr<ThreadData> data,
	const vvaseString &name, int &index) : command(command), data(std::move(data)),
	name(name.c_str()), index(index)
{
	assert(this->data);
	assert(this->data->OkForCommand(command));
}

//==========================================================================
// Class:			ThreadJob
// Function:		ThreadJob
//
// Description:		Copy constructor for the ThreadJob class.
//
// Input Arguments:
//		threadJob	= const ThreadJob& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ThreadJob::ThreadJob(const ThreadJob &threadJob)
{
	*this = threadJob;
}

//==========================================================================
// Class:			ThreadJob
// Function:		ThreadJob
//
// Description:		Move constructor for the ThreadJob class.
//
// Input Arguments:
//		threadJob	= const ThreadJob&& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ThreadJob::ThreadJob(ThreadJob &&threadJob)
{
	*this = std::move(threadJob);
}

//==========================================================================
// Class:			ThreadJob
// Function:		operator=
//
// Description:		Assignment operator.
//
// Input Arguments:
//		job	= const ThreadJob& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		ThreadJob& reference to this
//
//==========================================================================
ThreadJob& ThreadJob::operator=(const ThreadJob &job)
{
	// Check for self-assignment
	if (this == &job)
		return *this;

	// Do the copy
	command = job.command;
	index = job.index;
	name = job.name.c_str();// Force deep copy for thread-safety
	*data = *job.data;

	return *this;
}

//==========================================================================
// Class:			ThreadJob
// Function:		operator=
//
// Description:		Move assignment operator.
//
// Input Arguments:
//		job	= ThreadJob&& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		ThreadJob& reference to this
//
//==========================================================================
ThreadJob& ThreadJob::operator=(ThreadJob &&job)
{
	// Check for self-assignment
	if (this == &job)
		return *this;

	// Do the copy
	command = job.command;
	index = job.index;
	name = job.name.c_str();// Force deep copy for thread-safety
	data = std::move(job.data);

	return *this;
}

}// namespace VVASE
