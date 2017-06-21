
#include "utilities/componentManager.h"

namespace VVASE
{

class Analysis : public RegisterableComponent
{
public:
	virtual wxPanel* GetEditPanel() = 0;
	virtual wxPanel* GetNotebookPage() = 0;
	virtual wxTreeNote* GetTreeNode() = 0;
	virtual OptimizationInterface* GetOptimizationInterface() = 0;

	virtual bool Compute(const Car& car) = 0;

};

}// namespace VVASE