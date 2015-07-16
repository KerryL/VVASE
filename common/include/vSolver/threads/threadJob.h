/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  threadJob.h
// Created:  11/3/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the ThreadJob class.  This is the
//				 class containing information that is stored in the job queue, i.e. this
//				 class is used for communication from the GUI thread to the worker threads
//				 (in that direction only - reverse communication is handled with events).
// History:

#ifndef THREAD_JOB_H_
#define THREAD_JOB_H_

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

	ThreadJob();
	ThreadJob(const ThreadJob &threadJob);// For thread safety
	ThreadJob(ThreadCommand command);
	ThreadJob(ThreadCommand command, ThreadData *data,
		const wxString &name, int &index);
	~ThreadJob();

	ThreadCommand command;

	ThreadData *data;
	wxString name;
	int index;
	
	// Operators
	ThreadJob& operator=(const ThreadJob &job);
};

#endif// THREAD_JOB_H_