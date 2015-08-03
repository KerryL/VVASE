/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  geneticAlgorithm.cpp
// Created:  4/7/2009
// Author:  K. Loux
// Description:  This is a base class for genetic algorithms.  It works with generic genomes
//				 to ensure flexibility, so it is up to the user to translate between genomes
//				 and their object's phenotype.
// History:

// Local headers
#include "vSolver/optimization/geneticAlgorithm.h"
#include "vUtilities/debugLog.h"

// Standard C++ headers
#include <cstdlib>
#include <ctime>

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		GeneticAlgorithm
//
// Description:		Constructor for the GeneticAlgorithm class.
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
GeneticAlgorithm::GeneticAlgorithm()
{
	gsaMutex.Lock();
	DebugLog::GetInstance()->Log(_T("GeneticAlgorithm::GeneticAlgorithm() (lock)"));

	genomes = NULL;
	numberOfPhenotypes = NULL;
	fitnesses = NULL;

	generationLimit = 0;
	sortingMethod = SortMerge;

	DebugLog::GetInstance()->Log(_T("GeneticAlgorithm::GeneticAlgorithm() (unlock)"));
	gsaMutex.Unlock();

	InitializeAlgorithm(0, 0, 0, NULL, false, 0, 0.0, 0.0);
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		~GeneticAlgorithm
//
// Description:		Destructor for the GeneticAlgorithm class.
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
GeneticAlgorithm::~GeneticAlgorithm()
{
	// Delete dynamically allocated memory
	DeleteDynamicMemory();
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		SetPopulationSize
//
// Description:		Sets the population size for this algorithm.
//
// Input Arguments:
//		populationSize	= int specifying the number citizens
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GeneticAlgorithm::SetPopulationSize(int populationSize)
{
	DeleteDynamicMemory();

	wxMutexLocker lock(gsaMutex);
	if (populationSize > 0)
		this->populationSize = populationSize;
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		SetGenerationLimit
//
// Description:		Sets the generation limit for this algorithm.
//
// Input Arguments:
//		generationLimit	= int specifying the maximum number of generations
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GeneticAlgorithm::SetGenerationLimit(int generationLimit)
{
	DeleteDynamicMemory();

	wxMutexLocker lock(gsaMutex);
	if (generationLimit > 0)
		this->generationLimit = generationLimit;
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		SetCrossoverPoint
//
// Description:		Sets the crossover point for this algorithm.
//
// Input Arguments:
//		crossover	= int specifying the point to apply the crossover
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GeneticAlgorithm::SetCrossoverPoint(int crossover)
{
	wxMutexLocker lock(gsaMutex);

	if (crossover < 0)
		crossover = 0;
	// We add the requirement that number of genes is greater than zero because
	// this can be used to set the crossover point prior to the start of the
	// optimization, in which case NumberOfGenes = 0.  We do want to allow the
	// crossover to be set, however, because when we save an optimization to file,
	// we need the crossover variable to store what the user wanted.
	else if (crossover > numberOfGenes)
		this->crossover = numberOfGenes;
	else
		this->crossover = crossover;
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		SetElitismPercentage
//
// Description:		Sets the elitism percentage for this algorithm.
//
// Input Arguments:
//		elitism	= double specifying the new percentage to use for
//					  elitism
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GeneticAlgorithm::SetElitismPercentage(double elitism)
{
	wxMutexLocker lock(gsaMutex);

	// Make sure the value is between 0 and 1
	if (elitism < 0.0)
		this->elitism = 0.0;
	else if (elitism > 1.0)
		this->elitism = 1.0;
	else
		this->elitism = elitism;
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		SetMutationProbability
//
// Description:		Sets the mutation probability for the algorithm.
//
// Input Arguments:
//		mutation	= double specifying the probability of mutation occuring
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GeneticAlgorithm::SetMutationProbability(double mutation)
{
	wxMutexLocker lock(gsaMutex);

	// Make sure the value is between 0 and 1
	if (mutation < 0.0)
		this->mutation = 0.0;
	else if (mutation > 1.0)
		this->mutation = 1.0;
	else
		this->mutation = mutation;
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		InitializeAlgorithm
//
// Description:		Initializes all of the variables used in the optimization.
//
// Input Arguments:
//		populationSize		= int specifying the number of objects in the population
//		generationLimit		= int specifying the maximum number of times to run
//							  the loop
//		numberOfGenes		= int specifying the number of genes for this optimization
//		numberOfPhenotypes	= int* pointing to an array of integers that specify the
//							  number of possible permutations for each gene
//		minimize			= bool indicating whether the fitness function should
//							  be minimized or maximized
//		crossover			= int specifying the crossover point (or the crossover scheme)
//		elitism				= double specifying the elitism percentage
//		mutation			= double specifying the chance of mutation occurring
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GeneticAlgorithm::InitializeAlgorithm(int populationSize, int generationLimit,
	int numberOfGenes, int *numberOfPhenotypes, bool minimize, int crossover,
	double elitism, double mutation)
{
	DeleteDynamicMemory();

	gsaMutex.Lock();
	DebugLog::GetInstance()->Log(_T("GeneticAlgorithm::InitializeAlgorithm (lock)"));

	this->populationSize	= populationSize;
	this->generationLimit	= generationLimit;
	this->numberOfGenes		= numberOfGenes;
	this->minimize			= minimize;

	DebugLog::GetInstance()->Log(_T("GeneticAlgorithm::InitializeAlgorithm (unlock)"));
	gsaMutex.Unlock();

	// Use set functions to assign the rest to ensure the values are within range
	SetCrossoverPoint(crossover);
	SetElitismPercentage(elitism);
	SetMutationProbability(mutation);

	wxMutexLocker lock(gsaMutex);
	DebugLog::GetInstance()->Log(_T("GeneticAlgorithm::InitializeAlgorithm (locker)"));

	currentGeneration = -1;

	// Handle the memory allocation
	if (numberOfGenes > 0 && numberOfPhenotypes)
	{
		this->numberOfPhenotypes = new int[numberOfGenes];

		// Copy the phenotype array
		int i;
		for (i = 0; i < numberOfGenes; i++)
			this->numberOfPhenotypes[i] = numberOfPhenotypes[i];

		// If the number of generations is not zero and the population size is not zero,
		// allocate memory for the genomes and fitnesses
		if (generationLimit > 0 && populationSize > 0)
		{
			// Allocate memory for the top level of pointers
			genomes = new int**[generationLimit];
			fitnesses = new double*[generationLimit];

			// Allocate memory for the genomes and fitnesses within each generation
			for (i = 0; i < generationLimit; i++)
			{
				genomes[i] = new int*[populationSize];
				fitnesses[i] = new double[populationSize];

				// Allocate the memory for each gene for each citizen
				int j;
				for (j = 0; j < populationSize; j++)
					genomes[i][j] = new int[numberOfGenes];
			}
		}
	}
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		SimulateGeneration
//
// Description:		Determines the fitnesses for the current population.
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
void GeneticAlgorithm::SimulateGeneration()
{
	// Determine the fitness for each genome in this generation
	int currentCitizen;
	for (currentCitizen = 0; currentCitizen < populationSize; currentCitizen++)
		fitnesses[currentGeneration][currentCitizen] =
			DetermineFitness(genomes[currentGeneration][currentCitizen]);
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		PerformOptimization
//
// Description:		The main run loop for the algorithm.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, returns true on success, false on failure
//
//==========================================================================
bool GeneticAlgorithm::PerformOptimization()
{
	wxMutexLocker lock(gsaMutex);
	DebugLog::GetInstance()->Log(_T("GeneticAlgorithm::PerformOptimization (locker)"));

	// Make sure everything we need has been defined
	if (numberOfGenes == 0 || !numberOfPhenotypes || !genomes)
		return false;

	bool stopFlag = false;

	while (!stopFlag && currentGeneration + 1 < generationLimit)
	{
		currentGeneration++;
		Breed();

		// Calculate fitnesses
		SimulateGeneration();

		SortByFitness();

		// Allow room for derived classes to interrupt
		PerformAdditionalActions();
	}

	return true;
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		CreateFirstGeneration
//
// Description:		Randomly creates the first generation's genomes.
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
void GeneticAlgorithm::CreateFirstGeneration()
{
	srand((unsigned int)time(NULL));

	// For each citizen in the first generation, go through the process of creating a genome
	int currentCitizen, currentGene;
	for (currentCitizen = 0; currentCitizen < populationSize; currentCitizen++)
	{
		// For each gene in the genome, get a random value that is within the range.
		// (rand() % limit)  will give an integer between 0 and limit - 1.
		for (currentGene = 0; currentGene < numberOfGenes; currentGene++)
			genomes[0][currentCitizen][currentGene] = rand() % numberOfPhenotypes[currentGene];
	}
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		Breed
//
// Description:		Breeds within the current generation to create the next
//					generation.
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
void GeneticAlgorithm::Breed()
{
	// If this is the first generation (CurrentGeneration == 0), then
	// randomly create the first generation
	if (currentGeneration == 0)
	{
		CreateFirstGeneration();
		return;
	}

	// Perform the breeding
	int i, geneToMutate, randomCrossover, numberOfOffspring = 0;
	bool spawnTwoChildren;
	while (numberOfOffspring < populationSize)
	{
		// Look at elitism first - if we don't have the "elite" citizens in the new
		// generation, grab them now
		if (numberOfOffspring < int(elitism * populationSize))
		{
			// Copy the genome from the previous generation
			for (i = 0; i < numberOfGenes; i++)
				genomes[currentGeneration][numberOfOffspring][i] =
					genomes[currentGeneration - 1][numberOfOffspring][i];
		}
		else// Not a direct copy of a previous citizen
		{
			// If there is room for at least two more children in the new generation, spawn two children
			if (populationSize - numberOfOffspring > 1)
				spawnTwoChildren = true;
			else
				spawnTwoChildren = false;

			// TODO(Optional):  Make the chance to be selected as a parent related to the fitness
			// i.e. more fit citizen get selected to mate more often

			// The father is determined by the index, the mother is determined
			// randomly (but must not match the father).  Find the mother.
			int mother = rand() % int(populationSize / 2.0);
			if (numberOfOffspring == 0 && numberOfOffspring == mother)
				mother += 1;
			else if (numberOfOffspring == mother)
				mother -= 1;

			// Determine what crossover scheme to use.  If crossover is between 0 and numberOfGenes,
			// then all genes before that point come from one parent, and all genes after it come from
			// the other.  The opposite genes can be combined to form a second child.  If the crossover
			// point is greater than the NumberOfGenes, we randomly choose a crossover point every time
			// we spawn a new offspring.  If we are not using a crossover point, each gene has an equal
			// chance of coming from either parent.
			if (crossover == 0)// Not using a crossover point
			{
				// Determine if we should spawn the opposite child as well
				if (spawnTwoChildren)
				{
					// For each gene, roll the dice to see if the gene will come from the mother or father
					for (i = 0; i < numberOfGenes; i++)
					{
						// If the roll is less than 50%, take the gene from the father
						if (rand() % 2 == 0)
						{
							// The first child gets this gene from the father
							genomes[currentGeneration][numberOfOffspring][i] =
								genomes[currentGeneration - 1][numberOfOffspring][i];

							// The second child gets this gene from the mother
							genomes[currentGeneration][numberOfOffspring + 1][i] =
								genomes[currentGeneration - 1][mother][i];
						}
						else// This gene come from the mother
						{
							// The first child gets this gene from the mother
							genomes[currentGeneration][numberOfOffspring][i] =
								genomes[currentGeneration - 1][mother][i];

							// The second child gets this gene from the father
							genomes[currentGeneration][numberOfOffspring + 1][i] =
								genomes[currentGeneration - 1][numberOfOffspring][i];
						}
					}
				}
				else// Only spawn one child
				{
					// For each gene, roll the dice to see if the gene will come from the mother or father
					for (i = 0; i < numberOfGenes; i++)
					{
						// If the roll is less than 50%, take the gene from the father
						if (rand() % 2 == 0)
							genomes[currentGeneration][numberOfOffspring][i] =
								genomes[currentGeneration - 1][numberOfOffspring][i];
						else// This gene come from the mother
							genomes[currentGeneration][numberOfOffspring][i] =
								genomes[currentGeneration - 1][mother][i];
					}
				}
			}
			else if (crossover == numberOfGenes)// Random crossover point
			{
				// Determine the crossover point to use for this offspring
				randomCrossover = rand() % numberOfGenes;

				// For each gene, see if the gene will come from the mother or father
				for (i = 0; i < numberOfGenes; i++)
				{
					// If the current gene is before the crossover point, it comes from the father
					if (i < randomCrossover)
						genomes[currentGeneration][numberOfOffspring][i] =
							genomes[currentGeneration - 1][numberOfOffspring][i];
					else// The gene comes from the mother
						genomes[currentGeneration][numberOfOffspring][i] =
							genomes[currentGeneration - 1][mother][i];
				}

				// Determine if we should repeat for a second child
				if (spawnTwoChildren)
				{
					// For each gene, see if the gene will come from the mother or father
					for (i = 0; i < numberOfGenes; i++)
					{
						// If the current gene is before the crossover point, it comes from the father
						if (i < randomCrossover)
							genomes[currentGeneration][numberOfOffspring + 1][i] =
								genomes[currentGeneration - 1][numberOfOffspring][i];
						else// The gene comes from the mother
							genomes[currentGeneration][numberOfOffspring + 1][i] =
								genomes[currentGeneration - 1][mother][i];
					}
				}
			}
			else// Using a static crossover point
			{
				// For each gene, see if the gene will come from the mother or father
				for (i = 0; i < numberOfGenes; i++)
				{
					// If the current gene is before the crossover point, it comes from the father
					if (i < crossover)
						genomes[currentGeneration][numberOfOffspring][i] =
							genomes[currentGeneration - 1][numberOfOffspring][i];
					else// The gene comes from the mother
						genomes[currentGeneration][numberOfOffspring][i] =
							genomes[currentGeneration - 1][mother][i];
				}

				// Determine if we should repeat for a second child
				if (spawnTwoChildren)
				{
					// For each gene, see if the gene will come from the mother or father
					for (i = 0; i < numberOfGenes; i++)
					{
						// If the current gene is before the crossover point, it comes from the father
						if (i < crossover)
							genomes[currentGeneration][numberOfOffspring + 1][i] =
								genomes[currentGeneration - 1][numberOfOffspring][i];
						else// The gene comes from the mother
							genomes[currentGeneration][numberOfOffspring + 1][i] =
								genomes[currentGeneration - 1][mother][i];
					}
				}
			}

			// If mutations are active, determine whether or not a mutation occurs
			if (mutation > 0.0)
			{
				// Determine if a mutation occured
				if (rand() % 100 + 1 < int(mutation * 100.0))
				{
					// Mutate a random gene from this genome
					geneToMutate = rand() % numberOfGenes;
					genomes[currentGeneration][numberOfOffspring][geneToMutate] =
						rand() % numberOfPhenotypes[geneToMutate];
				}

				// If we had two children, check for another chance of mutation
				if (spawnTwoChildren)
				{
					// Determine if the second child should be mutated as well
					if (rand() % 100 + 1 < int(mutation * 100.0))
					{
						// Mutate a random gene from the second child's genome
						geneToMutate = rand() % numberOfGenes;
						genomes[currentGeneration][numberOfOffspring + 1][geneToMutate] =
							rand() % numberOfPhenotypes[geneToMutate];
					}
				}
			}

			// If we spawned two children, increment the number of offspring
			if (spawnTwoChildren)
				numberOfOffspring++;
		}

		// Increment the number of offspring
		numberOfOffspring++;
	}
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		SortByFitness
//
// Description:		Sorts the specified generation by fitness.  The Minimize
//					flag is checked to determine how to sort (high to low or
//					low to high).
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
void GeneticAlgorithm::SortByFitness()
{
	// Use the algorithm specified by the SortingMethod variable
	switch (sortingMethod)
	{
	case SortSelection:
		SelectionSort(fitnesses[currentGeneration], genomes[currentGeneration], populationSize);
		break;

	case SortQuicksort:
		Quicksort(fitnesses[currentGeneration], genomes[currentGeneration], populationSize);
		break;

	case SortMerge:
	default:
		MergeSort(fitnesses[currentGeneration], genomes[currentGeneration], populationSize);
		break;
	}
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		DeleteDynamicMemory
//
// Description:		Deletes dynamically allocated memory.
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
void GeneticAlgorithm::DeleteDynamicMemory()
{
	wxMutexLocker lock(gsaMutex);
	DebugLog::GetInstance()->Log(_T("GeneticAlgorithm::DeleteDynamicMemory (locker)"));

	// Delete the old genomes and fitnesses (if they exist)
	// Here we make the assumption that if and only if the genomes exist,
	// everything else exists, too
	int i, j;
	if (genomes)
	{
		for (i = 0; i < generationLimit; i++)
		{
			// Delete the genomes for each citizen in this generation
			for (j = 0; j < populationSize; j++)
			{
				delete [] genomes[i][j];
				genomes[i][j] = NULL;
			}
			delete [] genomes[i];
			genomes[i] = NULL;

			// Delete the fitnesses
			delete [] fitnesses[i];
			fitnesses[i] = NULL;
		}
		delete [] genomes;
		genomes = NULL;
		delete [] fitnesses;
		fitnesses = NULL;

		// Delete the number of phenotypes
		delete [] numberOfPhenotypes;
		numberOfPhenotypes = NULL;
	}
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		PerformAdditionalActions
//
// Description:		Performs any additional actions required (to be overridden).
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
void GeneticAlgorithm::PerformAdditionalActions()
{
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		SelectionSort
//
// Description:		Sorts the fitnesses and genomes by fitness using a
//					selection sort algorithm.
//
// Input Arguments:
//		fitnessArray	= double* pointing to the list of values to sort
//		genomeArray		= int** pointing to the list of genomes - these
//						  values are not used in the sorting process, but
//						  they must be sorted along with the corresponding
//						  fitness value
//		arraySize		= const int specifying the length of the array
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GeneticAlgorithm::SelectionSort(double *fitnessArray, int **genomeArray,
									  const int arraySize)
{
	// We will need to find the minimum or maximum value and swap it with
	// the next element in the list.
	int bestIndex, i, swapGene;
	double swapFitness;

	// Repeat this procedure until the list is sorted
	int sortIndex = 0;
	while (sortIndex < arraySize)
	{
		// Initialize bestIndex
		bestIndex = sortIndex;

		// Go through the list and find the next best value
		for (i = sortIndex; i < arraySize; i ++)
		{
			// This depends on whether we're finding minimums or maximums
			if ((fitnessArray[i] > fitnessArray[bestIndex] && !minimize) ||
				(fitnessArray[i] < fitnessArray[bestIndex] && minimize))
				bestIndex = i;
		}

		// Swap the next best value with the current element in the list
		swapFitness = fitnessArray[sortIndex];
		fitnessArray[sortIndex] = fitnessArray[bestIndex];
		fitnessArray[bestIndex] = swapFitness;

		// Use a for loop to swap the genomes
		for (i = 0; i < numberOfGenes; i++)
		{
			swapGene = genomeArray[sortIndex][i];
			genomeArray[sortIndex][i] = genomeArray[bestIndex][i];
			genomeArray[bestIndex][i] = swapGene;
		}

		// Increment the sort index
		sortIndex++;
	}
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		MergeSort
//
// Description:		Sorts the fitnesses and genomes by fitness using a
//					merge sort algorithm.
//
// Input Arguments:
//		fitnessArray	= double* pointing to the list of values to sort
//		genomeArray		= int** pointing to the list of genomes - these
//						  values are not used in the sorting process, but
//						  they must be sorted along with the corresponding
//						  fitness value
//		arraySize		= const int specifying the length of the array
//						  (can be different from PopulationSize in the case
//						  of recursive calls)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GeneticAlgorithm::MergeSort(double *fitnessArray, int **genomeArray,
								  const int arraySize)
{
	// If the length is 1 or less, it is already sorted
	if (arraySize <= 1)
		return;

	// Split the array in half, and recursively sort each half of the array
	int firstSize = arraySize / 2;
	int secondSize = arraySize - firstSize;
	double *secondFitnessArray = fitnessArray + firstSize;
	int **secondGenomeArray = genomeArray + firstSize;
	MergeSort(fitnessArray, genomeArray, firstSize);
	MergeSort(secondFitnessArray, secondGenomeArray, secondSize);

	// Create the result arrays
	int i;
	double *resultFitness = new double[arraySize];
	int **resultGenome = new int*[arraySize];
	for (i = 0; i < arraySize; i++)
		resultGenome[i] = new int[numberOfGenes];

	// Merge the two halfs of the array back into one array
	// with the same size as the original
	int firstIndex = 0;
	int secondIndex = 0;
	while (firstIndex < firstSize && secondIndex < secondSize)
	{
		// Now that both half arrays are sorted, we can compare and add
		// one element at a time to rebuild the array
		if ((fitnessArray[firstIndex] > secondFitnessArray[secondIndex] && !minimize) ||
			(fitnessArray[firstIndex] < secondFitnessArray[secondIndex] && minimize))
		{
			// Append the element from the first array to the result
			resultFitness[firstIndex + secondIndex] = fitnessArray[firstIndex];

			// Use a loop to do the same for the genome
			for (i = 0; i < numberOfGenes; i++)
				resultGenome[firstIndex + secondIndex][i] = genomeArray[firstIndex][i];

			// Increment the first index
			firstIndex++;
		}
		else
		{
			// Append the element from the second array to the result
			resultFitness[firstIndex + secondIndex] = secondFitnessArray[secondIndex];

			// Use a loop to do the same for the genome
			for (i = 0; i < numberOfGenes; i++)
				resultGenome[firstIndex + secondIndex][i] = secondGenomeArray[secondIndex][i];

			// Increment the second index
			secondIndex++;
		}
	}

	// Clean up by appending the remaining items to the list
	while (firstIndex < firstSize)
	{
		// Append the element from the first array to the result
		resultFitness[firstIndex + secondIndex] = fitnessArray[firstIndex];

		// Use a loop to do the same for the genome
		for (i = 0; i < numberOfGenes; i++)
			resultGenome[firstIndex + secondIndex][i] = genomeArray[firstIndex][i];

		// Increment the first index
		firstIndex++;
	}
	while (secondIndex < secondSize)
	{
		// Append the element from the second array to the result
		resultFitness[firstIndex + secondIndex] = secondFitnessArray[secondIndex];

		// Use a loop to do the same for the genome
		for (i = 0; i < numberOfGenes; i++)
			resultGenome[firstIndex + secondIndex][i] = secondGenomeArray[secondIndex][i];

		// Increment the second index
		secondIndex++;
	}

	// Assign the results to the input arrays
	for (firstIndex = 0; firstIndex < arraySize; firstIndex++)
	{
		fitnessArray[firstIndex] = resultFitness[firstIndex];

		for (i = 0; i < numberOfGenes; i++)
			genomeArray[firstIndex][i] = resultGenome[firstIndex][i];
	}

	// Delete the result arrays
	delete [] resultFitness;
	resultFitness = NULL;
	for (i = 0; i < arraySize; i++)
	{
		delete [] resultGenome[i];
		resultGenome[i] = NULL;
	}
	delete [] resultGenome;
	resultGenome = NULL;
}

//==========================================================================
// Class:			GeneticAlgorithm
// Function:		Quicksort
//
// Description:		Sorts the fitnesses and genomes by fitness using a
//					quicksort algorithm.
//
// Input Arguments:
//		fitnessArray	= double* pointing to the list of values to sort
//		genomeArray		= int** pointing to the list of genomes - these
//						  values are not used in the sorting process, but
//						  they must be sorted along with the corresponding
//						  fitness value
//		arraySize		= const int specifying the length of the array
//						  (can be different from PopulationSize in the case
//						  of recursive calls)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GeneticAlgorithm::Quicksort(double *fitnessArray, int **genomeArray,
								  const int arraySize)
{
	// If there is nothing to sort, don't do anything
	if (arraySize <= 1)
		return;

	// Pick a random pivot
	/*int PivotLocation = rand() % ArraySize;*/
	// Some texts recommend choosing a random pivot to avoid choosing bad pivots,
	// but we know our list will already be fairly random (toward the end... the
	// beginning is a bad choice because of elitism) so we'll choose the last
	// element in the array
	int pivotLocation = arraySize - 1;
	double pivot = fitnessArray[pivotLocation];

	// For swapping elements
	double swapFitness;
	int *swapGenome = new int[numberOfGenes];

	// Split the array into two groups:  Higher than pivot and lower than pivot
	int sortedElements = 0;
	int numberOfTopElements = 0;
	int swapGene, i;
	while (sortedElements < arraySize)
	{
		// Check to see if the element is larger or smaller than the pivot
		// and if we are sorting in ascending or descending order
		if ((fitnessArray[sortedElements] > pivot && !minimize) ||
			(fitnessArray[sortedElements] < pivot && minimize))
		{
			// Swap this element with the next element toward the top of the list
			swapFitness = fitnessArray[numberOfTopElements];
			fitnessArray[numberOfTopElements] = fitnessArray[sortedElements];
			fitnessArray[sortedElements] = swapFitness;

			// Also swap the genome
			for (i = 0; i < numberOfGenes; i++)
			{
				swapGene = genomeArray[numberOfTopElements][i];
				genomeArray[numberOfTopElements][i] = genomeArray[sortedElements][i];
				genomeArray[sortedElements][i] = swapGene;
			}

			// Increment the number of top elements
			numberOfTopElements++;
		}

		// Increment the number of elements we've sorted
		sortedElements++;
	}

	// Place our pivot in the right location (this is the last time the pivot will be moved)
	swapFitness = fitnessArray[numberOfTopElements];
	fitnessArray[numberOfTopElements] = fitnessArray[arraySize - 1];
	fitnessArray[arraySize - 1] = swapFitness;

	// We must also place the genome in the right location
	for (i = 0; i < numberOfGenes; i++)
	{
		swapGene = genomeArray[numberOfTopElements][i];
		genomeArray[numberOfTopElements][i] = genomeArray[arraySize - 1][i];
		genomeArray[arraySize - 1][i] = swapGene;
	}

	// Call this recursively on the two new lists
	Quicksort(fitnessArray, genomeArray, numberOfTopElements);
	Quicksort(fitnessArray + numberOfTopElements + 1, genomeArray + numberOfTopElements + 1,
		arraySize - numberOfTopElements - 1);

	// Delete the swap genome
	delete swapGenome;
	swapGenome = NULL;
}