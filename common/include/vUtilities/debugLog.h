/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  debugLog.h
// Created:  12/5/2011
// Author:  K. Loux
// Description:  Debug log class intended to aid with multi-threaded debugging.
//				 This class is designed to be optimized out for release builds.

#ifndef _DEBUG_LOG_H_
#define _DEBUG_LOG_H_

// Standard C++ headers
#include <vector>

// wxWidgeths headers
#include "wx/wx.h"

// Disable compiler warnings for unreferenced formal parameters in MSW
#ifdef __WXMSW__
#pragma warning(disable:4100)
#endif

// Comment out this flag for release builds
#define USE_DEBUG_LOG

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
	static DebugLog* GetInstance(void);
	static void Kill(void);
	void SetTarget(const LogTarget &_target);
	void Log(wxString message, int _indent = 0);
	
private:
	// Current indentation level for each thread
	// pair is <Thread ID, Indent Level>
	std::vector<std::pair<unsigned long, unsigned int> > indent;
	
	// Flag indicating how to log
	static LogTarget target;
	
	// Synchronization object
	wxMutex mutex;

	// The "real" DebugLog
	static DebugLog* logInstance;

	// Log file name
	const static wxString logFileName;

// The optimize-me-out implementation
#else
	// Include inline, non-functioning versions of all above public methods here
	static inline DebugLog* GetInstance(void) { return NULL; };
	static inline void Kill(void) { };
	static inline void SetTarget(const LogTarget &target) { };
	static inline void Log(wxString message, int _indent = 0) { };
#endif

private:
	// These are private for singletons
	DebugLog(void) { Log(_T("\n")); };// Always start with a newline
	DebugLog(const DebugLog &log) { };
	DebugLog& operator= (const DebugLog &log) { return *this; };
};

#endif// _DEBUG_LOG_H_