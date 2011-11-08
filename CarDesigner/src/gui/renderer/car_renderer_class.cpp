/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  car_renderer_class.cpp
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
#include "gui/renderer/car_renderer_class.h"
#include "vRenderer/3dcar/aarm_class.h"
#include "vRenderer/3dcar/link_class.h"
#include "vRenderer/3dcar/triangle_class.h"
#include "vRenderer/3dcar/damper3d_class.h"
#include "vRenderer/3dcar/spring3d_class.h"
#include "vRenderer/3dcar/plane_class.h"
#include "vRenderer/3dcar/point3d_class.h"
#include "vRenderer/3dcar/origin_class.h"
#include "vRenderer/3dcar/tire3d_class.h"
#include "vRenderer/3dcar/swaybar3d_class.h"
#include "vRenderer/3dcar/vector3d_class.h"
#include "vCar/car_class.h"
#include "vCar/brakes_class.h"
#include "vCar/drivetrain_class.h"
#include "vCar/suspension_class.h"
#include "vCar/tire_class.h"
#include "vCar/tireset_class.h"
#include "vUtilities/debug_class.h"
#include "vMath/car_math.h"
#include "vMath/vector_class.h"
#include "gui/gui_car_class.h"
#include "gui/appearance_options_class.h"
#include "gui/components/main_frame_class.h"
#include "vSolver/physics/kinematic_outputs_class.h"

#include "vMath/matrix_class.h"

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
//		_debugger	= const Debugger&, reference to debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
CAR_RENDERER::CAR_RENDERER(MAIN_FRAME &_MainFrame, GUI_CAR &_Car, const Debugger &_debugger)
						   : RenderWindow(_MainFrame, wxID_ANY, wxDefaultPosition,
						   wxDefaultSize, wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE),
						   debugger(_debugger), MainFrame(_MainFrame),
						   AppearanceOptions(_Car.GetAppearanceOptions()),
						   DisplayCar(_Car.GetWorkingCar()), ReferenceCar(_Car.GetOriginalCar())
{
	// Create the objects neccessary to render the scene
	CreateActors();

	// Initialize the helper orb to NOT active
	HelperOrbCornerPoint = CORNER::NumberOfHardpoints;
	HelperOrbLocation = CORNER::LocationRightFront;
	HelperOrbSuspensionPoint = SUSPENSION::NumberOfHardpoints;
	HelperOrbIsActive = false;

	// Set the camera view so that the car is visible
	Vector Position(-100.0, -100.0, 60.0), Up(0.0, 0.0, 1.0);
	Vector LookAt(ReferenceCar.Suspension->LeftRear.Hardpoints[CORNER::ContactPatch].x / 2.0, 0.0, 0.0);// FIXME:  This could be better
	SetCameraView(Position, LookAt, Up);
}

//==========================================================================
// Class:			CAR_RENDERER
// Function:		~CAR_RENDERER
//
// Description:		Denstructor for CAR_RENDERER class.
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
	delete Origin;
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
//		Outputs	= const KINEMATIC_OUTPUTS& containing the locations of all of the
//				  kinematic variables that this object displays in the scene
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CAR_RENDERER::UpdateDisplay(const KINEMATIC_OUTPUTS &Outputs)
{
	// Set the background color
	SetBackgroundColor(AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorBackground));

	// Update the car display
	UpdateCarDisplay();

	// Update the kinematic display
	UpdateKinematicsDisplay(Outputs);

	// Render the image.  We need to reset the frustum every time we do this just in case
	// the user zooms out very far, etc.
	AutoSetFrustum();
	Render();

	return;
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
	wxMutexLocker DisplayLock(DisplayCar.GetMutex());
	wxMutexLocker ReferenceLock(ReferenceCar.GetMutex());

	// Update the origin
	Origin->Update(AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeOriginShaftLength),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeOriginShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeOriginTipLength),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeOriginTipDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionOrigin),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityOrigin));

	// Figure out how big the ground plane should be
	double GroundPlaneScaleUp = 1.1;
	double RightMostPoint = max(ReferenceCar.Suspension->RightFront.Hardpoints[CORNER::ContactPatch].y +
		ReferenceCar.Tires->RightFront->Width / 2.0,
		ReferenceCar.Suspension->RightRear.Hardpoints[CORNER::ContactPatch].y +
		ReferenceCar.Tires->RightRear->Width / 2.0);
	double LeftMostPoint = min(ReferenceCar.Suspension->LeftFront.Hardpoints[CORNER::ContactPatch].y -
		ReferenceCar.Tires->LeftFront->Width / 2.0,
		ReferenceCar.Suspension->LeftRear.Hardpoints[CORNER::ContactPatch].y -
		ReferenceCar.Tires->LeftRear->Width / 2.0);
	double FrontMostPoint = min(ReferenceCar.Suspension->RightFront.Hardpoints[CORNER::ContactPatch].x -
		ReferenceCar.Tires->RightFront->Diameter / 2.0,
		ReferenceCar.Suspension->LeftFront.Hardpoints[CORNER::ContactPatch].x -
		ReferenceCar.Tires->LeftFront->Diameter / 2.0);
	double RearMostPoint = max(ReferenceCar.Suspension->RightRear.Hardpoints[CORNER::ContactPatch].x +
		ReferenceCar.Tires->RightRear->Diameter / 2.0,
		ReferenceCar.Suspension->LeftRear.Hardpoints[CORNER::ContactPatch].x +
		ReferenceCar.Tires->LeftRear->Diameter / 2.0);

	double XLength = fabs(FrontMostPoint - RearMostPoint) * GroundPlaneScaleUp;
	double YLength = fabs(RightMostPoint - LeftMostPoint) * GroundPlaneScaleUp;
	double XOffset = (RearMostPoint - FrontMostPoint) / 2.0 + FrontMostPoint;
	GroundPlane->Update(XLength, YLength, XOffset,
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorGroundPlane),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityGroundPlane));

	// Copy the tire paramters from the reference car
	DisplayCar.Tires->RightFront->Width = ReferenceCar.Tires->RightFront->Width;
	DisplayCar.Tires->LeftFront->Width = ReferenceCar.Tires->LeftFront->Width;
	DisplayCar.Tires->RightRear->Width = ReferenceCar.Tires->RightRear->Width;
	DisplayCar.Tires->LeftRear->Width = ReferenceCar.Tires->LeftRear->Width;

	// Determine whether or not the front sway bar links should be visible
	bool ShowBarLinks = true;
	if (DisplayCar.Suspension->FrontBarStyle == SUSPENSION::SwayBarNone ||
		!AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySwayBar))
		ShowBarLinks = false;

	// Right front corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	bool ShowBellCranksPushrods = true;
	if (DisplayCar.Suspension->RightFront.ActuationType == CORNER::ActuationOutboard ||
		!AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	TargetNormal = VVASEMath::GetPlaneNormal(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::LowerBallJoint],
							DisplayCar.Suspension->RightFront.Hardpoints[CORNER::UpperBallJoint],
							DisplayCar.Suspension->RightFront.Hardpoints[CORNER::OutboardTieRod]);
	OriginalNormal = VVASEMath::GetPlaneNormal(ReferenceCar.Suspension->RightFront.Hardpoints[CORNER::LowerBallJoint],
							ReferenceCar.Suspension->RightFront.Hardpoints[CORNER::UpperBallJoint],
							ReferenceCar.Suspension->RightFront.Hardpoints[CORNER::OutboardTieRod]);

	// We also need to account for toe and camber settings for the TargetNormal - apply camber first
	// NOTE:  This corner is on the right side of the car - we flip the sign on the camber and toe angles
	TargetNormal.Rotate(-DisplayCar.Suspension->RightFront.StaticCamber, Vector::AxisX);
	TargetNormal.Rotate(-DisplayCar.Suspension->RightFront.StaticToe, Vector::AxisZ);

	// Now continue with the update for this corner
	RightFrontLowerAArm->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::LowerFrontTubMount],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::LowerBallJoint],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::LowerRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	RightFrontUpperAArm->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::UpperFrontTubMount],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::UpperBallJoint],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::UpperRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	RightFrontPushrod->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::InboardPushrod],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::OutboardPushrod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	RightFrontTieRod->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::InboardTieRod],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::OutboardTieRod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTieRod),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTieRod));
	RightFrontTire->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::ContactPatch],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::WheelCenter], OriginalNormal, TargetNormal,
		DisplayCar.Tires->RightFront->Width,
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeTireInsideDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionTire),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTire),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTire));
	RightFrontDamper->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::OutboardShock],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::InboardShock],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperBody),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityDamper));
	RightFrontSpring->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::InboardSpring],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::OutboardSpring],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringEndPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSpring),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySpring));
	RightFrontUpright->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::LowerBallJoint],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::UpperBallJoint],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::OutboardTieRod],
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorUpright),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityUpright));
	RightFrontBellCrank->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::OutboardShock],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::InboardPushrod],
		VVASEMath::NearestPointOnAxis(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::BellCrankPivot1],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::BellCrankPivot2] -
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::BellCrankPivot1],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::InboardPushrod]),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	RightFrontBarLink->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::InboardBarLink],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::OutboardBarLink],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSwayBarLinkDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSwayBar),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSwayBar),
		ShowBarLinks);
	RightFrontHalfShaft->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::InboardHalfShaft],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::OutboardHalfShaft],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeHalfShaftDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionHalfShaft),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorHalfShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityHalfShaft) && ReferenceCar.HasFrontHalfShafts());

	// Left front corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (DisplayCar.Suspension->LeftFront.ActuationType == CORNER::ActuationOutboard ||
		!AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	TargetNormal = VVASEMath::GetPlaneNormal(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::LowerBallJoint],
							DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::UpperBallJoint],
							DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::OutboardTieRod]);
	OriginalNormal = VVASEMath::GetPlaneNormal(ReferenceCar.Suspension->LeftFront.Hardpoints[CORNER::LowerBallJoint],
							ReferenceCar.Suspension->LeftFront.Hardpoints[CORNER::UpperBallJoint],
							ReferenceCar.Suspension->LeftFront.Hardpoints[CORNER::OutboardTieRod]);

	// We also need to account for toe and camber settings for the TargetNormal - apply camber first
	TargetNormal.Rotate(DisplayCar.Suspension->LeftFront.StaticCamber, Vector::AxisX);
	TargetNormal.Rotate(DisplayCar.Suspension->LeftFront.StaticToe, Vector::AxisZ);

	// Now continue with the update for this corner
	LeftFrontLowerAArm->Update(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::LowerFrontTubMount],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::LowerBallJoint],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::LowerRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	LeftFrontUpperAArm->Update(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::UpperFrontTubMount],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::UpperBallJoint],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::UpperRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	LeftFrontPushrod->Update(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::InboardPushrod],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::OutboardPushrod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	LeftFrontTieRod->Update(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::InboardTieRod],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::OutboardTieRod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTieRod),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTieRod));
	LeftFrontTire->Update(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::ContactPatch],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::WheelCenter], OriginalNormal, TargetNormal,
		DisplayCar.Tires->LeftFront->Width,
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeTireInsideDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionTire),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTire),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTire));
	LeftFrontDamper->Update(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::OutboardShock],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::InboardShock],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperBody),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityDamper));
	LeftFrontSpring->Update(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::InboardSpring],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::OutboardSpring],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringEndPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSpring),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySpring));
	LeftFrontUpright->Update(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::LowerBallJoint],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::UpperBallJoint],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::OutboardTieRod],
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorUpright),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityUpright));
	LeftFrontBellCrank->Update(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::OutboardShock],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::InboardPushrod],
		VVASEMath::NearestPointOnAxis(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::BellCrankPivot1],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::BellCrankPivot2] -
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::BellCrankPivot1],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::InboardPushrod]),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	LeftFrontBarLink->Update(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::InboardBarLink],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::OutboardBarLink],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSwayBarLinkDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSwayBar),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSwayBar),
		ShowBarLinks);
	LeftFrontHalfShaft->Update(DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::InboardHalfShaft],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::OutboardHalfShaft],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeHalfShaftDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionHalfShaft),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorHalfShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityHalfShaft) && ReferenceCar.HasFrontHalfShafts());

	// Determine whether or not the rear sway bar links should be visible
	if (DisplayCar.Suspension->RearBarStyle == SUSPENSION::SwayBarNone ||
		!AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySwayBar))
		ShowBarLinks = false;
	else
		ShowBarLinks = true;

	// Right rear corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (DisplayCar.Suspension->RightRear.ActuationType == CORNER::ActuationOutboard ||
		!AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	TargetNormal = VVASEMath::GetPlaneNormal(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::LowerBallJoint],
							DisplayCar.Suspension->RightRear.Hardpoints[CORNER::UpperBallJoint],
							DisplayCar.Suspension->RightRear.Hardpoints[CORNER::OutboardTieRod]);
	OriginalNormal = VVASEMath::GetPlaneNormal(ReferenceCar.Suspension->RightRear.Hardpoints[CORNER::LowerBallJoint],
							ReferenceCar.Suspension->RightRear.Hardpoints[CORNER::UpperBallJoint],
							ReferenceCar.Suspension->RightRear.Hardpoints[CORNER::OutboardTieRod]);

	// We also need to account for toe and camber settings for the TargetNormal - apply camber first
	// NOTE:  This corner is on the right side of the car - we flip the sign on the camber and toe angles
	TargetNormal.Rotate(-DisplayCar.Suspension->RightRear.StaticCamber, Vector::AxisX);
	TargetNormal.Rotate(-DisplayCar.Suspension->RightRear.StaticToe, Vector::AxisZ);

	// Now continue with the update for this corner
	RightRearLowerAArm->Update(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::LowerFrontTubMount],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::LowerBallJoint],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::LowerRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	RightRearUpperAArm->Update(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::UpperFrontTubMount],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::UpperBallJoint],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::UpperRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	RightRearPushrod->Update(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::InboardPushrod],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::OutboardPushrod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	RightRearTieRod->Update(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::InboardTieRod],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::OutboardTieRod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTieRod),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTieRod));
	RightRearTire->Update(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::ContactPatch],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::WheelCenter], OriginalNormal, TargetNormal,
		DisplayCar.Tires->RightRear->Width,
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeTireInsideDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionTire),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTire),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTire));
	RightRearDamper->Update(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::OutboardShock],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::InboardShock],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperBody),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityDamper));
	RightRearSpring->Update(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::InboardSpring],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::OutboardSpring],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringEndPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSpring),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySpring));
	RightRearUpright->Update(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::LowerBallJoint],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::UpperBallJoint],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::OutboardTieRod],
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorUpright),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityUpright));
	RightRearBellCrank->Update(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::OutboardShock],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::InboardPushrod],
		VVASEMath::NearestPointOnAxis(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::BellCrankPivot1],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::BellCrankPivot2] -
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::BellCrankPivot1],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::InboardPushrod]),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	RightRearBarLink->Update(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::InboardBarLink],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::OutboardBarLink],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSwayBarLinkDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSwayBar),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSwayBar),
		ShowBarLinks);
	RightRearHalfShaft->Update(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::InboardHalfShaft],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::OutboardHalfShaft],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeHalfShaftDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionHalfShaft),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorHalfShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityHalfShaft) && ReferenceCar.HasRearHalfShafts());

	// Left rear corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (DisplayCar.Suspension->LeftRear.ActuationType == CORNER::ActuationOutboard ||
		!AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	TargetNormal = VVASEMath::GetPlaneNormal(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::LowerBallJoint],
							DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::UpperBallJoint],
							DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::OutboardTieRod]);
	OriginalNormal = VVASEMath::GetPlaneNormal(ReferenceCar.Suspension->LeftRear.Hardpoints[CORNER::LowerBallJoint],
							ReferenceCar.Suspension->LeftRear.Hardpoints[CORNER::UpperBallJoint],
							ReferenceCar.Suspension->LeftRear.Hardpoints[CORNER::OutboardTieRod]);

	// We also need to account for toe and camber settings for the TargetNormal - apply camber first
	TargetNormal.Rotate(DisplayCar.Suspension->LeftRear.StaticCamber, Vector::AxisX);
	TargetNormal.Rotate(DisplayCar.Suspension->LeftRear.StaticToe, Vector::AxisZ);

	// Now continue with the update for this corner
	LeftRearLowerAArm->Update(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::LowerFrontTubMount],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::LowerBallJoint],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::LowerRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	LeftRearUpperAArm->Update(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::UpperFrontTubMount],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::UpperBallJoint],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::UpperRearTubMount],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorAArm),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityAArm));
	LeftRearPushrod->Update(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::InboardPushrod],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::OutboardPushrod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	LeftRearTieRod->Update(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::InboardTieRod],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::OutboardTieRod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTieRod),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTieRod));
	LeftRearTire->Update(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::ContactPatch],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::WheelCenter], OriginalNormal, TargetNormal,
		DisplayCar.Tires->LeftRear->Width,
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeTireInsideDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionTire),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTire),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTire));
	LeftRearDamper->Update(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::OutboardShock],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::InboardShock],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeDamperBodyLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperBody),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorDamperShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityDamper));
	LeftRearSpring->Update(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::InboardSpring],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::OutboardSpring],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSpringEndPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSpringDamper),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSpring),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySpring));
	LeftRearUpright->Update(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::LowerBallJoint],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::UpperBallJoint],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::OutboardTieRod],
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorUpright),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityUpright));
	LeftRearBellCrank->Update(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::OutboardShock],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::InboardPushrod],
		VVASEMath::NearestPointOnAxis(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::BellCrankPivot1],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::BellCrankPivot2] -
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::BellCrankPivot1],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::InboardPushrod]),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPushrod),
		ShowBellCranksPushrods);
	LeftRearBarLink->Update(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::InboardBarLink],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::OutboardBarLink],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSwayBarLinkDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSwayBar),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSwayBar),
		ShowBarLinks);
	LeftRearHalfShaft->Update(DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::InboardHalfShaft],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::OutboardHalfShaft],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeHalfShaftDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionHalfShaft),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorHalfShaft),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityHalfShaft) && ReferenceCar.HasRearHalfShafts());

	// Front end
	SteeringRack->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::InboardTieRod],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::InboardTieRod],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeAArmDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionAArm),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorTieRod),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityTieRod));
	FrontSwayBar->Update(DisplayCar.Suspension->RightFront.Hardpoints[CORNER::InboardBarLink],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::InboardBarLink],
		DisplayCar.Suspension->RightFront.Hardpoints[CORNER::BarArmAtPivot],
		DisplayCar.Suspension->LeftFront.Hardpoints[CORNER::BarArmAtPivot], DisplayCar.Suspension->FrontBarStyle,
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSwayBarDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSwayBar),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSwayBar),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySwayBar));

	// Rear end
	RearSwayBar->Update(DisplayCar.Suspension->RightRear.Hardpoints[CORNER::InboardBarLink],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::InboardBarLink],
		DisplayCar.Suspension->RightRear.Hardpoints[CORNER::BarArmAtPivot],
		DisplayCar.Suspension->LeftRear.Hardpoints[CORNER::BarArmAtPivot], DisplayCar.Suspension->RearBarStyle,
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeSwayBarDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionSwayBar),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorSwayBar),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilitySwayBar));

	// Update the helper orb
	// Determine which of the location variables is valid
	Vector HelperOrbPosition(0.0, 0.0, 0.0);
	if (HelperOrbCornerPoint != CORNER::NumberOfHardpoints)
	{
		if (HelperOrbLocation == CORNER::LocationRightFront)
			HelperOrbPosition = DisplayCar.Suspension->RightFront.Hardpoints[HelperOrbCornerPoint];
		else if (HelperOrbLocation == CORNER::LocationLeftFront)
			HelperOrbPosition = DisplayCar.Suspension->LeftFront.Hardpoints[HelperOrbCornerPoint];
		else if (HelperOrbLocation == CORNER::LocationRightRear)
			HelperOrbPosition = DisplayCar.Suspension->RightRear.Hardpoints[HelperOrbCornerPoint];
		else
			HelperOrbPosition = DisplayCar.Suspension->LeftRear.Hardpoints[HelperOrbCornerPoint];
	}
	else if (HelperOrbSuspensionPoint != SUSPENSION::NumberOfHardpoints)
		HelperOrbPosition = DisplayCar.Suspension->Hardpoints[HelperOrbSuspensionPoint];

	HelperOrb->Update(HelperOrbPosition, AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeHelperOrbDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionHelperOrb),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorHelperOrb),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityHelperOrb) && HelperOrbIsActive);

	return;
}

//==========================================================================
// Class:			CAR_RENDERER
// Function:		UpdateKinematicsDisplay
//
// Description:		Updates the displayed 3D kinematic objects
//
// Input Arguments:
//		Outputs	= KINEMATIC_OUTPUTS containing the locations of all of the
//				  kinematic variables that this object displays in the scene
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CAR_RENDERER::UpdateKinematicsDisplay(KINEMATIC_OUTPUTS Outputs)
{
	// Update the point objects
	FrontRollCenter->Update(Outputs.Vectors[KINEMATIC_OUTPUTS::FrontKinematicRC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorRollMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityRollCenter));
	RearRollCenter->Update(Outputs.Vectors[KINEMATIC_OUTPUTS::RearKinematicRC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorRollMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityRollCenter));
	RightPitchCenter->Update(Outputs.Vectors[KINEMATIC_OUTPUTS::RightKinematicPC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPitchMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPitchCenter));
	LeftPitchCenter->Update(Outputs.Vectors[KINEMATIC_OUTPUTS::LeftKinematicPC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPitchMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPitchCenter));
	RightFrontInstantCenter->Update(Outputs.RightFrontVectors[KINEMATIC_OUTPUTS::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantCenter));
	LeftFrontInstantCenter->Update(Outputs.LeftFrontVectors[KINEMATIC_OUTPUTS::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantCenter));
	RightRearInstantCenter->Update(Outputs.RightRearVectors[KINEMATIC_OUTPUTS::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantCenter));
	LeftRearInstantCenter->Update(Outputs.LeftRearVectors[KINEMATIC_OUTPUTS::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerPointDiameter),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantCenter));

	// Update the vector objects
	FrontRollAxis->Update(Outputs.Vectors[KINEMATIC_OUTPUTS::FrontRollAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		Outputs.Vectors[KINEMATIC_OUTPUTS::FrontKinematicRC], Outputs.Vectors[KINEMATIC_OUTPUTS::FrontKinematicRC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorRollMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityRollAxis));
	RearRollAxis->Update(Outputs.Vectors[KINEMATIC_OUTPUTS::RearRollAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		Outputs.Vectors[KINEMATIC_OUTPUTS::RearKinematicRC], Outputs.Vectors[KINEMATIC_OUTPUTS::RearKinematicRC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorRollMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityRollAxis));
	RightPitchAxis->Update(Outputs.Vectors[KINEMATIC_OUTPUTS::RightPitchAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		Outputs.Vectors[KINEMATIC_OUTPUTS::RightKinematicPC], Outputs.Vectors[KINEMATIC_OUTPUTS::RightKinematicPC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPitchMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPitchAxis));
	LeftPitchAxis->Update(Outputs.Vectors[KINEMATIC_OUTPUTS::LeftPitchAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		Outputs.Vectors[KINEMATIC_OUTPUTS::LeftKinematicPC], Outputs.Vectors[KINEMATIC_OUTPUTS::LeftKinematicPC],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorPitchMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityPitchAxis));
	RightFrontInstantAxis->Update(Outputs.RightFrontVectors[KINEMATIC_OUTPUTS::InstantAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		Outputs.RightFrontVectors[KINEMATIC_OUTPUTS::InstantCenter],
		Outputs.RightFrontVectors[KINEMATIC_OUTPUTS::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantAxis));
	LeftFrontInstantAxis->Update(Outputs.LeftFrontVectors[KINEMATIC_OUTPUTS::InstantAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		Outputs.LeftFrontVectors[KINEMATIC_OUTPUTS::InstantCenter],
		Outputs.LeftFrontVectors[KINEMATIC_OUTPUTS::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantAxis));
	RightRearInstantAxis->Update(Outputs.RightRearVectors[KINEMATIC_OUTPUTS::InstantAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		Outputs.RightRearVectors[KINEMATIC_OUTPUTS::InstantCenter],
		Outputs.RightRearVectors[KINEMATIC_OUTPUTS::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantAxis));
	LeftRearInstantAxis->Update(Outputs.LeftRearVectors[KINEMATIC_OUTPUTS::InstantAxisDirection] *
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerLength) +
		Outputs.LeftRearVectors[KINEMATIC_OUTPUTS::InstantCenter],
		Outputs.LeftRearVectors[KINEMATIC_OUTPUTS::InstantCenter],
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerShaftDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipDiameter),
		AppearanceOptions.GetSize(APPEARANCE_OPTIONS::SizeMarkerTipLength),
		AppearanceOptions.GetResolution(APPEARANCE_OPTIONS::ResolutionMarker),
		AppearanceOptions.GetColor(APPEARANCE_OPTIONS::ColorInstantMarker),
		AppearanceOptions.GetVisibility(APPEARANCE_OPTIONS::VisibilityInstantAxis));

	return;
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
	Origin = new ORIGIN(*this);
	GroundPlane = new PLANE(*this);

	// Right front corner
	RightFrontLowerAArm = new AARM(*this);
	RightFrontUpperAArm = new AARM(*this);
	RightFrontPushrod = new LINK(*this);
	RightFrontTieRod = new LINK(*this);
	RightFrontTire = new TIRE3D(*this);
	RightFrontDamper = new DAMPER3D(*this);
	RightFrontSpring = new SPRING3D(*this);
	RightFrontUpright = new TRIANGLE3D(*this);
	RightFrontBellCrank = new TRIANGLE3D(*this);
	RightFrontBarLink = new LINK(*this);
	RightFrontHalfShaft = new LINK(*this);

	// Left front corner
	LeftFrontLowerAArm = new AARM(*this);
	LeftFrontUpperAArm = new AARM(*this);
	LeftFrontPushrod = new LINK(*this);
	LeftFrontTieRod = new LINK(*this);
	LeftFrontTire = new TIRE3D(*this);
	LeftFrontDamper = new DAMPER3D(*this);
	LeftFrontSpring = new SPRING3D(*this);
	LeftFrontUpright = new TRIANGLE3D(*this);
	LeftFrontBellCrank = new TRIANGLE3D(*this);
	LeftFrontBarLink = new LINK(*this);
	LeftFrontHalfShaft = new LINK(*this);

	// Right rear corner
	RightRearLowerAArm = new AARM(*this);
	RightRearUpperAArm = new AARM(*this);
	RightRearPushrod = new LINK(*this);
	RightRearTieRod = new LINK(*this);
	RightRearTire = new TIRE3D(*this);
	RightRearDamper = new DAMPER3D(*this);
	RightRearSpring = new SPRING3D(*this);
	RightRearUpright = new TRIANGLE3D(*this);
	RightRearBellCrank = new TRIANGLE3D(*this);
	RightRearBarLink = new LINK(*this);
	RightRearHalfShaft = new LINK(*this);

	// Left rear corner
	LeftRearLowerAArm = new AARM(*this);
	LeftRearUpperAArm = new AARM(*this);
	LeftRearPushrod = new LINK(*this);
	LeftRearTieRod = new LINK(*this);
	LeftRearTire = new TIRE3D(*this);
	LeftRearDamper = new DAMPER3D(*this);
	LeftRearSpring = new SPRING3D(*this);
	LeftRearUpright = new TRIANGLE3D(*this);
	LeftRearBellCrank = new TRIANGLE3D(*this);
	LeftRearBarLink = new LINK(*this);
	LeftRearHalfShaft = new LINK(*this);

	// Front end
	SteeringRack = new LINK(*this);
	FrontSwayBar = new SWAYBAR3D(*this);

	// Rear end
	RearSwayBar = new SWAYBAR3D(*this);

	// Kinematic display objects
	FrontRollCenter = new POINT3D(*this);
	RearRollCenter = new POINT3D(*this);
	RightPitchCenter = new POINT3D(*this);
	LeftPitchCenter = new POINT3D(*this);
	RightFrontInstantCenter = new POINT3D(*this);
	LeftFrontInstantCenter = new POINT3D(*this);
	RightRearInstantCenter = new POINT3D(*this);
	LeftRearInstantCenter = new POINT3D(*this);

	FrontRollAxis = new Vector3D(*this);
	RearRollAxis = new Vector3D(*this);
	RightPitchAxis = new Vector3D(*this);
	LeftPitchAxis = new Vector3D(*this);
	RightFrontInstantAxis = new Vector3D(*this);
	LeftFrontInstantAxis = new Vector3D(*this);
	RightRearInstantAxis = new Vector3D(*this);
	LeftRearInstantAxis = new Vector3D(*this);

	// Helper orb
	HelperOrb = new POINT3D(*this);

	return;
}

//==========================================================================
// Class:			CAR_RENDERER
// Function:		SetHelperOrbPosition
//
// Description:		Updates the position of the helper orb in the scene.
//					This takes five arguements, of which only one is (should
//					be) valid.
//
// Input Arguments:
//		CornerPoint		= const CORNER::HARDPOINTS& identifying the point at the
//						  corner of the car
//		CornerLocation	= const CORNER::HARDPOINTS& specifying the active corner
//		SuspensionPoint	= const SUSPENSION::HARDPOINTS& identifying the point on
//						  the central part of the car
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CAR_RENDERER::SetHelperOrbPosition(const CORNER::HARDPOINTS &CornerPoint,
										const CORNER::LOCATION &CornerLocation,
										const SUSPENSION::HARDPOINTS &SuspensionPoint)
{
	// Update the class members that describe the orb's position
	HelperOrbCornerPoint = CornerPoint;
	HelperOrbLocation = CornerLocation;
	HelperOrbSuspensionPoint = SuspensionPoint;

	// If we set the position, we'll assume that we want it to be active
	HelperOrbIsActive = true;

	return;
}