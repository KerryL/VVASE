/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  analysis.h
// Date:  6/20/2017
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Abstract base class for Analysis objects.

#ifndef ANALYSIS_H_
#define ANALYSIS_H_

#include "VVASE/core/utilities/componentManager.h"

namespace VVASE
{

// Local forward declarations
class Car;
class OptimizationInterface;

// wxWidgets forward declarations
class wxPanel;
class wxTreeNode;

class Analysis : public RegisterableComponent
{
public:
	virtual wxPanel* GetEditPanel() = 0;
	virtual wxPanel* GetNotebookPage() = 0;
	virtual wxTreeNode* GetTreeNode() = 0;
	virtual OptimizationInterface* GetOptimizationInterface() = 0;

	virtual bool Compute(const Car& car) = 0;

};

}// namespace VVASE

#endif// ANALYSIS_H_
