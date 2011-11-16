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
class GUI_CAR;
class CAR;
class Debugger;
class Convert;
class GA_OBJECT;
class MAIN_FRAME;
class GENETIC_ALGORITHM_PANEL;

class GENETIC_OPTIMIZATION : public GUI_OBJECT
{
public:
	// Constructor
	GENETIC_OPTIMIZATION(MAIN_FRAME &_MainFrame, const Debugger &_debugger, const Convert &Converter,
		wxString _PathAndFileName = wxEmptyString);

	// Destructor
	~GENETIC_OPTIMIZATION();

	// Mandatory overload
	ITEM_TYPE GetType(void) const { return GUI_OBJECT::TYPE_OPTIMIZATION; };

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
	GENETIC_ALGORITHM_PANEL *GetGAPanel(void) { return GAPanel; };

	// Accessor for the GA object itself
	GA_OBJECT &GetAlgorithm(void) { return *GeneticAlgorithm; };

	// Accessors for the car to optimize
	GUI_CAR *GetCarToOptimize(void) { return CarToOptimize; };
	void SetCarToOptimize(GUI_CAR *_CarToOptimize);

private:
	// Debugger printing utility
	const Debugger &debugger;

	// Gets the icon handle for this object's icon
	int GetIconHandle(void) const;

	// File I/O (required overloads)
	bool PerformLoadFromFile(void);
	bool PerformSaveToFile(void);

	// The panel that appears in the main notebook
	GENETIC_ALGORITHM_PANEL *GAPanel;

	// The object we're working with
	GUI_CAR *CarToOptimize;

	// The genetic algorithm object (actually, this is our special wrapper)
	GA_OBJECT *GeneticAlgorithm;
};

#endif// _GENETIC_OPTIMIZATION_H_