/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  millikenAnalysis.h
// Created:  3/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for MILLIKEN_ANALYSIS (Milliken Moment
//				 Method analysis) class.  See RCVD for more information.
// History:

#ifndef _MILLIKEN_ANALYSIS_H_
#define _MILLIKEN_ANALYSIS_H_

// CarDesigner forward declarations
class Debugger;

class MillikenAnalysis
{
public:
	// Constructor
	MillikenAnalysis(const Debugger &_debugger);
	MillikenAnalysis(const MillikenAnalysis &millikenAnalysis);

	// Destructor
	~MillikenAnalysis();

	// Overloaded operators
	MillikenAnalysis& operator = (const MillikenAnalysis &millikenAnalysis);

private:
	// Debugger message printing utility
	const Debugger &debugger;
};

#endif// _MILLIKEN_ANALYSIS_H_