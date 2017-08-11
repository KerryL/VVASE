/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  geneticOptimization.h
// Date:  4/7/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  This is a GUI interface for the genetic algorithm optimization.

#ifndef GENETIC_OPTIMIZATION_H_
#define GENETIC_OPTIMIZATION_H_

// Local headers
#include "VVASE/gui/guiObject.h"
#include "VVASE/core/car/subsystems/corner.h"
#include "VVASE/core/analysis/kinematicOutputs.h"
#include "VVASE/core/analysis/kinematics.h"

namespace VVASE
{

// Local forward declarations
class GuiCar;
class GAObject;
class MainFrame;
class GeneticAlgorithmPanel;

class GeneticOptimization : public GuiObject
{
public:
	GeneticOptimization(MainFrame &mainFrame,
		wxString pathAndFileName = wxEmptyString);
	~GeneticOptimization();

	// Mandatory overload
	ItemType GetType() const { return TypeOptimization; }

	// Mandatory overloads
	void UpdateData();
	void UpdateDisplay();

	// Controls for the optimization process
	void BeginOptimization();
	void HaltOptimization();

	// Decrements outstanding analysis counter
	void MarkAnalysisComplete();

	// Performs the post-optimization tasks, such as updating the interface and
	// updating the optimized car to match the best fit genome
	void CompleteOptimization();

	// Accessor for the GA panel object
	GeneticAlgorithmPanel *GetGAPanel() { return gaPanel; }

	// Accessor for the GA object itself
	GAObject &GetAlgorithm() { return *geneticAlgorithm; }

	// Accessors for the car to optimize
	const GuiCar& GetCarToOptimize() { return *carToOptimize; }
	void SetCarToOptimize(const GuiCar &carToOptimize);

private:
	int GetIconHandle() const;

	// File I/O (required overloads)
	bool PerformLoadFromFile();
	bool PerformSaveToFile();

	GeneticAlgorithmPanel *gaPanel;
	const GuiCar *carToOptimize;
	GAObject *geneticAlgorithm;
};

}// namespacde VVASE

#endif// GENETIC_OPTIMIZATION_H_
