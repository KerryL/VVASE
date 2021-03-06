/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  gaObject.h
// Created:  4/7/2009
// Author:  K. Loux
// Description:  This is a wrapper class for genetic algorithm implementation.
// History:

#ifndef GA_OBJECT_H_
#define GA_OBJECT_H_

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
class JobQueue;
class GuiCar;
class Car;
class GeneticOptimization;

class GAObject : public GeneticAlgorithm
{
public:
	GAObject(JobQueue &queue, GeneticOptimization &optimization);
	~GAObject();

	void SetUp(const Car &targetCar);

	// For storing the information about the genes
	struct Gene
	{
		Corner::Hardpoints hardpoint;
		Corner::Hardpoints tiedTo;
		Corner::Location location;
		Vector::Axis direction;

		double minimum;
		double maximum;
		int numberOfValues;
	};

	// The structure that represents our goals
	struct Goal
	{
		KinematicOutputs::OutputsComplete output;

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
	void ClearAllGenes() { geneList.Clear(); }
	void AddGene(const Corner::Hardpoints &hardpoint, const Corner::Hardpoints &tiedTo,
		const Corner::Location &location, const Vector::Axis &direction, const double &minimum,
		const double &maximum, const int &numberOfValues);
	void RemoveGene(const int &index) { geneList.Remove(index); }
	void UpdateGene(const int &index, const Corner::Hardpoints &hardpoint, const Corner::Hardpoints &tiedTo,
		const Corner::Location &location, const Vector::Axis &direction, const double &minimum,
		const double &maximum, const int &numberOfValues);
	int GetGeneCount() const { return geneList.GetCount(); }
	const Gene &GetGene(const int &index) const { return *(geneList[index]); }

	// For creating the fitness function
	void ClearAllGoals() { goalList.Clear(); }
	void AddGoal(const KinematicOutputs::OutputsComplete &output, const double &desiredValue,
		const double &expectedDeviation, const double &importance, const Kinematics::Inputs &beforeInputs,
		const Kinematics::Inputs &afterInputs);
	void RemoveGoal(const int &index) { goalList.Remove(index); }
	void UpdateGoal(const int &index, const KinematicOutputs::OutputsComplete &beforeOutput, const double &desiredValue,
		const double &expectedDeviation, const double &importance, const Kinematics::Inputs &beforeInputs,
		const Kinematics::Inputs &afterInputs);
	int GetGoalCount() const { return goalList.GetCount(); }
	const Goal &GetGoal(const int &index) const { return *(goalList[index]); }

	// File I/O
	bool Write(wxString fileName);
	bool Read(wxString fileName);

	void MarkAnalysisComplete() { inverseSemaphore.Post(); }
	int GetNumberOfInputs() const { wxMutexLocker lock(gsaMutex); DebugLog::GetInstance()->Log(_T("GAObject::GetNumberOfInputs()")); return inputList.GetCount(); }
	bool OptimizationIsRunning() const {  wxMutexLocker lock(gsaMutex); DebugLog::GetInstance()->Log(_T("GAObject::OptimizationIsRunning()")); return isRunning; }

	void UpdateResultingCar(Car& result) const;

private:
	JobQueue &queue;

	GeneticOptimization &optimization;

	// Mandatory override of fitness function
	double DetermineFitness(const int *citizen);

	// Optional override to display results every step
	void PerformAdditionalActions();

	// Optional override for determining fitnesses - overridden here to make use of threading
	void SimulateGeneration();

	// Array of cars with which the fitnesses are determined
	Car **workingCarArray;
	Car **originalCarArray;
	KinematicOutputs *kinematicOutputArray;
	unsigned int numberOfCars;

	// Original car to be optimized (only one needed for reference)
	const Car *targetCar;

	ManagedList<Gene> geneList;
	ManagedList<Goal> goalList;

	// The different input configurations to be run
	ManagedList<Kinematics::Inputs> inputList;
	void DetermineAllInputs();

	// Converts a genome into a citizen
	void SetCarGenome(int carIndex, const int *currentGenome);

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

#endif// GA_OBJECT_H_