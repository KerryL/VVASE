/*===================================================================================
                                       VVASE
                         Copyright Kerry R. Loux 2008-2017
===================================================================================*/

// File:  millikenAnalysis.h
// Date:  3/23/2008
// Auth:  K. Loux
// Desc:  Contains class declaration for MILLIKEN_ANALYSIS (Milliken Moment
//        Method analysis) class.  See RCVD for more information.

#ifndef MILLIKEN_ANALYSIS_H_
#define MILLIKEN_ANALYSIS_H_

namespace VVASE
{

// Local forward declarations
class Debugger;

class MillikenAnalysis : public Analysis
{
public:
	MillikenAnalysis(const Debugger &debugger);
	MillikenAnalysis(const MillikenAnalysis &millikenAnalysis);
	~MillikenAnalysis();

	// Overloaded operators
	MillikenAnalysis& operator = (const MillikenAnalysis &millikenAnalysis);

private:
	// Debugger message printing utility
	const Debugger &debugger;
};

}// namespace VVASE

#endif// MILLIKEN_ANALYSIS_H_

