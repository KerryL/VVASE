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
// Class:			GAObject
// Function:		GAObject
//
// Description:		Constructor for GAObject class.
//
// Input Arguments:
//		_mainFrame		= MainFrame& reference to the main application object
//		_optimization	= GeneticOptimization pointing to this object's owner
//		_converter		= const Convert& reference to application's conversion utility
//		_debugger		= const Debugger& reference to the debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GAObject::GAObject(MainFrame &_mainFrame, GeneticOptimization &_optimization, const Convert &_converter,
					 const Debugger &_debugger) : debugger(_debugger), converter(_converter),
					 mainFrame(_mainFrame), optimization(_optimization)
{
	// Initialize class members
	workingCarArray = NULL;
	originalCarArray = NULL;
	numberOfCars = 0;
	kinematicOutputArray = NULL;
	isRunning = false;
}

//==========================================================================
// Class:			GAObject
// Function:		~GAObject
//
// Description:		Destructor for GAObject class.
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
GAObject::~GAObject()
{
	wxMutexLocker lock(gsaMutex);

	// Delete this object's CAR members
	int i;
	for (i = 0; i < numberOfCars; i++)
	{
		delete originalCarArray[i];
		originalCarArray[i] = NULL;

		delete workingCarArray[i];
		workingCarArray[i] = NULL;
	}
	delete [] originalCarArray;
	originalCarArray = NULL;
	delete [] workingCarArray;
	workingCarArray = NULL;

	delete [] kinematicOutputArray;
	kinematicOutputArray = NULL;

	// Clean up the lists
	geneList.Clear();
	goalList.Clear();
	inputList.Clear();
}

//==========================================================================
// Class:			GAObject
// Function:		Constant declarations
//
// Description:		Where constants for the GAObject class are declared.
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
const int GAObject::currentFileVersion = 0;

//==========================================================================
// Class:			GAObject
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
void GAObject::SimulateGeneration(void)
{
	// Set the "optimization is running" flag
	isRunning = true;

	// Start a bunch of jobs to evaluate all the different genomes at each different input condition
	// Use a counter to keep track of the number of pending jobs
	KinematicsData *data;

	// Initialize the semaphore
	inverseSemaphore.Set(numberOfCars);

	int i, j, temp(optimization.GetIndex());
	for (i = 0; i < populationSize; i++)
	{
		for (j = 0; j < inputList.GetCount(); j++)
		{
			// Set up the car according to the current genome
			SetCarGenome(i * inputList.GetCount() + j, genomes[currentGeneration][i]);

			// Create the data and the job to send to the thread pool
			data = new KinematicsData(originalCarArray[i * inputList.GetCount() + j],
				workingCarArray[i * inputList.GetCount() + j], *inputList.GetObject(j),
				kinematicOutputArray + i * inputList.GetCount() + j);
			ThreadJob newJob(ThreadJob::CommandThreadKinematicsGA, data,
				optimization.GetCleanName(), temp);
			mainFrame.AddJob(newJob);
		}
	}

	// Wait until the analyses are complete
	inverseSemaphore.Wait();

	// Determine fitnesses for every genome we just simulated
	for (i = 0; i < populationSize; i++)
		fitnesses[currentGeneration][i] = DetermineFitness(&i);
}

//==========================================================================
// Class:			GAObject
// Function:		DetermineFitness
//
// Description:		Fitness function for the genetic algorithm.  This is slightly
//					different from the original intended usage of this method
//					due to the use of threads.  This will not need to call the
//					analysis methods, as at this point, all analyses are complete
//					and the data is available for use in the fitness functions.
//
// Input Arguments:
//		currentGenome	= const int* pointing to the index for the genome we're analyzing
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
double GAObject::DetermineFitness(const int *citizen)
{
	// Here, instead of actually pointing to the current genome, the argument points
	// to the index representing the current citizen.  Use Citizen[0] to get the
	// index.

	// Get the outputs of interest from each analysis that was conducted
	int i, j, k;
	double fitness(0.0);
	for (i = 0; i < inputList.GetCount(); i++)
	{
		// Go through all the goals
		for (j = 0; j < goalList.GetCount(); j++)
		{
			// Check to see if the current goal's input matches the current input list item
			if (goalList[j]->beforeInputs == *(inputList[i]))
			{
				// Check to see if the current goal is a delta between two inputs, or if we
				// only need to consider the current input
				if (goalList[j]->beforeInputs == goalList[i]->afterInputs)
				{
					// If the output is undefined, make it a really big number (ruin the fitness)
					if (VVASEMath::IsNaN(kinematicOutputArray[
						citizen[0] * inputList.GetCount() + i].GetOutputValue(goalList[j]->output)))
						fitness += 1e10;
					else
						// The output is not undefined.  Add to the fitness according to the
						// following formula:
						// Fitness (for each goal) = fabs(DesiredValue - ActualValue) * Importance / ExpectedDeviation
						// Total fitness is the sum of the individual fitnesses
						fitness += fabs(goalList[j]->desiredValue - kinematicOutputArray[
							citizen[0] * inputList.GetCount() + i].GetOutputValue(goalList[j]->output))
							* goalList[j]->importance / goalList[j]->expectedDeviation;
				}
				else// This goal requires examining the difference between two sets of inputs
				{
					// Determine the index for the second set of inputs
					for (k = 0; k < inputList.GetCount(); k++)
					{
						if (goalList[j]->afterInputs == *(inputList[k]))
							break;
					}

					// Proceed just as we did for the single input case:
					// If the output is undefined, make it a really big number (ruin the fitness)
					if (VVASEMath::IsNaN(kinematicOutputArray[
						citizen[0] * inputList.GetCount() + k].GetOutputValue(goalList[j]->output)))
						fitness += 1e10;
					else
						// The output is not undefined.  Add to the fitness according to the
						// following formula:
						// Fitness (for each goal) = fabs(DesiredValue - ChangeInActualValue)
						//     * Importance / ExpectedDeviation
						// Total fitness is the sum of the individual fitnesses
						fitness += fabs(goalList[j]->desiredValue - (kinematicOutputArray[
							citizen[0] * inputList.GetCount() + k].GetOutputValue(goalList[j]->output) - kinematicOutputArray[
							citizen[0] * inputList.GetCount() + i].GetOutputValue(goalList[j]->output)))
							* goalList[j]->importance / goalList[j]->expectedDeviation;
				}
			}
		}
	}

	return fitness;
}

//==========================================================================
// Class:			GAObject
// Function:		SetUp
//
// Description:		Calls the parent class's initialization routine.
//
// Input Arguments:
//		_targetCar	= Car* pointing to the object to optimize
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GAObject::SetUp(Car *_targetCar)
{
	// Ensure exclusive access to this object
	gsaMutex.Lock();

	// Initialize the run
	int *phenotypeSizes = new int[geneList.GetCount()];
	int i;
	for (i = 0; i < geneList.GetCount(); i++)
		phenotypeSizes[i] = geneList.GetObject(i)->numberOfValues;

	// Store the target car pointer
	assert(_targetCar);
	targetCar = _targetCar;

	// Release the mutex
	gsaMutex.Unlock();

	// Initialize the algorithm
	InitializeAlgorithm(populationSize, generationLimit, geneList.GetCount(),
		phenotypeSizes, true, crossover, elitism, mutation);

	// Ensure exclusive access to this object (again)
	wxMutexLocker lock(gsaMutex);

	// Delete the dynamically allocated memory
	delete [] phenotypeSizes;

	// Clear and re-fill the list of input conditions
	DetermineAllInputs();

	// Re-allocate the array of working cars
	for (i = 0; i < numberOfCars; i++)
	{
		delete originalCarArray[i];
		delete workingCarArray[i];
	}
	delete [] originalCarArray;
	delete [] workingCarArray;
	delete [] kinematicOutputArray;

	numberOfCars = populationSize * inputList.GetCount();

	kinematicOutputArray = new KinematicOutputs[numberOfCars];
	originalCarArray = new Car*[numberOfCars];
	workingCarArray = new Car*[numberOfCars];
	for (i = 0; i < numberOfCars; i++)
	{
		originalCarArray[i] = new Car(debugger);
		workingCarArray[i] = new Car(debugger);
	}
}

//==========================================================================
// Class:			GAObject
// Function:		SetCarGenome
//
// Description:		Updates the original car so it represents the specified genome.
//
// Input Arguments:
//		carIndex		= int specifying the index to use for the car arrays
//		currentGenome	= const int* pointing to the genome describing the desired car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GAObject::SetCarGenome(int carIndex, const int *currentGenome)
{
	int i;

	// Change the appropriate values to make this new car match the CurrentGenome
	Corner *currentCorner;
	Corner *oppositeCorner;

	// Get locks on all the cars we're manipulating
	wxMutexLocker targetLocker(targetCar->GetMutex());
	wxMutexLocker originalLocker(originalCarArray[carIndex]->GetMutex());

	// Set the original and working cars equal to the target car
	*originalCarArray[carIndex] = *targetCar;

	// Go through all of the genes, and adjust the variables to match the current genome
	Gene *currentGene;
	for (i = 0; i < geneList.GetCount(); i++)
	{
		// Get the current gene
		currentGene = geneList.GetObject(i);

		// Set the current and opposite corners
		if (currentGene->location == Corner::LocationLeftFront)
		{
			currentCorner = &originalCarArray[carIndex]->suspension->leftFront;
			oppositeCorner = &originalCarArray[carIndex]->suspension->rightFront;
		}
		else if (currentGene->location == Corner::LocationRightFront)
		{
			currentCorner = &originalCarArray[carIndex]->suspension->rightFront;
			oppositeCorner = &originalCarArray[carIndex]->suspension->leftFront;
		}
		else if (currentGene->location == Corner::LocationLeftRear)
		{
			currentCorner = &originalCarArray[carIndex]->suspension->leftRear;
			oppositeCorner = &originalCarArray[carIndex]->suspension->rightRear;
		}
		else
		{
			currentCorner = &originalCarArray[carIndex]->suspension->rightRear;
			oppositeCorner = &originalCarArray[carIndex]->suspension->leftRear;
		}

		// Determine which component of the vector to vary
		if (currentGene->direction == Vector::AxisX)
		{
			// Set the appropriate variable to the value that corresponds to this phenotype
			currentCorner->hardpoints[currentGene->hardpoint].x = currentGene->minimum +
				double(currentGene->numberOfValues - currentGenome[i] - 1)
				* (currentGene->maximum - currentGene->minimum) / double(currentGene->numberOfValues - 1);

			// If there is a tied-to variable specified, update that as well
			if (currentGene->tiedTo != Corner::NumberOfHardpoints)
				currentCorner->hardpoints[currentGene->tiedTo].x =
					currentCorner->hardpoints[currentGene->hardpoint].x;

			// If the suspension is symmetric, also update the point on the opposite corner
			if (targetCar->suspension->isSymmetric)
			{
				// Copy the values from one side to the other
				oppositeCorner->hardpoints[currentGene->hardpoint].x =
					currentCorner->hardpoints[currentGene->hardpoint].x;

				// If there was a tied-to variable specified, we must update that on
				// the other side of the car, too
				if (currentGene->tiedTo != Corner::NumberOfHardpoints)
					oppositeCorner->hardpoints[currentGene->tiedTo].x =
						oppositeCorner->hardpoints[currentGene->hardpoint].x;
			}
		}
		else if (currentGene->direction == Vector::AxisY)
		{
			// Set the appropriate variable to the value that corresponds to this phenotype
			currentCorner->hardpoints[currentGene->hardpoint].y = currentGene->minimum +
				double(currentGene->numberOfValues - currentGenome[i] - 1)
				* (currentGene->maximum - currentGene->minimum) / double(currentGene->numberOfValues - 1);

			// If there is a tied-to variable specified, update that as well
			if (currentGene->tiedTo != Corner::NumberOfHardpoints)
				currentCorner->hardpoints[currentGene->tiedTo].y =
					currentCorner->hardpoints[currentGene->hardpoint].y;

			// If the suspension is symmetric, also update the point on the opposite corner
			if (targetCar->suspension->isSymmetric)
			{
				// Copy the values from one side to the other (Note Y is flipped)
				oppositeCorner->hardpoints[currentGene->hardpoint].y =
					-currentCorner->hardpoints[currentGene->hardpoint].y;

				// If there was a tied-to variable specified, we must update that on
				// the other side of the car, too
				if (currentGene->tiedTo != Corner::NumberOfHardpoints)
					oppositeCorner->hardpoints[currentGene->tiedTo].y =
						oppositeCorner->hardpoints[currentGene->hardpoint].y;
			}
		}
		else// Vector::AxisZ
		{
			// Set the appropriate variable to the value that corresponds to this phenotype
			currentCorner->hardpoints[currentGene->hardpoint].z = currentGene->minimum +
				double(currentGene->numberOfValues - currentGenome[i] - 1)
				* (currentGene->maximum - currentGene->minimum) / double(currentGene->numberOfValues - 1);

			// If there is a tied-to variable specified, update that as well
			if (currentGene->tiedTo != Corner::NumberOfHardpoints)
				currentCorner->hardpoints[currentGene->tiedTo].z =
					currentCorner->hardpoints[currentGene->hardpoint].z;

			// If the suspension is symmetric, also update the point on the opposite corner
			if (targetCar->suspension->isSymmetric)
			{
				// Copy the values from one side to the other
				oppositeCorner->hardpoints[currentGene->hardpoint].z =
					currentCorner->hardpoints[currentGene->hardpoint].z;

				// If there was a tied-to variable specified, we must update that on
				// the other side of the car, too
				if (currentGene->tiedTo != Corner::NumberOfHardpoints)
					oppositeCorner->hardpoints[currentGene->tiedTo].z =
						oppositeCorner->hardpoints[currentGene->hardpoint].z;
			}
		}
	}
}

//==========================================================================
// Class:			GAObject
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
void GAObject::PerformAdditionalActions(void)
{
	// Compute average fitness for this generation
	double averageFitness = 0.0;
	int i;
	for (i = 0; i < populationSize; i++)
		averageFitness += fitnesses[currentGeneration][i];
	averageFitness /= (double)populationSize;

	// Get maximum fitness for this generation
	double maximumFitness = fitnesses[currentGeneration][0];

	// Display the average and best fitnesses
	debugger.Print(Debugger::PriorityVeryHigh, "Completed Generation %i", currentGeneration + 1);
	debugger.Print(Debugger::PriorityVeryHigh, "\tAverage Fitness:  %s", converter.FormatNumber(averageFitness).c_str());
	debugger.Print(Debugger::PriorityVeryHigh, "\tBest Fitness:     %s", converter.FormatNumber(maximumFitness).c_str());

	// Check to see if the simulation is still running
	if (currentGeneration == generationLimit - 1)
		isRunning = false;
}

//==========================================================================
// Class:			GAObject
// Function:		AddGene
//
// Description:		Adds a gene to the list to be optimized.
//
// Input Arguments:
//		variable		= const Corner::Hardpoints& specifying to the value to be changed
//		tiedTo			= const Corner::Hardpoints& specifying to a value that will always equal
//						  Variable
//		location		= const Corner::Location& specifying the associated corner
//		direction		= const Vector::Axis& specifying the component of the hardpoint
//						  to optimize
//		minimum			= const double& minimum value for this gene
//		maximum			= const double& maximum value for this gene
//		numberOfValues	= const int& specifying the resolution for this gene (include the min and max,
//						  must be at least 2)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GAObject::AddGene(const Corner::Hardpoints &hardpoint, const Corner::Hardpoints &tiedTo,
						const Corner::Location &location, const Vector::Axis &direction,
						const double &minimum, const double &maximum, const int &numberOfValues)
{
	// Create a new GENE object
	Gene *newGene = new Gene;

	// Copy the arguments to the new gene
	newGene->hardpoint		= hardpoint;
	newGene->tiedTo			= tiedTo;
	newGene->location		= location;
	newGene->direction		= direction;
	newGene->minimum		= minimum;
	newGene->maximum		= maximum;
	newGene->numberOfValues	= numberOfValues;

	// Add the gene to the list
	geneList.Add(newGene);
}

//==========================================================================
// Class:			GAObject
// Function:		AddGoal
//
// Description:		Adds a goal to the list of optimization criteria.
//
// Input Arguments:
//		output				= const KinematicOutputs::OutputsComplete& specifying
//							  the output to optimize
//		desiredValue		= const double& specifying the desired value of the
//							  output
//		expectedDeviation	= const double& specifying the allowable range for
//							  the value
//		importance			= const double& specifying the relative importance
//							  of this goal
//		beforeInputs		= const Kinematics::Inputs& specifying the state
//							  of the car
//		afterInputs			= const Kinematics::Inputs& specifying the second state
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
void GAObject::AddGoal(const KinematicOutputs::OutputsComplete &output, const double &desiredValue,
		const double &expectedDeviation, const double &importance, const Kinematics::Inputs &beforeInputs,
		const Kinematics::Inputs &afterInputs)
{
	// Create a new GOAL object
	Goal *newGoal = new Goal;

	// Copy the arguments to the new goal
	newGoal->output				= output;
	newGoal->desiredValue		= desiredValue;
	newGoal->expectedDeviation	= expectedDeviation;
	newGoal->importance			= importance;
	newGoal->beforeInputs		= beforeInputs;
	newGoal->afterInputs		= afterInputs;

	// Add the goal to the list
	goalList.Add(newGoal);
}

//==========================================================================
// Class:			GAObject
// Function:		UpdateGene
//
// Description:		Updates the specified gene.
//
// Input Arguments:
//		index			= const int& specifying the goal to update
//		hardpoint		= const Corner::Hardpoints& specifying to the value to be changed
//		tiedTo			= const Corner::Hardpoints& specifying to a value that will always equal
//						  Variable
//		location		= const Corner::Location& specifying the associated corner
//		direction		= const Vector::Axis& specifying the component of the hardpoint
//						  to optimize
//		minimum			= const double& minimum value for this gene
//		maximum			= const double& maximum value for this gene
//		numberOfValues	= const int& specifying the resolution for this gene (include the min and max,
//						  must be at least 2)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GAObject::UpdateGene(const int &index, const Corner::Hardpoints &hardpoint, const Corner::Hardpoints &tiedTo,
						const Corner::Location &location, const Vector::Axis &direction,
						const double &minimum, const double &maximum, const int &numberOfValues)
{
	// Copy the arguments to the gene we're updating
	geneList[index]->hardpoint		= hardpoint;
	geneList[index]->tiedTo			= tiedTo;
	geneList[index]->location		= location;
	geneList[index]->direction		= direction;
	geneList[index]->minimum		= minimum;
	geneList[index]->maximum		= maximum;
	geneList[index]->numberOfValues	= numberOfValues;
}

//==========================================================================
// Class:			GAObject
// Function:		UpdateGoal
//
// Description:		Updates the specified goal.
//
// Input Arguments:
//		index				= const int& specifying the goal to update
//		output				= const KinematicOutputs::OutputsComplete& specifying
//							  the output to optimize
//		desiredValue		= const double& specifying the desired value of the
//							  output
//		expectedDeviation	= const double& specifying the allowable range for
//							  the value
//		importance			= const double& specifying the relative importance
//							  of this goal
//		beforeInputs		= const Kinematics::Inputs& specifying the state
//							  of the car
//		afterInputs			= const Kinematics::Inputs& specifying the second state
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
void GAObject::UpdateGoal(const int &index, const KinematicOutputs::OutputsComplete &output, const double &desiredValue,
		const double &expectedDeviation, const double &importance, const Kinematics::Inputs &beforeInputs,
		const Kinematics::Inputs &afterInputs)
{
	// Copy the arguments to goal we're updating
	goalList[index]->output				= output;
	goalList[index]->desiredValue		= desiredValue;
	goalList[index]->expectedDeviation	= expectedDeviation;
	goalList[index]->importance			= importance;
	goalList[index]->beforeInputs		= beforeInputs;
	goalList[index]->afterInputs		= afterInputs;
}


//==========================================================================
// Class:			GAObject
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
void GAObject::DetermineAllInputs(void)
{
	// Clear out any existing inputs
	inputList.Clear();

	Kinematics::Inputs *input = new Kinematics::Inputs;
	*input = goalList[0]->beforeInputs;
	inputList.Add(input);

	// Go through all of the goals
	int i, j;
	for (i = 0; i < goalList.GetCount(); i++)
	{
		// Compare the before input with all of the inputs in the list
		j = 0;
		bool needsToBeAdded = true;
		while (j < inputList.GetCount())
		{
			if (goalList[i]->beforeInputs == *(inputList[j]))
				needsToBeAdded = false;

			j++;
		}

		if (needsToBeAdded)
		{
			// Since we're adding this to a managed list, we're allowed to reassign this pointer
			input = new Kinematics::Inputs;
			*input = goalList[i]->beforeInputs;
			inputList.Add(input);
		}

		// If the before and after inputs are not the same, then check the after inputs, too
		if (goalList[i]->beforeInputs != goalList[i]->afterInputs)
		{
			// Compare the after input with all of the inputs in the list
			j = 0;
			needsToBeAdded = true;
			while (j < inputList.GetCount())
			{
				if (goalList[i]->afterInputs == *(inputList[j]))
					needsToBeAdded = false;

				j++;
			}

			if (needsToBeAdded)
			{
				// Since we're adding this to a managed list, we're allowed to reassign this pointer
				input = new Kinematics::Inputs;
				*input = goalList[i]->afterInputs;
				inputList.Add(input);
			}
		}
	}
}

//==========================================================================
// Class:			GAObject
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
void GAObject::UpdateTargetCar(void)
{
	// Ensure exclusive access to this object
	wxMutexLocker lock(gsaMutex);

	// Update the target car to match the best fit car we've got
	*targetCar = *originalCarArray[generationLimit - 1];
}

//==========================================================================
// Class:			GAObject
// Function:		WriteFileHeader
//
// Description:		Writes the file header to the specified output stream.
//
// Input Arguments:
//		outFile	= std::ofstream* to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GAObject::WriteFileHeader(std::ofstream *outFile)
{
	// Set up the header information
	FileHeaderInfo header;
	header.fileVersion = currentFileVersion;

	// Set the write pointer to the start of the file
	outFile->seekp(0);

	// Write the header
	outFile->write((char*)&header, sizeof(FileHeaderInfo));
}

//==========================================================================
// Class:			GAObject
// Function:		ReadFileHeader
//
// Description:		Reads the file header information from the specified input stream.
//
// Input Arguments:
//		inFile	= std::ifstream* to read from
//
// Output Arguments:
//		None
//
// Return Value:
//		FileHeaderInfo containing the header information
//
//==========================================================================
GAObject::FileHeaderInfo GAObject::ReadFileHeader(std::ifstream *inFile)
{
	// Set get pointer to the start of the file
	inFile->seekg(0);

	// Read the header struct
	char buffer[sizeof(FileHeaderInfo)];
	inFile->read((char*)buffer, sizeof(FileHeaderInfo));

	return *reinterpret_cast<FileHeaderInfo*>(buffer);
}

//==========================================================================
// Class:			GAObject
// Function:		Write
//
// Description:		Saves this object to file.
//
// Input Arguments:
//		fileName	= wxString containing the path and file name to write to.
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success
//
//==========================================================================
bool GAObject::Write(wxString fileName)
{
	// Ensure exclusive access to this object
	wxMutexLocker lock(gsaMutex);

	// Open the specified file
	std::ofstream outFile(fileName.c_str(), ios::out | ios::binary);

	// Make sure the file was opened OK
	if (!outFile.is_open() || !outFile.good())
		return false;

	// Write the file header information
	WriteFileHeader(&outFile);

	// Write this object's data
	outFile.write((char*)&populationSize, sizeof(int));
	outFile.write((char*)&generationLimit, sizeof(int));
	outFile.write((char*)&elitism, sizeof(double));
	outFile.write((char*)&mutation, sizeof(double));
	outFile.write((char*)&crossover, sizeof(int));

	int i = geneList.GetCount();
	outFile.write((char*)&i, sizeof(int));
	for (i = 0; i < geneList.GetCount(); i++)
		outFile.write((char*)geneList[i], sizeof(Gene));

	i = goalList.GetCount();
	outFile.write((char*)&i, sizeof(int));
	for (i = 0; i < goalList.GetCount(); i++)
		outFile.write((char*)goalList[i], sizeof(Goal));

	// Close the file
	outFile.close();

	return true;
}

//==========================================================================
// Class:			GAObject
// Function:		Read
//
// Description:		Loads this object from file.
//
// Input Arguments:
//		fileName	= wxString containing path and file name to read from
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success
//
//==========================================================================
bool GAObject::Read(wxString fileName)
{
	// Ensure exclusive access to this object
	wxMutexLocker lock(gsaMutex);

	// Clear out current lists
	geneList.Clear();
	goalList.Clear();

	// Open the specified file
	std::ifstream inFile(fileName.c_str(), ios::in | ios::binary);

	// Make sure the file was opened OK
	if (!inFile.is_open() || !inFile.good())
		return false;

	// Read the file header information
	FileHeaderInfo header = ReadFileHeader(&inFile);

	// Check to make sure the version matches
	if (header.fileVersion != currentFileVersion)
	{
		debugger.Print(_T("ERROR:  Incompatible file versions - could not open file!"));

		inFile.close();

		return false;
	}

	// Read this object's data
	inFile.read((char*)&populationSize, sizeof(int));
	inFile.read((char*)&generationLimit, sizeof(int));
	inFile.read((char*)&elitism, sizeof(double));
	inFile.read((char*)&mutation, sizeof(double));
	inFile.read((char*)&crossover, sizeof(int));

	int tempCount, i;
	Gene *tempGene;
	inFile.read((char*)&tempCount, sizeof(int));
	for (i = 0; i < tempCount; i++)
	{
		tempGene = new Gene;
		inFile.read((char*)tempGene, sizeof(Gene));
		geneList.Add(tempGene);
	}

	Goal *tempGoal;
	inFile.read((char*)&tempCount, sizeof(int));
	for (i = 0; i < tempCount; i++)
	{
		tempGoal = new Goal;
		inFile.read((char*)tempGoal, sizeof(Goal));
		goalList.Add(tempGoal);
	}

	// Close the file
	inFile.close();

	return true;
}