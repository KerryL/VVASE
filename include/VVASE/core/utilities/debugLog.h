/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  debugLog.h
// Date:  12/5/2011
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Debug log class intended to aid with multi-threaded debugging.
//        This class is designed to be optimized out for release builds.

#ifndef DEBUG_LOG_H_
#define DEBUG_LOG_H_

// Local headers
#include "VVASE/core/utilities/vvaseString.h"

// Standard C++ headers
#include <vector>
#include <mutex>

// Disable compiler warnings for unreferenced formal parameters in MSW
#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

// Remove this flag for release builds
#ifdef _DEBUG
// Or comment it out to disable for debug builds
//#define USE_DEBUG_LOG
#endif

namespace VVASE
{

class DebugLog
{
public:
	enum LogTarget
	{
		TargetStdErr,
		TargetFile
	};
// The useful implementation
#ifdef USE_DEBUG_LOG
	static DebugLog* GetInstance();
	static void Kill();
	void SetTarget(const LogTarget &target);
	void Log(vvaseString message, int indent = 0);

private:
	// Current indentation level for each thread
	// pair is <Thread ID, Indent Level>
	std::vector<std::pair<unsigned long, unsigned int> > indent;

	// Flag indicating how to log
	static LogTarget target;

	// Synchronization object
	std::mutex mutex;

	// The "real" DebugLog
	static DebugLog* logInstance;

	// Log file name
	const static vvaseString logFileName;

// The not-implementation
#else
	// Include inline, non-functioning versions of all above public methods here
	static inline DebugLog* GetInstance() { return NULL; };
	static inline void Kill() { };
	static inline void SetTarget(const LogTarget &/*target*/) { };
	static inline void Log(vvaseString /*message*/) { };
	static inline void Log(vvaseString /*message*/, int /*indent*/) { };
#endif

private:
	// These are private for singletons
	DebugLog() { Log(_T("\n")); };// Always start with a newline
	DebugLog(const DebugLog &/*log*/) { };
	DebugLog& operator= (const DebugLog &/*log*/) { return *this; };
};

}// namespace VVASE

#endif// DEBUG_LOG_H_
