/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  genetic_algorithm_class.cpp
// Created:  4/7/2009
// Author:  K. Loux
// Description:  This is a base class for genetic algorithms.  It works with generic genomes
//				 to ensure flexibility, so it is up to the user to translate between genomes
//				 and their object's phenotype.
// History:

// Local headers
#include "vSolver/optimization/genetic_algorithm_class.h"

// Standard C++ headers
#include <cstdlib>
#include <ctime>

//==========================================================================
// Class:			GENETIC_ALGORITHM
// Function:		GENETIC_ALGORITHM
//
// Description:		Constructor for the GENETIC_ALGORITHM class.
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
GENETIC_ALGORITHM::GENETIC_ALGORITHM()
{
	// Ensure exlcusive access to this object
	GSAMutex.Lock();

	// Initialize the pointers
	Genomes = NULL;
	NumberOfPhenotypes = NULL;
	Fitnesses = NULL;

	// Initialize the number of generations
	GenerationLimit = 0;

	// Initialize the sorting method
	SortingMethod = SortMerge;

	// Release the mutex
	GSAMutex.Unlock();

	// Initialize the class members to zero
	InitializeAlgorithm(0, 0, 0, NULL, false, 0, 0.0, 0.0);
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
// Function:		~GENETIC_ALGORITHM
//
// Description:		Denstructor for the GENETIC_ALGORITHM class.
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
GENETIC_ALGORITHM::~GENETIC_ALGORITHM()
{
	// Ensure exlcusive access to this object
	wxMutexLocker Lock(GSAMutex);

	// Delete dynamically allocated memory
	DeleteDynamicMemory();
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
// Function:		SetPopulationSize
//
// Description:		Sets the population size for this algorithm.
//
// Input Arguments:
//		_PopulationSize	= int specifying the number citizens
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM::SetPopulationSize(int _PopulationSize)
{
	// Ensure exlcusive access to this object
	wxMutexLocker Lock(GSAMutex);

	// Make sure the data is OK, then do the assignment
	if (_PopulationSize > 0)
		PopulationSize = _PopulationSize;

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
// Function:		SetGenerationLimit
//
// Description:		Sets the generation limit for this algorithm.
//
// Input Arguments:
//		_GenerationLimit	= int specifying the maximum number of generations
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM::SetGenerationLimit(int _GenerationLimit)
{
	// Ensure exlcusive access to this object
	wxMutexLocker Lock(GSAMutex);

	// Make sure the data is OK, then do the assignment
	if (_GenerationLimit > 0)
		GenerationLimit = _GenerationLimit;

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
// Function:		SetCrossoverPoint
//
// Description:		Sets the crossover point for this algorithm.
//
// Input Arguments:
//		_Crossover	= int specifying the point to apply the crossover
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM::SetCrossoverPoint(int _Crossover)
{
	// Ensure exlcusive access to this object
	wxMutexLocker Lock(GSAMutex);

	if (_Crossover < 0)
		Crossover = 0;
	// We add the requirement that number of genes is greater than zero because
	// this can be used to set the crossover point prior to the start of the
	// optimization, in which case NumberOfGenes = 0.  We do want to allow the
	// crossover to be set, however, because when we save an optimization to file,
	// we need the crossover variable to store what the user wanted.
	else if (_Crossover > NumberOfGenes && NumberOfGenes > 0)
		Crossover = NumberOfGenes;
	else
		Crossover = _Crossover;

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
// Function:		SetElitismPercentage
//
// Description:		Sets the elitism percentage for this algorithm.
//
// Input Arguments:
//		_Elitism	= double specifying the new percentage to use for
//					  elitism
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM::SetElitismPercentage(double _Elitism)
{
	// Ensure exlcusive access to this object
	wxMutexLocker Lock(GSAMutex);

	// Make sure the value is between 0 and 1
	if (_Elitism < 0.0)
		Elitism = 0.0;
	else if (_Elitism > 1.0)
		Elitism = 1.0;
	else
		Elitism = _Elitism;

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
// Function:		SetMutationProbability
//
// Description:		Sets the mutation probability for the algorithm.
//
// Input Arguments:
//		_Mutation	= double specifying the probability of mutation occuring
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM::SetMutationProbability(double _Mutation)
{
	// Ensure exlcusive access to this object
	wxMutexLocker Lock(GSAMutex);

	// Make sure the value is between 0 and 1
	if (_Mutation < 0.0)
		Mutation = 0.0;
	else if (_Mutation > 1.0)
		Mutation = 1.0;
	else
		Mutation = _Mutation;

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
// Function:		InitializeAlgorithm
//
// Description:		Initializes all of the variables used in the optimization.
//
// Input Arguments:
//		_PopulationSize		= int specifying the number of objects in the population
//		_GenerationLimit	= int specifying the maximum number of times to run
//							  the loop
//		_NumberOfGenes		= int specifying the number of genes for this optimization
//		_NumberOfPhenotypes	= int* pointing to an array of integers that specify the
//							  number of possible permutations for each gene
//		_Minimize			= bool indiciating whether the fitness function should
//							  be minimized or maximized
//		_Crossover			= int specifying the crossover point (or the crossover scheme)
//		_Elitism			= double specifying the elitism percentage
//		_Mutation			= double specifying the chance of mutation occuring
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM::InitializeAlgorithm(int _PopulationSize, int _GenerationLimit,
											int _NumberOfGenes, int *_NumberOfPhenotypes,
											bool _Minimize, int _Crossover,
											double _Elitism, double _Mutation)
{
	// Ensure exlcusive access to this object
	GSAMutex.Lock();

	// Delete dynamically allocated memory before we overwrite our existing sizes
	DeleteDynamicMemory();

	// Copy the values to the class members
	PopulationSize	= _PopulationSize;
	GenerationLimit	= _GenerationLimit;
	NumberOfGenes	= _NumberOfGenes;
	Minimize		= _Minimize;

	// Release the mutex to avoid a deadlock in the following "Set" functions
	GSAMutex.Unlock();

	// Use set functions to assign the rest to ensure the values are within range
	SetCrossoverPoint(_Crossover);
	SetElitismPercentage(_Elitism);
	SetMutationProbability(_Mutation);

	// Ensure exlcusive access to this object (again)
	wxMutexLocker Lock(GSAMutex);

	// Initialize the current generation
	CurrentGeneration = -1;

	// Handle the memory allocation
	if (NumberOfGenes > 0 && _NumberOfPhenotypes)
	{
		// Allocate memory for the number of phenotypes array
		NumberOfPhenotypes = new int[NumberOfGenes];

		// Copy the phenotype array
		int i;
		for (i = 0; i < NumberOfGenes; i++)
			NumberOfPhenotypes[i] = _NumberOfPhenotypes[i];

		// If the number of generations is not zero and the population size is not zero,
		// allocate memory for the genomes and fitnesses
		if (GenerationLimit > 0 && PopulationSize > 0)
		{
			// Allocate memory for the top level of pointers
			Genomes = new int**[GenerationLimit];
			Fitnesses = new double*[GenerationLimit];

			// Allocate memory for the genomes and fitnesses within each generation
			for (i = 0; i < GenerationLimit; i++)
			{
				Genomes[i] = new int*[PopulationSize];
				Fitnesses[i] = new double[PopulationSize];

				// Allocate the memory for each gene for each citizen
				int j;
				for (j = 0; j < PopulationSize; j++)
					Genomes[i][j] = new int[NumberOfGenes];
			}
		}
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
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
void GENETIC_ALGORITHM::SimulateGeneration(void)
{
	// Determine the fitness for each genome in this generation
	int CurrentCitizen;
	for (CurrentCitizen = 0; CurrentCitizen < PopulationSize; CurrentCitizen++)
		Fitnesses[CurrentGeneration][CurrentCitizen] =
			DetermineFitness(Genomes[CurrentGeneration][CurrentCitizen]);

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
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
bool GENETIC_ALGORITHM::PerformOptimization(void)
{
	// Ensure exlcusive access to this object
	wxMutexLocker Lock(GSAMutex);

	// Make sure everything we need has been defined
	if (NumberOfGenes == 0 || !NumberOfPhenotypes || !Genomes)
		return false;

	// Create a local stop flag
	bool StopFlag = false;

	// The main loop
	while (!StopFlag && CurrentGeneration + 1 < GenerationLimit)
	{
		// Increment the generation number
		CurrentGeneration++;

		// Get the next generation's genomes
		Breed();

		// Simulate the current generation (calculate fitnesses)
		SimulateGeneration();

		// Sort the current generation by fitness
		SortByFitness();

		// Allow room for derived classes to interrupt
		PerformAdditionalActions();
	}

	return true;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
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
void GENETIC_ALGORITHM::CreateFirstGeneration(void)
{
	// Seed the random number generator
	srand((unsigned int)time(NULL));

	// For each citizen in the first generation, go through the process of creating a genome
	int CurrentCitizen, CurrentGene;
	for (CurrentCitizen = 0; CurrentCitizen < PopulationSize; CurrentCitizen++)
	{
		// For each gene in the genome, get a random value that is within the range.
		// (rand() % limit)  will give an integer between 0 and limit - 1.
		for (CurrentGene = 0; CurrentGene < NumberOfGenes; CurrentGene++)
			Genomes[0][CurrentCitizen][CurrentGene] = rand() % NumberOfPhenotypes[CurrentGene];
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
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
void GENETIC_ALGORITHM::Breed(void)
{
	// If this is the first generation (CurrentGeneration == 0), then
	// randomly create the first generation
	if (CurrentGeneration == 0)
	{
		CreateFirstGeneration();

		return;
	}

	// Perform the breeding
	int i, GeneToMutate, RandomCrossover, NumberOfOffspring = 0;
	bool SpawnTwoChildren;
	while (NumberOfOffspring < PopulationSize)
	{
		// Look at elitism first - if we don't have the "elite" citizens in the new
		// generation, grab them now
		if (NumberOfOffspring < int(Elitism * PopulationSize))
		{
			// Copy the genome from the previous generation
			for (i = 0; i < NumberOfGenes; i++)
				Genomes[CurrentGeneration][NumberOfOffspring][i] =
					Genomes[CurrentGeneration - 1][NumberOfOffspring][i];
		}
		else// Not a direct copy of a previous citizen
		{
			// If there is room for at least two more children in the new generation, spawn two children
			if (PopulationSize - NumberOfOffspring > 1)
				SpawnTwoChildren = true;
			else
				SpawnTwoChildren = false;

			// TODO(Optional):  Make the chance to be selected as a parent related to the fitness
			// i.e. more fit citizen get selected to mate more often

			// The father is determined by the index, the mother is determined
			// randomly (but must not match the father).  Find the mother.
			int Mother = rand() % int(PopulationSize / 2.0);
			if (NumberOfOffspring == 0 && NumberOfOffspring == Mother)
				Mother += 1;
			else if (NumberOfOffspring == Mother)
				Mother -= 1;

			// Determine what crossover scheme to use.  If Crossover is between 0 and NumberOfGenes,
			// then all genes before that point come from one parent, and all genes after it come from
			// the other.  The opposite genes can be combined to form a second child.  If the crossover
			// point is greater than the NumberOfGenes, we randomly choose a crossover point every time
			// we spawn a new offspring.  If we are not using a crossover point, each gene has an equal
			// chance of coming from either parent.
			if (Crossover == 0)// Not using a crossover point
			{
				// Determine if we should spawn the opposite child as well
				if (SpawnTwoChildren)
				{
					// For each gene, roll the dice to see if the gene will come from the mother or father
					for (i = 0; i < NumberOfGenes; i++)
					{
						// If the roll is less than 50%, take the gene from the father
						if (rand() % 2 == 0)
						{
							// The first child gets this gene from the father
							Genomes[CurrentGeneration][NumberOfOffspring][i] =
								Genomes[CurrentGeneration - 1][NumberOfOffspring][i];

							// The second child gets this gene from the mother
							Genomes[CurrentGeneration][NumberOfOffspring + 1][i] =
								Genomes[CurrentGeneration - 1][Mother][i];
						}
						else// This gene come from the mother
						{
							// The first child gets this gene from the mother
							Genomes[CurrentGeneration][NumberOfOffspring][i] =
								Genomes[CurrentGeneration - 1][Mother][i];

							// The second child gets this gene from the father
							Genomes[CurrentGeneration][NumberOfOffspring + 1][i] =
								Genomes[CurrentGeneration - 1][NumberOfOffspring][i];
						}
					}
				}
				else// Only spawn one child
				{
					// For each gene, roll the dice to see if the gene will come from the mother or father
					for (i = 0; i < NumberOfGenes; i++)
					{
						// If the roll is less than 50%, take the gene from the father
						if (rand() % 2 == 0)
							Genomes[CurrentGeneration][NumberOfOffspring][i] =
								Genomes[CurrentGeneration - 1][NumberOfOffspring][i];
						else// This gene come from the mother
							Genomes[CurrentGeneration][NumberOfOffspring][i] =
								Genomes[CurrentGeneration - 1][Mother][i];
					}
				}
			}
			else if (Crossover == NumberOfGenes)// Random crossover point
			{
				// Determine the crossover point to use for this offspring
				RandomCrossover = rand() % NumberOfGenes;

				// For each gene, see if the gene will come from the mother or father
				for (i = 0; i < NumberOfGenes; i++)
				{
					// If the current gene is before the crossover point, it comes from the father
					if (i < RandomCrossover)
						Genomes[CurrentGeneration][NumberOfOffspring][i] =
							Genomes[CurrentGeneration - 1][NumberOfOffspring][i];
					else// The gene comes from the mother
						Genomes[CurrentGeneration][NumberOfOffspring][i] =
							Genomes[CurrentGeneration - 1][Mother][i];
				}

				// Determine if we should repeat for a second child
				if (SpawnTwoChildren)
				{
					// For each gene, see if the gene will come from the mother or father
					for (i = 0; i < NumberOfGenes; i++)
					{
						// If the current gene is before the crossover point, it comes from the father
						if (i < RandomCrossover)
							Genomes[CurrentGeneration][NumberOfOffspring + 1][i] =
								Genomes[CurrentGeneration - 1][NumberOfOffspring][i];
						else// The gene comes from the mother
							Genomes[CurrentGeneration][NumberOfOffspring + 1][i] =
								Genomes[CurrentGeneration - 1][Mother][i];
					}
				}
			}
			else// Using a static crossover point
			{
				// For each gene, see if the gene will come from the mother or father
				for (i = 0; i < NumberOfGenes; i++)
				{
					// If the current gene is before the crossover point, it comes from the father
					if (i < Crossover)
						Genomes[CurrentGeneration][NumberOfOffspring][i] =
							Genomes[CurrentGeneration - 1][NumberOfOffspring][i];
					else// The gene comes from the mother
						Genomes[CurrentGeneration][NumberOfOffspring][i] =
							Genomes[CurrentGeneration - 1][Mother][i];
				}

				// Determine if we should repeat for a second child
				if (SpawnTwoChildren)
				{
					// For each gene, see if the gene will come from the mother or father
					for (i = 0; i < NumberOfGenes; i++)
					{
						// If the current gene is before the crossover point, it comes from the father
						if (i < Crossover)
							Genomes[CurrentGeneration][NumberOfOffspring + 1][i] =
								Genomes[CurrentGeneration - 1][NumberOfOffspring][i];
						else// The gene comes from the mother
							Genomes[CurrentGeneration][NumberOfOffspring + 1][i] =
								Genomes[CurrentGeneration - 1][Mother][i];
					}
				}
			}

			// If mutations are active, determine whether or not a mutation occurs
			if (Mutation > 0.0)
			{
				// Determine if a mutation occured
				if (rand() % 100 + 1 < int(Mutation * 100.0))
				{
					// Mutate a random gene from this genome
					GeneToMutate = rand() % NumberOfGenes;
					Genomes[CurrentGeneration][NumberOfOffspring][GeneToMutate] =
						rand() % NumberOfPhenotypes[GeneToMutate];
				}

				// If we had two children, check for another chance of mutation
				if (SpawnTwoChildren)
				{
					// Determine if the second child should be mutated as well
					if (rand() % 100 + 1 < int(Mutation * 100.0))
					{
						// Mutate a random gene from the second child's genome
						GeneToMutate = rand() % NumberOfGenes;
						Genomes[CurrentGeneration][NumberOfOffspring + 1][GeneToMutate] =
							rand() % NumberOfPhenotypes[GeneToMutate];
					}
				}
			}

			// If we spawned two children, increment the number of offspring
			if (SpawnTwoChildren)
				NumberOfOffspring++;
		}

		// Increment the number of offspring
		NumberOfOffspring++;
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
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
void GENETIC_ALGORITHM::SortByFitness(void)
{
	// Use the algorithm specified by the SortingMethod variable
	switch (SortingMethod)
	{
	case SortSelection:
		SelectionSort(Fitnesses[CurrentGeneration], Genomes[CurrentGeneration], PopulationSize);
		break;

	case SortQuicksort:
		Quicksort(Fitnesses[CurrentGeneration], Genomes[CurrentGeneration], PopulationSize);
		break;

	case SortMerge:
	default:
		MergeSort(Fitnesses[CurrentGeneration], Genomes[CurrentGeneration], PopulationSize);
		break;
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
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
void GENETIC_ALGORITHM::DeleteDynamicMemory(void)
{
	// Delete the old genomes and fitnesses (if they exist)
	// Here we make the assumption that if and only if the genomes exist,
	// everything else exists, too
	int i, j;
	if (Genomes)
	{
		for (i = 0; i < GenerationLimit; i++)
		{
			// Delete the genomes for each citizen in this generation
			for (j = 0; j < PopulationSize; j++)
			{
				delete [] Genomes[i][j];
				Genomes[i][j] = NULL;
			}
			delete [] Genomes[i];
			Genomes[i] = NULL;

			// Delete the fitnesses
			delete [] Fitnesses[i];
			Fitnesses[i] = NULL;
		}
		delete [] Genomes;
		delete [] Fitnesses;

		// Delete the number of phenotypes
		delete [] NumberOfPhenotypes;
		NumberOfPhenotypes = NULL;
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
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
void GENETIC_ALGORITHM::PerformAdditionalActions(void)
{
	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
// Function:		SelectionSort
//
// Description:		Sorts the fitnesses and genomes by fitness using a
//					selection sort algorithm.
//
// Input Arguments:
//		FitnessArray	= double* pointing to the list of values to sort
//		GenomeArray		= int** pointing to the list of genomes - these
//						  values are not used in the sorting process, but
//						  they must be sorted along with the corresponding
//						  fitness value
//		ArraySize		= const int specifying the length of the array
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GENETIC_ALGORITHM::SelectionSort(double *FitnessArray, int **GenomeArray,
									  const int ArraySize)
{
	// We will need to find the minimum or maximum value and swap it with
	// the next element in the list.
	int BestIndex, i, SwapGene;
	double SwapFitness;

	// Repeat this procedure until the list is sorted
	int SortIndex = 0;
	while (SortIndex < ArraySize)
	{
		// Initialize BestIndex
		BestIndex = SortIndex;

		// Go through the list and find the next best value
		for (i = SortIndex; i < ArraySize; i ++)
		{
			// This depends on whether we're finding minimums or maximums
			if ((FitnessArray[i] > FitnessArray[BestIndex] && !Minimize) ||
				(FitnessArray[i] < FitnessArray[BestIndex] && Minimize))
				BestIndex = i;
		}

		// Swap the next best value with the current element in the list
		SwapFitness = FitnessArray[SortIndex];
		FitnessArray[SortIndex] = FitnessArray[BestIndex];
		FitnessArray[BestIndex] = SwapFitness;

		// Use a for loop to swap the genomes
		for (i = 0; i < NumberOfGenes; i++)
		{
			SwapGene = GenomeArray[SortIndex][i];
			GenomeArray[SortIndex][i] = GenomeArray[BestIndex][i];
			GenomeArray[BestIndex][i] = SwapGene;
		}

		// Increment the sort index
		SortIndex++;
	}

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
// Function:		MergeSort
//
// Description:		Sorts the fitnesses and genomes by fitness using a
//					merge sort algorithm.
//
// Input Arguments:
//		FitnessArray	= double* pointing to the list of values to sort
//		GenomeArray		= int** pointing to the list of genomes - these
//						  values are not used in the sorting process, but
//						  they must be sorted along with the corresponding
//						  fitness value
//		ArraySize		= const int specifying the length of the array
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
void GENETIC_ALGORITHM::MergeSort(double *FitnessArray, int **GenomeArray,
								  const int ArraySize)
{
	// If the length is 1 or less, it is already sorted
	if (ArraySize <= 1)
		return;

	// Split the array in half, and recursively sort each half of the array
	int FirstSize = ArraySize / 2;
	int SecondSize = ArraySize - FirstSize;
	double *SecondFitnessArray = FitnessArray + FirstSize;
	int **SecondGenomeArray = GenomeArray + FirstSize;
	MergeSort(FitnessArray, GenomeArray, FirstSize);
	MergeSort(SecondFitnessArray, SecondGenomeArray, SecondSize);

	// Create the result arrays
	int i;
	double *ResultFitness = new double[ArraySize];
	int **ResultGenome = new int*[ArraySize];
	for (i = 0; i < ArraySize; i++)
		ResultGenome[i] = new int[NumberOfGenes];

	// Merge the two halfs of the array back into one array
	// with the same size as the original
	int FirstIndex = 0;
	int SecondIndex = 0;
	while (FirstIndex < FirstSize && SecondIndex < SecondSize)
	{
		// Now that both half arrays are sorted, we can compare and add
		// one element at a time to rebuild the array
		if ((FitnessArray[FirstIndex] > SecondFitnessArray[SecondIndex] && !Minimize) ||
			(FitnessArray[FirstIndex] < SecondFitnessArray[SecondIndex] && Minimize))
		{
			// Append the element from the first array to the result
			ResultFitness[FirstIndex + SecondIndex] = FitnessArray[FirstIndex];

			// Use a loop to do the same for the genome
			for (i = 0; i < NumberOfGenes; i++)
				ResultGenome[FirstIndex + SecondIndex][i] = GenomeArray[FirstIndex][i];

			// Increment the first index
			FirstIndex++;
		}
		else
		{
			// Append the element from the second array to the result
			ResultFitness[FirstIndex + SecondIndex] = SecondFitnessArray[SecondIndex];

			// Use a loop to do the same for the genome
			for (i = 0; i < NumberOfGenes; i++)
				ResultGenome[FirstIndex + SecondIndex][i] = SecondGenomeArray[SecondIndex][i];

			// Increment the second index
			SecondIndex++;
		}
	}

	// Clean up by appending the remaining items to the list
	while (FirstIndex < FirstSize)
	{
		// Append the element from the first array to the result
		ResultFitness[FirstIndex + SecondIndex] = FitnessArray[FirstIndex];

		// Use a loop to do the same for the genome
		for (i = 0; i < NumberOfGenes; i++)
			ResultGenome[FirstIndex + SecondIndex][i] = GenomeArray[FirstIndex][i];

		// Increment the first index
		FirstIndex++;
	}
	while (SecondIndex < SecondSize)
	{
		// Append the element from the second array to the result
		ResultFitness[FirstIndex + SecondIndex] = SecondFitnessArray[SecondIndex];

		// Use a loop to do the same for the genome
		for (i = 0; i < NumberOfGenes; i++)
			ResultGenome[FirstIndex + SecondIndex][i] = SecondGenomeArray[SecondIndex][i];

		// Increment the second index
		SecondIndex++;
	}

	// Assign the results to the input arrays
	for (FirstIndex = 0; FirstIndex < ArraySize; FirstIndex++)
	{
		FitnessArray[FirstIndex] = ResultFitness[FirstIndex];

		for (i = 0; i < NumberOfGenes; i++)
			GenomeArray[FirstIndex][i] = ResultGenome[FirstIndex][i];
	}

	// Delete the result arrays
	delete [] ResultFitness;
	ResultFitness = NULL;
	for (i = 0; i < ArraySize; i++)
	{
		delete [] ResultGenome[i];
		ResultGenome[i] = NULL;
	}
	delete [] ResultGenome;
	ResultGenome = NULL;

	return;
}

//==========================================================================
// Class:			GENETIC_ALGORITHM
// Function:		Quicksort
//
// Description:		Sorts the fitnesses and genomes by fitness using a
//					quicksort algorithm.
//
// Input Arguments:
//		FitnessArray	= double* pointing to the list of values to sort
//		GenomeArray		= int** pointing to the list of genomes - these
//						  values are not used in the sorting process, but
//						  they must be sorted along with the corresponding
//						  fitness value
//		ArraySize		= const int specifying the length of the array
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
void GENETIC_ALGORITHM::Quicksort(double *FitnessArray, int **GenomeArray,
								  const int ArraySize)
{
	// If there is nothing to sort, don't do anything
	if (ArraySize <= 1)
		return;

	// Pick a random pivot
	/*int PivotLocation = rand() % ArraySize;*/
	// Some texts recommend choosing a random pivot to avoid choosing bad pivots,
	// but we know our list will already be fairly random (toward the end... the
	// beginning is a bad choice because of elitism) so we'll choose the last
	// element in the array
	int PivotLocation = ArraySize - 1;
	double Pivot = FitnessArray[PivotLocation];

	// For swapping elements
	double SwapFitness;
	int *SwapGenome = new int[NumberOfGenes];

	// Split the array into two groups:  Higher than pivot and lower than pivot
	int SortedElements = 0;
	int NumberOfTopElements = 0;
	int SwapGene, i;
	while (SortedElements < ArraySize)
	{
		// Check to see if the element is larger or smaller than the pivot
		// and if we are sorting in ascending or descending order
		if ((FitnessArray[SortedElements] > Pivot && !Minimize) ||
			(FitnessArray[SortedElements] < Pivot && Minimize))
		{
			// Swap this element with the next element toward the top of the list
			SwapFitness = FitnessArray[NumberOfTopElements];
			FitnessArray[NumberOfTopElements] = FitnessArray[SortedElements];
			FitnessArray[SortedElements] = SwapFitness;

			// Also swap the genome
			for (i = 0; i < NumberOfGenes; i++)
			{
				SwapGene = GenomeArray[NumberOfTopElements][i];
				GenomeArray[NumberOfTopElements][i] = GenomeArray[SortedElements][i];
				GenomeArray[SortedElements][i] = SwapGene;
			}

			// Increment the number of top elements
			NumberOfTopElements++;
		}

		// Increment the number of elements we've sorted
		SortedElements++;
	}

	// Place our pivot in the right location (this is the last time the pivot will be moved)
	SwapFitness = FitnessArray[NumberOfTopElements];
	FitnessArray[NumberOfTopElements] = FitnessArray[ArraySize - 1];
	FitnessArray[ArraySize - 1] = SwapFitness;

	// We must also place the genome in the right location
	for (i = 0; i < NumberOfGenes; i++)
	{
		SwapGene = GenomeArray[NumberOfTopElements][i];
		GenomeArray[NumberOfTopElements][i] = GenomeArray[ArraySize - 1][i];
		GenomeArray[ArraySize - 1][i] = SwapGene;
	}

	// Call this recursively on the two new lists
	Quicksort(FitnessArray, GenomeArray, NumberOfTopElements);
	Quicksort(FitnessArray + NumberOfTopElements + 1, GenomeArray + NumberOfTopElements + 1,
		ArraySize - NumberOfTopElements - 1);

	// Delete the swap genome
	delete SwapGenome;
	SwapGenome = NULL;

	return;
}