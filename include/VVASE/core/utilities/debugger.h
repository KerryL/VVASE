/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  debugger.h
// Date:  2/24/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for Debugger.  This is a simple class that
//        prints information to a wxTextCtrl.  This cleans up the rest of the program.

#ifndef DEBUGGER_H_
#define DEBUGGER_H_

// Local headers
#include "VVASE/core/utilities/vvaseString.h"

// Standard C++ headers
#include <iostream>
#include <sstream>
#include <map>
#include <mutex>
#include <thread>

// Declaration of the EVT_DEBUG event
//DECLARE_LOCAL_EVENT_TYPE(EVT_DEBUG, -1)// TODO:  Remove

namespace VVASE
{

class Debugger : public vvaseOStream
{
public:
	// In lieu of a constructor/destructor
	static Debugger& GetInstance();
	static void Kill();

	// This enumeration describes how many debug messages we want to print
	enum class Priority
	{
		VeryHigh,	// These messages ALWAYS print (default) - for critical errors
		High,		// This type of message would include warnings that affect solution accuracy
		Medium,		// This type of message should warn against poor performance
		Low			// Anything else we might want to print (usually for debugging - function calls, etc.)
	};

	friend vvaseOStream& operator<<(vvaseOStream &os, const Priority& level);

	void SetDebugLevel(const Priority &level);
	inline Priority GetDebugLevel() const { std::lock_guard<std::mutex> lock(mutex); return debugLevel; }
	//void SetTargetOutput(wxEvtHandler *parent);// TODO:  Remove

private:
	// For singletons, the constructors and assignment operators are private
	Debugger();
	virtual ~Debugger() = default;

	static Debugger *debuggerInstance;

	Priority debugLevel = Priority::High;
	//wxEvtHandler *parent;// TODO:  Remove
	mutable std::mutex mutex;

	// This stream buffer is designed to allow streaming to occur from multiple threads
	// simultaneously without jumbling the text
	class DebuggerStreamBuffer : public vvaseStringBuf
	{
	public:
		DebuggerStreamBuffer(Debugger &log) : log(log) {}
		virtual ~DebuggerStreamBuffer();

		typedef std::map<std::thread::id, vvaseOStringStream*> BufferMap;
		BufferMap threadBuffer;
		mutable std::mutex mutex;
		void CreateThreadBuffer();

	protected:
		virtual int overflow(int c);
		virtual int sync();

	private:
		Debugger &log;
	} buffer;
};

}// namespace VVASE

#endif// DEBUGGER_H_
