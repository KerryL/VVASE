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

#ifndef _MILLIKEN_CLASS_H_
#define _MILLIKEN_CLASS_H_

// CarDesigner forward declarations
class Debugger;

class MILLIKEN_ANALYSIS
{
public:
	// Constructor
	MILLIKEN_ANALYSIS(const Debugger &_debugger);
	MILLIKEN_ANALYSIS(const MILLIKEN_ANALYSIS &MillikenAnalysis);

	// Destructor
	~MILLIKEN_ANALYSIS();

	// Overloaded operators
	MILLIKEN_ANALYSIS& operator = (const MILLIKEN_ANALYSIS &MillikenAnalysis);

private:
	// Debugger message printing utility
	const Debugger &debugger;
};

#endif// _MILLIKEN_CLASS_H_