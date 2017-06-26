/*===================================================================================
                                       VVASE
                         Copyright Kerry R. Loux 2007-2017
===================================================================================*/

// File:  debugger.h
// Date:  2/24/2008
// Auth:  K. Loux
// Desc:  Contains class declaration for Debugger.  This is a simple class that
//        prints information to a wxTextCtrl.  This cleans up the rest of the program.

#ifndef DEBUGGER_H_
#define DEBUGGER_H_

// Standard C++ headers
#include <iostream>
#include <sstream>
#include <map>

// wxWidgets headers
#include <wx/event.h>
#include <wx/thread.h>

// wxWidgets forward declarations
class wxString;
class wxTextCtrl;

// Declaration of the EVT_DEBUG event
DECLARE_LOCAL_EVENT_TYPE(EVT_DEBUG, -1)

namespace VVASE
{

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
	inline DebugLevel GetDebugLevel() const { wxMutexLocker lock(mutex); return debugLevel; }
	void SetTargetOutput(wxEvtHandler *parent);

private:
	// For singletons, the constructors and assignment operators are private
	Debugger();
	virtual ~Debugger() {}

	static Debugger *debuggerInstance;

	DebugLevel debugLevel;
	wxEvtHandler *parent;
	mutable wxMutex mutex;

	// This stream buffer is designed to allow streaming to occur from multiple threads
	// simultaneously without jumbling the text
	class DebuggerStreamBuffer : public std::stringbuf
	{
	public:
		DebuggerStreamBuffer(Debugger &log) : log(log) {}
		virtual ~DebuggerStreamBuffer();

		typedef std::map<wxThreadIdType, std::stringstream*> BufferMap;
		BufferMap threadBuffer;
		mutable wxMutex mutex;
		void CreateThreadBuffer(void);

	protected:
		virtual int overflow(int c);
		virtual int sync();

	private:
		Debugger &log;
	} buffer;
};

}// namespace VVASE

#endif// DEBUGGER_H_
