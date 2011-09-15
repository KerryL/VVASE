/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  debug_class.h
// Created:  2/24/2008
// Author:  K. Loux
// Description:  Contains class declaration for DEBUGGER.  This is a simple class that
//				 prints information to a wxTextCtrl.  This cleans up the rest of the program.
// History:
//	3/5/2008	- Transformed from function to class and added DEBUG_LEVEL, K. Loux
//	3/8/2008	- Modified to use wxString instead of char *, K. Loux
//	3/9/2008	- Changed the structure of this class to allow one object at a high level
//				  with pointers passed downstream.  Also moved the enumerations inside the
//				  class body, K. Loux.
//	5/2/2009	- Made Print() functions const to allow passing this object as a constant, K. Loux.
//	11/22/2009	- Moved to vUtilities.lib, K. Loux.
//	12/20/2009	- Modified for thread-safe operation, K. Loux.

#ifndef _DEBUG_CLASS_H_
#define _DEBUG_CLASS_H_

// wxWidgets headers
#include <wx/event.h>
#include <wx/thread.h>

// wxWidgets forward declarations
class wxString;
class wxTextCtrl;

// Declaration of the EVT_DEBUG event
DECLARE_LOCAL_EVENT_TYPE(EVT_DEBUG, -1)

class DEBUGGER
{
public:
	// Constructor
	DEBUGGER();

	// Destructor
	~DEBUGGER();

	// This enumeration describes how many debug messages we want to print
	enum DEBUG_LEVEL
	{
		PriorityVeryHigh,	// These messages ALWAYS print (default) - for critical errors
		PriorityHigh,		// This type of message would include warnings that affect solution accuracy
		PriorityMedium,		// This type of message should warn against poor performance
		PriorityLow			// Anything else we might want to print (usually for debugging - function calls, etc.)
	};

	// Prints the message to the output pane, if the DEBUG_LEVEL is high enough
	void Print(const wxString &Info, DEBUG_LEVEL Level = PriorityVeryHigh) const;
	void Print(const DEBUG_LEVEL &Level, const char *format, ...) const;

	// Sets the desired DEBUG_LEVEL
	void SetDebugLevel(const DEBUG_LEVEL &Level);

	// Returns the current debug level
	inline DEBUG_LEVEL GetDebugLevel(void) const { wxMutexLocker Lock(DebugMutex); return DebugLevel; };

	// Sets the event handler instead to which events are posted
	void SetTargetOutput(wxEvtHandler *_Parent);

private:
	// The limit for how important a message must be to be printed
	DEBUG_LEVEL DebugLevel;

	// For thread-safe debugging, we can give this a pointer to an event
	// queue and have it post there
	wxEvtHandler *Parent;

	// Synchronization object
	mutable wxMutex DebugMutex;
};

#endif// _DEBUG_CLASS_H_