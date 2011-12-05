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

#include "wx/wx.h"

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
	void SetTarget(const LogTarget &_target);
	void Log(wxString message, const int &_indent = 0);// FIXME:  This won't be thread-safe, will it?
	
private:
	// Current indentation level for each thread
	wxArrayInt indent;
	
	// Flag indicating how to log
	LogTarget target;
	
	// Synchronization object
	wxMutex mutex;

// The optimize-me-out implementation
#else
	// Include inline, non-functioning versions of all above public methods here
	void SetTarget(const LogTarget &target) { };
	void Log(wxString message, const int &_indent = 0) { };

#endif
};

#endif// _DEBUG_LOG_H_