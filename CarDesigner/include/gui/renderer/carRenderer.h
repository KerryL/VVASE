/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  carRenderer.h
// Created:  4/5/2008
// Author:  K. Loux
// Description:  Contains class declaration for the CarRenderer class.  Derived from
//				 RenderWindow, this class is associated with a loaded car file
//				 and contains the information and methods required to render a car in 3D.
// History:
//	5/17/2009	- Was derived from wxVTKRenderWindowInteractor, switched to custom
//				  RenderWindow base class (eliminated dependence on VTK).
//	11/22/2009	- Moved to vRenderer.lib, K. Loux.

#ifndef CAR_RENDERER_H_
#define CAR_RENDERER_H_

// VVASE headers
#include "vMath/vector.h"
#include "vCar/suspension.h"
#include "vRenderer/renderWindow.h"
#include "gui/guiObject.h"

// wxWidgets forward declarations
class wxString;

// VVASE forward declarations
class Car;
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
class AppearanceOptions;
class GuiCar;
class MainFrame;

class CarRenderer : public RenderWindow
{
public:
	CarRenderer(MainFrame &mainFrame, GuiCar &car,
		const wxWindowID& id, const wxGLAttributes& attributes);
	CarRenderer(MainFrame &mainFrame, GuiCar &car,
		const wxWindowID& id, const int attributes[]);
	~CarRenderer();

	void UpdateDisplay(const KinematicOutputs &outputs);

	//void WriteImageFile(wxString pathAndFileName);

	// For accessing the helper orb
	void SetHelperOrbPosition(const Corner::Hardpoints &cornerPoint, const Corner::Location &location,
		const Suspension::Hardpoints &suspensionPoint);
	inline void DeactivateHelperOrb() { helperOrbIsActive = false; };

private:
	// For context menus
	MainFrame &mainFrame;

	GuiCar& car;

	void InternalInitialization();

	// Called from the CarRenderer constructor only in order to initialize the display
	void CreateActors();

	// The methods that perform the updating
	void UpdateCarDisplay();
	void UpdateKinematicsDisplay(KinematicOutputs oOutputs);

	// Pointers to the car objects that we are rendering
	AppearanceOptions &appearanceOptions;
	Car &displayCar;
	const Car &referenceCar;// Required for correct representation of the tires - see UpdateDisplay()

	// Event handlers ---------------
	void OnLeftClick(wxMouseEvent& event);
	void OnRightClick(wxMouseEvent& event);
	void OnContextEdit(wxCommandEvent& event);
	// End event handlers -----------

	// The actors that we use to represent the car
	// The origin marker and ground plane
	Origin *origin;
	Plane3D *groundPlane;

	// Right front corner
	AArm *rightFrontLowerAArm;
	AArm *rightFrontUpperAArm;
	Link *rightFrontTieRod;
	Link *rightFrontPushrod;
	Tire3D *rightFrontTire;
	Damper3D *rightFrontDamper;
	Spring3D *rightFrontSpring;
	Triangle3D *rightFrontUpright;
	Triangle3D *rightFrontBellCrank;
	Link *rightFrontBarLink;
	Link *rightFrontHalfShaft;

	// Left front corner
	AArm *leftFrontLowerAArm;
	AArm *leftFrontUpperAArm;
	Link *leftFrontTieRod;
	Link *leftFrontPushrod;
	Tire3D *leftFrontTire;
	Damper3D *leftFrontDamper;
	Spring3D *leftFrontSpring;
	Triangle3D *leftFrontUpright;
	Triangle3D *leftFrontBellCrank;
	Link *leftFrontBarLink;
	Link *leftFrontHalfShaft;

	// Right rear corner
	AArm *rightRearLowerAArm;
	AArm *rightRearUpperAArm;
	Link *rightRearTieRod;
	Link *rightRearPushrod;
	Tire3D *rightRearTire;
	Damper3D *rightRearDamper;
	Spring3D *rightRearSpring;
	Triangle3D *rightRearUpright;
	Triangle3D *rightRearBellCrank;
	Link *rightRearBarLink;
	Link *rightRearHalfShaft;

	// Left rear corner
	AArm *leftRearLowerAArm;
	AArm *leftRearUpperAArm;
	Link *leftRearTieRod;
	Link *leftRearPushrod;
	Tire3D *leftRearTire;
	Damper3D *leftRearDamper;
	Spring3D *leftRearSpring;
	Triangle3D *leftRearUpright;
	Triangle3D *leftRearBellCrank;
	Link *leftRearBarLink;
	Link *leftRearHalfShaft;

	// Front end
	Link *steeringRack;
	Swaybar3D *frontSwayBar;
	Spring3D *frontThirdSpring;
	Damper3D *frontThirdDamper;

	// Rear end
	Swaybar3D *rearSwayBar;
	Spring3D *rearThirdSpring;
	Damper3D *rearThirdDamper;

	// Kinematic output visualization
	Point3D *frontRollCenter;
	Point3D *rearRollCenter;
	Point3D *rightPitchCenter;
	Point3D *leftPitchCenter;
	Point3D *rightFrontInstantCenter;
	Point3D *leftFrontInstantCenter;
	Point3D *rightRearInstantCenter;
	Point3D *leftRearInstantCenter;

	Vector3D *frontRollAxis;
	Vector3D *rearRollAxis;
	Vector3D *rightPitchAxis;
	Vector3D *leftPitchAxis;
	Vector3D *rightFrontInstantAxis;
	Vector3D *leftFrontInstantAxis;
	Vector3D *rightRearInstantAxis;
	Vector3D *leftRearInstantAxis;

	// Helper orb
	Corner::Hardpoints helperOrbCornerPoint;
	Corner::Location helperOrbLocation;
	Suspension::Hardpoints helperOrbSuspensionPoint;
	bool helperOrbIsActive;
	Point3D *helperOrb;
	Point3D *helperOrbOpposite;

	bool TraceClickToHardpoint(const double& x, const double& y,
		Suspension::Hardpoints& suspensionPoint,
		Corner::Hardpoints& leftFrontPoint, Corner::Hardpoints& rightFrontPoint,
		Corner::Hardpoints& leftRearPoint, Corner::Hardpoints& rightRearPoint) const;
	bool GetLineUnderPoint(const double& x, const double& y,
		Vector& point, Vector& direction) const;
	std::vector<const Primitive*> IntersectWithPrimitive(const Vector& point,
		const Vector& direction) const;

	const Primitive* GetClosestPrimitive(const std::vector<const Primitive*>& intersected) const;
	void GetSelectedHardpoint(const Vector& point, const Vector& direction, const Primitive* selected,
		Suspension::Hardpoints& suspensionPoint, Corner::Hardpoints& leftFrontPoint,
		Corner::Hardpoints& rightFrontPoint, Corner::Hardpoints& leftRearPoint,
		Corner::Hardpoints& rightRearPoint) const;

	wxMenu* BuildContextMenu() const;
	void DoEditPointDialog();
	
	Suspension::Hardpoints suspensionPoint;
	Corner::Hardpoints leftFrontPoint;
	Corner::Hardpoints rightFrontPoint;
	Corner::Hardpoints leftRearPoint;
	Corner::Hardpoints rightRearPoint;

	enum EventIds
	{
		idContextEdit = wxID_HIGHEST + 1400
	};

	DECLARE_EVENT_TABLE()
};

#endif// CAR_RENDERER_H_