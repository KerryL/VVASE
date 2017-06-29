/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  quasiStatic.cpp
// Date:  5/27/2016
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Quasi-static analysis object.  Calculates vehicle attitude when
//        subjected to external accelerations.

// Standard C++ headers
#include <cassert>

// Local headers
#include "vSolver/physics/quasiStatic.h"
#include "vSolver/physics/kinematics.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vSolver/physics/quasiStaticOutputs.h"
#include "vCar/car.h"
#include "vCar/massProperties.h"
#include "vCar/tireSet.h"
#include "vCar/tire.h"
#include "vMath/matrix.h"
#include "vMath/vector.h"
#include "vUtilities/debugger.h"

namespace VVASE
{

//==========================================================================
// Class:			QuasiStatic
// Function:		QuasiStatic
//
// Description:		Constructor for QuasiStatic class.
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
QuasiStatic::QuasiStatic()
{
}

//==========================================================================
// Class:			QuasiStatic
// Function:		Solve
//
// Description:		Main solver method for quasi-static simulations.  Uses
//					the Newton-Raphson method to converge on the kinematic
//					state that results in equillibrium.
//
// Input Arguments:
//		originalCar			= const Car*
//		workingCar			= const Car*
//		kinematicsInputs	= const Kinematics::Inputs&
//		inputs				= const Inputs&
//
// Output Arguments:
//		outputs				= QuasiStaticOutputs&
//
// Return Value:
//		Kinematics::Inputs
//
//==========================================================================
Kinematics::Inputs QuasiStatic::Solve(const Car* originalCar, Car* workingCar,
	const Kinematics::Inputs& kinematicsInputs, const Inputs& inputs,
	QuasiStaticOutputs& outputs) const
{
	Kinematics kinematics;
	kinematics.SetCenterOfRotation(kinematicsInputs.centerOfRotation);
	kinematics.SetFirstEulerRotation(kinematicsInputs.firstRotation);
	kinematics.SetRackTravel(inputs.rackTravel);

	unsigned int i(0);
	const unsigned int limit(100);
	const double epsilon(1.0e-3);
	const double maxError(1.0e-8);
	Matrix error(13, 1), tempError;
	Matrix guess(3, 1);// parameteric variables representing remaining kinematic state inputs
	Matrix jacobian(error.GetNumberOfRows(), guess.GetNumberOfRows());
	Matrix delta;

	guess(0,0) = 0.0;
	guess(1,0) = 0.0;
	guess(2,0) = 0.0;

	error(0,0) = 2.0 * maxError;

	WheelSet preLoad(ComputePreLoad(originalCar));
	WheelSet wheelLoads(originalCar->massProperties->cornerWeights);// [lb]
	wheelLoads.leftFront *= 32.174;
	wheelLoads.rightFront *= 32.174;
	wheelLoads.leftRear *= 32.174;
	wheelLoads.rightRear *= 32.174;
	WheelSet tireDeflections(ComputeTireDeflections(*originalCar->tires, wheelLoads));

	// TODO:  Compute lateral and longitudinal forces at each corner
	//        Use them to evaluate jacking effects
	//
	// Add to the calculation:
	// - Components of lateral and longitudinal force that add to suspension spring forces
	// - Braking/drive forces (as well as torque effects - significance of onboard/offboard torques?) (do we need a bias ratio(s)?)
	// - Tractive tire force =
	//   if (braking)
	//      one variable, torques calculated according to f/r brake split (and bias ratio if inboard rear?), resolved to forces using effective radius
	//   else// traction
	//      0 at ends that don't have drive wheels
	//      one variable, torques calculated according to bias ratio, resolved to forces using effective radius
	// - Can we make this part function independent of tire model?  Then we implement our crumby tire model elsewhere, but allow it to be easily replaced?
	// What would a real tire model need?  Veloctiy?
	// - Use sum Fy = 0 = sum(F_lat * cos(steer) + F_lon * sin(steer)) - totalMass * g * Gy
	// - Similar for sum Fx
	// - Also need a sum Mz = 0?  To ensure steady-state conditions?
	// - Add ficticious term to Fz at each corner when computing spring force
	//   - This term accounts for effect of in-plane tire forces acting in direction of spring actuation
	//   - Fz_fake = F_lat * RC_z / RC_y + F_lon * PC_z / PC_x (remember to check for undefined RCs and PCs)
	// ** Looking at MMM plots, I believe that with a proper tire model we could (and must) solve for steering angle as a function of Gx and Gy.
	//    This is consistent with thoughts I am having about including a real tire model, which would require velocity and yaw velocity inputs.

	while (i < limit && (error.GetNorm() > maxError ||
		ComputeDeltaWheelSets(kinematics.GetTireDeflections(), tireDeflections) > maxError))
	{
		kinematics.SetRoll(guess(0,0));
		kinematics.SetPitch(guess(1,0));
		kinematics.SetHeave(guess(2,0));
		kinematics.SetTireDeflections(tireDeflections);

		kinematics.UpdateKinematics(originalCar, workingCar, wxString::Format("Quasi-Static, i = %u (error)", i));
		wheelLoads = ComputeWheelLoads(originalCar, kinematics.GetOutputs(), preLoad);
		tireDeflections = ComputeTireDeflections(*originalCar->tires, wheelLoads);
		error = ComputeError(workingCar, inputs.gx, inputs.gy, kinematics.GetOutputs(), preLoad);

		kinematics.SetRoll(guess(0,0) + epsilon);
		kinematics.UpdateKinematics(originalCar, workingCar, wxString::Format("Quasi-Static, i = %u (roll)", i));

		tempError = ComputeError(workingCar, inputs.gx, inputs.gy, kinematics.GetOutputs(), preLoad);
		jacobian(0,0) = (tempError(0,0) - error(0,0)) / epsilon;
		jacobian(1,0) = (tempError(1,0) - error(1,0)) / epsilon;
		jacobian(2,0) = (tempError(2,0) - error(2,0)) / epsilon;
		jacobian(3,0) = (tempError(3,0) - error(3,0)) / epsilon;
		jacobian(4,0) = (tempError(4,0) - error(4,0)) / epsilon;
		jacobian(5,0) = (tempError(5,0) - error(5,0)) / epsilon;
		jacobian(6,0) = (tempError(6,0) - error(6,0)) / epsilon;
		jacobian(7,0) = (tempError(7,0) - error(7,0)) / epsilon;
		jacobian(8,0) = (tempError(8,0) - error(8,0)) / epsilon;
		jacobian(9,0) = (tempError(9,0) - error(9,0)) / epsilon;
		jacobian(10,0) = (tempError(10,0) - error(10,0)) / epsilon;
		jacobian(11,0) = (tempError(11,0) - error(11,0)) / epsilon;
		jacobian(12,0) = (tempError(12,0) - error(12,0)) / epsilon;

		kinematics.SetRoll(guess(0,0));
		kinematics.SetPitch(guess(1,0) + epsilon);
		kinematics.UpdateKinematics(originalCar, workingCar, wxString::Format("Quasi-Static, i = %u (pitch)", i));

		tempError = ComputeError(workingCar, inputs.gx, inputs.gy, kinematics.GetOutputs(), preLoad);
		jacobian(0,1) = (tempError(0,0) - error(0,0)) / epsilon;
		jacobian(1,1) = (tempError(1,0) - error(1,0)) / epsilon;
		jacobian(2,1) = (tempError(2,0) - error(2,0)) / epsilon;
		jacobian(3,1) = (tempError(3,0) - error(3,0)) / epsilon;
		jacobian(4,1) = (tempError(4,0) - error(4,0)) / epsilon;
		jacobian(5,1) = (tempError(5,0) - error(5,0)) / epsilon;
		jacobian(6,1) = (tempError(6,0) - error(6,0)) / epsilon;
		jacobian(7,1) = (tempError(7,0) - error(7,0)) / epsilon;
		jacobian(8,1) = (tempError(8,0) - error(8,0)) / epsilon;
		jacobian(9,1) = (tempError(9,0) - error(9,0)) / epsilon;
		jacobian(10,1) = (tempError(10,0) - error(10,0)) / epsilon;
		jacobian(11,1) = (tempError(11,0) - error(11,0)) / epsilon;
		jacobian(12,1) = (tempError(12,0) - error(12,0)) / epsilon;

		kinematics.SetPitch(guess(1,0));
		kinematics.SetHeave(guess(2,0) + epsilon);
		kinematics.UpdateKinematics(originalCar, workingCar, wxString::Format("Quasi-Static, i = %u (heave)", i));

		tempError = ComputeError(workingCar, inputs.gx, inputs.gy, kinematics.GetOutputs(), preLoad);
		jacobian(0,2) = (tempError(0,0) - error(0,0)) / epsilon;
		jacobian(1,2) = (tempError(1,0) - error(1,0)) / epsilon;
		jacobian(2,2) = (tempError(2,0) - error(2,0)) / epsilon;
		jacobian(3,2) = (tempError(3,0) - error(3,0)) / epsilon;
		jacobian(4,2) = (tempError(4,0) - error(4,0)) / epsilon;
		jacobian(5,2) = (tempError(5,0) - error(5,0)) / epsilon;
		jacobian(6,2) = (tempError(6,0) - error(6,0)) / epsilon;
		jacobian(7,2) = (tempError(7,0) - error(7,0)) / epsilon;
		jacobian(8,2) = (tempError(8,0) - error(8,0)) / epsilon;
		jacobian(9,2) = (tempError(9,0) - error(9,0)) / epsilon;
		jacobian(10,2) = (tempError(10,0) - error(10,0)) / epsilon;
		jacobian(11,2) = (tempError(11,0) - error(11,0)) / epsilon;
		jacobian(12,2) = (tempError(12,0) - error(12,0)) / epsilon;

		// Compute next guess
		if (!jacobian.LeftDivide(error, delta))
		{
			Debugger::GetInstance() << "Error:  Failed to invert jacobian" << Debugger::PriorityLow;
			Debugger::GetInstance() << "Error:  Failed solve for quasi-static state" << Debugger::PriorityVeryHigh;
			return kinematics.GetInputs();
		}
		guess -= delta;

		i++;
	}

	if (i == limit)
		Debugger::GetInstance() << "Warning:  Iteration limit reached (QuasiStatic::Solve)" << Debugger::PriorityMedium;

	outputs.wheelLoads = wheelLoads;

	outputs.roll = guess(0,0);
	outputs.pitch = guess(1,0);
	outputs.heave = guess(2,0);

	return kinematics.GetInputs();
}

//==========================================================================
// Class:			QuasiStatic
// Function:		ComputeWheelLoads
//
// Description:		Computes the vertical tire loads at each corner.
//
// Input Arguments:
//		originalCar		= const Car*
//		outputs			= const KinematicsOutputs&
//		preLoad			= const WheelSet& [in] spring compression
//
// Output Arguments:
//		None
//
// Return Value:
//		WheelSet [lb]
//
//==========================================================================
WheelSet QuasiStatic::ComputeWheelLoads(const Car* originalCar,
	const KinematicOutputs& outputs, const WheelSet& preLoad) const
{
	WheelSet wheelLoads;// [lb]

	wheelLoads.leftFront = originalCar->suspension->leftFront.spring.rate
		* (preLoad.leftFront + outputs.leftFront[KinematicOutputs::Spring])
		* outputs.leftFront[KinematicOutputs::SpringInstallationRatio]
		+ originalCar->massProperties->unsprungMass.leftFront * 32.174;
	wheelLoads.rightFront = originalCar->suspension->rightFront.spring.rate
		* (preLoad.rightFront + outputs.rightFront[KinematicOutputs::Spring])
		* outputs.rightFront[KinematicOutputs::SpringInstallationRatio]
		+ originalCar->massProperties->unsprungMass.rightFront * 32.174;
	wheelLoads.leftRear = originalCar->suspension->leftRear.spring.rate
		* (preLoad.leftRear + outputs.leftRear[KinematicOutputs::Spring])
		* outputs.leftRear[KinematicOutputs::SpringInstallationRatio]
		+ originalCar->massProperties->unsprungMass.leftRear * 32.174;
	wheelLoads.rightRear = originalCar->suspension->rightRear.spring.rate
		* (preLoad.rightRear + outputs.rightRear[KinematicOutputs::Spring])
		* outputs.rightRear[KinematicOutputs::SpringInstallationRatio]
		+ originalCar->massProperties->unsprungMass.rightRear * 32.174;

	// TODO:  Need to also consider in-plane tire forces here
	//        A portion of the spring forces at each corner (and ARBs?) can come from in-plane forces

	double arbTorque;
	if (originalCar->suspension->frontBarStyle != Suspension::SwayBarNone)
	{
		arbTorque = originalCar->suspension->barRate.front
			* outputs.doubles[KinematicOutputs::FrontARBTwist];// [in-lbf]

		// Our convention is +ve bar twist loads the left side and unloads the right side
		wheelLoads.leftFront += arbTorque * outputs.leftFront[KinematicOutputs::ARBInstallationRatio];
		wheelLoads.rightFront -= arbTorque * outputs.rightFront[KinematicOutputs::ARBInstallationRatio];
	}

	if (originalCar->suspension->rearBarStyle != Suspension::SwayBarNone)
	{
		arbTorque = originalCar->suspension->barRate.rear
			* outputs.doubles[KinematicOutputs::RearARBTwist];// [in-lbf]
		wheelLoads.leftRear += arbTorque * outputs.leftRear[KinematicOutputs::ARBInstallationRatio];
		wheelLoads.rightRear -= arbTorque * outputs.rightRear[KinematicOutputs::ARBInstallationRatio];
	}

	// TODO:  3rd springs

	return wheelLoads;
}

//==========================================================================
// Class:			QuasiStatic
// Function:		ComputeTireDeflections
//
// Description:		Computes the tire deflections at each corner.
//
// Input Arguments:
//		tires		= const TireSet&
//		wheelLoads	= const WheelSet&
//
// Output Arguments:
//		None
//
// Return Value:
//		WheelSet [in]
//
//==========================================================================
WheelSet QuasiStatic::ComputeTireDeflections(const TireSet& tires,
	const WheelSet& wheelLoads) const
{
	WheelSet deflections;
	deflections.leftFront = wheelLoads.leftFront / tires.leftFront->stiffness;
	deflections.rightFront = wheelLoads.leftFront / tires.rightFront->stiffness;
	deflections.leftRear = wheelLoads.leftFront / tires.leftRear->stiffness;
	deflections.rightRear = wheelLoads.leftFront / tires.rightRear->stiffness;

	return deflections;
}

//==========================================================================
// Class:			QuasiStatic
// Function:		ComputePreLoad
//
// Description:		Computes the spring deflection at each corner (for a car with
//					zero kinematic state).
//
// Input Arguments:
//		workingCar	= const Car*
//
// Output Arguments:
//		None
//
// Return Value:
//		WheelSet [in of spring compression]
//
//==========================================================================
WheelSet QuasiStatic::ComputePreLoad(const Car* originalCar) const
{
	const MassProperties* mp = originalCar->massProperties;
	const Suspension *s = originalCar->suspension;

	// First, compute load at each corner due to sprung mass
	WheelSet sprungWeight;// [lbf]
	sprungWeight.leftFront = (mp->cornerWeights.leftFront - mp->unsprungMass.leftFront) * 32.174;
	sprungWeight.rightFront = (mp->cornerWeights.rightFront - mp->unsprungMass.rightFront) * 32.174;
	sprungWeight.leftRear = (mp->cornerWeights.leftRear - mp->unsprungMass.leftRear) * 32.174;
	sprungWeight.rightRear = (mp->cornerWeights.rightRear - mp->unsprungMass.rightRear) * 32.174;

	// In order to accurately know spring pre-loads, we need to know installation ratio
	// at zero kinematic state.  TODO:  Might this be better stored as a property of the car?
	WheelSet zeroDeflections;
	zeroDeflections.leftFront = 0.0;
	zeroDeflections.rightFront = 0.0;
	zeroDeflections.leftRear = 0.0;
	zeroDeflections.rightRear = 0.0;

	Car workingCar(*originalCar);

	Kinematics kinematics;
	kinematics.SetRackTravel(0.0);
	kinematics.SetRoll(0.0);
	kinematics.SetPitch(0.0);
	kinematics.SetHeave(0.0);
	kinematics.SetTireDeflections(zeroDeflections);
	kinematics.SetCenterOfRotation(Vector(0.0, 0.0, 0.0));// This needs to be set to something valid, but isn't actually used
	kinematics.SetFirstEulerRotation(Vector::AxisX);// This needs to be set to something valid, but isn't actually used
	kinematics.UpdateKinematics(originalCar, &workingCar, _T("Pre-Load Calculation"));

	WheelSet preLoad;
	preLoad.leftFront = sprungWeight.leftFront / s->leftFront.spring.rate
		/ kinematics.GetOutputs().leftFront[KinematicOutputs::SpringInstallationRatio];
	preLoad.rightFront = sprungWeight.rightFront / s->rightFront.spring.rate
		/ kinematics.GetOutputs().rightFront[KinematicOutputs::SpringInstallationRatio];
	preLoad.leftRear = sprungWeight.leftRear / s->leftRear.spring.rate
		/ kinematics.GetOutputs().leftRear[KinematicOutputs::SpringInstallationRatio];
	preLoad.rightRear = sprungWeight.rightRear / s->rightRear.spring.rate
		/ kinematics.GetOutputs().rightRear[KinematicOutputs::SpringInstallationRatio];

	// TODO:  3rd springs
	if (s->frontHasThirdSpring || s->rearHasThirdSpring)
		Debugger::GetInstance() << "Warning:  3rd springs are not considered in quasi-static analysis" << Debugger::PriorityVeryHigh;

	return preLoad;
}

//==========================================================================
// Class:			QuasiStatic
// Function:		BuildSystemMatrix
//
// Description:		Builds the matrix used to solve for wheel loads.
//
// Input Arguments:
//		workingCar	= const Car*
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix
//
//==========================================================================
Matrix QuasiStatic::BuildSystemMatrix(const Car* workingCar) const
{
	// This is the "A" matrix in A * x = b
	// x is the vector of vertical tire loads:
	// x = [F_LF; F_RF; F_LR; F_RR]

	const Suspension* s(workingCar->suspension);
	Matrix m(13, 4);

	// Sum of y-moments about left front wheel
	m(0,0) = 0.0;
	m(0,1) = s->rightFront.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x;
	m(0,2) = s->leftRear.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x;
	m(0,3) = s->rightRear.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x;

	// Sum of y-moments about right front wheel
	m(1,0) = s->leftFront.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x;
	m(1,1) = 0.0;
	m(1,2) = s->leftRear.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x;
	m(1,3) = s->rightRear.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x;

	// Sum of y-moments about left rear wheel
	m(2,0) = s->leftFront.hardpoints[Corner::ContactPatch].x - s->leftRear.hardpoints[Corner::ContactPatch].x;
	m(2,1) = s->rightFront.hardpoints[Corner::ContactPatch].x - s->leftRear.hardpoints[Corner::ContactPatch].x;
	m(2,2) = 0.0;
	m(2,3) = s->rightRear.hardpoints[Corner::ContactPatch].x - s->leftRear.hardpoints[Corner::ContactPatch].x;

	// Sum of y-moments about right rear wheel
	m(3,0) = s->leftFront.hardpoints[Corner::ContactPatch].x - s->rightRear.hardpoints[Corner::ContactPatch].x;
	m(3,1) = s->rightFront.hardpoints[Corner::ContactPatch].x - s->rightRear.hardpoints[Corner::ContactPatch].x;
	m(3,2) = s->leftRear.hardpoints[Corner::ContactPatch].x - s->rightRear.hardpoints[Corner::ContactPatch].x;
	m(3,3) = 0.0;

	// Sum of x-moments about left front wheel
	m(4,0) = 0.0;
	m(4,1) = s->leftFront.hardpoints[Corner::ContactPatch].y - s->rightFront.hardpoints[Corner::ContactPatch].y;
	m(4,2) = s->leftFront.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y;
	m(4,3) = s->leftFront.hardpoints[Corner::ContactPatch].y - s->rightRear.hardpoints[Corner::ContactPatch].y;

	// Sum of x-moments about right front wheel
	m(5,0) = s->rightFront.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y;
	m(5,1) = 0.0;
	m(5,2) = s->rightFront.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y;
	m(5,3) = s->rightFront.hardpoints[Corner::ContactPatch].y - s->rightRear.hardpoints[Corner::ContactPatch].y;

	// Sum of x-moments about left rear wheel
	m(6,0) = s->leftRear.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y;
	m(6,1) = s->leftRear.hardpoints[Corner::ContactPatch].y - s->rightFront.hardpoints[Corner::ContactPatch].y;
	m(6,2) = 0.0;
	m(6,3) = s->leftRear.hardpoints[Corner::ContactPatch].y - s->rightRear.hardpoints[Corner::ContactPatch].y;

	// Sum of x-moments about right rear wheel
	m(7,0) = s->rightRear.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y;
	m(7,1) = s->rightRear.hardpoints[Corner::ContactPatch].y - s->rightFront.hardpoints[Corner::ContactPatch].y;
	m(7,2) = s->rightRear.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y;
	m(7,3) = 0.0;

	// Sum of z-forces
	m(8,0) = 1.0;
	m(8,1) = 1.0;
	m(8,2) = 1.0;
	m(8,3) = 1.0;

	// Constitutive constraint LF
	m(9,0) = 1.0;
	m(9,1) = 0.0;
	m(9,2) = 0.0;
	m(9,3) = 0.0;

	// Constitutive constraint RF
	m(10,0) = 0.0;
	m(10,1) = 1.0;
	m(10,2) = 0.0;
	m(10,3) = 0.0;

	// Constitutive constraint LR
	m(11,0) = 0.0;
	m(11,1) = 0.0;
	m(11,2) = 1.0;
	m(11,3) = 0.0;

	// Constitutive constraint RR
	m(12,0) = 0.0;
	m(12,1) = 0.0;
	m(12,2) = 0.0;
	m(12,3) = 1.0;

	// TODO:  Add sum of x and sum of y forces?

	return m;
}

//==========================================================================
// Class:			QuasiStatic
// Function:		BuildRightHandMatrix
//
// Description:		Builds the RH-side matrix used to solve for wheel loads.
//
// Input Arguments:
//		workingCar	= const Car*
//		gx			= const double&
//		gy			= const double&
//		outputs		= const KinematicOutputs&
//		preLoad		= const WheelSet& [inches spring compression]
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix
//
//==========================================================================
Matrix QuasiStatic::BuildRightHandMatrix(const Car* workingCar, const double& gx,
	const double& gy, const KinematicOutputs& outputs, const WheelSet& preLoad) const
{
	// This is the "b" matrix in A * x = b

	const MassProperties* mp(workingCar->massProperties);
	const Suspension* s(workingCar->suspension);
	const double gravity(32.174);// [ft/sec^2]
	Matrix m(13, 1);

	// TODO:  Consider (vertical) motion of sprung mass CG?

	const double sprungMass(mp->GetSprungMass());
	const double massMoment(mp->unsprungMass.leftFront * mp->unsprungCGHeights.leftFront +
		mp->unsprungMass.rightFront * mp->unsprungCGHeights.rightFront +
		mp->unsprungMass.leftRear * mp->unsprungCGHeights.leftRear +
		mp->unsprungMass.rightRear * mp->unsprungCGHeights.rightRear +
		sprungMass * mp->GetSprungMassCG(s).z);
	const double sprungCGx(mp->GetSprungMassCG(s).x);
	const double sprungCGy(mp->GetSprungMassCG(s).y);

	// Sum of y-moments about left front wheel
	m(0,0) = gravity * (-gx * massMoment
		+ mp->unsprungMass.rightFront * (s->rightFront.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x)
		+ mp->unsprungMass.leftRear * (s->leftRear.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x)
		+ mp->unsprungMass.rightRear * (s->rightRear.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x)
		+ sprungMass * (sprungCGx - s->leftFront.hardpoints[Corner::ContactPatch].x));

	// Sum of y-moments about right front wheel
	m(1,0) = gravity * (-gx * massMoment
		+ mp->unsprungMass.leftFront * (s->leftFront.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x)
		+ mp->unsprungMass.leftRear * (s->leftRear.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x)
		+ mp->unsprungMass.rightRear * (s->rightRear.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x)
		+ sprungMass * (sprungCGx - s->rightFront.hardpoints[Corner::ContactPatch].x));

	// Sum of y-moments about left rear wheel
	m(2,0) = gravity * (-gx * massMoment
		+ mp->unsprungMass.leftFront * (s->leftFront.hardpoints[Corner::ContactPatch].x - s->leftRear.hardpoints[Corner::ContactPatch].x)
		+ mp->unsprungMass.rightFront * (s->rightFront.hardpoints[Corner::ContactPatch].x - s->leftRear.hardpoints[Corner::ContactPatch].x)
		+ mp->unsprungMass.rightRear * (s->rightRear.hardpoints[Corner::ContactPatch].x - s->leftRear.hardpoints[Corner::ContactPatch].x)
		+ sprungMass * (sprungCGx - s->leftRear.hardpoints[Corner::ContactPatch].x));

	// Sum of y-moments about right rear wheel
	m(3,0) = gravity * (-gx * massMoment
		+ mp->unsprungMass.leftFront * (s->leftFront.hardpoints[Corner::ContactPatch].x - s->rightRear.hardpoints[Corner::ContactPatch].x)
		+ mp->unsprungMass.rightFront * (s->rightFront.hardpoints[Corner::ContactPatch].x - s->rightRear.hardpoints[Corner::ContactPatch].x)
		+ mp->unsprungMass.leftRear * (s->leftRear.hardpoints[Corner::ContactPatch].x - s->rightRear.hardpoints[Corner::ContactPatch].x)
		+ sprungMass * (sprungCGx - s->rightRear.hardpoints[Corner::ContactPatch].x));

	// Sum of x-moments about left front wheel
	m(4,0) = -gravity * (-gy * massMoment
		+ mp->unsprungMass.rightFront * (s->rightFront.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y)
		+ mp->unsprungMass.leftRear * (s->leftRear.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y)
		+ mp->unsprungMass.rightRear * (s->rightRear.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y)
		+ sprungMass * (sprungCGy - s->leftFront.hardpoints[Corner::ContactPatch].y));

	// Sum of x-moments about right front wheel
	m(5,0) = -gravity * (-gy * massMoment
		+ mp->unsprungMass.leftFront * (s->leftFront.hardpoints[Corner::ContactPatch].y - s->rightFront.hardpoints[Corner::ContactPatch].y)
		+ mp->unsprungMass.leftRear * (s->leftRear.hardpoints[Corner::ContactPatch].y - s->rightFront.hardpoints[Corner::ContactPatch].y)
		+ mp->unsprungMass.rightRear * (s->rightRear.hardpoints[Corner::ContactPatch].y - s->rightFront.hardpoints[Corner::ContactPatch].y)
		+ sprungMass * (sprungCGy - s->rightFront.hardpoints[Corner::ContactPatch].y));

	// Sum of x-moments about left rear wheel
	m(6,0) = -gravity * (-gy * massMoment
		+ mp->unsprungMass.rightFront * (s->rightFront.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y)
		+ mp->unsprungMass.leftFront * (s->leftFront.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y)
		+ mp->unsprungMass.rightRear * (s->rightRear.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y)
		+ sprungMass * (sprungCGy - s->leftRear.hardpoints[Corner::ContactPatch].y));

	// Sum of x-moments about right rear wheel
	m(7,0) = -gravity * (-gy * massMoment
		+ mp->unsprungMass.leftFront * (s->leftFront.hardpoints[Corner::ContactPatch].y - s->rightRear.hardpoints[Corner::ContactPatch].y)
		+ mp->unsprungMass.rightFront * (s->rightFront.hardpoints[Corner::ContactPatch].y - s->rightRear.hardpoints[Corner::ContactPatch].y)
		+ mp->unsprungMass.leftRear * (s->leftRear.hardpoints[Corner::ContactPatch].y - s->rightRear.hardpoints[Corner::ContactPatch].y)
		+ sprungMass * (sprungCGy - s->rightRear.hardpoints[Corner::ContactPatch].y));

	// Sum of z-forces
	m(8,0) = gravity * (mp->cornerWeights.leftFront + mp->cornerWeights.rightFront + mp->cornerWeights.leftRear + mp->cornerWeights.rightRear);

	// Constitutive constraints
	// TODO:  These equations need to consider lateral and longitudinal forces
	m(9,0) = (outputs.leftFront[KinematicOutputs::Spring] + preLoad.leftFront) * s->leftFront.spring.rate
		* outputs.leftFront[KinematicOutputs::SpringInstallationRatio] + mp->unsprungMass.leftFront * gravity;
	m(10,0) = (outputs.rightFront[KinematicOutputs::Spring] + preLoad.rightFront) * s->rightFront.spring.rate
		* outputs.rightFront[KinematicOutputs::SpringInstallationRatio] + mp->unsprungMass.rightFront * gravity;
	m(11,0) = (outputs.leftRear[KinematicOutputs::Spring] + preLoad.leftRear) * s->leftRear.spring.rate
		* outputs.leftRear[KinematicOutputs::SpringInstallationRatio] + mp->unsprungMass.leftRear * gravity;
	m(12,0) = (outputs.rightRear[KinematicOutputs::Spring] + preLoad.rightRear) * s->rightRear.spring.rate
		* outputs.rightRear[KinematicOutputs::SpringInstallationRatio] + mp->unsprungMass.rightRear * gravity;

	// TODO:  Add sum of x and sum of y forces?

	if (s->frontBarStyle != Suspension::SwayBarNone)
	{
		m(9,0) += outputs.doubles[KinematicOutputs::FrontARBTwist]
			* s->barRate.front * outputs.leftFront[KinematicOutputs::ARBInstallationRatio];
		m(10,0) -= outputs.doubles[KinematicOutputs::FrontARBTwist]
			* s->barRate.front * outputs.rightFront[KinematicOutputs::ARBInstallationRatio];
	}

	if (s->rearBarStyle != Suspension::SwayBarNone)
	{
		m(11,0) += outputs.doubles[KinematicOutputs::RearARBTwist]
			* s->barRate.rear * outputs.leftRear[KinematicOutputs::ARBInstallationRatio];
		m(12,0) -= outputs.doubles[KinematicOutputs::RearARBTwist]
			* s->barRate.rear * outputs.rightRear[KinematicOutputs::ARBInstallationRatio];
	}

	// TODO:  Inlcude 3rd springs

	return m;
}

//==========================================================================
// Class:			QuasiStatic
// Function:		ComputeError
//
// Description:		Computes the error for the given kinematic state.
//
// Input Arguments:
//		workingCar	= const Car*
//		gx			= const double&
//		gy			= const double&
//		outputs		= const KinematicOutputs&
//		preLoad		= const WheelSet&
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix
//
//==========================================================================
Matrix QuasiStatic::ComputeError(const Car* workingCar, const double& gx,
	const double& gy, const KinematicOutputs& outputs, const WheelSet& preLoad) const
{
	Matrix A(BuildSystemMatrix(workingCar));
	Matrix b(BuildRightHandMatrix(workingCar, gx, gy, outputs, preLoad));
	Matrix x;

	if (!A.LeftDivide(b, x))
	{
		// TODO:  What here?
		Debugger::GetInstance() << "Failed to solve for tire loads" << Debugger::PriorityHigh;
	}

	return b - A * x;
}

//==========================================================================
// Class:			QuasiStatic
// Function:		ComputeDeltaWheelSets
//
// Description:		Computes the difference between two wheel sets.
//
// Input Arguments:
//		w1	= const WheelSet&
//		w2	= const WheelSet&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double QuasiStatic::ComputeDeltaWheelSets(const WheelSet& w1, const WheelSet& w2)
{
	double delta(0.0);
	delta += fabs(w1.leftFront - w2.leftFront);
	delta += fabs(w1.rightFront - w2.rightFront);
	delta += fabs(w1.leftRear - w2.leftRear);
	delta += fabs(w1.rightRear - w2.rightRear);

	return delta;
}

}// namespace VVASE
