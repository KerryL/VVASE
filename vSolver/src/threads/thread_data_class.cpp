/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  thread_data_class.cpp
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class definition for the THREAD_DATA class.  This is an
//				 abstract class from which objects should be derived to pass data to
//				 different types of threads.  Using this class allows for a cleaner
//				 thread management system, where the queue only needs to know about
//				 this class, not any specific data for threads that perform specific
//				 tasks.
// History:

// VVASE headers
#include "vSolver/threads/thread_data_class.h"

//==========================================================================
// Class:			THREAD_DATA
// Function:		THREAD_DATA
//
// Description:		Constructor for the THREAD_DATA class (basic).
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
THREAD_DATA::THREAD_DATA()
{
}

//==========================================================================
// Class:			THREAD_DATA
// Function:		~THREAD_DATA
//
// Description:		Destructor for the THREAD_DATA class.
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
THREAD_DATA::~THREAD_DATA()
{
}