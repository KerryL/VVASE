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
	Matrix error(2, 1, maxError, maxError);
	Matrix jacobian(2,3);
	Matrix guess(3,1);// parameteric variables representing remaining kinematic state inputs
	Matrix delta;

	guess(0,0) = 0.0;
	guess(1,0) = 0.0;
	guess(2,0) = 0.0;

	WheelSet wheelLoads;

	while (i < limit && fabs(error(0, 0)) + fabs(error(1, 0)) > maxError)
	{
		kinematics.SetRoll(guess(0,0));
		kinematics.SetPitch(guess(1,0));
		kinematics.SetHeave(guess(2,0));

		kinematics.UpdateKinematics(originalCar, workingCar, wxString::Format("Quasi-Static, i = %u (error)", i));
		wheelLoads = ComputeWheelLoads(originalCar, kinematics.GetOutputs());

		error(0,0) = SumXMoments(workingCar, wheelLoads, inputs.gy);
		error(1,0) = SumYMoments(workingCar, wheelLoads, inputs.gx);

		kinematics.SetRoll(guess(0,0) + epsilon);
		kinematics.UpdateKinematics(originalCar, workingCar, wxString::Format("Quasi-Static, i = %u (roll)", i));
		wheelLoads = ComputeWheelLoads(originalCar, kinematics.GetOutputs());

		jacobian(0,0) = (SumXMoments(workingCar, wheelLoads, inputs.gy) - error(0,0)) / epsilon;
		jacobian(1,0) = (SumYMoments(workingCar, wheelLoads, inputs.gx) - error(1,0)) / epsilon;

		kinematics.SetRoll(guess(0,0));
		kinematics.SetPitch(guess(1,0) + epsilon);
		kinematics.UpdateKinematics(originalCar, workingCar, wxString::Format("Quasi-Static, i = %u (pitch)", i));
		wheelLoads = ComputeWheelLoads(originalCar, kinematics.GetOutputs());

		jacobian(0,1) = (SumXMoments(workingCar, wheelLoads, inputs.gy) - error(0,0)) / epsilon;
		jacobian(1,1) = (SumYMoments(workingCar, wheelLoads, inputs.gx) - error(1,0)) / epsilon;

		kinematics.SetPitch(guess(0,0));
		kinematics.SetHeave(guess(1,0) + epsilon);
		kinematics.UpdateKinematics(originalCar, workingCar, wxString::Format("Quasi-Static, i = %u (heave)", i));
		wheelLoads = ComputeWheelLoads(originalCar, kinematics.GetOutputs());

		jacobian(0,2) = (SumXMoments(workingCar, wheelLoads, inputs.gy) - error(0,0)) / epsilon;
		jacobian(1,2) = (SumYMoments(workingCar, wheelLoads, inputs.gx) - error(1,0)) / epsilon;

		// Compute next guess
		if (!jacobian.LeftDivide(error, delta))
		{
			Debugger::GetInstance() << "Error:  Failed to invert jacobian" << Debugger::PriorityLow;
			return kinematics.GetInputs();
		}
		guess -= delta;

		i++;
	}

	if (i == limit)
		Debugger::GetInstance() << "Warning:  Iteration limit reached (QuasiStatic::Solve)" << Debugger::PriorityMedium;

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
//		WheelSet
//
//==========================================================================
WheelSet QuasiStatic::ComputeWheelLoads(const Car* originalCar,
	const KinematicOutputs& outputs) const
{
	WheelSet preLoad(ComputePreLoad(originalCar));// [lb]
	WheelSet wheelLoads;

	wheelLoads.leftFront = preLoad.leftFront + originalCar->suspension->leftFront.spring.rate
		* outputs.leftFront[KinematicOutputs::Spring] * outputs.leftFront[KinematicOutputs::SpringInstallationRatio]
		* outputs.leftFront[KinematicOutputs::SpringInstallationRatio];
	wheelLoads.rightFront = preLoad.rightFront + originalCar->suspension->rightFront.spring.rate
		* outputs.rightFront[KinematicOutputs::Spring] * outputs.rightFront[KinematicOutputs::SpringInstallationRatio]
		* outputs.rightFront[KinematicOutputs::SpringInstallationRatio];
	wheelLoads.leftRear = preLoad.leftRear + originalCar->suspension->leftRear.spring.rate
		* outputs.leftRear[KinematicOutputs::Spring] * outputs.leftRear[KinematicOutputs::SpringInstallationRatio]
		* outputs.leftRear[KinematicOutputs::SpringInstallationRatio];
	wheelLoads.rightRear = preLoad.rightRear + originalCar->suspension->rightRear.spring.rate
		* outputs.rightRear[KinematicOutputs::Spring] * outputs.rightRear[KinematicOutputs::SpringInstallationRatio]
		* outputs.rightRear[KinematicOutputs::SpringInstallationRatio];

	// TODO:  3rd springs
	// TODO:  ARBs

	return wheelLoads;
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

	sum += gravity * gy * mp->centerOfGravity.z + mp->mass * gravity * mp->centerOfGravity.y;
	sum -= wheelLoads.leftFront * s->leftFront.hardpoints[Corner::ContactPatch].y;
	sum -= wheelLoads.rightFront * s->rightFront.hardpoints[Corner::ContactPatch].y;
	sum -= wheelLoads.leftRear * s->leftRear.hardpoints[Corner::ContactPatch].y;
	sum -= wheelLoads.rightRear * s->rightRear.hardpoints[Corner::ContactPatch].y;

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

	sum += gravity * gx * mp->centerOfGravity.z + mp->mass * gravity * mp->centerOfGravity.x;
	sum -= wheelLoads.leftFront * s->leftFront.hardpoints[Corner::ContactPatch].x;
	sum -= wheelLoads.rightFront * s->rightFront.hardpoints[Corner::ContactPatch].x;
	sum -= wheelLoads.leftRear * s->leftRear.hardpoints[Corner::ContactPatch].x;
	sum -= wheelLoads.rightRear * s->rightRear.hardpoints[Corner::ContactPatch].x;

	return sum;
}

//==========================================================================
// Class:			QuasiStatic
// Function:		ComputePreLoad
//
// Description:		Computes the spring pre-load at each corner (for a car with
//					zero kinematic state).
//
// Input Arguments:
//		workingCar	= const Car*
//
// Output Arguments:
//		None
//
// Return Value:
//		WheelSet [lb at the tire]
//
//==========================================================================
WheelSet QuasiStatic::ComputePreLoad(const Car* originalCar) const
{
	const MassProperties* mp = originalCar->massProperties;
	const Suspension *s = originalCar->suspension;

	// First, compute load at each corner due to sprung mass
	WheelSet preLoad;
	double sprungMass = mp->mass - mp->unsprungMass.leftFront
		- mp->unsprungMass.rightFront - mp->unsprungMass.leftRear - mp->unsprungMass.rightRear;// [slug]

	// Build a matrix equation of the form:
	// sum of moments about wheel O = 0 = m * g * x_cg - F_a * (x_a - x_O) - F_b * (x_b - x_O) - F_c * (x_c - x_O)
	// One equation for each wheel in x and y directions (2 equations per wheel)
	Matrix A(8, 4), b(8, 1), x;
	A(0,0) = 0.0;
	A(0,1) = s->rightFront.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y;
	A(0,2) = s->leftRear.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y;
	A(0,3) = s->rightRear.hardpoints[Corner::ContactPatch].y - s->leftFront.hardpoints[Corner::ContactPatch].y;
	b(0,0) = sprungMass * UnitConverter::G / 12.0 * (mp->centerOfGravity.y
		- s->leftFront.hardpoints[Corner::ContactPatch].y);// TODO:  Use unit convert to address this?

	A(1,0) = 0.0;
	A(1,1) = s->rightFront.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x;
	A(1,2) = s->leftRear.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x;
	A(1,3) = s->rightRear.hardpoints[Corner::ContactPatch].x - s->leftFront.hardpoints[Corner::ContactPatch].x;
	b(1,0) = sprungMass * UnitConverter::G / 12.0 * (mp->centerOfGravity.x
		- s->leftFront.hardpoints[Corner::ContactPatch].x);// TODO:  Use unit convert to address this?

	A(2,0) = s->leftFront.hardpoints[Corner::ContactPatch].y - s->rightFront.hardpoints[Corner::ContactPatch].y;
	A(2,1) = 0.0;
	A(2,2) = s->leftRear.hardpoints[Corner::ContactPatch].y - s->rightFront.hardpoints[Corner::ContactPatch].y;
	A(2,3) = s->rightRear.hardpoints[Corner::ContactPatch].y - s->rightFront.hardpoints[Corner::ContactPatch].y;
	b(2,0) = sprungMass * UnitConverter::G / 12.0 * (mp->centerOfGravity.y
		- s->rightFront.hardpoints[Corner::ContactPatch].y);// TODO:  Use unit convert to address this?

	A(3,0) = s->leftFront.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x;
	A(3,1) = 0.0;
	A(3,2) = s->leftRear.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x;
	A(3,3) = s->rightRear.hardpoints[Corner::ContactPatch].x - s->rightFront.hardpoints[Corner::ContactPatch].x;
	b(3,0) = sprungMass * UnitConverter::G / 12.0 * (mp->centerOfGravity.x
		- s->rightFront.hardpoints[Corner::ContactPatch].x);// TODO:  Use unit convert to address this?

	A(4,0) = s->leftFront.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y;
	A(4,1) = s->rightFront.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y;
	A(4,2) = 0.0;
	A(4,3) = s->rightRear.hardpoints[Corner::ContactPatch].y - s->leftRear.hardpoints[Corner::ContactPatch].y;
	b(4,0) = sprungMass * UnitConverter::G / 12.0 * (mp->centerOfGravity.y
		- s->leftRear.hardpoints[Corner::ContactPatch].y);// TODO:  Use unit convert to address this?

	A(5,0) = s->leftFront.hardpoints[Corner::ContactPatch].x - s->leftRear.hardpoints[Corner::ContactPatch].x;
	A(5,1) = s->rightFront.hardpoints[Corner::ContactPatch].x - s->leftRear.hardpoints[Corner::ContactPatch].x;
	A(5,2) = 0.0;
	A(5,3) = s->rightRear.hardpoints[Corner::ContactPatch].x - s->leftRear.hardpoints[Corner::ContactPatch].x;
	b(5,0) = sprungMass * UnitConverter::G / 12.0 * (mp->centerOfGravity.x
		- s->leftRear.hardpoints[Corner::ContactPatch].x);// TODO:  Use unit convert to address this?

	A(6,0) = s->leftFront.hardpoints[Corner::ContactPatch].y - s->rightRear.hardpoints[Corner::ContactPatch].y;
	A(6,1) = s->rightFront.hardpoints[Corner::ContactPatch].y - s->rightRear.hardpoints[Corner::ContactPatch].y;
	A(6,2) = s->leftRear.hardpoints[Corner::ContactPatch].y - s->rightRear.hardpoints[Corner::ContactPatch].y;
	A(6,3) = 0.0;
	b(6,0) = sprungMass * UnitConverter::G / 12.0 * (mp->centerOfGravity.y
		- s->rightRear.hardpoints[Corner::ContactPatch].y);// TODO:  Use unit convert to address this?

	A(7,0) = s->leftFront.hardpoints[Corner::ContactPatch].x - s->rightRear.hardpoints[Corner::ContactPatch].x;
	A(7,1) = s->rightFront.hardpoints[Corner::ContactPatch].x - s->rightRear.hardpoints[Corner::ContactPatch].x;
	A(7,2) = s->leftRear.hardpoints[Corner::ContactPatch].x - s->rightRear.hardpoints[Corner::ContactPatch].x;
	A(7,3) = 0.0;
	b(7,0) = sprungMass * UnitConverter::G / 12.0 * (mp->centerOfGravity.x
		- s->rightRear.hardpoints[Corner::ContactPatch].x);// TODO:  Use unit convert to address this?

	// TODO:  Test rank of A to identify statically indeterminate problems?
	//unsigned int rank(A.GetRank());
	if (!A.LeftDivide(b, x))
	{
		Debugger::GetInstance() << "Error:  Failed to solve for pre-load" << Debugger::PriorityMedium;
		preLoad.leftFront = 0.0;
		preLoad.rightFront = 0.0;
		preLoad.leftRear = 0.0;
		preLoad.rightRear = 0.0;
		return preLoad;
	}

	Matrix test = A * x - b;
	double t[8];
	int h;
	for (h = 0; h < 8; h++)
		t[h] = b(h,0);// TODO:  This should be zero, but it's not!

	assert(x.GetNumberOfRows() == 4);

	preLoad.leftFront = x(0,0);
	preLoad.rightFront = x(1,0);
	preLoad.leftRear = x(2,0);
	preLoad.rightRear = x(3,0);

	return preLoad;
}
