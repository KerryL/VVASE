/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  sweep.cpp
// Date:  1/20/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  This object maintains an array of the kinematic outputs of a car
//        and is associated with a 2D plot on which it can draw the outputs
//        as a function of ride, roll, heave, and steer.

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/datetime.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

// Local headers
#include "gui/iteration.h"
#include "vUtilities/debugger.h"
#include "vUtilities/unitConverter.h"
#include "vSolver/threads/threadJob.h"
#include "vSolver/threads/kinematicsData.h"
#include "vCar/car.h"
#include "gui/components/mainFrame.h"
#include "gui/components/mainTree.h"
#include "gui/plotPanel.h"
#include "vUtilities/machineDefinitions.h"
#include "vMath/carMath.h"
#include "gui/renderer/plotRenderer.h"

//==========================================================================
// Class:			Iteration
// Function:		Iteration
//
// Description:		Constructor for the Iteration class.
//
// Input Arguments:
//		mainFrame			= MainFrame& reference to main application window
//		pathAndFileName	= wxString containing the location of this object on
//							  the hard disk
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Iteration::Iteration(MainFrame &mainFrame, wxString pathAndFileName)
	: GuiObject(mainFrame, pathAndFileName)
{
	int i;
	for (i = 0; i < NumberOfPlots; i++)
		plotActive[i] = false;

	// Initialize the pointers to the X-axis data
	axisValuesPitch = NULL;
	axisValuesRoll = NULL;
	axisValuesHeave = NULL;
	axisValuesRackTravel = NULL;

	// Unless we know anything different, assume we want to associate ourselves with
	// all of the open car objects
	associatedWithAllOpenCars = true;

	// Initialize our "are we caught up" flags
	analysesDisplayed = true;
	secondAnalysisPending = false;
	pendingAnalysisCount = 0;

	// Initialize the working car variables
	workingCarArray = NULL;
	numberOfWorkingCars = 0;

	// Create the renderer
	plotPanel = new PlotPanel(dynamic_cast<wxWindow*>(&mainFrame));
	notebookTab = dynamic_cast<wxWindow*>(plotPanel);

	// Do this prior to initialization so saved files overwrite these defaults
	ReadDefaultsFromConfig();

	// Get an index for this item and add it to the list in the mainFrame
	// MUST be included BEFORE the naming, which must come BEFORE the call to Initialize
	index = mainFrame.AddObjectToList(this);

	name.Printf("Unsaved Iteration %i", index + 1);
	Initialize();
}

//==========================================================================
// Class:			Iteration
// Function:		~Iteration
//
// Description:		Destructor for the Iteration class.
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
Iteration::~Iteration()
{
	ClearAllLists();

	delete [] axisValuesPitch;
	axisValuesPitch = NULL;

	delete [] axisValuesRoll;
	axisValuesRoll = NULL;

	delete [] axisValuesHeave;
	axisValuesHeave = NULL;

	delete [] axisValuesRackTravel;
	axisValuesRackTravel = NULL;

	int i;
	for (i = 0; i < numberOfWorkingCars; i++)
	{
		delete workingCarArray[i];
		workingCarArray[i] = NULL;
	}
	delete [] workingCarArray;
	workingCarArray = NULL;
}

//==========================================================================
// Class:			Iteration
// Function:		Constant declarations
//
// Description:		Where constants for the Iteration class are delcared.
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
//const int Iteration::currentFileVersion = 0;// OBSOLETE 11/17/2010 - Added alternative title and axis labels, etc.
const int Iteration::currentFileVersion = 1;

//==========================================================================
// Class:			Iteration
// Function:		AddCar
//
// Description:		Adds the passed car to the list for the analysis.
//
// Input Arguments:
//		toAdd	= GuiCar* pointing to the car to add
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Iteration::AddCar(GuiCar *toAdd)
{
	if (toAdd == NULL)
		return;

	associatedCars.push_back(toAdd);
}

//==========================================================================
// Class:			Iteration
// Function:		RemoveCar
//
// Description:		Removes the car at the passed address from the analysis.
//
// Input Arguments:
//		toRemove	= GuiCar* pointing to the car to be removed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Iteration::RemoveCar(GuiCar *toRemove)
{
	if (toRemove == NULL)
		return;

	unsigned int indexToRemove;
	for (indexToRemove = 0; indexToRemove < associatedCars.size(); indexToRemove++)
	{
		// Check to see if the pointer in the argument matches the pointer
		// in the list
		if (associatedCars[indexToRemove] == toRemove)
			break;
	}

	if (indexToRemove == associatedCars.size())
		return;

	outputLists[indexToRemove]->Clear();
	outputLists.Remove(indexToRemove);

	associatedCars.erase(associatedCars.begin() + indexToRemove);
}

//==========================================================================
// Class:			Iteration
// Function:		RemoveAllCars
//
// Description:		Resets this object back to a state representative of
//					when it was first created.
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
void Iteration::RemoveAllCars()
{
	ClearAllLists();
}

//==========================================================================
// Class:			Iteration
// Function:		GetIconHandle
//
// Description:		Gets the icon handle from the systems tree.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		integer specifying the icon handle
//
//==========================================================================
int Iteration::GetIconHandle() const
{
	return systemsTree->GetIconHandle(MainTree::IterationIcon);
}

//==========================================================================
// Class:			Iteration
// Function:		UpdateData
//
// Description:		Updates the iteration outputs for all of the cars for
//					the entire range.
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
void Iteration::UpdateData()
{
	// Make sure we are in a state from which we can handle this analysis
	if (!analysesDisplayed)
	{
		// If we are not in an acceptable state, toggle the flag indicating that we
		// need an update, but do not proceed any further.
		secondAnalysisPending = true;
		return;
	}

	// Reset the flags controlling access to this method
	analysesDisplayed = false;
	secondAnalysisPending = false;

	// Make sure we have no more analyses pending
	while (pendingAnalysisCount > 0)
	{
		// Just wait for the threads to finish up
		wxSafeYield();
		wxMilliSleep(50);
	}

	UpdateAutoAssociate();

	// If we don't have any associated cars, then don't do anything
	if (associatedCars.size() == 0)
	{
		// Just update the plot to make it look pretty
		UpdateDisplay();

		return;
	}

	// Get the original values for the kinematic inputs that we will be changing
	Kinematics::Inputs kinematicInputs;

	// Set tire deflections to zero
	kinematicInputs.tireDeflections.leftFront = 0.0;
	kinematicInputs.tireDeflections.rightFront = 0.0;
	kinematicInputs.tireDeflections.leftRear = 0.0;
	kinematicInputs.tireDeflections.rightRear = 0.0;

	// Determine the step size for each input
	double pitchStep	= (range.endPitch - range.startPitch) / (numberOfPoints - 1);// [rad]
	double rollStep		= (range.endRoll - range.startRoll) / (numberOfPoints - 1);// [rad]
	double heaveStep	= (range.endHeave - range.startHeave) / (numberOfPoints - 1);// [in]
	double rackStep		= (range.endRackTravel - range.startRackTravel) / (numberOfPoints - 1);// [in]

	// Delete the X-axis variables
	delete [] axisValuesPitch;
	delete [] axisValuesRoll;
	delete [] axisValuesHeave;
	delete [] axisValuesRackTravel;

	// Determine the number of points required to store data in the case of 3D plots
	unsigned int totalPoints = numberOfPoints;
	if (yAxisType != AxisTypeUnused)
		totalPoints *= numberOfPoints;

	// Re-create the arrays with the appropriate number of points
	axisValuesPitch			= new double[totalPoints];
	axisValuesRoll			= new double[totalPoints];
	axisValuesHeave			= new double[totalPoints];
	axisValuesRackTravel	= new double[totalPoints];

	// Clear out and re-allocate our output lists
	unsigned int i;
	for (i = 0; i < outputLists.GetCount(); i++)
		outputLists[i]->Clear();
	outputLists.Clear();

	// Initialize the semaphore count
	// FIXME:  Check return value to ensure no errors!
	pendingAnalysisCount = associatedCars.size() * totalPoints;

	// Make sure the working cars are initialized
	if (pendingAnalysisCount != (unsigned int)numberOfWorkingCars)
	{
		int i;
		for (i = 0; i < numberOfWorkingCars; i++)
		{
			delete workingCarArray[i];
			workingCarArray[i] = NULL;
		}
		delete [] workingCarArray;
		numberOfWorkingCars = pendingAnalysisCount;
		workingCarArray = new Car*[numberOfWorkingCars];
		for (i = 0; i < numberOfWorkingCars; i++)
			workingCarArray[i] = new Car();
	}

	// Go through car-by-car
	unsigned int currentCar, currentPoint;
	for (currentCar = 0; currentCar < associatedCars.size(); currentCar++)
	{
		// Create a list to store the outputs for this car
		ManagedList<KinematicOutputs> *currentList = new ManagedList<KinematicOutputs>;

		// Add this list to our list of lists (bit confusing?)
		outputLists.Add(currentList);

		// Run the analysis for each point through the range
		for (currentPoint = 0; currentPoint < totalPoints; currentPoint++)
		{
			// Add the current position of the car to the array for the plot's X-axis
			if (yAxisType == AxisTypeUnused)
			{
				axisValuesPitch[currentPoint]		= range.startPitch + pitchStep * currentPoint;
				axisValuesRoll[currentPoint]		= range.startRoll + rollStep * currentPoint;
				axisValuesHeave[currentPoint]		= range.startHeave + heaveStep * currentPoint;
				axisValuesRackTravel[currentPoint]	= range.startRackTravel + rackStep * currentPoint;
			}
			else
			{
				// If we also have a Y-axis, we need to be more careful when determining the axis values
				// FIXME:  I think this is for 3D plotting?
			}

			// Assign the inputs
			kinematicInputs.pitch = axisValuesPitch[currentPoint];
			kinematicInputs.roll = axisValuesRoll[currentPoint];
			kinematicInputs.heave = axisValuesHeave[currentPoint];
			kinematicInputs.rackTravel = axisValuesRackTravel[currentPoint];
			kinematicInputs.firstRotation = mainFrame.GetInputs().firstRotation;
			kinematicInputs.centerOfRotation = mainFrame.GetInputs().centerOfRotation;

			// Run The analysis
			KinematicOutputs *newOutputs = new KinematicOutputs;
			KinematicsData *data = new KinematicsData(&associatedCars[currentCar]->GetOriginalCar(),
				workingCarArray[currentCar * numberOfPoints + currentPoint], kinematicInputs, newOutputs);
			ThreadJob job(ThreadJob::CommandThreadKinematicsIteration, data,
				associatedCars[currentCar]->GetCleanName() + _T(":") + name, index);
			mainFrame.AddJob(job);

			// Add the outputs to the iteration's list
			currentList->Add(newOutputs);
		}
	}
}

//==========================================================================
// Class:			Iteration
// Function:		UpdateDisplay
//
// Description:		Updates the visual display of this object.
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
void Iteration::UpdateDisplay()
{
	// Make sure we have no more analyses waiting
	if (pendingAnalysisCount != 0)
		return;
	
	if (plotPanel)
	{
		plotPanel->ClearAllCurves();

		// Create the datasets for the plot
		// Need to create one dataset per curve per car
		Dataset2D *dataSet;
		unsigned int i, j, k, n;
		double *x, *y;
		for (i = 0; i < NumberOfPlots; i++)
		{
			if (plotActive[i])
			{
				for (j = 0; j < (unsigned int)associatedCars.size(); j++)
				{
					// Create the dataset
					dataSet = new Dataset2D(CountValidValues(j, (PlotID)i));
					x = dataSet->GetXPointer();
					y = dataSet->GetYPointer();
					
					// Populate all values
					n= 0;
					for (k = 0; k < (unsigned int)numberOfPoints; k++)
					{
						if (VVASEMath::IsNaN(GetDataValue(j, k, (PlotID)i)))
							continue;

						x[n] = GetDataValue(j, k,
								(PlotID)(KinematicOutputs::NumberOfOutputScalars + xAxisType));
						y[n] = GetDataValue(j, k, (PlotID)i);
						n++;
					}
					
					// Add the dataset to the plot
					plotPanel->AddCurve(dataSet, associatedCars[j]->GetCleanName()
						+ _T(", ") + GetPlotName((PlotID)i) + _T(" [") +
						GetPlotUnits((PlotID)i) + _T("]"));

					// Set the x-axis information if this is the first pass
					if (plotPanel->GetCurveCount() == 1)
						plotPanel->SetXAxisGridText(
						GetPlotName((PlotID)(KinematicOutputs::NumberOfOutputScalars + xAxisType)) + _T(" [") +
						GetPlotUnits((PlotID)(KinematicOutputs::NumberOfOutputScalars + xAxisType)) + _T("]"));
				}
			}
		}

		ApplyPlotFormatting();
		plotPanel->UpdateDisplay();
	}

	analysesDisplayed = true;

	// If we have a second analysis pending, handle it now
	if (secondAnalysisPending)
		UpdateData();
}

//==========================================================================
// Class:			Iteration
// Function:		CountValidValues
//
// Description:		Counts plottable values in the specified data set
//
// Input Arguments:
//		carIndex	= const unsigned int&
//		index		= const PlotID&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//==========================================================================
unsigned int Iteration::CountValidValues(const unsigned int &carIndex, const PlotID &index) const
{
	unsigned int i, count(0);
	for (i = 0; i < numberOfPoints; i++)
	{
		if (!VVASEMath::IsNaN(GetDataValue(carIndex, i, index)))
			count++;
	}

	return count;
}

//==========================================================================
// Class:			Iteration
// Function:		ClearAllLists
//
// Description:		Updates the iteration outputs for all of the cars for
//					the entire range.
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
void Iteration::ClearAllLists()
{
	unsigned int currentList;
	for (currentList = 0; currentList < outputLists.GetCount(); currentList++)
		outputLists[currentList]->Clear();

	outputLists.Clear();
	associatedCars.clear();
}

//==========================================================================
// Class:			Iteration
// Function:		PerformSaveToFile
//
// Description:		Saves this object to file.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success
//
//==========================================================================
bool Iteration::PerformSaveToFile()
{
	std::ofstream outFile(pathAndFileName.mb_str(), ios::out | ios::binary);
	if (!outFile.is_open() || !outFile.good())
		return false;

	WriteFileHeader(&outFile);

	// Write this object's data
	outFile.write((char*)&associatedWithAllOpenCars, sizeof(bool));
	outFile.write((char*)plotActive, sizeof(bool) * NumberOfPlots);
	outFile.write((char*)&numberOfPoints, sizeof(int));
	outFile.write((char*)&range, sizeof(Range));
	outFile.write((char*)&xAxisType, sizeof(AxisType));

	// Added for file version 1 on 11/17/2010
	outFile.write((char*)&yAxisType, sizeof(AxisType));
	outFile.write((char*)&generateTitleFromFileName, sizeof(bool));
	outFile.write(title.c_str(), (title.Len() + 1) * sizeof(char));
	outFile.write((char*)&autoGenerateXLabel, sizeof(bool));
	outFile.write(xLabel.c_str(), (xLabel.Len() + 1) * sizeof(char));
	outFile.write((char*)&autoGenerateZLabel, sizeof(bool));
	outFile.write(zLabel.c_str(), (zLabel.Len() + 1) * sizeof(char));
	bool temp(plotPanel->GetRenderer()->GetMajorGridOn());
	outFile.write((char*)&temp, sizeof(bool));

	outFile.close();

	return true;
}

//==========================================================================
// Class:			Iteration
// Function:		PerformLoadFromFile
//
// Description:		Loads this object from file.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success
//
//==========================================================================
bool Iteration::PerformLoadFromFile()
{
	std::ifstream inFile(pathAndFileName.mb_str(), ios::in | ios::binary);
	if (!inFile.is_open() || !inFile.good())
		return false;

	FileHeaderInfo header = ReadFileHeader(&inFile);

	// Check to make sure the version matches
	if (header.fileVersion > currentFileVersion)
	{
		Debugger::GetInstance() << "ERROR:  Unrecognized file version - unable to open file!" << Debugger::PriorityHigh;
		inFile.close();

		return false;
	}
	else if (header.fileVersion != currentFileVersion)
		Debugger::GetInstance() << "Warning:  Opening out-of-date file version." << Debugger::PriorityHigh;

	// Read this object's data
	inFile.read((char*)&associatedWithAllOpenCars, sizeof(bool));
	inFile.read((char*)plotActive, sizeof(bool) * NumberOfPlots);
	inFile.read((char*)&numberOfPoints, sizeof(int));
	inFile.read((char*)&range, sizeof(Range));
	inFile.read((char*)&xAxisType, sizeof(AxisType));

	// Stop here if we don't have file version 1 or newer; allow defaults to be used for
	// remaining parameters (as set in constructor)
	if (header.fileVersion < 1)
	{
		inFile.close();
		return true;
	}

	// New as of file version 1
	inFile.read((char*)&yAxisType, sizeof(AxisType));
	inFile.read((char*)&generateTitleFromFileName, sizeof(bool));
	char oneChar;
	title.Empty();
	while (inFile.read(&oneChar, sizeof(char)), oneChar != '\0')
		title.Append(oneChar);
	inFile.read((char*)&autoGenerateXLabel, sizeof(bool));
	xLabel.Empty();
	while (inFile.read(&oneChar, sizeof(char)), oneChar != '\0')
		xLabel.Append(oneChar);
	inFile.read((char*)&autoGenerateZLabel, sizeof(bool));
	zLabel.Empty();
	while (inFile.read(&oneChar, sizeof(char)), oneChar != '\0')
		zLabel.Append(oneChar);
	bool temp;
	inFile.read((char*)&temp, sizeof(bool));
	if (temp)
		plotPanel->GetRenderer()->SetMajorGridOn();
	else
		plotPanel->GetRenderer()->SetMajorGridOff();

	inFile.close();

	return true;
}

//==========================================================================
// Class:			Iteration
// Function:		ReadDefaultsFromConfig
//
// Description:		Read the default iteration settings from the config file.
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
void Iteration::ReadDefaultsFromConfig()
{
	wxFileConfig *configurationFile = new wxFileConfig(_T(""), _T(""),
		wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() + mainFrame.pathToConfigFile, _T(""),
		wxCONFIG_USE_RELATIVE_PATH);

	// Attempt to read from the config file; set the defaults if the value isn't found
	configurationFile->Read(_T("/Iteration/GenerateTitleFromFileName"), &generateTitleFromFileName, true);
	configurationFile->Read(_T("/Iteration/Title"), &title, wxEmptyString);
	configurationFile->Read(_T("/Iteration/AutoGenerateXLabel"), &autoGenerateXLabel, true);
	configurationFile->Read(_T("/Iteration/XLabel"), &xLabel, wxEmptyString);
	configurationFile->Read(_T("/Iteration/AutoGenerateZLabel"), &autoGenerateZLabel, true);
	configurationFile->Read(_T("/Iteration/ZLabel"), &zLabel, wxEmptyString);
	configurationFile->Read(_T("/Iteration/ShowGridLines"), &showGridLines, true);

	bool temp;
	configurationFile->Read(_T("/Iteration/ShowMinorGridLines"), &temp, false);
	if (temp)
		plotPanel->GetRenderer()->SetMinorGridOn();
	else
		plotPanel->GetRenderer()->SetMinorGridOff();
	configurationFile->Read(_T("/Iteration/ShowLegend"), &temp, true);
	if (temp)
		plotPanel->GetRenderer()->SetLegendOn();
	else
		plotPanel->GetRenderer()->SetLegendOff();

	plotPanel->SetDefaultLineSize(configurationFile->Read(_T("/Iteration/LineSize"), 2.0));
	plotPanel->SetDefaultMarkerSize(configurationFile->Read(_T("/Iteration/MarkerSize"), 0L));

	configurationFile->Read(_T("/Iteration/StartPitch"), &range.startPitch, 0.0);
	configurationFile->Read(_T("/Iteration/StartRoll"), &range.startRoll, 0.0);
	configurationFile->Read(_T("/Iteration/StartHeave"), &range.startHeave, 0.0);
	configurationFile->Read(_T("/Iteration/StartRackTravel"), &range.startRackTravel, 0.0);

	configurationFile->Read(_T("/Iteration/EndPitch"), &range.endPitch, 0.0);
	configurationFile->Read(_T("/Iteration/EndRoll"), &range.endRoll, 0.0);
	configurationFile->Read(_T("/Iteration/EndHeave"), &range.endHeave, 0.0);
	configurationFile->Read(_T("/Iteration/EndRackTravel"), &range.endRackTravel, 0.0);

	configurationFile->Read(_T("/Iteration/NumberOfPoints"), (int*)&numberOfPoints, 10);

	configurationFile->Read(_T("/Iteration/XAxisType"), (int*)&xAxisType, (int)AxisTypeUnused);
	configurationFile->Read(_T("/Iteration/YAxisType"), (int*)&yAxisType, (int)AxisTypeUnused);

	wxString activePlotString;
	configurationFile->Read(_T("Iteration/ActivePlots"), &activePlotString, wxEmptyString);

	// Process the empty plot string to turn it into booleans
	if (activePlotString.Len() > 0)
	{
		size_t i(0), lasti(0);
		unsigned long plotIndex;
		while (i = activePlotString.find(_T(';'), i + 1), i != std::string::npos)
		{
			// Convert the string to a number
			if (activePlotString.SubString(lasti, i - 1).ToULong(&plotIndex))
			{
				// Make sure the number isn't greater than the size of the array
				if (plotIndex < NumberOfPlots)
					plotActive[plotIndex] = true;
			}

			lasti = i + 1;
		}
	}

	delete configurationFile;
	configurationFile = NULL;
}

//==========================================================================
// Class:			Iteration
// Function:		WriteDefaultsToConfig
//
// Description:		Writes the current iteration settings to the config file.
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
void Iteration::WriteDefaultsToConfig() const
{
	// Create a configuration file object
	wxFileConfig *configurationFile = new wxFileConfig(_T(""), _T(""),
		wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() + mainFrame.pathToConfigFile, _T(""),
		wxCONFIG_USE_RELATIVE_PATH);

	// Write to the config file
	configurationFile->Write(_T("/Iteration/GenerateTitleFromFileName"), generateTitleFromFileName);
	configurationFile->Write(_T("/Iteration/Title"), title);
	configurationFile->Write(_T("/Iteration/AutoGenerateXLabel"), autoGenerateXLabel);
	configurationFile->Write(_T("/Iteration/XLabel"), xLabel);
	configurationFile->Write(_T("/Iteration/AutoGenerateZLabel"), autoGenerateZLabel);
	configurationFile->Write(_T("/Iteration/ZLabel"), zLabel);
	configurationFile->Write(_T("/Iteration/ShowGridLines"), showGridLines);

	configurationFile->Write(_T("/Iteration/ShowMinorGridLines"), plotPanel->GetRenderer()->GetMinorGridOn());
	configurationFile->Write(_T("/Iteration/ShowLegend"), plotPanel->GetRenderer()->LegendIsVisible());
	configurationFile->Write(_T("/Iteration/LineSize"), plotPanel->GetDefaultLineSize());
	configurationFile->Write(_T("/Iteration/MarkerSize"), plotPanel->GetDefaultMarkerSize());

	configurationFile->Write(_T("/Iteration/StartPitch"), range.startPitch);
	configurationFile->Write(_T("/Iteration/StartRoll"), range.startRoll);
	configurationFile->Write(_T("/Iteration/StartHeave"), range.startHeave);
	configurationFile->Write(_T("/Iteration/StartRackTravel"), range.startRackTravel);

	configurationFile->Write(_T("/Iteration/EndPitch"), range.endPitch);
	configurationFile->Write(_T("/Iteration/EndRoll"), range.endRoll);
	configurationFile->Write(_T("/Iteration/EndHeave"), range.endHeave);
	configurationFile->Write(_T("/Iteration/EndRackTravel"), range.endRackTravel);

	configurationFile->Write(_T("/Iteration/NumberOfPoints"), numberOfPoints);

	configurationFile->Write(_T("/Iteration/XAxisType"), (int)xAxisType);
	configurationFile->Write(_T("/Iteration/YAxisType"), (int)yAxisType);

	// Encode the active plots into a string that can be saved into the configuration file
	wxString activePlotString(wxEmptyString), temp;
	int i;
	for (i = 0; i < NumberOfPlots; i++)
	{
		if (plotActive[i])
		{
			temp.Printf("%i;", i);
			activePlotString += temp;
		}
	}
	configurationFile->Write(_T("Iteration/ActivePlots"), activePlotString);

	// Delete file object
	delete configurationFile;
	configurationFile = NULL;
}

//==========================================================================
// Class:			Iteration
// Function:		ApplyPlotFormatting
//
// Description:		Applies formatting (names, etc.) to plot curves.
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
void Iteration::ApplyPlotFormatting()
{
	if (autoGenerateZLabel)
	{
		unsigned int i;
		wxString label;
		for (i = 0; i < NumberOfPlots; i++)
		{
			if (plotActive[i])
			{
				if (label.IsEmpty())
					label.assign(GetPlotName((PlotID)i) + _T(" [") + GetPlotUnits((PlotID)i) + _T("]"));
				else
					label.assign(_T("Multiple Values"));
			}
		}

		plotPanel->GetRenderer()->SetLeftYLabel(label);
	}
	else
		plotPanel->GetRenderer()->SetLeftYLabel(zLabel);

	// FIXME:  Implement dual labels
	//plotPanel->GetRenderer()->SetRightYLabel();

	if (autoGenerateXLabel)
		plotPanel->GetRenderer()->SetXLabel(
		GetPlotName((PlotID)(KinematicOutputs::NumberOfOutputScalars + xAxisType)) + _T(" [")
		+ GetPlotUnits((PlotID)(KinematicOutputs::NumberOfOutputScalars + xAxisType)) + _T("]"));
	else
		plotPanel->GetRenderer()->SetXLabel(xLabel);

	if (generateTitleFromFileName)
		plotPanel->GetRenderer()->SetTitle(GetCleanName());
	else
		plotPanel->GetRenderer()->SetTitle(title);

	if (showGridLines)
		plotPanel->GetRenderer()->SetMajorGridOn();
	else
		plotPanel->GetRenderer()->SetMajorGridOff();
}

//==========================================================================
// Class:			Iteration
// Function:		SetRange
//
// Description:		Sets this object's range to the specified values.
//
// Input Arguments:
//		_range	= const Iteration::Range& specifying the desired range
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Iteration::SetRange(const Iteration::Range &range)
{
	this->range = range;

	// Make sure the chosen X-axis type is not for a parameter with zero range
	// This is based on the priority Roll->Steer->Heave->Pitch.
	if ((xAxisType == Iteration::AxisTypeRoll && VVASEMath::IsZero(range.startRoll - range.endRoll)) ||
		(xAxisType == Iteration::AxisTypeRackTravel && VVASEMath::IsZero(range.startRackTravel - range.endRackTravel)) ||
		(xAxisType == Iteration::AxisTypeHeave && VVASEMath::IsZero(range.startHeave - range.endHeave)) ||
		(xAxisType == Iteration::AxisTypePitch && VVASEMath::IsZero(range.startPitch - range.endPitch)) ||
		xAxisType == Iteration::AxisTypeUnused)
	{
		if (fabs(range.startRoll - range.endRoll) > 0.0)
			xAxisType = AxisTypeRoll;
		else if (fabs(range.startRackTravel - range.endRackTravel) > 0.0)
			xAxisType = AxisTypeRackTravel;
		else if (fabs(range.startHeave - range.endHeave) > 0.0)
			xAxisType = AxisTypeHeave;
		else if (fabs(range.startPitch - range.endPitch) > 0.0)
			xAxisType = AxisTypePitch;
	}

	// FIXME:  Determine if also have a Y-axis!

	// Set the "this item has changed" flag
	SetModified();
}

//==========================================================================
// Class:			Iteration
// Function:		SetNumberOfPoints
//
// Description:		Sets the number of points used to generate the plots for
//					this object.
//
// Input Arguments:
//		_numberOfPoints	= const int& reference to number of points to use
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Iteration::SetNumberOfPoints(const int &numberOfPoints)
{
	if (numberOfPoints >= 2)
		this->numberOfPoints = numberOfPoints;

	SetModified();
}

//==========================================================================
// Class:			Iteration
// Function:		SetActivePlot
//
// Description:		Sets the flag indicating whether the specified plot is
//					active or not.
//
// Input Arguments:
//		plotID	=	Iteration::PlotID specifying the plot we're intersted in
//		active	=	const bool& true for plot is shown, false otherwise
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Iteration::SetActivePlot(PlotID plotID, const bool &active)
{
	// Make sure the plot ID is valid
	assert(plotID < NumberOfPlots);

	// Set the status for the specified plot
	plotActive[plotID] = active;

	// Sets the "this object has changed" flag
	SetModified();
}

//==========================================================================
// Class:			Iteration
// Function:		UpdateAutoAssociate
//
// Description:		Updates the list of associated cars, if auto-associated
//					is turned on.
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
void Iteration::UpdateAutoAssociate()
{
	// Make sure auto-associate is on
	if (!associatedWithAllOpenCars)
		return;

	// Clear our list of cars
	ClearAllLists();

	// Go through the list of open objects in mainFrame, and add all TypeCar objects
	// to the list
	int i;
	for (i = 0; i < mainFrame.GetObjectCount(); i++)
	{
		if (mainFrame.GetObjectByIndex(i)->GetType() == GuiObject::TypeCar)
			AddCar(static_cast<GuiCar*>(mainFrame.GetObjectByIndex(i)));
	}
}

//==========================================================================
// Class:			Iteration
// Function:		ShowassociatedCarsDialog
//
// Description:		Allows the user to update the list of associated cars.
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
void Iteration::ShowAssociatedCarsDialog()
{
	// Call the auto-associate update function
	UpdateAutoAssociate();

	// Set up the list of cars in mainFrame
	wxArrayString choices;
	std::vector<GuiCar*> openCars;
	int i;
	for (i = 0; i < mainFrame.GetObjectCount(); i++)
	{
		// If the object is a car, add the name to our list of choices, and
		// add the car itself to our list of cars
		if (mainFrame.GetObjectByIndex(i)->GetType() == GuiObject::TypeCar)
		{
			choices.Add(mainFrame.GetObjectByIndex(i)->GetCleanName());
			openCars.push_back(static_cast<GuiCar*>(mainFrame.GetObjectByIndex(i)));
		}
	}

	// Make sure there is at least one car open
	if (openCars.size() == 0)
	{
		Debugger::GetInstance() << "ERROR:  Cannot display dialog - no open cars!" << Debugger::PriorityHigh;
		return;
	}

	// Initialize the selections - this array contains the indeces of the
	// items that are selected
    wxArrayInt selections;
	for (i = 0; i < (int)openCars.size(); i++)
	{
		// If the item is associated with this iteration, add its index to the array
		if (AssociatedWithCar(openCars[i]))
			selections.Add(i);
	}

	if (wxGetSelectedChoices(selections, _T("Select the cars to associate with this iteration:"),
		_T("Associated Cars"), choices, &mainFrame) != -1)
	{
		// Remove all items from our lists
		ClearAllLists();

		// Go through the indices that were selected and add them to our list
		for (i = 0; i < (signed int)selections.GetCount(); i++)
			AddCar(openCars[selections[i]]);

		// If not all of the cars in the list are selected, make sure we're not auto-associating
		if (openCars.size() != associatedCars.size())
			associatedWithAllOpenCars = false;

		mainFrame.SetAssociateWithAllCars();
	}

	// Update the analyses
	UpdateData();
}

//==========================================================================
// Class:			Iteration
// Function:		AssociatedWithCar
//
// Description:		Checks to see if the specified car is associated with
//					this object.
//
// Input Arguments:
//		test	= GuiCar* with which we are comparing our list
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument matches a car in our list
//
//==========================================================================
bool Iteration::AssociatedWithCar(GuiCar *test) const
{
	// Go through our list of associated cars, and see if Test points to any of
	// those cars
	unsigned int i;
	for (i = 0; i < associatedCars.size(); i++)
	{
		if (associatedCars[i] == test)
			return true;
	}

	return false;
}

//==========================================================================
// Class:			Iteration
// Function:		GetDataValue
//
// Description:		Accesses the output data for the specified car and the
//					specified curve.
//
// Input Arguments:
//		associatedCarIndex	= integer specifying the car whose output is requested
//		point				= integer specifying the point whose output is requested
//		id					= PlotID specifying what data we are interested in
//
// Output Arguments:
//		None
//
// Return Value:
//		double, the value of the requested point, converted to user units
//
//==========================================================================
double Iteration::GetDataValue(int associatedCarIndex, int point, PlotID id) const
{
	double value = 0.0;
	Vector temp;

	// Make sure the arguments are valid
	if (associatedCarIndex >= (int)associatedCars.size() ||
		point > (int)numberOfPoints || id > NumberOfPlots)
		return value;

	// Depending on the specified PlotID, choose which member of the KinematicOutputs
	// object to return
	if (id < Pitch)
		value = UnitConverter::GetInstance().ConvertOutput((*outputLists[associatedCarIndex])[point]->GetOutputValue(
			(KinematicOutputs::OutputsComplete)id), KinematicOutputs::GetOutputUnitType(
			(KinematicOutputs::OutputsComplete)id));
	else if (id == Pitch)
		value = UnitConverter::GetInstance().ConvertAngleOutput(axisValuesPitch[point]);
	else if (id == Roll)
		value = UnitConverter::GetInstance().ConvertAngleOutput(axisValuesRoll[point]);
	else if (id == Heave)
		value = UnitConverter::GetInstance().ConvertDistanceOutput(axisValuesHeave[point]);
	else if (id == RackTravel)
		value = UnitConverter::GetInstance().ConvertDistanceOutput(axisValuesRackTravel[point]);
	else
		value = 0.0;

	return value;
}

//==========================================================================
// Class:			Iteration
// Function:		ExportDataToFile
//
// Description:		Exports the data for this object to a comma or tab-delimited
//					text file.
//
// Input Arguments:
//		pathAndFileName	= wxString pointing to the location where the file is
//						  to be saved
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Iteration::ExportDataToFile(wxString pathAndFileName) const
{
	// Determine what type of delimiter to use
	wxString extension(pathAndFileName.substr(pathAndFileName.find_last_of('.') + 1));
	wxChar delimiter;
	if (extension.Cmp(_T("txt")) == 0)
		delimiter = '\t';
	else if (extension.Cmp(_T("csv")) == 0)
		delimiter = ',';
	else
	{
		Debugger::GetInstance() << "ERROR:  Could not export data!  Unable to determine delimiter choice!" << Debugger::PriorityHigh;
		return;
	}

	// Perform the save - open the file
#ifdef UNICODE
	wofstream exportFile(pathAndFileName.mb_str(), ios::out);
#else
	ofstream exportFile(pathAndFileName.mb_str(), ios::out);
#endif

	// Warn the user if the file could not be opened failed
	if (!exportFile.is_open() || !exportFile.good())
	{
		Debugger::GetInstance() << "ERROR:  Could not export data to '" <<  pathAndFileName << "'!" << Debugger::PriorityHigh;
		return;
	}

	// Write the information
	// The column headings consist of three rows:  Plot Name, Units, Car Name
	// After the third row, we start writing the data
	unsigned int currentCar, currentPlot, row;
	const unsigned int numberOfHeadingRows = 3;
	for (row = 0; row < numberOfPoints + numberOfHeadingRows; row++)
	{
		for (currentCar = 0; currentCar < associatedCars.size(); currentCar++)
		{
			for (currentPlot = 0; currentPlot < NumberOfPlots; currentPlot++)
			{
				if (!plotActive[currentPlot] && currentPlot != (unsigned int)xAxisType + KinematicOutputs::NumberOfOutputScalars)
					continue;

				if (row == 0)
					exportFile << GetPlotName((PlotID)currentPlot) << delimiter;
				else if (row == 1)
					exportFile << "(" << GetPlotUnits((PlotID)currentPlot) << ")" << delimiter;
				else if (row == 2)
					exportFile << associatedCars[currentCar]->GetCleanName() << delimiter;
				else
					exportFile << GetDataValue(currentCar, row - numberOfHeadingRows, (PlotID)currentPlot)
						<< delimiter;
			}
		}

		exportFile << endl;
	}

	exportFile.close();
}

//==========================================================================
// Class:			Iteration
// Function:		GetPlotName
//
// Description:		Returns a string containing the name of the specified plot.
//
// Input Arguments:
//		id	= PlotID specifying the plot we are interested in
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the plot
//
//==========================================================================
wxString Iteration::GetPlotName(PlotID id) const
{
	wxString name;

	// Depending on the specified PLOT_ID, choose the name of the string
	// Vectors are a special case - depending on which component of the vector is chosen,
	// we need to append a different string to the end of the Name
	if (id < Pitch)
		name = KinematicOutputs::GetOutputName((KinematicOutputs::OutputsComplete)id);
	else if (id == Pitch)
		name = _T("Pitch");
	else if (id == Roll)
		name = _T("Roll");
	else if (id == Heave)
		name = _T("Heave");
	else if (id == RackTravel)
		name = _T("Rack Travel");
	else
		name = _T("Unrecognized name");

	return name;
}

//==========================================================================
// Class:			Iteration
// Function:		GetPlotUnits
//
// Description:		Returns a string containing the units of the specified plot.
//
// Input Arguments:
//		id	= PlotID specifying the plot we are interested in
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the units of the plot
//
//==========================================================================
wxString Iteration::GetPlotUnits(PlotID id) const
{
	// The value to return
	wxString units;

	// Depending on the specified PLOT_ID, choose the units string
	if (id < Pitch)
		units = UnitConverter::GetInstance().GetUnitType(
			KinematicOutputs::GetOutputUnitType((KinematicOutputs::OutputsComplete)id));
	else if (id == Pitch || id == Roll)
		units = UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeAngle);
	else if (id == Heave || id == RackTravel)
		units = UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance);
	else
		units = _T("Unrecognized units");

	return units;
}

//==========================================================================
// Class:			Iteration
// Function:		WriteFileHeader
//
// Description:		Writes the file header to the specified output stream.
//
// Input Arguments:
//		outFile	= std::ofstream* to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Iteration::WriteFileHeader(std::ofstream *outFile)
{
	// Set up the header information
	FileHeaderInfo header;
	header.fileVersion = currentFileVersion;

	// Set the write pointer to the start of the file
	outFile->seekp(0);

	// Write the header
	outFile->write((char*)&header, sizeof(FileHeaderInfo));
}

//==========================================================================
// Class:			Iteration
// Function:		ReadFileHeader
//
// Description:		Reads the file header information from the specified input stream.
//
// Input Arguments:
//		inFile	= std::ifstream* to read from
//
// Output Arguments:
//		None
//
// Return Value:
//		FileHeaderInfo containing the header information
//
//==========================================================================
Iteration::FileHeaderInfo Iteration::ReadFileHeader(std::ifstream *inFile)
{
	// Set get pointer to the start of the file
	inFile->seekg(0);

	// Read the header struct
	char buffer[sizeof(FileHeaderInfo)];
	inFile->read((char*)buffer, sizeof(FileHeaderInfo));

	return *reinterpret_cast<FileHeaderInfo*>(buffer);
}

//==========================================================================
// Class:			Iteration
// Function:		SetXAxisType
//
// Description:		Sets the X axis to the specified quantity.
//
// Input Arguments:
//		xAxisType	= AxisType to plot against
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Iteration::SetXAxisType(AxisType xAxisType)
{
	this->xAxisType = xAxisType;

	SetModified();
	UpdateDisplay();
}

//==========================================================================
// Class:			Iteration
// Function:		SetYAxisType
//
// Description:		Sets the Y axis to the specified quantity.
//
// Input Arguments:
//		yAxisType	= AxisType to plot against
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Iteration::SetYAxisType(AxisType yAxisType)
{
	this->yAxisType = yAxisType;

	SetModified();
	UpdateDisplay();
}

//==========================================================================
// Class:			Iteration
// Function:		SetAutoAssociate
//
// Description:		Sets the auto-associate with all cars flag.
//
// Input Arguments:
//		autoAssociate	= bool describing whether the flag should be set to
//						  true or false
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Iteration::SetAutoAssociate(bool autoAssociate)
{
	associatedWithAllOpenCars = autoAssociate;
	UpdateAutoAssociate();

	// Set the "this item has changed" flag
	SetModified();

	// Update this object
	UpdateData();
	UpdateDisplay();
}

//==========================================================================
// Class:			Iteration
// Function:		MarkAnalysisComplete
//
// Description:		To be called after one of this object's kinematics analyses
//					completes.
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
void Iteration::MarkAnalysisComplete()
{
	assert(pendingAnalysisCount > 0);

	pendingAnalysisCount--;
	if (pendingAnalysisCount == 0)
		UpdateDisplay();
}