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

// VVASE forward declarations
class GUI_CAR;
class Car;
class Debugger;
class MAIN_FRAME;
class GENETIC_OPTIMIZATION;

class GA_OBJECT : public GeneticAlgorithm
{
public:
	// Constructor
	GA_OBJECT(MAIN_FRAME &_MainFrame, GENETIC_OPTIMIZATION &_Optimization,
		const Convert &_converter, const Debugger &_debugger);

	// Destructor
	~GA_OBJECT();

	// Sets up for the run
	void SetUp(Car *_TargetCar);

	// For storing the information about the genes
	struct GENE
	{
		// The value to be altered
		Corner::Hardpoints Hardpoint;

		// A variable that will always be set to the same value as Variable (optional)
		Corner::Hardpoints TiedTo;

		// The corner containing these points
		Corner::Location Location;

		// The component of the hardpoint to optimitze
		Vector::Axis Direction;

		// The minimum value for the gene
		double Minimum;

		// The maximum value for the gene
		double Maximum;

		// The number of possible values (including the min and max) for the gene
		int NumberOfValues;
	};

	// The structure that represents our goals
	struct GOAL
	{
		// The output associated with this goal
		KinematicOutputs::OutputsComplete Output;

		// The desired value
		double DesiredValue;

		// The values used to compute the weight for this output
		double ExpectedDeviation;
		double Importance;

		// The conditions at which this goal is evaluated.  There are two sets of inputs
		// here, so that a delta goal can be implemented (i.e. change in roll center height
		// with ride).
		Kinematics::Inputs BeforeInputs;
		Kinematics::Inputs AfterInputs;
	};

	// For changing what is being optimized
	void ClearAllGenes(void) { GeneList.Clear(); };
	void AddGene(const Corner::Hardpoints &Hardpoint, const Corner::Hardpoints &TiedTo,
		const Corner::Location &Location, const Vector::Axis &Direction, const double &Minimum,
		const double &Maximum, const int &NumberOfValues);
	void RemoveGene(const int &Index) { GeneList.Remove(Index); };
	void UpdateGene(const int &Index, const Corner::Hardpoints &Hardpoint, const Corner::Hardpoints &TiedTo,
		const Corner::Location &Location, const Vector::Axis &Direction, const double &Minimum,
		const double &Maximum, const int &NumberOfValues);
	int GetGeneCount(void) const { return GeneList.GetCount(); };
	const GENE &GetGene(const int &Index) const { return *(GeneList[Index]); };

	// For creating the fitness function
	void ClearAllGoals(void) { GoalList.Clear(); };
	void AddGoal(const KinematicOutputs::OutputsComplete &Output, const double &DesiredValue,
		const double &ExpectedDeviation, const double &Importance, const Kinematics::Inputs &BeforeInputs,
		const Kinematics::Inputs &AfterInputs);
	void RemoveGoal(const int &Index) { GoalList.Remove(Index); };
	void UpdateGoal(const int &Index, const KinematicOutputs::OutputsComplete &BeforeOutput, const double &DesiredValue,
		const double &ExpectedDeviation, const double &Importance, const Kinematics::Inputs &BeforeInputs,
		const Kinematics::Inputs &AfterInputs);
	int GetGoalCount(void) const { return GoalList.GetCount(); };
	const GOAL &GetGoal(const int &Index) const { return *(GoalList[Index]); };

	// File I/O
	bool Write(wxString FileName);
	bool Read(wxString FileName);

	// Accessor for synchronization object
	void MarkAnalysisComplete(void) { inverseSemaphore.Post(); };

	// Gets the number of analyses to be performed for each generation
	int GetNumberOfInputs(void) const { wxMutexLocker lock(gsaMutex); return InputList.GetCount(); };

	// Returns status of this object (running or not)
	bool OptimizationIsRunning(void) const {  wxMutexLocker lock(gsaMutex); return IsRunning; };

	// Returns the best car with the best fit genome
	void UpdateTargetCar(void);

private:
	// Debugger printing utility
	const Debugger &debugger;

	// Application's unit converter
	const Convert &converter;

	// Main application object
	MAIN_FRAME &MainFrame;

	// The object that owns this optimization
	GENETIC_OPTIMIZATION &Optimization;

	// Mandatory override of fitness function
	double DetermineFitness(const int *Citizen);

	// Optional override to display results every step
	void PerformAdditionalActions(void);

	// Optional override for determining fitnesses - overridden here to make use of threading
	void SimulateGeneration(void);

	// Array of cars with which the fitnesses are determined
	Car **WorkingCarArray;
	Car **OriginalCarArray;
	KinematicOutputs *KinematicOutputArray;
	int NumberOfCars;

	// Original car to be optimized (only one needed for reference)
	Car *TargetCar;

	// The list of genes that we're optimizing
	ManagedList<GENE> GeneList;

	// The list of goal's we're optimizing to
	ManagedList<GOAL> GoalList;

	// The different input configurations to be run
	ManagedList<Kinematics::Inputs> InputList;
	void DetermineAllInputs(void);

	// Converts a genome into a citizen
	void SetCarGenome(int CarIndex, const int *CurrentGenome);

	// Flag for whether or not the optimization is running
	bool IsRunning;

	// Synchronization object allowing this thread to wait for analyses to be completed
	InverseSemaphore inverseSemaphore;

	// File header information
	struct FILE_HEADER_INFO
	{
		int FileVersion;
	};

	// Writes and reads the file header information for saved cars
	void WriteFileHeader(std::ofstream *OutFile);
	FILE_HEADER_INFO ReadFileHeader(std::ifstream *InFile);

	// Our current file version
	static const int CurrentFileVersion;
};

#endif// _GA_OBJECT_H_