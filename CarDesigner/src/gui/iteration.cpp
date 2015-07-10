/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  iteration.cpp
// Created:  1/20/2009
// Author:  K. Loux
// Description:  This object maintains an array of the kinematic outputs of a car
//				 and is associated with a 2D plot on which it can draw the outputs
//				 as a function of ride, roll, heave, and steer.
// History:
//	3/11/2009	- Finished implementation of enum/array style data members for KinematicOutputs
//				  class, K. Loux.
//	5/19/2009	- Changed to derived class from GuiObject, K. Loux.
//	10/18/2010	- Fixed bugs causing crash when new Iteration is canceled during creation, K. Loux.
//	11/9/2010	- Added provisions for 3D plotting, K. Loux.
//	11/16/2010	- Moved active plots selection and range inputs to edit panel, K. Loux.

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/datetime.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

// VVASE headers
#include "gui/iteration.h"
#include "vUtilities/debugger.h"
#include "vUtilities/convert.h"
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
//		_mainFrame			= MainFrame& reference to main application window
//		_pathAndFileName	= wxString containing the location of this object on
//							  the hard disk
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Iteration::Iteration(MainFrame &_mainFrame, wxString _pathAndFileName)
					 : GuiObject(_mainFrame, _pathAndFileName)
{
	// Initialize our list of plots to OFF
	int i;
	for (i = 0; i < NumberOfPlots; i++)
		plotActive[i] = false;

	// Read default settings from config file
	// Do this prior to initialization so saved files overwrite these defaults
	ReadDefaultsFromConfig();

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

	// Get an index for this item and add it to the list in the mainFrame
	// MUST be included BEFORE the naming, which must come BEFORE the call to Initialize
	index = mainFrame.AddObjectToList(this);

	// Create the name based on the index
	name.Printf("Unsaved Iteration %i", index + 1);

	// Complete initialization of this object
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
	// Clear out the lists
	ClearAllLists();

	// Delete the X-axis values
	delete [] axisValuesPitch;
	axisValuesPitch = NULL;

	delete [] axisValuesRoll;
	axisValuesRoll = NULL;

	delete [] axisValuesHeave;
	axisValuesHeave = NULL;

	delete [] axisValuesRackTravel;
	axisValuesRackTravel = NULL;

	// Delete the working car array
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
	// Make sure we have a valid pointer
	if (toAdd == NULL)
		return;

	// Add the car to the list
	associatedCars.Add(toAdd);
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
	// Make sure we have a valid pointer
	if (toRemove == NULL)
		return;

	// Find the car to be removed
	int indexToRemove;
	for (indexToRemove = 0; indexToRemove < associatedCars.GetCount(); indexToRemove++)
	{
		// Check to see if the pointer in the argument matches the pointer
		// in the list
		if (associatedCars[indexToRemove] == toRemove)
			break;
	}

	// Make sure we found a car
	if (indexToRemove == associatedCars.GetCount())
		return;

	// Remove the outputs associated with the car
	outputLists[indexToRemove]->Clear();
	outputLists.Remove(indexToRemove);

	// Remove the car itself
	associatedCars.Remove(indexToRemove);
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
void Iteration::RemoveAllCars(void)
{
	// Remove all entries from the lists
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
int Iteration::GetIconHandle(void) const
{
	// Return the proper icon handle
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
void Iteration::UpdateData(void)
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

	// Call the UpdateAutoAssociate method
	UpdateAutoAssociate();

	// If we don't have any associated cars, then don't do anything
	if (associatedCars.GetCount() == 0)
	{
		// Just update the plot to make it look pretty
		UpdateDisplay();

		return;
	}

	// Get the original values for the kinematic inputs that we will be changing
	Kinematics::Inputs kinematicInputs;

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
	int totalPoints = numberOfPoints;
	if (yAxisType != AxisTypeUnused)
		totalPoints *= numberOfPoints;

	// Re-create the arrays with the appropriate number of points
	axisValuesPitch			= new double[totalPoints];
	axisValuesRoll			= new double[totalPoints];
	axisValuesHeave			= new double[totalPoints];
	axisValuesRackTravel	= new double[totalPoints];

	// Clear out and re-allocate our output lists
	int i;
	for (i = 0; i < outputLists.GetCount(); i++)
		outputLists[i]->Clear();
	outputLists.Clear();

	// Initialize the semaphore count
	// FIXME:  Check return value to ensure no errors!
	pendingAnalysisCount = associatedCars.GetCount() * totalPoints;

	// Make sure the working cars are initialized
	if (pendingAnalysisCount != (unsigned int)numberOfWorkingCars)
	{
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
	int currentCar, currentPoint;
	for (currentCar = 0; currentCar < associatedCars.GetCount(); currentCar++)
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
void Iteration::UpdateDisplay(void)
{
	// Make sure we have no more analyses waiting
	if (pendingAnalysisCount != 0)
		return;
	
	// Make sure the renderer exists so we don't do this until after we're fully created
	if (plotPanel)
	{
		// Clear out existing data from the plot
		plotPanel->ClearAllCurves();

		// Create the datasets for the plot
		// Need to create one dataset per curve per car
		Dataset2D *dataSet;
		unsigned int i, j, k;
		double *x, *y;
		for (i = 0; i < NumberOfPlots; i++)
		{
			// Check to see if this plot is active
			if (plotActive[i])
			{
				for (j = 0; j < (unsigned int)associatedCars.GetCount(); j++)
				{
					// Create the dataset
					dataSet = new Dataset2D(numberOfPoints);
					x = dataSet->GetXPointer();
					y = dataSet->GetYPointer();
					
					// Populate all values
					for (k = 0; k < (unsigned int)numberOfPoints; k++)
					{
						x[k] = GetDataValue(j, k,
								(PlotID)(KinematicOutputs::NumberOfOutputScalars + xAxisType));
						y[k] = GetDataValue(j, k, (PlotID)i);
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

		// Apply formatting
		ApplyPlotFormatting();

		// Update the display associated with this object
		plotPanel->UpdateDisplay();
	}

	// Reset the "are we caught up?" flag
	analysesDisplayed = true;

	// If we have a second analysis pending, handle it now
	if (secondAnalysisPending)
		UpdateData();
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
void Iteration::ClearAllLists(void)
{
	// Clear out all of our lists
	int currentList;
	for (currentList = 0; currentList < outputLists.GetCount(); currentList++)
		// Remove all of the sub-lists
		outputLists[currentList]->Clear();

	// Remove the parent list
	outputLists.Clear();

	// Remove the list of associated cars
	associatedCars.Clear();
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
bool Iteration::PerformSaveToFile(void)
{
	// Open the specified file
	std::ofstream outFile(pathAndFileName.mb_str(), ios::out | ios::binary);

	// Make sure the file was opened OK
	if (!outFile.is_open() || !outFile.good())
		return false;

	// Write the file header information
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
	bool temp(plotPanel->GetRenderer()->GetGridOn());
	outFile.write((char*)&temp, sizeof(bool));

	// Close the file
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
bool Iteration::PerformLoadFromFile(void)
{
	// Open the specified file
	std::ifstream inFile(pathAndFileName.mb_str(), ios::in | ios::binary);

	// Make sure the file was opened OK
	if (!inFile.is_open() || !inFile.good())
		return false;

	// Read the file header information
	FileHeaderInfo header = ReadFileHeader(&inFile);

	// Check to make sure the version matches
	if (header.fileVersion > currentFileVersion)
	{
		Debugger::GetInstance().Print(_T("ERROR:  Unrecognized file version - unable to open file!"));
		inFile.close();

		return false;
	}
	else if (header.fileVersion != currentFileVersion)
		Debugger::GetInstance().Print(_T("Warning:  Opening out-of-date file version."));

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
		plotPanel->GetRenderer()->SetGridOn();
	else
		plotPanel->GetRenderer()->SetGridOff();

	// Close the file
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
void Iteration::ReadDefaultsFromConfig(void)
{
	// Create a configuration file object
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
	configurationFile->Read(_T("/Iteration/ShowGridLines"), &showGridLines, false);

	configurationFile->Read(_T("/Iteration/StartPitch"), &range.startPitch, 0.0);
	configurationFile->Read(_T("/Iteration/StartRoll"), &range.startRoll, 0.0);
	configurationFile->Read(_T("/Iteration/StartHeave"), &range.startHeave, 0.0);
	configurationFile->Read(_T("/Iteration/StartRackTravel"), &range.startRackTravel, 0.0);

	configurationFile->Read(_T("/Iteration/EndPitch"), &range.endPitch, 0.0);
	configurationFile->Read(_T("/Iteration/EndRoll"), &range.endRoll, 0.0);
	configurationFile->Read(_T("/Iteration/EndHeave"), &range.endHeave, 0.0);
	configurationFile->Read(_T("/Iteration/EndRackTravel"), &range.endRackTravel, 0.0);

	configurationFile->Read(_T("/Iteration/NumberOfPoints"), &numberOfPoints, 10);

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

	// Delete file object
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
void Iteration::WriteDefaultsToConfig(void) const
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
void Iteration::ApplyPlotFormatting(void)
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

	plotPanel->GetRenderer()->SetGridOn(showGridLines);
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
void Iteration::SetRange(const Iteration::Range &_range)
{
	// Assign the passed range to the class member
	range = _range;

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
void Iteration::SetNumberOfPoints(const int &_numberOfPoints)
{
	// Make sure the value is at least 2, then make the assignment
	if (_numberOfPoints >= 2)
		numberOfPoints = _numberOfPoints;

	// Set the "this item has changed" flag
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
void Iteration::UpdateAutoAssociate(void)
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
void Iteration::ShowAssociatedCarsDialog(void)
{
	// Call the auto-associate update function
	UpdateAutoAssociate();

	// Set up the list of cars in mainFrame
	wxArrayString choices;
	ObjectList<GuiCar> openCars;
	int i;
	for (i = 0; i < mainFrame.GetObjectCount(); i++)
	{
		// If the object is a car, add the name to our list of choices, and
		// add the car itself to our list of cars
		if (mainFrame.GetObjectByIndex(i)->GetType() == GuiObject::TypeCar)
		{
			choices.Add(mainFrame.GetObjectByIndex(i)->GetCleanName());
			openCars.Add(static_cast<GuiCar*>(mainFrame.GetObjectByIndex(i)));
		}
	}

	// Make sure there is at least one car open
	if (openCars.GetCount() == 0)
	{
		Debugger::GetInstance().Print(_T("ERROR:  Cannot display dialog - no open cars!"), Debugger::PriorityHigh);
		return;
	}

	// Initialize the selections - this array contains the indeces of the
	// items that are selected
    wxArrayInt selections;
	for (i = 0; i < openCars.GetCount(); i++)
	{
		// If the item is associated with this iteration, add its index to the array
		if (AssociatedWithCar(openCars[i]))
			selections.Add(i);
	}

	// Display the dialog
	bool selectionsMade = wxGetMultipleChoices(selections, _T("Select the cars to associate with this iteration:"),
		_T("Associated Cars"), choices, &mainFrame) > 0;

	// Check to make sure the user didn't cancel
	if (!selectionsMade)
	{
		openCars.Clear();
		return;
	}

	// Remove all items from our lists
	ClearAllLists();

	// Go through the indices that were selected and add them to our list
	for (i = 0; i < (signed int)selections.GetCount(); i++)
		AddCar(openCars.GetObject(selections[i]));

	// If not all of the cars in the list are selected, make sure we're not auto-associating
	if (openCars.GetCount() != associatedCars.GetCount())
		associatedWithAllOpenCars = false;

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
	int i;
	for (i = 0; i < associatedCars.GetCount(); i++)
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
	// The value to return
	double value = 0.0;
	Vector temp;

	// Make sure the arguments are valid
	if (associatedCarIndex >= associatedCars.GetCount() ||
		point > numberOfPoints || id > NumberOfPlots)
		return value;

	// Depending on the specified PLOT_ID, choose which member of the KINEMATIC_OUTPUTS
	// object to return
	if (id < Pitch)
		value = Convert::GetInstance().ConvertTo(outputLists[associatedCarIndex]->GetObject(point)->GetOutputValue(
			(KinematicOutputs::OutputsComplete)id), KinematicOutputs::GetOutputUnitType(
			(KinematicOutputs::OutputsComplete)id));
	else if (id == Pitch)
		value = Convert::GetInstance().ConvertAngle(axisValuesPitch[point]);
	else if (id == Roll)
		value = Convert::GetInstance().ConvertAngle(axisValuesRoll[point]);
	else if (id == Heave)
		value = Convert::GetInstance().ConvertDistance(axisValuesHeave[point]);
	else if (id == RackTravel)
		value = Convert::GetInstance().ConvertDistance(axisValuesRackTravel[point]);
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
		// Tab delimited
		delimiter = '\t';
	else if (extension.Cmp(_T("csv")) == 0)
		// Comma separated values
		delimiter = ',';
	else
	{
		// Unrecognized file extension
		Debugger::GetInstance().Print(_T("ERROR:  Could not export data!  Unable to determine delimiter choice!"));

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
		Debugger::GetInstance().Print(_T("ERROR:  Could not export data to '") + pathAndFileName + _T("'!"));

		return;
	}

	// Write the information
	// The column headings consist of three rows:  Plot Name, Units, Car Name
	// After the third row, we start writing the data
	int currentCar, currentPlot, row;
	int numberOfHeadingRows = 3;
	for (row = 0; row < numberOfPoints + numberOfHeadingRows; row++)
	{
		for (currentCar = 0; currentCar < associatedCars.GetCount(); currentCar++)
		{
			for (currentPlot = 0; currentPlot < NumberOfPlots; currentPlot++)
			{
				if (!plotActive[currentPlot] && currentPlot != (int)xAxisType + KinematicOutputs::NumberOfOutputScalars)
					continue;

				if (row == 0)
					// Write the name of the current column
					exportFile << GetPlotName((PlotID)currentPlot) << delimiter;
				else if (row == 1)
					// Write the units for this column
					exportFile << "(" << GetPlotUnits((PlotID)currentPlot) << ")" << delimiter;
				else if (row == 2)
					// Write the name of the current car
					exportFile << associatedCars[currentCar]->GetCleanName() << delimiter;
				else
					// Write the data for the current plot
					exportFile << GetDataValue(currentCar, row - numberOfHeadingRows, (PlotID)currentPlot)
						<< delimiter;
			}
		}

		// Add a new line at the end of every row
		exportFile << endl;
	}

	// Close the file
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
	// The value to return
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
		units = Convert::GetInstance().GetUnitType(
			KinematicOutputs::GetOutputUnitType((KinematicOutputs::OutputsComplete)id));
	else if (id == Pitch || id == Roll)
		units = Convert::GetInstance().GetUnitType(Convert::UnitTypeAngle);
	else if (id == Heave || id == RackTravel)
		units = Convert::GetInstance().GetUnitType(Convert::UnitTypeDistance);
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
//		_xAxisType	= AxisType to plot against
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Iteration::SetXAxisType(AxisType _xAxisType)
{
	// Set the xAxisType
	xAxisType = _xAxisType;

	// Set the "this item has changed" flag
	SetModified();

	// Update the iteration
	UpdateDisplay();
}

//==========================================================================
// Class:			Iteration
// Function:		SetYAxisType
//
// Description:		Sets the Y axis to the specified quantity.
//
// Input Arguments:
//		_yAxisType	= AxisType to plot against
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Iteration::SetYAxisType(AxisType _yAxisType)
{
	// Set the yAxisType
	yAxisType = _yAxisType;

	// Set the "this item has changed" flag
	SetModified();

	// Update the iteration
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
	// Update the auto-associate flag
	associatedWithAllOpenCars = autoAssociate;

	// Set the "this item has changed" flag
	SetModified();

	// Update this object
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
void Iteration::MarkAnalysisComplete(void)
{
	assert(pendingAnalysisCount > 0);
	
	// Decrement the counter
	pendingAnalysisCount--;

	// If the current count is zero, update the display
	if (pendingAnalysisCount == 0)
		UpdateDisplay();
}