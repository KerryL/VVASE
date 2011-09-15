/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  debug_class.cpp
// Created:  2/24/2008
// Author:  K. Loux
// Description:  A simple class that prints information to a log file.  Just
//				 to clean up the rest of the program.
// History:
//	3/5/2008	- Transformed from function to class and added DEBUG_LEVEL, K. Loux.
//	3/8/2008	- Modified to use the wxWidgets class wxString instead of char *, K. Loux.
//	3/9/2008	- Changed the structure of the DEBUGGER class to handle the entire application
//				  with one object, and to print the output to a wxTextCtl object, K. Loux.
//	5/2/2009	- Made Print() functions const to allow passing this object as a constant, K. Loux.
//	11/22/2009	- Moved to vUtilities.lib, K. Loux.
//	12/20/2009	- Modified for thread-safe operation, K. Loux.

// Standard C headers
#include <stdarg.h>
#include <iostream>

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vUtilities/debug_class.h"
#include "vSolver/threads/thread_event_class.h"

// Define the EVT_DEBUG event type
DEFINE_LOCAL_EVENT_TYPE(EVT_DEBUG)

//==========================================================================
// Class:			DEBUGGER
// Function:		DEBUGGER
//
// Description:		Constructor for the DEBUGGER class.  Initializes the
//					debug level to HIGH_PRIOIRTY.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DEBUGGER::DEBUGGER()
{
	// Ensure exclusive access to this object
	wxMutexLocker Lock(DebugMutex);

	// Set a default debug level
	DebugLevel = PriorityHigh;

	// Initialize the output target to NULL
	Parent = NULL;
}

//==========================================================================
// Class:			DEBUGGER
// Function:		~DEBUGGER
//
// Description:		Destructor for the DEBUGGER class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DEBUGGER::~DEBUGGER()
{
	// Ensure exclusive access to this object
	wxMutexLocker Lock(DebugMutex);
}

//==========================================================================
// Class:			DEBUGGER
// Function:		SetDebugLevel
//
// Description:		Sets the debug level to the specified level
//
// Input Argurments:
//		Level	= const DEBUG_LEVEL&, the level to which this will be set
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DEBUGGER::SetDebugLevel(const DEBUG_LEVEL &Level)
{
	// Ensure exlusive access to this object
	wxMutexLocker Lock(DebugMutex);

	// Set the class member as specified
	DebugLevel = Level;

	return;
}

//==========================================================================
// Class:			DEBUGGER
// Function:		SetTargetOutput
//
// Description:		Assigns the event handler for event posting.
//
// Input Argurments:
//		_Parent	= *wxEvtHandler to which the events are sent.
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DEBUGGER::SetTargetOutput(wxEvtHandler *_Parent)
{
	// Ensure exclusive access to this object
	wxMutexLocker Lock(DebugMutex);

	// Set the event handler
	Parent = _Parent;

	return;
}

//==========================================================================
// Class:			DEBUGGER
// Function:		Print
//
// Description:		This function writes the specified text to the wxTextCtrl
//					associated with this object, but ONLY if the debug level
//					of the message is greater than the debug level of this
//					object.  An end-of-line character is appended at the end
//					of every Print call.
//
// Input Argurments:
//		Info	= const wxString& to be printed
//		Level	= DEBUG_LEVEL of this message
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DEBUGGER::Print(const wxString &Info, DEBUG_LEVEL Level) const
{
	// Ensure exclusive access to this object
	wxMutexLocker Lock(DebugMutex);

	// Lower debug level -> higher priority
	// Show messages having a debug level higher than or equal to the set debug level
	if (int(Level) <= int(DebugLevel))
	{
		// If the event handler is assigned, then use it
		if (Parent != NULL)
		{
			// Create a debug event
			wxCommandEvent evt(EVT_DEBUG, 0);

			// The priority of the message
			evt.SetInt(Level);

			// Format the message
			wxString Message(Info);
			if (Level == PriorityVeryHigh)
				Message.Prepend(_T("      "));
			Message.append(_T("\n"));

			// The text of the message
			evt.SetString(Message);

			// Add it to the parent's event queue
			Parent->AddPendingEvent(evt);
		}
		else// Send the output to the terminal
			// Write the message
			std::cout << Info << std::endl;
	}

	return;
}

//==========================================================================
// Class:			DEBUGGER
// Function:		Print
//
// Description:		Variable argument version of Print().  This formats the
//					input string and calls the other overload of Print().
//
// Input Argurments:
//		Level	= const DEBUG_LEVEL& of this message
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
void DEBUGGER::Print(const DEBUG_LEVEL &Level, const char *format, ...) const
{
	// Get the variable arguments from the function call
	char Output[512];
	va_list args;
	va_start(args, format);

	// Format the arguments into a string - use the secure version of vsnprintf if
	// it is available
#ifdef __WIN32__
	if (vsnprintf_s((char*)Output, sizeof(Output), sizeof(Output), format, args) == -1)
#else
	if (vsnprintf((char*)Output, sizeof(Output), format, args) == 1)
#endif
	{
		// Warn about message truncation
		Print(_T("Warning:  Debugger message truncated!"), Level);
	}

	// End the variable argument macro
	va_end(args);

	// Print the formatted message at the specified level
	Print(_T(Output), Level);

	return;
}