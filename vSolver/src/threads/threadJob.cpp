/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  threadJob.cpp
// Created:  11/3/2009
// Author:  K. Loux
// Description:  Contains the class definition for the THREAD_JOB class.  This is the
//				 class containing information that is stored in the job queue, i.e. this
//				 class is used for communication from the GUI thread to the worker threads
//				 (in that direction only - reverse communication is handled with events).
// History:

// VVASE headers
#include "vSolver/threads/threadJob.h"
#include "vSolver/threads/threadData.h"

//==========================================================================
// Class:			THREAD_JOB
// Function:		THREAD_JOB
//
// Description:		Constructor for the THREAD_JOB class (default).
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
THREAD_JOB::THREAD_JOB() : Command(THREAD_JOB::COMMAND_THREAD_NULL), Data(NULL)
{
}

//==========================================================================
// Class:			THREAD_JOB
// Function:		THREAD_JOB
//
// Description:		Constructor for the THREAD_JOB class (basic).
//
// Input Arguments:
//		_Command		= THREAD_COMMANDS specifying the command type for this job
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
THREAD_JOB::THREAD_JOB(THREAD_COMMANDS _Command) : Command(_Command), Data(NULL)
{
	// Only permit certian types of jobs
	assert(Command == THREAD_JOB::COMMAND_THREAD_EXIT ||
		Command == THREAD_JOB::COMMAND_THREAD_STARTED ||
		Command == THREAD_JOB::COMMAND_THREAD_NULL);
}

//==========================================================================
// Class:			THREAD_JOB
// Function:		THREAD_JOB
//
// Description:		Constructor for the THREAD_JOB class.
//
// Input Arguments:
//		_Command	= THREAD_COMMANDS specifying the command type for this job
//		_Name		= const wxString& Name of the car
//		_Index		= int& representing the object index for the associated object
//					  in the MAIN_FRAME
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
THREAD_JOB::THREAD_JOB(THREAD_COMMANDS _Command, THREAD_DATA *_Data,
					   const wxString &_Name, int &_Index) : Command(_Command),
					   Data(_Data), Name(_Name), Index(_Index)
{
	// Make sure the correct data was passed to this job
	assert(Data);
	assert(Data->OkForCommand(Command));
}

//==========================================================================
// Class:			THREAD_JOB
// Function:		~THREAD_JOB
//
// Description:		Destructor for the THREAD_JOB class.
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
THREAD_JOB::~THREAD_JOB()
{
}