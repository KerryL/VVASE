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
class CAR;
class PlotPanel;

class ITERATION : public GUI_OBJECT
{
public:
	// Constructor
	ITERATION(MAIN_FRAME &_MainFrame, const Debugger &_debugger,
		wxString _PathAndFileName = wxEmptyString);

	// Destructor
	~ITERATION();

	// Structure that defines the starting and stopping points for
	// this analysis
	struct RANGE
	{
		double StartPitch;		// [rad]
		double StartRoll;		// [rad]
		double StartHeave;		// [in]
		double StartRackTravel;	// [in]
		double EndPitch;		// [rad]
		double EndRoll;			// [rad]
		double EndHeave;		// [in]
		double EndRackTravel;	// [in]
	};

	// For getting/setting the number of points in between the start and stop
	inline int GetNumberOfPoints(void) const { return NumberOfPoints; };
	void SetNumberOfPoints(const int &_NumberOfPoints);

	// For associating/disassociating cars with this analysis
	void AddCar(GUI_CAR *ToAdd);
	void RemoveCar(GUI_CAR *ToRemove);
	void RemoveAllCars(void);

	// Runs the kinematic analysis for all associated cars (mandatory overload)
	void UpdateData(void);

	// Updates the displays associated with this object
	void UpdateDisplay(void);

	// Accessors for the range object
	inline ITERATION::RANGE GetRange(void) const { return Range; };
	void SetRange(const ITERATION::RANGE &_Range);

	// For exporting the data to a comma or tab delimited text file
	void ExportDataToFile(wxString PathAndFileName) const;

	// Sets/gets the auto-association flag
	void SetAutoAssociate(bool AutoAssociate);
	inline bool GetAutoAssociate(void) const { return AssociatedWithAllOpenCars; };

	// Displays a dialog showing all of the open cars, and allows the user to select them
	void ShowAssociatedCarsDialog(void);

	// Returns the number of associated cars
	inline int GetAssociatedCarCount(void) const { return AssociatedCars.GetCount(); };

	// Returns this object's type (mandatory overload)
	ITEM_TYPE GetType(void) const { return TYPE_ITERATION; };

	// This is the list of possible plots that we can display
	// Enumeration picks up where the KINEMATIC_OUTPUTS class left off
	enum PLOT_ID
	{
		Pitch = KINEMATIC_OUTPUTS::NumberOfOutputScalars,
		Roll,
		Heave,
		RackTravel,

		NumberOfPlots
	};

	// These are the accessors for getting/setting what plots are shown
	inline bool GetActivePlot(PLOT_ID PlotID) const { return PlotActive[PlotID]; };
	void SetActivePlot(PLOT_ID PlotID, const bool &Active);

	// For specifying the dependant variable
	enum AXIS_TYPE
	{
		AxisTypePitch,
		AxisTypeRoll,
		AxisTypeHeave,
		AxisTypeRackTravel,

		AxisTypeUnused
	};

	// For accessing the X-Axis data
	inline AXIS_TYPE GetXAxisType(void) const { return XAxisType; };
	void SetXAxisType(AXIS_TYPE _XAxisType);

	// For accessing the Y-Axis data
	inline AXIS_TYPE GetYAxisType(void) const { return YAxisType; };
	void SetYAxisType(AXIS_TYPE _YAxisType);

	// For accessing the Z-Axis data
	double GetDataValue(int AssociatedCarIndex, int Point, PLOT_ID Id) const;

	// Decrements the number of pending jobs for this object
	void MarkAnalysisComplete(void);

	// For retrieving the strings associated with the plots and their units
	wxString GetPlotName(PLOT_ID Id) const;
	wxString GetPlotUnits(PLOT_ID Id) const;

	// Accessors for plot display objects
	void SetAutoGenerateTitle(const bool &_GenerateTitleFromFileName) { GenerateTitleFromFileName = _GenerateTitleFromFileName; };
	void SetTitle(const wxString &_Title) { Title = _Title; };
	void SetAutoGenerateXLabel(const bool &_AutoGenerateXLabel) { AutoGenerateXLabel = _AutoGenerateXLabel; };
	void SetXLabel(const wxString &_XLabel) { XLabel = _XLabel; };
	void SetAutoGenerateZLabel(const bool &_AutoGenerateZLabel) { AutoGenerateZLabel = _AutoGenerateZLabel; };
	void SetZLabel(const wxString &_ZLabel) { ZLabel = _ZLabel; };
	void SetShowGridLines(const bool &_ShowGridLines) { ShowGridLines = _ShowGridLines; };

	bool GetAutoGenerateTitle(void) const { return GenerateTitleFromFileName; };
	wxString GetTitle(void) const { return Title; };
	bool GetAutoGenerateXLabel(void) const { return AutoGenerateXLabel; };
	wxString GetXLabel(void) const { return XLabel; };
	bool GetAutoGenerateZLabel(void) const { return AutoGenerateZLabel; };
	wxString GetZLabel(void) const { return ZLabel; };
	bool GetShowGridLines(void) const { return ShowGridLines; };

	// Methods for reading and writing default configuration settings
	void ReadDefaultsFromConfig(void);
	void WriteDefaultsToConfig(void) const;

private:
	// The application's conversion object
	const Convert &Converter;

	// This object's start and stop points
	RANGE Range;

	// The number of points at which the analysis will be performed
	// (per axis for iterations with multiple independent variables)
	int NumberOfPoints;

	// Gets the icon handle for this object's icon
	int GetIconHandle(void) const;

	// Performs the saving and loading to/from file (mandatory overloads)
	bool PerformLoadFromFile(void);
	bool PerformSaveToFile(void);

	// Updates the list of cars with which we are associated if the auto associate flag is true
	void UpdateAutoAssociate(void);

	// Checks to see if the specified car is one with which this object is associated
	bool AssociatedWithCar(GUI_CAR *Test) const;

	// The data for this analysis - we need one list of outputs for every car
	// Every list of outputs has one entry for every point within our range
	ManagedList<ManagedList< KINEMATIC_OUTPUTS> > OutputLists;

	// List of cars that are associated with this analysis
	ObjectList<GUI_CAR> AssociatedCars;

	// Array of working cars to use for the analyses
	CAR **WorkingCarArray;
	int NumberOfWorkingCars;

	// This array holds the values against which the data can be plotted
	// (user gets to choose what the X axis should be)
	double *AxisValuesPitch;//		[rad]
	double *AxisValuesRoll;//		[rad]
	double *AxisValuesHeave;//		[in]
	double *AxisValuesRackTravel;//	[in]

	// Specifies the independent variables for the plot
	AXIS_TYPE XAxisType;
	AXIS_TYPE YAxisType;

	// Clears out all of the lists
	void ClearAllLists(void);

	// Flag indicating whether or not this object should automatically associate
	// with all open GUI_CAR objects
	bool AssociatedWithAllOpenCars;

	// The flags indicating what plots are active
	bool PlotActive[NumberOfPlots];

	// Plot display objects
	bool GenerateTitleFromFileName;
	wxString Title;
	bool AutoGenerateXLabel;
	wxString XLabel;
	bool AutoGenerateZLabel;
	wxString ZLabel;
	bool ShowGridLines;

	// Applies user-defined plot settings
	void ApplyPlotFormatting(void);

	// For getting converted values
	double ConvertValue(KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE _Output);

	// File header information
	struct FILE_HEADER_INFO
	{
		int FileVersion;
	};

	// Writes and reads the file header information for saved cars
	void WriteFileHeader(std::ofstream *OutFile);
	FILE_HEADER_INFO ReadFileHeader(std::ifstream *InFile);

	// Our current file version
	static const int CurrentFileVersion;

	// Synchronization object
	INVERSE_SEMAPHORE InverseSemaphore;
	// Flag indicating that this object's analyses are complete and another analysis can be started
	bool AnalysesDisplayed;
	bool SecondAnalysisPending;

	PlotPanel *plotPanel;
};

#endif// _ITERATION_H_