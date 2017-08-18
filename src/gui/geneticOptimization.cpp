/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  geneticOptimization.cpp
// Date:  4/7/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  This is a GUI interface for the genetic algorithm optimization.

// Local headers
#include "VVASE/gui/geneticOptimization.h"
#include "guiCar.h"
#include "VVASE/gui/guiObject.h"
#include "gaObject.h"
#include "VVASE/gui/components/mainTree.h"
#include "VVASE/gui/components/mainFrame.h"
#include "geneticAlgorithmPanel.h"
#include "VVASE/core/car/car.h"
#include "VVASE/core/car/subsystems/suspension.h"
#include "VVASE/core/analysis/kinematics.h"
#include "VVASE/core/analysis/kinematicOutputs.h"
#include "VVASE/core/threads/threadJob.h"
#include "VVASE/core/threads/optimizationData.h"
#include "VVASE/core/utilities/carMath.h"
#include "VVASE/gui/utilities/wxRelatedUtilities.h"

namespace VVASE
{

//==========================================================================
// Class:			GeneticOptimization
// Function:		GeneticOptimization
//
// Description:		Constructor for GeneticOptimization class.
//
// Input Arguments:
//		mainFrame			= MainFrame& reference to main application object
//		pathAndFileName	= wxString containing this objects location on the disk,
//							  if we are to load from file
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GeneticOptimization::GeneticOptimization(MainFrame &mainFrame,
	wxString pathAndFileName) : GuiObject(mainFrame, pathAndFileName)
{
	geneticAlgorithm = new GAObject(mainFrame.GetJobQueue(), *this);

	// Get an index for this item and add it to the list in the mainFrame
	// MUST be included BEFORE the naming, which must come BEFORE the call to Initialize
	index = mainFrame.AddObjectToList(this);

	name.Printf("Unsaved Optimization %i", index + 1);

	gaPanel = new GeneticAlgorithmPanel(mainFrame, *this);
	notebookTab = reinterpret_cast<wxWindow*>(gaPanel);

	carToOptimize = NULL;

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
void GeneticOptimization::BeginOptimization()
{
	// Make sure we have a car to optimize and genes and goals
	assert(carToOptimize);
	assert(geneticAlgorithm->GetGeneCount());
	assert(geneticAlgorithm->GetGoalCount());

	// Add a job for the genetic algorithm - this is the manager thread from which
	// the analysis jobs are created
	std::unique_ptr<OptimizationData> data(std::make_unique<OptimizationData>(geneticAlgorithm));
	ThreadJob job(ThreadJob::CommandThreadGeneticOptimization, std::move(data),
		wxUtilities::ToVVASEString(name), index);
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
void GeneticOptimization::HaltOptimization()
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
bool GeneticOptimization::PerformLoadFromFile()
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
bool GeneticOptimization::PerformSaveToFile()
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
void GeneticOptimization::MarkAnalysisComplete()
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
int GeneticOptimization::GetIconHandle() const
{
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
void GeneticOptimization::CompleteOptimization()
{
	// Set the text on the start/stop optimization button back to "Start GA"
	// FIXME!!!

	GuiCar *result = new GuiCar(mainFrame);
	geneticAlgorithm->UpdateResultingCar(result->GetOriginalCar());
	result->SetName(_T("Unsaved Result : ") + name);
	result->SetModified();
	mainFrame.SetActiveIndex(result->GetIndex());
	mainFrame.UpdateAnalysis();
}

//==========================================================================
// Class:			GeneticOptimization
// Function:		SetCarToOptimize
//
// Description:		Sets the pointer for the car to optimized to the address
//					specified.
//
// Input Arguments:
//		carToOptimize	= const GuiCar& to be optimized
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GeneticOptimization::SetCarToOptimize(const GuiCar &carToOptimize)
{
	this->carToOptimize = &carToOptimize;
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
void GeneticOptimization::UpdateDisplay()
{
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
void GeneticOptimization::UpdateData()
{
	// This is called at various points during the load process due to different
	// GUI screens being created and their requests for data.  This can result in
	// errors.  To combat this, we check to make sure we're done loading before
	// we continue.
	if (!objectIsInitialized)
		return;

	gaPanel->UpdateInformation();
}

}// namespace VVASE
