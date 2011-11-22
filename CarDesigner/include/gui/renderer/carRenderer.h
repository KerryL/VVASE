/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  carRenderer.h
// Created:  4/5/2008
// Author:  K. Loux
// Description:  Contains class declaration for the CAR_RENDERER class.  Derived from
//				 RENDER_WINDOW, this class is associated with a loaded car file
//				 and contains the information and methods required to render a car in 3D.
// History:
//	5/17/2009	- Was derived from wxVTKRenderWindowInteractor, switched to custom
//				  RENDER_WINDOW base class (eliminated dependence on VTK).
//	11/22/2009	- Moved to vRenderer.lib, K. Loux.

#ifndef _CAR_RENDERER_H_
#define _CAR_RENDERER_H_

// VVASE headers
#include "vMath/vector.h"
#include "vCar/suspension.h"
#include "vRenderer/renderWindow.h"
#include "gui/guiObject.h"

// wxWidgets forward declarations
class wxString;

// VVASE forward declarations
class Debugger;
class CAR;
class AArm;
class Link;
class Origin;
class Plane3D;
class Tire3D;
class Damper3D;
class Spring3D;
class Swaybar3D;
class Triangle3D;
class Point3D;
class Vector3D;
class KinematicOutputs;
class APPEARANCE_OPTIONS;
class GUI_CAR;
class MAIN_FRAME;

class CAR_RENDERER : public RenderWindow
{
public:
	// Constructor
	CAR_RENDERER(MAIN_FRAME &_MainFrame, GUI_CAR &_Car, int args[], const Debugger &_debugger);

	// Destructor
	~CAR_RENDERER();

	// Called to update the image on the screen
	void UpdateDisplay(const KinematicOutputs &outputs);

	// For writing the rendered image to file
	void WriteImageFile(wxString PathAndFileName);

	// Returns the type of the selected item, or -1 for no selection
	bool IsThisCarSelected(const Primitive *PickedActor, const GUI_OBJECT::ITEM_TYPE *ObjectType) const;

	// For accessing the helper orb
	void SetHelperOrbPosition(const CORNER::HARDPOINTS &CornerPoint, const CORNER::LOCATION &Location,
		const SUSPENSION::HARDPOINTS &SuspensionPoint);
	inline void DeactivateHelperOrb(void) { HelperOrbIsActive = false; };

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// For context menus
	MAIN_FRAME &MainFrame;

	// Called from the CAR_RENDERER constructor only in order to initialize the display
	void CreateActors(void);

	// The methods that perform the updating
	void UpdateCarDisplay(void);
	void UpdateKinematicsDisplay(KinematicOutputs Outputs);

	// Pointers to the car objects that we are rendering
	APPEARANCE_OPTIONS &AppearanceOptions;
	CAR &DisplayCar;
	const CAR &ReferenceCar;// Required for correct representation of the tires - see UpdateDisplay()

	// The actors that we use to represent the car
	// The origin marker and ground plane
	Origin *origin;
	Plane3D *GroundPlane;

	// Right front corner
	AArm *RightFrontLowerAArm;
	AArm *RightFrontUpperAArm;
	Link *RightFrontTieRod;
	Link *RightFrontPushrod;
	Tire3D *RightFrontTire;
	Damper3D *RightFrontDamper;
	Spring3D *RightFrontSpring;
	Triangle3D *RightFrontUpright;
	Triangle3D *RightFrontBellCrank;
	Link *RightFrontBarLink;
	Link *RightFrontHalfShaft;

	// Left front corner
	AArm *LeftFrontLowerAArm;
	AArm *LeftFrontUpperAArm;
	Link *LeftFrontTieRod;
	Link *LeftFrontPushrod;
	Tire3D *LeftFrontTire;
	Damper3D *LeftFrontDamper;
	Spring3D *LeftFrontSpring;
	Triangle3D *LeftFrontUpright;
	Triangle3D *LeftFrontBellCrank;
	Link *LeftFrontBarLink;
	Link *LeftFrontHalfShaft;

	// Right rear corner
	AArm *RightRearLowerAArm;
	AArm *RightRearUpperAArm;
	Link *RightRearTieRod;
	Link *RightRearPushrod;
	Tire3D *RightRearTire;
	Damper3D *RightRearDamper;
	Spring3D *RightRearSpring;
	Triangle3D *RightRearUpright;
	Triangle3D *RightRearBellCrank;
	Link *RightRearBarLink;
	Link *RightRearHalfShaft;

	// Left rear corner
	AArm *LeftRearLowerAArm;
	AArm *LeftRearUpperAArm;
	Link *LeftRearTieRod;
	Link *LeftRearPushrod;
	Tire3D *LeftRearTire;
	Damper3D *LeftRearDamper;
	Spring3D *LeftRearSpring;
	Triangle3D *LeftRearUpright;
	Triangle3D *LeftRearBellCrank;
	Link *LeftRearBarLink;
	Link *LeftRearHalfShaft;

	// Front end
	Link *SteeringRack;
	Swaybar3D *FrontSwayBar;
	Spring3D *FrontThirdSpring;
	Damper3D *FrontThirdDamper;

	// Rear end
	Swaybar3D *RearSwayBar;
	Spring3D *RearThirdSpring;
	Damper3D *RearThirdDamper;

	// Kinematic output visualization
	Point3D *FrontRollCenter;
	Point3D *RearRollCenter;
	Point3D *RightPitchCenter;
	Point3D *LeftPitchCenter;
	Point3D *RightFrontInstantCenter;
	Point3D *LeftFrontInstantCenter;
	Point3D *RightRearInstantCenter;
	Point3D *LeftRearInstantCenter;

	Vector3D *FrontRollAxis;
	Vector3D *RearRollAxis;
	Vector3D *RightPitchAxis;
	Vector3D *LeftPitchAxis;
	Vector3D *RightFrontInstantAxis;
	Vector3D *LeftFrontInstantAxis;
	Vector3D *RightRearInstantAxis;
	Vector3D *LeftRearInstantAxis;

	// Helper orb
	CORNER::HARDPOINTS HelperOrbCornerPoint;
	CORNER::LOCATION HelperOrbLocation;
	SUSPENSION::HARDPOINTS HelperOrbSuspensionPoint;
	bool HelperOrbIsActive;
	Point3D *HelperOrb;
};

#endif// _CAR_RENDERER_H_