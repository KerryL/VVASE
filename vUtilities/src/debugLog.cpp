/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  debugLog.cpp
// Created:  12/5/2011
// Author:  K. Loux
// Description:  Debug log class intended to aid with multi-threaded debugging.
//				 This class is designed to be optimized out for release builds.

// Standard C++ headers
#include <iostream>
#include <fstream>

// Local headers
#include "vUtilities/debugLog.h"

// Only build this class if we need to
#ifdef USE_DEBUG_LOG

//==========================================================================
// Class:			DebugLog
// Function:		Static member initialization
//
// Description:		Returns a pointer to this object, 
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
DebugLog* DebugLog::logInstance = NULL;
DebugLog::LogTarget DebugLog::target = DebugLog::TargetFile;
const wxString DebugLog::logFileName = _T("VVASEdebug.log");

//==========================================================================
// Class:			DebugLog
// Function:		GetInstance
//
// Description:		Returns a pointer to this object, and creates this object
//					if it doesn't already exist.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		DebugLog* pointing to this object
//
//==========================================================================
DebugLog* DebugLog::GetInstance(void)
{
	if (!logInstance)
		logInstance = new DebugLog();

	return logInstance;
}

//==========================================================================
// Class:			DebugLog
// Function:		SetTarget
//
// Description:		Sets the output target for this object.
//
// Input Arguments:
//		_target	= const LogTarget&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DebugLog::SetTarget(const LogTarget &_target)
{
	wxMutexLocker lock(mutex);
	target = _target;
}

//==========================================================================
// Class:			DebugLog
// Function:		Log
//
// Description:		Returns a pointer to this object, and creates this object
//					if it doesn't already exist.
//
// Input Arguments:
//		message	= wxString to be logged
//		_indent	= int specifying wheter the indent level for the current thread
//				  should be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		DebugLog* pointing to this object
//
//==========================================================================
void DebugLog::Log(wxString message, int _indent)
{
	wxMutexLocker lock(mutex);

	// Handle indentation
	unsigned long threadID = wxThread::GetCurrentId();
	unsigned int i, currentIndent(0);
	for (i = 0; i < indent.size(); i++)
	{
		if (indent[i].first == threadID)
		{
			currentIndent = indent[i].second;
			indent[i].second += _indent;

			// Apply minus indents prior to this message; positive indents apply for future messages only
			if (_indent < 0)
				currentIndent = indent[i].second;

			break;
		}
	}

	// If we didn't find this thread's ID, add it to the list
	if (i == indent.size())
		indent.push_back(std::pair<unsigned long, unsigned int>(threadID, _indent));

	// This string is prepended once per indent level
	wxString indentChar(_T("  "));
	for (i = 0; i < currentIndent; i++)
		message.Prepend(indentChar);

	// Also prepend the thread ID
	wxString idString;
	idString.Printf("[%5lu] ", threadID);
	message.Prepend(idString);

	// Handle logging depending on specified target
	if (target == TargetFile)
	{
		std::ofstream file(logFileName, std::ios::out | std::ios::app);
		if (file.is_open() && file.good())
		{
			file << std::endl << message;
			file.close();
		}
	}
	else if (target == TargetStdErr)
		std::cerr << message << std::endl;
	else
		assert(false);
}

//==========================================================================
// Class:			DebugLog
// Function:		Kill
//
// Description:		Deletes this object, if it exists
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
void DebugLog::Kill(void)
{
	delete logInstance;
	logInstance = NULL;
}
#endif