/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  stopWatch.cpp
// Date:  8/17/2017
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Helper class for getting elapsed times.

// Local headers
#include "VVASE/core/utilities/stopWatch.h"

void StopWatch::Start()
{
	start = std::chrono::steady_clock::now();
}
