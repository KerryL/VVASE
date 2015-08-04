/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  iteration.h
// Created:  1/20/2009
// Author:  K. Loux
// Description:  This object maintains an array of the kinematic outputs of a car
//				 and is associated with a plot on which it can draw the outputs
//				 as a function of ride, roll, heave, and steer.
// History:
//	3/11/2009	- Finished implementation of enum/array style data members for KinematicOutputs
//				  class, K. Loux.
//	5/19/2009	- Changed to derived class from GuiObject, K. Loux.
//	11/9/2010	- Added provisions for 3D plotting, K. Loux.
//	11/16/2010	- Moved active plots selection and range inputs to edit panel, K. Loux.

#ifndef ITERATION_H_
#define ITERATION_H_

// wxWidgets headers
#include <wx/thread.h>

// VVASE headers
#include "vUtilities/managedList.h"
#include "gui/guiCar.h"
#include "vSolver/physics/kinematics.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vSolver/threads/inverseSemaphore.h"

// VVASE forward declarations
class Car;
class PlotPanel;

class Iteration : public GuiObject
{
public:
	Iteration(MainFrame &mainFrame, wxString pathAndFileName = wxEmptyString);
	~Iteration();

	// Structure that defines the starting and stopping points for
	// this analysis
	struct Range
	{
		double startPitch;		// [rad]
		double startRoll;		// [rad]
		double startHeave;		// [in]
		double startRackTravel;	// [in]
		double endPitch;		// [rad]
		double endRoll;			// [rad]
		double endHeave;		// [in]
		double endRackTravel;	// [in]
	};

	// For getting/setting the number of points in between the start and stop
	inline int GetNumberOfPoints() const { return numberOfPoints; }
	void SetNumberOfPoints(const int &numberOfPoints);

	// For associating/disassociating cars with this analysis
	void AddCar(GuiCar *toAdd);
	void RemoveCar(GuiCar *toRemove);
	void RemoveAllCars();

	// Runs the kinematic analysis for all associated cars (mandatory overload)
	void UpdateData();

	// Updates the displays associated with this object
	void UpdateDisplay();

	// Accessors for the range object
	inline Iteration::Range GetRange() const { return range; }
	void SetRange(const Iteration::Range &range);

	// For exporting the data to a comma or tab delimited text file
	void ExportDataToFile(wxString pathAndFileName) const;

	// Sets/gets the auto-association flag
	void SetAutoAssociate(bool autoAssociate);
	inline bool GetAutoAssociate() const { return associatedWithAllOpenCars; }

	// Displays a dialog showing all of the open cars, and allows the user to select them
	void ShowAssociatedCarsDialog();

	// Returns the number of associated cars
	inline int GetAssociatedCarCount() const { return associatedCars.size(); }

	// Returns this object's type (mandatory overload)
	ItemType GetType() const { return TypeIteration; }

	// This is the list of possible plots that we can display
	// Enumeration picks up where the KINEMATIC_OUTPUTS class left off
	enum PlotID
	{
		Pitch = KinematicOutputs::NumberOfOutputScalars,
		Roll,
		Heave,
		RackTravel,

		NumberOfPlots
	};

	// These are the accessors for getting/setting what plots are shown
	inline bool GetActivePlot(PlotID plotID) const { return plotActive[plotID]; }
	void SetActivePlot(PlotID plotID, const bool &active);

	// For specifying the dependant variable
	enum AxisType
	{
		AxisTypePitch,
		AxisTypeRoll,
		AxisTypeHeave,
		AxisTypeRackTravel,

		AxisTypeUnused
	};

	// For accessing the X-Axis data
	inline AxisType GetXAxisType() const { return xAxisType; }
	void SetXAxisType(AxisType xAxisType);

	// For accessing the Y-Axis data
	inline AxisType GetYAxisType() const { return yAxisType; }
	void SetYAxisType(AxisType yAxisType);

	// For accessing the Z-Axis data
	double GetDataValue(int associatedCarIndex, int point, PlotID id) const;

	// Decrements the number of pending jobs for this object
	void MarkAnalysisComplete();

	// For retrieving the strings associated with the plots and their units
	wxString GetPlotName(PlotID id) const;
	wxString GetPlotUnits(PlotID id) const;

	// Accessors for plot display objects
	void SetAutoGenerateTitle(const bool &generateTitleFromFileName) { this->generateTitleFromFileName = generateTitleFromFileName; }
	void SetTitle(const wxString &title) { this->title = title; }
	void SetAutoGenerateXLabel(const bool &autoGenerateXLabel) { this->autoGenerateXLabel = autoGenerateXLabel; }
	void SetXLabel(const wxString &xLabel) { this->xLabel = xLabel; }
	void SetAutoGenerateZLabel(const bool &autoGenerateZLabel) { this->autoGenerateZLabel = autoGenerateZLabel; }
	void SetZLabel(const wxString &zLabel) { this->zLabel = zLabel; }
	void SetShowGridLines(const bool &showGridLines) { this->showGridLines = showGridLines; }

	bool GetAutoGenerateTitle() const { return generateTitleFromFileName; }
	wxString GetTitle() const { return title; }
	bool GetAutoGenerateXLabel() const { return autoGenerateXLabel; }
	wxString GetXLabel() const { return xLabel; }
	bool GetAutoGenerateZLabel() const { return autoGenerateZLabel; }
	wxString GetZLabel() const { return zLabel; }
	bool GetShowGridLines() const { return showGridLines; }

	// Methods for reading and writing default configuration settings
	void ReadDefaultsFromConfig();
	void WriteDefaultsToConfig() const;

private:
	// This object's start and stop points
	Range range;

	// The number of points at which the analysis will be performed
	// (per axis for iterations with multiple independent variables)
	unsigned int numberOfPoints;

	// Gets the icon handle for this object's icon
	int GetIconHandle() const;

	// Performs the saving and loading to/from file (mandatory overloads)
	bool PerformLoadFromFile();
	bool PerformSaveToFile();

	void UpdateAutoAssociate();

	bool AssociatedWithCar(GuiCar *test) const;

	// The data for this analysis - we need one list of outputs for every car
	// Every list of outputs has one entry for every point within our range
	ManagedList<ManagedList< KinematicOutputs> > outputLists;

	std::vector<GuiCar*> associatedCars;

	// Array of working cars to use for the analyses
	Car **workingCarArray;
	int numberOfWorkingCars;

	// This array holds the values against which the data can be plotted
	// (user gets to choose what the X axis should be)
	double *axisValuesPitch;//		[rad]
	double *axisValuesRoll;//		[rad]
	double *axisValuesHeave;//		[in]
	double *axisValuesRackTravel;//	[in]

	// Specifies the independent variables for the plot
	AxisType xAxisType;
	AxisType yAxisType;

	void ClearAllLists();

	// Flag indicating whether or not this object should automatically associate
	// with all open GUI_CAR objects
	bool associatedWithAllOpenCars;

	// The flags indicating what plots are active
	bool plotActive[NumberOfPlots];

	// Plot display objects
	bool generateTitleFromFileName;
	wxString title;
	bool autoGenerateXLabel;
	wxString xLabel;
	bool autoGenerateZLabel;
	wxString zLabel;
	bool showGridLines;

	void ApplyPlotFormatting();

	double ConvertValue(KinematicOutputs::OutputsComplete output);
	unsigned int CountValidValues(const unsigned int &carIndex, const PlotID &index) const;

	// File header information
	struct FileHeaderInfo
	{
		int fileVersion;
	};

	// Writes and reads the file header information for saved cars
	void WriteFileHeader(std::ofstream *outFile);
	FileHeaderInfo ReadFileHeader(std::ifstream *inFile);

	// Our current file version
	static const int currentFileVersion;

	// Synchronization object
	unsigned int pendingAnalysisCount;
	
	// Flag indicating that this object's analyses are complete and another analysis can be started
	bool analysesDisplayed;// FIXME:  I'd like to see these go away - are they needed?  Is there a cleaner way to handle it?
	bool secondAnalysisPending;

	PlotPanel *plotPanel;
};

#endif// ITERATION_H_