/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  optimizationData.cpp
// Date:  1/12/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the OptimizationData class.  This contains
//        information required to conduct genetic optimizations.

// Local headers
#include "VVASE/core/threads/optimizationData.h"
#include "VVASE/core/threads/threadJob.h"

namespace VVASE
{

//==========================================================================
// Class:			OptimizationData
// Function:		OptimizationData
//
// Description:		Constructor for the OptimizationData class.
//
// Input Arguments:
//		geneticAlgorithm	= GeneticAlgorithm* pointing to the optimization object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
OptimizationData::OptimizationData(GeneticAlgorithm *geneticAlgorithm)
	: ThreadData(), geneticAlgorithm(geneticAlgorithm)
{
}

//==========================================================================
// Class:			OptimizationData
// Function:		~OptimizationData
//
// Description:		Destructor for the OptimizationData class.
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
OptimizationData::~OptimizationData()
{
}

//==========================================================================
// Class:			OptimizationData
// Function:		OkForCommand
//
// Description:		Checks to make sure this type of data is correct for the
//					specified command.
//
// Input Arguments:
//		command		= ThreadJob::ThreadCommand& to be checked
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
bool OptimizationData::OkForCommand(ThreadJob::ThreadCommand &command)
{
	// Make sure the command is one of the expected types
	return command == ThreadJob::CommandThreadGeneticOptimization;
}

}// namespace VVASE
