/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  quasiStatic.cpp
// Created:  5/27/2016
// Author:  K. Loux
// Description:  Quasi-static analysis object.  Calculates vehicle attitude when subject
//				 to external accelerations.
// History:

// Standard C++ headers
#include <cassert>

// Local headers
#include "vSolver/physics/quasiStatic.h"
#include "vSolver/physics/kinematics.h"
#include "vSolver/physics/kinematicOutputs.h"
#include "vCar/car.h"
#include "vCar/massProperties.h"
#include "vCar/tireSet.h"
#include "vCar/tire.h"
#include "vMath/matrix.h"
#include "vMath/vector.h"
#include "vUtilities/debugger.h"

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
//		None
//
// Return Value:
//		Kinematics::Inputs
//
//==========================================================================
Kinematics::Inputs QuasiStatic::Solve(const Car* originalCar, Car* workingCar,
	const Kinematics::Inputs& kinematicsInputs, const Inputs& inputs) const
{
	Kinematics kinematics;
	kinematics.SetCenterOfRotation(kinematicsInputs.centerOfRotation);
	kinematics.SetFirstEulerRotation(kinematicsInputs.firstRotation);
	kinematics.SetRackTravel(inputs.rackTravel);

	unsigned int i(0);
	const unsigned int limit(100);
	const double epsilon(1.0e-3);
	const double maxError(1.0e-8);
	Matrix error(4, 1, maxError, maxError), tempError;
	Matrix jacobian(4,3);
	Matrix guess(3,1);// parameteric variables representing remaining kinematic state inputs
	Matrix delta;

	guess(0,0) = 0.0;
	guess(1,0) = 0.0;
	guess(2,0) = 0.0;

	WheelSet wheelLoads(originalCar->massProperties->cornerWeights);// [lb]
	wheelLoads.leftFront *= 32.174;
	wheelLoads.rightFront *= 32.174;
	wheelLoads.leftRear *= 32.174;
	wheelLoads.rightRear *= 32.174;

	while (i < limit && fabs(error(0, 0)) + fabs(error(1, 0)) + fabs(error(2, 0))  + fabs(error(3, 0)) > maxError)
	{
		kinematics.SetRoll(guess(0,0));
		kinematics.SetPitch(guess(1,0));
		kinematics.SetHeave(guess(2,0));
		kinematics.SetTireDeflections(ComputeTireDeflections(originalCar, wheelLoads));

		kinematics.UpdateKinematics(originalCar, workingCar, wxString::Format("Quasi-Static, i = %u (error)", i));
		wheelLoads = ComputeWheelLoads(originalCar, kinematics.GetOutputs());
		error = ComputeError(workingCar, inputs.gx, inputs.gy, wheelLoads, kinematics.GetOutputs());

		kinematics.SetRoll(guess(0,0) + epsilon);
		kinematics.UpdateKinematics(originalCar, workingCar, wxString::Format("Quasi-Static, i = %u (roll)", i));

		tempError = ComputeError(workingCar, inputs.gx, inputs.gy, ComputeWheelLoads(originalCar, kinematics.GetOutputs()), kinematics.GetOutputs());
		jacobian(0,0) = (tempError(0,0) - error(0,0)) / epsilon;
		jacobian(1,0) = (tempError(1,0) - error(1,0)) / epsilon;
		jacobian(2,0) = (tempError(2,0) - error(2,0)) / epsilon;
		jacobian(3,0) = (tempError(3,0) - error(3,0)) / epsilon;

		kinematics.SetRoll(guess(0,0));
		kinematics.SetPitch(guess(1,0) + epsilon);
		kinematics.UpdateKinematics(originalCar, workingCar, wxString::Format("Quasi-Static, i = %u (pitch)", i));

		tempError = ComputeError(workingCar, inputs.gx, inputs.gy, ComputeWheelLoads(originalCar, kinematics.GetOutputs()), kinematics.GetOutputs());
		jacobian(0,1) = (tempError(0,0) - error(0,0)) / epsilon;
		jacobian(1,1) = (tempError(1,0) - error(1,0)) / epsilon;
		jacobian(2,1) = (tempError(2,0) - error(2,0)) / epsilon;
		jacobian(3,1) = (tempError(3,0) - error(3,0)) / epsilon;

		kinematics.SetPitch(guess(0,0));
		kinematics.SetHeave(guess(1,0) + epsilon);
		kinematics.UpdateKinematics(originalCar, workingCar, wxString::Format("Quasi-Static, i = %u (heave)", i));

		tempError = ComputeError(workingCar, inputs.gx, inputs.gy, ComputeWheelLoads(originalCar, kinematics.GetOutputs()), kinematics.GetOutputs());
		jacobian(0,2) = (tempError(0,0) - error(0,0)) / epsilon;
		jacobian(1,2) = (tempError(1,0) - error(1,0)) / epsilon;
		jacobian(2,2) = (tempError(2,0) - error(2,0)) / epsilon;
		jacobian(3,2) = (tempError(3,0) - error(3,0)) / epsilon;

		// Compute next guess
		if (!jacobian.LeftDivide(error, delta))
		{
			Debugger::GetInstance() << "Error:  Failed to invert jacobian" << Debugger::PriorityLow;
			return kinematics.GetInputs();
		}
		guess -= delta;

		i++;

		// TODO:  Remove
		Debugger::GetInstance() << "Results:\n" + wxString::Format(_T("%f\t%f\n%f\t%f"), wheelLoads.leftFront, wheelLoads.rightFront, wheelLoads.leftRear, wheelLoads.rightRear) << Debugger::PriorityVeryHigh;
		Debugger::GetInstance() << "Error:\n" + wxString::Format(_T("%f"),fabs(error(0, 0)) + fabs(error(1, 0)) + fabs(error(2, 0))  + fabs(error(3, 0))) << Debugger::PriorityVeryHigh;
		Debugger::GetInstance() << "State:\n" + wxString::Format(_T("P = %f, R = %f, H = %f"), guess(0,0), guess(1,0), guess(2,0)) << Debugger::PriorityVeryHigh;
	}

	if (i == limit)
		Debugger::GetInstance() << "Warning:  Iteration limit reached (QuasiStatic::Solve)" << Debugger::PriorityMedium;

	// TODO:  Remove
	/*Debugger::GetInstance() << "Results:\n" + wxString::Format(_T("%f\t%f\n%f\t%f"), wheelLoads.leftFront, wheelLoads.rightFront, wheelLoads.leftRear, wheelLoads.rightRear) << Debugger::PriorityVeryHigh;
	Debugger::GetInstance() << "Error:\n" + wxString::Format(_T("%f"),fabs(error(0, 0)) + fabs(error(1, 0)) + fabs(error(2, 0))  + fabs(error(3, 0))) << Debugger::PriorityVeryHigh;
	Debugger::GetInstance() << "State:\n" + wxString::Format(_T("P = %f, R = %f, H = %f"), guess(0,0), guess(1,0), guess(2,0)) << Debugger::PriorityVeryHigh;*/

	// TODO:  Validate return values

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
//
// Output Arguments:
//		None
//
// Return Value:
//		WheelSet [lb]
//
//==========================================================================
WheelSet QuasiStatic::ComputeWheelLoads(const Car* originalCar,
	const KinematicOutputs& outputs) const
{
	WheelSet preLoad(ComputePreLoad(originalCar));// [in]
	WheelSet wheelLoads;// [lb]

	wheelLoads.leftFront = originalCar->suspension->leftFront.spring.rate
		* (preLoad.leftFront + outputs.leftFront[KinematicOutputs::Spring])
		/ outputs.leftFront[KinematicOutputs::SpringInstallationRatio]
		+ originalCar->massProperties->unsprungMass.leftFront * 32.174;
	wheelLoads.rightFront = originalCar->suspension->rightFront.spring.rate
		* (preLoad.rightFront + outputs.rightFront[KinematicOutputs::Spring])
		/ outputs.rightFront[KinematicOutputs::SpringInstallationRatio]
		+ originalCar->massProperties->unsprungMass.rightFront * 32.174;
	wheelLoads.leftRear = originalCar->suspension->leftRear.spring.rate
		* (preLoad.leftRear + outputs.leftRear[KinematicOutputs::Spring])
		/ outputs.leftRear[KinematicOutputs::SpringInstallationRatio]
		+ originalCar->massProperties->unsprungMass.leftRear * 32.174;
	wheelLoads.rightRear = originalCar->suspension->rightRear.spring.rate
		* (preLoad.rightRear + outputs.rightRear[KinematicOutputs::Spring])
		/ outputs.rightRear[KinematicOutputs::SpringInstallationRatio]
		+ originalCar->massProperties->unsprungMass.rightRear * 32.174;

	// TODO:  3rd springs
	// TODO:  ARBs

	return wheelLoads;
}

//==========================================================================
// Class:			QuasiStatic
// Function:		ComputeTireDeflections
//
// Description:		Computes the tire deflections at each corner.
//
// Input Arguments:
//		originalCar	= const Car*
//		wheelLoads	= const WheelSet&
//
// Output Arguments:
//		None
//
// Return Value:
//		WheelSet [in]
//
//==========================================================================
WheelSet QuasiStatic::ComputeTireDeflections(const Car* originalCar,
	const WheelSet& wheelLoads) const
{
	WheelSet deflections;
	deflections.leftFront = wheelLoads.leftFront / originalCar->tires->leftFront->stiffness;
	deflections.rightFront = wheelLoads.leftFront / originalCar->tires->rightFront->stiffness;
	deflections.leftRear = wheelLoads.leftFront / originalCar->tires->leftRear->stiffness;
	deflections.rightRear = wheelLoads.leftFront / originalCar->tires->rightRear->stiffness;

	return deflections;
}

//==========================================================================
// Class:			QuasiStatic
// Function:		SumXMoments
//
// Description:		Computes the sum of the moments about the X-axis.
//
// Input Arguments:
//		workingCar	= const Car*
//		wheelLoads	= const WheelSet&
//		gy			= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double QuasiStatic::SumXMoments(const Car* workingCar, const WheelSet& wheelLoads, const double& gy) const
{
	const double gravity(UnitConverter::G / 12.0);
	MassProperties* mp = workingCar->massProperties;
	Suspension* s = workingCar->suspension;
	double sum(0.0);

	/*sum += gravity * gy * cgHeight + mp->mass * gravity * mp->centerOfGravity.y;
	sum -= wheelLoads.leftFront * s->leftFront.hardpoints[Corner::ContactPatch].y;
	sum -= wheelLoads.rightFront * s->rightFront.hardpoints[Corner::ContactPatch].y;
	sum -= wheelLoads.leftRear * s->leftRear.hardpoints[Corner::ContactPatch].y;
	sum -= wheelLoads.rightRear * s->rightRear.hardpoints[Corner::ContactPatch].y;*/

	return sum;
}

//==========================================================================
// Class:			QuasiStatic
// Function:		SumYMoments
//
// Description:		Computes the sum of the moments about the Y-axis.
//
// Input Arguments:
//		workingCar	= const Car*
//		wheelLoads	= const WheelSet&
//		gx			= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double QuasiStatic::SumYMoments(const Car* workingCar, const WheelSet& wheelLoads, const double& gx) const
{
	const double gravity(UnitConverter::G / 12.0);
	MassProperties* mp = workingCar->massProperties;
	Suspension* s = workingCar->suspension;
	double sum(0.0);

	/*sum += gravity * gx * mp->centerOfGravity.z + mp->mass * gravity * mp->centerOfGravity.x;
	sum -= wheelLoads.leftFront * s->leftFront.hardpoints[Corner::ContactPatch].x;
	sum -= wheelLoads.rightFront * s->rightFront.hardpoints[Corner::ContactPatch].x;
	sum -= wheelLoads.leftRear * s->leftRear.hardpoints[Corner::ContactPatch].x;
	sum -= wheelLoads.rightRear * s->rightRear.hardpoints[Corner::ContactPatch].x;*/

	return sum;
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
	
	WheelSet preLoad;
	preLoad.leftFront = sprungWeight.leftFront / s->leftFront.spring.rate;
	preLoad.rightFront = sprungWeight.rightFront / s->rightFront.spring.rate;
	preLoad.leftRear = sprungWeight.leftRear / s->leftRear.spring.rate;
	preLoad.rightRear = sprungWeight.rightRear / s->rightRear.spring.rate;
	
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
//		gx			= const double&
//		gy			= const double&
//		outputs		= const KinematicOutputs&
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix
//
//==========================================================================
Matrix QuasiStatic::BuildSystemMatrix(const Car* workingCar,
	const double& gx, const double& gy, const KinematicOutputs& outputs) const
{
	// This is the "A" matrix in A * x = b
	// x is the vector of vertical tire loads:
	// x = [F_LF; F_RF; F_LR; F_RR]

	const MassProperties* mp(workingCar->massProperties);
	const Suspension* s(workingCar->suspension);
	const TireSet* t(workingCar->tires);
	Matrix m(9, 4);

	// Sum of y-moments about left front wheel
	m(0,0) = 0.0;
	m(0,1) = s->leftFront.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x;
	m(0,2) = s->leftFront.hardpoints[Corner::ContactPatch].x - s->leftRear.hardpoints[Corner::ContactPatch].x;
	m(0,3) = s->leftFront.hardpoints[Corner::ContactPatch].x - s->rightRear.hardpoints[Corner::ContactPatch].x;

	// Sum of y-moments about right front wheel
	m(1,0) = s->rightFront.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x;
	m(1,1) = 0.0;
	m(1,2) = s->rightFront.hardpoints[Corner::ContactPatch].x - s->leftRear.hardpoints[Corner::ContactPatch].x;
	m(1,3) = s->rightFront.hardpoints[Corner::ContactPatch].x - s->rightRear.hardpoints[Corner::ContactPatch].x;

	// Sum of x-moments about left front wheel
	m(2,0) = 0.0;
	m(2,1) = s->rightFront.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y;
	m(2,2) = s->leftRear.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y;
	m(2,3) = s->rightRear.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y;

	// Sum of x-moments about left rear wheel
	m(3,0) = s->leftFront.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y;
	m(3,1) = s->rightFront.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y;
	m(3,2) = 0.0;
	m(3,3) = s->rightRear.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y;

	// Sum of z-forces
	m(4,0) = 1.0;
	m(4,1) = 1.0;
	m(4,2) = 1.0;
	m(4,3) = 1.0;

	// Co-planar constraint LF
	m(5,0) = 1.0 / t->leftFront->stiffness
		+ 1.0 / (s->leftFront.spring.rate * outputs.leftFront[KinematicOutputs::SpringInstallationRatio]
			* outputs.leftFront[KinematicOutputs::SpringInstallationRatio]);
	m(5,1) = 0.0;
	m(5,2) = 0.0;
	m(5,3) = 0.0;

	// Co-planar constraint RF
	m(6,0) = 0.0;
	m(6,1) = 1.0 / t->rightFront->stiffness
		+ 1.0 / (s->rightFront.spring.rate * outputs.rightFront[KinematicOutputs::SpringInstallationRatio]
			* outputs.rightFront[KinematicOutputs::SpringInstallationRatio]);
	m(6,2) = 0.0;
	m(6,3) = 0.0;

	// Co-planar constraint LR
	m(7,0) = 0.0;
	m(7,1) = 0.0;
	m(7,2) = 1.0 / t->leftRear->stiffness
		+ 1.0 / (s->leftRear.spring.rate * outputs.leftRear[KinematicOutputs::SpringInstallationRatio]
			* outputs.leftRear[KinematicOutputs::SpringInstallationRatio]);
	m(7,3) = 0.0;

	// Co-planar constraint RR
	m(8,0) = 0.0;
	m(8,1) = 0.0;
	m(8,2) = 0.0;
	m(8,3) = 1.0 / t->rightRear->stiffness
		+ 1.0 / (s->rightRear.spring.rate * outputs.rightRear[KinematicOutputs::SpringInstallationRatio]
			* outputs.rightRear[KinematicOutputs::SpringInstallationRatio]);

	// TODO:  ARBs + 3rd springs

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
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix
//
//==========================================================================
Matrix QuasiStatic::BuildRightHandMatrix(const Car* workingCar, const double& gx, const double& gy) const
{
	// This is the "b" matrix in A * x = b

	const MassProperties* mp(workingCar->massProperties);
	const Suspension* s(workingCar->suspension);
	const TireSet* t(workingCar->tires);
	const double gravity(32.174);// [ft/sec^2]
	Matrix m(9, 1);

	const double sprungMass(mp->GetSprungMass());
	const double massMoment(mp->unsprungMass.leftFront * mp->unsprungCGHeights.leftFront +
		mp->unsprungMass.rightFront * mp->unsprungCGHeights.rightFront +
		mp->unsprungMass.leftRear * mp->unsprungCGHeights.leftRear +
		mp->unsprungMass.rightRear * mp->unsprungCGHeights.rightRear +
		sprungMass * mp->GetSprungMassCG(s).z);
	const double sprungCGx(mp->GetSprungMassCG(s).x);
	const double sprungCGy(mp->GetSprungMassCG(s).y);

	// Sum of y-moments about left front wheel
	m(0,0) = -gravity * (gx * massMoment
		+ mp->unsprungMass.rightFront * (s->rightFront.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x)
		+ mp->unsprungMass.leftRear * (s->leftRear.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x)
		+ mp->unsprungMass.rightRear * (s->rightRear.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x)
		+ sprungMass * (sprungCGx - s->leftFront.hardpoints[Corner::ContactPatch].x));

	// Sum of y-moments about right front wheel
	m(1,0) = -gravity * (gx * massMoment
		+ mp->unsprungMass.leftFront * (s->leftFront.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x)
		+ mp->unsprungMass.leftRear * (s->leftRear.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x)
		+ mp->unsprungMass.rightRear * (s->rightRear.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x)
		+ sprungMass * (sprungCGx - s->rightFront.hardpoints[Corner::ContactPatch].x));

	// Sum of x-moments about left front wheel
	m(2,0) = gravity * (gy * massMoment
		+ mp->unsprungMass.rightFront * (s->rightFront.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y)
		+ mp->unsprungMass.leftRear * (s->leftRear.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y)
		+ mp->unsprungMass.rightRear * (s->rightRear.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y)
		+ sprungMass * (sprungCGy - s->leftFront.hardpoints[Corner::ContactPatch].y));

	// Sum of x-moments about left rear wheel
	m(3,0) = gravity * (gy * massMoment
		+ mp->unsprungMass.rightFront * (s->rightFront.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y)
		+ mp->unsprungMass.leftFront * (s->leftFront.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y)
		+ mp->unsprungMass.rightRear * (s->rightRear.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y)
		+ sprungMass * (sprungCGy - s->leftRear.hardpoints[Corner::ContactPatch].y));

	// Sum of z-forces
	m(4,0) = gravity * (mp->cornerWeights.leftFront + mp->cornerWeights.rightFront + mp->cornerWeights.leftRear + mp->cornerWeights.rightRear);

	// Co-planar constraints
	m(5,0) = -mp->unsprungMass.leftFront * gravity / t->leftFront->stiffness;
	m(6,0) = -mp->unsprungMass.rightFront * gravity / t->rightFront->stiffness;
	m(7,0) = -mp->unsprungMass.leftRear * gravity / t->leftRear->stiffness;
	m(8,0) = -mp->unsprungMass.rightRear * gravity / t->rightRear->stiffness;

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
//		wheelLoads	= const WheelSet&
//		outputs		= const KinematicOutputs&
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix
//
//==========================================================================
Matrix QuasiStatic::ComputeError(const Car* workingCar, const double& gx,
	const double& gy, const WheelSet& wheelLoads, const KinematicOutputs& outputs) const
{
	Matrix A(BuildSystemMatrix(workingCar, gx, gy, outputs));
	Matrix b(BuildRightHandMatrix(workingCar, gx, gy));
	Matrix x;

	if (!A.LeftDivide(b, x))
	{
		// TODO:  What here?
	}

	/*Debugger::GetInstance() << _T("\nA = \n") + A.Print() << Debugger::PriorityVeryHigh;
	Debugger::GetInstance() << _T("\nb = \n") + b.Print() << Debugger::PriorityVeryHigh;
	Debugger::GetInstance() << _T("\nx = \n") + x.Print() << Debugger::PriorityVeryHigh;
	Debugger::GetInstance() << _T("\nA * x = \n") + (A * x).Print() << Debugger::PriorityVeryHigh;

	x(0,0) = workingCar->massProperties->cornerWeights.leftFront * 32.174;
	x(1,0) = workingCar->massProperties->cornerWeights.rightFront * 32.174;
	x(2,0) = workingCar->massProperties->cornerWeights.leftRear * 32.174;
	x(3,0) = workingCar->massProperties->cornerWeights.rightRear * 32.174;
	Debugger::GetInstance() << _T("\nA * x* = \n") + (A * x).Print() << Debugger::PriorityVeryHigh;*/

	x(0,0) -= wheelLoads.leftFront;
	x(1,0) -= wheelLoads.rightFront;
	x(2,0) -= wheelLoads.leftRear;
	x(3,0) -= wheelLoads.rightRear;

	return x;
}
