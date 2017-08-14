/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  millikenAnalysis.h
// Date:  3/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for MILLIKEN_ANALYSIS (Milliken Moment
//        Method analysis) class.  See RCVD for more information.

#ifndef MILLIKEN_ANALYSIS_H_
#define MILLIKEN_ANALYSIS_H_

// Local headers
#include "VVASE/core/analysis/analysis.h"

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

	// Required by Analysis
	wxPanel* GetEditPanel() override;
	wxPanel* GetNotebookPage() override;
	wxTreeNode* GetTreeNode() override;
	OptimizationInterface* GetOptimizationInterface() override;
	bool Compute(const Car& car) override;

private:
	// Debugger message printing utility
	const Debugger &debugger;
};

}// namespace VVASE

#endif// MILLIKEN_ANALYSIS_H_
