/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  carRenderer.cpp
// Created:  4/5/2008
// Author:  K. Loux
// Description:  Contains class declaration for the CarRenderer class.  Derived from
//				 RenderWindow, this class is associated with a loaded car file
//				 and contains the information and methods required to render a car in 3D.
// History:
//	3/17/2009	- Changed SetHelperOrbPosition() to take a series of enums rather than a Vector to
//				  allow the position to be taken from the DisplayCar rather than the coordinates
//				  of the OriginalCar sent from the editing panels, K. Loux.
//	4/11/2009	- Added call to Renderer->ResetCamera() to constructor to avoid opening a car
//				  that appears very small in the render window so that the instant centers
//				  can be seen, K. Loux.
//	5/17/2009	- Was derived from wxVTKRenderWindowInteractor, switched to custom
//				  RenderWindow base class (eliminated dependance on VTK).
//	11/22/2009	- Moved to vRenderer.lib, K. Loux.

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "gui/renderer/carRenderer.h"
#include "vRenderer/3dcar/aarm.h"
#include "vRenderer/3dcar/link.h"
#include "vRenderer/3dcar/triangle3D.h"
#include "vRenderer/3dcar/damper3D.h"
#include "vRenderer/3dcar/spring3D.h"
#include "vRenderer/3dcar/plane3D.h"
#include "vRenderer/3dcar/point3D.h"
#include "vRenderer/3dcar/origin.h"
#include "vRenderer/3dcar/tire3D.h"
#include "vRenderer/3dcar/swaybar3D.h"
#include "vRenderer/3dcar/vector3D.h"
#include "vCar/car.h"
#include "vCar/brakes.h"
#include "vCar/drivetrain.h"
#include "vCar/suspension.h"
#include "vCar/tire.h"
#include "vCar/tireSet.h"
#include "vUtilities/debugger.h"
#include "vMath/carMath.h"
#include "vMath/vector.h"
#include "gui/guiCar.h"
#include "gui/appearanceOptions.h"
#include "gui/components/mainFrame.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vMath/matrix.h"
#include "vUtilities/debugLog.h"
#include "vRenderer/3dcar/debugShape.h"

//==========================================================================
// Class:			CarRenderer
// Function:		CarRenderer
//
// Description:		Constructor for CarRenderer class.  Initializes the
//					renderer and sets up the canvas.
//
// Input Arguments:
//		_mainFrame	= MainFrame& reference to the owner of this object
//		_car		= GuiCar& reference to the car that we are to render
//		args		= int[] NOTE: Under GTK, must contain WX_GL_DOUBLEBUFFER at minimum
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
CarRenderer::CarRenderer(MainFrame &_mainFrame, GuiCar &_car, int args[])
						   : RenderWindow(_mainFrame, wxID_ANY, args, wxDefaultPosition,
						   wxDefaultSize, wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE),
						   mainFrame(_mainFrame),
						   appearanceOptions(_car.GetAppearanceOptions()),
						   displayCar(_car.GetWorkingCar()), referenceCar(_car.GetOriginalCar())
{
	// Create the objects necessary to render the scene
	CreateActors();

	// Initialize the helper orb to NOT active
	helperOrbCornerPoint = Corner::NumberOfHardpoints;
	helperOrbLocation = Corner::LocationRightFront;
	helperOrbSuspensionPoint = Suspension::NumberOfHardpoints;
	helperOrbIsActive = false;

	// Set the camera view so that the car is visible
	Vector position(-100.0, -100.0, 60.0), up(0.0, 0.0, 1.0);
	Vector lookAt(referenceCar.suspension->leftRear.hardpoints[Corner::ContactPatch].x / 2.0, 0.0, 0.0);// FIXME:  This could be better
	SetCameraView(position, lookAt, up);

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
}

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
	// Set the background color
	SetBackgroundColor(appearanceOptions.GetColor(AppearanceOptions::ColorBackground));

	// Update the car display
	UpdateCarDisplay();

	// Update the kinematic display
	UpdateKinematicsDisplay(outputs);

	// Render the image.  We need to reset the frustum every time we do this just in case
	// the user zooms out very far, etc.
	AutoSetFrustum();
	
	// Force a re-draw
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
void CarRenderer::UpdateCarDisplay(void)
{
	// For drawing the tire
	Vector targetNormal;
	Vector originalNormal;

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
	if (displayCar.suspension->rightFront.actuationType == Corner::ActuationOutboard ||
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
	targetNormal.Rotate(-displayCar.suspension->rightFront.staticCamber, Vector::AxisX);
	targetNormal.Rotate(-displayCar.suspension->rightFront.staticToe, Vector::AxisZ);

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
	rightFrontDamper->Update(displayCar.suspension->rightFront.hardpoints[Corner::OutboardShock],
		displayCar.suspension->rightFront.hardpoints[Corner::InboardShock],
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
	rightFrontBellCrank->Update(displayCar.suspension->rightFront.hardpoints[Corner::OutboardShock],
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
	if (displayCar.suspension->leftFront.actuationType == Corner::ActuationOutboard ||
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
	targetNormal.Rotate(displayCar.suspension->leftFront.staticCamber, Vector::AxisX);
	targetNormal.Rotate(displayCar.suspension->leftFront.staticToe, Vector::AxisZ);

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
	leftFrontDamper->Update(displayCar.suspension->leftFront.hardpoints[Corner::OutboardShock],
		displayCar.suspension->leftFront.hardpoints[Corner::InboardShock],
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
	leftFrontBellCrank->Update(displayCar.suspension->leftFront.hardpoints[Corner::OutboardShock],
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
	if (displayCar.suspension->rightRear.actuationType == Corner::ActuationOutboard ||
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
	targetNormal.Rotate(-displayCar.suspension->rightRear.staticCamber, Vector::AxisX);
	targetNormal.Rotate(-displayCar.suspension->rightRear.staticToe, Vector::AxisZ);

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
	rightRearDamper->Update(displayCar.suspension->rightRear.hardpoints[Corner::OutboardShock],
		displayCar.suspension->rightRear.hardpoints[Corner::InboardShock],
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
	rightRearBellCrank->Update(displayCar.suspension->rightRear.hardpoints[Corner::OutboardShock],
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
	if (displayCar.suspension->leftRear.actuationType == Corner::ActuationOutboard ||
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
	targetNormal.Rotate(displayCar.suspension->leftRear.staticCamber, Vector::AxisX);
	targetNormal.Rotate(displayCar.suspension->leftRear.staticToe, Vector::AxisZ);

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
	leftRearDamper->Update(displayCar.suspension->leftRear.hardpoints[Corner::OutboardShock],
		displayCar.suspension->leftRear.hardpoints[Corner::InboardShock],
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
	leftRearBellCrank->Update(displayCar.suspension->leftRear.hardpoints[Corner::OutboardShock],
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
		displayCar.suspension->rearBarStyle,
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySwayBar));

	// Update the helper orb
	// Determine which of the location variables is valid
	Vector helperOrbPosition(0.0, 0.0, 0.0);
	if (helperOrbCornerPoint != Corner::NumberOfHardpoints)
	{
		if (helperOrbLocation == Corner::LocationRightFront)
			helperOrbPosition = displayCar.suspension->rightFront.hardpoints[helperOrbCornerPoint];
		else if (helperOrbLocation == Corner::LocationLeftFront)
			helperOrbPosition = displayCar.suspension->leftFront.hardpoints[helperOrbCornerPoint];
		else if (helperOrbLocation == Corner::LocationRightRear)
			helperOrbPosition = displayCar.suspension->rightRear.hardpoints[helperOrbCornerPoint];
		else
			helperOrbPosition = displayCar.suspension->leftRear.hardpoints[helperOrbCornerPoint];
	}
	else if (helperOrbSuspensionPoint != Suspension::NumberOfHardpoints)
		helperOrbPosition = displayCar.suspension->hardpoints[helperOrbSuspensionPoint];

	helperOrb->Update(helperOrbPosition, appearanceOptions.GetSize(AppearanceOptions::SizeHelperOrbDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHelperOrb),
		appearanceOptions.GetColor(AppearanceOptions::ColorHelperOrb),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHelperOrb) && helperOrbIsActive);
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
void CarRenderer::CreateActors(void)
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