/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  geneticAlgorithmPanel.h
// Created:  7/30/2009
// Author:  K. Loux
// Description:  Contains the class declarations for a panel that allows users to specify
//				 the parameters, genes and goals of a genetic algorithm.
// History:

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vSolver/physics/kinematics.h"

// wxWidgets forward declarations
class wxGrid;
class wxGridEvent;
class wxGauge;

// VVASE forward declarations
class MAIN_FRAME;
class Convert;
class GENETIC_OPTIMIZATION;

class GENETIC_ALGORITHM_PANEL : public wxScrolledWindow
{
public:
	// Constructor
	GENETIC_ALGORITHM_PANEL(MAIN_FRAME &_MainFrame, GENETIC_OPTIMIZATION &_Optimization);

	// Destructor
	~GENETIC_ALGORITHM_PANEL();

	// Updates the GA parameters
	void UpdateInformation(void);

	// Updates the status bar display
	void IncrementStatusBars(void);

private:
	// Application's converter object
	const Convert &Converter;

	// The optimization object that we represent
	GENETIC_OPTIMIZATION &Optimization;

	// The main application object
	MAIN_FRAME &MainFrame;

	// Updates various parts of the panel's display
	void UpdateSelectableCars(void);
	void UpdateGeneList(void);
	void UpdateGoalList(void);

	void ResetStatusBars(void);

	// Creates a string representing the input configuration
	wxString GetInputString(const KINEMATICS::INPUTS &Inputs,
		const KINEMATICS::INPUTS *SecondInputs = NULL) const;

	// Updates the GA object so it matches the current input parameters
	bool UpdateGAParameters(const bool &ShowWarnings = true);

	// Method for creating the controls
	void CreateControls(void);

	// Controls
	wxButton *AddGene;
	wxButton *EditGene;
	wxButton *RemoveGene;
	wxButton *AddGoal;
	wxButton *EditGoal;
	wxButton *RemoveGoal;
	wxButton *StartStopOptimization;

	wxComboBox *SelectedCar;

	wxGrid *GeneList;
	wxGrid *GoalList;

	wxGauge *GenerationProgress;
	wxGauge *OverallProgress;

	wxTextCtrl *PopulationSize;
	wxTextCtrl *GenerationLimit;
	wxTextCtrl *ElitismFraction;
	wxTextCtrl *CrossoverPoint;
	wxTextCtrl *MutationProbability;

	// Event IDs
	enum BUTTON_EVENT_ID
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
	virtual void TextBoxChangeEvent(wxCommandEvent &event);

	// For the event table
	DECLARE_EVENT_TABLE();
};