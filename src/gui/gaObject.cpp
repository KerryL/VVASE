/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  gaObject.cpp
// Date:  4/7/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  This is a class for testing genetic algorithm implementation.

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "gaObject.h"
#include "VVASE/gui/geneticOptimization.h"
#include "VVASE/core/analysis/kinematics.h"
#include "VVASE/core/analysis/kinematicOutputs.h"
#include "VVASE/core/threads/kinematicsData.h"
#include "VVASE/core/threads/threadEvent.h"
#include "VVASE/core/car/car.h"
#include "VVASE/core/car/subsystems/suspension.h"
#include "VVASE/core/utilities/carMath.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/debugger.h"
#include "VVASE/core/threads/jobQueue.h"

namespace VVASE
{

//==========================================================================
// Class:			GAObject
// Function:		GAObject
//
// Description:		Constructor for GAObject class.
//
// Input Arguments:
//		queue			= JobQueue& reference to the job queue
//		optimization	= GeneticOptimization pointing to this object's owner
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GAObject::GAObject(JobQueue &queue, GeneticOptimization &optimization)
	: queue(queue), optimization(optimization)
{
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
	DebugLog::GetInstance()->Log(_T("GAObject::~GAObject()"));

	unsigned int i;
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
	geneList.clear();
	goalList.clear();
	inputList.clear();
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
// Description:		Overriden method from GeneticAlgorithm that allows the
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
void GAObject::SimulateGeneration()
{
	isRunning = true;

	// Start a bunch of jobs to evaluate all the different genomes at each different input condition
	// Use a counter to keep track of the number of pending jobs
	KinematicsData *data;

	//inverseSemaphore.Set(numberOfCars);// TODO:  Check return value to ensure no errors!
	// TODO:  Re-implement

	int i, temp(optimization.GetIndex());
	unsigned int j;
	for (i = 0; i < populationSize; i++)
	{
		for (j = 0; j < inputList.size(); j++)
		{
			SetCarGenome(i * inputList.size() + j, genomes[currentGeneration][i]);

			data = new KinematicsData(originalCarArray[i * inputList.size() + j],
				workingCarArray[i * inputList.size() + j], inputList[j],
				kinematicOutputArray + i * inputList.size() + j);
			ThreadJob newJob(ThreadJob::CommandThreadKinematicsGA, data,
				optimization.GetCleanName(), temp);
			queue.AddJob(newJob);
		}
	}

	//inverseSemaphore.Wait();
	// TODO:  Re-implement

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
	unsigned int i, j, k;
	double fitness(0.0);
	for (i = 0; i < inputList.size(); i++)
	{
		// Go through all the goals
		for (j = 0; j < goalList.size(); j++)
		{
			// Check to see if the current goal's input matches the current input list item
			if (goalList[j].beforeInputs == inputList[i])
			{
				// Check to see if the current goal is a delta between two inputs, or if we
				// only need to consider the current input
				if (goalList[j].beforeInputs == goalList[i].afterInputs)
				{
					// If the output is undefined, make it a really big number (ruin the fitness)
					if (Math::IsNaN(kinematicOutputArray[
						citizen[0] * inputList.size() + i].GetOutputValue(goalList[j].output)))
						fitness += 1e10;
					else
						// The output is not undefined.  Add to the fitness according to the
						// following formula:
						// Fitness (for each goal) = fabs(DesiredValue - ActualValue) * Importance / ExpectedDeviation
						// Total fitness is the sum of the individual fitnesses
						fitness += fabs(goalList[j].desiredValue - kinematicOutputArray[
							citizen[0] * inputList.size() + i].GetOutputValue(goalList[j].output))
							* goalList[j].importance / goalList[j].expectedDeviation;
				}
				else// This goal requires examining the difference between two sets of inputs
				{
					// Determine the index for the second set of inputs
					for (k = 0; k < inputList.size(); k++)
					{
						if (goalList[j].afterInputs == inputList[k])
							break;
					}

					// Proceed just as we did for the single input case:
					// If the output is undefined, make it a really big number (ruin the fitness)
					if (Math::IsNaN(kinematicOutputArray[
						citizen[0] * inputList.size() + k].GetOutputValue(goalList[j].output)))
						fitness += 1e10;
					else
						// The output is not undefined.  Add to the fitness according to the
						// following formula:
						// Fitness (for each goal) = fabs(DesiredValue - ChangeInActualValue)
						//     * Importance / ExpectedDeviation
						// Total fitness is the sum of the individual fitnesses
						fitness += fabs(goalList[j].desiredValue - (kinematicOutputArray[
							citizen[0] * inputList.size() + k].GetOutputValue(goalList[j].output) - kinematicOutputArray[
							citizen[0] * inputList.size() + i].GetOutputValue(goalList[j].output)))
							* goalList[j].importance / goalList[j].expectedDeviation;
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
//		targetCar	= const Car& pointing to the object to optimize
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GAObject::SetUp(const Car &targetCar)
{
	gsaMutex.Lock();
	DebugLog::GetInstance()->Log(_T("GAObject::SetUp (lock)"));

	std::vector<int> phenotypeSizes(geneList.size());
	unsigned int i;
	for (i = 0; i < geneList.size(); i++)
		phenotypeSizes[i] = geneList[i].numberOfValues;

	this->targetCar = &targetCar;

	DebugLog::GetInstance()->Log(_T("GAObject::SetUp (unlock)"));
	gsaMutex.Unlock();

	const int phenotypeCount(phenotypeSizes.size());
	InitializeAlgorithm(populationSize, generationLimit, geneList.size(),
		&phenotypeCount, true, crossover, elitism, mutation);

	wxMutexLocker lock(gsaMutex);
	DebugLog::GetInstance()->Log(_T("GAObject::SetUp (locker)"));

	DetermineAllInputs();

	for (i = 0; i < numberOfCars; i++)
	{
		delete originalCarArray[i];
		delete workingCarArray[i];
	}
	delete [] originalCarArray;
	delete [] workingCarArray;
	delete [] kinematicOutputArray;

	numberOfCars = populationSize * inputList.size();

	kinematicOutputArray = new KinematicOutputs[numberOfCars];
	originalCarArray = new Car*[numberOfCars];
	workingCarArray = new Car*[numberOfCars];
	for (i = 0; i < numberOfCars; i++)
	{
		originalCarArray[i] = new Car();
		workingCarArray[i] = new Car();
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
	// Change the appropriate values to make this new car match the CurrentGenome
	Corner *currentCorner;
	Corner *oppositeCorner;

	// Get locks on all the cars we're manipulating
	// NOTE:  Always lock working car first, then lock original car (consistency required to prevent deadlocks)
	wxMutexLocker targetLocker(targetCar->GetMutex());
	DebugLog::GetInstance()->Log(_T("GAObject::SetCarGenome (targetLocker)"));
	wxMutexLocker originalLocker(originalCarArray[carIndex]->GetMutex());
	DebugLog::GetInstance()->Log(_T("GAObject::SetCarGenome (originalLocker)"));

	// Set the original and working cars equal to the target car
	*originalCarArray[carIndex] = *targetCar;

	// Go through all of the genes, and adjust the variables to match the current genome
	Gene *currentGene;
	unsigned int i;
	Suspension* currentSuspension(dynamic_cast<Suspension*>(originalCarArray[carIndex]->GetSubsystem(Suspension::GetName()))));
	Suspension* targetSuspension(dynamic_cast<Suspension*>(targetCar->GetSubsystem(Suspension::GetName())));
	for (i = 0; i < geneList.size(); i++)
	{
		// Get the current gene
		currentGene = &geneList[i];

		// Set the current and opposite corners
		if (currentGene->location == Corner::LocationLeftFront)
		{
			currentCorner = &suspension->leftFront;
			oppositeCorner = &suspension->rightFront;
		}
		else if (currentGene->location == Corner::LocationRightFront)
		{
			currentCorner = &suspension->rightFront;
			oppositeCorner = &suspension->leftFront;
		}
		else if (currentGene->location == Corner::LocationLeftRear)
		{
			currentCorner = &suspension->leftRear;
			oppositeCorner = &suspension->rightRear;
		}
		else
		{
			currentCorner = &suspension->rightRear;
			oppositeCorner = &suspension->leftRear;
		}

		// Determine which component of the vector to vary
		if (currentGene->direction == Math::Axis::X)
		{
			// Set the appropriate variable to the value that corresponds to this phenotype
			currentCorner->hardpoints[currentGene->hardpoint].x() = currentGene->minimum +
				double(currentGene->numberOfValues - currentGenome[i] - 1)
				* (currentGene->maximum - currentGene->minimum) / double(currentGene->numberOfValues - 1);

			// If there is a tied-to variable specified, update that as well
			if (currentGene->tiedTo != Corner::NumberOfHardpoints)
				currentCorner->hardpoints[currentGene->tiedTo].x() =
					currentCorner->hardpoints[currentGene->hardpoint].x();

			// If the suspension is symmetric, also update the point on the opposite corner
			if (targetSuspension->isSymmetric)
			{
				// Copy the values from one side to the other
				oppositeCorner->hardpoints[currentGene->hardpoint].x() =
					currentCorner->hardpoints[currentGene->hardpoint].x();

				// If there was a tied-to variable specified, we must update that on
				// the other side of the car, too
				if (currentGene->tiedTo != Corner::NumberOfHardpoints)
					oppositeCorner->hardpoints[currentGene->tiedTo].x() =
						oppositeCorner->hardpoints[currentGene->hardpoint].x();
			}
		}
		else if (currentGene->direction == Math::Axis::Y)
		{
			// Set the appropriate variable to the value that corresponds to this phenotype
			currentCorner->hardpoints[currentGene->hardpoint].y() = currentGene->minimum +
				double(currentGene->numberOfValues - currentGenome[i] - 1)
				* (currentGene->maximum - currentGene->minimum) / double(currentGene->numberOfValues - 1);

			// If there is a tied-to variable specified, update that as well
			if (currentGene->tiedTo != Corner::NumberOfHardpoints)
				currentCorner->hardpoints[currentGene->tiedTo].y() =
					currentCorner->hardpoints[currentGene->hardpoint].y();

			// If the suspension is symmetric, also update the point on the opposite corner
			if (targetSuspension->isSymmetric)
			{
				// Copy the values from one side to the other (Note Y is flipped)
				oppositeCorner->hardpoints[currentGene->hardpoint].y() =
					-currentCorner->hardpoints[currentGene->hardpoint].y();

				// If there was a tied-to variable specified, we must update that on
				// the other side of the car, too
				if (currentGene->tiedTo != Corner::NumberOfHardpoints)
					oppositeCorner->hardpoints[currentGene->tiedTo].y() =
						oppositeCorner->hardpoints[currentGene->hardpoint].y();
			}
		}
		else// Math::Axis::Z
		{
			// Set the appropriate variable to the value that corresponds to this phenotype
			currentCorner->hardpoints[currentGene->hardpoint].z() = currentGene->minimum +
				double(currentGene->numberOfValues - currentGenome[i] - 1)
				* (currentGene->maximum - currentGene->minimum) / double(currentGene->numberOfValues - 1);

			// If there is a tied-to variable specified, update that as well
			if (currentGene->tiedTo != Corner::NumberOfHardpoints)
				currentCorner->hardpoints[currentGene->tiedTo].z() =
					currentCorner->hardpoints[currentGene->hardpoint].z();

			// If the suspension is symmetric, also update the point on the opposite corner
			if (targetSuspension->isSymmetric)
			{
				// Copy the values from one side to the other
				oppositeCorner->hardpoints[currentGene->hardpoint].z() =
					currentCorner->hardpoints[currentGene->hardpoint].z();

				// If there was a tied-to variable specified, we must update that on
				// the other side of the car, too
				if (currentGene->tiedTo != Corner::NumberOfHardpoints)
					oppositeCorner->hardpoints[currentGene->tiedTo].z() =
						oppositeCorner->hardpoints[currentGene->hardpoint].z();
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
void GAObject::PerformAdditionalActions()
{
	double averageFitness = 0.0;
	int i;
	for (i = 0; i < populationSize; i++)
		averageFitness += fitnesses[currentGeneration][i];
	averageFitness /= (double)populationSize;

	double maximumFitness = fitnesses[currentGeneration][0];

	Debugger::GetInstance() << "Completed Generation " << currentGeneration + 1 << Debugger::Priority::VeryHigh;
	Debugger::GetInstance() << "\tAverage Fitness:  " <<
		UnitConverter::GetInstance().FormatNumber(averageFitness) << Debugger::Priority::VeryHigh;
	Debugger::GetInstance() << "\tBest Fitness:     " <<
		UnitConverter::GetInstance().FormatNumber(maximumFitness) << Debugger::Priority::VeryHigh;

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
//		direction		= const Math::Axis& specifying the component of the hardpoint
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
						const Corner::Location &location, const Math::Axis &direction,
						const double &minimum, const double &maximum, const int &numberOfValues)
{
	Gene newGene;
	newGene.hardpoint		= hardpoint;
	newGene.tiedTo			= tiedTo;
	newGene.location		= location;
	newGene.direction		= direction;
	newGene.minimum			= minimum;
	newGene.maximum			= maximum;
	newGene.numberOfValues	= numberOfValues;

	geneList.push_back(newGene);
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
	Goal newGoal;
	newGoal.output				= output;
	newGoal.desiredValue		= desiredValue;
	newGoal.expectedDeviation	= expectedDeviation;
	newGoal.importance			= importance;
	newGoal.beforeInputs		= beforeInputs;
	newGoal.afterInputs			= afterInputs;

	goalList.push_back(newGoal);
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
//		direction		= const Eigen::Vector3d::Axis& specifying the component of the hardpoint
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
						const Corner::Location &location, const Math::Axis &direction,
						const double &minimum, const double &maximum, const int &numberOfValues)
{
	geneList[index].hardpoint		= hardpoint;
	geneList[index].tiedTo			= tiedTo;
	geneList[index].location		= location;
	geneList[index].direction		= direction;
	geneList[index].minimum			= minimum;
	geneList[index].maximum			= maximum;
	geneList[index].numberOfValues	= numberOfValues;
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
	goalList[index].output				= output;
	goalList[index].desiredValue		= desiredValue;
	goalList[index].expectedDeviation	= expectedDeviation;
	goalList[index].importance			= importance;
	goalList[index].beforeInputs		= beforeInputs;
	goalList[index].afterInputs			= afterInputs;
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
void GAObject::DetermineAllInputs()
{
	inputList.clear();
	inputList.push_back(goalList[0].beforeInputs);

	for (const auto& goal : goalList)
	{
		// Compare the before input with all of the inputs in the list
		bool needsToBeAdded = true;
		for (const auto& input : inputList)
		{
			if (goal.beforeInputs == input)
				needsToBeAdded = false;
		}

		if (needsToBeAdded)
			inputList.push_back(goal.beforeInputs);

		// If the before and after inputs are not the same, then check the after inputs, too
		if (goal.beforeInputs != goal.afterInputs)
		{
			// Compare the after input with all of the inputs in the list
			needsToBeAdded = true;
			for (const auto& input : inputList)
			{
				if (goal.afterInputs == input)
					needsToBeAdded = false;
			}

			if (needsToBeAdded)
				inputList.push_back(goal.afterInputs);
		}
	}
}

//==========================================================================
// Class:			GAObject
// Function:		UpdateResultingCar
//
// Description:		Updates the target car to match the best fit genome.
//
// Input Arguments:
//		result	= Car&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GAObject::UpdateResultingCar(Car &result) const
{
	wxMutexLocker lock(gsaMutex);
	DebugLog::GetInstance()->Log(_T("GAObject::UpdateResultingCar (locker)"));

	wxMutexLocker carLock(result.GetMutex());
	result = *originalCarArray[generationLimit - 1];
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
	FileHeaderInfo header;
	header.fileVersion = currentFileVersion;
	outFile->seekp(0);
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
	std::ofstream outFile(fileName.mb_str(), std::ios::binary);

	// Make sure the file was opened OK
	if (!outFile.is_open() || !outFile.good())
		return false;

	// Write the file header information
	WriteFileHeader(&outFile);

	// Write this object's data
	outFile.write(reinterpret_cast<const char*>(&populationSize), sizeof(populationSize));
	outFile.write(reinterpret_cast<const char*>(&generationLimit), sizeof(generationLimit));
	outFile.write(reinterpret_cast<const char*>(&elitism), sizeof(elitism));
	outFile.write(reinterpret_cast<const char*>(&mutation), sizeof(mutation));
	outFile.write(reinterpret_cast<const char*>(&crossover), sizeof(crossover));

	const unsigned int geneCount(geneList.size());
	outFile.write(reinterpret_cast<const char*>(&geneCount), sizeof(geneCount));
	for (const auto& gene : geneList)
		outFile.write(reinterpret_cast<const char*>(&gene), sizeof(gene));

	const unsigned int goalCount(goalList.size());
	outFile.write(reinterpret_cast<const char*>(&goalCount), sizeof(goalCount));
	for (const auto& goal : goalList)
		outFile.write(reinterpret_cast<const char*>(&goal), sizeof(goal));

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
	wxMutexLocker lock(gsaMutex);

	geneList.clear();
	goalList.clear();

	std::ifstream inFile(fileName.mb_str(), std::ios::binary);
	if (!inFile.is_open() || !inFile.good())
		return false;

	FileHeaderInfo header = ReadFileHeader(&inFile);

	if (header.fileVersion != currentFileVersion)
	{
		Debugger::GetInstance() << "ERROR:  Incompatible file versions - could not open file!" << Debugger::Priority::High;
		inFile.close();
		return false;
	}

	// Read this object's data
	inFile.read(reinterpret_cast<char*>(&populationSize), sizeof(populationSize));
	inFile.read(reinterpret_cast<char*>(&generationLimit), sizeof(generationLimit));
	inFile.read(reinterpret_cast<char*>(&elitism), sizeof(elitism));
	inFile.read(reinterpret_cast<char*>(&mutation), sizeof(mutation));
	inFile.read(reinterpret_cast<char*>(&crossover), sizeof(crossover));

	int tempCount, i;
	Gene tempGene;
	inFile.read(reinterpret_cast<char*>(&tempCount), sizeof(tempCount));
	for (i = 0; i < tempCount; i++)
	{
		inFile.read(reinterpret_cast<char*>(&tempGene), sizeof(tempGene));
		geneList.push_back(tempGene);
	}

	Goal tempGoal;
	inFile.read(reinterpret_cast<char*>(&tempCount), sizeof(tempCount));
	for (i = 0; i < tempCount; i++)
	{
		inFile.read(reinterpret_cast<char*>(&tempGoal), sizeof(tempGoal));
		goalList.push_back(tempGoal);
	}

	inFile.close();

	return true;
}

}// namespace VVASE
