/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  guiCar.cpp
// Created:  4/15/2008
// Author:  K. Loux
// Description:  A high-level car object.  This class defines the interaction between
//				 the user and the Car class.
// History:
//	1/24/2009	- Major application structure change - MainFrame uses GuiObject instead of
//				  GuiCar.  GuiObject changed to only contain either GuiCar or Iteration
//				  objects.
//	2/17/2009	- Moved the Kinematics object into this class so we can update the 3D car
//				  more easily.
//	5/19/2009	- Changed to derived class from GuiObject, K. Loux.

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/treectrl.h>

// VVASE headers
#include "vCar/car.h"
#include "vCar/aerodynamics.h"
#include "vCar/brakes.h"
#include "vCar/drivetrain.h"
#include "vCar/engine.h"
#include "vCar/massProperties.h"
#include "vCar/suspension.h"
#include "vCar/tire.h"
#include "gui/renderer/carRenderer.h"
#include "gui/appearanceOptions.h"
#include "gui/components/mainFrame.h"
#include "gui/components/mainNotebook.h"
#include "gui/guiCar.h"
#include "gui/components/mainTree.h"
#include "vSolver/physics/kinematics.h"
#include "vSolver/threads/threadJob.h"
#include "vSolver/threads/kinematicsData.h"

//==========================================================================
// Class:			GuiCar
// Function:		GuiCar
//
// Description:		Constructor for the GuiCar class.
//
// Input Arguments:
//		mainFrame		= MainFrame&, reference to the main application window
//		pathAndFileName	= wxString containing the location of this object
//						  on the hard disk
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
GuiCar::GuiCar(MainFrame &mainFrame, wxString pathAndFileName)
	: GuiObject(mainFrame, pathAndFileName)
{
	originalCar = new Car();
	workingCar = new Car(*originalCar);

	appearanceOptions = new AppearanceOptions(mainFrame, *this);

	int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0};
	renderer = new CarRenderer(mainFrame, *this, args);
	notebookTab = reinterpret_cast<wxWindow*>(renderer);

	// Get an index for this item and add it to the list in the mainFrame
	// MUST be included BEFORE the naming, which must come BEFORE the call to Initialize
	index = mainFrame.AddObjectToList(this);

	name.Printf("Unsaved Car %i", index + 1);

	Initialize();
	
	// After calling Initialize() (after loading the car from file, if necessary)
	// set the size of the view window and the camera view to fit everything in
	// the scene.
	Vector position(-100.0, -100.0, 60.0), up(0.0, 0.0, 1.0);
	Vector lookAt(originalCar->suspension->rightFront.hardpoints[Corner::ContactPatch] +
		(originalCar->suspension->leftRear.hardpoints[Corner::ContactPatch] - 
		originalCar->suspension->rightFront.hardpoints[Corner::ContactPatch]) * 0.5);
	renderer->SetCameraView(position, lookAt, up);
	
	const double scale = 1.2;// 20% bigger than car
	renderer->SetViewOrthogonal(true);
	renderer->SetTopMinusBottom(originalCar->suspension->leftFront.hardpoints[
		Corner::ContactPatch].Distance(
		originalCar->suspension->rightRear.hardpoints[Corner::ContactPatch]) * scale);
	renderer->SetViewOrthogonal(mainFrame.GetUseOrtho());

	// Add the children to the systems tree
	int i;
	int iconHandle = -1;
	for (i = 0; i < GuiCar::NumberOfSubsystems; i++)
	{
		// Get the appropriate icon
		switch (i)
		{
		case GuiCar::SubsystemAerodynamics:
			iconHandle = mainFrame.GetSystemsTree()->GetIconHandle(MainTree::AerodynamicsIcon);
			break;

		case GuiCar::SubsystemBrakes:
			iconHandle = mainFrame.GetSystemsTree()->GetIconHandle(MainTree::BrakesIcon);
			break;

		case GuiCar::SubsystemDrivetrain:
			iconHandle = mainFrame.GetSystemsTree()->GetIconHandle(MainTree::DrivetrainIcon);
			break;

		case GuiCar::SubsystemEngine:
			iconHandle = mainFrame.GetSystemsTree()->GetIconHandle(MainTree::EngineIcon);
			break;

		case GuiCar::SubsystemMassProperties:
			iconHandle = mainFrame.GetSystemsTree()->GetIconHandle(MainTree::MassPropertiesIcon);
			break;

		case GuiCar::SubsystemSuspension:
			iconHandle = mainFrame.GetSystemsTree()->GetIconHandle(MainTree::SuspensionIcon);
			break;

		case GuiCar::SubsystemTires:
			iconHandle = mainFrame.GetSystemsTree()->GetIconHandle(MainTree::TiresIcon);
			break;

		default:
			iconHandle = -1;
			break;
		}

		// Add the entry to the tree
		subsystems[i] = mainFrame.GetSystemsTree()->AppendItem(treeID,
			GetSubsystemName((Subsystems)i), iconHandle, iconHandle);
	}
}

//==========================================================================
// Class:			GuiCar
// Function:		~GuiCar
//
// Description:		Destructor for the GuiCar class.
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
GuiCar::~GuiCar()
{
	// Delete the car objects
	delete originalCar;
	originalCar = NULL;

	delete workingCar;
	workingCar = NULL;

	// Delete the appearance options
	delete appearanceOptions;
	appearanceOptions = NULL;
}

//==========================================================================
// Class:			GuiCar
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
int GuiCar::GetIconHandle() const
{
	// Return the proper icon handle
	return systemsTree->GetIconHandle(MainTree::CarIcon);
}

//==========================================================================
// Class:			GuiCar
// Function:		UpdateData
//
// Description:		Updates the 3D display models.
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
void GuiCar::UpdateData()
{
	// This is called at various points during the load process due to different
	// GUI screens being created and their requests for data.  This can result in
	// errors.  To combat this, we check to make sure we're done loading before
	// we continue.
	if (!objectIsInitialized)
		return;

	// Update the wheel centers
	originalCar->ComputeWheelCenters();

	// Re-run the kinematics to update the car's position
	KinematicsData *data = new KinematicsData(originalCar, workingCar, mainFrame.GetInputs(), &kinematicOutputs);
	ThreadJob job(ThreadJob::CommandThreadKinematicsNormal, data, name, index);
	mainFrame.AddJob(job);
}

//==========================================================================
// Class:			GuiCar
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
void GuiCar::UpdateDisplay()
{
	// Update the display associated with this object
	renderer->UpdateDisplay(kinematicOutputs);
}

//==========================================================================
// Class:			GuiCar
// Function:		SetUseOrtho
//
// Description:		Sets the renderer view to use the specified projection matrix.
//
// Input Arguments:
//		useOrtho	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void GuiCar::SetUseOrtho(const bool &useOrtho)
{
	renderer->SetViewOrthogonal(useOrtho);
}

//==========================================================================
// Class:			GuiCar
// Function:		PerformSaveToFile
//
// Description:		Saves the car object to file.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for car was saved, false for car was not saved
//
//==========================================================================
bool GuiCar::PerformSaveToFile()
{
	// Make sure we have exclusive access
	wxMutexLocker lock(originalCar->GetMutex());

	// Perform the save - the object we want to save is OriginalCar - this is the
	// one that contains the information about the vehicle as it was input by the
	// user.
	std::ofstream outFile;
	bool saveSuccessful(originalCar->SaveCarToFile(pathAndFileName, &outFile));

	// Also write the appearance options after checking to make sure that OutFile was properly opened
	if (outFile.is_open() && outFile.good())
	{
		appearanceOptions->Write(&outFile);
		outFile.close();
	}

	return saveSuccessful;
}

//==========================================================================
// Class:			GuiCar
// Function:		PerformLoadFromFile
//
// Description:		Loads the car object from file.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for successful load, false otherwise
//
//==========================================================================
bool GuiCar::PerformLoadFromFile()
{
	std::ifstream inFile;
	int fileVersion;

	// Make sure we have exclusive access
	wxMutexLocker lock(originalCar->GetMutex());

	// Open the car
	bool loadSuccessful(originalCar->LoadCarFromFile(pathAndFileName, &inFile, &fileVersion));

	// Also load the appearance options after checking to make sure InFile was correctly opened
	if (inFile.is_open() && inFile.good())
	{
		appearanceOptions->Read(&inFile, fileVersion);
		inFile.close();
	}

	return loadSuccessful;
}

//==========================================================================
// Class:			GuiCar
// Function:		GetSubsystemName
//
// Description:		Returns a string containing the name of the specifyied
//					subsystem.
//
// Input Arguments:
//		subsystem	= Subsystems specifying the type of subsystem in which
//					  we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the subsystem
//
//==========================================================================
wxString GuiCar::GetSubsystemName(Subsystems subsystem)
{
	switch (subsystem)
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