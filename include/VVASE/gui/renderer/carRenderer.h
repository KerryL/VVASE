/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  carRenderer.h
// Date:  4/5/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the CarRenderer class.  Derived from
//        RenderWindow, this class is associated with a loaded car file
//        and contains the information and methods required to render a car in 3D.

#ifndef CAR_RENDERER_H_
#define CAR_RENDERER_H_

// Local headers
#include "VVASE/core/car/subsystems/suspension.h"
#include "VVASE/gui/guiObject.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

// wxWidgets forward declarations
class wxString;

namespace VVASE
{

// Local forward declarations
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

class CarRenderer : public LibPlot2D::RenderWindow
{
public:
	CarRenderer(MainFrame &mainFrame, GuiCar &car,
		const wxWindowID& id, const wxGLAttributes& attributes);

	void UpdateDisplay(const KinematicOutputs &outputs);

	//void WriteImageFile(wxString pathAndFileName);

	// For accessing the helper orb
	void SetHelperOrbPosition(const Corner::Hardpoints &cornerPoint, const Corner::Location &location,
		const Suspension::Hardpoints &suspensionPoint);
	inline void DeactivateHelperOrb() { helperOrbIsActive = false; };

private:
	// For context menus
	MainFrame& mainFrame;

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
	std::unique_ptr<Origin> origin;
	std::unique_ptr<Plane3D> groundPlane;

	// Right front corner
	std::unique_ptr<AArm> rightFrontLowerAArm;
	std::unique_ptr<AArm> rightFrontUpperAArm;
	std::unique_ptr<Link> rightFrontTieRod;
	std::unique_ptr<Link> rightFrontPushrod;
	std::unique_ptr<Tire3D> rightFrontTire;
	std::unique_ptr<Damper3D> rightFrontDamper;
	std::unique_ptr<Spring3D> rightFrontSpring;
	std::unique_ptr<Triangle3D> rightFrontUpright;
	std::unique_ptr<Triangle3D> rightFrontBellCrank;
	std::unique_ptr<Link> rightFrontBarLink;
	std::unique_ptr<Link> rightFrontHalfShaft;

	// Left front corner
	std::unique_ptr<AArm> leftFrontLowerAArm;
	std::unique_ptr<AArm> leftFrontUpperAArm;
	std::unique_ptr<Link> leftFrontTieRod;
	std::unique_ptr<Link> leftFrontPushrod;
	std::unique_ptr<Tire3D> leftFrontTire;
	std::unique_ptr<Damper3D> leftFrontDamper;
	std::unique_ptr<Spring3D> leftFrontSpring;
	std::unique_ptr<Triangle3D> leftFrontUpright;
	std::unique_ptr<Triangle3D> leftFrontBellCrank;
	std::unique_ptr<Link> leftFrontBarLink;
	std::unique_ptr<Link> leftFrontHalfShaft;

	// Right rear corner
	std::unique_ptr<AArm> rightRearLowerAArm;
	std::unique_ptr<AArm> rightRearUpperAArm;
	std::unique_ptr<Link> rightRearTieRod;
	std::unique_ptr<Link> rightRearPushrod;
	std::unique_ptr<Tire3D> rightRearTire;
	std::unique_ptr<Damper3D> rightRearDamper;
	std::unique_ptr<Spring3D> rightRearSpring;
	std::unique_ptr<Triangle3D> rightRearUpright;
	std::unique_ptr<Triangle3D> rightRearBellCrank;
	std::unique_ptr<Link> rightRearBarLink;
	std::unique_ptr<Link> rightRearHalfShaft;

	// Left rear corner
	std::unique_ptr<AArm> leftRearLowerAArm;
	std::unique_ptr<AArm> leftRearUpperAArm;
	std::unique_ptr<Link> leftRearTieRod;
	std::unique_ptr<Link> leftRearPushrod;
	std::unique_ptr<Tire3D> leftRearTire;
	std::unique_ptr<Damper3D> leftRearDamper;
	std::unique_ptr<Spring3D> leftRearSpring;
	std::unique_ptr<Triangle3D> leftRearUpright;
	std::unique_ptr<Triangle3D> leftRearBellCrank;
	std::unique_ptr<Link> leftRearBarLink;
	std::unique_ptr<Link> leftRearHalfShaft;

	// Front end
	std::unique_ptr<Link> steeringRack;
	std::unique_ptr<Swaybar3D> frontSwayBar;
	std::unique_ptr<Spring3D> frontThirdSpring;
	std::unique_ptr<Damper3D> frontThirdDamper;

	// Rear end
	std::unique_ptr<Swaybar3D> rearSwayBar;
	std::unique_ptr<Spring3D> rearThirdSpring;
	std::unique_ptr<Damper3D> rearThirdDamper;

	// Kinematic output visualization
	std::unique_ptr<Point3D> frontRollCenter;
	std::unique_ptr<Point3D> rearRollCenter;
	std::unique_ptr<Point3D> rightPitchCenter;
	std::unique_ptr<Point3D> leftPitchCenter;
	std::unique_ptr<Point3D> rightFrontInstantCenter;
	std::unique_ptr<Point3D> leftFrontInstantCenter;
	std::unique_ptr<Point3D> rightRearInstantCenter;
	std::unique_ptr<Point3D> leftRearInstantCenter;

	std::unique_ptr<Vector3D> frontRollAxis;
	std::unique_ptr<Vector3D> rearRollAxis;
	std::unique_ptr<Vector3D> rightPitchAxis;
	std::unique_ptr<Vector3D> leftPitchAxis;
	std::unique_ptr<Vector3D> rightFrontInstantAxis;
	std::unique_ptr<Vector3D> leftFrontInstantAxis;
	std::unique_ptr<Vector3D> rightRearInstantAxis;
	std::unique_ptr<Vector3D> leftRearInstantAxis;

	// Helper orb
	Corner::Hardpoints helperOrbCornerPoint;
	Corner::Location helperOrbLocation;
	Suspension::Hardpoints helperOrbSuspensionPoint;
	bool helperOrbIsActive;
	std::unique_ptr<Point3D> helperOrb;
	std::unique_ptr<Point3D> helperOrbOpposite;

	bool TraceClickToHardpoint(const double& x, const double& y,
		Suspension::Hardpoints& suspensionPoint,
		Corner::Hardpoints& leftFrontPoint, Corner::Hardpoints& rightFrontPoint,
		Corner::Hardpoints& leftRearPoint, Corner::Hardpoints& rightRearPoint) const;
	bool GetLineUnderPoint(const double& x, const double& y,
		Eigen::Vector3d& point, Eigen::Vector3d& direction) const;
	std::vector<const LibPlot2D::Primitive*> IntersectWithPrimitive(const Eigen::Vector3d& point,
		const Eigen::Vector3d& direction) const;

	const LibPlot2D::Primitive* GetClosestPrimitive(const std::vector<const LibPlot2D::Primitive*>& intersected) const;
	void GetSelectedHardpoint(const Eigen::Vector3d& point, const Eigen::Vector3d& direction, const LibPlot2D::Primitive* selected,
		Suspension::Hardpoints& suspensionPoint, Corner::Hardpoints& leftFrontPoint,
		Corner::Hardpoints& rightFrontPoint, Corner::Hardpoints& leftRearPoint,
		Corner::Hardpoints& rightRearPoint) const;

	bool Unproject(const double& x, const double& y, const double& z, Eigen::Vector3d& point) const;

	wxMenu* BuildContextMenu() const;
	void DoEditPointDialog();

	Suspension::Hardpoints suspensionPoint;
	Corner::Hardpoints leftFrontPoint;
	Corner::Hardpoints rightFrontPoint;
	Corner::Hardpoints leftRearPoint;
	Corner::Hardpoints rightRearPoint;

	static const std::string mSimpleGeometryShader;

	//bool HasGeometryShader() const override { return true; }

	std::string GetDefaultGeometryShader() const override
	{ return mSimpleGeometryShader; }

	enum EventIds
	{
		idContextEdit = wxID_HIGHEST + 1400
	};

	DECLARE_EVENT_TABLE()
};

}// namespace VVASE

#endif// CAR_RENDERER_H_
