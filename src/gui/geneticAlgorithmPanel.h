/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  geneticAlgorithmPanel.h
// Date:  7/30/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declarations for a panel that allows users to specify
//        the parameters, genes and goals of a genetic algorithm.

#ifndef GENETIC_ALGORITHM_PANEL_H_
#define GENETIC_ALGORITHM_PANEL_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "VVASE/core/analysis/kinematics.h"

// wxWidgets forward declarations
class wxGrid;
class wxGridEvent;
class wxGauge;

namespace VVASE
{

// Local forward declarations
class MainFrame;
class GeneticOptimization;
class GuiCar;

class GeneticAlgorithmPanel : public wxScrolledWindow
{
public:
	GeneticAlgorithmPanel(MainFrame &mainFrame, GeneticOptimization &optimization);
	~GeneticAlgorithmPanel();

	void UpdateInformation();
	void IncrementStatusBars();

private:
	// The optimization object that we represent
	GeneticOptimization &optimization;

	MainFrame &mainFrame;

	// Updates various parts of the panel's display
	void UpdateSelectableCars();
	void UpdateGeneList();
	void UpdateGoalList();

	void ResetStatusBars();

	// Creates a string representing the input configuration
	wxString GetInputString(const Kinematics::Inputs &inputs,
		const Kinematics::Inputs *secondInputs = NULL) const;

	// Updates the GA object so it matches the current input parameters
	bool UpdateGAParameters(const bool &showWarnings = true);

	// Method for creating the controls
	void CreateControls();

	// Controls
	wxButton *addGene;
	wxButton *editGene;
	wxButton *removeGene;
	wxButton *addGoal;
	wxButton *editGoal;
	wxButton *removeGoal;
	wxButton *startStopOptimization;

	wxComboBox *selectedCar;

	wxGrid *geneList;
	wxGrid *goalList;

	wxGauge *generationProgress;
	wxGauge *overallProgress;

	wxTextCtrl *populationSize;
	wxTextCtrl *generationLimit;
	wxTextCtrl *elitismFraction;
	wxTextCtrl *crossoverPoint;
	wxTextCtrl *mutationProbability;

	enum ButtonEventId
	{
		IdAddGene = wxID_HIGHEST + 1200,
		IdRemoveGene,
		IdEditGene,

		IdAddGoal,
		IdEditGoal,
		IdRemoveGoal,

		IdStartStopOptimization,
		IdSelectCar,

		IdGeneList,
		IdGoalList
	};

	// Event handlers
	virtual void AddGeneButtonClickedEvent(wxCommandEvent &event);
	virtual void EditGeneButtonClickedEvent(wxCommandEvent &event);
	virtual void RemoveGeneButtonClickedEvent(wxCommandEvent &event);
	virtual void AddGoalButtonClickedEvent(wxCommandEvent &event);
	virtual void EditGoalButtonClickedEvent(wxCommandEvent &event);
	virtual void RemoveGoalButtonClickedEvent(wxCommandEvent &event);
	virtual void StartStopOptimizationClickedEvent(wxCommandEvent &event);
	virtual void GeneListSelectCellEvent(wxGridEvent &event);
	virtual void GoalListSelectCellEvent(wxGridEvent &event);
	virtual void GeneGridDoubleClickedEvent(wxGridEvent &event);
	virtual void GoalGridDoubleClickedEvent(wxGridEvent &event);
	virtual void TextBoxChangeEvent(wxCommandEvent &event);

	void EditSelectedGene();
	void EditSelectedGoal();

	const GuiCar* GetSelectedCar();

	DECLARE_EVENT_TABLE();
};

}// namespace VVASE

#endif// GENETIC_ALGORITHM_PANEL_H_
