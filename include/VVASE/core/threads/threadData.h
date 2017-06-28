/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  threadData.h
// Date:  1/12/2009
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the ThreadData class.  This is an
//        abstract class from which objects should be derived to pass data to
//        different types of threads.  Using this class allows for a cleaner
//        thread management system, where the queue only needs to know about
//        this class, not any specific data for threads that perform specific
//        tasks.

#ifndef THREAD_DATA_H_
#define THREAD_DATA_H_

// Local headers
#include "vSolver/threads/threadJob.h"

namespace VVASE
{

class ThreadData
{
public:
	ThreadData();
	virtual ~ThreadData();

	// For checking to make sure the proper data was used with
	// each command type
	virtual bool OkForCommand(ThreadJob::ThreadCommand &command) = 0;
};

}// namespace VVASE

#endif// THREAD_DATA_H_
