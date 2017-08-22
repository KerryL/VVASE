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
#include "VVASE/gui/components/mainFrame.h"
#include "VVASE/gui/dialogs/vectorEditDialog.h"
#include "VVASE/core/analysis/kinematicOutputs.h"
#include "VVASE/core/utilities/debugLog.h"
#include "VVASE/gui/renderer/3dcar/debugShape.h"
#include "VVASE/core/utilities/geometryMath.h"

// LibPlot2D headers
#include <lp2d/renderer/primitives/primitive.h>

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
	SetView3D(true);
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
	MutexLocker displayLock(displayCar.GetMutex());
	DebugLog::GetInstance()->Log(_T("CarRenderer::UpdateCarDisplay (displayLock)"), 0);
	MutexLocker referenceLock(referenceCar.GetMutex());
	DebugLog::GetInstance()->Log(_T("CarRenderer::UpdateCarDisplay (referenceLock)"), 0);

	// Update the origin
	origin->Update(appearanceOptions.GetSize(AppearanceOptions::SizeOriginShaftLength),
		appearanceOptions.GetSize(AppearanceOptions::SizeOriginShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeOriginTipLength),
		appearanceOptions.GetSize(AppearanceOptions::SizeOriginTipDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionOrigin),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityOrigin));

	const auto* referenceSuspension(referenceCar.GetSubsystem<Suspension>());
	const auto* referenceTires(referenceCar.GetSubsystem<TireSet>());

	// Figure out how big the ground plane should be
	double groundPlaneScaleUp = 1.1;
	double rightMostPoint = std::max(referenceSuspension->rightFront.hardpoints[Corner::ContactPatch].y() +
		referenceTires->rightFront->width / 2.0,
		referenceSuspension->rightRear.hardpoints[Corner::ContactPatch].y() +
		referenceTires->rightRear->width / 2.0);
	double leftMostPoint = std::min(referenceSuspension->leftFront.hardpoints[Corner::ContactPatch].y() -
		referenceTires->leftFront->width / 2.0,
		referenceSuspension->leftRear.hardpoints[Corner::ContactPatch].y() -
		referenceTires->leftRear->width / 2.0);
	double frontMostPoint = std::min(referenceSuspension->rightFront.hardpoints[Corner::ContactPatch].x() -
		referenceTires->rightFront->diameter / 2.0,
		referenceSuspension->leftFront.hardpoints[Corner::ContactPatch].x() -
		referenceTires->leftFront->diameter / 2.0);
	double rearMostPoint = std::max(referenceSuspension->rightRear.hardpoints[Corner::ContactPatch].x() +
		referenceTires->rightRear->diameter / 2.0,
		referenceSuspension->leftRear.hardpoints[Corner::ContactPatch].x() +
		referenceTires->leftRear->diameter / 2.0);

	double xLength = fabs(frontMostPoint - rearMostPoint) * groundPlaneScaleUp;
	double yLength = fabs(rightMostPoint - leftMostPoint) * groundPlaneScaleUp;
	double xOffset = (rearMostPoint - frontMostPoint) / 2.0 + frontMostPoint;
	groundPlane->Update(xLength, yLength, xOffset,
		appearanceOptions.GetColor(AppearanceOptions::ColorGroundPlane),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityGroundPlane));

	auto* displaySuspension(displayCar.GetSubsystem<Suspension>());
	auto* displayTires(displayCar.GetSubsystem<TireSet>());

	// Copy the tire parameters from the reference car
	displayTires->rightFront->width = referenceTires->rightFront->width;
	displayTires->leftFront->width = referenceTires->leftFront->width;
	displayTires->rightRear->width = referenceTires->rightRear->width;
	displayTires->leftRear->width = referenceTires->leftRear->width;

	// Determine whether or not the front sway bar links should be visible
	bool ShowBarLinks = true;
	if (displaySuspension->frontBarStyle == Suspension::SwayBarNone ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySwayBar))
		ShowBarLinks = false;

	// Right front corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	bool ShowBellCranksPushrods = true;
	if (displaySuspension->rightFront.actuationType == Corner::ActuationOutboardRockerArm ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	targetNormal = VVASE::Math::GetPlaneNormal(displaySuspension->rightFront.hardpoints[Corner::LowerBallJoint],
							displaySuspension->rightFront.hardpoints[Corner::UpperBallJoint],
							displaySuspension->rightFront.hardpoints[Corner::OutboardTieRod]);
	originalNormal = VVASE::Math::GetPlaneNormal(referenceSuspension->rightFront.hardpoints[Corner::LowerBallJoint],
							referenceSuspension->rightFront.hardpoints[Corner::UpperBallJoint],
							referenceSuspension->rightFront.hardpoints[Corner::OutboardTieRod]);

	// We also need to account for toe and camber settings for the targetNormal - apply camber first
	// NOTE:  This corner is on the right side of the car - we flip the sign on the camber and toe angles
	GeometryMath::Rotate(targetNormal, -displaySuspension->rightFront.staticCamber, Eigen::Vector3d::UnitX());
	GeometryMath::Rotate(targetNormal, -displaySuspension->rightFront.staticToe, Eigen::Vector3d::UnitZ());

	// Now continue with the update for this corner
	rightFrontLowerAArm->Update(displaySuspension->rightFront.hardpoints[Corner::LowerFrontTubMount],
		displaySuspension->rightFront.hardpoints[Corner::LowerBallJoint],
		displaySuspension->rightFront.hardpoints[Corner::LowerRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	rightFrontUpperAArm->Update(displaySuspension->rightFront.hardpoints[Corner::UpperFrontTubMount],
		displaySuspension->rightFront.hardpoints[Corner::UpperBallJoint],
		displaySuspension->rightFront.hardpoints[Corner::UpperRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	rightFrontPushrod->Update(displaySuspension->rightFront.hardpoints[Corner::InboardPushrod],
		displaySuspension->rightFront.hardpoints[Corner::OutboardPushrod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	rightFrontTieRod->Update(displaySuspension->rightFront.hardpoints[Corner::InboardTieRod],
		displaySuspension->rightFront.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	rightFrontTire->Update(displaySuspension->rightFront.hardpoints[Corner::ContactPatch],
		displaySuspension->rightFront.hardpoints[Corner::WheelCenter], originalNormal, targetNormal,
		displayTires->rightFront->width,
		appearanceOptions.GetSize(AppearanceOptions::SizeTireInsideDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionTire),
		appearanceOptions.GetColor(AppearanceOptions::ColorTire),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTire));
	rightFrontDamper->Update(displaySuspension->rightFront.hardpoints[Corner::OutboardDamper],
		displaySuspension->rightFront.hardpoints[Corner::InboardDamper],
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperBody),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityDamper));
	rightFrontSpring->Update(displaySuspension->rightFront.hardpoints[Corner::InboardSpring],
		displaySuspension->rightFront.hardpoints[Corner::OutboardSpring],
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringEndPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorSpring),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySpring));
	rightFrontUpright->Update(displaySuspension->rightFront.hardpoints[Corner::LowerBallJoint],
		displaySuspension->rightFront.hardpoints[Corner::UpperBallJoint],
		displaySuspension->rightFront.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetColor(AppearanceOptions::ColorUpright),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityUpright));
	rightFrontBellCrank->Update(displaySuspension->rightFront.hardpoints[Corner::OutboardDamper],
		displaySuspension->rightFront.hardpoints[Corner::InboardPushrod],
		VVASE::Math::NearestPointOnAxis(displaySuspension->rightFront.hardpoints[Corner::BellCrankPivot1],
		displaySuspension->rightFront.hardpoints[Corner::BellCrankPivot2] -
		displaySuspension->rightFront.hardpoints[Corner::BellCrankPivot1],
		displaySuspension->rightFront.hardpoints[Corner::InboardPushrod]),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	rightFrontBarLink->Update(displaySuspension->rightFront.hardpoints[Corner::InboardBarLink],
		displaySuspension->rightFront.hardpoints[Corner::OutboardBarLink],
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarLinkDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		ShowBarLinks);
	rightFrontHalfShaft->Update(displaySuspension->rightFront.hardpoints[Corner::InboardHalfShaft],
		displaySuspension->rightFront.hardpoints[Corner::OutboardHalfShaft],
		appearanceOptions.GetSize(AppearanceOptions::SizeHalfShaftDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHalfShaft),
		appearanceOptions.GetColor(AppearanceOptions::ColorHalfShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHalfShaft) && referenceCar.HasFrontHalfShafts());

	// left front corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (displaySuspension->leftFront.actuationType == Corner::ActuationOutboardRockerArm ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	targetNormal = VVASE::Math::GetPlaneNormal(displaySuspension->leftFront.hardpoints[Corner::LowerBallJoint],
							displaySuspension->leftFront.hardpoints[Corner::UpperBallJoint],
							displaySuspension->leftFront.hardpoints[Corner::OutboardTieRod]);
	originalNormal = VVASE::Math::GetPlaneNormal(referenceSuspension->leftFront.hardpoints[Corner::LowerBallJoint],
							referenceSuspension->leftFront.hardpoints[Corner::UpperBallJoint],
							referenceSuspension->leftFront.hardpoints[Corner::OutboardTieRod]);

	// We also need to account for toe and camber settings for the targetNormal - apply camber first
	GeometryMath::Rotate(targetNormal, displaySuspension->leftFront.staticCamber, Eigen::Vector3d::UnitX());
	GeometryMath::Rotate(targetNormal, displaySuspension->leftFront.staticToe, Eigen::Vector3d::UnitZ());

	// Now continue with the update for this corner
	leftFrontLowerAArm->Update(displaySuspension->leftFront.hardpoints[Corner::LowerFrontTubMount],
		displaySuspension->leftFront.hardpoints[Corner::LowerBallJoint],
		displaySuspension->leftFront.hardpoints[Corner::LowerRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	leftFrontUpperAArm->Update(displaySuspension->leftFront.hardpoints[Corner::UpperFrontTubMount],
		displaySuspension->leftFront.hardpoints[Corner::UpperBallJoint],
		displaySuspension->leftFront.hardpoints[Corner::UpperRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	leftFrontPushrod->Update(displaySuspension->leftFront.hardpoints[Corner::InboardPushrod],
		displaySuspension->leftFront.hardpoints[Corner::OutboardPushrod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	leftFrontTieRod->Update(displaySuspension->leftFront.hardpoints[Corner::InboardTieRod],
		displaySuspension->leftFront.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	leftFrontTire->Update(displaySuspension->leftFront.hardpoints[Corner::ContactPatch],
		displaySuspension->leftFront.hardpoints[Corner::WheelCenter], originalNormal, targetNormal,
		displayTires->leftFront->width,
		appearanceOptions.GetSize(AppearanceOptions::SizeTireInsideDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionTire),
		appearanceOptions.GetColor(AppearanceOptions::ColorTire),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTire));
	leftFrontDamper->Update(displaySuspension->leftFront.hardpoints[Corner::OutboardDamper],
		displaySuspension->leftFront.hardpoints[Corner::InboardDamper],
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperBody),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityDamper));
	leftFrontSpring->Update(displaySuspension->leftFront.hardpoints[Corner::InboardSpring],
		displaySuspension->leftFront.hardpoints[Corner::OutboardSpring],
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringEndPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorSpring),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySpring));
	leftFrontUpright->Update(displaySuspension->leftFront.hardpoints[Corner::LowerBallJoint],
		displaySuspension->leftFront.hardpoints[Corner::UpperBallJoint],
		displaySuspension->leftFront.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetColor(AppearanceOptions::ColorUpright),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityUpright));
	leftFrontBellCrank->Update(displaySuspension->leftFront.hardpoints[Corner::OutboardDamper],
		displaySuspension->leftFront.hardpoints[Corner::InboardPushrod],
		VVASE::Math::NearestPointOnAxis(displaySuspension->leftFront.hardpoints[Corner::BellCrankPivot1],
		displaySuspension->leftFront.hardpoints[Corner::BellCrankPivot2] -
		displaySuspension->leftFront.hardpoints[Corner::BellCrankPivot1],
		displaySuspension->leftFront.hardpoints[Corner::InboardPushrod]),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	leftFrontBarLink->Update(displaySuspension->leftFront.hardpoints[Corner::InboardBarLink],
		displaySuspension->leftFront.hardpoints[Corner::OutboardBarLink],
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarLinkDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		ShowBarLinks);
	leftFrontHalfShaft->Update(displaySuspension->leftFront.hardpoints[Corner::InboardHalfShaft],
		displaySuspension->leftFront.hardpoints[Corner::OutboardHalfShaft],
		appearanceOptions.GetSize(AppearanceOptions::SizeHalfShaftDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHalfShaft),
		appearanceOptions.GetColor(AppearanceOptions::ColorHalfShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHalfShaft) && referenceCar.HasFrontHalfShafts());

	// Determine whether or not the rear sway bar links should be visible
	if (displaySuspension->rearBarStyle == Suspension::SwayBarNone ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySwayBar))
		ShowBarLinks = false;
	else
		ShowBarLinks = true;

	// right rear corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (displaySuspension->rightRear.actuationType == Corner::ActuationOutboardRockerArm ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	targetNormal = VVASE::Math::GetPlaneNormal(displaySuspension->rightRear.hardpoints[Corner::LowerBallJoint],
							displaySuspension->rightRear.hardpoints[Corner::UpperBallJoint],
							displaySuspension->rightRear.hardpoints[Corner::OutboardTieRod]);
	originalNormal = VVASE::Math::GetPlaneNormal(referenceSuspension->rightRear.hardpoints[Corner::LowerBallJoint],
							referenceSuspension->rightRear.hardpoints[Corner::UpperBallJoint],
							referenceSuspension->rightRear.hardpoints[Corner::OutboardTieRod]);

	// We also need to account for toe and camber settings for the targetNormal - apply camber first
	// NOTE:  This corner is on the right side of the car - we flip the sign on the camber and toe angles
	GeometryMath::Rotate(targetNormal, -displaySuspension->rightRear.staticCamber, Eigen::Vector3d::UnitX());
	GeometryMath::Rotate(targetNormal, -displaySuspension->rightRear.staticToe, Eigen::Vector3d::UnitZ());

	// Now continue with the update for this corner
	rightRearLowerAArm->Update(displaySuspension->rightRear.hardpoints[Corner::LowerFrontTubMount],
		displaySuspension->rightRear.hardpoints[Corner::LowerBallJoint],
		displaySuspension->rightRear.hardpoints[Corner::LowerRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	rightRearUpperAArm->Update(displaySuspension->rightRear.hardpoints[Corner::UpperFrontTubMount],
		displaySuspension->rightRear.hardpoints[Corner::UpperBallJoint],
		displaySuspension->rightRear.hardpoints[Corner::UpperRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	rightRearPushrod->Update(displaySuspension->rightRear.hardpoints[Corner::InboardPushrod],
		displaySuspension->rightRear.hardpoints[Corner::OutboardPushrod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	rightRearTieRod->Update(displaySuspension->rightRear.hardpoints[Corner::InboardTieRod],
		displaySuspension->rightRear.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	rightRearTire->Update(displaySuspension->rightRear.hardpoints[Corner::ContactPatch],
		displaySuspension->rightRear.hardpoints[Corner::WheelCenter], originalNormal, targetNormal,
		displayTires->rightRear->width,
		appearanceOptions.GetSize(AppearanceOptions::SizeTireInsideDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionTire),
		appearanceOptions.GetColor(AppearanceOptions::ColorTire),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTire));
	rightRearDamper->Update(displaySuspension->rightRear.hardpoints[Corner::OutboardDamper],
		displaySuspension->rightRear.hardpoints[Corner::InboardDamper],
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperBody),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityDamper));
	rightRearSpring->Update(displaySuspension->rightRear.hardpoints[Corner::InboardSpring],
		displaySuspension->rightRear.hardpoints[Corner::OutboardSpring],
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringEndPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorSpring),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySpring));
	rightRearUpright->Update(displaySuspension->rightRear.hardpoints[Corner::LowerBallJoint],
		displaySuspension->rightRear.hardpoints[Corner::UpperBallJoint],
		displaySuspension->rightRear.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetColor(AppearanceOptions::ColorUpright),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityUpright));
	rightRearBellCrank->Update(displaySuspension->rightRear.hardpoints[Corner::OutboardDamper],
		displaySuspension->rightRear.hardpoints[Corner::InboardPushrod],
		VVASE::Math::NearestPointOnAxis(displaySuspension->rightRear.hardpoints[Corner::BellCrankPivot1],
		displaySuspension->rightRear.hardpoints[Corner::BellCrankPivot2] -
		displaySuspension->rightRear.hardpoints[Corner::BellCrankPivot1],
		displaySuspension->rightRear.hardpoints[Corner::InboardPushrod]),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	rightRearBarLink->Update(displaySuspension->rightRear.hardpoints[Corner::InboardBarLink],
		displaySuspension->rightRear.hardpoints[Corner::OutboardBarLink],
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarLinkDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		ShowBarLinks);
	rightRearHalfShaft->Update(displaySuspension->rightRear.hardpoints[Corner::InboardHalfShaft],
		displaySuspension->rightRear.hardpoints[Corner::OutboardHalfShaft],
		appearanceOptions.GetSize(AppearanceOptions::SizeHalfShaftDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHalfShaft),
		appearanceOptions.GetColor(AppearanceOptions::ColorHalfShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHalfShaft) && referenceCar.HasRearHalfShafts());

	// left rear corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (displaySuspension->leftRear.actuationType == Corner::ActuationOutboardRockerArm ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	targetNormal = VVASE::Math::GetPlaneNormal(displaySuspension->leftRear.hardpoints[Corner::LowerBallJoint],
							displaySuspension->leftRear.hardpoints[Corner::UpperBallJoint],
							displaySuspension->leftRear.hardpoints[Corner::OutboardTieRod]);
	originalNormal = VVASE::Math::GetPlaneNormal(referenceSuspension->leftRear.hardpoints[Corner::LowerBallJoint],
							referenceSuspension->leftRear.hardpoints[Corner::UpperBallJoint],
							referenceSuspension->leftRear.hardpoints[Corner::OutboardTieRod]);

	// We also need to account for toe and camber settings for the targetNormal - apply camber first
	GeometryMath::Rotate(targetNormal, displaySuspension->leftRear.staticCamber, Eigen::Vector3d::UnitX());
	GeometryMath::Rotate(targetNormal, displaySuspension->leftRear.staticToe, Eigen::Vector3d::UnitZ());

	// Now continue with the update for this corner
	leftRearLowerAArm->Update(displaySuspension->leftRear.hardpoints[Corner::LowerFrontTubMount],
		displaySuspension->leftRear.hardpoints[Corner::LowerBallJoint],
		displaySuspension->leftRear.hardpoints[Corner::LowerRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	leftRearUpperAArm->Update(displaySuspension->leftRear.hardpoints[Corner::UpperFrontTubMount],
		displaySuspension->leftRear.hardpoints[Corner::UpperBallJoint],
		displaySuspension->leftRear.hardpoints[Corner::UpperRearTubMount],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	leftRearPushrod->Update(displaySuspension->leftRear.hardpoints[Corner::InboardPushrod],
		displaySuspension->leftRear.hardpoints[Corner::OutboardPushrod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	leftRearTieRod->Update(displaySuspension->leftRear.hardpoints[Corner::InboardTieRod],
		displaySuspension->leftRear.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	leftRearTire->Update(displaySuspension->leftRear.hardpoints[Corner::ContactPatch],
		displaySuspension->leftRear.hardpoints[Corner::WheelCenter], originalNormal, targetNormal,
		displayTires->leftRear->width,
		appearanceOptions.GetSize(AppearanceOptions::SizeTireInsideDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionTire),
		appearanceOptions.GetColor(AppearanceOptions::ColorTire),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTire));
	leftRearDamper->Update(displaySuspension->leftRear.hardpoints[Corner::OutboardDamper],
		displaySuspension->leftRear.hardpoints[Corner::InboardDamper],
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperBody),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityDamper));
	leftRearSpring->Update(displaySuspension->leftRear.hardpoints[Corner::InboardSpring],
		displaySuspension->leftRear.hardpoints[Corner::OutboardSpring],
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringEndPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorSpring),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySpring));
	leftRearUpright->Update(displaySuspension->leftRear.hardpoints[Corner::LowerBallJoint],
		displaySuspension->leftRear.hardpoints[Corner::UpperBallJoint],
		displaySuspension->leftRear.hardpoints[Corner::OutboardTieRod],
		appearanceOptions.GetColor(AppearanceOptions::ColorUpright),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityUpright));
	leftRearBellCrank->Update(displaySuspension->leftRear.hardpoints[Corner::OutboardDamper],
		displaySuspension->leftRear.hardpoints[Corner::InboardPushrod],
		VVASE::Math::NearestPointOnAxis(displaySuspension->leftRear.hardpoints[Corner::BellCrankPivot1],
		displaySuspension->leftRear.hardpoints[Corner::BellCrankPivot2] -
		displaySuspension->leftRear.hardpoints[Corner::BellCrankPivot1],
		displaySuspension->leftRear.hardpoints[Corner::InboardPushrod]),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	leftRearBarLink->Update(displaySuspension->leftRear.hardpoints[Corner::InboardBarLink],
		displaySuspension->leftRear.hardpoints[Corner::OutboardBarLink],
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarLinkDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		ShowBarLinks);
	leftRearHalfShaft->Update(displaySuspension->leftRear.hardpoints[Corner::InboardHalfShaft],
		displaySuspension->leftRear.hardpoints[Corner::OutboardHalfShaft],
		appearanceOptions.GetSize(AppearanceOptions::SizeHalfShaftDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHalfShaft),
		appearanceOptions.GetColor(AppearanceOptions::ColorHalfShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHalfShaft) && referenceCar.HasRearHalfShafts());

	// Front end
	steeringRack->Update(displaySuspension->rightFront.hardpoints[Corner::InboardTieRod],
		displaySuspension->leftFront.hardpoints[Corner::InboardTieRod],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	frontSwayBar->Update(displaySuspension->rightFront.hardpoints[Corner::InboardBarLink],
		displaySuspension->leftFront.hardpoints[Corner::InboardBarLink],
		displaySuspension->rightFront.hardpoints[Corner::BarArmAtPivot],
		displaySuspension->leftFront.hardpoints[Corner::BarArmAtPivot],
		displaySuspension->hardpoints[Suspension::FrontBarMidPoint],
		displaySuspension->hardpoints[Suspension::FrontBarPivotAxis],
		displaySuspension->frontBarStyle,
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySwayBar));

	// Rear end
	rearSwayBar->Update(displaySuspension->rightRear.hardpoints[Corner::InboardBarLink],
		displaySuspension->leftRear.hardpoints[Corner::InboardBarLink],
		displaySuspension->rightRear.hardpoints[Corner::BarArmAtPivot],
		displaySuspension->leftRear.hardpoints[Corner::BarArmAtPivot],
		displaySuspension->hardpoints[Suspension::RearBarMidPoint],
		displaySuspension->hardpoints[Suspension::RearBarPivotAxis],
		displaySuspension->rearBarStyle,
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
			helperOrbPosition = displaySuspension->rightFront.hardpoints[helperOrbCornerPoint];
			helperOrbOppositePosition = displaySuspension->leftFront.hardpoints[helperOrbCornerPoint];
		}
		else if (helperOrbLocation == Corner::LocationLeftFront)
		{
			helperOrbPosition = displaySuspension->leftFront.hardpoints[helperOrbCornerPoint];
			helperOrbOppositePosition = displaySuspension->rightFront.hardpoints[helperOrbCornerPoint];
		}
		else if (helperOrbLocation == Corner::LocationRightRear)
		{
			helperOrbPosition = displaySuspension->rightRear.hardpoints[helperOrbCornerPoint];
			helperOrbOppositePosition = displaySuspension->leftRear.hardpoints[helperOrbCornerPoint];
		}
		else
		{
			helperOrbPosition = displaySuspension->leftRear.hardpoints[helperOrbCornerPoint];
			helperOrbOppositePosition = displaySuspension->rightRear.hardpoints[helperOrbCornerPoint];
		}
	}
	else if (helperOrbSuspensionPoint != Suspension::NumberOfHardpoints)
		helperOrbPosition = displaySuspension->hardpoints[helperOrbSuspensionPoint];

	helperOrb->Update(helperOrbPosition, appearanceOptions.GetSize(AppearanceOptions::SizeHelperOrbDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHelperOrb),
		appearanceOptions.GetColor(AppearanceOptions::ColorHelperOrb),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHelperOrb) && helperOrbIsActive);
	helperOrbOpposite->Update(helperOrbOppositePosition, appearanceOptions.GetSize(AppearanceOptions::SizeHelperOrbDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHelperOrb),
		appearanceOptions.GetColor(AppearanceOptions::ColorHelperOrb),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHelperOrb) && helperOrbIsActive
		&& displaySuspension->isSymmetric && helperOrbCornerPoint != Corner::NumberOfHardpoints);
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
	origin = std::make_unique<Origin>(*this);
	groundPlane = std::make_unique<Plane3D>(*this);

	// right front corner
	rightFrontLowerAArm = std::make_unique<AArm>(*this);
	rightFrontUpperAArm = std::make_unique<AArm>(*this);
	rightFrontPushrod = std::make_unique<Link>(*this);
	rightFrontTieRod = std::make_unique<Link>(*this);
	rightFrontTire = std::make_unique<Tire3D>(*this);
	rightFrontDamper = std::make_unique<Damper3D>(*this);
	rightFrontSpring = std::make_unique<Spring3D>(*this);
	rightFrontUpright = std::make_unique<Triangle3D>(*this);
	rightFrontBellCrank = std::make_unique<Triangle3D>(*this);
	rightFrontBarLink = std::make_unique<Link>(*this);
	rightFrontHalfShaft = std::make_unique<Link>(*this);

	// left front corner
	leftFrontLowerAArm = std::make_unique<AArm>(*this);
	leftFrontUpperAArm = std::make_unique<AArm>(*this);
	leftFrontPushrod = std::make_unique<Link>(*this);
	leftFrontTieRod = std::make_unique<Link>(*this);
	leftFrontTire = std::make_unique<Tire3D>(*this);
	leftFrontDamper = std::make_unique<Damper3D>(*this);
	leftFrontSpring = std::make_unique<Spring3D>(*this);
	leftFrontUpright = std::make_unique<Triangle3D>(*this);
	leftFrontBellCrank = std::make_unique<Triangle3D>(*this);
	leftFrontBarLink = std::make_unique<Link>(*this);
	leftFrontHalfShaft = std::make_unique<Link>(*this);

	// right rear corner
	rightRearLowerAArm = std::make_unique<AArm>(*this);
	rightRearUpperAArm = std::make_unique<AArm>(*this);
	rightRearPushrod = std::make_unique<Link>(*this);
	rightRearTieRod = std::make_unique<Link>(*this);
	rightRearTire = std::make_unique<Tire3D>(*this);
	rightRearDamper = std::make_unique<Damper3D>(*this);
	rightRearSpring = std::make_unique<Spring3D>(*this);
	rightRearUpright = std::make_unique<Triangle3D>(*this);
	rightRearBellCrank = std::make_unique<Triangle3D>(*this);
	rightRearBarLink = std::make_unique<Link>(*this);
	rightRearHalfShaft = std::make_unique<Link>(*this);

	// left rear corner
	leftRearLowerAArm = std::make_unique<AArm>(*this);
	leftRearUpperAArm = std::make_unique<AArm>(*this);
	leftRearPushrod = std::make_unique<Link>(*this);
	leftRearTieRod = std::make_unique<Link>(*this);
	leftRearTire = std::make_unique<Tire3D>(*this);
	leftRearDamper = std::make_unique<Damper3D>(*this);
	leftRearSpring = std::make_unique<Spring3D>(*this);
	leftRearUpright = std::make_unique<Triangle3D>(*this);
	leftRearBellCrank = std::make_unique<Triangle3D>(*this);
	leftRearBarLink = std::make_unique<Link>(*this);
	leftRearHalfShaft = std::make_unique<Link>(*this);

	// Front end
	steeringRack = std::make_unique<Link>(*this);
	frontSwayBar = std::make_unique<Swaybar3D>(*this);

	// Rear end
	rearSwayBar = std::make_unique<Swaybar3D>(*this);

	// Kinematic display objects
	frontRollCenter = std::make_unique<Point3D>(*this);
	rearRollCenter = std::make_unique<Point3D>(*this);
	rightPitchCenter = std::make_unique<Point3D>(*this);
	leftPitchCenter = std::make_unique<Point3D>(*this);
	rightFrontInstantCenter = std::make_unique<Point3D>(*this);
	leftFrontInstantCenter = std::make_unique<Point3D>(*this);
	rightRearInstantCenter = std::make_unique<Point3D>(*this);
	leftRearInstantCenter = std::make_unique<Point3D>(*this);

	frontRollAxis = std::make_unique<Vector3D>(*this);
	rearRollAxis = std::make_unique<Vector3D>(*this);
	rightPitchAxis = std::make_unique<Vector3D>(*this);
	leftPitchAxis = std::make_unique<Vector3D>(*this);
	rightFrontInstantAxis = std::make_unique<Vector3D>(*this);
	leftFrontInstantAxis = std::make_unique<Vector3D>(*this);
	rightRearInstantAxis = std::make_unique<Vector3D>(*this);
	leftRearInstantAxis = std::make_unique<Vector3D>(*this);

	// Helper orb
	helperOrb = std::make_unique<Point3D>(*this);
	helperOrbOpposite = std::make_unique<Point3D>(*this);
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

	std::vector<const LibPlot2D::Primitive*> intersected(IntersectWithPrimitive(point, direction));

	while (intersected.size() > 0)
	{
		const LibPlot2D::Primitive* selected(GetClosestPrimitive(intersected));

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
	if (mIsInteracting || mObservedLeftButtonDown)
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
	if (mIsInteracting)
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

	direction = (point2 - point).normalized();
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
//		std::vector<const LibPlot2D::Primitive*>
//
//==========================================================================
std::vector<const LibPlot2D::Primitive*> CarRenderer::IntersectWithPrimitive(
	const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	std::vector<const LibPlot2D::Primitive*> intersected;

	/*for (const auto& primitive : mPrimitiveList)
	{
		if (primitive->IsIntersectedBy(point, direction))
			intersected.push_back(primitive);
	}*/// TODO:  Re-implement

	return intersected;
}

//==========================================================================
// Class:			CarRenderer
// Function:		GetClosestPrimitive
//
// Description:		Selects the primitive closest to the eyepoint.
//
// Input Arguments:
//		intersected	= const std::vector<const LibPlot2D::Primitive*>&
//
// Output Arguments:
//		None
//
// Return Value:
//		const LibPlot2D::Primitive*
//
//==========================================================================
const LibPlot2D::Primitive* CarRenderer::GetClosestPrimitive(
	const std::vector<const LibPlot2D::Primitive*>& intersected) const
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
//		selected		= const LibPlot2D::Primitive&
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
	const LibPlot2D::Primitive* selected, Suspension::Hardpoints& suspensionPoint,
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

	const auto* displaySuspension(displayCar.GetSubsystem<Suspension>());

	// TODO:  Now what... need to find hardpoint from vector point
	unsigned int i;
	for (i = 0; i < Corner::NumberOfHardpoints; i++)
	{
		if (VVASE::Math::IsZero(displaySuspension->leftFront.hardpoints[i] - closestPoint))
		{
			leftFrontPoint = static_cast<Corner::Hardpoints>(i);
			return;
		}
		else if (VVASE::Math::IsZero(displaySuspension->rightFront.hardpoints[i] - closestPoint))
		{
			rightFrontPoint = static_cast<Corner::Hardpoints>(i);
			return;
		}
		else if (VVASE::Math::IsZero(displaySuspension->leftRear.hardpoints[i] - closestPoint))
		{
			leftRearPoint = static_cast<Corner::Hardpoints>(i);
			return;
		}
		else if (VVASE::Math::IsZero(displaySuspension->rightRear.hardpoints[i] - closestPoint))
		{
			rightRearPoint = static_cast<Corner::Hardpoints>(i);
			return;
		}
	}

	for (i = 0; i < Suspension::NumberOfHardpoints; i++)
	{
		if (VVASE::Math::IsZero(displaySuspension->hardpoints[i] - closestPoint))
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

	auto* originalSuspension(car.GetOriginalCar().GetSubsystem<Suspension>());

	if (suspensionPoint != Suspension::NumberOfHardpoints)
	{
		pointToEdit = &originalSuspension->hardpoints[suspensionPoint];
		pointName = Suspension::GetHardpointName(suspensionPoint);
	}
	else if (leftFrontPoint != Corner::NumberOfHardpoints)
	{
		if (originalSuspension->isSymmetric)
			pointToEdit = &originalSuspension->rightFront.hardpoints[leftFrontPoint];
		else
			pointToEdit = &originalSuspension->leftFront.hardpoints[leftFrontPoint];

		pointName = Corner::GetLocationName(Corner::LocationLeftFront)
			+ _T(" ") + Corner::GetHardpointName(leftFrontPoint);
	}
	else if (rightFrontPoint != Corner::NumberOfHardpoints)
	{
		pointToEdit = &originalSuspension->rightFront.hardpoints[rightFrontPoint];
		pointName = Corner::GetLocationName(Corner::LocationRightFront)
			+ _T(" ") + Corner::GetHardpointName(rightFrontPoint);
	}
	else if (leftRearPoint != Corner::NumberOfHardpoints)
	{
		if (originalSuspension->isSymmetric)
			pointToEdit = &originalSuspension->rightRear.hardpoints[leftRearPoint];
		else
			pointToEdit = &originalSuspension->leftRear.hardpoints[leftRearPoint];

		pointName = Corner::GetLocationName(Corner::LocationLeftRear)
			+ _T(" ") + Corner::GetHardpointName(leftRearPoint);
	}
	else if (rightRearPoint != Corner::NumberOfHardpoints)
	{
		pointToEdit = &originalSuspension->rightRear.hardpoints[rightRearPoint];
		pointName = Corner::GetLocationName(Corner::LocationRightRear)
			+ _T(" ") + Corner::GetHardpointName(rightRearPoint);
	}
	else
		return;

	Eigen::Vector3d tempPoint(*pointToEdit);
	VectorEditDialog dialog(mainFrame, tempPoint, pointName);

	if (dialog.ShowModal() != wxOK)
		return;

	if (VVASE::Math::IsZero(*pointToEdit - tempPoint))
		return;

	{
		MutexLocker lock(car.GetOriginalCar().GetMutex());
		mainFrame.GetUndoRedoStack().AddOperation(mainFrame.GetActiveIndex(),
			UndoRedoStack::Operation::DataTypeVector, pointToEdit);
		*pointToEdit = tempPoint;
		originalSuspension->UpdateSymmetry();
	}

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

//==========================================================================
// Class:			RenderWindow
// Function:		Unproject
//
// Description:		Returns the 3D location of the specified screen coordinate.
//
// Input Arguments:
//		x	= const double&
//		y	= const double&
//		z	= const double&
//
// Output Arguments:
//		point	= Eigen::Vector3d&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool CarRenderer::Unproject(const double& x, const double& y, const double& z,
	Eigen::Vector3d& point) const
{
	/*assert(viewToModel);

	Matrix modelViewMatrix(4, 4);
	ConvertGLToMatrix(modelViewMatrix, glModelviewMatrix);

	Matrix projectionMatrix;
	if (view3D)
		projectionMatrix = Generate3DProjectionMatrix();
	else
		projectionMatrix = Generate2DProjectionMatrix();

	Matrix a(projectionMatrix * modelViewMatrix);

	int w, h;
	GetSize(&w, &h);
	Matrix v(4, 1, x / w * 2.0 - 1.0, (h - y) / h * 2.0 - 1.0, z, 1.0);

	Matrix out(a.GetInverse() * v);
	if (VVASEMath::IsZero(out(3,0)))
		return false;

	out(3,0) = 1.0 / out(3,0);
	point.x() = out(0,0) * out(3,0);
	point.y() = out(1,0) * out(3,0);
	point.z() = out(2,0) * out(3,0);*/

	return true;
}

}// namespace VVASE
