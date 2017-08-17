/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  stopWatch.h
// Date:  8/17/2017
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Helper class for getting elapsed times.

#ifndef STOP_WATCH_H_
#define STOP_WATCH_H_

// Standard C++ headers
#include <chrono>
#include <type_traits>

class StopWatch
{
public:
	void Start();
	template <typename T, typename Units>
	T GetElapsedTime() const;

private:
	std::chrono::steady_clock::time_point start;
};

template <typename T, typename Units>
T StopWatch::GetElapsedTime() const
{
	return std::chrono::duration<T>(std::chrono::duration_cast<Units>(std::chrono::steady_clock::now() - start)).count();
}

#endif// STOP_WATCH_H_
