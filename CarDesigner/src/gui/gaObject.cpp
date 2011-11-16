/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  gaObject.cpp
// Created:  4/7/2009
// Author:  K. Loux
// Description:  This is a class for testing genetic algorithm implementation.
// History:

// Standard C++ headers
#include <fstream>

// CarDesigner headers
#include "gui/gaObject.h"
#include "gui/geneticOptimization.h"
#include "gui/components/mainFrame.h"
#include "vSolver/physics/kinematics.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vSolver/threads/kinematicsData.h"
#include "vCar/car.h"
#include "vCar/suspension.h"
#include "vMath/carMath.h"
#include "vUtilities/convert.h"
#include "vUtilities/machineDefinitions.h"

//==========================================================================
// Class:			GA_OBJECT
// Function:		GA_OBJECT
//
// Description:		Constructor for GA_OBJECT class.
//
// Input Arguments:
//		_MainFrame		= MAIN_FRAME& reference to the main application object
//		_Optimization	= GENETIC_OPTIMIZATION pointing to this object's owner
//		_Converter		= const Convert& reference to application's conversion utility
//		_debugger		= const Debugger& reference to the debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GA_OBJECT::GA_OBJECT(MAIN_FRAME &_MainFrame, GENETIC_OPTIMIZATION &_Optimization, const Convert &_converter,
					 const Debugger &_debugger) : debugger(_debugger), converter(_converter),
					 MainFrame(_MainFrame), Optimization(_Optimization)
{
	// Initialize class members
	WorkingCarArray = NULL;
	OriginalCarArray = NULL;
	NumberOfCars = 0;
	KinematicOutputArray = NULL;
	IsRunning = false;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		~GA_OBJECT
//
// Description:		Destructor for GA_OBJECT class.
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
GA_OBJECT::~GA_OBJECT()
{
	wxMutexLocker lock(gsaMutex);

	// Delete this object's CAR members
	int i;
	for (i = 0; i < NumberOfCars; i++)
	{
		delete OriginalCarArray[i];
		OriginalCarArray[i] = NULL;

		delete WorkingCarArray[i];
		WorkingCarArray[i] = NULL;
	}
	delete [] OriginalCarArray;
	OriginalCarArray = NULL;
	delete [] WorkingCarArray;
	WorkingCarArray = NULL;

	delete [] KinematicOutputArray;
	KinematicOutputArray = NULL;

	// Clean up the lists
	GeneList.Clear();
	GoalList.Clear();
	InputList.Clear();
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		Constant declarations
//
// Description:		Where constants for the GA_OBJECT class are declared.
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
const int GA_OBJECT::CurrentFileVersion = 0;

//==========================================================================
// Class:			GA_OBJECT
// Function:		SimulateGeneration
//
// Description:		Overriden method from GENETIC_ALGORITHM that allows the
//					use of threading.
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
void GA_OBJECT::SimulateGeneration(void)
{
	// Set the "optimization is running" flag
	IsRunning = true;

	// Start a bunch of jobs to evaluate all the different genomes at each different input condition
	// Use a counter to keep track of the number of pending jobs
	KinematicsData *Data;

	// Initialize the semaphore
	inverseSemaphore.Set(NumberOfCars);

	int i, j, temp(Optimization.GetIndex());
	for (i = 0; i < populationSize; i++)
	{
		for (j = 0; j < InputList.GetCount(); j++)
		{
			// Set up the car according to the current genome
			SetCarGenome(i * InputList.GetCount() + j, genomes[currentGeneration][i]);

			// Create the data and the job to send to the thread pool
			Data = new KinematicsData(OriginalCarArray[i * InputList.GetCount() + j],
				WorkingCarArray[i * InputList.GetCount() + j], *InputList.GetObject(j),
				KinematicOutputArray + i * InputList.GetCount() + j);
			ThreadJob NewJob(ThreadJob::CommandThreadKinematicsGA, Data,
				Optimization.GetCleanName(), temp);
			MainFrame.AddJob(NewJob);
		}
	}

	// Wait until the analyses are complete
	inverseSemaphore.Wait();

	// Determine fitnesses for every genome we just simulated
	for (i = 0; i < populationSize; i++)
		fitnesses[currentGeneration][i] = DetermineFitness(&i);

	return;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		DetermineFitness
//
// Description:		Fitness function for the genetic algorithm.  This is slightly
//					different from the original intended usage of this method
//					due to the use of threads.  This will not need to call the
//					analysis methods, as at this point, all analyses are complete
//					and the data is available for use in the fitness functions.
//
// Input Arguments:
//		CurrentGenome	= const int* pointing to the index for the genome we're analyzing
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
double GA_OBJECT::DetermineFitness(const int *Citizen)
{
	// Here, instead of actually pointing to the current genome, the argument points
	// to the index representing the current citizen.  Use Citizen[0] to get the
	// index.

	// Get the outputs of interest from each analysis that was conducted
	int i, j, k;
	double Fitness(0.0);
	for (i = 0; i < InputList.GetCount(); i++)
	{
		// Go through all the goals
		for (j = 0; j < GoalList.GetCount(); j++)
		{
			// Check to see if the current goal's input matches the current input list item
			if (GoalList[j]->BeforeInputs == *(InputList[i]))
			{
				// Check to see if the current goal is a delta between two inputs, or if we
				// only need to consider the current input
				if (GoalList[j]->BeforeInputs == GoalList[i]->AfterInputs)
				{
					// If the output is undefined, make it a really big number (ruin the fitness)
					if (VVASEMath::IsNaN(KinematicOutputArray[
						Citizen[0] * InputList.GetCount() + i].GetOutputValue(GoalList[j]->Output)))
						Fitness += 1e10;
					else
						// The output is not undefined.  Add to the fitness according to the
						// following formula:
						// Fitness (for each goal) = fabs(DesiredValue - ActualValue) * Importance / ExpectedDeviation
						// Total fitness is the sum of the individual fitnesses
						Fitness += fabs(GoalList[j]->DesiredValue - KinematicOutputArray[
							Citizen[0] * InputList.GetCount() + i].GetOutputValue(GoalList[j]->Output))
							* GoalList[j]->Importance / GoalList[j]->ExpectedDeviation;
				}
				else// This goal requires examining the difference between two sets of inputs
				{
					// Determine the index for the second set of inputs
					for (k = 0; k < InputList.GetCount(); k++)
					{
						if (GoalList[j]->AfterInputs == *(InputList[k]))
							break;
					}

					// Proceed just as we did for the single input case:
					// If the output is undefined, make it a really big number (ruin the fitness)
					if (VVASEMath::IsNaN(KinematicOutputArray[
						Citizen[0] * InputList.GetCount() + k].GetOutputValue(GoalList[j]->Output)))
						Fitness += 1e10;
					else
						// The output is not undefined.  Add to the fitness according to the
						// following formula:
						// Fitness (for each goal) = fabs(DesiredValue - ChangeInActualValue)
						//     * Importance / ExpectedDeviation
						// Total fitness is the sum of the individual fitnesses
						Fitness += fabs(GoalList[j]->DesiredValue - (KinematicOutputArray[
							Citizen[0] * InputList.GetCount() + k].GetOutputValue(GoalList[j]->Output) - KinematicOutputArray[
							Citizen[0] * InputList.GetCount() + i].GetOutputValue(GoalList[j]->Output)))
							* GoalList[j]->Importance / GoalList[j]->ExpectedDeviation;
				}
			}
		}
	}

	return Fitness;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		SetUp
//
// Description:		Calls the parent class's initialization routine.
//
// Input Arguments:
//		_TargetCar	= CAR* pointing to the object to optimize
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GA_OBJECT::SetUp(CAR *_TargetCar)
{
	// Ensure exclusive access to this object
	gsaMutex.Lock();

	// Initialize the run
	int *PhenotypeSizes = new int[GeneList.GetCount()];
	int i;
	for (i = 0; i < GeneList.GetCount(); i++)
		PhenotypeSizes[i] = GeneList.GetObject(i)->NumberOfValues;

	// Store the target car pointer
	assert(_TargetCar);
	TargetCar = _TargetCar;

	// Release the mutex
	gsaMutex.Unlock();

	// Initialize the algorithm
	InitializeAlgorithm(populationSize, generationLimit, GeneList.GetCount(), PhenotypeSizes, true,
		crossover, elitism, mutation);

	// Ensure exlcusive access to this object (again)
	wxMutexLocker lock(gsaMutex);

	// Delete the dynamically allocated memory
	delete [] PhenotypeSizes;

	// Clear and re-fill the list of input conditions
	DetermineAllInputs();

	// Re-allocate the array of working cars
	for (i = 0; i < NumberOfCars; i++)
	{
		delete OriginalCarArray[i];
		delete WorkingCarArray[i];
	}
	delete [] OriginalCarArray;
	delete [] WorkingCarArray;
	delete [] KinematicOutputArray;

	NumberOfCars = populationSize * InputList.GetCount();

	KinematicOutputArray = new KinematicOutputs[NumberOfCars];
	OriginalCarArray = new CAR*[NumberOfCars];
	WorkingCarArray = new CAR*[NumberOfCars];
	for (i = 0; i < NumberOfCars; i++)
	{
		OriginalCarArray[i] = new CAR(debugger);
		WorkingCarArray[i] = new CAR(debugger);
	}

	return;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		SetCarGenome
//
// Description:		Updates the original car so it represents the specified genome.
//
// Input Arguments:
//		CarIndex		= int specifying the index to use for the car arrays
//		CurrentGenome	= const int* pointing to the genome describing the desired car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GA_OBJECT::SetCarGenome(int CarIndex, const int *CurrentGenome)
{
	int i;

	// Change the appropriate values to make this new car match the CurrentGenome
	CORNER *CurrentCorner;
	CORNER *OppositeCorner;

	// Get locks on all the cars we're manipulating
	wxMutexLocker TargetLocker(TargetCar->GetMutex());
	wxMutexLocker OriginalLocker(OriginalCarArray[CarIndex]->GetMutex());

	// Set the original and working cars equal to the target car
	*OriginalCarArray[CarIndex] = *TargetCar;

	// Go through all of the genes, and adjust the variables to match the current genome
	GENE *CurrentGene;
	for (i = 0; i < GeneList.GetCount(); i++)
	{
		// Get the current gene
		CurrentGene = GeneList.GetObject(i);

		// Set the current and opposite corners
		if (CurrentGene->Location == CORNER::LocationLeftFront)
		{
			CurrentCorner = &OriginalCarArray[CarIndex]->Suspension->LeftFront;
			OppositeCorner = &OriginalCarArray[CarIndex]->Suspension->RightFront;
		}
		else if (CurrentGene->Location == CORNER::LocationRightFront)
		{
			CurrentCorner = &OriginalCarArray[CarIndex]->Suspension->RightFront;
			OppositeCorner = &OriginalCarArray[CarIndex]->Suspension->LeftFront;
		}
		else if (CurrentGene->Location == CORNER::LocationLeftRear)
		{
			CurrentCorner = &OriginalCarArray[CarIndex]->Suspension->LeftRear;
			OppositeCorner = &OriginalCarArray[CarIndex]->Suspension->RightRear;
		}
		else
		{
			CurrentCorner = &OriginalCarArray[CarIndex]->Suspension->RightRear;
			OppositeCorner = &OriginalCarArray[CarIndex]->Suspension->LeftRear;
		}

		// Determine which component of the vector to vary
		if (CurrentGene->Direction == Vector::AxisX)
		{
			// Set the appropriate variable to the value that corresponds to this phenotype
			CurrentCorner->Hardpoints[CurrentGene->Hardpoint].x = CurrentGene->Minimum +
				double(CurrentGene->NumberOfValues - CurrentGenome[i] - 1)
				* (CurrentGene->Maximum - CurrentGene->Minimum) / double(CurrentGene->NumberOfValues - 1);

			// If there is a tied-to variable specified, update that as well
			if (CurrentGene->TiedTo != CORNER::NumberOfHardpoints)
				CurrentCorner->Hardpoints[CurrentGene->TiedTo].x =
					CurrentCorner->Hardpoints[CurrentGene->Hardpoint].x;

			// If the suspension is symmetric, also update the point on the opposite corner
			if (TargetCar->Suspension->IsSymmetric)
			{
				// Copy the values from one side to the other
				OppositeCorner->Hardpoints[CurrentGene->Hardpoint].x =
					CurrentCorner->Hardpoints[CurrentGene->Hardpoint].x;

				// If there was a tied-to variable specified, we must update that on
				// the other side of the car, too
				if (CurrentGene->TiedTo != CORNER::NumberOfHardpoints)
					OppositeCorner->Hardpoints[CurrentGene->TiedTo].x =
						OppositeCorner->Hardpoints[CurrentGene->Hardpoint].x;
			}
		}
		else if (CurrentGene->Direction == Vector::AxisY)
		{
			// Set the appropriate variable to the value that corresponds to this phenotype
			CurrentCorner->Hardpoints[CurrentGene->Hardpoint].y = CurrentGene->Minimum +
				double(CurrentGene->NumberOfValues - CurrentGenome[i] - 1)
				* (CurrentGene->Maximum - CurrentGene->Minimum) / double(CurrentGene->NumberOfValues - 1);

			// If there is a tied-to variable specified, update that as well
			if (CurrentGene->TiedTo != CORNER::NumberOfHardpoints)
				CurrentCorner->Hardpoints[CurrentGene->TiedTo].y =
					CurrentCorner->Hardpoints[CurrentGene->Hardpoint].y;

			// If the suspension is symmetric, also update the point on the opposite corner
			if (TargetCar->Suspension->IsSymmetric)
			{
				// Copy the values from one side to the other (Note Y is flipped)
				OppositeCorner->Hardpoints[CurrentGene->Hardpoint].y =
					-CurrentCorner->Hardpoints[CurrentGene->Hardpoint].y;

				// If there was a tied-to variable specified, we must update that on
				// the other side of the car, too
				if (CurrentGene->TiedTo != CORNER::NumberOfHardpoints)
					OppositeCorner->Hardpoints[CurrentGene->TiedTo].y =
						OppositeCorner->Hardpoints[CurrentGene->Hardpoint].y;
			}
		}
		else// Vector::AxisZ
		{
			// Set the appropriate variable to the value that corresponds to this phenotype
			CurrentCorner->Hardpoints[CurrentGene->Hardpoint].z = CurrentGene->Minimum +
				double(CurrentGene->NumberOfValues - CurrentGenome[i] - 1)
				* (CurrentGene->Maximum - CurrentGene->Minimum) / double(CurrentGene->NumberOfValues - 1);

			// If there is a tied-to variable specified, update that as well
			if (CurrentGene->TiedTo != CORNER::NumberOfHardpoints)
				CurrentCorner->Hardpoints[CurrentGene->TiedTo].z =
					CurrentCorner->Hardpoints[CurrentGene->Hardpoint].z;

			// If the suspension is symmetric, also update the point on the opposite corner
			if (TargetCar->Suspension->IsSymmetric)
			{
				// Copy the values from one side to the other
				OppositeCorner->Hardpoints[CurrentGene->Hardpoint].z =
					CurrentCorner->Hardpoints[CurrentGene->Hardpoint].z;

				// If there was a tied-to variable specified, we must update that on
				// the other side of the car, too
				if (CurrentGene->TiedTo != CORNER::NumberOfHardpoints)
					OppositeCorner->Hardpoints[CurrentGene->TiedTo].z =
						OppositeCorner->Hardpoints[CurrentGene->Hardpoint].z;
			}
		}
	}

	return;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		PerformAdditionalActions
//
// Description:		Prints current status to the screen.
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
void GA_OBJECT::PerformAdditionalActions(void)
{
	// Compute average fitness for this generation
	double AverageFitness = 0.0;
	int i;
	for (i = 0; i < populationSize; i++)
		AverageFitness += fitnesses[currentGeneration][i];
	AverageFitness /= (double)populationSize;

	// Get maximum fitness for this generation
	double MaximumFitness = fitnesses[currentGeneration][0];

	// Display the average and best fitnesses
	debugger.Print(Debugger::PriorityVeryHigh, "Completed Generation %i", currentGeneration + 1);
	debugger.Print(Debugger::PriorityVeryHigh, "\tAverage Fitness:  %s", converter.FormatNumber(AverageFitness).c_str());
	debugger.Print(Debugger::PriorityVeryHigh, "\tBest Fitness:     %s", converter.FormatNumber(MaximumFitness).c_str());

	// Check to see if the simulation is still running
	if (currentGeneration == generationLimit - 1)
		IsRunning = false;

	return;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		AddGene
//
// Description:		Adds a gene to the list to be optimized.
//
// Input Arguments:
//		Variable		= const CORNER::HARDPOINTS& specifying to the value to be changed
//		TiedTo			= const CORNER::HARDPOINTS& specifying to a value that will always equal
//						  Variable
//		Location		= const CORNER::LOCATION& specifying the associated corner
//		Direction		= const Vector::Axis& specifying the component of the hardpoint
//						  to optimize
//		Minimum			= const double& minimum value for this gene
//		Maximum			= const double& maximum value for this gene
//		NumberOfValues	= const int& specifying the resolution for this gene (include the min and max,
//						  must be at least 2)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GA_OBJECT::AddGene(const CORNER::HARDPOINTS &Hardpoint, const CORNER::HARDPOINTS &TiedTo,
						const CORNER::LOCATION &Location, const Vector::Axis &Direction,
						const double &Minimum, const double &Maximum, const int &NumberOfValues)
{
	// Create a new GENE object
	GENE *NewGene = new GENE;

	// Copy the arguments to the new gene
	NewGene->Hardpoint		= Hardpoint;
	NewGene->TiedTo			= TiedTo;
	NewGene->Location		= Location;
	NewGene->Direction		= Direction;
	NewGene->Minimum		= Minimum;
	NewGene->Maximum		= Maximum;
	NewGene->NumberOfValues	= NumberOfValues;

	// Add the gene to the list
	GeneList.Add(NewGene);

	return;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		AddGoal
//
// Description:		Adds a goal to the list of optimization criteria.
//
// Input Arguments:
//		Output				= const KinematicOutputs::OutputsComplete& specifying
//							  the output to optimize
//		DesiredValue		= const double& specifying the desired value of the
//							  output
//		ExpectedDeviation	= const double& specifying the allowable range for
//							  the value
//		Importance			= const double& specifying the relative importance
//							  of this goal
//		BeforeInputs		= const Kinematics::Inputs& specifying the state
//							  of the car
//		AfterInputs			= const Kinematics::Inputs& specifying the second state
//							  of the car (if specified, the optimization is the
//							  difference between the output at the first and
//							  second state)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GA_OBJECT::AddGoal(const KinematicOutputs::OutputsComplete &Output, const double &DesiredValue,
		const double &ExpectedDeviation, const double &Importance, const Kinematics::Inputs &BeforeInputs,
		const Kinematics::Inputs &AfterInputs)
{
	// Create a new GOAL object
	GOAL *NewGoal = new GOAL;

	// Copy the arguments to the new goal
	NewGoal->Output				= Output;
	NewGoal->DesiredValue		= DesiredValue;
	NewGoal->ExpectedDeviation	= ExpectedDeviation;
	NewGoal->Importance			= Importance;
	NewGoal->BeforeInputs		= BeforeInputs;
	NewGoal->AfterInputs		= AfterInputs;

	// Add the goal to the list
	GoalList.Add(NewGoal);

	return;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		UpdateGene
//
// Description:		Updates the specified gene.
//
// Input Arguments:
//		Index			= const int& specifying the goal to update
//		Hardpoint		= const CORNER::HARDPOINTS& specifying to the value to be changed
//		TiedTo			= const CORNER::HARDPOINTS& specifying to a value that will always equal
//						  Variable
//		Location		= const CORNER::LOCATION& specifying the associated corner
//		Direction		= const Vector::Axis& specifying the component of the hardpoint
//						  to optimize
//		Minimum			= const double& minimum value for this gene
//		Maximum			= const double& maximum value for this gene
//		NumberOfValues	= const int& specifying the resolution for this gene (include the min and max,
//						  must be at least 2)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GA_OBJECT::UpdateGene(const int &Index, const CORNER::HARDPOINTS &Hardpoint, const CORNER::HARDPOINTS &TiedTo,
						const CORNER::LOCATION &Location, const Vector::Axis &Direction,
						const double &Minimum, const double &Maximum, const int &NumberOfValues)
{
	// Copy the arguments to the gene we're updating
	GeneList[Index]->Hardpoint		= Hardpoint;
	GeneList[Index]->TiedTo			= TiedTo;
	GeneList[Index]->Location		= Location;
	GeneList[Index]->Direction		= Direction;
	GeneList[Index]->Minimum		= Minimum;
	GeneList[Index]->Maximum		= Maximum;
	GeneList[Index]->NumberOfValues	= NumberOfValues;

	return;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		UpdateGoal
//
// Description:		Updates the specified goal.
//
// Input Arguments:
//		Index				= const int& specifying the goal to update
//		Output				= const KinematicOutputs::OutputsComplete& specifying
//							  the output to optimize
//		DesiredValue		= const double& specifying the desired value of the
//							  output
//		ExpectedDeviation	= const double& specifying the allowable range for
//							  the value
//		Importance			= const double& specifying the relative importance
//							  of this goal
//		BeforeInputs		= const Kinematics::Inputs& specifying the state
//							  of the car
//		AfterInputs			= const Kinematics::Inputs& specifying the second state
//							  of the car (if specified, the optimization is the
//							  difference between the output at the first and
//							  second state)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GA_OBJECT::UpdateGoal(const int &Index, const KinematicOutputs::OutputsComplete &Output, const double &DesiredValue,
		const double &ExpectedDeviation, const double &Importance, const Kinematics::Inputs &BeforeInputs,
		const Kinematics::Inputs &AfterInputs)
{
	// Copy the arguments to goal we're updating
	GoalList[Index]->Output				= Output;
	GoalList[Index]->DesiredValue		= DesiredValue;
	GoalList[Index]->ExpectedDeviation	= ExpectedDeviation;
	GoalList[Index]->Importance			= Importance;
	GoalList[Index]->BeforeInputs		= BeforeInputs;
	GoalList[Index]->AfterInputs		= AfterInputs;

	return;
}


//==========================================================================
// Class:			GA_OBJECT
// Function:		DetermineAllInputs
//
// Description:		Parses all of the available inputs and compiles a list of
//					each set of input conditions so every input is represented
//					exactly once.
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
void GA_OBJECT::DetermineAllInputs(void)
{
	// Clear out any existing inputs
	InputList.Clear();

	Kinematics::Inputs *Input = new Kinematics::Inputs;
	*Input = GoalList[0]->BeforeInputs;
	InputList.Add(Input);

	// Go through all of the goals
	int i, j;
	for (i = 0; i < GoalList.GetCount(); i++)
	{
		// Compare the before input with all of the inputs in the list
		j = 0;
		bool NeedsToBeAdded = true;
		while (j < InputList.GetCount())
		{
			if (GoalList[i]->BeforeInputs == *(InputList[j]))
				NeedsToBeAdded = false;

			j++;
		}

		if (NeedsToBeAdded)
		{
			// Since we're adding this to a managed list, we're allowed to reassign this pointer
			Input = new Kinematics::Inputs;
			*Input = GoalList[i]->BeforeInputs;
			InputList.Add(Input);
		}

		// If the before and after inputs are not the same, then check the after inputs, too
		if (GoalList[i]->BeforeInputs != GoalList[i]->AfterInputs)
		{
			// Compare the after input with all of the inputs in the list
			j = 0;
			NeedsToBeAdded = true;
			while (j < InputList.GetCount())
			{
				if (GoalList[i]->AfterInputs == *(InputList[j]))
					NeedsToBeAdded = false;

				j++;
			}

			if (NeedsToBeAdded)
			{
				// Since we're adding this to a managed list, we're allowed to reassign this pointer
				Input = new Kinematics::Inputs;
				*Input = GoalList[i]->AfterInputs;
				InputList.Add(Input);
			}
		}
	}

	return;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		UpdateTargetCar
//
// Description:		Updates the target car to match the best fit genome.
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
void GA_OBJECT::UpdateTargetCar(void)
{
	// Ensure exclusive access to this object
	wxMutexLocker lock(gsaMutex);

	// Update the target car to match the best fit car we've got
	*TargetCar = *OriginalCarArray[generationLimit - 1];

	return;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		WriteFileHeader
//
// Description:		Writes the file header to the specified output stream.
//
// Input Arguments:
//		OutFile	= std::ofstream* to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GA_OBJECT::WriteFileHeader(std::ofstream *OutFile)
{
	// Set up the header information
	FILE_HEADER_INFO Header;
	Header.FileVersion = CurrentFileVersion;

	// Set the write pointer to the start of the file
	OutFile->seekp(0);

	// Write the header
	OutFile->write((char*)&Header, sizeof(FILE_HEADER_INFO));

	return;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		ReadFileHeader
//
// Description:		Reads the file header information from the specified input stream.
//
// Input Arguments:
//		InFile	= std::ifstream* to read from
//
// Output Arguments:
//		None
//
// Return Value:
//		FILE_HEADER_INFO containing the header information
//
//==========================================================================
GA_OBJECT::FILE_HEADER_INFO GA_OBJECT::ReadFileHeader(std::ifstream *InFile)
{
	// Set get pointer to the start of the file
	InFile->seekg(0);

	// Read the header struct
	char Buffer[sizeof(FILE_HEADER_INFO)];
	InFile->read((char*)Buffer, sizeof(FILE_HEADER_INFO));

	return *reinterpret_cast<FILE_HEADER_INFO*>(Buffer);
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		Write
//
// Description:		Saves this object to file.
//
// Input Arguments:
//		FileName	= wxString containing the path and file name to write to.
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success
//
//==========================================================================
bool GA_OBJECT::Write(wxString FileName)
{
	// Ensure exclusive access to this object
	wxMutexLocker lock(gsaMutex);

	// Open the specified file
	std::ofstream OutFile(FileName.c_str(), ios::out | ios::binary);

	// Make sure the file was opened OK
	if (!OutFile.is_open() || !OutFile.good())
		return false;

	// Write the file header information
	WriteFileHeader(&OutFile);

	// Write this object's data
	OutFile.write((char*)&populationSize, sizeof(int));
	OutFile.write((char*)&generationLimit, sizeof(int));
	OutFile.write((char*)&elitism, sizeof(double));
	OutFile.write((char*)&mutation, sizeof(double));
	OutFile.write((char*)&crossover, sizeof(int));

	int i = GeneList.GetCount();
	OutFile.write((char*)&i, sizeof(int));
	for (i = 0; i < GeneList.GetCount(); i++)
		OutFile.write((char*)GeneList[i], sizeof(GENE));

	i = GoalList.GetCount();
	OutFile.write((char*)&i, sizeof(int));
	for (i = 0; i < GoalList.GetCount(); i++)
		OutFile.write((char*)GoalList[i], sizeof(GOAL));

	// Close the file
	OutFile.close();

	return true;
}

//==========================================================================
// Class:			GA_OBJECT
// Function:		Read
//
// Description:		Loads this object from file.
//
// Input Arguments:
//		FileName	= wxString containing path and file name to read from
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success
//
//==========================================================================
bool GA_OBJECT::Read(wxString FileName)
{
	// Ensure exclusive access to this object
	wxMutexLocker lock(gsaMutex);

	// Clear out current lists
	GeneList.Clear();
	GoalList.Clear();

	// Open the specified file
	std::ifstream InFile(FileName.c_str(), ios::in | ios::binary);

	// Make sure the file was opened OK
	if (!InFile.is_open() || !InFile.good())
		return false;

	// Read the file header information
	FILE_HEADER_INFO Header = ReadFileHeader(&InFile);

	// Check to make sure the version matches
	if (Header.FileVersion != CurrentFileVersion)
	{
		debugger.Print(_T("ERROR:  Incompatible file versions - could not open file!"));

		InFile.close();

		return false;
	}

	// Read this object's data
	InFile.read((char*)&populationSize, sizeof(int));
	InFile.read((char*)&generationLimit, sizeof(int));
	InFile.read((char*)&elitism, sizeof(double));
	InFile.read((char*)&mutation, sizeof(double));
	InFile.read((char*)&crossover, sizeof(int));

	int TempCount, i;
	GENE *TempGene;
	InFile.read((char*)&TempCount, sizeof(int));
	for (i = 0; i < TempCount; i++)
	{
		TempGene = new GENE;
		InFile.read((char*)TempGene, sizeof(GENE));
		GeneList.Add(TempGene);
	}

	GOAL *TempGoal;
	InFile.read((char*)&TempCount, sizeof(int));
	for (i = 0; i < TempCount; i++)
	{
		TempGoal = new GOAL;
		InFile.read((char*)TempGoal, sizeof(GOAL));
		GoalList.Add(TempGoal);
	}

	// Close the file
	InFile.close();

	return true;
}