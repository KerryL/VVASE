/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  guiCar.cpp
// Date:  4/15/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  A high-level car object.  This class defines the interaction between
//        the user and the Car class.

// wxWidgets headers
#include <wx/treectrl.h>

// Local headers
#include "VVASE/core/car/car.h"
#include "VVASE/core/car/subsystems/aerodynamics.h"
#include "VVASE/core/car/subsystems/brakes.h"
#include "VVASE/core/car/subsystems/drivetrain.h"
#include "VVASE/core/car/subsystems/engine.h"
#include "VVASE/core/car/subsystems/massProperties.h"
#include "VVASE/core/car/subsystems/suspension.h"
#include "VVASE/core/car/subsystems/tire.h"
#include "VVASE/gui/renderer/carRenderer.h"
#include "appearanceOptions.h"
#include "VVASE/gui/components/mainFrame.h"
#include "VVASE/gui/components/mainNotebook.h"
#include "guiCar.h"
#include "VVASE/gui/components/mainTree.h"
#include "VVASE/core/analysis/kinematics.h"
#include "VVASE/core/threads/threadJob.h"
#include "VVASE/core/threads/kinematicsData.h"
#include "VVASE/core/threads/threadDefs.h"
#include "VVASE/gui/utilities/wxRelatedUtilities.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

namespace VVASE
{

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
	// Get an index for this item and add it to the list in the mainFrame
	// MUST be included BEFORE the naming, which must come BEFORE the call to Initialize
	index = mainFrame.AddObjectToList(std::unique_ptr<GuiCar>(this));

	originalCar = std::make_unique<Car>();
	workingCar = std::make_unique<Car>(*originalCar);

	appearanceOptions = std::make_unique<AppearanceOptions>(mainFrame, *this);

	wxGLAttributes displayAttributes;
	displayAttributes.PlatformDefaults().RGBA().DoubleBuffer().SampleBuffers(1).Samplers(4).Stencil(1).EndList();
	assert(wxGLCanvas::IsDisplaySupported(displayAttributes));
	renderer = new CarRenderer(mainFrame, *this, wxID_ANY, displayAttributes);
	notebookTab = reinterpret_cast<wxWindow*>(renderer);

	name.Printf("Unsaved Car %i", index + 1);

	Initialize();

	const Suspension* originalSuspension(originalCar->GetSubsystem<Suspension>());

	// After calling Initialize() (after loading the car from file, if necessary)
	// set the size of the view window and the camera view to fit everything in
	// the scene.
	const Eigen::Vector3d position(-100.0, -100.0, 60.0), up(0.0, 0.0, 1.0);
	const Eigen::Vector3d lookAt(originalSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)] +
		(originalSuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)] -
		originalSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)]) * 0.5);
	renderer->SetCameraView(position, lookAt, up);

	const double scale = 1.2;// 20% bigger than car
	const double referenceDimension((originalSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)] -
		originalSuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)]).norm());
	renderer->SetViewOrthogonal(true);
	if (renderer->GetAspectRatio() > 1.0)// smaller up-down
		renderer->SetTopMinusBottom(referenceDimension * scale);
	else// smaller left-right
		renderer->SetTopMinusBottom(referenceDimension * scale / renderer->GetAspectRatio());
	renderer->SetViewOrthogonal(mainFrame.GetUseOrtho());

	// TODO:  Needs to be re-worked with new scheme
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
	ComputeARBSignConventions();

	Kinematics::Inputs inputs;
	if (mainFrame.ActiveAnalysisIsKinematic())
	{
		inputs = mainFrame.GetInputs();
		inputs.tireDeflections.leftFront = 0.0;
		inputs.tireDeflections.rightFront = 0.0;
		inputs.tireDeflections.leftRear = 0.0;
		inputs.tireDeflections.rightRear = 0.0;
		outputs.hasQuasiStaticOutputs = false;
	}
	else
	{
		QuasiStatic quasiStatic;
		inputs = quasiStatic.Solve(originalCar.get(), workingCar.get(), mainFrame.GetInputs(),
			mainFrame.GetQuasiStaticInputs(), outputs.quasiStaticOutputs);
		outputs.hasQuasiStaticOutputs = true;
	}

	// Re-run the kinematics to update the car's position
	std::unique_ptr<KinematicsData> data(std::make_unique<KinematicsData>(originalCar.get(), workingCar.get(), inputs, &outputs.kinematicOutputs));
	ThreadJob job(ThreadJob::CommandThreadKinematicsNormal, std::move(data), wxUtilities::ToVVASEString(name), index);
	mainFrame.AddJob(std::move(job));
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
	renderer->UpdateDisplay(outputs.kinematicOutputs);
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
	MutexLocker lock(originalCar->GetMutex());

	// Perform the save - the object we want to save is OriginalCar - this is the
	// one that contains the information about the vehicle as it was input by the
	// user.
	std::ofstream outFile;
	bool saveSuccessful(originalCar->SaveCarToFile(wxUtilities::ToVVASEString(pathAndFileName), outFile));

	// Also write the appearance options after checking to make sure that OutFile was properly opened
	if (outFile.is_open() && outFile.good())
		appearanceOptions->Write(outFile);

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
	MutexLocker lock(originalCar->GetMutex());

	// Open the car
	bool loadSuccessful(originalCar->LoadCarFromFile(wxUtilities::ToVVASEString(pathAndFileName), inFile, fileVersion));

	// Also load the appearance options after checking to make sure InFile was correctly opened
	if (inFile.is_open() && inFile.good())
		appearanceOptions->Read(inFile, fileVersion);

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

//==========================================================================
// Class:			GuiCar
// Function:		ComputeARBSignConventions
//
// Description:		Computes the required sign convention for ARB twist.
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
void GuiCar::ComputeARBSignConventions()
{
	Suspension* originalSuspension(originalCar->GetSubsystem<Suspension>());

	// Reset convention
	originalSuspension->frontBarSignGreaterThan = true;
	originalSuspension->rearBarSignGreaterThan = true;

	// Test convention
	WheelSet tireDeflections;
	tireDeflections.leftFront = 0.0;
	tireDeflections.rightFront = 0.0;
	tireDeflections.leftRear = 0.0;
	tireDeflections.rightRear = 0.0;

	Kinematics kinematics;
	kinematics.SetCenterOfRotation(Eigen::Vector3d(0.0, 0.0, 0.0));
	kinematics.SetRotationSequence(Kinematics::RotationSequence::PitchRoll);
	kinematics.SetHeave(0.0);
	kinematics.SetRoll(0.01);
	kinematics.SetPitch(0.0);
	kinematics.SetRackTravel(0.0);
	kinematics.SetTireDeflections(tireDeflections);
	kinematics.UpdateKinematics(originalCar.get(), workingCar.get(), wxUtilities::ToVVASEString(name) + _T(" -> ARB Sign Convention Test"));

	// Adjust convention if necessary
	if (kinematics.GetOutputs().doubles[KinematicOutputs::FrontARBTwist] < 0.0)
		originalSuspension->frontBarSignGreaterThan = false;

	if (kinematics.GetOutputs().doubles[KinematicOutputs::RearARBTwist] < 0.0)
		originalSuspension->rearBarSignGreaterThan = false;
}

}// namespace VVASE
