/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  thread_job_class.h
// Created:  11/3/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the THREAD_JOB class.  This is the
//				 class containing information that is stored in the job queue, i.e. this
//				 class is used for communication from the GUI thread to the worker threads
//				 (in that direction only - reverse communication is handled with events).
// History:

#ifndef _THREAD_JOB_CLASS_H_
#define _THREAD_JOB_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE forward declarations
class THREAD_DATA;

class THREAD_JOB
{
public:
	// Commands to send to worker thread
	enum THREAD_COMMANDS
	{
		COMMAND_THREAD_EXIT = -1,
		COMMAND_THREAD_NULL =  0,
		COMMAND_THREAD_STARTED,
		COMMAND_THREAD_KINEMATICS_NORMAL,
		COMMAND_THREAD_KINEMATICS_ITERATION,
		COMMAND_THREAD_KINEMATICS_FOR_GA,
		COMMAND_THREAD_GENETIC_OPTIMIZATION
	};

	// Constructors
	THREAD_JOB();
	THREAD_JOB(THREAD_COMMANDS _Command);
	THREAD_JOB(THREAD_COMMANDS _Command, THREAD_DATA *_Data,
		const wxString &_Name, int &_Index);

	// Destructor
	~THREAD_JOB();

	// The command to be sent
	THREAD_COMMANDS Command;

	// Data to be sent to worker threads
	THREAD_DATA *Data;
	wxString Name;
	int Index;
};

#endif// _THREAD_JOB_CLASS_H_