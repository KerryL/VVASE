/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  threadJob.h
// Created:  11/3/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the THREAD_JOB class.  This is the
//				 class containing information that is stored in the job queue, i.e. this
//				 class is used for communication from the GUI thread to the worker threads
//				 (in that direction only - reverse communication is handled with events).
// History:

#ifndef _THREAD_JOB_H_
#define _THREAD_JOB_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
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
		CommandThreadKinematicsIteration,
		CommandThreadKinematicsGA,
		CommandThreadGeneticOptimization
	};

	// Constructors
	ThreadJob();
	ThreadJob(ThreadCommand _command);
	ThreadJob(ThreadCommand _command, ThreadData *_data,
		const wxString &_name, int &_index);

	// Destructor
	~ThreadJob();

	// The command to be sent
	ThreadCommand command;

	// Data to be sent to worker threads
	ThreadData *data;
	wxString name;
	int index;
};

#endif// _THREAD_JOB_H_