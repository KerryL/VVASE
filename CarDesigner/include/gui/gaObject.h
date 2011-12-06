/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  gaObject.h
// Created:  4/7/2009
// Author:  K. Loux
// Description:  This is a wrapper class for genetic algorithm implementation.
// History:

#ifndef _GA_OBJECT_H_
#define _GA_OBJECT_H_

// VVASE headers
#include "vUtilities/managedList.h"
#include "vCar/corner.h"
#include "vMath/vector.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vSolver/physics/kinematics.h"
#include "vSolver/optimization/geneticAlgorithm.h"
#include "vSolver/threads/inverseSemaphore.h"
#include "vUtilities/debugLog.h"

// VVASE forward declarations
class GuiCar;
class Car;
class MainFrame;
class GeneticOptimization;

class GAObject : public GeneticAlgorithm
{
public:
	// Constructor
	GAObject(MainFrame &_mainFrame, GeneticOptimization &_optimization);

	// Destructor
	~GAObject();

	// Sets up for the run
	void SetUp(Car *_targetCar);

	// For storing the information about the genes
	struct Gene
	{
		// The value to be altered
		Corner::Hardpoints hardpoint;

		// A variable that will always be set to the same value as Variable (optional)
		Corner::Hardpoints tiedTo;

		// The corner containing these points
		Corner::Location location;

		// The component of the hardpoint to optimitze
		Vector::Axis direction;

		// The minimum value for the gene
		double minimum;

		// The maximum value for the gene
		double maximum;

		// The number of possible values (including the min and max) for the gene
		int numberOfValues;
	};

	// The structure that represents our goals
	struct Goal
	{
		// The output associated with this goal
		KinematicOutputs::OutputsComplete output;

		// The desired value
		double desiredValue;

		// The values used to compute the weight for this output
		double expectedDeviation;
		double importance;

		// The conditions at which this goal is evaluated.  There are two sets of inputs
		// here, so that a delta goal can be implemented (i.e. change in roll center height
		// with ride).
		Kinematics::Inputs beforeInputs;
		Kinematics::Inputs afterInputs;
	};

	// For changing what is being optimized
	void ClearAllGenes(void) { geneList.Clear(); };
	void AddGene(const Corner::Hardpoints &hardpoint, const Corner::Hardpoints &tiedTo,
		const Corner::Location &location, const Vector::Axis &direction, const double &minimum,
		const double &maximum, const int &numberOfValues);
	void RemoveGene(const int &index) { geneList.Remove(index); };
	void UpdateGene(const int &index, const Corner::Hardpoints &hardpoint, const Corner::Hardpoints &tiedTo,
		const Corner::Location &location, const Vector::Axis &direction, const double &minimum,
		const double &maximum, const int &numberOfValues);
	int GetGeneCount(void) const { return geneList.GetCount(); };
	const Gene &GetGene(const int &index) const { return *(geneList[index]); };

	// For creating the fitness function
	void ClearAllGoals(void) { goalList.Clear(); };
	void AddGoal(const KinematicOutputs::OutputsComplete &output, const double &desiredValue,
		const double &expectedDeviation, const double &importance, const Kinematics::Inputs &beforeInputs,
		const Kinematics::Inputs &afterInputs);
	void RemoveGoal(const int &index) { goalList.Remove(index); };
	void UpdateGoal(const int &index, const KinematicOutputs::OutputsComplete &beforeOutput, const double &desiredValue,
		const double &expectedDeviation, const double &importance, const Kinematics::Inputs &beforeInputs,
		const Kinematics::Inputs &afterInputs);
	int GetGoalCount(void) const { return goalList.GetCount(); };
	const Goal &GetGoal(const int &index) const { return *(goalList[index]); };

	// File I/O
	bool Write(wxString fileName);
	bool Read(wxString fileName);

	// Accessor for synchronization object
	void MarkAnalysisComplete(void) { inverseSemaphore.Post(); };

	// Gets the number of analyses to be performed for each generation
	int GetNumberOfInputs(void) const { wxMutexLocker lock(gsaMutex); DebugLog::GetInstance()->Log(_T("GAObject::GetNumberOfInputs()")); return inputList.GetCount(); };

	// Returns status of this object (running or not)
	bool OptimizationIsRunning(void) const {  wxMutexLocker lock(gsaMutex); DebugLog::GetInstance()->Log(_T("GAObject::OptimizationIsRunning()")); return isRunning; };

	// Returns the best car with the best fit genome
	void UpdateTargetCar(void);

private:
	// Main application object
	MainFrame &mainFrame;

	// The object that owns this optimization
	GeneticOptimization &optimization;

	// Mandatory override of fitness function
	double DetermineFitness(const int *citizen);

	// Optional override to display results every step
	void PerformAdditionalActions(void);

	// Optional override for determining fitnesses - overridden here to make use of threading
	void SimulateGeneration(void);

	// Array of cars with which the fitnesses are determined
	Car **workingCarArray;
	Car **originalCarArray;
	KinematicOutputs *kinematicOutputArray;
	int numberOfCars;

	// Original car to be optimized (only one needed for reference)
	Car *targetCar;

	// The list of genes that we're optimizing
	ManagedList<Gene> geneList;

	// The list of goal's we're optimizing to
	ManagedList<Goal> goalList;

	// The different input configurations to be run
	ManagedList<Kinematics::Inputs> inputList;
	void DetermineAllInputs(void);

	// Converts a genome into a citizen
	void SetCarGenome(int carIndex, const int *currentGenome);

	// Flag for whether or not the optimization is running
	bool isRunning;

	// Synchronization object allowing this thread to wait for analyses to be completed
	InverseSemaphore inverseSemaphore;

	// File header information
	struct FileHeaderInfo
	{
		int fileVersion;
	};

	// Writes and reads the file header information for saved cars
	void WriteFileHeader(std::ofstream *outFile);
	FileHeaderInfo ReadFileHeader(std::ifstream *inFile);

	// Our current file version
	static const int currentFileVersion;
};

#endif// _GA_OBJECT_H_