/*===================================================================================
                                       VVASE
                         Copyright Kerry R. Loux 2007-2017
===================================================================================*/

// File:  quasiStatic.h
// Date:  5/27/2016
// Auth:  K. Loux
// Desc:  Quasi-static analysis object.  Calculates vehicle attitude when subject
//        to external accelerations.

#ifndef QUASI_STATIC_H_
#define QUASI_STATIC_H_

// Local headers
#include "vSolver/physics/kinematics.h"
#include "vUtilities/wheelSetStructures.h"

namespace VVASE
{

// Local forward declarations
class Car;
class KinematicOutputs;
class QuasiStaticOutputs;
class Matrix;
class TireSet;

class QuasiStatic : public Analysis
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
		const Kinematics::Inputs& kinematicsInputs, const Inputs& inputs,
		QuasiStaticOutputs& outputs) const;

private:
	WheelSet ComputeWheelLoads(const Car* originalCar, const KinematicOutputs& outputs,
		const WheelSet& preLoad) const;
	WheelSet ComputePreLoad(const Car* workingCar) const;
	WheelSet ComputeTireDeflections(const TireSet& tires, const WheelSet& wheelLoads) const;

	Matrix BuildSystemMatrix(const Car* workingCar) const;
	Matrix BuildRightHandMatrix(const Car* workingCar, const double& gx, const double& gy,
		const KinematicOutputs& outputs, const WheelSet& preLoad) const;
	Matrix ComputeError(const Car* workingCar, const double& gx, const double& gy,
		const KinematicOutputs& outputs, const WheelSet& preLoad) const;
	static double ComputeDeltaWheelSets(const WheelSet& w1, const WheelSet& w2);
};

}// namespace VVASE

#endif// QUASI_STATIC_H_
