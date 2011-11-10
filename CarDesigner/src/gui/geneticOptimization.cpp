/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  geneticOptimization.cpp
// Created:  4/7/2009
// Author:  K. Loux
// Description:  This is a GUI interface for the genetic algorithm optimization.
// History:
//	1/11/2010	- Moved from test class to GUI_OBJECT derived object for permanent
//				  implementation.

// CarDesigner headers
#include "gui/geneticOptimization.h"
#include "gui/guiCar.h"
#include "gui/guiObject.h"
#include "gui/gaObject.h"
#include "gui/components/mainTree.h"
#include "gui/components/mainFrame.h"
#include "gui/geneticAlgorithmPanel.h"
#include "vCar/car.h"
#include "vCar/suspension.h"
#include "vSolver/physics/kinematics.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vSolver/threads/threadJob.h"
#include "vSolver/threads/optimizationData.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			GENETIC_OPTIMIZATION
// Function:		GENETIC_OPTIMIZATION
//
// Description:		Constructor for GENETIC_OPTIMIZATION class.
//
// Input Arguments:
//		_MainFrame			= MAIN_FRAME& reference to main application object
//		_debugger			= const Debugger& reference to the debug printing utility
//		Converter			= const Convert& reference to application's conversion utility
//		_PathAndFileName	= wxString containing this objects location on the disk,
//							  if we are to load from file
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GENETIC_OPTIMIZATION::GENETIC_OPTIMIZATION(MAIN_FRAME &_MainFrame, const Debugger &_debugger,
										   const Convert &Converter,
										   wxString _PathAndFileName) : GUI_OBJECT(_MainFrame, _debugger,
										   _PathAndFileName), debugger(_debugger)
{
	// Create the genetic algorithm
	GeneticAlgorithm = new GA_OBJECT(_MainFrame, *this, Converter, debugger);

	// Get an index for this item and add it to the list in the MainFrame
	// MUST be included BEFORE the naming, which must come BEFORE the call to Initialize
	Index = MainFrame.AddObjectToList(this);

	// Create the name based on the index
	Name.Printf("Unsaved Optimization %i", Index + 1);

	// Create the panel containing the algorithm's controls
	GAPanel = new GENETIC_ALGORITHM_PANEL(_MainFrame, *this);
	notebookTab = reinterpret_cast<wxWindow*>(GAPanel);

	// Initialize the car to optimize
	CarToOptimize = NULL;

	// Complete initialization of this object
	Initialize();
}

//==========================================================================
// Class:			GENETIC_OPTIMIZATION
// Function:		~GENETIC_OPTIMIZATION
//
// Description:		Destructor for GENETIC_OPTIMIZATION class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GENETIC_OPTIMIZATION::~GENETIC_OPTIMIZATION()
{
	// Delete the genetic algorithm object
	delete GeneticAlgorithm;
	GeneticAlgorithm = NULL;
}

//==========================================================================
// Class:			GENETIC_OPTIMIZATION
// Function:		BeginOptimization
//
// Description:		Spawns the GA thread.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_OPTIMIZATION::BeginOptimization(void)
{
	// Make sure we have a car to optimize and genes and goals
	assert(CarToOptimize);
	assert(GeneticAlgorithm->GetGeneCount());
	assert(GeneticAlgorithm->GetGoalCount());

	// Add a job for the genetic algorithm - this is the manager thread from which
	// the analysis jobs are created
	OPTIMIZATION_DATA *Data = new OPTIMIZATION_DATA(GeneticAlgorithm);
	THREAD_JOB Job(THREAD_JOB::COMMAND_THREAD_GENETIC_OPTIMIZATION, Data,
		Name, Index);
	MainFrame.AddJob(Job);

	return;
}

//==========================================================================
// Class:			GENETIC_OPTIMIZATION
// Function:		HaltOptimization
//
// Description:		Stops the GA thread.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_OPTIMIZATION::HaltOptimization(void)
{
	return;
}

//==========================================================================
// Class:			GENETIC_OPTIMIZATION
// Function:		PerformLoadFromFile
//
// Description:		Loads this object's data from file.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
bool GENETIC_OPTIMIZATION::PerformLoadFromFile(void)
{
	// Perform the load
	bool LoadSuccessful = GeneticAlgorithm->Read(PathAndFileName);

	return LoadSuccessful;
}

//==========================================================================
// Class:			GENETIC_OPTIMIZATION
// Function:		PerformSaveToFile
//
// Description:		Saves this object's data to file.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
bool GENETIC_OPTIMIZATION::PerformSaveToFile(void)
{
	return GeneticAlgorithm->Write(PathAndFileName);
}

//==========================================================================
// Class:			GENETIC_OPTIMIZATION
// Function:		MarkAnalysisComplete
//
// Description:		Decrements the counter of outstanding analyses.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_OPTIMIZATION::MarkAnalysisComplete(void)
{
	// Tell the algorithm that we've completed an analysis
	GeneticAlgorithm->MarkAnalysisComplete();

	// Also tell the panel that we've completed an analysis - this is what
	// makes the status bars fill in
	GAPanel->IncrementStatusBars();

	return;
}

//==========================================================================
// Class:			GENETIC_OPTIMIZATION
// Function:		GetIconHandle
//
// Description:		Returns the handle for this object's entry in the SystemsTree.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
int GENETIC_OPTIMIZATION::GetIconHandle(void) const
{
	// Return the proper icon handle
	return SystemsTree->GetIconHandle(MAIN_TREE::OptimizationIcon);
}

//==========================================================================
// Class:			GENETIC_OPTIMIZATION
// Function:		CompleteOptimization
//
// Description:		Performs the post-optimization tasks, such as updating
//					the GUI_CAR to match the best fit genome.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_OPTIMIZATION::CompleteOptimization(void)
{
	// Set the text on the start/stop optimization button back to "Start GA"
	// FIXME!!!

	// Update the selected car to reflect the optimization
	GeneticAlgorithm->UpdateTargetCar();

	// Mark the target car as modified
	CarToOptimize->SetModified();

	return;
}

//==========================================================================
// Class:			GENETIC_OPTIMIZATION
// Function:		SetCarToOptimize
//
// Description:		Sets the pointer for the car to optimized to the address
//					specified.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_OPTIMIZATION::SetCarToOptimize(GUI_CAR *_CarToOptimize)
{
	// Make sure the car exists
	assert(_CarToOptimize);

	// Do the assignment
	CarToOptimize = _CarToOptimize;

	return;
}

//==========================================================================
// Class:			GENETIC_OPTIMIZATION
// Function:		UpdateDisplay
//
// Description:		Updates the display of the panel representing this object.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_OPTIMIZATION::UpdateDisplay(void)
{
	// Update the display
	GAPanel->UpdateInformation();

	return;
}

//==========================================================================
// Class:			GENETIC_OPTIMIZATION
// Function:		UpdateData
//
// Description:		Updates the display of the panel representing this object.
//					This is identical to UpdateDisplay().
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_OPTIMIZATION::UpdateData(void)
{
	// This is called at various points during the load process due to different
	// GUI screens being created and their requests for data.  This can result in
	// errors.  To combat this, we check to make sure we're done loading before
	// we continue.
	if (!ObjectIsInitialized)
		return;

	// Update the display
	GAPanel->UpdateInformation();

	return;
}