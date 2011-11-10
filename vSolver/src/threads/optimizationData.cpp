/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  optimizationData.cpp
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the OPTIMIZATION_DATA class.  This contains
//				 information required to conduct genetic optimizations.
// History:

// VVASE headers
#include "vSolver/threads/optimizationData.h"
#include "vSolver/threads/threadJob.h"

//==========================================================================
// Class:			OPTIMIZATION_DATA
// Function:		OPTIMIZATION_DATA
//
// Description:		Constructor for the OPTIMIZATION_DATA class.
//
// Input Arguments:
//		_GeneticAlgorithm	= GENETIC_ALGORITHM* pointing to the optimization object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
OPTIMIZATION_DATA::OPTIMIZATION_DATA(GENETIC_ALGORITHM *_GeneticAlgorithm)
									 : THREAD_DATA(), GeneticAlgorithm(_GeneticAlgorithm)
{
}

//==========================================================================
// Class:			OPTIMIZATION_DATA
// Function:		~OPTIMIZATION_DATA
//
// Description:		Destructor for the OPTIMIZATION_DATA class.
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
OPTIMIZATION_DATA::~OPTIMIZATION_DATA()
{
}

//==========================================================================
// Class:			OPTIMIZATION_DATA
// Function:		OkForCommand
//
// Description:		Checks to make sure this type of data is correct for the
//					specified command.
//
// Input Arguments:
//		Command		= THREAD_JOB::THREAD_COMMANDS& to be checked
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
bool OPTIMIZATION_DATA::OkForCommand(THREAD_JOB::THREAD_COMMANDS &Command)
{
	// Make sure the command is one of the expected types
	return Command == THREAD_JOB::COMMAND_THREAD_GENETIC_OPTIMIZATION;
}