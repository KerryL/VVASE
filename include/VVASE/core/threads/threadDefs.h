/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  threadDefs.h
// Date:  8/17/2017
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Helpful definitions for working with threads.

#ifndef THREAD_DEFS_H_
#define THREAD_DEFS_H_

// Standard C++ headers
#include <mutex>

namespace VVASE
{
typedef std::lock_guard<std::mutex> MutexLocker;
typedef std::unique_lock<std::mutex> UniqueMutexLocker;
}// namespace VVASE

#endif// THREAD_DEFS_H_
