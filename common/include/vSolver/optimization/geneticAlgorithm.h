/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  geneticAlgorithm.h
// Created:  4/7/2009
// Author:  K. Loux
// Description:  This is a base class for genetic algorithms.  It works with generic genomes
//				 to ensure flexibility, so it is up to the user to translate between genomes
//				 and their object's phenotype.
// History:

#ifndef _GENETIC_ALGORITHM_H_
#define _GENETIC_ALGORITHM_H_

// wxWidgets headers
#include <wx/thread.h>

// Class declaration
class GeneticAlgorithm
{
public:
	// Constructor
	GeneticAlgorithm();

	// Destructor
	virtual ~GeneticAlgorithm();

	// The algorithm control
	void InitializeAlgorithm(int _populationSize, int _generationLimit,
		int _numberOfGenes, int *_numberOfPhenotypes, bool _minimize = false,
		int _crossover = 0, double _elitism = 0.0, double _mutation = 0.0);
	bool PerformOptimization(void);

	// Sorting methods - We allow for multiple sorting algorithms
	// because some problems may have different requirements
	// (recursive algorithms are fast, but on large arrays they have
	// potential to overflow the stack, for example)
	enum SortingMethod
	{
		SortSelection,
		SortQuicksort,
		SortMerge
	};

	// Private data accessors
	void SetPopulationSize(int _populationSize);
	inline int GetPopulationSize(void) const { wxMutexLocker lock(gsaMutex); return populationSize; };

	void SetGenerationLimit(int _generationLimit);
	inline int GetGenerationLimit(void) const { wxMutexLocker lock(gsaMutex); return generationLimit; };

	void SetElitismPercentage(double _elitism);
	inline double GetElitismPercentage(void) const { wxMutexLocker lock(gsaMutex); return elitism; };

	void SetMutationProbability(double _mutation);
	inline double GetMutationProbability(void) const { wxMutexLocker lock(gsaMutex); return mutation; };

	void SetCrossoverPoint(int _crossover);
	inline int GetCrossoverPoint(void) const { wxMutexLocker Lock(gsaMutex); return crossover; };

	inline void SetSortingMethod(SortingMethod _sortingMethod) { wxMutexLocker lock(gsaMutex); sortingMethod = _sortingMethod; };
	inline SortingMethod GetSortingMethod(void) const { wxMutexLocker lock(gsaMutex); return sortingMethod; };

protected:
	// The fitness function (MUST be overridden)
	virtual double DetermineFitness(const int *currentGenome) = 0;

	// This function allows room for derived classes to take additional actions
	// (report status, interrupt operation, etc.)
	virtual void PerformAdditionalActions(void);

	// The maximum number of generations
	int generationLimit;

	// The current generation
	int currentGeneration;

	// The population size
	int populationSize;

	// The crossover point
	int crossover;// 0 to NumberOfGenes

	// The elitism percentage
	double elitism;// 0.0 to 1.0

	// The mutation probability
	double mutation;// 0.0 to 1.0

	// The genes and phenotypes
	int numberOfGenes;
	int *numberOfPhenotypes;

	// The genomes (first index:  generation, second index:  citizen, third index:  gene)
	int ***genomes;

	// The fitnesses (first index:  generation, second index:  citizen)
	double **fitnesses;

	// For thread-safety
	mutable wxMutex gsaMutex;

private:
	// Creates the initial list of genomes (random)
	void CreateFirstGeneration(void);

	// The main working function for the algorithm - made virtual to make threading possible
	virtual void SimulateGeneration(void);

	// Breed within the current generation to come up with the next generation
	// of genomes
	void Breed(void);

	// This object's sorting method
	SortingMethod sortingMethod;

	// Sorts the current generation's genomes and fitnesses by fitness
	void SortByFitness(void);

	// The actual sorting algorithms
	void SelectionSort(double *fitnessArray, int **genomeArray, const int arraySize);
	void MergeSort(double *fitnessArray, int **genomeArray, const int arraySize);
	void Quicksort(double *fitnessArray, int **genomeArray, const int arraySize);

	// Flag that inidicates we should minimize fitness function rather than maximize it
	bool Minimize;

	// Deletes all dynamically allocated memory
	void DeleteDynamicMemory(void);
};

#endif// _GENETIC_ALGORITHM_H_