/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  optimization_data_class.h
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the OPTIMIZATION_DATA class.  This contains
//				 information required to conduct genetic optimizations.
// History:

#ifndef _OPTIMIZATION_DATA_CLASS_H_
#define _OPTIMIZATION_DATA_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vSolver/threads/thread_data_class.h"

// VVASE forward declarations
class GENETIC_ALGORITHM;

class OPTIMIZATION_DATA : public THREAD_DATA
{
public:
	// Constructor
	OPTIMIZATION_DATA(GENETIC_ALGORITHM *_GeneticAlgorithm);

	// Destructor
	~OPTIMIZATION_DATA();

	// Data required to perform genetic optimizations (for GENETIC_OPTIMIZATION objects)
	GENETIC_ALGORITHM *GeneticAlgorithm;

	// Mandatory overload from THREAD_DATA
	bool OkForCommand(THREAD_JOB::THREAD_COMMANDS &Command);
};

#endif// _OPTIMIZATION_DATA_CLASS_H_