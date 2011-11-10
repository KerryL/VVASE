/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  genetic_algorithm_class.h
// Created:  4/7/2009
// Author:  K. Loux
// Description:  This is a base class for genetic algorithms.  It works with generic genomes
//				 to ensure flexibility, so it is up to the user to translate between genomes
//				 and their object's phenotype.
// History:

#ifndef _GENETIC_ALGORITHM_CLASS_H_
#define _GENETIC_ALGORITHM_CLASS_H_

// wxWidgets headers
#include <wx/thread.h>

// Class declaration
class GENETIC_ALGORITHM
{
public:
	// Constructor
	GENETIC_ALGORITHM();

	// Destructor
	virtual ~GENETIC_ALGORITHM();

	// The algorithm control
	void InitializeAlgorithm(int _PopulationSize, int _GenerationLimit,
		int _NumberOfGenes, int *_NumberOfPhenotypes, bool _Minimize = false,
		int _Crossover = 0, double _Elitism = 0.0, double _Mutation = 0.0);
	bool PerformOptimization(void);

	// Sorting methods - We allow for multiple sorting algorithms
	// because some problems may have different requirements
	// (recursive algorithms are fast, but on large arrays they have
	// potential to overflow the stack, for example)
	enum SORTING_METHOD
	{
		SortSelection,
		SortQuicksort,
		SortMerge
	};

	// Private data accessors
	void SetPopulationSize(int _PopulationSize);
	inline int GetPopulationSize(void) const { wxMutexLocker Lock(GSAMutex); return PopulationSize; };

	void SetGenerationLimit(int _GenerationLimit);
	inline int GetGenerationLimit(void) const { wxMutexLocker Lock(GSAMutex); return GenerationLimit; };

	void SetElitismPercentage(double _Elitism);
	inline double GetElitismPercentage(void) const { wxMutexLocker Lock(GSAMutex); return Elitism; };

	void SetMutationProbability(double _Mutation);
	inline double GetMutationProbability(void) const { wxMutexLocker Lock(GSAMutex); return Mutation; };

	void SetCrossoverPoint(int _Crossover);
	inline int GetCrossoverPoint(void) const { wxMutexLocker Lock(GSAMutex); return Crossover; };

	inline void SetSortingMethod(SORTING_METHOD _SortingMethod) { wxMutexLocker Lock(GSAMutex); SortingMethod = _SortingMethod; };
	inline SORTING_METHOD GetSortingMethod(void) const { wxMutexLocker Lock(GSAMutex); return SortingMethod; };

protected:
	// The fitness function (MUST be overridden)
	virtual double DetermineFitness(const int *CurrentGenome) = 0;

	// This function allows room for derived classes to take additional actions
	// (report status, interrupt operation, etc.)
	virtual void PerformAdditionalActions(void);

	// The maximum number of generations
	int GenerationLimit;

	// The current generation
	int CurrentGeneration;

	// The population size
	int PopulationSize;

	// The crossover point
	int Crossover;// 0 to NumberOfGenes

	// The elitism percentage
	double Elitism;// 0.0 to 1.0

	// The mutation probability
	double Mutation;// 0.0 to 1.0

	// The genes and phenotypes
	int NumberOfGenes;
	int *NumberOfPhenotypes;

	// The genomes (first index:  generation, second index:  citizen, third index:  gene)
	int ***Genomes;

	// The fitnesses (first index:  generation, second index:  citizen)
	double **Fitnesses;

	// For thread-safety
	mutable wxMutex GSAMutex;

private:
	// Creates the initial list of genomes (random)
	void CreateFirstGeneration(void);

	// The main working function for the algorithm - made virtual to make threading possible
	virtual void SimulateGeneration(void);

	// Breed within the current generation to come up with the next generation
	// of genomes
	void Breed(void);

	// This object's sorting method
	SORTING_METHOD SortingMethod;

	// Sorts the current generation's genomes and fitnesses by fitness
	void SortByFitness(void);

	// The actual sorting algorithms
	void SelectionSort(double *FitnessArray, int **GenomeArray, const int ArraySize);
	void MergeSort(double *FitnessArray, int **GenomeArray, const int ArraySize);
	void Quicksort(double *FitnessArray, int **GenomeArray, const int ArraySize);

	// Flag that inidicates we should minimize fitness function rather than maximize it
	bool Minimize;

	// Deletes all dynamically allocated memory
	void DeleteDynamicMemory(void);
};

#endif// _GENETIC_ALGORITHM_CLASS_H_