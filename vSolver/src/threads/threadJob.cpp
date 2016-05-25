/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  threadJob.cpp
// Created:  11/3/2009
// Author:  K. Loux
// Description:  Contains the class definition for the ThreadJob class.  This is the
//				 class containing information that is stored in the job queue, i.e. this
//				 class is used for communication from the GUI thread to the worker threads
//				 (in that direction only - reverse communication is handled with events).
// History:

// VVASE headers
#include "vSolver/threads/threadJob.h"
#include "vSolver/threads/threadData.h"

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
ThreadJob::ThreadJob() : command(ThreadJob::CommandThreadNull), data(NULL)
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
ThreadJob::ThreadJob(ThreadCommand command) : command(command), data(NULL)
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
//		name	= const wxString& Name of the car
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
ThreadJob::ThreadJob(ThreadCommand command, ThreadData *data,
	const wxString &name, int &index) : command(command), data(data),
	name(name.c_str()), index(index)
{
	assert(data);
	assert(data->OkForCommand(command));
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
	data = NULL;
	*this = threadJob;
}

//==========================================================================
// Class:			ThreadJob
// Function:		~ThreadJob
//
// Description:		Destructor for the ThreadJob class.
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
ThreadJob::~ThreadJob()
{
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
	data = job.data;
	
	return *this;
}