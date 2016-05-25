/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  optimizationData.h
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the OptimizationData class.  This contains
//				 information required to conduct genetic optimizations.
// History:

#ifndef OPTIMIZATION_DATA_H_
#define OPTIMIZATION_DATA_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vSolver/threads/threadData.h"

// VVASE forward declarations
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

#endif// OPTIMIZATION_DATA_H_