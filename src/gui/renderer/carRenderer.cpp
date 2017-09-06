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

// GLEW headers (needs to be first)
#include <GL/glew.h>

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

#undef None

// LibPlot2D headers
#include <lp2d/renderer/primitives/primitive.h>

namespace VVASE
{

//=============================================================================
// Class:			CarRenderer
// Function:		mDefaultVertexShader
//
// Description:		Default vertex shader.
//
// Input Arguments:
//		0	= position
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
const std::string CarRenderer::mSimpleGeometryShader(
	"#version 300 es\n"
	"#extension GL_EXT_geometry_shader : enable\n"
	"\n"
	"uniform mat4 projectionMatrix;\n"
	"\n"
	"layout (triangles) in;\n"
	"layout (triangle_strip, max_vertices = 3) out;\n"
	"\n"
	"in highp vec4 vertexColor[];\n"
	"\n"
	"out fragmentData\n"
	"{\n"
	"    highp vec4 color;\n"
	"    highp vec3 normal;\n"
	"    highp vec3 position;\n"
	"} f;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    gl_Position = gl_in[0].gl_Position;\n"
	"    vec3 vector1 = gl_in[1].gl_Position.xyz - gl_Position.xyz;\n"
	"    vec3 vector2 = gl_in[2].gl_Position.xyz - gl_Position.xyz;\n"
	"    vec3 localNormal = transpose(inverse(mat3(projectionMatrix))) * normalize(cross(vector2, vector1));\n"// TODO:  Instead of projection matrix, use "normalMatrix" which can be inverted once for all geometry on CPU side
	"\n"
	"    for (int i = 0; i < 3; i++)\n"
	"    {\n"
	"        gl_Position = gl_in[i].gl_Position;\n"
	"        f.color = vertexColor[i];\n"
	"        f.normal = localNormal;\n"
	"        f.position = vec3(projectionMatrix * gl_Position);\n"
	"        EmitVertex();\n"
	"    }\n"
	"\n"
	"    EndPrimitive();\n"
	"}\n"
);

//=============================================================================
// Class:			CarRenderer
// Function:		mFragmentShaderWithLighting
//
// Description:		Fragment shader including lighting model.
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
//=============================================================================
const std::string CarRenderer::mFragmentShaderWithLighting(
	"#version 300 es\n"
	"\n"
	"struct Light\n"
	"{\n"
	"    highp vec3 position;\n"
	"    highp vec3 color;\n"
	"};\n"
	"#define LIGHT_COUNT 2\n"
	"uniform Light light;\n//[LIGHT_COUNT];\n"
	"uniform highp vec3 cameraPosition;\n"// Line 10
	"\n"
	"in fragmentData\n"
	"{\n"
	"    highp vec4 color;\n"
	"    highp vec3 normal;\n"
	"    highp vec3 position;\n"
	"} f;\n"
	"\n"
	"out highp vec4 outputColor;\n"
	"\n"// Line 20
	"highp float Clamp(highp float v, highp float low, highp float high)\n"
	"{\n"
	"    if (v > high)\n"
	"        return high;\n"
	"    else if (v < low)\n"
	"        return low;\n"
	"    return v;\n"
	"}\n"
	"\n"
	"void main()\n"// Line 30
	"{\n"
	"    highp vec3 lightDirection = normalize(light.position - f.position);\n"
	"    highp float diffuseCoefficient = dot(f.normal, lightDirection);\n"
	"    diffuseCoefficient = Clamp(diffuseCoefficient, 0.0, 1.0);\n"
	"    highp vec3 diffuse = vec3(f.color) * diffuseCoefficient * light.color;\n"
	"\n"
	"    highp float ambientCoefficient = 0.6;\n"// TODO:  Can constants be optimized?
	"    highp vec3 ambient = vec3(f.color) * ambientCoefficient * light.color;\n"
	"    outputColor = vec4(diffuse + ambient, f.color.a);\n"
	"}\n"
);

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
// Function:		AssignLightingUniforms
//
// Description:		Assigns uniforms required for lighting model.
//
// Input Arguments:
//		program	= const GLuint&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CarRenderer::AssignLightingUniforms(const GLuint& program)
{
	const GLuint light0Position(glGetUniformLocation(program, "light.position"));
	const GLuint light0Color(glGetUniformLocation(program, "light.color"));
	mCameraPositionLocation = glGetUniformLocation(program, "cameraPosition");

	glUseProgram(program);

	const float position0[3] = { 0.0f, 0.0f, 100.0f };
	glUniform3fv(light0Position, 1, position0);

	const float color0[3] = { 1.0f, 1.0f, 1.0f };
	glUniform3fv(light0Color, 1, color0);

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

//=============================================================================
// Class:			CarRenderer
// Function:		AssignDefaultUniforms
//
// Description:		Assigns uniform locations and/or values for default program.
//
// Input Arguments:
//		shader	= ShaderInfo&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void CarRenderer::AssignDefaultUniforms(ShaderInfo& shader)
{
	RenderWindow::AssignDefaultUniforms(shader);

	AssignLightingUniforms(shader.programId);
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
	helperOrbCornerPoint = Corner::Hardpoints::Count;
	helperOrbLocation = Corner::Location::RightFront;
	helperOrbSuspensionPoint = Suspension::Hardpoints::Count;
	helperOrbIsActive = false;

#ifdef USE_DEBUG_SHAPE
	DebugShape::SetRenderer(*this);
#endif
}

//==========================================================================
// Class:			CarRenderer
// Function:		UpdateSpecialUniforms
//
// Description:		Updates camera position uniform.
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
void CarRenderer::UpdateSpecialUniforms()
{
	const auto cameraPosition(GetCameraPosition());
	const float cameraPositionFloat[3] = { static_cast<float>(cameraPosition.x()), static_cast<float>(cameraPosition.y()), static_cast<float>(cameraPosition.z()) };

	glUseProgram(mShaders.front().programId);
	glUniform3fv(mCameraPositionLocation, 1, cameraPositionFloat);
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
	EVT_MENU(idContextEdit,		CarRenderer::OnContextEdit)
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
	double rightMostPoint = std::max(referenceSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].y() +
		referenceTires->rightFront->width / 2.0,
		referenceSuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].y() +
		referenceTires->rightRear->width / 2.0);
	double leftMostPoint = std::min(referenceSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].y() -
		referenceTires->leftFront->width / 2.0,
		referenceSuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].y() -
		referenceTires->leftRear->width / 2.0);
	double frontMostPoint = std::min(referenceSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].x() -
		referenceTires->rightFront->diameter / 2.0,
		referenceSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].x() -
		referenceTires->leftFront->diameter / 2.0);
	double rearMostPoint = std::max(referenceSuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].x() +
		referenceTires->rightRear->diameter / 2.0,
		referenceSuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)].x() +
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
	if (displaySuspension->frontBarStyle == Suspension::BarStyle::None ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySwayBar))
		ShowBarLinks = false;

	// Right front corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	bool ShowBellCranksPushrods = true;
	if (displaySuspension->rightFront.actuationType == Corner::ActuationType::OutboardRockerArm ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	targetNormal = VVASE::Math::GetPlaneNormal(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
							displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
							displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)]);
	originalNormal = VVASE::Math::GetPlaneNormal(referenceSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
							referenceSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
							referenceSuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)]);

	// We also need to account for toe and camber settings for the targetNormal - apply camber first
	// NOTE:  This corner is on the right side of the car - we flip the sign on the camber and toe angles
	GeometryMath::Rotate(targetNormal, -displaySuspension->rightFront.staticCamber, Eigen::Vector3d::UnitX());
	GeometryMath::Rotate(targetNormal, -displaySuspension->rightFront.staticToe, Eigen::Vector3d::UnitZ());

	// Now continue with the update for this corner
	rightFrontLowerAArm->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::LowerFrontTubMount)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::LowerRearTubMount)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	rightFrontUpperAArm->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::UpperFrontTubMount)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::UpperRearTubMount)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	rightFrontPushrod->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardPushrod)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	rightFrontTieRod->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	rightFrontTire->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::WheelCenter)], originalNormal, targetNormal,
		displayTires->rightFront->width,
		appearanceOptions.GetSize(AppearanceOptions::SizeTireInsideDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionTire),
		appearanceOptions.GetColor(AppearanceOptions::ColorTire),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTire));
	rightFrontDamper->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardDamper)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardDamper)],
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperBody),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityDamper));
	rightFrontSpring->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardSpring)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardSpring)],
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringEndPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorSpring),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySpring));
	rightFrontUpright->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)],
		appearanceOptions.GetColor(AppearanceOptions::ColorUpright),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityUpright));
	rightFrontBellCrank->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardDamper)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)],
		VVASE::Math::NearestPointOnAxis(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot1)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot2)] -
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot1)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)]),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	rightFrontBarLink->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarLinkDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		ShowBarLinks);
	rightFrontHalfShaft->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardHalfShaft)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardHalfShaft)],
		appearanceOptions.GetSize(AppearanceOptions::SizeHalfShaftDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHalfShaft),
		appearanceOptions.GetColor(AppearanceOptions::ColorHalfShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHalfShaft) && referenceCar.HasFrontHalfShafts());

	// left front corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (displaySuspension->leftFront.actuationType == Corner::ActuationType::OutboardRockerArm ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	targetNormal = VVASE::Math::GetPlaneNormal(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
							displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
							displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)]);
	originalNormal = VVASE::Math::GetPlaneNormal(referenceSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
							referenceSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
							referenceSuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)]);

	// We also need to account for toe and camber settings for the targetNormal - apply camber first
	GeometryMath::Rotate(targetNormal, displaySuspension->leftFront.staticCamber, Eigen::Vector3d::UnitX());
	GeometryMath::Rotate(targetNormal, displaySuspension->leftFront.staticToe, Eigen::Vector3d::UnitZ());

	// Now continue with the update for this corner
	leftFrontLowerAArm->Update(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::LowerFrontTubMount)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::LowerRearTubMount)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	leftFrontUpperAArm->Update(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::UpperFrontTubMount)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::UpperRearTubMount)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	leftFrontPushrod->Update(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardPushrod)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	leftFrontTieRod->Update(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	leftFrontTire->Update(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::WheelCenter)], originalNormal, targetNormal,
		displayTires->leftFront->width,
		appearanceOptions.GetSize(AppearanceOptions::SizeTireInsideDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionTire),
		appearanceOptions.GetColor(AppearanceOptions::ColorTire),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTire));
	leftFrontDamper->Update(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardDamper)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardDamper)],
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperBody),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityDamper));
	leftFrontSpring->Update(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardSpring)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardSpring)],
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringEndPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorSpring),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySpring));
	leftFrontUpright->Update(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)],
		appearanceOptions.GetColor(AppearanceOptions::ColorUpright),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityUpright));
	leftFrontBellCrank->Update(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardDamper)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)],
		VVASE::Math::NearestPointOnAxis(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot1)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot2)] -
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot1)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)]),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	leftFrontBarLink->Update(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarLinkDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		ShowBarLinks);
	leftFrontHalfShaft->Update(displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardHalfShaft)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardHalfShaft)],
		appearanceOptions.GetSize(AppearanceOptions::SizeHalfShaftDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHalfShaft),
		appearanceOptions.GetColor(AppearanceOptions::ColorHalfShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHalfShaft) && referenceCar.HasFrontHalfShafts());

	// Determine whether or not the rear sway bar links should be visible
	if (displaySuspension->rearBarStyle == Suspension::BarStyle::None ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySwayBar))
		ShowBarLinks = false;
	else
		ShowBarLinks = true;

	// right rear corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (displaySuspension->rightRear.actuationType == Corner::ActuationType::OutboardRockerArm ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	targetNormal = VVASE::Math::GetPlaneNormal(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
							displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
							displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)]);
	originalNormal = VVASE::Math::GetPlaneNormal(referenceSuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
							referenceSuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
							referenceSuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)]);

	// We also need to account for toe and camber settings for the targetNormal - apply camber first
	// NOTE:  This corner is on the right side of the car - we flip the sign on the camber and toe angles
	GeometryMath::Rotate(targetNormal, -displaySuspension->rightRear.staticCamber, Eigen::Vector3d::UnitX());
	GeometryMath::Rotate(targetNormal, -displaySuspension->rightRear.staticToe, Eigen::Vector3d::UnitZ());

	// Now continue with the update for this corner
	rightRearLowerAArm->Update(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::LowerFrontTubMount)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::LowerRearTubMount)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	rightRearUpperAArm->Update(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::UpperFrontTubMount)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::UpperRearTubMount)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	rightRearPushrod->Update(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardPushrod)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	rightRearTieRod->Update(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	rightRearTire->Update(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::WheelCenter)], originalNormal, targetNormal,
		displayTires->rightRear->width,
		appearanceOptions.GetSize(AppearanceOptions::SizeTireInsideDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionTire),
		appearanceOptions.GetColor(AppearanceOptions::ColorTire),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTire));
	rightRearDamper->Update(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardDamper)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardDamper)],
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperBody),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityDamper));
	rightRearSpring->Update(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardSpring)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardSpring)],
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringEndPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorSpring),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySpring));
	rightRearUpright->Update(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)],
		appearanceOptions.GetColor(AppearanceOptions::ColorUpright),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityUpright));
	rightRearBellCrank->Update(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardDamper)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)],
		VVASE::Math::NearestPointOnAxis(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot1)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot2)] -
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot1)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)]),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	rightRearBarLink->Update(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarLinkDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		ShowBarLinks);
	rightRearHalfShaft->Update(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardHalfShaft)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardHalfShaft)],
		appearanceOptions.GetSize(AppearanceOptions::SizeHalfShaftDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHalfShaft),
		appearanceOptions.GetColor(AppearanceOptions::ColorHalfShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHalfShaft) && referenceCar.HasRearHalfShafts());

	// left rear corner
	// Determine whether or not the bellcranks and pushrods should be drawn for this corner
	ShowBellCranksPushrods = true;
	if (displaySuspension->leftRear.actuationType == Corner::ActuationType::OutboardRockerArm ||
		!appearanceOptions.GetVisibility(AppearanceOptions::VisibilityPushrod))
		ShowBellCranksPushrods = false;

	// First, calculate the normal vectors for the original and new upright planes
	targetNormal = VVASE::Math::GetPlaneNormal(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
							displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
							displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)]);
	originalNormal = VVASE::Math::GetPlaneNormal(referenceSuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
							referenceSuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
							referenceSuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)]);

	// We also need to account for toe and camber settings for the targetNormal - apply camber first
	GeometryMath::Rotate(targetNormal, displaySuspension->leftRear.staticCamber, Eigen::Vector3d::UnitX());
	GeometryMath::Rotate(targetNormal, displaySuspension->leftRear.staticToe, Eigen::Vector3d::UnitZ());

	// Now continue with the update for this corner
	leftRearLowerAArm->Update(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::LowerFrontTubMount)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::LowerRearTubMount)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	leftRearUpperAArm->Update(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::UpperFrontTubMount)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::UpperRearTubMount)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorAArm),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityAArm));
	leftRearPushrod->Update(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardPushrod)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	leftRearTieRod->Update(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	leftRearTire->Update(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::ContactPatch)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::WheelCenter)], originalNormal, targetNormal,
		displayTires->leftRear->width,
		appearanceOptions.GetSize(AppearanceOptions::SizeTireInsideDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionTire),
		appearanceOptions.GetColor(AppearanceOptions::ColorTire),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTire));
	leftRearDamper->Update(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardDamper)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardDamper)],
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperShaftDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeDamperBodyLength),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperBody),
		appearanceOptions.GetColor(AppearanceOptions::ColorDamperShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityDamper));
	leftRearSpring->Update(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardSpring)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardSpring)],
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringDiameter),
		appearanceOptions.GetSize(AppearanceOptions::SizeSpringEndPointDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSpringDamper),
		appearanceOptions.GetColor(AppearanceOptions::ColorSpring),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySpring));
	leftRearUpright->Update(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::LowerBallJoint)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::UpperBallJoint)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardTieRod)],
		appearanceOptions.GetColor(AppearanceOptions::ColorUpright),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityUpright));
	leftRearBellCrank->Update(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardDamper)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)],
		VVASE::Math::NearestPointOnAxis(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot1)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot2)] -
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::BellCrankPivot1)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardPushrod)]),
		appearanceOptions.GetColor(AppearanceOptions::ColorPushrod),
		ShowBellCranksPushrods);
	leftRearBarLink->Update(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardBarLink)],
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarLinkDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		ShowBarLinks);
	leftRearHalfShaft->Update(displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardHalfShaft)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::OutboardHalfShaft)],
		appearanceOptions.GetSize(AppearanceOptions::SizeHalfShaftDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHalfShaft),
		appearanceOptions.GetColor(AppearanceOptions::ColorHalfShaft),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHalfShaft) && referenceCar.HasRearHalfShafts());

	// Front end
	steeringRack->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardTieRod)],
		appearanceOptions.GetSize(AppearanceOptions::SizeAArmDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionAArm),
		appearanceOptions.GetColor(AppearanceOptions::ColorTieRod),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityTieRod));
	frontSwayBar->Update(displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
		displaySuspension->rightFront.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)],
		displaySuspension->leftFront.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)],
		displaySuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontBarMidPoint)],
		displaySuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::FrontBarPivotAxis)],
		displaySuspension->frontBarStyle,
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySwayBar));

	// Rear end
	rearSwayBar->Update(displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::InboardBarLink)],
		displaySuspension->rightRear.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)],
		displaySuspension->leftRear.hardpoints[static_cast<int>(Corner::Hardpoints::BarArmAtPivot)],
		displaySuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearBarMidPoint)],
		displaySuspension->hardpoints[static_cast<int>(Suspension::Hardpoints::RearBarPivotAxis)],
		displaySuspension->rearBarStyle,
		appearanceOptions.GetSize(AppearanceOptions::SizeSwayBarDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionSwayBar),
		appearanceOptions.GetColor(AppearanceOptions::ColorSwayBar),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilitySwayBar));

	// Update the helper orb
	// Determine which of the location variables is valid
	Eigen::Vector3d helperOrbPosition(0.0, 0.0, 0.0);
	Eigen::Vector3d helperOrbOppositePosition(0.0, 0.0, 0.0);
	if (helperOrbCornerPoint != Corner::Hardpoints::Count)
	{
		if (helperOrbLocation == Corner::Location::RightFront)
		{
			helperOrbPosition = displaySuspension->rightFront.hardpoints[static_cast<int>(helperOrbCornerPoint)];
			helperOrbOppositePosition = displaySuspension->leftFront.hardpoints[static_cast<int>(helperOrbCornerPoint)];
		}
		else if (helperOrbLocation == Corner::Location::LeftFront)
		{
			helperOrbPosition = displaySuspension->leftFront.hardpoints[static_cast<int>(helperOrbCornerPoint)];
			helperOrbOppositePosition = displaySuspension->rightFront.hardpoints[static_cast<int>(helperOrbCornerPoint)];
		}
		else if (helperOrbLocation == Corner::Location::RightRear)
		{
			helperOrbPosition = displaySuspension->rightRear.hardpoints[static_cast<int>(helperOrbCornerPoint)];
			helperOrbOppositePosition = displaySuspension->leftRear.hardpoints[static_cast<int>(helperOrbCornerPoint)];
		}
		else
		{
			helperOrbPosition = displaySuspension->leftRear.hardpoints[static_cast<int>(helperOrbCornerPoint)];
			helperOrbOppositePosition = displaySuspension->rightRear.hardpoints[static_cast<int>(helperOrbCornerPoint)];
		}
	}
	else if (helperOrbSuspensionPoint != Suspension::Hardpoints::Count)
		helperOrbPosition = displaySuspension->hardpoints[static_cast<int>(helperOrbSuspensionPoint)];

	helperOrb->Update(helperOrbPosition, appearanceOptions.GetSize(AppearanceOptions::SizeHelperOrbDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHelperOrb),
		appearanceOptions.GetColor(AppearanceOptions::ColorHelperOrb),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHelperOrb) && helperOrbIsActive);
	helperOrbOpposite->Update(helperOrbOppositePosition, appearanceOptions.GetSize(AppearanceOptions::SizeHelperOrbDiameter),
		appearanceOptions.GetResolution(AppearanceOptions::ResolutionHelperOrb),
		appearanceOptions.GetColor(AppearanceOptions::ColorHelperOrb),
		appearanceOptions.GetVisibility(AppearanceOptions::VisibilityHelperOrb) && helperOrbIsActive
		&& displaySuspension->isSymmetric && helperOrbCornerPoint != Corner::Hardpoints::Count);
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
	if (mIsInteracting || !mObservedLeftButtonDown)
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
		Corner::NumberOfHardpoints, Corner::Location::RightFront, (Suspension::Hardpoints)(event.GetRow() - 1));*/
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
	suspensionPoint = Suspension::Hardpoints::Count;
	leftFrontPoint = Corner::Hardpoints::Count;
	rightFrontPoint = Corner::Hardpoints::Count;
	leftRearPoint = Corner::Hardpoints::Count;
	rightRearPoint = Corner::Hardpoints::Count;

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
	for (i = 0; i < static_cast<unsigned int>(Corner::Hardpoints::Count); i++)
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

	for (i = 0; i < static_cast<unsigned int>(Suspension::Hardpoints::Count); i++)
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

	if (suspensionPoint != Suspension::Hardpoints::Count)
	{
		pointToEdit = &originalSuspension->hardpoints[static_cast<int>(suspensionPoint)];
		pointName = Suspension::GetHardpointName(suspensionPoint);
	}
	else if (leftFrontPoint != Corner::Hardpoints::Count)
	{
		if (originalSuspension->isSymmetric)
			pointToEdit = &originalSuspension->rightFront.hardpoints[static_cast<int>(leftFrontPoint)];
		else
			pointToEdit = &originalSuspension->leftFront.hardpoints[static_cast<int>(leftFrontPoint)];

		pointName = Corner::GetLocationName(Corner::Location::LeftFront)
			+ _T(" ") + Corner::GetHardpointName(leftFrontPoint);
	}
	else if (rightFrontPoint != Corner::Hardpoints::Count)
	{
		pointToEdit = &originalSuspension->rightFront.hardpoints[static_cast<int>(rightFrontPoint)];
		pointName = Corner::GetLocationName(Corner::Location::RightFront)
			+ _T(" ") + Corner::GetHardpointName(rightFrontPoint);
	}
	else if (leftRearPoint != Corner::Hardpoints::Count)
	{
		if (originalSuspension->isSymmetric)
			pointToEdit = &originalSuspension->rightRear.hardpoints[static_cast<int>(leftRearPoint)];
		else
			pointToEdit = &originalSuspension->leftRear.hardpoints[static_cast<int>(leftRearPoint)];

		pointName = Corner::GetLocationName(Corner::Location::LeftRear)
			+ _T(" ") + Corner::GetHardpointName(leftRearPoint);
	}
	else if (rightRearPoint != Corner::Hardpoints::Count)
	{
		pointToEdit = &originalSuspension->rightRear.hardpoints[static_cast<int>(rightRearPoint)];
		pointName = Corner::GetLocationName(Corner::Location::RightRear)
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
