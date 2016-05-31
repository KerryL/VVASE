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
	sprungWeight.leftFront = mp->cornerWeights.leftFront - mp->unsprungMass.leftFront * 32.174;// TODO:  Use unsprung weight instead?
	sprungWeight.rightFront = mp->cornerWeights.rightFront - mp->unsprungMass.rightFront * 32.174;// TODO:  Use unsprung weight instead?
	sprungWeight.leftRear = mp->cornerWeights.leftRear - mp->unsprungMass.leftRear * 32.174;// TODO:  Use unsprung weight instead?
	sprungWeight.rightRear = mp->cornerWeights.rightRear - mp->unsprungMass.rightRear * 32.174;// TODO:  Use unsprung weight instead?
	
	WheelSet preLoad;
	preLoad.leftFront = sprungWeight.leftFront / s->leftFront.spring.rate;
	preLoad.rightFront = sprungWeight.rightFront / s->rightFront.spring.rate;
	preLoad.leftRear = sprungWeight.leftRear / s->leftRear.spring.rate;
	preLoad.rightRear = sprungWeight.rightRear / s->rightRear.spring.rate;
	
	return preLoad;
}
