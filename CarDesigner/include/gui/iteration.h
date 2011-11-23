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
//	3/11/2009	- Finished implementation of enum/array style data members for KINEMATIC_OUTPUTS
//				  class, K. Loux.
//	5/19/2009	- Changed to derived class from GUI_OBJECT, K. Loux.
//	11/9/2010	- Added provisions for 3D plotting, K. Loux.
//	11/16/2010	- Moved active plots selection and range inputs to edit panel, K. Loux.

#ifndef _ITERATION_H_
#define _ITERATION_H_

// wxWidgets headers
#include <wx/thread.h>

// VVASE headers
#include "vUtilities/managedList.h"
#include "gui/guiCar.h"
#include "vSolver/physics/kinematics.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vSolver/threads/inverseSemaphore.h"

// VVASE forward declarations
class Debugger;
class Convert;
class Car;
class PlotPanel;

class Iteration : public GuiObject
{
public:
	// Constructor
	Iteration(MainFrame &_mainFrame, const Debugger &_debugger,
		wxString _pathAndFileName = wxEmptyString);

	// Destructor
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
	inline int GetNumberOfPoints(void) const { return numberOfPoints; };
	void SetNumberOfPoints(const int &_numberOfPoints);

	// For associating/disassociating cars with this analysis
	void AddCar(GuiCar *toAdd);
	void RemoveCar(GuiCar *toRemove);
	void RemoveAllCars(void);

	// Runs the kinematic analysis for all associated cars (mandatory overload)
	void UpdateData(void);

	// Updates the displays associated with this object
	void UpdateDisplay(void);

	// Accessors for the range object
	inline Iteration::Range GetRange(void) const { return range; };
	void SetRange(const Iteration::Range &_range);

	// For exporting the data to a comma or tab delimited text file
	void ExportDataToFile(wxString pathAndFileName) const;

	// Sets/gets the auto-association flag
	void SetAutoAssociate(bool autoAssociate);
	inline bool GetAutoAssociate(void) const { return associatedWithAllOpenCars; };

	// Displays a dialog showing all of the open cars, and allows the user to select them
	void ShowAssociatedCarsDialog(void);

	// Returns the number of associated cars
	inline int GetAssociatedCarCount(void) const { return associatedCars.GetCount(); };

	// Returns this object's type (mandatory overload)
	ItemType GetType(void) const { return TypeIteration; };

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
	inline bool GetActivePlot(PlotID plotID) const { return plotActive[plotID]; };
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
	inline AxisType GetXAxisType(void) const { return xAxisType; };
	void SetXAxisType(AxisType _xAxisType);

	// For accessing the Y-Axis data
	inline AxisType GetYAxisType(void) const { return yAxisType; };
	void SetYAxisType(AxisType _yAxisType);

	// For accessing the Z-Axis data
	double GetDataValue(int associatedCarIndex, int point, PlotID id) const;

	// Decrements the number of pending jobs for this object
	void MarkAnalysisComplete(void);

	// For retrieving the strings associated with the plots and their units
	wxString GetPlotName(PlotID id) const;
	wxString GetPlotUnits(PlotID id) const;

	// Accessors for plot display objects
	void SetAutoGenerateTitle(const bool &_generateTitleFromFileName) { generateTitleFromFileName = _generateTitleFromFileName; };
	void SetTitle(const wxString &_title) { title = _title; };
	void SetAutoGenerateXLabel(const bool &_autoGenerateXLabel) { autoGenerateXLabel = _autoGenerateXLabel; };
	void SetXLabel(const wxString &_xLabel) { xLabel = _xLabel; };
	void SetAutoGenerateZLabel(const bool &_autoGenerateZLabel) { autoGenerateZLabel = _autoGenerateZLabel; };
	void SetZLabel(const wxString &_zLabel) { zLabel = _zLabel; };
	void SetShowGridLines(const bool &_showGridLines) { showGridLines = _showGridLines; };

	bool GetAutoGenerateTitle(void) const { return generateTitleFromFileName; };
	wxString GetTitle(void) const { return title; };
	bool GetAutoGenerateXLabel(void) const { return autoGenerateXLabel; };
	wxString GetXLabel(void) const { return xLabel; };
	bool GetAutoGenerateZLabel(void) const { return autoGenerateZLabel; };
	wxString GetZLabel(void) const { return zLabel; };
	bool GetShowGridLines(void) const { return showGridLines; };

	// Methods for reading and writing default configuration settings
	void ReadDefaultsFromConfig(void);
	void WriteDefaultsToConfig(void) const;

private:
	// The application's conversion object
	const Convert &converter;

	// This object's start and stop points
	Range range;

	// The number of points at which the analysis will be performed
	// (per axis for iterations with multiple independent variables)
	int numberOfPoints;

	// Gets the icon handle for this object's icon
	int GetIconHandle(void) const;

	// Performs the saving and loading to/from file (mandatory overloads)
	bool PerformLoadFromFile(void);
	bool PerformSaveToFile(void);

	// Updates the list of cars with which we are associated if the auto associate flag is true
	void UpdateAutoAssociate(void);

	// Checks to see if the specified car is one with which this object is associated
	bool AssociatedWithCar(GuiCar *test) const;

	// The data for this analysis - we need one list of outputs for every car
	// Every list of outputs has one entry for every point within our range
	ManagedList<ManagedList< KinematicOutputs> > outputLists;

	// List of cars that are associated with this analysis
	ObjectList<GuiCar> associatedCars;

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

	// Clears out all of the lists
	void ClearAllLists(void);

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

	// Applies user-defined plot settings
	void ApplyPlotFormatting(void);

	// For getting converted values
	double ConvertValue(KinematicOutputs::OutputsComplete _output);

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
	InverseSemaphore inverseSemaphore;
	// Flag indicating that this object's analyses are complete and another analysis can be started
	bool analysesDisplayed;// FIXME:  I'd like to see these go away - are they needed?  Is there a cleaner way to handle it?
	bool secondAnalysisPending;

	PlotPanel *plotPanel;
};

#endif// _ITERATION_H_