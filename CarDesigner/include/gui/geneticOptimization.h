/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  geneticOptimization.h
// Created:  4/7/2009
// Author:  K. Loux
// Description:  This is a GUI interface for the genetic algorithm optimization.
// History:
//	1/11/2010	- Moved from test class to GuiObject derived object for permanent
//				  implementation.

#ifndef GENETIC_OPTIMIZATION_H_
#define GENETIC_OPTIMIZATION_H_

// VVASE headers
#include "gui/guiObject.h"
#include "vUtilities/managedList.h"
#include "vCar/corner.h"
#include "vMath/vector.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vSolver/physics/kinematics.h"

// VVASE forward declarations
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

#endif// GENETIC_OPTIMIZATION_H_