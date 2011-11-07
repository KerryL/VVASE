/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  ga_object_class.cpp
// Created:  4/7/2009
// Author:  K. Loux
// Description:  This is a class for testing genetic algorithm implementation.
// History:

// Standard C++ headers
#include <fstream>

// CarDesigner headers
#include "gui/ga_object_class.h"
#include "gui/genetic_optimization_class.h"
#include "gui/components/main_frame_class.h"
#include "vSolver/physics/kinematics_class.h"
#include "vSolver/physics/kinematic_outputs_class.h"
#include "vSolver/threads/kinematics_data_class.h"
#include "vCar/car_class.h"
#include "vCar/suspension_class.h"
#include "vMath/car_math.h"
#include "vUtilities/convert_class.h"
#include "vUtilities/machine_defs.h"

//==========================================================================
// Class:			GA_OBJECT
// Function:		GA_OBJECT
//
// Description:		Constructor for GA_OBJECT class.
//
// Input Arguments:
//		_MainFrame		= MAIN_FRAME& reference to the main application object
//		_Optimization	= GENETIC_OPTIMIZATION pointing to this object's owner
//		_Converter		= const CONVERT& reference to application's conversion utility
//		_Debugger		= const DEBUGGER& reference to the debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GA_OBJECT::GA_OBJECT(MAIN_FRAME &_MainFrame, GENETIC_OPTIMIZATION &_Optimization, const CONVERT &_Converter,
					 const DEBUGGER &_Debugger) : Debugger(_Debugger), Converter(_Converter),
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
	wxMutexLocker Lock(GSAMutex);

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
// Description:		Where constants for the GA_OBJECT class are delcared.
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
	KINEMATICS_DATA *Data;

	// Initialize the semaphore
	InverseSemaphore.Set(NumberOfCars);

	int i, j, temp(Optimization.GetIndex());
	for (i = 0; i < PopulationSize; i++)
	{
		for (j = 0; j < InputList.GetCount(); j++)
		{
			// Set up the car according to the current genome
			SetCarGenome(i * InputList.GetCount() + j, Genomes[CurrentGeneration][i]);

			// Create the data and the job to send to the thread pool
			Data = new KINEMATICS_DATA(OriginalCarArray[i * InputList.GetCount() + j],
				WorkingCarArray[i * InputList.GetCount() + j], *InputList.GetObject(j),
				KinematicOutputArray + i * InputList.GetCount() + j);
			THREAD_JOB NewJob(THREAD_JOB::COMMAND_THREAD_KINEMATICS_FOR_GA, Data,
				Optimization.GetCleanName(), temp);
			MainFrame.AddJob(NewJob);
		}
	}

	// Wait until the analyses are complete
	InverseSemaphore.Wait();

	// Determine fitnesses for every genome we just simulated
	for (i = 0; i < PopulationSize; i++)
		Fitnesses[CurrentGeneration][i] = DetermineFitness(&i);

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
//					and the data is available for use in the fitness funcitons.
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
	// Here, instead of actually pointing to the current genome, the agrument points
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
	// Ensure exlcusive access to this object
	GSAMutex.Lock();

	// Initialize the run
	int *PhenotypeSizes = new int[GeneList.GetCount()];
	int i;
	for (i = 0; i < GeneList.GetCount(); i++)
		PhenotypeSizes[i] = GeneList.GetObject(i)->NumberOfValues;

	// Store the target car pointer
	assert(_TargetCar);
	TargetCar = _TargetCar;

	// Release the mutex
	GSAMutex.Unlock();

	// Initialize the algorithm
	InitializeAlgorithm(PopulationSize, GenerationLimit, GeneList.GetCount(), PhenotypeSizes, true,
		Crossover, Elitism, Mutation);

	// Ensure exlcusive access to this object (again)
	wxMutexLocker Lock(GSAMutex);

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

	NumberOfCars = PopulationSize * InputList.GetCount();

	KinematicOutputArray = new KINEMATIC_OUTPUTS[NumberOfCars];
	OriginalCarArray = new CAR*[NumberOfCars];
	WorkingCarArray = new CAR*[NumberOfCars];
	for (i = 0; i < NumberOfCars; i++)
	{
		OriginalCarArray[i] = new CAR(Debugger);
		WorkingCarArray[i] = new CAR(Debugger);
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
		if (CurrentGene->Direction == VECTOR::AxisX)
		{
			// Set the appropriate variable to the value that corresponds to this phenotype
			CurrentCorner->Hardpoints[CurrentGene->Hardpoint].X = CurrentGene->Minimum +
				double(CurrentGene->NumberOfValues - CurrentGenome[i] - 1)
				* (CurrentGene->Maximum - CurrentGene->Minimum) / double(CurrentGene->NumberOfValues - 1);

			// If there is a tied-to variable specified, update that as well
			if (CurrentGene->TiedTo != CORNER::NumberOfHardpoints)
				CurrentCorner->Hardpoints[CurrentGene->TiedTo].X =
					CurrentCorner->Hardpoints[CurrentGene->Hardpoint].X;

			// If the suspension is symmetric, also update the point on the opposite corner
			if (TargetCar->Suspension->IsSymmetric)
			{
				// Copy the values from one side to the other
				OppositeCorner->Hardpoints[CurrentGene->Hardpoint].X =
					CurrentCorner->Hardpoints[CurrentGene->Hardpoint].X;

				// If there was a tied-to variable specified, we must update that on
				// the other side of the car, too
				if (CurrentGene->TiedTo != CORNER::NumberOfHardpoints)
					OppositeCorner->Hardpoints[CurrentGene->TiedTo].X =
						OppositeCorner->Hardpoints[CurrentGene->Hardpoint].X;
			}
		}
		else if (CurrentGene->Direction == VECTOR::AxisY)
		{
			// Set the appropriate variable to the value that corresponds to this phenotype
			CurrentCorner->Hardpoints[CurrentGene->Hardpoint].Y = CurrentGene->Minimum +
				double(CurrentGene->NumberOfValues - CurrentGenome[i] - 1)
				* (CurrentGene->Maximum - CurrentGene->Minimum) / double(CurrentGene->NumberOfValues - 1);

			// If there is a tied-to variable specified, update that as well
			if (CurrentGene->TiedTo != CORNER::NumberOfHardpoints)
				CurrentCorner->Hardpoints[CurrentGene->TiedTo].Y =
					CurrentCorner->Hardpoints[CurrentGene->Hardpoint].Y;

			// If the suspension is symmetric, also update the point on the opposite corner
			if (TargetCar->Suspension->IsSymmetric)
			{
				// Copy the values from one side to the other (Note Y is flipped)
				OppositeCorner->Hardpoints[CurrentGene->Hardpoint].Y =
					-CurrentCorner->Hardpoints[CurrentGene->Hardpoint].Y;

				// If there was a tied-to variable specified, we must update that on
				// the other side of the car, too
				if (CurrentGene->TiedTo != CORNER::NumberOfHardpoints)
					OppositeCorner->Hardpoints[CurrentGene->TiedTo].Y =
						OppositeCorner->Hardpoints[CurrentGene->Hardpoint].Y;
			}
		}
		else// VECTOR::AxisZ
		{
			// Set the appropriate variable to the value that corresponds to this phenotype
			CurrentCorner->Hardpoints[CurrentGene->Hardpoint].Z = CurrentGene->Minimum +
				double(CurrentGene->NumberOfValues - CurrentGenome[i] - 1)
				* (CurrentGene->Maximum - CurrentGene->Minimum) / double(CurrentGene->NumberOfValues - 1);

			// If there is a tied-to variable specified, update that as well
			if (CurrentGene->TiedTo != CORNER::NumberOfHardpoints)
				CurrentCorner->Hardpoints[CurrentGene->TiedTo].Z =
					CurrentCorner->Hardpoints[CurrentGene->Hardpoint].Z;

			// If the suspension is symmetric, also update the point on the opposite corner
			if (TargetCar->Suspension->IsSymmetric)
			{
				// Copy the values from one side to the other
				OppositeCorner->Hardpoints[CurrentGene->Hardpoint].Z =
					CurrentCorner->Hardpoints[CurrentGene->Hardpoint].Z;

				// If there was a tied-to variable specified, we must update that on
				// the other side of the car, too
				if (CurrentGene->TiedTo != CORNER::NumberOfHardpoints)
					OppositeCorner->Hardpoints[CurrentGene->TiedTo].Z =
						OppositeCorner->Hardpoints[CurrentGene->Hardpoint].Z;
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
	for (i = 0; i < PopulationSize; i++)
		AverageFitness += Fitnesses[CurrentGeneration][i];
	AverageFitness /= (float)PopulationSize;

	// Get maximum fitness for this generation
	double MaximumFitness = Fitnesses[CurrentGeneration][0];

	// Dispaly the average and best fitnesses
	Debugger.Print(DEBUGGER::PriorityVeryHigh, "Completed Generation %i", CurrentGeneration + 1);
	Debugger.Print(DEBUGGER::PriorityVeryHigh, "\tAverage Fitness:  %s", Converter.FormatNumber(AverageFitness).c_str());
	Debugger.Print(DEBUGGER::PriorityVeryHigh, "\tBest Fitness:     %s", Converter.FormatNumber(MaximumFitness).c_str());

	// Check to see if the simulation is still running
	if (CurrentGeneration == GenerationLimit - 1)
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
//		TiedTo			= const CORNER::HARDPOINTS& specifying to a valuethat will always equal
//						  Variable
//		Location		= const CORNER::LOCATION& specifying the associated corner
//		Direction		= const VECTOR::AXIS& specifying the component of the hardpoint
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
						const CORNER::LOCATION &Location, const VECTOR::AXIS &Direction,
						const double &Minimum, const double &Maximum, const int &NumberOfValues)
{
	// Create a new GENE object
	GENE *NewGene = new GENE;

	// Copy the arguements to the new gene
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
//		Output				= const KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE& specifying
//							  the output to optimize
//		DesiredValue		= const double& specifying the desired value of the
//							  output
//		ExpectedDeviation	= const double& specifying the allowable range for
//							  the value
//		Importance			= const double& specifying the relative imporance
//							  of this goal
//		BeforeInputs		= const KINEMATICS::INPUTS& specifying the state
//							  of the car
//		AfterInputs			= const KINEMATICS::INPUTS& specifying the second state
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
void GA_OBJECT::AddGoal(const KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE &Output, const double &DesiredValue,
		const double &ExpectedDeviation, const double &Importance, const KINEMATICS::INPUTS &BeforeInputs,
		const KINEMATICS::INPUTS &AfterInputs)
{
	// Create a new GOAL object
	GOAL *NewGoal = new GOAL;

	// Copy the arguements to the new goal
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
//		TiedTo			= const CORNER::HARDPOINTS& specifying to a valuethat will always equal
//						  Variable
//		Location		= const CORNER::LOCATION& specifying the associated corner
//		Direction		= const VECTOR::AXIS& specifying the component of the hardpoint
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
						const CORNER::LOCATION &Location, const VECTOR::AXIS &Direction,
						const double &Minimum, const double &Maximum, const int &NumberOfValues)
{
	// Copy the arguements to the gene we're updating
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
//		Output				= const KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE& specifying
//							  the output to optimize
//		DesiredValue		= const double& specifying the desired value of the
//							  output
//		ExpectedDeviation	= const double& specifying the allowable range for
//							  the value
//		Importance			= const double& specifying the relative imporance
//							  of this goal
//		BeforeInputs		= const KINEMATICS::INPUTS& specifying the state
//							  of the car
//		AfterInputs			= const KINEMATICS::INPUTS& specifying the second state
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
void GA_OBJECT::UpdateGoal(const int &Index, const KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE &Output, const double &DesiredValue,
		const double &ExpectedDeviation, const double &Importance, const KINEMATICS::INPUTS &BeforeInputs,
		const KINEMATICS::INPUTS &AfterInputs)
{
	// Copy the arguements to goal we're updating
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

	KINEMATICS::INPUTS *Input = new KINEMATICS::INPUTS;
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
			Input = new KINEMATICS::INPUTS;
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
				Input = new KINEMATICS::INPUTS;
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
	// Ensure exlcusive access to this object
	wxMutexLocker Lock(GSAMutex);

	// Update the target car to match the best fit car we've got
	*TargetCar = *OriginalCarArray[GenerationLimit - 1];

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
	// Ensure exlcusive access to this object
	wxMutexLocker Lock(GSAMutex);

	// Open the specified file
	std::ofstream OutFile(FileName.c_str(), ios::out | ios::binary);

	// Make sure the file was opened OK
	if (!OutFile.is_open() || !OutFile.good())
		return false;

	// Write the file header information
	WriteFileHeader(&OutFile);

	// Write this object's data
	OutFile.write((char*)&PopulationSize, sizeof(int));
	OutFile.write((char*)&GenerationLimit, sizeof(int));
	OutFile.write((char*)&Elitism, sizeof(double));
	OutFile.write((char*)&Mutation, sizeof(double));
	OutFile.write((char*)&Crossover, sizeof(int));

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
	// Ensure exlcusive access to this object
	wxMutexLocker Lock(GSAMutex);

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
		Debugger.Print(_T("ERROR:  Incompatable file versions - could not open file!"));

		InFile.close();

		return false;
	}

	// Read this object's data
	InFile.read((char*)&PopulationSize, sizeof(int));
	InFile.read((char*)&GenerationLimit, sizeof(int));
	InFile.read((char*)&Elitism, sizeof(double));
	InFile.read((char*)&Mutation, sizeof(double));
	InFile.read((char*)&Crossover, sizeof(int));

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