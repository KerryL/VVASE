/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  optimizationData.h
// Date:  1/12/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the OptimizationData class.  This contains
//        information required to conduct genetic optimizations.

#ifndef OPTIMIZATION_DATA_H_
#define OPTIMIZATION_DATA_H_

// Local headers
#include "VVASE/core/threads/threadData.h"

namespace VVASE
{

// Local forward declarations
class GeneticAlgorithm;

class OptimizationData : public ThreadData
{
public:
	OptimizationData(GeneticAlgorithm *geneticAlgorithm);
	~OptimizationData();

	// Data required to perform genetic optimizations (for GeneticOptimization objects)
	GeneticAlgorithm *geneticAlgorithm;

	// Mandatory overload from ThreadData
	bool OkForCommand(ThreadJob::ThreadCommand &command);
};

}// namespace VVASE

#endif// OPTIMIZATION_DATA_H_
