/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  debugger.cpp
// Date:  2/24/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  A simple class that prints information to a log file.  Just
//        to clean up the rest of the program.

// Standard C++ headers
#include <stdarg.h>
#include <iostream>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "vUtilities/debugger.h"
#include "vSolver/threads/threadEvent.h"
#include "vUtilities/debugLog.h"

// Uncomment to enable writing all debug messages to stdout
//#define DEBUG_TO_STDOUT

// Define the EVT_DEBUG event type
DEFINE_LOCAL_EVENT_TYPE(EVT_DEBUG)

namespace VVASE
{

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
	wxMutexLocker lock(mutex);

	debugLevel = PriorityHigh;
	parent = NULL;
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
	wxMutexLocker lock(mutex);
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
	wxMutexLocker lock(mutex);
	this->parent = parent;
}

//==========================================================================
// Class:			Debugger::DebuggerStreamBuffer
// Function:		~DebuggerStreamBuffer
//
// Description:		Destructor for DebuggerStreamBuffer class.
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
Debugger::DebuggerStreamBuffer::~DebuggerStreamBuffer()
{
	BufferMap::iterator it;
	for (it = threadBuffer.begin(); it != threadBuffer.end(); it++)
		delete it->second;
}

//==========================================================================
// Class:			Debugger::DebuggerStreamBuffer
// Function:		overflow
//
// Description:		Override of the standard overflow method.  Called when
//					new data is inserted into the stream.  This is overridden
//					so we can control which buffer the data is inserted into.
//					The buffers are controlled on a per-thread basis.
//
// Input Arguments:
//		c	= int
//
// Output Arguments:
//		None
//
// Return Value:
//		int
//
//==========================================================================
int Debugger::DebuggerStreamBuffer::overflow(int c)
{
	CreateThreadBuffer();
	if (c != traits_type::eof())
		*threadBuffer[wxThread::GetCurrentId()] << (char)c;

	return c;
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
// Class:			Debugger::DebuggerStreamBuffer
// Function:		CreateThreadBuffer
//
// Description:		Checks for existance of a buffer for the current thread,
//					and creates the buffer if it doesn't exist.
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
void Debugger::DebuggerStreamBuffer::CreateThreadBuffer(void)
{
	if (threadBuffer.find(wxThread::GetCurrentId()) == threadBuffer.end())
	{
		wxMutexLocker lock(mutex);
		if (threadBuffer.find(wxThread::GetCurrentId()) == threadBuffer.end())
			threadBuffer[wxThread::GetCurrentId()] = new std::stringstream;
	}
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

	debugger.buffer.CreateThreadBuffer();
	wxMutexLocker lock(debugger.buffer.mutex);

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
			wxString message(debugger.buffer.threadBuffer[wxThread::GetCurrentId()]->str());
			if (level == Debugger::PriorityVeryHigh)
				message.Prepend(_T("      "));
			message.append(_T("\n"));

			evt.SetString(message);
			debugger.parent->AddPendingEvent(evt);
		}
#ifndef DEBUG_TO_STDOUT
		else// Send the output to the terminal
			std::cout << debugger.buffer.threadBuffer[wxThread::GetCurrentId()]->str() << std::endl;
		// TODO:  Modify this to be like CombinedLogger and output to multiple sources?  other std::ostreams?
		// Then could make another ostream for writing to wxTextCtrl
#endif
	}

#ifdef DEBUG_TO_STDOUT
	std::cout << debugger.buffer.threadBuffer[wxThread::GetCurrentId()]->str() << std::endl;
#endif

	debugger.buffer.threadBuffer[wxThread::GetCurrentId()]->str("");
	return os;
}

}// namespace VVASE
