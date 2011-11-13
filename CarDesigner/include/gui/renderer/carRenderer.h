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

#ifndef _CAR_RENDERER_CLASS_H_
#define _CAR_RENDERER_CLASS_H_

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
class AARM;
class LINK;
class ORIGIN;
class PLANE;
class TIRE3D;
class DAMPER3D;
class SPRING3D;
class SWAYBAR3D;
class TRIANGLE3D;
class POINT3D;
class Vector3D;
class KINEMATIC_OUTPUTS;
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
	void UpdateDisplay(const KINEMATIC_OUTPUTS &Outputs);

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
	void UpdateKinematicsDisplay(KINEMATIC_OUTPUTS Outputs);

	// Pointers to the car objects that we are rendering
	APPEARANCE_OPTIONS &AppearanceOptions;
	CAR &DisplayCar;
	const CAR &ReferenceCar;// Required for correct representation of the tires - see UpdateDisplay()

	// The actors that we use to represent the car
	// The origin marker and ground plane
	ORIGIN *Origin;
	PLANE *GroundPlane;

	// Right front corner
	AARM *RightFrontLowerAArm;
	AARM *RightFrontUpperAArm;
	LINK *RightFrontTieRod;
	LINK *RightFrontPushrod;
	TIRE3D *RightFrontTire;
	DAMPER3D *RightFrontDamper;
	SPRING3D *RightFrontSpring;
	TRIANGLE3D *RightFrontUpright;
	TRIANGLE3D *RightFrontBellCrank;
	LINK *RightFrontBarLink;
	LINK *RightFrontHalfShaft;

	// Left front corner
	AARM *LeftFrontLowerAArm;
	AARM *LeftFrontUpperAArm;
	LINK *LeftFrontTieRod;
	LINK *LeftFrontPushrod;
	TIRE3D *LeftFrontTire;
	DAMPER3D *LeftFrontDamper;
	SPRING3D *LeftFrontSpring;
	TRIANGLE3D *LeftFrontUpright;
	TRIANGLE3D *LeftFrontBellCrank;
	LINK *LeftFrontBarLink;
	LINK *LeftFrontHalfShaft;

	// Right rear corner
	AARM *RightRearLowerAArm;
	AARM *RightRearUpperAArm;
	LINK *RightRearTieRod;
	LINK *RightRearPushrod;
	TIRE3D *RightRearTire;
	DAMPER3D *RightRearDamper;
	SPRING3D *RightRearSpring;
	TRIANGLE3D *RightRearUpright;
	TRIANGLE3D *RightRearBellCrank;
	LINK *RightRearBarLink;
	LINK *RightRearHalfShaft;

	// Left rear corner
	AARM *LeftRearLowerAArm;
	AARM *LeftRearUpperAArm;
	LINK *LeftRearTieRod;
	LINK *LeftRearPushrod;
	TIRE3D *LeftRearTire;
	DAMPER3D *LeftRearDamper;
	SPRING3D *LeftRearSpring;
	TRIANGLE3D *LeftRearUpright;
	TRIANGLE3D *LeftRearBellCrank;
	LINK *LeftRearBarLink;
	LINK *LeftRearHalfShaft;

	// Front end
	LINK *SteeringRack;
	SWAYBAR3D *FrontSwayBar;
	SPRING3D *FrontThirdSpring;
	DAMPER3D *FrontThirdDamper;

	// Rear end
	SWAYBAR3D *RearSwayBar;
	SPRING3D *RearThirdSpring;
	DAMPER3D *RearThirdDamper;

	// Kinematic output visualization
	POINT3D *FrontRollCenter;
	POINT3D *RearRollCenter;
	POINT3D *RightPitchCenter;
	POINT3D *LeftPitchCenter;
	POINT3D *RightFrontInstantCenter;
	POINT3D *LeftFrontInstantCenter;
	POINT3D *RightRearInstantCenter;
	POINT3D *LeftRearInstantCenter;

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
	POINT3D *HelperOrb;
};

#endif// _CAR_RENDERER_CLASS_H_