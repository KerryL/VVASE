/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  gui_car_class.cpp
// Created:  4/15/2008
// Author:  K. Loux
// Description:  A high-level car object.  This class defines the interaction between
//				 the GUI and the actual CAR class.
// History:
//	1/24/2009	- Major application structure change - MAIN_FRAME uses GUI_OBJECT instead of
//				  GUI_CAR.  GUI_OBJECT changed to only contain either GUI_CAR or ITERATION
//				  objects.
//	2/17/2009	- Moved the KINEMATICS object into this class so we can update the 3D car
//				  more easily.
//	5/19/2009	- Changed to derived class from GUI_OBJECT, K. Loux.

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/treectrl.h>

// VVASE headers
#include "vCar/car_class.h"
#include "vCar/aerodynamics_class.h"
#include "vCar/brakes_class.h"
#include "vCar/drivetrain_class.h"
#include "vCar/engine_class.h"
#include "vCar/mass_class.h"
#include "vCar/suspension_class.h"
#include "vCar/tire_class.h"
#include "gui/renderer/car_renderer_class.h"
#include "gui/appearance_options_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/components/main_notebook_class.h"
#include "gui/gui_car_class.h"
#include "gui/components/main_tree_class.h"
#include "vSolver/physics/kinematics_class.h"
#include "vSolver/threads/thread_job_class.h"
#include "vSolver/threads/kinematics_data_class.h"

//==========================================================================
// Class:			GUI_CAR
// Function:		GUI_CAR
//
// Description:		Constructor for the GUI_CAR class.
//
// Input Argurments:
//		_MainFrame			= MAIN_FRAME&, reference to the main application window
//		_Debugger			= const DEBUGGER&, reference to debugger utility object
//		_PathAndFileName	= wxString containing the location of this object
//							  on the hard disk
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GUI_CAR::GUI_CAR(MAIN_FRAME &_MainFrame, const DEBUGGER &_Debugger,
				 wxString _PathAndFileName)
				 : GUI_OBJECT(_MainFrame, _Debugger, _PathAndFileName)
{
	// Allocate memory for the original and working cars
	OriginalCar = new CAR(Debugger);
	WorkingCar = new CAR(*OriginalCar);

	// Create the appearance options object
	AppearanceOptions = new APPEARANCE_OPTIONS(MainFrame, *this, Debugger);

	// Create the 3D output window
	Renderer = new CAR_RENDERER(MainFrame, *this, Debugger);

	// Get an index for this item and add it to the list in the MainFrame
	// MUST be included BEFORE the naming, which must come BEFORE the call to Initialize
	Index = MainFrame.AddObjectToList(this);

	// Create the name based on the index
	Name.Printf("Unsaved Car %i", Index + 1);

	// Complete initialization of this object before creating sub-system icons
	Initialize();

	// Add the children to the systems tree
	int i;
	int IconHandle = -1;
	for (i = 0; i < GUI_CAR::NumberOfSubsystems; i++)
	{
		// No icons unless we're in windows
#ifdef __WXMSW__
		// Get the appropriate icon
		switch (i)
		{
		case GUI_CAR::SubsystemAerodynamics:
			IconHandle = _MainFrame.GetSystemsTree()->GetIconHandle(MAIN_TREE::AerodynamicsIcon);
			break;

		case GUI_CAR::SubsystemBrakes:
			IconHandle = _MainFrame.GetSystemsTree()->GetIconHandle(MAIN_TREE::BrakesIcon);
			break;

		case GUI_CAR::SubsystemDrivetrain:
			IconHandle = _MainFrame.GetSystemsTree()->GetIconHandle(MAIN_TREE::DrivetrainIcon);
			break;

		case GUI_CAR::SubsystemEngine:
			IconHandle = _MainFrame.GetSystemsTree()->GetIconHandle(MAIN_TREE::EngineIcon);
			break;

		case GUI_CAR::SubsystemMassProperties:
			IconHandle = _MainFrame.GetSystemsTree()->GetIconHandle(MAIN_TREE::MassPropertiesIcon);
			break;

		case GUI_CAR::SubsystemSuspension:
			IconHandle = _MainFrame.GetSystemsTree()->GetIconHandle(MAIN_TREE::SuspensionIcon);
			break;

		case GUI_CAR::SubsystemTires:
			IconHandle = _MainFrame.GetSystemsTree()->GetIconHandle(MAIN_TREE::TiresIcon);
			break;

		default:
			IconHandle = -1;
			break;
		}
#endif

		// Add the entry to the tree
		Subsystems[i] = _MainFrame.GetSystemsTree()->AppendItem(TreeID,
			GetSubsystemName((SUBSYSTEMS)i), IconHandle, IconHandle);
	}
}

//==========================================================================
// Class:			GUI_CAR
// Function:		~GUI_CAR
//
// Description:		Destructor for the GUI_CAR class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GUI_CAR::~GUI_CAR()
{
	// Delete the car objects
	delete OriginalCar;
	OriginalCar = NULL;

	delete WorkingCar;
	WorkingCar = NULL;

	// Delete the appearance options
	delete AppearanceOptions;
	AppearanceOptions = NULL;
}

//==========================================================================
// Class:			GUI_CAR
// Function:		GetIconHandle
//
// Description:		Gets the icon handle from the systems tree.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		integer specifying the icon handle
//
//==========================================================================
int GUI_CAR::GetIconHandle(void) const
{
	// Return the proper icon handle
	return SystemsTree->GetIconHandle(MAIN_TREE::CarIcon);
}

//==========================================================================
// Class:			GUI_CAR
// Function:		UpdateData
//
// Description:		Updates the 3D display models.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GUI_CAR::UpdateData(void)
{
	// This is called at various points during the load process due to different
	// GUI screens being created and their requests for data.  This can result in
	// errors.  To combat this, we check to make sure we're done loading before
	// we continue.
	if (!ObjectIsInitialized)
		return;

	// Update the wheel centers
	OriginalCar->ComputeWheelCenters();

	// Re-run the kinematics to update the car's position
	KINEMATICS_DATA *Data = new KINEMATICS_DATA(OriginalCar, WorkingCar, MainFrame.GetInputs(), &KinematicOutputs);
	THREAD_JOB Job(THREAD_JOB::COMMAND_THREAD_KINEMATICS_NORMAL, Data, Name, Index);
	MainFrame.AddJob(Job);

	return;
}

//==========================================================================
// Class:			GUI_CAR
// Function:		UpdateDisplay
//
// Description:		Updates the visual display of this object.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GUI_CAR::UpdateDisplay(void)
{
	// Update the display associated with this object
	static_cast<CAR_RENDERER*>(Renderer)->UpdateDisplay(KinematicOutputs);

	return;
}

//==========================================================================
// Class:			GUI_CAR
// Function:		PerformSaveToFile
//
// Description:		Saves the car object to file.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for car was saved, false for car was not saved
//
//==========================================================================
bool GUI_CAR::PerformSaveToFile(void)
{
	// Make sure we have exclusive access
	wxMutexLocker Lock(OriginalCar->GetMutex());

	// Perform the save - the object we want to save is OriginalCar - this is the
	// one that contains the information about the vehicle as it was input by the
	// user.
	std::ofstream OutFile;
	bool SaveSuccessful(OriginalCar->SaveCarToFile(PathAndFileName, &OutFile));

	// Also write the appearance options after checking to make sure that OutFile was properly opened
	if (OutFile.is_open() && OutFile.good())
	{
		AppearanceOptions->Write(&OutFile);
		OutFile.close();
	}

	return SaveSuccessful;
}

//==========================================================================
// Class:			GUI_CAR
// Function:		PerformLoadFromFile
//
// Description:		Loads the car object from file.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for successful load, false otherwise
//
//==========================================================================
bool GUI_CAR::PerformLoadFromFile(void)
{
	std::ifstream InFile;
	int FileVersion;

	// Make sure we have exclusive access
	wxMutexLocker Lock(OriginalCar->GetMutex());

	// Open the car
	bool LoadSuccessful(OriginalCar->LoadCarFromFile(PathAndFileName, &InFile, &FileVersion));

	// Also load the appearance options after checking to make sure InFile was correctly opened
	if (InFile.is_open() && InFile.good())
	{
		AppearanceOptions->Read(&InFile, FileVersion);
		InFile.close();
	}

	return LoadSuccessful;
}

//==========================================================================
// Class:			GUI_CAR
// Function:		GetSubsystemName
//
// Description:		Returns a string containing the name of the specifyied
//					subsystem.
//
// Input Argurments:
//		Subsystem	= SUBSYSTEM specifying the type of subsystem in which
//					  we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the subsystem
//
//==========================================================================
wxString GUI_CAR::GetSubsystemName(SUBSYSTEMS Subsystem)
{
	switch (Subsystem)
	{
	case SubsystemAerodynamics:
		return _T("Aerodynamics");
		break;

	case SubsystemBrakes:
		return _T("Brakes");
		break;

	case SubsystemDrivetrain:
		return _T("Drivetrain");
		break;

	case SubsystemEngine:
		return _T("Engine");
		break;

	case SubsystemMassProperties:
		return _T("Mass Properties");
		break;

	case SubsystemSuspension:
		return _T("Suspension");
		break;

	case SubsystemTires:
		return _T("Tires");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}