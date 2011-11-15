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
//	3/11/2009	- Finished implementation of enum/array style data members for KINEMATIC_OUTPUTS
//				  class, K. Loux.
//	5/19/2009	- Changed to derived class from GUI_OBJECT, K. Loux.
//	10/18/2010	- Fixed bugs causing crash when new ITERATION is cancelled during creation, K. Loux.
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
// Class:			ITERATION
// Function:		ITERATION
//
// Description:		Constructor for the ITERATION class.
//
// Input Arguments:
//		_MainFrame			= MAIN_FRAME& reference to main application window
//		_debugger			= const Debugger& reference to the application's debug
//							  message printing utility
//		_PathAndFileName	= wxString containing the location of this object on
//							  the hard disk
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ITERATION::ITERATION(MAIN_FRAME &_MainFrame, const Debugger &_debugger,
					 wxString _PathAndFileName)
					 : GUI_OBJECT(_MainFrame, _debugger, _PathAndFileName),
					 Converter(_MainFrame.GetConverter())
{
	// Initialize our list of plots to OFF
	int i;
	for (i = 0; i < NumberOfPlots; i++)
		PlotActive[i] = false;

	// Read default settings from config file
	// Do this prior to initialization so saved files overwrite these defaults
	ReadDefaultsFromConfig();

	// Initialize the pointers to the X-axis data
	AxisValuesPitch = NULL;
	AxisValuesRoll = NULL;
	AxisValuesHeave = NULL;
	AxisValuesRackTravel = NULL;

	// Unless we know anything different, assume we want to associate ourselves with
	// all of the open car objects
	AssociatedWithAllOpenCars = true;

	// Initialize our "are we caught up" flags
	AnalysesDisplayed = true;
	SecondAnalysisPending = false;

	// Initialize the working car variables
	WorkingCarArray = NULL;
	NumberOfWorkingCars = 0;

	// Create the renderer
	plotPanel = new PlotPanel(dynamic_cast<wxWindow*>(&MainFrame), debugger);
	notebookTab = dynamic_cast<wxWindow*>(plotPanel);

	// Get an index for this item and add it to the list in the MainFrame
	// MUST be included BEFORE the naming, which must come BEFORE the call to Initialize
	Index = MainFrame.AddObjectToList(this);

	// Create the name based on the index
	Name.Printf("Unsaved Iteration %i", Index + 1);

	// Complete initialization of this object
	Initialize();
}

//==========================================================================
// Class:			ITERATION
// Function:		~ITERATION
//
// Description:		Destructor for the ITERATION class.
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
ITERATION::~ITERATION()
{
	// Clear out the lists
	ClearAllLists();

	// Delete the X-axis values
	delete [] AxisValuesPitch;
	AxisValuesPitch = NULL;

	delete [] AxisValuesRoll;
	AxisValuesRoll = NULL;

	delete [] AxisValuesHeave;
	AxisValuesHeave = NULL;

	delete [] AxisValuesRackTravel;
	AxisValuesRackTravel = NULL;

	// Delete the working car array
	int i;
	for (i = 0; i < NumberOfWorkingCars; i++)
	{
		delete WorkingCarArray[i];
		WorkingCarArray[i] = NULL;
	}
	delete [] WorkingCarArray;
	WorkingCarArray = NULL;
}

//==========================================================================
// Class:			ITERATION
// Function:		Constant declarations
//
// Description:		Where constants for the ITERATION class are delcared.
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
//const int ITERATION::CurrentFileVersion = 0;// OBSOLETE 11/17/2010 - Added alternative title and axis labels, etc.
const int ITERATION::CurrentFileVersion = 1;

//==========================================================================
// Class:			ITERATION
// Function:		AddCar
//
// Description:		Adds the passed car to the list for the analysis.
//
// Input Arguments:
//		ToAdd	= GUI_CAR* pointing to the car to add
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ITERATION::AddCar(GUI_CAR *ToAdd)
{
	// Make sure we have a valid pointer
	if (ToAdd == NULL)
		return;

	// Add the car to the list
	AssociatedCars.Add(ToAdd);

	return;
}

//==========================================================================
// Class:			ITERATION
// Function:		RemoveCar
//
// Description:		Removes the car at the passed address from the analysis.
//
// Input Arguments:
//		ToRemove	= GUI_CAR* pointing to the car to be removed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ITERATION::RemoveCar(GUI_CAR *ToRemove)
{
	// Make sure we have a valid pointer
	if (ToRemove == NULL)
		return;

	// Find the car to be removed
	int IndexToRemove;
	for (IndexToRemove = 0; IndexToRemove < AssociatedCars.GetCount(); IndexToRemove++)
	{
		// Check to see if the pointer in the argument matches the pointer
		// in the list
		if (AssociatedCars[IndexToRemove] == ToRemove)
			break;
	}

	// Make sure we found a car
	if (IndexToRemove == AssociatedCars.GetCount())
		return;

	// Remove the outputs associated with the car
	OutputLists[IndexToRemove]->Clear();
	OutputLists.Remove(IndexToRemove);

	// Remove the car itself
	AssociatedCars.Remove(IndexToRemove);

	return;
}

//==========================================================================
// Class:			ITERATION
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
void ITERATION::RemoveAllCars(void)
{
	// Remove all entries from the lists
	ClearAllLists();

	return;
}

//==========================================================================
// Class:			ITERATION
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
int ITERATION::GetIconHandle(void) const
{
	// Return the proper icon handle
	return SystemsTree->GetIconHandle(MAIN_TREE::IterationIcon);
}

//==========================================================================
// Class:			ITERATION
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
void ITERATION::UpdateData(void)
{
	// Make sure we are in a state from which we can handle this analysis
	if (!AnalysesDisplayed)
	{
		// If we are not in an acceptable state, toggle the flag indicating that we
		// need an update, but do not proceed any further.
		SecondAnalysisPending = true;
		return;
	}

	// Reset the flags controlling access to this method
	AnalysesDisplayed = false;
	SecondAnalysisPending = false;

	// Make sure the semaphore is cleared (this should be redundant,
	// since we have the flags above, but is kept just in case)
	while (InverseSemaphore.GetCount() > 0)
	{
		// Just wait for the threads to finish up
		wxSafeYield();
		wxMilliSleep(50);
	}

	// Call the UpdateAutoAssociate method
	UpdateAutoAssociate();

	// If we don't have any associated cars, then don't do anything
	if (AssociatedCars.GetCount() == 0)
	{
		// Just update the plot to make it look pretty
		UpdateDisplay();

		return;
	}

	// Get the original values for the kinematic inputs that we will be changing
	KINEMATICS::INPUTS KinematicInputs;

	// Determine the step size for each input
	double PitchStep	= (Range.EndPitch - Range.StartPitch) / (NumberOfPoints - 1);// [rad]
	double RollStep		= (Range.EndRoll - Range.StartRoll) / (NumberOfPoints - 1);// [rad]
	double HeaveStep	= (Range.EndHeave - Range.StartHeave) / (NumberOfPoints - 1);// [in]
	double RackStep		= (Range.EndRackTravel - Range.StartRackTravel) / (NumberOfPoints - 1);// [in]

	// Delete the X-axis variables
	delete [] AxisValuesPitch;
	delete [] AxisValuesRoll;
	delete [] AxisValuesHeave;
	delete [] AxisValuesRackTravel;

	// Determine the number of points required to store data in the case of 3D plots
	int TotalPoints = NumberOfPoints;
	if (YAxisType != AxisTypeUnused)
		TotalPoints *= NumberOfPoints;

	// Re-create the arrays with the appropriate number of points
	AxisValuesPitch			= new double[TotalPoints];
	AxisValuesRoll			= new double[TotalPoints];
	AxisValuesHeave			= new double[TotalPoints];
	AxisValuesRackTravel	= new double[TotalPoints];

	// Clear out and re-allocate our output lists
	int i;
	for (i = 0; i < OutputLists.GetCount(); i++)
		OutputLists[i]->Clear();
	OutputLists.Clear();

	// Initialize the semaphore count
	InverseSemaphore.Set(AssociatedCars.GetCount() * TotalPoints);

	// Make sure the working cars are initialized
	if (InverseSemaphore.GetCount() != (unsigned int)NumberOfWorkingCars)
	{
		for (i = 0; i < NumberOfWorkingCars; i++)
		{
			delete WorkingCarArray[i];
			WorkingCarArray[i] = NULL;
		}
		delete [] WorkingCarArray;
		NumberOfWorkingCars = InverseSemaphore.GetCount();
		WorkingCarArray = new CAR*[NumberOfWorkingCars];
		for (i = 0; i < NumberOfWorkingCars; i++)
			WorkingCarArray[i] = new CAR(debugger);
	}

	// Go through car-by-car
	int CurrentCar, CurrentPoint;
	for (CurrentCar = 0; CurrentCar < AssociatedCars.GetCount(); CurrentCar++)
	{
		// Create a list to store the outputs for this car
		ManagedList<KINEMATIC_OUTPUTS> *CurrentList = new ManagedList<KINEMATIC_OUTPUTS>;

		// Add this list to our list of lists (bit confusing?)
		OutputLists.Add(CurrentList);

		// Run the analysis for each point through the range
		for (CurrentPoint = 0; CurrentPoint < TotalPoints; CurrentPoint++)
		{
			// Add the current position of the car to the array for the plot's X-axis
			if (YAxisType == AxisTypeUnused)
			{
				AxisValuesPitch[CurrentPoint]		= Range.StartPitch + PitchStep * CurrentPoint;
				AxisValuesRoll[CurrentPoint]		= Range.StartRoll + RollStep * CurrentPoint;
				AxisValuesHeave[CurrentPoint]		= Range.StartHeave + HeaveStep * CurrentPoint;
				AxisValuesRackTravel[CurrentPoint]	= Range.StartRackTravel + RackStep * CurrentPoint;
			}
			else
			{
				// If we also have a Y-axis, we need to be more careful when determining the axis values
				// FIXME:  I think this is for 3D plotting?
			}

			// Assign the inputs
			KinematicInputs.Pitch = AxisValuesPitch[CurrentPoint];
			KinematicInputs.Roll = AxisValuesRoll[CurrentPoint];
			KinematicInputs.Heave = AxisValuesHeave[CurrentPoint];
			KinematicInputs.RackTravel = AxisValuesRackTravel[CurrentPoint];
			KinematicInputs.FirstRotation = MainFrame.GetInputs().FirstRotation;
			KinematicInputs.CenterOfRotation = MainFrame.GetInputs().CenterOfRotation;

			// Run The analysis
			KINEMATIC_OUTPUTS *NewOutputs = new KINEMATIC_OUTPUTS;
			KINEMATICS_DATA *Data = new KINEMATICS_DATA(&AssociatedCars[CurrentCar]->GetOriginalCar(),
				WorkingCarArray[CurrentCar * NumberOfPoints + CurrentPoint], KinematicInputs, NewOutputs);
			THREAD_JOB Job(THREAD_JOB::COMMAND_THREAD_KINEMATICS_ITERATION, Data,
				AssociatedCars[CurrentCar]->GetCleanName() + _T(":") + Name, Index);
			MainFrame.AddJob(Job);

			// Add the outputs to the iteration's list
			CurrentList->Add(NewOutputs);
		}
	}

	return;
}

//==========================================================================
// Class:			ITERATION
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
void ITERATION::UpdateDisplay(void)
{
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
			// Set the x-axis information if this is the first pass
			if (i == 0)
			{
				// FIXME:  Implement
			}

			// Check to see if this plot is active
			if (PlotActive[i])
			{
				for (j = 0; j < (unsigned int)AssociatedCars.GetCount(); j++)
				{
					// Create the dataset
					dataSet = new Dataset2D(NumberOfPoints);
					x = dataSet->GetXPointer();
					y = dataSet->GetYPointer();
					
					// Populate all values
					for (k = 0; k < (unsigned int)NumberOfPoints; k++)
					{
						x[k] = GetDataValue(j, k,
								(PLOT_ID)(KINEMATIC_OUTPUTS::NumberOfOutputScalars + XAxisType));
						y[k] = GetDataValue(j, k, (PLOT_ID)i);
					}
					
					// Add the dataset to the plot
					plotPanel->AddCurve(dataSet, AssociatedCars[j]->GetCleanName()
						+ _T(", ") + GetPlotName((PLOT_ID)i));
				}
			}
		}

		// Apply formatting
		ApplyPlotFormatting();

		// Update the display associated with this object
		plotPanel->UpdateDisplay();
	}

	// Reset the "are we caught up?" flag
	AnalysesDisplayed = true;

	// If we have a second analysis pending, handle it now
	if (SecondAnalysisPending)
		UpdateData();

	return;
}

//==========================================================================
// Class:			ITERATION
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
void ITERATION::ClearAllLists(void)
{
	// Clear out all of our lists
	int CurrentList;
	for (CurrentList = 0; CurrentList < OutputLists.GetCount(); CurrentList++)
		// Remove all of the sub-lists
		OutputLists[CurrentList]->Clear();

	// Remove the parent list
	OutputLists.Clear();

	// Remove the list of associated cars
	AssociatedCars.Clear();

	return;
}

//==========================================================================
// Class:			ITERATION
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
bool ITERATION::PerformSaveToFile(void)
{
	// Open the specified file
	std::ofstream OutFile(PathAndFileName.c_str(), ios::out | ios::binary);

	// Make sure the file was opened OK
	if (!OutFile.is_open() || !OutFile.good())
		return false;

	// Write the file header information
	WriteFileHeader(&OutFile);

	// Write this object's data
	OutFile.write((char*)&AssociatedWithAllOpenCars, sizeof(bool));
	OutFile.write((char*)PlotActive, sizeof(bool) * NumberOfPlots);
	OutFile.write((char*)&NumberOfPoints, sizeof(int));
	OutFile.write((char*)&Range, sizeof(RANGE));
	OutFile.write((char*)&XAxisType, sizeof(AXIS_TYPE));

	// Added for file version 1 on 11/17/2010
	OutFile.write((char*)&YAxisType, sizeof(AXIS_TYPE));
	OutFile.write((char*)&GenerateTitleFromFileName, sizeof(bool));
	OutFile.write(Title.c_str(), (Title.Len() + 1) * sizeof(char));
	OutFile.write((char*)&AutoGenerateXLabel, sizeof(bool));
	OutFile.write(XLabel.c_str(), (XLabel.Len() + 1) * sizeof(char));
	OutFile.write((char*)&AutoGenerateZLabel, sizeof(bool));
	OutFile.write(ZLabel.c_str(), (ZLabel.Len() + 1) * sizeof(char));
	bool temp(plotPanel->GetRenderer()->GetGridOn());
	OutFile.write((char*)&temp, sizeof(bool));

	// Close the file
	OutFile.close();

	return true;
}

//==========================================================================
// Class:			ITERATION
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
bool ITERATION::PerformLoadFromFile(void)
{
	// Open the specified file
	std::ifstream InFile(PathAndFileName.c_str(), ios::in | ios::binary);

	// Make sure the file was opened OK
	if (!InFile.is_open() || !InFile.good())
		return false;

	// Read the file header information
	FILE_HEADER_INFO Header = ReadFileHeader(&InFile);

	// Check to make sure the version matches
	if (Header.FileVersion > CurrentFileVersion)
	{
		debugger.Print(_T("ERROR:  Unrecognized file version - unable to open file!"));
		InFile.close();

		return false;
	}
	else if (Header.FileVersion != CurrentFileVersion)
		debugger.Print(_T("Warning:  Opening out-of-date file version."));

	// Read this object's data
	InFile.read((char*)&AssociatedWithAllOpenCars, sizeof(bool));
	InFile.read((char*)PlotActive, sizeof(bool) * NumberOfPlots);
	InFile.read((char*)&NumberOfPoints, sizeof(int));
	InFile.read((char*)&Range, sizeof(RANGE));
	InFile.read((char*)&XAxisType, sizeof(AXIS_TYPE));

	// Stop here if we don't have file version 1 or newer; allow defaults to be used for
	// remaining parameters (as set in constructor)
	if (Header.FileVersion < 1)
	{
		InFile.close();
		return true;
	}

	// New as of file version 1
	InFile.read((char*)&YAxisType, sizeof(AXIS_TYPE));
	InFile.read((char*)&GenerateTitleFromFileName, sizeof(bool));
	char OneChar;
	Title.Empty();
	while (InFile.read(&OneChar, sizeof(char)), OneChar != '\0')
		Title.Append(OneChar);
	InFile.read((char*)&AutoGenerateXLabel, sizeof(bool));
	XLabel.Empty();
	while (InFile.read(&OneChar, sizeof(char)), OneChar != '\0')
		XLabel.Append(OneChar);
	InFile.read((char*)&AutoGenerateZLabel, sizeof(bool));
	ZLabel.Empty();
	while (InFile.read(&OneChar, sizeof(char)), OneChar != '\0')
		ZLabel.Append(OneChar);
	bool temp;
	InFile.read((char*)&temp, sizeof(bool));
	if (temp)
		plotPanel->GetRenderer()->SetGridOn();
	else
		plotPanel->GetRenderer()->SetGridOff();

	// Close the file
	InFile.close();

	return true;
}

//==========================================================================
// Class:			ITERATION
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
void ITERATION::ReadDefaultsFromConfig(void)
{
	// Create a configuration file object
	wxFileConfig *ConfigurationFile = new wxFileConfig(_T(""), _T(""),
		wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() + MainFrame.PathToConfigFile, _T(""),
		wxCONFIG_USE_RELATIVE_PATH);

	// Attempt to read from the config file; set the defaults if the value isn't found
	ConfigurationFile->Read(_T("/Iteration/GenerateTitleFromFileName"), &GenerateTitleFromFileName, true);
	ConfigurationFile->Read(_T("/Iteration/Title"), &Title, wxEmptyString);
	ConfigurationFile->Read(_T("/Iteration/AutoGenerateXLabel"), &AutoGenerateXLabel, true);
	ConfigurationFile->Read(_T("/Iteration/XLabel"), &XLabel, wxEmptyString);
	ConfigurationFile->Read(_T("/Iteration/AutoGenerateZLabel"), &AutoGenerateZLabel, true);
	ConfigurationFile->Read(_T("/Iteration/ZLabel"), &ZLabel, wxEmptyString);
	ConfigurationFile->Read(_T("/Iteration/ShowGridLines"), &ShowGridLines, false);

	ConfigurationFile->Read(_T("/Iteration/StartPitch"), &Range.StartPitch, 0.0);
	ConfigurationFile->Read(_T("/Iteration/StartRoll"), &Range.StartRoll, 0.0);
	ConfigurationFile->Read(_T("/Iteration/StartHeave"), &Range.StartHeave, 0.0);
	ConfigurationFile->Read(_T("/Iteration/StartRackTravel"), &Range.StartRackTravel, 0.0);

	ConfigurationFile->Read(_T("/Iteration/EndPitch"), &Range.EndPitch, 0.0);
	ConfigurationFile->Read(_T("/Iteration/EndRoll"), &Range.EndRoll, 0.0);
	ConfigurationFile->Read(_T("/Iteration/EndHeave"), &Range.EndHeave, 0.0);
	ConfigurationFile->Read(_T("/Iteration/EndRackTravel"), &Range.EndRackTravel, 0.0);

	ConfigurationFile->Read(_T("/Iteration/NumberOfPoints"), &NumberOfPoints, 10);

	ConfigurationFile->Read(_T("/Iteration/XAxisType"), (int*)&XAxisType, (int)AxisTypeUnused);
	ConfigurationFile->Read(_T("/Iteration/YAxisType"), (int*)&YAxisType, (int)AxisTypeUnused);

	wxString ActivePlotString;
	ConfigurationFile->Read(_T("Iteration/ActivePlots"), &ActivePlotString, wxEmptyString);

	// Process the empty plot string to turn it into booleans
	if (ActivePlotString.Len() > 0)
	{
		size_t i(0), lasti(0);
		unsigned long PlotIndex;
		while (i = ActivePlotString.find(_T(';'), i + 1), i != std::string::npos)
		{
			// Convert the string to a number
			if (ActivePlotString.SubString(lasti, i - 1).ToULong(&PlotIndex))
			{
				// Make sure the number isn't greater than the size of the array
				if (PlotIndex < NumberOfPlots)
					PlotActive[PlotIndex] = true;
			}

			lasti = i + 1;
		}
	}

	// Delete file object
	delete ConfigurationFile;
	ConfigurationFile = NULL;

	return;
}

//==========================================================================
// Class:			ITERATION
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
void ITERATION::WriteDefaultsToConfig(void) const
{
	// Create a configuration file object
	wxFileConfig *ConfigurationFile = new wxFileConfig(_T(""), _T(""),
		wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() + MainFrame.PathToConfigFile, _T(""),
		wxCONFIG_USE_RELATIVE_PATH);

	// Write to the config file
	ConfigurationFile->Write(_T("/Iteration/GenerateTitleFromFileName"), GenerateTitleFromFileName);
	ConfigurationFile->Write(_T("/Iteration/Title"), Title);
	ConfigurationFile->Write(_T("/Iteration/AutoGenerateXLabel"), AutoGenerateXLabel);
	ConfigurationFile->Write(_T("/Iteration/XLabel"), XLabel);
	ConfigurationFile->Write(_T("/Iteration/AutoGenerateZLabel"), AutoGenerateZLabel);
	ConfigurationFile->Write(_T("/Iteration/ZLabel"), ZLabel);
	ConfigurationFile->Write(_T("/Iteration/ShowGridLines"), ShowGridLines);

	ConfigurationFile->Write(_T("/Iteration/StartPitch"), Range.StartPitch);
	ConfigurationFile->Write(_T("/Iteration/StartRoll"), Range.StartRoll);
	ConfigurationFile->Write(_T("/Iteration/StartHeave"), Range.StartHeave);
	ConfigurationFile->Write(_T("/Iteration/StartRackTravel"), Range.StartRackTravel);

	ConfigurationFile->Write(_T("/Iteration/EndPitch"), Range.EndPitch);
	ConfigurationFile->Write(_T("/Iteration/EndRoll"), Range.EndRoll);
	ConfigurationFile->Write(_T("/Iteration/EndHeave"), Range.EndHeave);
	ConfigurationFile->Write(_T("/Iteration/EndRackTravel"), Range.EndRackTravel);

	ConfigurationFile->Write(_T("/Iteration/NumberOfPoints"), NumberOfPoints);

	ConfigurationFile->Write(_T("/Iteration/XAxisType"), (int)XAxisType);
	ConfigurationFile->Write(_T("/Iteration/YAxisType"), (int)YAxisType);

	// Encode the active plots into a string that can be saved into the configuration file
	wxString ActivePlotString(wxEmptyString), Temp;
	int i;
	for (i = 0; i < NumberOfPlots; i++)
	{
		if (PlotActive[i])
		{
			Temp.Printf("%i;", i);
			ActivePlotString += Temp;
		}
	}
	ConfigurationFile->Write(_T("Iteration/ActivePlots"), ActivePlotString);

	// Delete file object
	delete ConfigurationFile;
	ConfigurationFile = NULL;

	return;
}

//==========================================================================
// Class:			ITERATION
// Function:		SetRange
//
// Description:		Sets this object's range to the specified values.
//
// Input Arguments:
//		_Range	= const ITERATION::RANGE& specifying the desired range
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ITERATION::ApplyPlotFormatting(void)
{
	if (AutoGenerateZLabel)
	{
		unsigned int i;
		wxString label;
		for (i = 0; i < NumberOfPlots; i++)
		{
			if (PlotActive[i])
			{
				if (label.IsEmpty())
					label.assign(GetPlotName((PLOT_ID)i) + _T(" [") + GetPlotUnits((PLOT_ID)i) + _T("]"));
				else
					label.assign(_T("Multipl Values"));
			}
		}

		plotPanel->GetRenderer()->SetLeftYLabel(label);
	}
	else
		plotPanel->GetRenderer()->SetLeftYLabel(ZLabel);

	// FIXME:  Implement dual labels
	//plotPanel->GetRenderer()->SetRightYLabel();

	if (AutoGenerateXLabel)
		plotPanel->GetRenderer()->SetXLabel(
		GetPlotName((PLOT_ID)(KINEMATIC_OUTPUTS::NumberOfOutputScalars + XAxisType)) + _T(" [")
		+ GetPlotUnits((PLOT_ID)(KINEMATIC_OUTPUTS::NumberOfOutputScalars + XAxisType)) + _T("]"));
	else
		plotPanel->GetRenderer()->SetXLabel(XLabel);

	if (AutoGenerateXLabel)
		plotPanel->GetRenderer()->SetTitle(GetCleanName());
	else
		plotPanel->GetRenderer()->SetTitle(Title);

	plotPanel->GetRenderer()->SetGridOn(ShowGridLines);
}

//==========================================================================
// Class:			ITERATION
// Function:		SetRange
//
// Description:		Sets this object's range to the specified values.
//
// Input Arguments:
//		_Range	= const ITERATION::RANGE& specifying the desired range
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ITERATION::SetRange(const ITERATION::RANGE &_Range)
{
	// Assign the passed range to the class member
	Range = _Range;

	// Make sure the chosen X-axis type is not for a parameter with zero range
	// This is based on the priority Roll->Steer->Heave->Pitch.
	if ((XAxisType == ITERATION::AxisTypeRoll && VVASEMath::IsZero(Range.StartRoll - Range.EndRoll)) ||
		(XAxisType == ITERATION::AxisTypeRackTravel && VVASEMath::IsZero(Range.StartRackTravel - Range.EndRackTravel)) ||
		(XAxisType == ITERATION::AxisTypeHeave && VVASEMath::IsZero(Range.StartHeave - Range.EndHeave)) ||
		(XAxisType == ITERATION::AxisTypePitch && VVASEMath::IsZero(Range.StartPitch - Range.EndPitch)) ||
		XAxisType == ITERATION::AxisTypeUnused)
	{
		if (fabs(Range.StartRoll - Range.EndRoll) > 0.0)
			XAxisType = AxisTypeRoll;
		else if (fabs(Range.StartRackTravel - Range.EndRackTravel) > 0.0)
			XAxisType = AxisTypeRackTravel;
		else if (fabs(Range.StartHeave - Range.EndHeave) > 0.0)
			XAxisType = AxisTypeHeave;
		else if (fabs(Range.StartPitch - Range.EndPitch) > 0.0)
			XAxisType = AxisTypePitch;
	}

	// FIXME:  Determine if also have a Y-axis!

	// Set the "this item has changed" flag
	SetModified();

	return;
}

//==========================================================================
// Class:			ITERATION
// Function:		SetNumberOfPoints
//
// Description:		Sets the number of points used to generate the plots for
//					this object.
//
// Input Arguments:
//		_NumberOfPoints	= const int& reference to number of points to use
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ITERATION::SetNumberOfPoints(const int &_NumberOfPoints)
{
	// Make sure the value is at least 2, then make the assignment
	if (_NumberOfPoints >= 2)
		NumberOfPoints = _NumberOfPoints;

	// Set the "this item has changed" flag
	SetModified();

	return;
}

//==========================================================================
// Class:			ITERATION
// Function:		SetActivePlot
//
// Description:		Sets the flag indicating whether the specified plot is
//					active or not.
//
// Input Arguments:
//		PlotID	=	ITERATION::PLOT_ID specifying the plot we're intersted in
//		Active	=	const bool& true for plot is shown, false otherwise
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ITERATION::SetActivePlot(PLOT_ID PlotID, const bool &Active)
{
	// Make sure the plot ID is valid
	assert(PlotID < NumberOfPlots);

	// Set the status for the specified plot
	PlotActive[PlotID] = Active;

	// Sets the "this object has changed" flag
	SetModified();

	return;
}

//==========================================================================
// Class:			ITERATION
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
void ITERATION::UpdateAutoAssociate(void)
{
	// Make sure auto-associate is on
	if (!AssociatedWithAllOpenCars)
		return;

	// Clear our list of cars
	ClearAllLists();

	// Go through the list of open objects in MainFrame, and add all TYPE_CAR objects
	// to the list
	int i;
	for (i = 0; i < MainFrame.GetObjectCount(); i++)
	{
		if (MainFrame.GetObjectByIndex(i)->GetType() == GUI_OBJECT::TYPE_CAR)
			AddCar(static_cast<GUI_CAR*>(MainFrame.GetObjectByIndex(i)));
	}

	return;
}

//==========================================================================
// Class:			ITERATION
// Function:		ShowAssociatedCarsDialog
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
void ITERATION::ShowAssociatedCarsDialog(void)
{
	// Call the auto-associate update function
	UpdateAutoAssociate();

	// Set up the list of cars in MainFrame
	wxArrayString Choices;
	ObjectList<GUI_CAR> OpenCars;
	int i;
	for (i = 0; i < MainFrame.GetObjectCount(); i++)
	{
		// If the object is a car, add the name to our list of choices, and
		// add the car itself to our list of cars
		if (MainFrame.GetObjectByIndex(i)->GetType() == GUI_OBJECT::TYPE_CAR)
		{
			Choices.Add(MainFrame.GetObjectByIndex(i)->GetCleanName());
			OpenCars.Add(static_cast<GUI_CAR*>(MainFrame.GetObjectByIndex(i)));
		}
	}

	// Make sure there is at least one car open
	if (OpenCars.GetCount() == 0)
	{
		debugger.Print(_T("ERROR:  Cannot display dialog - no open cars!"), Debugger::PriorityHigh);
		return;
	}

	// Initialize the selections - this array contains the indeces of the
	// items that are selected
    wxArrayInt Selections;
	for (i = 0; i < OpenCars.GetCount(); i++)
	{
		// If the item is associated with this iteration, add its index to the array
		if (AssociatedWithCar(OpenCars[i]))
			Selections.Add(i);
	}

	// Display the dialog
	bool SelectionsMade = wxGetMultipleChoices(Selections, _T("Select the cars to associate with this iteration:"),
		_T("Associated Cars"), Choices, &MainFrame);

	// Check to make sure the user didn't cancel
	if (!SelectionsMade)
	{
		OpenCars.Clear();
		return;
	}

	// Remove all items from our lists
	ClearAllLists();

	// Go through the indices that were selected and add them to our list
	for (i = 0; i < (signed int)Selections.GetCount(); i++)
		AddCar(OpenCars.GetObject(Selections[i]));

	// If not all of the cars in the list are selected, make sure we're not auto-associating
	if (OpenCars.GetCount() != AssociatedCars.GetCount())
		AssociatedWithAllOpenCars = false;

	// Update the analyses
	UpdateData();

	return;
}

//==========================================================================
// Class:			ITERATION
// Function:		AssociatedWithCar
//
// Description:		Checks to see if the specified car is associated with
//					this object.
//
// Input Arguments:
//		Test	= GUI_CAR* with which we are comparing our list
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument matches a car in our list
//
//==========================================================================
bool ITERATION::AssociatedWithCar(GUI_CAR *Test) const
{
	// Go through our list of associated cars, and see if Test points to any of
	// those cars
	int i;
	for (i = 0; i < AssociatedCars.GetCount(); i++)
	{
		if (AssociatedCars[i] == Test)
			return true;
	}

	return false;
}

//==========================================================================
// Class:			ITERATION
// Function:		GetDataValue
//
// Description:		Accesses the output data for the specified car and the
//					specified curve.
//
// Input Arguments:
//		AssociatedCarIndex	= integer specifying the car whose output is requested
//		Point				= integer specifying the point whose output is requested
//		Id					= PLOT_ID specifying what data we are interested in
//
// Output Arguments:
//		None
//
// Return Value:
//		double, the value of the requested point, converted to user units
//
//==========================================================================
double ITERATION::GetDataValue(int AssociatedCarIndex, int Point, PLOT_ID Id) const
{
	// The value to return
	double Value = 0.0;
	Vector Temp;

	// Make sure the arguments are valid
	if (AssociatedCarIndex >= AssociatedCars.GetCount() ||
		Point > NumberOfPoints || Id > NumberOfPlots)
		return Value;

	// Depending on the specified PLOT_ID, choose which member of the KINEMATIC_OUTPUTS
	// object to return
	if (Id < Pitch)
		Value = Converter.ConvertTo(OutputLists[AssociatedCarIndex]->GetObject(Point)->GetOutputValue(
			(KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)Id), KINEMATIC_OUTPUTS::GetOutputUnitType(
			(KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)Id));
	else if (Id == Pitch)
		Value = Converter.ConvertAngle(AxisValuesPitch[Point]);
	else if (Id == Roll)
		Value = Converter.ConvertAngle(AxisValuesRoll[Point]);
	else if (Id == Heave)
		Value = Converter.ConvertDistance(AxisValuesHeave[Point]);
	else if (Id == RackTravel)
		Value = Converter.ConvertDistance(AxisValuesRackTravel[Point]);
	else
		Value = 0.0;

	return Value;
}

//==========================================================================
// Class:			ITERATION
// Function:		ExportDataToFile
//
// Description:		Exports the data for this object to a comma or tab-delimited
//					text file.
//
// Input Arguments:
//		PathAndFileName	= wxString pointing to the location where the file is
//						  to be saved
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ITERATION::ExportDataToFile(wxString PathAndFileName) const
{
	// Determine what type of delimiter to use
	wxString Extension(PathAndFileName.substr(PathAndFileName.find_last_of('.') + 1));
	wxChar Delimiter;
	if (Extension.Cmp(_T("txt")) == 0)
		// Tab delimited
		Delimiter = '\t';
	else if (Extension.Cmp(_T("csv")) == 0)
		// Comma separated values
		Delimiter = ',';
	else
	{
		// Unrecognized file extension
		debugger.Print(_T("ERROR:  Could not export data!  Unable to determine delimiter choice!"));

		return;
	}

	// Perform the save - open the file
	ofstream ExportFile(PathAndFileName.c_str(), ios::out);

	// Warn the user if the file could not be opened failed
	if (!ExportFile.is_open() || !ExportFile.good())
	{
		debugger.Print(_T("ERROR:  Could not export data to '") + PathAndFileName + _T("'!"));

		return;
	}

	// Write the information
	// The column headings consist of three rows:  Plot Name, Units, Car Name
	// After the third row, we start writing the data
	int CurrentCar, CurrentPlot, Row;
	int NumberOfHeadingRows = 3;
	for (Row = 0; Row < NumberOfPoints + NumberOfHeadingRows; Row++)
	{
		for (CurrentCar = 0; CurrentCar < AssociatedCars.GetCount(); CurrentCar++)
		{
			for (CurrentPlot = 0; CurrentPlot < NumberOfPlots; CurrentPlot++)
			{
				if (Row == 0)
					// Write the name of the current column
					ExportFile << GetPlotName((PLOT_ID)CurrentPlot) << Delimiter;
				else if (Row == 1)
					// Write the units for this column
					ExportFile << "(" << GetPlotUnits((PLOT_ID)CurrentPlot) << ")" << Delimiter;
				else if (Row == 2)
					// Write the name of the current car
					ExportFile << AssociatedCars[CurrentCar]->GetCleanName() << Delimiter;
				else
					// Write the data for the current plot
					ExportFile << GetDataValue(CurrentCar, Row - NumberOfHeadingRows, (PLOT_ID)CurrentPlot)
						<< Delimiter;
			}
		}

		// Add a new line at the end of every row
		ExportFile << endl;
	}

	// Close the file
	ExportFile.close();

	return;
}

//==========================================================================
// Class:			ITERATION
// Function:		GetPlotName
//
// Description:		Returns a string containing the name of the specified plot.
//
// Input Arguments:
//		Id	= PLOT_ID specifying the plot we are interested in
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the plot
//
//==========================================================================
wxString ITERATION::GetPlotName(PLOT_ID Id) const
{
	// The value to return
	wxString Name;

	// Depending on the specified PLOT_ID, choose the name of the string
	// Vectors are a special case - depending on which component of the vector is chosen,
	// we need to append a different string to the end of the Name
	if (Id < Pitch)
		Name = KINEMATIC_OUTPUTS::GetOutputName((KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)Id);
	else if (Id == Pitch)
		Name = _T("Pitch");
	else if (Id == Roll)
		Name = _T("Roll");
	else if (Id == Heave)
		Name = _T("Heave");
	else if (Id == RackTravel)
		Name = _T("Rack Travel");
	else
		Name = _T("Unrecognized name");

	return Name;
}

//==========================================================================
// Class:			ITERATION
// Function:		GetPlotUnits
//
// Description:		Returns a string containing the units of the specified plot.
//
// Input Arguments:
//		Id	= PLOT_ID specifying the plot we are interested in
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the units of the plot
//
//==========================================================================
wxString ITERATION::GetPlotUnits(PLOT_ID Id) const
{
	// The value to return
	wxString Units;

	// Depending on the specified PLOT_ID, choose the units string
	if (Id < Pitch)
		Units = Converter.GetUnitType(KINEMATIC_OUTPUTS::GetOutputUnitType((KINEMATIC_OUTPUTS::OUTPUTS_COMPLETE)Id));
	else if (Id == Pitch || Id == Roll)
		Units = Converter.GetUnitType(Convert::UnitTypeAngle);
	else if (Id == Heave || Id == RackTravel)
		Units = Converter.GetUnitType(Convert::UnitTypeDistance);
	else
		Units = _T("Unrecognized units");

	return Units;
}

//==========================================================================
// Class:			ITERATION
// Function:		WriteFileHeader
//
// Description:		Writes the file header to the specified output stream.
//
// Input Arguments:
//		OutFile	= std::ofstream* to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ITERATION::WriteFileHeader(std::ofstream *OutFile)
{
	// Set up the header information
	FILE_HEADER_INFO Header;
	Header.FileVersion = CurrentFileVersion;

	// Set the write pointer to the start of the file
	OutFile->seekp(0);

	// Write the header
	OutFile->write((char*)&Header, sizeof(FILE_HEADER_INFO));

	return;
}

//==========================================================================
// Class:			ITERATION
// Function:		ReadFileHeader
//
// Description:		Reads the file header information from the specified input stream.
//
// Input Arguments:
//		InFile	= std::ifstream* to read from
//
// Output Arguments:
//		None
//
// Return Value:
//		FILE_HEADER_INFO containing the header information
//
//==========================================================================
ITERATION::FILE_HEADER_INFO ITERATION::ReadFileHeader(std::ifstream *InFile)
{
	// Set get pointer to the start of the file
	InFile->seekg(0);

	// Read the header struct
	char Buffer[sizeof(FILE_HEADER_INFO)];
	InFile->read((char*)Buffer, sizeof(FILE_HEADER_INFO));

	return *reinterpret_cast<FILE_HEADER_INFO*>(Buffer);
}

//==========================================================================
// Class:			ITERATION
// Function:		SetXAxisType
//
// Description:		Sets the X axis to the specified quantity.
//
// Input Arguments:
//		_XAxisType	= AXIS_TYPE to plot against
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ITERATION::SetXAxisType(AXIS_TYPE _XAxisType)
{
	// Set the XAxisType
	XAxisType = _XAxisType;

	// Set the "this item has changed" flag
	SetModified();

	// Update the iteration
	UpdateDisplay();

	return;
}

//==========================================================================
// Class:			ITERATION
// Function:		SetYAxisType
//
// Description:		Sets the Y axis to the specified quantity.
//
// Input Arguments:
//		_YAxisType	= AXIS_TYPE to plot against
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ITERATION::SetYAxisType(AXIS_TYPE _YAxisType)
{
	// Set the YAxisType
	YAxisType = _YAxisType;

	// Set the "this item has changed" flag
	SetModified();

	// Update the iteration
	UpdateDisplay();

	return;
}

//==========================================================================
// Class:			ITERATION
// Function:		SetAutoAssociate
//
// Description:		Sets the auto-associate with all cars flag.
//
// Input Arguments:
//		AutoAssociate	= bool describing whether the flag should be set to
//						  true or false
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ITERATION::SetAutoAssociate(bool AutoAssociate)
{
	// Update the auto-associate flag
	AssociatedWithAllOpenCars = AutoAssociate;

	// Set the "this item has changed" flag
	SetModified();

	// Update this object
	UpdateDisplay();

	return;
}

//==========================================================================
// Class:			ITERATION
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
void ITERATION::MarkAnalysisComplete(void)
{
	// Post our semaphore
	InverseSemaphore.Post();

	// If the current count is zero, update the display
	if (InverseSemaphore.GetCount() == 0)
		UpdateDisplay();

	return;
}