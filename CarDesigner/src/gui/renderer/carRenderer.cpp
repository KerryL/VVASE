/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  carRenderer.cpp
// Created:  4/5/2008
// Author:  K. Loux
// Description:  Contains class declaration for the CAR_RENDERER class.  Derived from
//				 RENDER_WINDOW, this class is associated with a loaded car file
//				 and contains the information and methods required to render a car in 3D.
// History:
//	3/17/2009	- Changed SetHelperOrbPosition() to take a series of enums rather than a Vector to
//				  allow the position to be taken from the DisplayCar rather than the coordinates
//				  of the OriginalCar sent from the editing panels, K. Loux.
//	4/11/2009	- Added call to Renderer->ResetCamera() to constructor to avoid opening a car
//				  that appears very small in the render window so that the instant centers
//				  can be seen, K. Loux.
//	5/17/2009	- Was derived from wxVTKRenderWindowInteractor, switched to custom
//				  RENDER_WINDOW base class (eliminated dependance on VTK).
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

//==========================================================================
// Class:			CAR_RENDERER
// Function:		CAR_RENDERER
//
// Description:		Constructor for CAR_RENDERER class.  Initializes the
//					renderer and sets up the canvas.
//
// Input Arguments:
//		_MainFrame	= MAIN_FRAME& reference to the owner of this object
//		_Car		= GUI_CAR& reference to the car that we are to render
//		args		= int[] NOTE: Under GTK, must contain WX_GL_DOUBLEBUFFER at minimum
//		_debugger	= const Debugger&, reference to debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
CAR_RENDERER::CAR_RENDERER(MAIN_FRAME &_MainFrame, GUI_CAR &_Car, int args[],
						   const Debugger &_debugger)
						   : RenderWindow(_MainFrame, wxID_ANY, args, wxDefaultPosition,
						   wxDefaultSize, wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE),
						   debugger(_debugger), MainFrame(_MainFrame),
						   AppearanceOptions(_Car.GetAppearanceOptions()),
						   DisplayCar(_Car.GetWorkingCar()), ReferenceCar(_Car.GetOriginalCar())
{
	// Create the objects necessary to render the scene
	CreateActors();

	// Initialize the helper orb to NOT active
	HelperOrbCornerPoint = Corner::NumberOfHardpoints;
	HelperOrbLocation = Corner::LocationRightFront;
	HelperOrbSuspensionPoint = Suspension::NumberOfHardpoints;
	HelperOrbIsActive = false;

	// Set the camera view so that the car is visible
	Vector Position(-100.0, -100.0, 60.0), Up(0.0, 0.0, 1.0);
	Vector LookAt(ReferenceCar.suspension->leftRear.hardpoints[Corner::ContactPatch].x / 2.0, 0.0, 0.0);// FIXME:  This could be better
	SetCameraView(Position, LookAt, Up);
}

//==========================================================================
// Class:			CAR_RENDERER
// Function:		~CAR_RENDERER
//
// Description:		Destructor for CAR_RENDERER class.
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
CAR_RENDERER::~CAR_RENDERER()
{
	// Ideally, this stuff would be taken care of by adding these to some
	// ManagedList, but for now we explicitly delete them one by one...
	delete origin;
	delete GroundPlane;

	delete RightFrontLowerAArm;
	delete RightFrontUpperAArm;
	delete RightFrontPushrod;
	delete RightFrontTieRod;
	delete RightFrontTire;
	delete RightFrontDamper;
	delete RightFrontSpring;
	delete RightFrontUpright;
	delete RightFrontBellCrank;
	delete RightFrontBarLink;
	delete RightFrontHalfShaft;

	delete LeftFrontLowerAArm;
	delete LeftFrontUpperAArm;
	delete LeftFrontPushrod;
	delete LeftFrontTieRod;
	delete LeftFrontTire;
	delete LeftFrontDamper;
	delete LeftFrontSpring;
	delete LeftFrontUpright;
	delete LeftFrontBellCrank;
	delete LeftFrontBarLink;
	delete LeftFrontHalfShaft;

	delete RightRearLowerAArm;
	delete RightRearUpperAArm;
	delete RightRearPushrod;
	delete RightRearTieRod;
	delete RightRearTire;
	delete RightRearDamper;
	delete RightRearSpring;
	delete RightRearUpright;
	delete RightRearBellCrank;
	delete RightRearBarLink;
	delete RightRearHalfShaft;

	delete LeftRearLowerAArm;
	delete LeftRearUpperAArm;
	delete LeftRearPushrod;
	delete LeftRearTieRod;
	delete LeftRearTire;
	delete LeftRearDamper;
	delete LeftRearSpring;
	delete LeftRearUpright;
	delete LeftRearBellCrank;
	delete LeftRearBarLink;
	delete LeftRearHalfShaft;

	delete SteeringRack;
	delete FrontSwayBar;

	delete RearSwayBar;

	delete FrontRollCenter;
	delete RearRollCenter;
	delete RightPitchCenter;
	delete LeftPitchCenter;
	delete RightFrontInstantCenter;
	delete LeftFrontInstantCenter;
	delete RightRearInstantCenter;
	delete LeftRearInstantCenter;

	delete FrontRollAxis;
	delete RearRollAxis;
	delete RightPitchAxis;
	delete LeftPitchAxis;
	delete RightFrontInstantAxis;
	delete LeftFrontInstantAxis;
	delete RightRearInstantAxis;
	delete LeftRearInstantAxis;

	delete HelperOrb;
}

//==========================================================================
// Class:			CAR_RENDERER
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
void CAR_RENDERER::UpdateDisplay(const KinematicOutputs &outputs)
{
	// Set the background color
	SetBackgroundColor(AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorBackground));

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
// Class:			CAR_RENDERER
// Function:		UpdateCarDisplay
//
// Description:		Updates the displayed 3D car to match the new position
//					of the class member DisplayCar.
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
void CAR_RENDERER::UpdateCarDisplay(void)
{
	// For drawing the tire
	Vector TargetNormal;
	Vector OriginalNormal;

	// Get locks on the car's mutexes
	wxMutexLocker displayLock(DisplayCar.GetMutex());
	wxMutexLocker referenceLock(ReferenceCar.GetMutex());

	// Update the origin
	origin->Update(AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeOriginShaftLength),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeOriginShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeOriginTipLength),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeOriginTipDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionOrigin),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityOrigin));

	// Figure out how big the ground plane should be
	double GroundPlaneScaleUp = 1.1;
	double RightMostPoint = max(ReferenceCar.suspension->rightFront.hardpoints[Corner::ContactPatch].y +
		ReferenceCar.tires->rightFront->width / 2.0,
		ReferenceCar.suspension->rightRear.hardpoints[Corner::ContactPatch].y +
		ReferenceCar.tires->rightRear->width / 2.0);
	double LeftMostPoint = min(ReferenceCar.suspension->leftFront.hardpoints[Corner::ContactPatch].y -
		ReferenceCar.tires->leftFront->width / 2.0,
		ReferenceCar.suspension->leftRear.hardpoints[Corner::ContactPatch].y -
		ReferenceCar.tires->leftRear->width / 2.0);
	double FrontMostPoint = min(ReferenceCar.suspension->rightFront.hardpoints[Corner::ContactPatch].x -
		ReferenceCar.tires->rightFront->diameter / 2.0,
		ReferenceCar.suspension->leftFront.hardpoints[Corner::ContactPatch].x -
		ReferenceCar.tires->leftFront->diameter / 2.0);
	double RearMostPoint = max(ReferenceCar.suspension->rightRear.hardpoints[Corner::ContactPatch].x +
		ReferenceCar.tires->rightRear->diameter / 2.0,
		ReferenceCar.suspension->leftRear.hardpoints[Corner::ContactPatch].x +
		ReferenceCar.tires->leftRear->diameter / 2.0);

	double XLength = fabs(FrontMostPoint - RearMostPoint) * GroundPlaneScaleUp;
	double YLength = fabs(RightMostPoint - LeftMostPoint) * GroundPlaneScaleUp;
	double XOffset = (RearMostPoint - FrontMostPoint) / 2.0 + FrontMostPoint;
	GroundPlane->Update(XLength, YLength, XOffset,
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorGroundPlane),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityGroundPlane));

	// Copy the tire parameters from the reference car
	DisplayCar.tires->rightFront->width = ReferenceCar.tires->rightFront->width;
	DisplayCar.tires->leftFront->width = ReferenceCar.tires->leftFront->width;
	DisplayCar.tires->rightRear->width = ReferenceCar.tires->rightRear->width;
	DisplayCar.tires->leftRear->width = ReferenceCar.tires->leftRear->width;

	// Determine whether or not the front sway bar links should be visible
	bool ShowBarLinks = true;
	if (DisplayCar.suspension->frontBarStyle == Suspension::SwayBarNone ||
		!AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySwayBar))
		ShowBarLinks = false;

	// Right front corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	bool ShowBellCranksPushrods = true;
	if (DisplayCar.suspension->rightFront.actuationType == Corner::ActuationOutboard ||
		!AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	TargetNormal = VVASEMath::GetPlaneNormal(DisplayCar.suspension->rightFront.hardpoints[Corner::LowerBallJoint],
							DisplayCar.suspension->rightFront.hardpoints[Corner::UpperBallJoint],
							DisplayCar.suspension->rightFront.hardpoints[Corner::OutboardTieRod]);
	OriginalNormal = VVASEMath::GetPlaneNormal(ReferenceCar.suspension->rightFront.hardpoints[Corner::LowerBallJoint],
							ReferenceCar.suspension->rightFront.hardpoints[Corner::UpperBallJoint],
							ReferenceCar.suspension->rightFront.hardpoints[Corner::OutboardTieRod]);

	// We also need to account for toe and camber settings for the TargetNormal - apply camber first
	// NOTE:  This corner is on the right side of the car - we flip the sign on the camber and toe angles
	TargetNormal.Rotate(-DisplayCar.suspension->rightFront.staticCamber, Vector::AxisX);
	TargetNormal.Rotate(-DisplayCar.suspension->rightFront.staticToe, Vector::AxisZ);

	// Now continue with the update for this corner
	RightFrontLowerAArm->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::LowerFrontTubMount],
		DisplayCar.suspension->rightFront.hardpoints[Corner::LowerBallJoint],
		DisplayCar.suspension->rightFront.hardpoints[Corner::LowerRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	RightFrontUpperAArm->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::UpperFrontTubMount],
		DisplayCar.suspension->rightFront.hardpoints[Corner::UpperBallJoint],
		DisplayCar.suspension->rightFront.hardpoints[Corner::UpperRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	RightFrontPushrod->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::InboardPushrod],
		DisplayCar.suspension->rightFront.hardpoints[Corner::OutboardPushrod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	RightFrontTieRod->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::InboardTieRod],
		DisplayCar.suspension->rightFront.hardpoints[Corner::OutboardTieRod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTieRod),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTieRod));
	RightFrontTire->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::ContactPatch],
		DisplayCar.suspension->rightFront.hardpoints[Corner::WheelCenter], OriginalNormal, TargetNormal,
		DisplayCar.tires->rightFront->width,
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeTireInsideDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionTire),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTire),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTire));
	RightFrontDamper->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::OutboardShock],
		DisplayCar.suspension->rightFront.hardpoints[Corner::InboardShock],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperBody),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityDamper));
	RightFrontSpring->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::InboardSpring],
		DisplayCar.suspension->rightFront.hardpoints[Corner::OutboardSpring],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringEndPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSpring),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySpring));
	RightFrontUpright->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::LowerBallJoint],
		DisplayCar.suspension->rightFront.hardpoints[Corner::UpperBallJoint],
		DisplayCar.suspension->rightFront.hardpoints[Corner::OutboardTieRod],
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorUpright),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityUpright));
	RightFrontBellCrank->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::OutboardShock],
		DisplayCar.suspension->rightFront.hardpoints[Corner::InboardPushrod],
		VVASEMath::NearestPointOnAxis(DisplayCar.suspension->rightFront.hardpoints[Corner::BellCrankPivot1],
		DisplayCar.suspension->rightFront.hardpoints[Corner::BellCrankPivot2] -
		DisplayCar.suspension->rightFront.hardpoints[Corner::BellCrankPivot1],
		DisplayCar.suspension->rightFront.hardpoints[Corner::InboardPushrod]),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	RightFrontBarLink->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::InboardBarLink],
		DisplayCar.suspension->rightFront.hardpoints[Corner::OutboardBarLink],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSwayBarLinkDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSwayBar),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSwayBar),
		ShowBarLinks);
	RightFrontHalfShaft->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::InboardHalfShaft],
		DisplayCar.suspension->rightFront.hardpoints[Corner::OutboardHalfShaft],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeHalfShaftDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionHalfShaft),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorHalfShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityHalfShaft) && ReferenceCar.HasFrontHalfShafts());

	// Left front corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (DisplayCar.suspension->leftFront.actuationType == Corner::ActuationOutboard ||
		!AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	TargetNormal = VVASEMath::GetPlaneNormal(DisplayCar.suspension->leftFront.hardpoints[Corner::LowerBallJoint],
							DisplayCar.suspension->leftFront.hardpoints[Corner::UpperBallJoint],
							DisplayCar.suspension->leftFront.hardpoints[Corner::OutboardTieRod]);
	OriginalNormal = VVASEMath::GetPlaneNormal(ReferenceCar.suspension->leftFront.hardpoints[Corner::LowerBallJoint],
							ReferenceCar.suspension->leftFront.hardpoints[Corner::UpperBallJoint],
							ReferenceCar.suspension->leftFront.hardpoints[Corner::OutboardTieRod]);

	// We also need to account for toe and camber settings for the TargetNormal - apply camber first
	TargetNormal.Rotate(DisplayCar.suspension->leftFront.staticCamber, Vector::AxisX);
	TargetNormal.Rotate(DisplayCar.suspension->leftFront.staticToe, Vector::AxisZ);

	// Now continue with the update for this corner
	LeftFrontLowerAArm->Update(DisplayCar.suspension->leftFront.hardpoints[Corner::LowerFrontTubMount],
		DisplayCar.suspension->leftFront.hardpoints[Corner::LowerBallJoint],
		DisplayCar.suspension->leftFront.hardpoints[Corner::LowerRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	LeftFrontUpperAArm->Update(DisplayCar.suspension->leftFront.hardpoints[Corner::UpperFrontTubMount],
		DisplayCar.suspension->leftFront.hardpoints[Corner::UpperBallJoint],
		DisplayCar.suspension->leftFront.hardpoints[Corner::UpperRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	LeftFrontPushrod->Update(DisplayCar.suspension->leftFront.hardpoints[Corner::InboardPushrod],
		DisplayCar.suspension->leftFront.hardpoints[Corner::OutboardPushrod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	LeftFrontTieRod->Update(DisplayCar.suspension->leftFront.hardpoints[Corner::InboardTieRod],
		DisplayCar.suspension->leftFront.hardpoints[Corner::OutboardTieRod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTieRod),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTieRod));
	LeftFrontTire->Update(DisplayCar.suspension->leftFront.hardpoints[Corner::ContactPatch],
		DisplayCar.suspension->leftFront.hardpoints[Corner::WheelCenter], OriginalNormal, TargetNormal,
		DisplayCar.tires->leftFront->width,
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeTireInsideDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionTire),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTire),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTire));
	LeftFrontDamper->Update(DisplayCar.suspension->leftFront.hardpoints[Corner::OutboardShock],
		DisplayCar.suspension->leftFront.hardpoints[Corner::InboardShock],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperBody),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityDamper));
	LeftFrontSpring->Update(DisplayCar.suspension->leftFront.hardpoints[Corner::InboardSpring],
		DisplayCar.suspension->leftFront.hardpoints[Corner::OutboardSpring],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringEndPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSpring),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySpring));
	LeftFrontUpright->Update(DisplayCar.suspension->leftFront.hardpoints[Corner::LowerBallJoint],
		DisplayCar.suspension->leftFront.hardpoints[Corner::UpperBallJoint],
		DisplayCar.suspension->leftFront.hardpoints[Corner::OutboardTieRod],
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorUpright),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityUpright));
	LeftFrontBellCrank->Update(DisplayCar.suspension->leftFront.hardpoints[Corner::OutboardShock],
		DisplayCar.suspension->leftFront.hardpoints[Corner::InboardPushrod],
		VVASEMath::NearestPointOnAxis(DisplayCar.suspension->leftFront.hardpoints[Corner::BellCrankPivot1],
		DisplayCar.suspension->leftFront.hardpoints[Corner::BellCrankPivot2] -
		DisplayCar.suspension->leftFront.hardpoints[Corner::BellCrankPivot1],
		DisplayCar.suspension->leftFront.hardpoints[Corner::InboardPushrod]),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	LeftFrontBarLink->Update(DisplayCar.suspension->leftFront.hardpoints[Corner::InboardBarLink],
		DisplayCar.suspension->leftFront.hardpoints[Corner::OutboardBarLink],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSwayBarLinkDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSwayBar),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSwayBar),
		ShowBarLinks);
	LeftFrontHalfShaft->Update(DisplayCar.suspension->leftFront.hardpoints[Corner::InboardHalfShaft],
		DisplayCar.suspension->leftFront.hardpoints[Corner::OutboardHalfShaft],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeHalfShaftDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionHalfShaft),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorHalfShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityHalfShaft) && ReferenceCar.HasFrontHalfShafts());

	// Determine whether or not the rear sway bar links should be visible
	if (DisplayCar.suspension->rearBarStyle == Suspension::SwayBarNone ||
		!AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySwayBar))
		ShowBarLinks = false;
	else
		ShowBarLinks = true;

	// Right rear corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (DisplayCar.suspension->rightRear.actuationType == Corner::ActuationOutboard ||
		!AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	TargetNormal = VVASEMath::GetPlaneNormal(DisplayCar.suspension->rightRear.hardpoints[Corner::LowerBallJoint],
							DisplayCar.suspension->rightRear.hardpoints[Corner::UpperBallJoint],
							DisplayCar.suspension->rightRear.hardpoints[Corner::OutboardTieRod]);
	OriginalNormal = VVASEMath::GetPlaneNormal(ReferenceCar.suspension->rightRear.hardpoints[Corner::LowerBallJoint],
							ReferenceCar.suspension->rightRear.hardpoints[Corner::UpperBallJoint],
							ReferenceCar.suspension->rightRear.hardpoints[Corner::OutboardTieRod]);

	// We also need to account for toe and camber settings for the TargetNormal - apply camber first
	// NOTE:  This corner is on the right side of the car - we flip the sign on the camber and toe angles
	TargetNormal.Rotate(-DisplayCar.suspension->rightRear.staticCamber, Vector::AxisX);
	TargetNormal.Rotate(-DisplayCar.suspension->rightRear.staticToe, Vector::AxisZ);

	// Now continue with the update for this corner
	RightRearLowerAArm->Update(DisplayCar.suspension->rightRear.hardpoints[Corner::LowerFrontTubMount],
		DisplayCar.suspension->rightRear.hardpoints[Corner::LowerBallJoint],
		DisplayCar.suspension->rightRear.hardpoints[Corner::LowerRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	RightRearUpperAArm->Update(DisplayCar.suspension->rightRear.hardpoints[Corner::UpperFrontTubMount],
		DisplayCar.suspension->rightRear.hardpoints[Corner::UpperBallJoint],
		DisplayCar.suspension->rightRear.hardpoints[Corner::UpperRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	RightRearPushrod->Update(DisplayCar.suspension->rightRear.hardpoints[Corner::InboardPushrod],
		DisplayCar.suspension->rightRear.hardpoints[Corner::OutboardPushrod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	RightRearTieRod->Update(DisplayCar.suspension->rightRear.hardpoints[Corner::InboardTieRod],
		DisplayCar.suspension->rightRear.hardpoints[Corner::OutboardTieRod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTieRod),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTieRod));
	RightRearTire->Update(DisplayCar.suspension->rightRear.hardpoints[Corner::ContactPatch],
		DisplayCar.suspension->rightRear.hardpoints[Corner::WheelCenter], OriginalNormal, TargetNormal,
		DisplayCar.tires->rightRear->width,
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeTireInsideDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionTire),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTire),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTire));
	RightRearDamper->Update(DisplayCar.suspension->rightRear.hardpoints[Corner::OutboardShock],
		DisplayCar.suspension->rightRear.hardpoints[Corner::InboardShock],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperBody),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityDamper));
	RightRearSpring->Update(DisplayCar.suspension->rightRear.hardpoints[Corner::InboardSpring],
		DisplayCar.suspension->rightRear.hardpoints[Corner::OutboardSpring],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringEndPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSpring),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySpring));
	RightRearUpright->Update(DisplayCar.suspension->rightRear.hardpoints[Corner::LowerBallJoint],
		DisplayCar.suspension->rightRear.hardpoints[Corner::UpperBallJoint],
		DisplayCar.suspension->rightRear.hardpoints[Corner::OutboardTieRod],
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorUpright),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityUpright));
	RightRearBellCrank->Update(DisplayCar.suspension->rightRear.hardpoints[Corner::OutboardShock],
		DisplayCar.suspension->rightRear.hardpoints[Corner::InboardPushrod],
		VVASEMath::NearestPointOnAxis(DisplayCar.suspension->rightRear.hardpoints[Corner::BellCrankPivot1],
		DisplayCar.suspension->rightRear.hardpoints[Corner::BellCrankPivot2] -
		DisplayCar.suspension->rightRear.hardpoints[Corner::BellCrankPivot1],
		DisplayCar.suspension->rightRear.hardpoints[Corner::InboardPushrod]),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	RightRearBarLink->Update(DisplayCar.suspension->rightRear.hardpoints[Corner::InboardBarLink],
		DisplayCar.suspension->rightRear.hardpoints[Corner::OutboardBarLink],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSwayBarLinkDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSwayBar),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSwayBar),
		ShowBarLinks);
	RightRearHalfShaft->Update(DisplayCar.suspension->rightRear.hardpoints[Corner::InboardHalfShaft],
		DisplayCar.suspension->rightRear.hardpoints[Corner::OutboardHalfShaft],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeHalfShaftDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionHalfShaft),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorHalfShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityHalfShaft) && ReferenceCar.HasRearHalfShafts());

	// Left rear corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (DisplayCar.suspension->leftRear.actuationType == Corner::ActuationOutboard ||
		!AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	TargetNormal = VVASEMath::GetPlaneNormal(DisplayCar.suspension->leftRear.hardpoints[Corner::LowerBallJoint],
							DisplayCar.suspension->leftRear.hardpoints[Corner::UpperBallJoint],
							DisplayCar.suspension->leftRear.hardpoints[Corner::OutboardTieRod]);
	OriginalNormal = VVASEMath::GetPlaneNormal(ReferenceCar.suspension->leftRear.hardpoints[Corner::LowerBallJoint],
							ReferenceCar.suspension->leftRear.hardpoints[Corner::UpperBallJoint],
							ReferenceCar.suspension->leftRear.hardpoints[Corner::OutboardTieRod]);

	// We also need to account for toe and camber settings for the TargetNormal - apply camber first
	TargetNormal.Rotate(DisplayCar.suspension->leftRear.staticCamber, Vector::AxisX);
	TargetNormal.Rotate(DisplayCar.suspension->leftRear.staticToe, Vector::AxisZ);

	// Now continue with the update for this corner
	LeftRearLowerAArm->Update(DisplayCar.suspension->leftRear.hardpoints[Corner::LowerFrontTubMount],
		DisplayCar.suspension->leftRear.hardpoints[Corner::LowerBallJoint],
		DisplayCar.suspension->leftRear.hardpoints[Corner::LowerRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	LeftRearUpperAArm->Update(DisplayCar.suspension->leftRear.hardpoints[Corner::UpperFrontTubMount],
		DisplayCar.suspension->leftRear.hardpoints[Corner::UpperBallJoint],
		DisplayCar.suspension->leftRear.hardpoints[Corner::UpperRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	LeftRearPushrod->Update(DisplayCar.suspension->leftRear.hardpoints[Corner::InboardPushrod],
		DisplayCar.suspension->leftRear.hardpoints[Corner::OutboardPushrod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	LeftRearTieRod->Update(DisplayCar.suspension->leftRear.hardpoints[Corner::InboardTieRod],
		DisplayCar.suspension->leftRear.hardpoints[Corner::OutboardTieRod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTieRod),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTieRod));
	LeftRearTire->Update(DisplayCar.suspension->leftRear.hardpoints[Corner::ContactPatch],
		DisplayCar.suspension->leftRear.hardpoints[Corner::WheelCenter], OriginalNormal, TargetNormal,
		DisplayCar.tires->leftRear->width,
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeTireInsideDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionTire),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTire),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTire));
	LeftRearDamper->Update(DisplayCar.suspension->leftRear.hardpoints[Corner::OutboardShock],
		DisplayCar.suspension->leftRear.hardpoints[Corner::InboardShock],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperBody),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityDamper));
	LeftRearSpring->Update(DisplayCar.suspension->leftRear.hardpoints[Corner::InboardSpring],
		DisplayCar.suspension->leftRear.hardpoints[Corner::OutboardSpring],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringEndPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSpring),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySpring));
	LeftRearUpright->Update(DisplayCar.suspension->leftRear.hardpoints[Corner::LowerBallJoint],
		DisplayCar.suspension->leftRear.hardpoints[Corner::UpperBallJoint],
		DisplayCar.suspension->leftRear.hardpoints[Corner::OutboardTieRod],
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorUpright),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityUpright));
	LeftRearBellCrank->Update(DisplayCar.suspension->leftRear.hardpoints[Corner::OutboardShock],
		DisplayCar.suspension->leftRear.hardpoints[Corner::InboardPushrod],
		VVASEMath::NearestPointOnAxis(DisplayCar.suspension->leftRear.hardpoints[Corner::BellCrankPivot1],
		DisplayCar.suspension->leftRear.hardpoints[Corner::BellCrankPivot2] -
		DisplayCar.suspension->leftRear.hardpoints[Corner::BellCrankPivot1],
		DisplayCar.suspension->leftRear.hardpoints[Corner::InboardPushrod]),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	LeftRearBarLink->Update(DisplayCar.suspension->leftRear.hardpoints[Corner::InboardBarLink],
		DisplayCar.suspension->leftRear.hardpoints[Corner::OutboardBarLink],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSwayBarLinkDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSwayBar),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSwayBar),
		ShowBarLinks);
	LeftRearHalfShaft->Update(DisplayCar.suspension->leftRear.hardpoints[Corner::InboardHalfShaft],
		DisplayCar.suspension->leftRear.hardpoints[Corner::OutboardHalfShaft],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeHalfShaftDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionHalfShaft),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorHalfShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityHalfShaft) && ReferenceCar.HasRearHalfShafts());

	// Front end
	SteeringRack->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::InboardTieRod],
		DisplayCar.suspension->leftFront.hardpoints[Corner::InboardTieRod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTieRod),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTieRod));
	FrontSwayBar->Update(DisplayCar.suspension->rightFront.hardpoints[Corner::InboardBarLink],
		DisplayCar.suspension->leftFront.hardpoints[Corner::InboardBarLink],
		DisplayCar.suspension->rightFront.hardpoints[Corner::BarArmAtPivot],
		DisplayCar.suspension->leftFront.hardpoints[Corner::BarArmAtPivot], DisplayCar.suspension->frontBarStyle,
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSwayBarDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSwayBar),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSwayBar),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySwayBar));

	// Rear end
	RearSwayBar->Update(DisplayCar.suspension->rightRear.hardpoints[Corner::InboardBarLink],
		DisplayCar.suspension->leftRear.hardpoints[Corner::InboardBarLink],
		DisplayCar.suspension->rightRear.hardpoints[Corner::BarArmAtPivot],
		DisplayCar.suspension->leftRear.hardpoints[Corner::BarArmAtPivot], DisplayCar.suspension->rearBarStyle,
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSwayBarDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSwayBar),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSwayBar),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySwayBar));

	// Update the helper orb
	// Determine which of the location variables is valid
	Vector HelperOrbPosition(0.0, 0.0, 0.0);
	if (HelperOrbCornerPoint != Corner::NumberOfHardpoints)
	{
		if (HelperOrbLocation == Corner::LocationRightFront)
			HelperOrbPosition = DisplayCar.suspension->rightFront.hardpoints[HelperOrbCornerPoint];
		else if (HelperOrbLocation == Corner::LocationLeftFront)
			HelperOrbPosition = DisplayCar.suspension->leftFront.hardpoints[HelperOrbCornerPoint];
		else if (HelperOrbLocation == Corner::LocationRightRear)
			HelperOrbPosition = DisplayCar.suspension->rightRear.hardpoints[HelperOrbCornerPoint];
		else
			HelperOrbPosition = DisplayCar.suspension->leftRear.hardpoints[HelperOrbCornerPoint];
	}
	else if (HelperOrbSuspensionPoint != Suspension::NumberOfHardpoints)
		HelperOrbPosition = DisplayCar.suspension->hardpoints[HelperOrbSuspensionPoint];

	HelperOrb->Update(HelperOrbPosition, AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeHelperOrbDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionHelperOrb),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorHelperOrb),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityHelperOrb) && HelperOrbIsActive);
}

//==========================================================================
// Class:			CAR_RENDERER
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
void CAR_RENDERER::UpdateKinematicsDisplay(KinematicOutputs outputs)
{
	// Update the point objects
	FrontRollCenter->Update(outputs.vectors[KinematicOutputs::FrontKinematicRC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorRollMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityRollCenter));
	RearRollCenter->Update(outputs.vectors[KinematicOutputs::RearKinematicRC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorRollMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityRollCenter));
	RightPitchCenter->Update(outputs.vectors[KinematicOutputs::RightKinematicPC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPitchMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPitchCenter));
	LeftPitchCenter->Update(outputs.vectors[KinematicOutputs::LeftKinematicPC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPitchMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPitchCenter));
	RightFrontInstantCenter->Update(outputs.rightFrontVectors[KinematicOutputs::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantCenter));
	LeftFrontInstantCenter->Update(outputs.leftFrontVectors[KinematicOutputs::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantCenter));
	RightRearInstantCenter->Update(outputs.rightRearVectors[KinematicOutputs::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantCenter));
	LeftRearInstantCenter->Update(outputs.leftRearVectors[KinematicOutputs::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantCenter));

	// Update the vector objects
	FrontRollAxis->Update(outputs.vectors[KinematicOutputs::FrontRollAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		outputs.vectors[KinematicOutputs::FrontKinematicRC], outputs.vectors[KinematicOutputs::FrontKinematicRC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorRollMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityRollAxis));
	RearRollAxis->Update(outputs.vectors[KinematicOutputs::RearRollAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		outputs.vectors[KinematicOutputs::RearKinematicRC], outputs.vectors[KinematicOutputs::RearKinematicRC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorRollMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityRollAxis));
	RightPitchAxis->Update(outputs.vectors[KinematicOutputs::RightPitchAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		outputs.vectors[KinematicOutputs::RightKinematicPC], outputs.vectors[KinematicOutputs::RightKinematicPC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPitchMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPitchAxis));
	LeftPitchAxis->Update(outputs.vectors[KinematicOutputs::LeftPitchAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		outputs.vectors[KinematicOutputs::LeftKinematicPC], outputs.vectors[KinematicOutputs::LeftKinematicPC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPitchMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPitchAxis));
	RightFrontInstantAxis->Update(outputs.rightFrontVectors[KinematicOutputs::InstantAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		outputs.rightFrontVectors[KinematicOutputs::InstantCenter],
		outputs.rightFrontVectors[KinematicOutputs::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantAxis));
	LeftFrontInstantAxis->Update(outputs.leftFrontVectors[KinematicOutputs::InstantAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		outputs.leftFrontVectors[KinematicOutputs::InstantCenter],
		outputs.leftFrontVectors[KinematicOutputs::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantAxis));
	RightRearInstantAxis->Update(outputs.rightRearVectors[KinematicOutputs::InstantAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		outputs.rightRearVectors[KinematicOutputs::InstantCenter],
		outputs.rightRearVectors[KinematicOutputs::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantAxis));
	LeftRearInstantAxis->Update(outputs.leftRearVectors[KinematicOutputs::InstantAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		outputs.leftRearVectors[KinematicOutputs::InstantCenter],
		outputs.leftRearVectors[KinematicOutputs::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantAxis));
}

//==========================================================================
// Class:			CAR_RENDERER
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
void CAR_RENDERER::CreateActors(void)
{
	// Create the origin and ground plane
	origin = new Origin(*this);
	GroundPlane = new Plane3D(*this);

	// Right front corner
	RightFrontLowerAArm = new AArm(*this);
	RightFrontUpperAArm = new AArm(*this);
	RightFrontPushrod = new Link(*this);
	RightFrontTieRod = new Link(*this);
	RightFrontTire = new Tire3D(*this);
	RightFrontDamper = new Damper3D(*this);
	RightFrontSpring = new Spring3D(*this);
	RightFrontUpright = new Triangle3D(*this);
	RightFrontBellCrank = new Triangle3D(*this);
	RightFrontBarLink = new Link(*this);
	RightFrontHalfShaft = new Link(*this);

	// Left front corner
	LeftFrontLowerAArm = new AArm(*this);
	LeftFrontUpperAArm = new AArm(*this);
	LeftFrontPushrod = new Link(*this);
	LeftFrontTieRod = new Link(*this);
	LeftFrontTire = new Tire3D(*this);
	LeftFrontDamper = new Damper3D(*this);
	LeftFrontSpring = new Spring3D(*this);
	LeftFrontUpright = new Triangle3D(*this);
	LeftFrontBellCrank = new Triangle3D(*this);
	LeftFrontBarLink = new Link(*this);
	LeftFrontHalfShaft = new Link(*this);

	// Right rear corner
	RightRearLowerAArm = new AArm(*this);
	RightRearUpperAArm = new AArm(*this);
	RightRearPushrod = new Link(*this);
	RightRearTieRod = new Link(*this);
	RightRearTire = new Tire3D(*this);
	RightRearDamper = new Damper3D(*this);
	RightRearSpring = new Spring3D(*this);
	RightRearUpright = new Triangle3D(*this);
	RightRearBellCrank = new Triangle3D(*this);
	RightRearBarLink = new Link(*this);
	RightRearHalfShaft = new Link(*this);

	// Left rear corner
	LeftRearLowerAArm = new AArm(*this);
	LeftRearUpperAArm = new AArm(*this);
	LeftRearPushrod = new Link(*this);
	LeftRearTieRod = new Link(*this);
	LeftRearTire = new Tire3D(*this);
	LeftRearDamper = new Damper3D(*this);
	LeftRearSpring = new Spring3D(*this);
	LeftRearUpright = new Triangle3D(*this);
	LeftRearBellCrank = new Triangle3D(*this);
	LeftRearBarLink = new Link(*this);
	LeftRearHalfShaft = new Link(*this);

	// Front end
	SteeringRack = new Link(*this);
	FrontSwayBar = new Swaybar3D(*this);

	// Rear end
	RearSwayBar = new Swaybar3D(*this);

	// Kinematic display objects
	FrontRollCenter = new Point3D(*this);
	RearRollCenter = new Point3D(*this);
	RightPitchCenter = new Point3D(*this);
	LeftPitchCenter = new Point3D(*this);
	RightFrontInstantCenter = new Point3D(*this);
	LeftFrontInstantCenter = new Point3D(*this);
	RightRearInstantCenter = new Point3D(*this);
	LeftRearInstantCenter = new Point3D(*this);

	FrontRollAxis = new Vector3D(*this);
	RearRollAxis = new Vector3D(*this);
	RightPitchAxis = new Vector3D(*this);
	LeftPitchAxis = new Vector3D(*this);
	RightFrontInstantAxis = new Vector3D(*this);
	LeftFrontInstantAxis = new Vector3D(*this);
	RightRearInstantAxis = new Vector3D(*this);
	LeftRearInstantAxis = new Vector3D(*this);

	// Helper orb
	HelperOrb = new Point3D(*this);
}

//==========================================================================
// Class:			CAR_RENDERER
// Function:		SetHelperOrbPosition
//
// Description:		Updates the position of the helper orb in the scene.
//					This takes five arguments, of which only one is (should
//					be) valid.
//
// Input Arguments:
//		CornerPoint		= const Corner::Hardpoints& identifying the point at the
//						  corner of the car
//		CornerLocation	= const Corner::Location& specifying the active corner
//		SuspensionPoint	= const Suspension::Hardpoints& identifying the point on
//						  the central part of the car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CAR_RENDERER::SetHelperOrbPosition(const Corner::Hardpoints &CornerPoint,
										const Corner::Location &CornerLocation,
										const Suspension::Hardpoints &SuspensionPoint)
{
	// Update the class members that describe the orb's position
	HelperOrbCornerPoint = CornerPoint;
	HelperOrbLocation = CornerLocation;
	HelperOrbSuspensionPoint = SuspensionPoint;

	// If we set the position, we'll assume that we want it to be active
	HelperOrbIsActive = true;
}