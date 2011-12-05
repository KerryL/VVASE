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
//	3/5/2008	- Transformed from function to class and added DEBUG_LEVEL, K. Loux.
//	3/8/2008	- Modified to use the wxWidgets class wxString instead of char *, K. Loux.
//	3/9/2008	- Changed the structure of the Debugger class to handle the entire application
//				  with one object, and to print the output to a wxTextCtl object, K. Loux.
//	5/2/2009	- Made Print() functions const to allow passing this object as a constant, K. Loux.
//	11/22/2009	- Moved to vUtilities.lib, K. Loux.
//	12/20/2009	- Modified for thread-safe operation, K. Loux.
//	11/7/2011	- Corrected camelCase, K. Loux.

// Standard C headers
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
Debugger::Debugger()
{
	// Ensure exclusive access to this object
	wxMutexLocker lock(debugMutex);

	// Set a default debug level
	debugLevel = PriorityHigh;

	// Initialize the output target to NULL
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
	// Ensure exclusive access to this object
	wxMutexLocker lock(debugMutex);
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
	// Ensure exlusive access to this object
	wxMutexLocker lock(debugMutex);

	// Set the class member as specified
	debugLevel = level;
}

//==========================================================================
// Class:			Debugger
// Function:		SetTargetOutput
//
// Description:		Assigns the event handler for event posting.
//
// Input Arguments:
//		_parent	= *wxEvtHandler to which the events are sent.
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Debugger::SetTargetOutput(wxEvtHandler *_parent)
{
	// Ensure exclusive access to this object
	wxMutexLocker lock(debugMutex);

	// Set the event handler
	parent = _parent;
}

//==========================================================================
// Class:			Debugger
// Function:		Print
//
// Description:		This function writes the specified text to the wxTextCtrl
//					associated with this object, but ONLY if the debug level
//					of the message is greater than the debug level of this
//					object.  An end-of-line character is appended at the end
//					of every Print call.
//
// Input Arguments:
//		info	= const wxString& to be printed
//		level	= DebugLevel of this message
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Debugger::Print(const wxString &info, DebugLevel level) const
{
	// Ensure exclusive access to this object
	wxMutexLocker lock(debugMutex);
	DebugLog::GetInstance()->Log(_T("Debugger::Print (locker)"));

	// Lower debug level -> higher priority
	// Show messages having a debug level higher than or equal to the set debug level
	if (int(level) <= int(debugLevel))
	{
		// If the event handler is assigned, then use it
		if (parent != NULL)
		{
			// Create a debug event
			wxCommandEvent evt(EVT_DEBUG, 0);

			// The priority of the message
			evt.SetInt(level);

			// Format the message
			wxString message(info);
			if (level == PriorityVeryHigh)
				message.Prepend(_T("      "));
			message.append(_T("\n"));

			// The text of the message
			evt.SetString(message);

			// Add it to the parent's event queue
			parent->AddPendingEvent(evt);
		}
		else// Send the output to the terminal
			// Write the message
			std::cout << info << std::endl;
	}
}

//==========================================================================
// Class:			Debugger
// Function:		Print
//
// Description:		Variable argument version of Print().  This formats the
//					input string and calls the other overload of Print().
//
// Input Arguments:
//		level	= const DebugLevel& of this message
//		format	= const char* to be formatted
//		...		= additional arguments depending on the format
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Debugger::Print(const DebugLevel &level, const char *format, ...) const
{
	// Get the variable arguments from the function call
	char output[512];
	va_list args;
	va_start(args, format);

	// Format the arguments into a string - use the secure version of vsnprintf if
	// it is available
#ifdef __WIN32__
	if (vsnprintf_s((char*)output, sizeof(output), sizeof(output), format, args) == -1)
#else
	if (vsnprintf((char*)output, sizeof(output), format, args) == 1)
#endif
	{
		// Warn about message truncation
		Print(_T("Warning:  Debugger message truncated!"), level);
	}

	// End the variable argument macro
	va_end(args);

	// Print the formatted message at the specified level
	Print(_T(output), level);
}