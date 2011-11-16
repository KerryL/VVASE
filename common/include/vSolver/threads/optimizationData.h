/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  optimizationData.h
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the OPTIMIZATION_DATA class.  This contains
//				 information required to conduct genetic optimizations.
// History:

#ifndef _OPTIMIZATION_DATA_H_
#define _OPTIMIZATION_DATA_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vSolver/threads/threadData.h"

// VVASE forward declarations
class GeneticAlgorithm;

class OptimizationData : public ThreadData
{
public:
	// Constructor
	OptimizationData(GeneticAlgorithm *_geneticAlgorithm);

	// Destructor
	~OptimizationData();

	// Data required to perform genetic optimizations (for GENETIC_OPTIMIZATION objects)
	GeneticAlgorithm *geneticAlgorithm;

	// Mandatory overload from THREAD_DATA
	bool OkForCommand(ThreadJob::ThreadCommands &command);
};

#endif// _OPTIMIZATION_DATA_H_