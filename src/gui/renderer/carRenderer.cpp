/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  carRenderer.cpp
// Date:  4/5/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the CarRenderer class.  Derived from
//        RenderWindow, this class is associated with a loaded car file
//        and contains the information and methods required to render a car in 3D.

// Standard C++ headers
#include <algorithm>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "VVASE/gui/renderer/carRenderer.h"
#include "VVASE/gui/renderer/3dcar/aarm.h"
#include "VVASE/gui/renderer/3dcar/link.h"
#include "VVASE/gui/renderer/3dcar/triangle3D.h"
#include "VVASE/gui/renderer/3dcar/damper3D.h"
#include "VVASE/gui/renderer/3dcar/spring3D.h"
#include "VVASE/gui/renderer/3dcar/plane3D.h"
#include "VVASE/gui/renderer/3dcar/point3D.h"
#include "VVASE/gui/renderer/3dcar/origin.h"
#include "VVASE/gui/renderer/3dcar/tire3D.h"
#include "VVASE/gui/renderer/3dcar/swaybar3D.h"
#include "VVASE/gui/renderer/3dcar/vector3D.h"
#include "VVASE/gui/renderer/primitives/primitive.h"
#include "VVASE/core/car/car.h"
#include "VVASE/core/car/subsystems/brakes.h"
#include "VVASE/core/car/subsystems/drivetrain.h"
#include "VVASE/core/car/subsystems/suspension.h"
#include "VVASE/core/car/subsystems/tire.h"
#include "VVASE/core/car/subsystems/tireSet.h"
#include "VVASE/core/utilities/debugger.h"
#include "VVASE/core/utilities/carMath.h"
#include "../guiCar.h"
#include "../appearanceOptions.h"
#include "../components/mainFrame.h"
#include "../dialogs/vectorEditDialog.h"
#include "VVASE/core/analysis/kinematicOutputs.h"
#include "VVASE/core/utilities/debugLog.h"
#include "VVASE/gui/renderer/3dcar/debugShape.h"

namespace VVASE
{

//==========================================================================
// Class:			CarRenderer
// Function:		CarRenderer
//
// Description:		Constructor for CarRenderer class.  Initializes the
//					renderer and sets up the canvas.
//
// Input Arguments:
//		mainFrame	= MainFrame& reference to the owner of this object
//		car			= GuiCar& reference to the car that we are to render
//		id			= wxWindowID
//		attributes	= const wxGLAttributes&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
CarRenderer::CarRenderer(MainFrame &mainFrame, GuiCar &car,
	const wxWindowID& id, const wxGLAttributes& attributes)
	: RenderWindow(mainFrame, id, attributes, wxDefaultPosition, wxDefaultSize,
	wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE), mainFrame(mainFrame), car(car),
	appearanceOptions(car.GetAppearanceOptions()), displayCar(car.GetWorkingCar()),
	referenceCar(car.GetOriginalCar())
{
	InternalInitialization();
}

//==========================================================================
// Class:			CarRenderer
// Function:		CarRenderer
//
// Description:		Constructor for CarRenderer class.  Initializes the
//					renderer and sets up the canvas.
//
// Input Arguments:
//		mainFrame	= MainFrame& reference to the owner of this object
//		car			= GuiCar& reference to the car that we are to render
//		id			= wxWindowID
//		attributes	= const int[]
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
CarRenderer::CarRenderer(MainFrame &mainFrame, GuiCar &car,
	const wxWindowID& id, const int attributes[])
	: RenderWindow(mainFrame, id, attributes, wxDefaultPosition, wxDefaultSize,
	wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE), mainFrame(mainFrame), car(car),
	appearanceOptions(car.GetAppearanceOptions()), displayCar(car.GetWorkingCar()),
	referenceCar(car.GetOriginalCar())
{
	InternalInitialization();
}

//==========================================================================
// Class:			CarRenderer
// Function:		InternalInitialization
//
// Description:		Initializes internal class variables.  To be called from constructors.
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
void CarRenderer::InternalInitialization()
{
	CreateActors();

	// Initialize the helper orb to NOT active
	helperOrbCornerPoint = Corner::NumberOfHardpoints;
	helperOrbLocation = Corner::LocationRightFront;
	helperOrbSuspensionPoint = Suspension::NumberOfHardpoints;
	helperOrbIsActive = false;

#ifdef USE_DEBUG_SHAPE
	DebugShape::SetRenderer(*this);
#endif
}

//==========================================================================
// Class:			CarRenderer
// Function:		~CarRenderer
//
// Description:		Destructor for CarRenderer class.
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
CarRenderer::~CarRenderer()
{
	// Ideally, this stuff would be taken care of by adding these to some
	// ManagedList, but for now we explicitly delete them one by one...
	delete origin;
	delete groundPlane;

	delete rightFrontLowerAArm;
	delete rightFrontUpperAArm;
	delete rightFrontPushrod;
	delete rightFrontTieRod;
	delete rightFrontTire;
	delete rightFrontDamper;
	delete rightFrontSpring;
	delete rightFrontUpright;
	delete rightFrontBellCrank;
	delete rightFrontBarLink;
	delete rightFrontHalfShaft;

	delete leftFrontLowerAArm;
	delete leftFrontUpperAArm;
	delete leftFrontPushrod;
	delete leftFrontTieRod;
	delete leftFrontTire;
	delete leftFrontDamper;
	delete leftFrontSpring;
	delete leftFrontUpright;
	delete leftFrontBellCrank;
	delete leftFrontBarLink;
	delete leftFrontHalfShaft;

	delete rightRearLowerAArm;
	delete rightRearUpperAArm;
	delete rightRearPushrod;
	delete rightRearTieRod;
	delete rightRearTire;
	delete rightRearDamper;
	delete rightRearSpring;
	delete rightRearUpright;
	delete rightRearBellCrank;
	delete rightRearBarLink;
	delete rightRearHalfShaft;

	delete leftRearLowerAArm;
	delete leftRearUpperAArm;
	delete leftRearPushrod;
	delete leftRearTieRod;
	delete leftRearTire;
	delete leftRearDamper;
	delete leftRearSpring;
	delete leftRearUpright;
	delete leftRearBellCrank;
	delete leftRearBarLink;
	delete leftRearHalfShaft;

	delete steeringRack;
	delete frontSwayBar;

	delete rearSwayBar;

	delete frontRollCenter;
	delete rearRollCenter;
	delete rightPitchCenter;
	delete leftPitchCenter;
	delete rightFrontInstantCenter;
	delete leftFrontInstantCenter;
	delete rightRearInstantCenter;
	delete leftRearInstantCenter;

	delete frontRollAxis;
	delete rearRollAxis;
	delete rightPitchAxis;
	delete leftPitchAxis;
	delete rightFrontInstantAxis;
	delete leftFrontInstantAxis;
	delete rightRearInstantAxis;
	delete leftRearInstantAxis;

	delete helperOrb;
	delete helperOrbOpposite;
}

//==========================================================================
// Class:			CarRenderer
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
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
BEGIN_EVENT_TABLE(CarRenderer, RenderWindow)
	EVT_LEFT_UP(				CarRenderer::OnLeftClick)
	EVT_RIGHT_UP(				CarRenderer::OnRightClick)
	EVT_MENU(idContextEdit,	CarRenderer::OnContextEdit)
END_EVENT_TABLE();

//==========================================================================
// Class:			CarRenderer
// Function:		UpdateDisplay
//
// Description:		Updates the displayed 3D car to match the new position
//					of the class member DisplayCar.
//
// Input Arguments:
//		outputs	= const KinematicOutputs& containing the locations of all of the
//				  kinematic variables that this object displays in the scene
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CarRenderer::UpdateDisplay(const KinematicOutputs &outputs)
{
	SetBackgroundColor(appearanceOptions.GetColor(AppearanceOptions::ColorBackground));
	UpdateCarDisplay();
	UpdateKinematicsDisplay(outputs);

	AutoSetFrustum();
	Refresh();
}

//==========================================================================
// Class:			CarRenderer
// Function:		UpdateCarDisplay
//
// Description:		Updates the displayed 3D car to match the new position
//					of the class member displayCar.
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
void CarRenderer::UpdateCarDisplay()
{
	// For drawing the tire
	Eigen::Vector3d targetNormal;
	Eigen::Vector3d originalNormal;

	// Get locks on the car's mutexes
	// NOTE:  Always lock working car first, then lock original car (consistency required to prevent deadlocks)
	wxMutexLocker displayLock(displayCar.GetMutex());
	DebugLog::GetInstance()->Log("CarRenderer::UpdateCarDisplay (displayLock)", 0);
	wxMutexLocker referenceLock(referenceCar.GetMutex());
	DebugLog::GetInstance()->Log("CarRenderer::UpdateCarDisplay (referenceLock)", 0);

	// Update the origin
	origin->Update(appearanceOptions.GetSize(AppearanceOptions::SizeOriginShaftLength),
		appearanceOptions.GetSize(AppearanceOptions::SizeOriginShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeOriginTipLength),
		appearanceOptions.GetSize(AppearanceOptions::SizeOriginTipDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionOrigin),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityOrigin));

	// Figure out how big the ground plane should be
	double groundPlaneScaleUp = 1.1;
	double rightMostPoint = max(referenceCar.suspension->rightFront.hardpoints[Corner::ContactPatch].y +
		referenceCar.tires->rightFront->width / 2.0,
		referenceCar.suspension->rightRear.hardpoints[Corner::ContactPatch].y +
		referenceCar.tires->rightRear->width / 2.0);
	double leftMostPoint = min(referenceCar.suspension->leftFront.hardpoints[Corner::ContactPatch].y -
		referenceCar.tires->leftFront->width / 2.0,
		referenceCar.suspension->leftRear.hardpoints[Corner::ContactPatch].y -
		referenceCar.tires->leftRear->width / 2.0);
	double frontMostPoint = min(referenceCar.suspension->rightFront.hardpoints[Corner::ContactPatch].x -
		referenceCar.tires->rightFront->diameter / 2.0,
		referenceCar.suspension->leftFront.hardpoints[Corner::ContactPatch].x -
		referenceCar.tires->leftFront->diameter / 2.0);
	double rearMostPoint = max(referenceCar.suspension->rightRear.hardpoints[Corner::ContactPatch].x +
		referenceCar.tires->rightRear->diameter / 2.0,
		referenceCar.suspension->leftRear.hardpoints[Corner::ContactPatch].x +
		referenceCar.tires->leftRear->diameter / 2.0);

	double xLength = fabs(frontMostPoint - rearMostPoint) * groundPlaneScaleUp;
	double yLength = fabs(rightMostPoint - leftMostPoint) * groundPlaneScaleUp;
	double xOffset = (rearMostPoint - frontMostPoint) / 2.0 + frontMostPoint;
	groundPlane->Update(xLength, yLength, xOffset,
		appearanceOptions.GetColor(AppearanceOptions::ColorGroundPlane),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityGroundPlane));

	// Copy the tire parameters from the reference car
	displayCar.tires->rightFront->width = referenceCar.tires->rightFront->width;
	displayCar.tires->leftFront->width = referenceCar.tires->leftFront->width;
	displayCar.tires->rightRear->width = referenceCar.tires->rightRear->width;
	displayCar.tires->leftRear->width = referenceCar.tires->leftRear->width;

	// Determine whether or not the front sway bar links should be visible
	bool ShowBarLinks = true;
	if (displayCar.suspension->frontBarStyle == Suspension::SwayBarNone ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySwayBar))
		ShowBarLinks = false;

	// Right front corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	bool ShowBellCranksPushrods = true;
	if (displayCar.suspension->rightFront.actuationType == Corner::ActuationOutboardRockerArm ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	targetNormal = VVASEMath::GetPlaneNormal(displayCar.suspension->rightFront.hardpoints[Corner::LowerBallJoint],
							displayCar.suspension->rightFront.hardpoints[Corner::UpperBallJoint],
							displayCar.suspension->rightFront.hardpoints[Corner::OutboardTieRod]);
	originalNormal = VVASEMath::GetPlaneNormal(referenceCar.suspension->rightFront.hardpoints[Corner::LowerBallJoint],
							referenceCar.suspension->rightFront.hardpoints[Corner::UpperBallJoint],
							referenceCar.suspension->rightFront.hardpoints[Corner::OutboardTieRod]);

	// We also need to account for toe and camber settings for the TargetNormal - apply camber first
	// NOTE:  This corner is on the right side of the car - we flip the sign on the camber and toe angles
	targetNormal.Rotate(-displayCar.suspension->rightFront.staticCamber, Eigen::Vector3d::AxisX);
	targetNormal.Rotate(-displayCar.suspension->rightFront.staticToe, Eigen::Vector3d::AxisZ);

	// Now continue with the update for this corner
	rightFrontLowerAArm->Update(displayCar.suspension->rightFront.hardpoints[Corner::LowerFrontTubMount],
		displayCar.suspension->rightFront.hardpoints[Corner::LowerBallJoint],
		displayCar.suspension->rightFront.hardpoints[Corner::LowerRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	rightFrontUpperAArm->Update(displayCar.suspension->rightFront.hardpoints[Corner::UpperFrontTubMount],
		displayCar.suspension->rightFront.hardpoints[Corner::UpperBallJoint],
		displayCar.suspension->rightFront.hardpoints[Corner::UpperRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	rightFrontPushrod->Update(displayCar.suspension->rightFront.hardpoints[Corner::InboardPushrod],
		displayCar.suspension->rightFront.hardpoints[Corner::OutboardPushrod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	rightFrontTieRod->Update(displayCar.suspension->rightFront.hardpoints[Corner::InboardTieRod],
		displayCar.suspension->rightFront.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	rightFrontTire->Update(displayCar.suspension->rightFront.hardpoints[Corner::ContactPatch],
		displayCar.suspension->rightFront.hardpoints[Corner::WheelCenter], originalNormal, targetNormal,
		displayCar.tires->rightFront->width,
		appearanceOptions.GetSize(AppearanceOptions::SizeTireInsideDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionTire),
		appearanceOptions.GetColor(AppearanceOptions::ColorTire),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTire));
	rightFrontDamper->Update(displayCar.suspension->rightFront.hardpoints[Corner::OutboardDamper],
		displayCar.suspension->rightFront.hardpoints[Corner::InboardDamper],
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperBody),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityDamper));
	rightFrontSpring->Update(displayCar.suspension->rightFront.hardpoints[Corner::InboardSpring],
		displayCar.suspension->rightFront.hardpoints[Corner::OutboardSpring],
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringEndPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorSpring),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySpring));
	rightFrontUpright->Update(displayCar.suspension->rightFront.hardpoints[Corner::LowerBallJoint],
		displayCar.suspension->rightFront.hardpoints[Corner::UpperBallJoint],
		displayCar.suspension->rightFront.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetColor(AppearanceOptions::ColorUpright),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityUpright));
	rightFrontBellCrank->Update(displayCar.suspension->rightFront.hardpoints[Corner::OutboardDamper],
		displayCar.suspension->rightFront.hardpoints[Corner::InboardPushrod],
		VVASEMath::NearestPointOnAxis(displayCar.suspension->rightFront.hardpoints[Corner::BellCrankPivot1],
		displayCar.suspension->rightFront.hardpoints[Corner::BellCrankPivot2] -
		displayCar.suspension->rightFront.hardpoints[Corner::BellCrankPivot1],
		displayCar.suspension->rightFront.hardpoints[Corner::InboardPushrod]),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	rightFrontBarLink->Update(displayCar.suspension->rightFront.hardpoints[Corner::InboardBarLink],
		displayCar.suspension->rightFront.hardpoints[Corner::OutboardBarLink],
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarLinkDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		ShowBarLinks);
	rightFrontHalfShaft->Update(displayCar.suspension->rightFront.hardpoints[Corner::InboardHalfShaft],
		displayCar.suspension->rightFront.hardpoints[Corner::OutboardHalfShaft],
		appearanceOptions.GetSize(AppearanceOptions::SizeHalfShaftDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHalfShaft),
		appearanceOptions.GetColor(AppearanceOptions::ColorHalfShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHalfShaft) && referenceCar.HasFrontHalfShafts());

	// left front corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (displayCar.suspension->leftFront.actuationType == Corner::ActuationOutboardRockerArm ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	targetNormal = VVASEMath::GetPlaneNormal(displayCar.suspension->leftFront.hardpoints[Corner::LowerBallJoint],
							displayCar.suspension->leftFront.hardpoints[Corner::UpperBallJoint],
							displayCar.suspension->leftFront.hardpoints[Corner::OutboardTieRod]);
	originalNormal = VVASEMath::GetPlaneNormal(referenceCar.suspension->leftFront.hardpoints[Corner::LowerBallJoint],
							referenceCar.suspension->leftFront.hardpoints[Corner::UpperBallJoint],
							referenceCar.suspension->leftFront.hardpoints[Corner::OutboardTieRod]);

	// We also need to account for toe and camber settings for the TargetNormal - apply camber first
	targetNormal.Rotate(displayCar.suspension->leftFront.staticCamber, Eigen::Vector3d::AxisX);
	targetNormal.Rotate(displayCar.suspension->leftFront.staticToe, Eigen::Vector3d::AxisZ);

	// Now continue with the update for this corner
	leftFrontLowerAArm->Update(displayCar.suspension->leftFront.hardpoints[Corner::LowerFrontTubMount],
		displayCar.suspension->leftFront.hardpoints[Corner::LowerBallJoint],
		displayCar.suspension->leftFront.hardpoints[Corner::LowerRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	leftFrontUpperAArm->Update(displayCar.suspension->leftFront.hardpoints[Corner::UpperFrontTubMount],
		displayCar.suspension->leftFront.hardpoints[Corner::UpperBallJoint],
		displayCar.suspension->leftFront.hardpoints[Corner::UpperRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	leftFrontPushrod->Update(displayCar.suspension->leftFront.hardpoints[Corner::InboardPushrod],
		displayCar.suspension->leftFront.hardpoints[Corner::OutboardPushrod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	leftFrontTieRod->Update(displayCar.suspension->leftFront.hardpoints[Corner::InboardTieRod],
		displayCar.suspension->leftFront.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	leftFrontTire->Update(displayCar.suspension->leftFront.hardpoints[Corner::ContactPatch],
		displayCar.suspension->leftFront.hardpoints[Corner::WheelCenter], originalNormal, targetNormal,
		displayCar.tires->leftFront->width,
		appearanceOptions.GetSize(AppearanceOptions::SizeTireInsideDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionTire),
		appearanceOptions.GetColor(AppearanceOptions::ColorTire),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTire));
	leftFrontDamper->Update(displayCar.suspension->leftFront.hardpoints[Corner::OutboardDamper],
		displayCar.suspension->leftFront.hardpoints[Corner::InboardDamper],
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperBody),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityDamper));
	leftFrontSpring->Update(displayCar.suspension->leftFront.hardpoints[Corner::InboardSpring],
		displayCar.suspension->leftFront.hardpoints[Corner::OutboardSpring],
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringEndPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorSpring),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySpring));
	leftFrontUpright->Update(displayCar.suspension->leftFront.hardpoints[Corner::LowerBallJoint],
		displayCar.suspension->leftFront.hardpoints[Corner::UpperBallJoint],
		displayCar.suspension->leftFront.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetColor(AppearanceOptions::ColorUpright),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityUpright));
	leftFrontBellCrank->Update(displayCar.suspension->leftFront.hardpoints[Corner::OutboardDamper],
		displayCar.suspension->leftFront.hardpoints[Corner::InboardPushrod],
		VVASEMath::NearestPointOnAxis(displayCar.suspension->leftFront.hardpoints[Corner::BellCrankPivot1],
		displayCar.suspension->leftFront.hardpoints[Corner::BellCrankPivot2] -
		displayCar.suspension->leftFront.hardpoints[Corner::BellCrankPivot1],
		displayCar.suspension->leftFront.hardpoints[Corner::InboardPushrod]),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	leftFrontBarLink->Update(displayCar.suspension->leftFront.hardpoints[Corner::InboardBarLink],
		displayCar.suspension->leftFront.hardpoints[Corner::OutboardBarLink],
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarLinkDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		ShowBarLinks);
	leftFrontHalfShaft->Update(displayCar.suspension->leftFront.hardpoints[Corner::InboardHalfShaft],
		displayCar.suspension->leftFront.hardpoints[Corner::OutboardHalfShaft],
		appearanceOptions.GetSize(AppearanceOptions::SizeHalfShaftDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHalfShaft),
		appearanceOptions.GetColor(AppearanceOptions::ColorHalfShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHalfShaft) && referenceCar.HasFrontHalfShafts());

	// Determine whether or not the rear sway bar links should be visible
	if (displayCar.suspension->rearBarStyle == Suspension::SwayBarNone ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySwayBar))
		ShowBarLinks = false;
	else
		ShowBarLinks = true;

	// right rear corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (displayCar.suspension->rightRear.actuationType == Corner::ActuationOutboardRockerArm ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	targetNormal = VVASEMath::GetPlaneNormal(displayCar.suspension->rightRear.hardpoints[Corner::LowerBallJoint],
							displayCar.suspension->rightRear.hardpoints[Corner::UpperBallJoint],
							displayCar.suspension->rightRear.hardpoints[Corner::OutboardTieRod]);
	originalNormal = VVASEMath::GetPlaneNormal(referenceCar.suspension->rightRear.hardpoints[Corner::LowerBallJoint],
							referenceCar.suspension->rightRear.hardpoints[Corner::UpperBallJoint],
							referenceCar.suspension->rightRear.hardpoints[Corner::OutboardTieRod]);

	// We also need to account for toe and camber settings for the TargetNormal - apply camber first
	// NOTE:  This corner is on the right side of the car - we flip the sign on the camber and toe angles
	targetNormal.Rotate(-displayCar.suspension->rightRear.staticCamber, Eigen::Vector3d::AxisX);
	targetNormal.Rotate(-displayCar.suspension->rightRear.staticToe, Eigen::Vector3d::AxisZ);

	// Now continue with the update for this corner
	rightRearLowerAArm->Update(displayCar.suspension->rightRear.hardpoints[Corner::LowerFrontTubMount],
		displayCar.suspension->rightRear.hardpoints[Corner::LowerBallJoint],
		displayCar.suspension->rightRear.hardpoints[Corner::LowerRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	rightRearUpperAArm->Update(displayCar.suspension->rightRear.hardpoints[Corner::UpperFrontTubMount],
		displayCar.suspension->rightRear.hardpoints[Corner::UpperBallJoint],
		displayCar.suspension->rightRear.hardpoints[Corner::UpperRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	rightRearPushrod->Update(displayCar.suspension->rightRear.hardpoints[Corner::InboardPushrod],
		displayCar.suspension->rightRear.hardpoints[Corner::OutboardPushrod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	rightRearTieRod->Update(displayCar.suspension->rightRear.hardpoints[Corner::InboardTieRod],
		displayCar.suspension->rightRear.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	rightRearTire->Update(displayCar.suspension->rightRear.hardpoints[Corner::ContactPatch],
		displayCar.suspension->rightRear.hardpoints[Corner::WheelCenter], originalNormal, targetNormal,
		displayCar.tires->rightRear->width,
		appearanceOptions.GetSize(AppearanceOptions::SizeTireInsideDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionTire),
		appearanceOptions.GetColor(AppearanceOptions::ColorTire),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTire));
	rightRearDamper->Update(displayCar.suspension->rightRear.hardpoints[Corner::OutboardDamper],
		displayCar.suspension->rightRear.hardpoints[Corner::InboardDamper],
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperBody),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityDamper));
	rightRearSpring->Update(displayCar.suspension->rightRear.hardpoints[Corner::InboardSpring],
		displayCar.suspension->rightRear.hardpoints[Corner::OutboardSpring],
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringEndPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorSpring),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySpring));
	rightRearUpright->Update(displayCar.suspension->rightRear.hardpoints[Corner::LowerBallJoint],
		displayCar.suspension->rightRear.hardpoints[Corner::UpperBallJoint],
		displayCar.suspension->rightRear.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetColor(AppearanceOptions::ColorUpright),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityUpright));
	rightRearBellCrank->Update(displayCar.suspension->rightRear.hardpoints[Corner::OutboardDamper],
		displayCar.suspension->rightRear.hardpoints[Corner::InboardPushrod],
		VVASEMath::NearestPointOnAxis(displayCar.suspension->rightRear.hardpoints[Corner::BellCrankPivot1],
		displayCar.suspension->rightRear.hardpoints[Corner::BellCrankPivot2] -
		displayCar.suspension->rightRear.hardpoints[Corner::BellCrankPivot1],
		displayCar.suspension->rightRear.hardpoints[Corner::InboardPushrod]),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	rightRearBarLink->Update(displayCar.suspension->rightRear.hardpoints[Corner::InboardBarLink],
		displayCar.suspension->rightRear.hardpoints[Corner::OutboardBarLink],
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarLinkDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		ShowBarLinks);
	rightRearHalfShaft->Update(displayCar.suspension->rightRear.hardpoints[Corner::InboardHalfShaft],
		displayCar.suspension->rightRear.hardpoints[Corner::OutboardHalfShaft],
		appearanceOptions.GetSize(AppearanceOptions::SizeHalfShaftDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHalfShaft),
		appearanceOptions.GetColor(AppearanceOptions::ColorHalfShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHalfShaft) && referenceCar.HasRearHalfShafts());

	// left rear corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (displayCar.suspension->leftRear.actuationType == Corner::ActuationOutboardRockerArm ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	targetNormal = VVASEMath::GetPlaneNormal(displayCar.suspension->leftRear.hardpoints[Corner::LowerBallJoint],
							displayCar.suspension->leftRear.hardpoints[Corner::UpperBallJoint],
							displayCar.suspension->leftRear.hardpoints[Corner::OutboardTieRod]);
	originalNormal = VVASEMath::GetPlaneNormal(referenceCar.suspension->leftRear.hardpoints[Corner::LowerBallJoint],
							referenceCar.suspension->leftRear.hardpoints[Corner::UpperBallJoint],
							referenceCar.suspension->leftRear.hardpoints[Corner::OutboardTieRod]);

	// We also need to account for toe and camber settings for the TargetNormal - apply camber first
	targetNormal.Rotate(displayCar.suspension->leftRear.staticCamber, Eigen::Vector3d::AxisX);
	targetNormal.Rotate(displayCar.suspension->leftRear.staticToe, Eigen::Vector3d::AxisZ);

	// Now continue with the update for this corner
	leftRearLowerAArm->Update(displayCar.suspension->leftRear.hardpoints[Corner::LowerFrontTubMount],
		displayCar.suspension->leftRear.hardpoints[Corner::LowerBallJoint],
		displayCar.suspension->leftRear.hardpoints[Corner::LowerRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	leftRearUpperAArm->Update(displayCar.suspension->leftRear.hardpoints[Corner::UpperFrontTubMount],
		displayCar.suspension->leftRear.hardpoints[Corner::UpperBallJoint],
		displayCar.suspension->leftRear.hardpoints[Corner::UpperRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	leftRearPushrod->Update(displayCar.suspension->leftRear.hardpoints[Corner::InboardPushrod],
		displayCar.suspension->leftRear.hardpoints[Corner::OutboardPushrod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	leftRearTieRod->Update(displayCar.suspension->leftRear.hardpoints[Corner::InboardTieRod],
		displayCar.suspension->leftRear.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	leftRearTire->Update(displayCar.suspension->leftRear.hardpoints[Corner::ContactPatch],
		displayCar.suspension->leftRear.hardpoints[Corner::WheelCenter], originalNormal, targetNormal,
		displayCar.tires->leftRear->width,
		appearanceOptions.GetSize(AppearanceOptions::SizeTireInsideDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionTire),
		appearanceOptions.GetColor(AppearanceOptions::ColorTire),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTire));
	leftRearDamper->Update(displayCar.suspension->leftRear.hardpoints[Corner::OutboardDamper],
		displayCar.suspension->leftRear.hardpoints[Corner::InboardDamper],
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperBody),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityDamper));
	leftRearSpring->Update(displayCar.suspension->leftRear.hardpoints[Corner::InboardSpring],
		displayCar.suspension->leftRear.hardpoints[Corner::OutboardSpring],
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringEndPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorSpring),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySpring));
	leftRearUpright->Update(displayCar.suspension->leftRear.hardpoints[Corner::LowerBallJoint],
		displayCar.suspension->leftRear.hardpoints[Corner::UpperBallJoint],
		displayCar.suspension->leftRear.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetColor(AppearanceOptions::ColorUpright),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityUpright));
	leftRearBellCrank->Update(displayCar.suspension->leftRear.hardpoints[Corner::OutboardDamper],
		displayCar.suspension->leftRear.hardpoints[Corner::InboardPushrod],
		VVASEMath::NearestPointOnAxis(displayCar.suspension->leftRear.hardpoints[Corner::BellCrankPivot1],
		displayCar.suspension->leftRear.hardpoints[Corner::BellCrankPivot2] -
		displayCar.suspension->leftRear.hardpoints[Corner::BellCrankPivot1],
		displayCar.suspension->leftRear.hardpoints[Corner::InboardPushrod]),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	leftRearBarLink->Update(displayCar.suspension->leftRear.hardpoints[Corner::InboardBarLink],
		displayCar.suspension->leftRear.hardpoints[Corner::OutboardBarLink],
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarLinkDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		ShowBarLinks);
	leftRearHalfShaft->Update(displayCar.suspension->leftRear.hardpoints[Corner::InboardHalfShaft],
		displayCar.suspension->leftRear.hardpoints[Corner::OutboardHalfShaft],
		appearanceOptions.GetSize(AppearanceOptions::SizeHalfShaftDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHalfShaft),
		appearanceOptions.GetColor(AppearanceOptions::ColorHalfShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHalfShaft) && referenceCar.HasRearHalfShafts());

	// Front end
	steeringRack->Update(displayCar.suspension->rightFront.hardpoints[Corner::InboardTieRod],
		displayCar.suspension->leftFront.hardpoints[Corner::InboardTieRod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	frontSwayBar->Update(displayCar.suspension->rightFront.hardpoints[Corner::InboardBarLink],
		displayCar.suspension->leftFront.hardpoints[Corner::InboardBarLink],
		displayCar.suspension->rightFront.hardpoints[Corner::BarArmAtPivot],
		displayCar.suspension->leftFront.hardpoints[Corner::BarArmAtPivot],
		displayCar.suspension->hardpoints[Suspension::FrontBarMidPoint],
		displayCar.suspension->hardpoints[Suspension::FrontBarPivotAxis],
		displayCar.suspension->frontBarStyle,
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySwayBar));

	// Rear end
	rearSwayBar->Update(displayCar.suspension->rightRear.hardpoints[Corner::InboardBarLink],
		displayCar.suspension->leftRear.hardpoints[Corner::InboardBarLink],
		displayCar.suspension->rightRear.hardpoints[Corner::BarArmAtPivot],
		displayCar.suspension->leftRear.hardpoints[Corner::BarArmAtPivot],
		displayCar.suspension->hardpoints[Suspension::RearBarMidPoint],
		displayCar.suspension->hardpoints[Suspension::RearBarPivotAxis],
		displayCar.suspension->rearBarStyle,
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySwayBar));

	// Update the helper orb
	// Determine which of the location variables is valid
	Eigen::Vector3d helperOrbPosition(0.0, 0.0, 0.0);
	Eigen::Vector3d helperOrbOppositePosition(0.0, 0.0, 0.0);
	if (helperOrbCornerPoint != Corner::NumberOfHardpoints)
	{
		if (helperOrbLocation == Corner::LocationRightFront)
		{
			helperOrbPosition = displayCar.suspension->rightFront.hardpoints[helperOrbCornerPoint];
			helperOrbOppositePosition = displayCar.suspension->leftFront.hardpoints[helperOrbCornerPoint];
		}
		else if (helperOrbLocation == Corner::LocationLeftFront)
		{
			helperOrbPosition = displayCar.suspension->leftFront.hardpoints[helperOrbCornerPoint];
			helperOrbOppositePosition = displayCar.suspension->rightFront.hardpoints[helperOrbCornerPoint];
		}
		else if (helperOrbLocation == Corner::LocationRightRear)
		{
			helperOrbPosition = displayCar.suspension->rightRear.hardpoints[helperOrbCornerPoint];
			helperOrbOppositePosition = displayCar.suspension->leftRear.hardpoints[helperOrbCornerPoint];
		}
		else
		{
			helperOrbPosition = displayCar.suspension->leftRear.hardpoints[helperOrbCornerPoint];
			helperOrbOppositePosition = displayCar.suspension->rightRear.hardpoints[helperOrbCornerPoint];
		}
	}
	else if (helperOrbSuspensionPoint != Suspension::NumberOfHardpoints)
		helperOrbPosition = displayCar.suspension->hardpoints[helperOrbSuspensionPoint];

	helperOrb->Update(helperOrbPosition, appearanceOptions.GetSize(AppearanceOptions::SizeHelperOrbDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHelperOrb),
		appearanceOptions.GetColor(AppearanceOptions::ColorHelperOrb),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHelperOrb) && helperOrbIsActive);
	helperOrbOpposite->Update(helperOrbOppositePosition, appearanceOptions.GetSize(AppearanceOptions::SizeHelperOrbDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHelperOrb),
		appearanceOptions.GetColor(AppearanceOptions::ColorHelperOrb),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHelperOrb) && helperOrbIsActive
		&& displayCar.suspension->isSymmetric && helperOrbCornerPoint != Corner::NumberOfHardpoints);
}

//==========================================================================
// Class:			CarRenderer
// Function:		UpdateKinematicsDisplay
//
// Description:		Updates the displayed 3D kinematic objects
//
// Input Arguments:
//		Outputs	= KinematicOutputs containing the locations of all of the
//				  kinematic variables that this object displays in the scene
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CarRenderer::UpdateKinematicsDisplay(KinematicOutputs outputs)
{
	// Update the point objects
	frontRollCenter->Update(outputs.vectors[KinematicOutputs::FrontKinematicRC],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorRollMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityRollCenter));
	rearRollCenter->Update(outputs.vectors[KinematicOutputs::RearKinematicRC],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorRollMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityRollCenter));
	rightPitchCenter->Update(outputs.vectors[KinematicOutputs::RightKinematicPC],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorPitchMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPitchCenter));
	leftPitchCenter->Update(outputs.vectors[KinematicOutputs::LeftKinematicPC],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorPitchMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPitchCenter));
	rightFrontInstantCenter->Update(outputs.rightFrontVectors[KinematicOutputs::InstantCenter],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorInstantMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityInstantCenter));
	leftFrontInstantCenter->Update(outputs.leftFrontVectors[KinematicOutputs::InstantCenter],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorInstantMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityInstantCenter));
	rightRearInstantCenter->Update(outputs.rightRearVectors[KinematicOutputs::InstantCenter],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorInstantMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityInstantCenter));
	leftRearInstantCenter->Update(outputs.leftRearVectors[KinematicOutputs::InstantCenter],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorInstantMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityInstantCenter));

	// Update the vector objects
	frontRollAxis->Update(outputs.vectors[KinematicOutputs::FrontRollAxisDirection] *
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerLength) +
		outputs.vectors[KinematicOutputs::FrontKinematicRC], outputs.vectors[KinematicOutputs::FrontKinematicRC],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorRollMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityRollAxis));
	rearRollAxis->Update(outputs.vectors[KinematicOutputs::RearRollAxisDirection] *
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerLength) +
		outputs.vectors[KinematicOutputs::RearKinematicRC], outputs.vectors[KinematicOutputs::RearKinematicRC],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorRollMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityRollAxis));
	rightPitchAxis->Update(outputs.vectors[KinematicOutputs::RightPitchAxisDirection] *
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerLength) +
		outputs.vectors[KinematicOutputs::RightKinematicPC], outputs.vectors[KinematicOutputs::RightKinematicPC],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorPitchMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPitchAxis));
	leftPitchAxis->Update(outputs.vectors[KinematicOutputs::LeftPitchAxisDirection] *
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerLength) +
		outputs.vectors[KinematicOutputs::LeftKinematicPC], outputs.vectors[KinematicOutputs::LeftKinematicPC],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorPitchMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPitchAxis));
	rightFrontInstantAxis->Update(outputs.rightFrontVectors[KinematicOutputs::InstantAxisDirection] *
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerLength) +
		outputs.rightFrontVectors[KinematicOutputs::InstantCenter],
		outputs.rightFrontVectors[KinematicOutputs::InstantCenter],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorInstantMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityInstantAxis));
	leftFrontInstantAxis->Update(outputs.leftFrontVectors[KinematicOutputs::InstantAxisDirection] *
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerLength) +
		outputs.leftFrontVectors[KinematicOutputs::InstantCenter],
		outputs.leftFrontVectors[KinematicOutputs::InstantCenter],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorInstantMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityInstantAxis));
	rightRearInstantAxis->Update(outputs.rightRearVectors[KinematicOutputs::InstantAxisDirection] *
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerLength) +
		outputs.rightRearVectors[KinematicOutputs::InstantCenter],
		outputs.rightRearVectors[KinematicOutputs::InstantCenter],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorInstantMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityInstantAxis));
	leftRearInstantAxis->Update(outputs.leftRearVectors[KinematicOutputs::InstantAxisDirection] *
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerLength) +
		outputs.leftRearVectors[KinematicOutputs::InstantCenter],
		outputs.leftRearVectors[KinematicOutputs::InstantCenter],
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeMarkerTipLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionMarker),
		appearanceOptions.GetColor(AppearanceOptions::ColorInstantMarker),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityInstantAxis));
}

//==========================================================================
// Class:			CarRenderer
// Function:		CreateActors
//
// Description:		Creates all of the objects that will be rendered in the
//					scene.
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
void CarRenderer::CreateActors()
{
	// Create the origin and ground plane
	origin = new Origin(*this);
	groundPlane = new Plane3D(*this);

	// right front corner
	rightFrontLowerAArm = new AArm(*this);
	rightFrontUpperAArm = new AArm(*this);
	rightFrontPushrod = new Link(*this);
	rightFrontTieRod = new Link(*this);
	rightFrontTire = new Tire3D(*this);
	rightFrontDamper = new Damper3D(*this);
	rightFrontSpring = new Spring3D(*this);
	rightFrontUpright = new Triangle3D(*this);
	rightFrontBellCrank = new Triangle3D(*this);
	rightFrontBarLink = new Link(*this);
	rightFrontHalfShaft = new Link(*this);

	// left front corner
	leftFrontLowerAArm = new AArm(*this);
	leftFrontUpperAArm = new AArm(*this);
	leftFrontPushrod = new Link(*this);
	leftFrontTieRod = new Link(*this);
	leftFrontTire = new Tire3D(*this);
	leftFrontDamper = new Damper3D(*this);
	leftFrontSpring = new Spring3D(*this);
	leftFrontUpright = new Triangle3D(*this);
	leftFrontBellCrank = new Triangle3D(*this);
	leftFrontBarLink = new Link(*this);
	leftFrontHalfShaft = new Link(*this);

	// right rear corner
	rightRearLowerAArm = new AArm(*this);
	rightRearUpperAArm = new AArm(*this);
	rightRearPushrod = new Link(*this);
	rightRearTieRod = new Link(*this);
	rightRearTire = new Tire3D(*this);
	rightRearDamper = new Damper3D(*this);
	rightRearSpring = new Spring3D(*this);
	rightRearUpright = new Triangle3D(*this);
	rightRearBellCrank = new Triangle3D(*this);
	rightRearBarLink = new Link(*this);
	rightRearHalfShaft = new Link(*this);

	// left rear corner
	leftRearLowerAArm = new AArm(*this);
	leftRearUpperAArm = new AArm(*this);
	leftRearPushrod = new Link(*this);
	leftRearTieRod = new Link(*this);
	leftRearTire = new Tire3D(*this);
	leftRearDamper = new Damper3D(*this);
	leftRearSpring = new Spring3D(*this);
	leftRearUpright = new Triangle3D(*this);
	leftRearBellCrank = new Triangle3D(*this);
	leftRearBarLink = new Link(*this);
	leftRearHalfShaft = new Link(*this);

	// Front end
	steeringRack = new Link(*this);
	frontSwayBar = new Swaybar3D(*this);

	// Rear end
	rearSwayBar = new Swaybar3D(*this);

	// Kinematic display objects
	frontRollCenter = new Point3D(*this);
	rearRollCenter = new Point3D(*this);
	rightPitchCenter = new Point3D(*this);
	leftPitchCenter = new Point3D(*this);
	rightFrontInstantCenter = new Point3D(*this);
	leftFrontInstantCenter = new Point3D(*this);
	rightRearInstantCenter = new Point3D(*this);
	leftRearInstantCenter = new Point3D(*this);

	frontRollAxis = new Vector3D(*this);
	rearRollAxis = new Vector3D(*this);
	rightPitchAxis = new Vector3D(*this);
	leftPitchAxis = new Vector3D(*this);
	rightFrontInstantAxis = new Vector3D(*this);
	leftFrontInstantAxis = new Vector3D(*this);
	rightRearInstantAxis = new Vector3D(*this);
	leftRearInstantAxis = new Vector3D(*this);

	// Helper orb
	helperOrb = new Point3D(*this);
	helperOrbOpposite = new Point3D(*this);
}

//==========================================================================
// Class:			CarRenderer
// Function:		SetHelperOrbPosition
//
// Description:		Updates the position of the helper orb in the scene.
//					This takes five arguments, of which only one is (should
//					be) valid.
//
// Input Arguments:
//		cornerPoint		= const Corner::Hardpoints& identifying the point at the
//						  corner of the car
//		cornerLocation	= const Corner::Location& specifying the active corner
//		suspensionPoint	= const Suspension::Hardpoints& identifying the point on
//						  the central part of the car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CarRenderer::SetHelperOrbPosition(const Corner::Hardpoints &cornerPoint,
										const Corner::Location &cornerLocation,
										const Suspension::Hardpoints &suspensionPoint)
{
	// Update the class members that describe the orb's position
	helperOrbCornerPoint = cornerPoint;
	helperOrbLocation = cornerLocation;
	helperOrbSuspensionPoint = suspensionPoint;

	// If we set the position, we'll assume that we want it to be active
	helperOrbIsActive = true;
}

//==========================================================================
// Class:			CarRenderer
// Function:		TraceClickToHardpoint
//
// Description:		Returns the hardpoint ID for the point beneath the
//					specified coordinate.
//
// Input Arguments:
//		x	= const double&
//		y	= const double&
//
// Output Arguments:
//		suspensionPoint	= Suspension::Hardpoint&
//		leftFrontPoint	= Corner::Hardpoint&
//		leftFrontPoint	= Corner::Hardpoint&
//		leftFrontPoint	= Corner::Hardpoint&
//		leftFrontPoint	= Corner::Hardpoint&
//
// Return Value:
//		bool, true if a hardpoint lies beneath the specified point, false otherwise
//
//==========================================================================
bool CarRenderer::TraceClickToHardpoint(const double& x, const double& y,
	Suspension::Hardpoints& suspensionPoint,
	Corner::Hardpoints& leftFrontPoint, Corner::Hardpoints& rightFrontPoint,
	Corner::Hardpoints& leftRearPoint, Corner::Hardpoints& rightRearPoint) const
{
	Eigen::Vector3d point, direction;
	if (!GetLineUnderPoint(x, y, point, direction))
		return false;

	std::vector<const Primitive*> intersected(IntersectWithPrimitive(point, direction));

	while (intersected.size() > 0)
	{
		const Primitive* selected(GetClosestPrimitive(intersected));

		// Prune away actors that we don't want the user to interact with directly
		if (origin->ContainsThisActor(selected) ||
			groundPlane->ContainsThisActor(selected) ||
			rightFrontTire->ContainsThisActor(selected) ||
			rightRearTire->ContainsThisActor(selected) ||
			leftFrontTire->ContainsThisActor(selected) ||
			leftRearTire->ContainsThisActor(selected) ||
			rightFrontUpright->ContainsThisActor(selected) ||
			rightRearUpright->ContainsThisActor(selected) ||
			leftFrontUpright->ContainsThisActor(selected) ||
			leftRearUpright->ContainsThisActor(selected) ||
			rightFrontBellCrank->ContainsThisActor(selected) ||
			rightRearBellCrank->ContainsThisActor(selected) ||
			leftFrontBellCrank->ContainsThisActor(selected) ||
			leftRearBellCrank->ContainsThisActor(selected) ||
			frontRollCenter->ContainsThisActor(selected) ||
			rearRollCenter->ContainsThisActor(selected) ||
			rightPitchCenter->ContainsThisActor(selected) ||
			leftPitchCenter->ContainsThisActor(selected) ||
			rightFrontInstantCenter->ContainsThisActor(selected) ||
			leftFrontInstantCenter->ContainsThisActor(selected) ||
			rightRearInstantCenter->ContainsThisActor(selected) ||
			leftRearInstantCenter->ContainsThisActor(selected) ||
			frontRollAxis->ContainsThisActor(selected) ||
			rearRollAxis->ContainsThisActor(selected) ||
			rightPitchAxis->ContainsThisActor(selected) ||
			leftPitchAxis->ContainsThisActor(selected) ||
			rightFrontInstantAxis->ContainsThisActor(selected) ||
			leftFrontInstantAxis->ContainsThisActor(selected) ||
			rightRearInstantAxis->ContainsThisActor(selected) ||
			leftRearInstantAxis->ContainsThisActor(selected) ||
			helperOrb->ContainsThisActor(selected) ||
			helperOrbOpposite->ContainsThisActor(selected))
		{
			intersected.erase(std::find(intersected.begin(), intersected.end(), selected));
			continue;
		}

		GetSelectedHardpoint(point, direction, selected, suspensionPoint,
			leftFrontPoint, rightFrontPoint, leftRearPoint, rightRearPoint);
		return true;
	}

	return false;
}

//==========================================================================
// Class:			CarRenderer
// Function:		OnLeftClick
//
// Description:		Handles left click events for this class.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CarRenderer::OnLeftClick(wxMouseEvent& event)
{
	event.Skip();
	if (isInteracting || sawLeftButtonGoDown)
		return;

	Suspension::Hardpoints suspensionPoint;
	Corner::Hardpoints leftFrontPoint;
	Corner::Hardpoints rightFrontPoint;
	Corner::Hardpoints leftRearPoint;
	Corner::Hardpoints rightRearPoint;
	if (!TraceClickToHardpoint(event.GetX(), event.GetY(), suspensionPoint,
		leftFrontPoint, rightFrontPoint, leftRearPoint, rightRearPoint))
		return;

	// TODO:  Highlight the corresponding row in the edit panel
	// If the edit panel is not visible, only move the helper orb?

	// Set the position of the helper orb
	/*static_cast<CarRenderer*>(parent.GetParent().GetCurrentObject()->GetNotebookTab())->SetHelperOrbPosition(
		Corner::NumberOfHardpoints, Corner::LocationRightFront, (Suspension::Hardpoints)(event.GetRow() - 1));*/
}

//==========================================================================
// Class:			CarRenderer
// Function:		OnRightClick
//
// Description:		Handles right click events for this class.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CarRenderer::OnRightClick(wxMouseEvent& event)
{
	event.Skip();
	if (isInteracting)
		return;

	if (!TraceClickToHardpoint(event.GetX(), event.GetY(), suspensionPoint,
		leftFrontPoint, rightFrontPoint, leftRearPoint, rightRearPoint))
		return;

	wxMenu* contextMenu(BuildContextMenu());
	PopupMenu(contextMenu);
	delete contextMenu;

	// TODO:  Would also be cool to have a Solve-> menu where the user
	// specifies which direction(s) to move the point and what output
	// to set to what, then have it go (like, "set RC here by moving this point").
}

//==========================================================================
// Class:			CarRenderer
// Function:		GetLineUnderPoint
//
// Description:		Finds the equation of the line under the specified point
//					on the screen.
//
// Input Arguments:
//		x	= const double&
//		y	= const double&
//
// Output Arguments:
//		point		= Eigen::Vector3d&
//		direction	= Eigen::Vector3d&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool CarRenderer::GetLineUnderPoint(const double& x, const double& y,
	Eigen::Vector3d& point, Eigen::Vector3d& direction) const
{
	const double zOrdinate1(0.0), zOrdinate2(1.0);// Must be between 0 and 1 and be different from each other
	if (!Unproject(x, y, zOrdinate1, point))
		return false;

	Eigen::Vector3d point2;
	if (!Unproject(x, y, zOrdinate2, point2))
		return false;

	direction = (point2 - point).Normalize();
	return true;
}

//==========================================================================
// Class:			CarRenderer
// Function:		IntersectWithPrimitive
//
// Description:		Generates a list of primitives whose bounding volume is
//					intersected by the specified line.
//
// Input Arguments:
//		point		= const Eigen::Vector3d&
//		direction	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		std::vector<const Primitive*>
//
//==========================================================================
std::vector<const Primitive*> CarRenderer::IntersectWithPrimitive(
	const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	std::vector<const Primitive*> intersected;

	unsigned int i;
	for (i = 0; i < primitiveList.GetCount(); i++)
	{
		if (primitiveList[i]->IsIntersectedBy(point, direction))
			intersected.push_back(primitiveList[i]);
	}

	return intersected;
}

//==========================================================================
// Class:			CarRenderer
// Function:		GetClosestPrimitive
//
// Description:		Selects the primitive closest to the eyepoint.
//
// Input Arguments:
//		intersected	= const std::vector<const Primitive*>&
//
// Output Arguments:
//		None
//
// Return Value:
//		const Primitive*
//
//==========================================================================
const Primitive* CarRenderer::GetClosestPrimitive(const std::vector<const Primitive*>& intersected) const
{
	/*double closestDistance(std::numeric_limits<double>::max());
	unsigned int i;
	for (i = 0; i < intersected.size(); i++)
	{
		//intersected[i]->
		// TODO:  Implement
	}*/

	return intersected.front();
}

//==========================================================================
// Class:			CarRenderer
// Function:		GetSelectedHardpoint
//
// Description:		Returns the hardpoint ID corresponding to the selected
//					primitive.
//
// Input Arguments:
//		point			= const Eigen::Vector3d&
//		direction		= const Eigen::Vector3d&
//		selected		= const Primitive&
//
// Output Arguments:
//		suspensionPoint	= Suspension::Hardpoint&
//		leftFrontPoint	= Corner::Hardpoint&
//		leftFrontPoint	= Corner::Hardpoint&
//		leftFrontPoint	= Corner::Hardpoint&
//		leftFrontPoint	= Corner::Hardpoint&
//
// Return Value:
//		None
//
//==========================================================================
void CarRenderer::GetSelectedHardpoint(const Eigen::Vector3d& point, const Eigen::Vector3d& direction,
	const Primitive* selected, Suspension::Hardpoints& suspensionPoint,
	Corner::Hardpoints& leftFrontPoint, Corner::Hardpoints& rightFrontPoint,
	Corner::Hardpoints& leftRearPoint, Corner::Hardpoints& rightRearPoint) const
{
	suspensionPoint = Suspension::NumberOfHardpoints;
	leftFrontPoint = Corner::NumberOfHardpoints;
	rightFrontPoint = Corner::NumberOfHardpoints;
	leftRearPoint = Corner::NumberOfHardpoints;
	rightRearPoint = Corner::NumberOfHardpoints;

	Eigen::Vector3d closestPoint;

	if (rightFrontLowerAArm->ContainsThisActor(selected))
		closestPoint = rightFrontLowerAArm->FindClosestPoint(point, direction);
	else if (rightFrontUpperAArm->ContainsThisActor(selected))
		closestPoint = rightFrontUpperAArm->FindClosestPoint(point, direction);
	else if (rightFrontTieRod->ContainsThisActor(selected))
		closestPoint = rightFrontTieRod->FindClosestPoint(point, direction);
	else if (rightFrontPushrod->ContainsThisActor(selected))
		closestPoint = rightFrontPushrod->FindClosestPoint(point, direction);
	else if (rightFrontDamper->ContainsThisActor(selected))
		closestPoint = rightFrontDamper->FindClosestPoint(point, direction);
	else if (rightFrontSpring->ContainsThisActor(selected))
		closestPoint = rightFrontSpring->FindClosestPoint(point, direction);
	else if (rightFrontBarLink->ContainsThisActor(selected))
		closestPoint = rightFrontBarLink->FindClosestPoint(point, direction);
	else if (rightFrontHalfShaft->ContainsThisActor(selected))
		closestPoint = rightFrontHalfShaft->FindClosestPoint(point, direction);
	else if (leftFrontLowerAArm->ContainsThisActor(selected))
		closestPoint = leftFrontLowerAArm->FindClosestPoint(point, direction);
	else if (leftFrontUpperAArm->ContainsThisActor(selected))
		closestPoint = leftFrontUpperAArm->FindClosestPoint(point, direction);
	else if (leftFrontTieRod->ContainsThisActor(selected))
		closestPoint = leftFrontTieRod->FindClosestPoint(point, direction);
	else if (leftFrontPushrod->ContainsThisActor(selected))
		closestPoint = leftFrontPushrod->FindClosestPoint(point, direction);
	else if (leftFrontDamper->ContainsThisActor(selected))
		closestPoint = leftFrontDamper->FindClosestPoint(point, direction);
	else if (leftFrontSpring->ContainsThisActor(selected))
		closestPoint = leftFrontSpring->FindClosestPoint(point, direction);
	else if (leftFrontBarLink->ContainsThisActor(selected))
		closestPoint = leftFrontBarLink->FindClosestPoint(point, direction);
	else if (leftFrontHalfShaft->ContainsThisActor(selected))
		closestPoint = leftFrontHalfShaft->FindClosestPoint(point, direction);
	else if (rightRearLowerAArm->ContainsThisActor(selected))
		closestPoint = rightRearLowerAArm->FindClosestPoint(point, direction);
	else if (rightRearUpperAArm->ContainsThisActor(selected))
		closestPoint = rightRearUpperAArm->FindClosestPoint(point, direction);
	else if (rightRearTieRod->ContainsThisActor(selected))
		closestPoint = rightRearTieRod->FindClosestPoint(point, direction);
	else if (rightRearPushrod->ContainsThisActor(selected))
		closestPoint = rightRearPushrod->FindClosestPoint(point, direction);
	else if (rightRearDamper->ContainsThisActor(selected))
		closestPoint = rightRearDamper->FindClosestPoint(point, direction);
	else if (rightRearSpring->ContainsThisActor(selected))
		closestPoint = rightRearSpring->FindClosestPoint(point, direction);
	else if (rightRearBarLink->ContainsThisActor(selected))
		closestPoint = rightRearBarLink->FindClosestPoint(point, direction);
	else if (rightRearHalfShaft->ContainsThisActor(selected))
		closestPoint = rightRearHalfShaft->FindClosestPoint(point, direction);
	else if (leftRearLowerAArm->ContainsThisActor(selected))
		closestPoint = leftRearLowerAArm->FindClosestPoint(point, direction);
	else if (leftRearUpperAArm->ContainsThisActor(selected))
		closestPoint = leftRearUpperAArm->FindClosestPoint(point, direction);
	else if (leftRearTieRod->ContainsThisActor(selected))
		closestPoint = leftRearTieRod->FindClosestPoint(point, direction);
	else if (leftRearPushrod->ContainsThisActor(selected))
		closestPoint = leftRearPushrod->FindClosestPoint(point, direction);
	else if (leftRearDamper->ContainsThisActor(selected))
		closestPoint = leftRearDamper->FindClosestPoint(point, direction);
	else if (leftRearSpring->ContainsThisActor(selected))
		closestPoint = leftRearSpring->FindClosestPoint(point, direction);
	else if (leftRearBarLink->ContainsThisActor(selected))
		closestPoint = leftRearBarLink->FindClosestPoint(point, direction);
	else if (leftRearHalfShaft->ContainsThisActor(selected))
		closestPoint = leftRearHalfShaft->FindClosestPoint(point, direction);
	else if (steeringRack->ContainsThisActor(selected))
		closestPoint = steeringRack->FindClosestPoint(point, direction);
	/*else if (frontSwayBar->ContainsThisActor(selected))
		closestPoint = frontSwayBar->FindClosestPoint(point, direction);*/// Points handled by links
	else if (frontThirdSpring->ContainsThisActor(selected))
		closestPoint = frontThirdSpring->FindClosestPoint(point, direction);
	else if (frontThirdDamper->ContainsThisActor(selected))
		closestPoint = frontThirdDamper->FindClosestPoint(point, direction);
	/*else if (rearSwayBar->ContainsThisActor(selected))
		closestPoint = rearSwayBar->FindClosestPoint(point, direction);*/// Points handled by links
	else if (rearThirdSpring->ContainsThisActor(selected))
		closestPoint = rearThirdSpring->FindClosestPoint(point, direction);
	else if (rearThirdDamper->ContainsThisActor(selected))
		closestPoint = rearThirdDamper->FindClosestPoint(point, direction);

	// TODO:  Now what... need to find hardpoint from vector point
	unsigned int i;
	for (i = 0; i < Corner::NumberOfHardpoints; i++)
	{
		if (VVASEMath::IsZero(displayCar.suspension->leftFront.hardpoints[i] - closestPoint))
		{
			leftFrontPoint = static_cast<Corner::Hardpoints>(i);
			return;
		}
		else if (VVASEMath::IsZero(displayCar.suspension->rightFront.hardpoints[i] - closestPoint))
		{
			rightFrontPoint = static_cast<Corner::Hardpoints>(i);
			return;
		}
		else if (VVASEMath::IsZero(displayCar.suspension->leftRear.hardpoints[i] - closestPoint))
		{
			leftRearPoint = static_cast<Corner::Hardpoints>(i);
			return;
		}
		else if (VVASEMath::IsZero(displayCar.suspension->rightRear.hardpoints[i] - closestPoint))
		{
			rightRearPoint = static_cast<Corner::Hardpoints>(i);
			return;
		}
	}

	for (i = 0; i < Suspension::NumberOfHardpoints; i++)
	{
		if (VVASEMath::IsZero(displayCar.suspension->hardpoints[i] - closestPoint))
		{
			suspensionPoint = static_cast<Suspension::Hardpoints>(i);
			return;
		}
	}
}

//==========================================================================
// Class:			CarRenderer
// Function:		BuildContextMenu
//
// Description:		Builds a context menu for a hardpoint.
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
wxMenu* CarRenderer::BuildContextMenu() const
{
	wxMenu* menu(new wxMenu);
	menu->Append(idContextEdit, _T("Edit"));
	return menu;
}

//==========================================================================
// Class:			CarRenderer
// Function:		DoEditPointDialog
//
// Description:		Displays a dialog for editing hardpoints.
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
void CarRenderer::DoEditPointDialog()
{
	Eigen::Vector3d* pointToEdit;
	wxString pointName;
	if (suspensionPoint != Suspension::NumberOfHardpoints)
	{
		pointToEdit = &car.GetOriginalCar().suspension->hardpoints[suspensionPoint];
		pointName = Suspension::GetHardpointName(suspensionPoint);
	}
	else if (leftFrontPoint != Corner::NumberOfHardpoints)
	{
		if (car.GetOriginalCar().suspension->isSymmetric)
			pointToEdit = &car.GetOriginalCar().suspension->rightFront.hardpoints[leftFrontPoint];
		else
			pointToEdit = &car.GetOriginalCar().suspension->leftFront.hardpoints[leftFrontPoint];

		pointName = Corner::GetLocationName(Corner::LocationLeftFront)
			+ _T(" ") + Corner::GetHardpointName(leftFrontPoint);
	}
	else if (rightFrontPoint != Corner::NumberOfHardpoints)
	{
		pointToEdit = &car.GetOriginalCar().suspension->rightFront.hardpoints[rightFrontPoint];
		pointName = Corner::GetLocationName(Corner::LocationRightFront)
			+ _T(" ") + Corner::GetHardpointName(rightFrontPoint);
	}
	else if (leftRearPoint != Corner::NumberOfHardpoints)
	{
		if (car.GetOriginalCar().suspension->isSymmetric)
			pointToEdit = &car.GetOriginalCar().suspension->rightRear.hardpoints[leftRearPoint];
		else
			pointToEdit = &car.GetOriginalCar().suspension->leftRear.hardpoints[leftRearPoint];

		pointName = Corner::GetLocationName(Corner::LocationLeftRear)
			+ _T(" ") + Corner::GetHardpointName(leftRearPoint);
	}
	else if (rightRearPoint != Corner::NumberOfHardpoints)
	{
		pointToEdit = &car.GetOriginalCar().suspension->rightRear.hardpoints[rightRearPoint];
		pointName = Corner::GetLocationName(Corner::LocationRightRear)
			+ _T(" ") + Corner::GetHardpointName(rightRearPoint);
	}
	else
		return;

	Eigen::Vector3d tempPoint(*pointToEdit);
	VectorEditDialog dialog(mainFrame, tempPoint, pointName);

	if (dialog.ShowModal() != wxOK)
		return;

	if (VVASEMath::IsZero(*pointToEdit - tempPoint))
		return;

	wxMutex& mutex(car.GetOriginalCar().GetMutex());
	mutex.Lock();
	mainFrame.GetUndoRedoStack().AddOperation(mainFrame.GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeVector, pointToEdit);
	*pointToEdit = tempPoint;
	car.GetOriginalCar().suspension->UpdateSymmetry();

	mutex.Unlock();

	car.SetModified();

	mainFrame.UpdateAnalysis();
	mainFrame.UpdateOutputPanel();
}

//==========================================================================
// Class:			CarRenderer
// Function:		OnContextEdit
//
// Description:		Handles context menu Edit click events.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CarRenderer::OnContextEdit(wxCommandEvent& WXUNUSED(event))
{
	DoEditPointDialog();
}

}// namespace VVASE
