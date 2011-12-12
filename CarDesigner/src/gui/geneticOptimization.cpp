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
//	1/11/2010	- Moved from test class to GuiObject derived object for permanent
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
// Class:			GeneticOptimization
// Function:		GeneticOptimization
//
// Description:		Constructor for GeneticOptimization class.
//
// Input Arguments:
//		_mainFrame			= MainFrame& reference to main application object
//		_pathAndFileName	= wxString containing this objects location on the disk,
//							  if we are to load from file
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GeneticOptimization::GeneticOptimization(MainFrame &_mainFrame, 
										   wxString _pathAndFileName) : GuiObject(_mainFrame,
										   _pathAndFileName)
{
	// Create the genetic algorithm
	geneticAlgorithm = new GAObject(_mainFrame.GetEventHandler(), *this);

	// Get an index for this item and add it to the list in the mainFrame
	// MUST be included BEFORE the naming, which must come BEFORE the call to Initialize
	index = mainFrame.AddObjectToList(this);

	// Create the name based on the index
	name.Printf("Unsaved Optimization %i", index + 1);

	// Create the panel containing the algorithm's controls
	gaPanel = new GeneticAlgorithmPanel(_mainFrame, *this);
	notebookTab = reinterpret_cast<wxWindow*>(gaPanel);

	// Initialize the car to optimize
	carToOptimize = NULL;

	// Complete initialization of this object
	Initialize();
}

//==========================================================================
// Class:			GeneticOptimization
// Function:		~GeneticOptimization
//
// Description:		Destructor for GeneticOptimization class.
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
GeneticOptimization::~GeneticOptimization()
{
	// Delete the genetic algorithm object
	delete geneticAlgorithm;
	geneticAlgorithm = NULL;
}

//==========================================================================
// Class:			GeneticOptimization
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
void GeneticOptimization::BeginOptimization(void)
{
	// Make sure we have a car to optimize and genes and goals
	assert(carToOptimize);
	assert(geneticAlgorithm->GetGeneCount());
	assert(geneticAlgorithm->GetGoalCount());

	// Add a job for the genetic algorithm - this is the manager thread from which
	// the analysis jobs are created
	OptimizationData *data = new OptimizationData(geneticAlgorithm);
	ThreadJob job(ThreadJob::CommandThreadGeneticOptimization, data,
		name, index);
	mainFrame.AddJob(job);
}

//==========================================================================
// Class:			GeneticOptimization
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
void GeneticOptimization::HaltOptimization(void)
{
}

//==========================================================================
// Class:			GeneticOptimization
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
bool GeneticOptimization::PerformLoadFromFile(void)
{
	// Perform the load
	bool loadSuccessful = geneticAlgorithm->Read(pathAndFileName);

	return loadSuccessful;
}

//==========================================================================
// Class:			GeneticOptimization
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
bool GeneticOptimization::PerformSaveToFile(void)
{
	return geneticAlgorithm->Write(pathAndFileName);
}

//==========================================================================
// Class:			GeneticOptimization
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
void GeneticOptimization::MarkAnalysisComplete(void)
{
	// Tell the algorithm that we've completed an analysis
	geneticAlgorithm->MarkAnalysisComplete();

	// Also tell the panel that we've completed an analysis - this is what
	// makes the status bars fill in
	gaPanel->IncrementStatusBars();
}

//==========================================================================
// Class:			GeneticOptimization
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
int GeneticOptimization::GetIconHandle(void) const
{
	// Return the proper icon handle
	return systemsTree->GetIconHandle(MainTree::OptimizationIcon);
}

//==========================================================================
// Class:			GeneticOptimization
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
void GeneticOptimization::CompleteOptimization(void)
{
	// Set the text on the start/stop optimization button back to "Start GA"
	// FIXME!!!

	// Update the selected car to reflect the optimization
	geneticAlgorithm->UpdateTargetCar();

	// Mark the target car as modified
	carToOptimize->SetModified();
}

//==========================================================================
// Class:			GeneticOptimization
// Function:		SetCarToOptimize
//
// Description:		Sets the pointer for the car to optimized to the address
//					specified.
//
// Input Arguments:
//		_carToOptimize	= GuiCar* to be optimized
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GeneticOptimization::SetCarToOptimize(GuiCar *_carToOptimize)
{
	// Make sure the car exists
	assert(_carToOptimize);

	// Do the assignment
	carToOptimize = _carToOptimize;
}

//==========================================================================
// Class:			GeneticOptimization
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
void GeneticOptimization::UpdateDisplay(void)
{
	// Update the display
	gaPanel->UpdateInformation();
}

//==========================================================================
// Class:			GeneticOptimization
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
void GeneticOptimization::UpdateData(void)
{
	// This is called at various points during the load process due to different
	// GUI screens being created and their requests for data.  This can result in
	// errors.  To combat this, we check to make sure we're done loading before
	// we continue.
	if (!objectIsInitialized)
		return;

	// Update the display
	gaPanel->UpdateInformation();
}