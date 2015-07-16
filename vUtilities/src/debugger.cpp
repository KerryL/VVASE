/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  debugger.cpp
// Created:  2/24/2008
// Author:  K. Loux
// Description:  A simple class that prints information to a log file.  Just
//				 to clean up the rest of the program.
// History:
//	3/5/2008	- Transformed from function to class and added DebugLevel, K. Loux.
//	3/8/2008	- Modified to use the wxWidgets class wxString instead of char *, K. Loux.
//	3/9/2008	- Changed the structure of the Debugger class to handle the entire application
//				  with one object, and to print the output to a wxTextCtl object, K. Loux.
//	5/2/2009	- Made Print() functions const to allow passing this object as a constant, K. Loux.
//	11/22/2009	- Moved to vUtilities.lib, K. Loux.
//	12/20/2009	- Modified for thread-safe operation, K. Loux.
//	11/7/2011	- Corrected camelCase, K. Loux.
//	12/5/2011	- Made this object a singleton, K. Loux.

// Standard C++ headers
#include <stdarg.h>
#include <iostream>

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vUtilities/debugger.h"
#include "vSolver/threads/threadEvent.h"
#include "vUtilities/debugLog.h"

// Define the EVT_DEBUG event type
DEFINE_LOCAL_EVENT_TYPE(EVT_DEBUG)

//==========================================================================
// Class:			Debugger
// Function:		Debugger
//
// Description:		Constructor for the Debugger class.  Initializes the
//					debug level to PriorityHigh.
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
Debugger::Debugger() : std::ostream(&buffer), buffer(*this)
{
	wxMutexLocker lock(debugMutex);

	debugLevel = PriorityHigh;
	parent = NULL;
}

//==========================================================================
// Class:			Debugger
// Function:		~Debugger
//
// Description:		Destructor for the Debugger class.
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
Debugger::~Debugger()
{
	wxMutexLocker lock(debugMutex);
}

//==========================================================================
// Class:			Debugger
// Function:		Initialization of static members
//
// Description:		Required for singleton operation.
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
Debugger* Debugger::debuggerInstance = NULL;

//==========================================================================
// Class:			Debugger
// Function:		GetInstance
//
// Description:		Returns a reference to this.  Creates object if it doesn't
//					already exist.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Debugger&, reference to this object
//
//==========================================================================
Debugger& Debugger::GetInstance()
{
	if (!debuggerInstance)
		debuggerInstance = new Debugger();
	
	return *debuggerInstance;
}

//==========================================================================
// Class:			Debugger
// Function:		Kill
//
// Description:		Deletes the instance of this class, if it exists.
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
void Debugger::Kill()
{
	if (debuggerInstance)
	{
		delete debuggerInstance;
		debuggerInstance = NULL;
	}
}

//==========================================================================
// Class:			Debugger
// Function:		SetDebugLevel
//
// Description:		Sets the debug level to the specified level
//
// Input Arguments:
//		level	= const DebugLevel&, the level to which this will be set
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Debugger::SetDebugLevel(const DebugLevel &level)
{
	wxMutexLocker lock(debugMutex);
	debugLevel = level;
}

//==========================================================================
// Class:			Debugger
// Function:		SetTargetOutput
//
// Description:		Assigns the event handler for event posting.
//
// Input Arguments:
//		parent	= *wxEvtHandler to which the events are sent.
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Debugger::SetTargetOutput(wxEvtHandler *parent)
{
	wxMutexLocker lock(debugMutex);
	this->parent = parent;
}

//==========================================================================
// Class:			Debugger::DebuggerStreamBuffer
// Function:		sync
//
// Description:		Handles endl calls - only purpose is to enforce use of
//					debug level flags instead of std::endl.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		int
//
//==========================================================================
int Debugger::DebuggerStreamBuffer::sync()
{
	assert(false && "Use Debugger::DebugLevel flags instead of std::endl");
	return 0;
}

//==========================================================================
// Class:			Debugger (friend)
// Function:		operator<<
//
// Description:		Special overload for handling DebugLevel flags.  These are
//					interpreted as endl calls or simply flush the buffer,
//					depending on debug level.
//
// Input Arguments:
//		os		= std::ostream &os
//		level	= const Debugger::DebugLevel&
//
// Output Arguments:
//		None
//
// Return Value:
//		std::ostream&
//
//==========================================================================
std::ostream& operator<<(std::ostream &os, const Debugger::DebugLevel& level)
{
	assert(typeid(Debugger&) == typeid(os));
	Debugger& debugger(dynamic_cast<Debugger&>(os));
	wxMutexLocker lock(debugger.debugMutex);

	// Lower debug level -> higher priority
	// Show messages having a debug level higher than or equal to the set debug level
	if (int(level) <= int(debugger.debugLevel))
	{
		// If the event handler is assigned, then use it
		if (debugger.parent != NULL)
		{
			wxCommandEvent evt(EVT_DEBUG, 0);
			evt.SetInt(level);

			// Format the message
			wxString message(debugger.buffer.str());
			if (level == Debugger::PriorityVeryHigh)
				message.Prepend(_T("      "));
			message.append(_T("\n"));

			evt.SetString(message);
			debugger.parent->AddPendingEvent(evt);
		}
		else// Send the output to the terminal
			std::cout << debugger.buffer.str() << std::endl;
		// TODO:  Modify this to be like CombinedLogger and output to multiple sources?  other std::ostreams?
		// Then could make another ostream for writing to wxTextCtrl
	}

	debugger.buffer.str("");
	return os;
}