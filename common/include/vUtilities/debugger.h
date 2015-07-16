/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  debugger.h
// Created:  2/24/2008
// Author:  K. Loux
// Description:  Contains class declaration for Debugger.  This is a simple class that
//				 prints information to a wxTextCtrl.  This cleans up the rest of the program.
// History:
//	3/5/2008	- Transformed from function to class and added DebugLevel, K. Loux
//	3/8/2008	- Modified to use wxString instead of char *, K. Loux
//	3/9/2008	- Changed the structure of this class to allow one object at a high level
//				  with pointers passed downstream.  Also moved the enumerations inside the
//				  class body, K. Loux.
//	5/2/2009	- Made Print() functions const to allow passing this object as a constant, K. Loux.
//	11/22/2009	- Moved to vUtilities.lib, K. Loux.
//	12/20/2009	- Modified for thread-safe operation, K. Loux.
//	11/7/2011	- Corrected camelCase, K. Loux.
//	12/5/2011	- Made this object a singleton, K. Loux.

#ifndef DEBUG_H_
#define DEBUG_H_

// Standard C++ headers
#include <iostream>
#include <sstream>

// wxWidgets headers
#include <wx/event.h>
#include <wx/thread.h>

// wxWidgets forward declarations
class wxString;
class wxTextCtrl;

// Declaration of the EVT_DEBUG event
DECLARE_LOCAL_EVENT_TYPE(EVT_DEBUG, -1)

class Debugger : public std::ostream
{
public:
	// In lieu of a constructor/destructor
	static Debugger& GetInstance();
	static void Kill();
	
	// This enumeration describes how many debug messages we want to print
	enum DebugLevel
	{
		PriorityVeryHigh,	// These messages ALWAYS print (default) - for critical errors
		PriorityHigh,		// This type of message would include warnings that affect solution accuracy
		PriorityMedium,		// This type of message should warn against poor performance
		PriorityLow			// Anything else we might want to print (usually for debugging - function calls, etc.)
	};

	friend std::ostream& operator<<(std::ostream &os, const Debugger::DebugLevel& level);

	void SetDebugLevel(const DebugLevel &level);
	inline DebugLevel GetDebugLevel() const { wxMutexLocker lock(debugMutex); return debugLevel; }
	void SetTargetOutput(wxEvtHandler *parent);

private:
	// For singletons, the constructors and assignment operators are private
	Debugger();
	virtual ~Debugger();
	
	static Debugger *debuggerInstance;
	
	DebugLevel debugLevel;
	wxEvtHandler *parent;
	mutable wxMutex debugMutex;

	class DebuggerStreamBuffer : public std::stringbuf
	{
	public:
		DebuggerStreamBuffer(Debugger &log) : log(log) {}

	protected:
		virtual int sync();

	private:
		Debugger &log;
	} buffer;
};

#endif// DEBUG_H_
