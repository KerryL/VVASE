/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  geneticOptimization.h
// Created:  4/7/2009
// Author:  K. Loux
// Description:  This is a GUI interface for the genetic algorithm optimization.
// History:
//	1/11/2010	- Moved from test class to GUI_OBJECT derived object for permanent
//				  implementation.

#ifndef _GENETIC_OPTIMIZATION_H_
#define _GENETIC_OPTIMIZATION_H_

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
	// Constructor
	GeneticOptimization(MainFrame &_mainFrame,
		wxString _pathAndFileName = wxEmptyString);

	// Destructor
	~GeneticOptimization();

	// Mandatory overload
	ItemType GetType(void) const { return TypeOptimization; };

	// Mandatory overloads
	void UpdateData(void);
	void UpdateDisplay(void);

	// Controls for the optimization process
	void BeginOptimization(void);
	void HaltOptimization(void);

	// Decrements outstanding analysis counter
	void MarkAnalysisComplete(void);

	// Performs the post-optimization tasks, such as updating the interface and
	// updating the optimized car to match the best fit genome
	void CompleteOptimization(void);

	// Accessor for the GA panel object
	GeneticAlgorithmPanel *GetGAPanel(void) { return gaPanel; };

	// Accessor for the GA object itself
	GAObject &GetAlgorithm(void) { return *geneticAlgorithm; };

	// Accessors for the car to optimize
	GuiCar *GetCarToOptimize(void) { return carToOptimize; };
	void SetCarToOptimize(GuiCar *_carToOptimize);

private:
	// Gets the icon handle for this object's icon
	int GetIconHandle(void) const;

	// File I/O (required overloads)
	bool PerformLoadFromFile(void);
	bool PerformSaveToFile(void);

	// The panel that appears in the main notebook
	GeneticAlgorithmPanel *gaPanel;

	// The object we're working with
	GuiCar *carToOptimize;

	// The genetic algorithm object (actually, this is our special wrapper)
	GAObject *geneticAlgorithm;
};

#endif// _GENETIC_OPTIMIZATION_H_