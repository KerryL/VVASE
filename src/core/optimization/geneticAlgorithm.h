/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  geneticAlgorithm.h
// Date:  4/7/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  This is a base class for genetic algorithms.  It works with generic genomes
//	      to ensure flexibility, so it is up to the user to translate between genomes
//	      and their object's phenotype.

#ifndef GENETIC_ALGORITHM_H_
#define GENETIC_ALGORITHM_H_

// wxWidgets headers
#include <wx/thread.h>

namespace VVASE
{

class GeneticAlgorithm
{
public:
	GeneticAlgorithm();
	virtual ~GeneticAlgorithm();

	// The algorithm control
	void InitializeAlgorithm(int populationSize, int generationLimit,
		int numberOfGenes, const int *numberOfPhenotypes, bool minimize = false,
		int crossover = 0, double elitism = 0.0, double mutation = 0.0);
	bool PerformOptimization();

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
	void SetPopulationSize(int populationSize);
	inline int GetPopulationSize() const { wxMutexLocker lock(gsaMutex); return populationSize; };

	void SetGenerationLimit(int generationLimit);
	inline int GetGenerationLimit() const { wxMutexLocker lock(gsaMutex); return generationLimit; };

	void SetElitismPercentage(double elitism);
	inline double GetElitismPercentage() const { wxMutexLocker lock(gsaMutex); return elitism; };

	void SetMutationProbability(double mutation);
	inline double GetMutationProbability() const { wxMutexLocker lock(gsaMutex); return mutation; };

	void SetCrossoverPoint(int crossover);
	inline int GetCrossoverPoint() const { wxMutexLocker Lock(gsaMutex); return crossover; };

	inline void SetSortingMethod(SortingMethod sortingMethod) { wxMutexLocker lock(gsaMutex); this->sortingMethod = sortingMethod; };
	inline SortingMethod GetSortingMethod() const { wxMutexLocker lock(gsaMutex); return sortingMethod; };

protected:
	// The fitness function (MUST be overridden)
	virtual double DetermineFitness(const int *currentGenome) = 0;

	// This function allows room for derived classes to take additional actions
	// (report status, interrupt operation, etc.)
	virtual void PerformAdditionalActions();

	int currentGeneration;

	int generationLimit;
	int populationSize;
	int crossover;// 0 to NumberOfGenes
	double elitism;// 0.0 to 1.0
	double mutation;// 0.0 to 1.0

	int numberOfGenes;
	int *numberOfPhenotypes;

	// The genomes (first index:  generation, second index:  citizen, third index:  gene)
	int ***genomes;

	// The fitnesses (first index:  generation, second index:  citizen)
	double **fitnesses;

	mutable wxMutex gsaMutex;

private:
	void CreateFirstGeneration();

	virtual void SimulateGeneration();

	void Breed();

	SortingMethod sortingMethod;

	void SortByFitness();

	// The actual sorting algorithms
	void SelectionSort(double *fitnessArray, int **genomeArray, const int arraySize);
	void MergeSort(double *fitnessArray, int **genomeArray, const int arraySize);
	void Quicksort(double *fitnessArray, int **genomeArray, const int arraySize);

	bool minimize;

	void DeleteDynamicMemory();
};

}// namespace VVASE

#endif// GENETIC_ALGORITHM_H_
