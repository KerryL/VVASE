/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  threadData.cpp
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class definition for the ThreadData class.  This is an
//				 abstract class from which objects should be derived to pass data to
//				 different types of threads.  Using this class allows for a cleaner
//				 thread management system, where the queue only needs to know about
//				 this class, not any specific data for threads that perform specific
//				 tasks.
// History:

// VVASE headers
#include "vSolver/threads/threadData.h"

//==========================================================================
// Class:			ThreadData
// Function:		ThreadData
//
// Description:		Constructor for the ThreadData class (basic).
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
ThreadData::ThreadData()
{
}

//==========================================================================
// Class:			ThreadData
// Function:		~ThreadData
//
// Description:		Destructor for the ThreadData class.
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
ThreadData::~ThreadData()
{
}