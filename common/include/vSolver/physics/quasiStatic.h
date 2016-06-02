/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  quasiStatic.h
// Created:  5/27/2016
// Author:  K. Loux
// Description:  Quasi-static analysis object.  Calculates vehicle attitude when subject
//				 to external accelerations.
// History:

#ifndef QUASI_STATIC_H_
#define QUASI_STATIC_H_

// Local headers
#include "vSolver/physics/kinematics.h"
#include "vUtilities/wheelSetStructures.h"

// Local forward declarations
class Car;
class KinematicOutputs;
class Matrix;

class QuasiStatic
{
public:
	QuasiStatic();

	struct Inputs
	{
		double rackTravel;// [in]
		double gx;// [G]
		double gy;// [G]
	};

	Kinematics::Inputs Solve(const Car* originalCar, Car* workingCar,
		const Kinematics::Inputs& kinematicsInputs, const Inputs& inputs) const;

private:
	WheelSet ComputeWheelLoads(const Car* originalCar, const KinematicOutputs& outputs) const;
	WheelSet ComputePreLoad(const Car* workingCar) const;
	WheelSet ComputeTireDeflections(const Car* originalCar, const WheelSet& wheelLoads) const;

	Matrix BuildSystemMatrix(const Car* workingCar) const;
	Matrix BuildRightHandMatrix(const Car* workingCar, const double& gx, const double& gy,
		const KinematicOutputs& outputs, const WheelSet& preLoad) const;
	Matrix ComputeError(const Car* workingCar, const double& gx, const double& gy,
		const KinematicOutputs& outputs, const WheelSet& preLoad) const;
};

#endif// QUASI_STATIC_H_